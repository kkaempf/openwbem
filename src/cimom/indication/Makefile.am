include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowindicationserver.a libowindicationreplayer.a
SHAREDLIBADD = -lpthread

OW_SHARED_LIBS = libowindicationserver.so libowindicationreplayer.so
thelibdir = $(libdir)/openwbem

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

libowindicationserver_a_SOURCES = \
OW_IndicationServer.hpp \
OW_IndicationServerImpl.hpp \
OW_IndicationServerImpl.cpp

libowindicationreplayer_a_SOURCES = \
OW_IndicationRepLayer.hpp \
OW_IndicationRepLayerImpl.hpp \
OW_IndicationRepLayerImpl.cpp

INCLUDES = \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/xml



