EXTRA_DIST = *.h po/*

if EthernetIPIncl
noinst_LTLIBRARIES = daq_EthernetIP.la
daq_EthernetIP_la_CXXFLAGS = -I tuxeip/src -DMOD_INCL -fpic
daq_EthernetIP_la_LIBTOOLFLAGS = --tag=disable-shared
daq_EthernetIP_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = daq_EthernetIP.la
daq_EthernetIP_la_CXXFLAGS = -I tuxeip/src
daq_EthernetIP_la_LIBTOOLFLAGS = --tag=disable-static
daq_EthernetIP_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

tuxeip_SOURCES = tuxeip/src/AB.c tuxeip/src/CM.c tuxeip/src/Ethernet_IP.c tuxeip/src/MR.c tuxeip/src/SendData.c tuxeip/src/CIP_IOI.c tuxeip/src/ErrCodes.c tuxeip/src/LGX.c tuxeip/src/PLC.c
daq_EthernetIP_la_SOURCES = moduls.cpp ethernet_ip_daq.cpp $(tuxeip_SOURCES)

I18N_mod = $(oscd_modulpref)EthernetIP
include ../../../../I18N.mk
