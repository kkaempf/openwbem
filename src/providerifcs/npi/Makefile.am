SUBDIRS = common
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libnpiprovifc.a
OW_SHARED_LIBS = libnpiprovifc.so
SHAREDLIBADD = \
-L$(top_builddir)/src/providerifcs/npi/common -lnpisupport

thelibdir = $(pkglibdir)/provifcs

libnpiprovifc_a_SOURCES = \
NPIProvider.hpp \
OW_FTABLERef.hpp \
OW_NPIAssociatorProviderProxy.cpp \
OW_NPIAssociatorProviderProxy.hpp \
OW_NPIInstanceProviderProxy.cpp \
OW_NPIInstanceProviderProxy.hpp \
OW_NPIMethodProviderProxy.cpp \
OW_NPIMethodProviderProxy.hpp \
OW_NPIProviderIFC.cpp \
OW_NPIProviderIFC.hpp \
OW_NPIProviderIFCUtils.hpp \
NPIProvider.hpp 

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi/common \
-I$(top_srcdir)/src/server

