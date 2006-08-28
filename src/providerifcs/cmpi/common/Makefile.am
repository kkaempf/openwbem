MAKEDYLIB=1

if ENABLE_CMPI

include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libcmpisupport.a
OW_SHARED_LIBS = libcmpisupport.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/src/provider -lowprovider 
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libcmpisupport_a_SOURCES = \
cmpiArray.cpp \
cmpiBroker.cpp \
cmpiBrokerEnc.cpp \
cmpiBrokerExt.cpp \
cmpiContextArgs.cpp \
cmpiDateTime.cpp \
cmpiEnumeration.cpp \
cmpiInstance.cpp \
cmpiObject.cpp \
cmpiObjectPath.cpp \
cmpiResult.cpp \
cmpiString.cpp \
cmpiThreadContext.cpp \
cmpiValue.cpp \
CmpiImpl.cpp \
cmpidt.h \
cmpift.h \
cmpios.h \
cmpimacs.h \
cmpisrv.h \
cmpiThreadContext.h \
CmpiArgs.h \
CmpiArray.h \
CmpiBaseMI.h \
CmpiBroker.h \
CmpiContext.h \
CmpiData.h \
CmpiDateTime.h \
CmpiEnumeration.h \
CmpiInstance.h \
CmpiInstanceMI.h \
CmpiMethodMI.h \
CmpiObject.h \
CmpiObjectPath.h \
CmpiResult.h \
CmpiStatus.h \
CmpiString.h \
CmpiAssociationMI.h \
CmpiIndicationMI.h \
CmpiPropertyMI.h \
CmpiProviderBase.h \
CmpiSelectExp.h

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi 

endif

