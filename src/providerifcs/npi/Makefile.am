SUBDIRS = common
include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libnpiprovifc.a
OW_SHARED_LIBS = libnpiprovifc.so
SHAREDLIBADD = -lpthread

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




libnpiprovifc_a_SOURCES = \
NPIAssociationProvider.cpp \
NPIAssociationProvider.hpp \
NPIBaseProvider.cpp \
NPIBaseProvider.hpp \
NPIExternal.cpp \
NPIExternal.hpp \
NPIInstanceProvider.cpp \
NPIInstanceProvider.hpp \
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
OW_NPIProviderIFCUtils.hpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi/common \
-I$(top_srcdir)/src/server

