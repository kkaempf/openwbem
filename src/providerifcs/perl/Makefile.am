SUBDIRS = perlProvider
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libperlprovifc.a
OW_SHARED_LIBS = libperlprovifc.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/providerifcs/npi/common -lnpisupport \
-L$(top_builddir)/src/wql/common -lowwqlcommon 

thelibdir = $(pkglibdir)/provifcs

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_S=
else
ASSOCIATION_S= \
OW_PerlAssociatorProviderProxy.cpp \
OW_PerlAssociatorProviderProxy.hpp
endif

libperlprovifc_a_SOURCES = \
$(ASSOCIATION_S) \
OW_PerlInstanceProviderProxy.cpp \
OW_PerlInstanceProviderProxy.hpp \
OW_PerlMethodProviderProxy.cpp \
OW_PerlMethodProviderProxy.hpp \
OW_PerlPolledProviderProxy.cpp \
OW_PerlPolledProviderProxy.hpp \
OW_PerlProviderIFC.cpp \
OW_PerlProviderIFC.hpp \
OW_PerlIndicationProviderProxy.cpp \
OW_PerlIndicationProviderProxy.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi \
-I$(top_srcdir)/src/providerifcs/npi/common \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/wql/common

