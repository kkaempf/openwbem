include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libcmpisupport.a
OW_SHARED_LIBS = libcmpisupport.so
SHAREDLIBADD =

thelibdir = $(libdir)

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -s $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done

libcmpisupport_a_SOURCES = \
cmpidt.h \
cmpift.h \
cmpimacs.h \
CmpiBroker.h \
CmpiContext.h \
CmpiData.h \
CmpiInstance.h \
CmpiInstanceMI.h \
CmpiObjectPath.h \
CmpiResult.h \
CmpiString.h \
cmpisrv.h \
cmpiArray.cpp \
cmpiBrokerEnc.cpp \
cmpiContextArgs.cpp \
cmpiDateTime.cpp \
cmpiInstance.cpp \
cmpiObject.cpp \
cmpiObjectPath.cpp \
cmpiResult.cpp \
cmpiString.cpp \
cmpiThreadContext.h \
cmpiThreadContext.cpp \
cmpiValue.cpp 

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

pkginclude_HEADERS = \
cmpidt.h \
cmpift.h \
cmpimacs.h \
cmpisrv.h \
CmpiArgs.h \
CmpiArray.h \
CmpiBroker.h \
CmpiContext.h \
CmpiData.h \
CmpiDateTime.h \
CmpiEnumeration.h \
CmpiInstance.h \
CmpiInstanceMI.h \
CmpiMethodMI.h \
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

