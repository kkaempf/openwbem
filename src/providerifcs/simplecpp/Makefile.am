include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libsimplecppprovifc.a
OW_SHARED_LIBS = libsimplecppprovifc.so
SHAREDLIBADD =

thelibdir = $(pkglibdir)/provifcs

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


if ENABLE_PROPERTY_PROVIDERS
PROPERTY_PROVIDER_S= \
OW_SimpleCppPropertyProviderIFC.hpp \
OW_SimpleCppPropertyProviderIFC.cpp
else
PROPERTY_PROVIDER_S=
endif


libsimplecppprovifc_a_SOURCES = \
OW_SimpleCppAssociatorProviderIFC.hpp \
OW_SimpleCppAssociatorProviderIFC.cpp \
OW_SimpleCppInstanceProviderIFC.hpp \
OW_SimpleCppInstanceProviderIFC.cpp \
$(PROPERTY_PROVIDER_S) \
OW_SimpleCppProviderBaseIFC.hpp \
OW_SimpleCppProviderIFC.cpp \
OW_SimpleCppProviderIFC.hpp \
OW_SimpleCppProxyProvider.cpp \
OW_SimpleCppProxyProvider.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/server

