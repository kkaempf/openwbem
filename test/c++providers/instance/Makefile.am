include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = \
liblaptopbat.a \
liblinuxprocess.a \
librpmip.a 


OW_SHARED_LIBS = \
liblaptopbat.so \
liblinuxprocess.so \
librpmip.so 

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


liblaptopbat_a_SOURCES = \
OW_LaptopBattery.cpp

liblinuxprocess_a_SOURCES = \
OW_LinuxProcess.cpp

librpmip_a_SOURCES = \
OW_RPM.cpp

EXTRA_DIST = LaptopBattery.mof LinuxProcess.mof SoftwarePackage.mof

INCLUDES = \
-I$(top_srcdir)/src/common -I$(top_srcdir)/src/thread -I$(top_srcdir)/src \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/net/socket 


