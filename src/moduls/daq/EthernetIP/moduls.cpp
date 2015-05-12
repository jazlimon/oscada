
//OpenSCADA system modules EthernetIP file: moduls.cpp
/***************************************************************************
 *   Copyright (C) 2015 by Anibal Limon, <limon.anibal@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ethernet_ip_daq.h"

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt daq_EthernetIP_module( int nMod )
#else
    TModule::SAt module( int nMod )
#endif
    {
	if(nMod == 0)	return TModule::SAt(DAQ_ID,DAQ_TYPE,DAQ_SUBVER);

	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *daq_EthernetIP_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(DAQ_ID,DAQ_TYPE,DAQ_SUBVER)) return new EthernetIP::TTpContr( source );

	return NULL;
    }
}
