/*
 *  iso_server.c
 *
 *  Copyright 2013 Michael Zillgith
 *
 *  This file is part of libIEC61850.
 *
 *  libIEC61850 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libIEC61850 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libIEC61850.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See COPYING file for the complete license text.
 */

#include "libiec61850_platform_includes.h"

#include "stack_config.h"

#ifndef DEBUG_ISO_SERVER
#ifdef DEBUG
#define DEBUG_ISO_SERVER 1
#else
#define DEBUG_ISO_SERVER 0
#endif /*DEBUG */
#endif /* DEBUG_ISO_SERVER */

#include "mms_server_connection.h"

#include "thread.h"

#include "iso_server.h"

#include "iso_server_private.h"

#define TCP_PORT 102
#define SECURE_TCP_PORT 3782
#define BACKLOG 10

struct sIsoServer {
    IsoServerState state;
    ConnectionIndicationHandler connectionHandler;
    void* connectionHandlerParameter;
    AcseAuthenticationParameter authParameter;
    Thread serverThread;
    Socket serverSocket;
    int tcpPort;
    char* localIpAddress;

    LinkedList openClientConnections;
    Semaphore connectionCounterMutex;
    int connectionCounter;
};

void
private_IsoServer_increaseConnectionCounter(IsoServer self);

void
private_IsoServer_decreaseConnectionCounter(IsoServer self);


static void
isoServerThread(void* isoServerParam)
{
    IsoServer self = (IsoServer) isoServerParam;

    if (DEBUG_ISO_SERVER)
        printf("ISO_SERVER: isoServerThread %p started\n", &isoServerParam);

    Socket connectionSocket;

    self->serverSocket = (Socket) TcpServerSocket_create(self->localIpAddress, self->tcpPort);

    if (self->serverSocket == NULL) {
        self->state = ISO_SVR_STATE_ERROR;
        goto cleanUp;
    }

    ServerSocket_setBacklog((ServerSocket) self->serverSocket, BACKLOG);

    ServerSocket_listen((ServerSocket) self->serverSocket);

    self->state = ISO_SVR_STATE_RUNNING;

    while (self->state == ISO_SVR_STATE_RUNNING)
    {

        if ((connectionSocket = ServerSocket_accept((ServerSocket) self->serverSocket)) == NULL) {
            break;;
        }
        else {
            IsoConnection isoConnection = IsoConnection_create(connectionSocket, self);

            private_IsoServer_increaseConnectionCounter(self);

            LinkedList_add(self->openClientConnections, isoConnection);

            self->connectionHandler(ISO_CONNECTION_OPENED, self->connectionHandlerParameter,
                    isoConnection);

        }

    }

    self->state = ISO_SVR_STATE_STOPPED;

    cleanUp:
    self->serverSocket = NULL;

    if (DEBUG_ISO_SERVER)
           printf("ISO_SERVER: isoServerThread %p stopped\n", &isoServerParam);

}

IsoServer
IsoServer_create()
{
    IsoServer self = (IsoServer) calloc(1, sizeof(struct sIsoServer));

    self->state = ISO_SVR_STATE_IDLE;
    self->tcpPort = TCP_PORT;

    self->openClientConnections = LinkedList_create();

    self->connectionCounterMutex = Semaphore_create(1);
    self->connectionCounter = 0;

    return self;
}

void
IsoServer_setTcpPort(IsoServer self, int port)
{
    self->tcpPort = port;
}

void
IsoServer_setLocalIpAddress(IsoServer self, char* ipAddress)
{
	self->localIpAddress = ipAddress;
}

IsoServerState
IsoServer_getState(IsoServer self)
{
    return self->state;
}

void
IsoServer_setAuthenticationParameter(IsoServer self, AcseAuthenticationParameter authParameter)
{
    self->authParameter = authParameter;
}

AcseAuthenticationParameter
IsoServer_getAuthenticationParameter(IsoServer self)
{
    return self->authParameter;
}

void
IsoServer_startListening(IsoServer self)
{
    self->serverThread = Thread_create((ThreadExecutionFunction) isoServerThread, self, false);

    Thread_start(self->serverThread);

    while (self->state == ISO_SVR_STATE_IDLE)
        Thread_sleep(1);

    if (DEBUG_ISO_SERVER)
        printf("ISO_SERVER: new iso server thread started\n");
}

void
IsoServer_stopListening(IsoServer self)
{
    self->state = ISO_SVR_STATE_STOPPED;

    if (self->serverSocket != NULL) {
        ServerSocket_destroy((ServerSocket) self->serverSocket);
        self->serverSocket = NULL;
    }

    if (self->serverThread != NULL)
        Thread_destroy(self->serverThread);

    /* Close all open client connections */
    LinkedList openConnection = LinkedList_getNext(self->openClientConnections);

    while (openConnection != NULL) {
        IsoConnection isoConnection = (IsoConnection) openConnection->data;

        IsoConnection_close(isoConnection);

        openConnection = LinkedList_getNext(openConnection);
    }

    /* Wait for connection threads to finish */
    while (private_IsoServer_getConnectionCounter(self) > 0)
        Thread_sleep(10);

    if (DEBUG_ISO_SERVER)
        printf("ISO_SERVER: IsoServer_stopListening finished!\n");
}

void
IsoServer_closeConnection(IsoServer self, IsoConnection isoConnection)
{
    if (self->state != ISO_SVR_STATE_IDLE) {
        self->connectionHandler(ISO_CONNECTION_CLOSED, self->connectionHandlerParameter,
                isoConnection);
    }

    LinkedList_remove(self->openClientConnections, isoConnection);
}

void
IsoServer_setConnectionHandler(IsoServer self, ConnectionIndicationHandler handler,
        void* parameter)
{
    self->connectionHandler = handler;
    self->connectionHandlerParameter = parameter;
}

void
IsoServer_destroy(IsoServer self)
{
    if (self->state == ISO_SVR_STATE_RUNNING)
        IsoServer_stopListening(self);

    LinkedList_destroy(self->openClientConnections);

    Semaphore_destroy(self->connectionCounterMutex);

    free(self);
}

void
private_IsoServer_increaseConnectionCounter(IsoServer self)
{
    Semaphore_wait(self->connectionCounterMutex);
    self->connectionCounter++;
    if (DEBUG_ISO_SERVER)
        printf("IsoServer: increase connection counter to %i!\n", self->connectionCounter);
    Semaphore_post(self->connectionCounterMutex);
}

void
private_IsoServer_decreaseConnectionCounter(IsoServer self)
{

    Semaphore_wait(self->connectionCounterMutex);
    self->connectionCounter--;

    if (DEBUG_ISO_SERVER)
        printf("IsoServer: decrease connection counter to %i!\n", self->connectionCounter);
    Semaphore_post(self->connectionCounterMutex);
}

int
private_IsoServer_getConnectionCounter(IsoServer self)
{
    int connectionCounter;

    Semaphore_wait(self->connectionCounterMutex);
    connectionCounter = self->connectionCounter;
    Semaphore_post(self->connectionCounterMutex);

    return connectionCounter;
}
