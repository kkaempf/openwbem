DEBUG = 1
WARNLEVEL = /W0
LIBNAME = libowxml
OPENSSLBASE = C:\openssl

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src

!IFDEF DEBUG
BUILDTYPE = Debug
CPPDEFS = /D"OW_XML_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
LINKDBG = /DEBUG
!ELSE
BUILDTYPE = Release
CPPDEFS = /D"OW_XML_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
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

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(OBJS)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\libowxml.map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\libowxml.lib" $(OBJS) $(LIBS)

clean:
	-@del $(OBJDIR)\*.obj
	-@del $(OBJDIR)\*.map
	-@del $(OBJDIR)\*.def
	-@del $(OBJDIR)\*.dll

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))


$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))

###############################################################################

XMLSRCDIR = $(SRCDIR)\xml

$(OBJDIR)\OW_CIMtoXML.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMXMLParser.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Param.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_RequestHandlerIFCXML.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLAttribute.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLCIMFactory.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLClass.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLEscape.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLNode.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLOperationGeneric.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLParseException.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLParserCore.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLParserDOM.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLParserSAX.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLPrettyPrint.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLPullParser.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLQualifier.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLUnescape.obj: $(XMLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(XMLSRCDIR)\$(@B).cpp


