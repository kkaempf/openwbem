include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowxml.a
SHAREDLIBADD =

OW_SHARED_LIBS = libowxml.so
thelibdir = $(libdir)

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



libowxml_a_SOURCES = \
OW_CIMXMLParser.cpp \
OW_RequestHandlerIFCXML.cpp \
OW_XMLParser.cpp \
OW_XMLOperationGeneric.cpp \
OW_Param.cpp \
OW_XMLClass.cpp \
OW_XMLQualifier.cpp \
OW_XMLParameters.cpp \
OW_XMLEscape.cpp \
OW_XMLUnescape.cpp \
OW_XMLCIMFactory.cpp \
OW_CIMtoXML.cpp \
OW_XMLPrettyPrint.cpp

pkginclude_HEADERS = \
OW_CIMXMLParser.hpp \
OW_RequestHandlerIFCXML.hpp \
OW_Param.hpp         \
OW_XMLClass.hpp \
OW_XMLOperationGeneric.hpp \
OW_XMLParameters.hpp \
OW_XMLParser.hpp \
OW_XMLQualifier.hpp \
OW_XMLEscape.hpp \
OW_XMLUnescape.hpp \
OW_XMLCIMFactory.hpp \
OW_CIMtoXML.hpp \
OW_XMLPrettyPrint.hpp



INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/client

