SUBDIRS = common
include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libcmpiprovifc.a
OW_SHARED_LIBS = libcmpiprovifc.so
SHAREDLIBADD = \
-L$(top_builddir)/src/providerifcs/cmpi/common -lcmpisupport \
-L$(top_builddir)/src/wql/common -lowwqlcommon 

thelibdir = $(pkglibdir)/provifcs

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

#OW_CMPIAssociatorProviderProxy.cpp \
#OW_CMPIPolledProviderProxy.cpp  \
#OW_CMPIIndicationProviderProxy.cpp \
#OW_CMPIMethodProviderProxy.cpp  \

libcmpiprovifc_a_SOURCES = \
CMPIProvider.hpp \
OW_CMPIInstanceProviderProxy.cpp \
OW_CMPIInstanceProviderProxy.hpp \
OW_CMPIAssociatorProviderProxy.hpp \
OW_CMPIMethodProviderProxy.hpp  \
OW_CMPIPolledProviderProxy.hpp  \
OW_CMPIIndicationProviderProxy.hpp \
OW_CMPIProviderIFC.cpp \
OW_CMPIProviderIFC.hpp \
OW_CMPIProviderIFCUtils.hpp \
OW_FTABLERef.hpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cmpi/common \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/wql/common

