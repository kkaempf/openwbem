include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
liblaptopbat.a \
liblinuxprocess.a \
libtestinstance.a \
libinfiniteip.a

#librpmip.a 

OW_SHARED_LIBS = \
liblaptopbat.so \
liblinuxprocess.so \
libtestinstance.so \
libinfiniteip.so

#librpmip.so 

libinfiniteip_a_SOURCES = \
OW_Infinity.cpp

liblaptopbat_a_SOURCES = \
OW_LaptopBattery.cpp

liblinuxprocess_a_SOURCES = \
OW_LinuxProcess.cpp

#librpmip_a_SOURCES = \
#OW_RPM.cpp

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


