include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libcppindicationexportxmlhttpprovider.a

OW_SHARED_LIBS = libcppindicationexportxmlhttpprovider.so

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
OW_IndicationExporter.hpp \
OW_IndicationExporter.cpp


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml

