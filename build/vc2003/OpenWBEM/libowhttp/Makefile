DEBUG = 1
WARNLEVEL = /W0
BASEDIR = ..

ZLIBBASE = "c:\zlib"
OPENSSLBASE = "c:\openssl"

BUILDTYPE = Debug
LIBNAME = libowhttp

TARGET = $(LIBNAME).dll
BASEDIR = ..
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src
TOOLSDIR = ..\tools

INCS = \
/I".." \
/I$(SRCDIR)\common \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\common\socket \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\client \
/I$(SRCDIR)\http\common \
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
libopenwbem.lib 

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

COMMONOBJS = \
	$(OBJDIR)\OW_HTTPChunkedIStream.obj \
	$(OBJDIR)\OW_HTTPChunkedOStream.obj \
	$(OBJDIR)\OW_HTTPChunkException.obj \
	$(OBJDIR)\OW_HTTPCounter.obj \
	$(OBJDIR)\OW_HTTPDeflateIStream.obj \
	$(OBJDIR)\OW_HTTPDeflateOStream.obj \
	$(OBJDIR)\OW_HTTPException.obj \
	$(OBJDIR)\OW_HTTPLenLimitIStream.obj \
	$(OBJDIR)\OW_HTTPUtils.obj

CLIENTOBJS = \
	$(OBJDIR)\OW_HTTPClient.obj

SERVICEOBJS = \
	$(OBJDIR)\OW_HTTPServer.obj \
	$(OBJDIR)\OW_HTTPSvrConnection.obj

OBJS = $(COMMONOBJS) $(CLIENTOBJS) $(SERVICEOBJS)

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

COMMONSRCDIR = $(SRCDIR)\http\common
CLIENTSRCDIR = $(SRCDIR)\http\client
SERVICESRCDIR = $(SRCDIR)\services\http


$(OBJDIR)\OW_HTTPChunkedIStream.obj: $(COMMONSRCDIR)\OW_HTTPChunkedIStream.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPChunkedIStream.cpp

$(OBJDIR)\OW_HTTPChunkedOStream.obj: $(COMMONSRCDIR)\OW_HTTPChunkedOStream.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPChunkedOStream.cpp

$(OBJDIR)\OW_HTTPChunkException.obj: $(COMMONSRCDIR)\OW_HTTPChunkException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPChunkException.cpp

$(OBJDIR)\OW_HTTPCounter.obj: $(COMMONSRCDIR)\OW_HTTPCounter.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPCounter.cpp

$(OBJDIR)\OW_HTTPDeflateIStream.obj: $(COMMONSRCDIR)\OW_HTTPDeflateIStream.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPDeflateIStream.cpp

$(OBJDIR)\OW_HTTPDeflateOStream.obj: $(COMMONSRCDIR)\OW_HTTPDeflateOStream.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPDeflateOStream.cpp

$(OBJDIR)\OW_HTTPException.obj: $(COMMONSRCDIR)\OW_HTTPException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPException.cpp

$(OBJDIR)\OW_HTTPLenLimitIStream.obj: $(COMMONSRCDIR)\OW_HTTPLenLimitIStream.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPLenLimitIStream.cpp

$(OBJDIR)\OW_HTTPUtils.obj: $(COMMONSRCDIR)\OW_HTTPUtils.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\OW_HTTPUtils.cpp

$(OBJDIR)\OW_HTTPClient.obj: $(CLIENTSRCDIR)\OW_HTTPClient.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\OW_HTTPClient.cpp

$(OBJDIR)\OW_HTTPServer.obj: $(SERVICESRCDIR)\OW_HTTPServer.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SERVICESRCDIR)\OW_HTTPServer.cpp

$(OBJDIR)\OW_HTTPSvrConnection.obj: $(SERVICESRCDIR)\OW_HTTPSvrConnection.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SERVICESRCDIR)\OW_HTTPSvrConnection.cpp

