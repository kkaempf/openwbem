include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowxml.a
SHAREDLIBADD =

OW_SHARED_LIBS = libowxml.so
thelibdir = $(libdir)

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

