DEBUG = 1
WARNLEVEL = /W0
BASEDIR = ..

ZLIBBASE = "c:\zlib"
OPENSSLBASE = "c:\openssl"

BUILDTYPE = Debug
LIBNAME = libcimxmllistener

TARGET = $(LIBNAME).dll
BASEDIR = ..
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src

INCS = \
/I".." \
/I$(SRCDIR)\common \
/I$(SRCDIR)\common\socket \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\http\common \
/I$(SRCDIR)\http\client \
/I$(SRCDIR)\services\http \
/I$(SRCDIR)\client \
/I$(SRCDIR)\xml \
/I$(SRCDIR)\requesthandlers\cimxml \
/I$(OPENSSLBASE)\include \
/I$(ZLIBBASE)\include

LIBPATHS = \
/LIBPATH:$(OPENSSLBASE)\lib\vc \
/LIBPATH:$(ZLIBBASE)\lib \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
libeay32.lib \
ssleay32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
zdll.lib \
libopenwbem.lib \
libowhttp.lib \
libowclient.lib \
libowxml.lib 

!IFDEF DEBUG
CPPDEFS = /D"OW_HTTP_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
OBJDIR = Debug
LINKDBG = /DEBUG
!ELSE
CPPDEFS = /D"OW_HTTP_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
OBJDIR = Release
LINKDBG = 
!ENDIF

OBJS = \
$(OBJDIR)\OW_CIMListenerCallback.obj \
$(OBJDIR)\OW_XMLListener.obj \
$(OBJDIR)\OW_ListenerAuthenticator.obj \
$(OBJDIR)\OW_HTTPXMLCIMListener.obj

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(OBJS) 
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" $(OBJS) $(LIBS)

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

LIBSRCDIR = $(SRCDIR)\cimxmllistener

$(OBJDIR)\OW_CIMListenerCallback.obj: $(LIBSRCDIR)\OW_CIMListenerCallback.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\OW_CIMListenerCallback.cpp

$(OBJDIR)\OW_XMLListener.obj: $(LIBSRCDIR)\OW_XMLListener.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\OW_XMLListener.cpp

$(OBJDIR)\OW_ListenerAuthenticator.obj: $(LIBSRCDIR)\OW_ListenerAuthenticator.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\OW_ListenerAuthenticator.cpp

$(OBJDIR)\OW_HTTPXMLCIMListener.obj: $(LIBSRCDIR)\OW_HTTPXMLCIMListener.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\OW_HTTPXMLCIMListener.cpp

