include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libcppindicationexportxmlhttpprovider.a

OW_SHARED_LIBS = libcppindicationexportxmlhttpprovider.so
SHAREDLIBADD = -lpthread $(top_builddir)/src/http/common/libowhttpcommon.a $(top_builddir)/src/http/client/libowhttpclient.a $(top_builddir)/src/xml/libowxml.a
#SHAREDLIBADD = -L$(top_builddir)/src/http/common -lowhttpcommon -L$(top_builddir)/src/http/client -lowhttpclient -L$(top_builddir)/src/xml -lowxml

thelibdir = $(pkglibdir)/c++providers
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


libcppindicationexportxmlhttpprovider_a_SOURCES = \
OW_CppIndicationExportXMLHTTPProvider.cpp \
OW_CppIndicationExportXMLHTTPProvider.hpp \
OW_IndicationExporter.hpp \
OW_IndicationExporter.cpp


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

