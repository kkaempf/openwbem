MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
liblaptopbat.a \
liblinuxprocess.a \
librpmip.a \
libtestinstance.a \
libinfiniteip.a


OW_SHARED_LIBS = \
liblaptopbat.$(LIB_EXT) \
liblinuxprocess.$(LIB_EXT) \
librpmip.$(LIB_EXT) \
libtestinstance.$(LIB_EXT) \
libinfiniteip.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

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
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cimom/common

LOCAL_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

liblaptopbat_PROJECT_LIBS = $(LOCAL_LIBS)
liblaptopbat_EXTERNAL_LIBS = $(EXTERNAL_LIBS)
liblinuxprocess_PROJECT_LIBS = $(LOCAL_LIBS)
liblinuxprocess_EXTERNAL_LIBS = $(EXTERNAL_LIBS)
librpmip_PROJECT_LIBS = $(LOCAL_LIBS)
librpmip_EXTERNAL_LIBS = $(EXTERNAL_LIBS)
libtestinstance_PROJECT_LIBS = $(LOCAL_LIBS)
libtestinstance_EXTERNAL_LIBS = $(EXTERNAL_LIBS)
libinfiniteip_PROJECT_LIBS = $(LOCAL_LIBS)
libinfiniteip_EXTERNAL_LIBS = $(EXTERNAL_LIBS)
