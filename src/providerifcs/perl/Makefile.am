SUBDIRS = common
include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libperlprovifc.a
OW_SHARED_LIBS = libperlprovifc.so
SHAREDLIBADD = \
-L$(top_builddir)/src/providerifcs/perl/common -lperlsupport \
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


libperlprovifc_a_SOURCES = \
NPIProvider.hpp \
OW_PerlAssociatorProviderProxy.cpp \
OW_PerlAssociatorProviderProxy.hpp \
OW_PerlInstanceProviderProxy.cpp \
OW_PerlInstanceProviderProxy.hpp \
OW_PerlMethodProviderProxy.cpp \
OW_PerlMethodProviderProxy.hpp \
OW_PerlPolledProviderProxy.cpp \
OW_PerlPolledProviderProxy.hpp \
OW_PerlProviderIFC.cpp \
OW_PerlProviderIFC.hpp \
OW_PerlProviderIFCUtils.hpp \
OW_PerlIndicationProviderProxy.cpp \
OW_PerlIndicationProviderProxy.hpp \
OW_FTABLERef.hpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/perl/common \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/wql/common

