include $(top_srcdir)/Makefile-shared-lib.inc
include $(top_srcdir)/Makefile-shared-lib-inst.inc

noinst_LIBRARIES = libcmpisupport.a
OW_SHARED_LIBS = libcmpisupport.so
SHAREDLIBADD =

thelibdir = $(libdir)

libcmpisupport_a_SOURCES = \
cmpiArray.cpp \
cmpiBrokerEnc.cpp \
cmpiContextArgs.cpp \
cmpiDateTime.cpp \
cmpiInstance.cpp \
cmpiObject.cpp \
cmpiObjectPath.cpp \
cmpiResult.cpp \
cmpiString.cpp \
cmpiThreadContext.cpp \
cmpiValue.cpp 

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

pkginclude_HEADERS = \
cmpidt.h \
cmpift.h \
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
CmpiString.h 

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi \
-I$(top_srcdir)/src/server 

