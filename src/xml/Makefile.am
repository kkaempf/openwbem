MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowxml.a
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem

OW_SHARED_LIBS = libowxml.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libowxml_a_SOURCES = \
OW_CIMXMLParser.cpp \
OW_RequestHandlerIFCXML.cpp \
OW_XMLParserCore.cpp \
OW_XMLOperationGeneric.cpp \
OW_Param.cpp \
OW_XMLClass.cpp \
OW_XMLQualifier.cpp \
OW_XMLEscape.cpp \
OW_XMLUnescape.cpp \
OW_XMLCIMFactory.cpp \
OW_CIMtoXML.cpp \
OW_XMLPrettyPrint.cpp \
OW_XMLParseException.cpp \
OW_XMLPullParser.cpp \
OW_XMLParserSAX.cpp \
OW_XMLParserDOM.cpp \
OW_XMLNode.cpp \
OW_XMLAttribute.cpp

pkginclude_HEADERS = \
OW_CIMXMLParser.hpp \
OW_RequestHandlerIFCXML.hpp \
OW_Param.hpp         \
OW_XMLClass.hpp \
OW_XMLOperationGeneric.hpp \
OW_XMLParserCore.hpp \
OW_XMLQualifier.hpp \
OW_XMLEscape.hpp \
OW_XMLUnescape.hpp \
OW_XMLCIMFactory.hpp \
OW_CIMtoXML.hpp \
OW_XMLPrettyPrint.hpp \
OW_XMLParseException.hpp \
OW_XMLPullParser.hpp \
OW_XMLParserSAX.hpp \
OW_XMLParserDOM.hpp \
OW_XMLNode.hpp \
OW_XMLAttribute.hpp \
OW_XMLFwd.hpp



INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs

