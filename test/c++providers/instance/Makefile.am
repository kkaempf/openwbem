include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = \
liblaptopbat.a \
liblinuxprocess.a \
librpmip.a \
libtestinstance.a \
libinfiniteip.a


OW_SHARED_LIBS = \
liblaptopbat.so \
liblinuxprocess.so \
librpmip.so \
libtestinstance.so \
libinfiniteip.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done

libinfiniteip_a_SOURCES = \
OW_Infinity.cpp

liblaptopbat_a_SOURCES = \
OW_LaptopBattery.cpp

liblinuxprocess_a_SOURCES = \
OW_LinuxProcess.cpp

librpmip_a_SOURCES = \
OW_RPM.cpp

libtestinstance_a_SOURCES = \
testinstance.cpp

EXTRA_DIST = \
demo_device.mof \
demo.mof \
demo_system.mof \
LaptopBattery.mof \
InfiniteInstances.mof \
LinuxProcess.mof \
SoftwarePackage.mof \
testinstance.mof

INCLUDES = \
-I$(top_srcdir)/src/common -I$(top_srcdir)/src/thread -I$(top_srcdir)/src \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cimom 


