EXTRA_DIST = *.h po/*

if EthernetIPIncl
noinst_LTLIBRARIES = daq_EthernetIP.la
daq_EthernetIP_la_CXXFLAGS = -DMOD_INCL -fpic
daq_EthernetIP_la_LIBTOOLFLAGS = --tag=disable-shared
daq_EthernetIP_la_LDFLAGS = -module
else
oscd_modul_LTLIBRARIES = daq_EthernetIP.la
daq_EthernetIP_la_LIBTOOLFLAGS = --tag=disable-static
daq_EthernetIP_la_LDFLAGS = -module -avoid-version $(top_builddir)/src/liboscada.la
endif

daq_EthernetIP_la_SOURCES = moduls.cpp ethernet_ip_daq.cpp

I18N_mod = $(oscd_modulpref)EthernetIP
include ../../../../I18N.mk
