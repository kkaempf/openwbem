DEBUG = 1
WARNLEVEL = /W0
LIBNAME = libowxml
OPENSSLBASE = C:\openssl

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src
TOOLSDIR = ..\tools

MAP2DEF = $(TOOLSDIR)\owmap2def.exe

INCS = /I".." /I$(SRCDIR)\common /I$(SRCDIR)\cim /I$(SRCDIR)\ifcs /I$(SRCDIR)\common\socket /I"C:\OpenSSL\include" /I$(SRCDIR)\client

!IFDEF DEBUG
BUILDTYPE = Debug
CPPDEFS = /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /EHsc /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
LINKDBG = /DEBUG
!ELSE
BUILDTYPE = Release
CPPDEFS = /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /EHsc /RTC1 /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
LINKDBG = 
!ENDIF

BASEDIR = ..
TARGET = $(LIBNAME).dll
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)
OBJDIR = $(BUILDTYPE)

INCS = \
/I$(BASEDIR) \
/I$(SRCDIR)\common \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\common\socket \
/I$(OPENSSLBASE)\include \
/I$(SRCDIR)\client


LIBPATHS = \
/LIBPATH:$(OPENSSLBASE)\lib\vc \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
libeay32.lib \
ssleay32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem.lib 

XMLSRCDIR = $(SRCDIR)\xml
OBJS = \
	$(OBJDIR)\OW_CIMtoXML.obj \
	$(OBJDIR)\OW_CIMXMLParser.obj \
	$(OBJDIR)\OW_Param.obj \
	$(OBJDIR)\OW_RequestHandlerIFCXML.obj \
	$(OBJDIR)\OW_XMLAttribute.obj \
	$(OBJDIR)\OW_XMLCIMFactory.obj \
	$(OBJDIR)\OW_XMLClass.obj \
	$(OBJDIR)\OW_XMLEscape.obj \
	$(OBJDIR)\OW_XMLNode.obj \
	$(OBJDIR)\OW_XMLOperationGeneric.obj \
	$(OBJDIR)\OW_XMLParseException.obj \
	$(OBJDIR)\OW_XMLParserCore.obj \
	$(OBJDIR)\OW_XMLParserDOM.obj \
	$(OBJDIR)\OW_XMLParserSAX.obj \
	$(OBJDIR)\OW_XMLPrettyPrint.obj \
	$(OBJDIR)\OW_XMLPullParser.obj \
	$(OBJDIR)\OW_XMLQualifier.obj \
	$(OBJDIR)\OW_XMLUnescape.obj

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(OBJS) $(MAP2DEF)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\libowxml.map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\libowxml.lib" $(OBJS) $(LIBS)
	$(MAP2DEF) $(TARGETDIR)\libowxml.map $(TARGETDIR)\libowxml.def
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\libowxml.map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\libowxml.lib" /DEF:"$(TARGETDIR)\libowxml.def" \
		$(OBJS) $(LIBS)

clean:
	-@del $(OBJDIR)\*.obj
	-@del $(OBJDIR)\*.map
	-@del $(OBJDIR)\*.def
	-@del $(OBJDIR)\*.dll

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))


$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))

$(MAP2DEF): $(TOOLSDIR)\owmap2def.cpp
	cl /EHsc /Fe"$(TOOLSDIR)\owmap2def.exe" $(TOOLSDIR)\owmap2def.cpp dbghelp.lib

###############################################################################

XMLSRCDIR = $(SRCDIR)\xml

$(OBJDIR)\OW_CIMtoXML.obj: $(XMLSRCDIR)\OW_CIMtoXML.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_CIMtoXML.cpp

$(OBJDIR)\OW_CIMXMLParser.obj: $(XMLSRCDIR)\OW_CIMXMLParser.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_CIMXMLParser.cpp

$(OBJDIR)\OW_Param.obj: $(XMLSRCDIR)\OW_Param.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_Param.cpp

$(OBJDIR)\OW_RequestHandlerIFCXML.obj: $(XMLSRCDIR)\OW_RequestHandlerIFCXML.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_RequestHandlerIFCXML.cpp

$(OBJDIR)\OW_XMLAttribute.obj: $(XMLSRCDIR)\OW_XMLAttribute.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLAttribute.cpp

$(OBJDIR)\OW_XMLCIMFactory.obj: $(XMLSRCDIR)\OW_XMLCIMFactory.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLCIMFactory.cpp

$(OBJDIR)\OW_XMLClass.obj: $(XMLSRCDIR)\OW_XMLClass.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLClass.cpp

$(OBJDIR)\OW_XMLEscape.obj: $(XMLSRCDIR)\OW_XMLEscape.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLEscape.cpp

$(OBJDIR)\OW_XMLNode.obj: $(XMLSRCDIR)\OW_XMLNode.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLNode.cpp

$(OBJDIR)\OW_XMLOperationGeneric.obj: $(XMLSRCDIR)\OW_XMLOperationGeneric.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLOperationGeneric.cpp

$(OBJDIR)\OW_XMLParseException.obj: $(XMLSRCDIR)\OW_XMLParseException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLParseException.cpp

$(OBJDIR)\OW_XMLParserCore.obj: $(XMLSRCDIR)\OW_XMLParserCore.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLParserCore.cpp

$(OBJDIR)\OW_XMLParserDOM.obj: $(XMLSRCDIR)\OW_XMLParserDOM.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLParserDOM.cpp

$(OBJDIR)\OW_XMLParserSAX.obj: $(XMLSRCDIR)\OW_XMLParserSAX.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLParserSAX.cpp

$(OBJDIR)\OW_XMLPrettyPrint.obj: $(XMLSRCDIR)\OW_XMLPrettyPrint.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLPrettyPrint.cpp

$(OBJDIR)\OW_XMLPullParser.obj: $(XMLSRCDIR)\OW_XMLPullParser.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLPullParser.cpp

$(OBJDIR)\OW_XMLQualifier.obj: $(XMLSRCDIR)\OW_XMLQualifier.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLQualifier.cpp

$(OBJDIR)\OW_XMLUnescape.obj: $(XMLSRCDIR)\OW_XMLUnescape.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\OW_XMLUnescape.cpp


