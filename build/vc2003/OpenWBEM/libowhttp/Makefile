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

MAP2DEF = $(TOOLSDIR)\owmap2def.exe

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
CPPFLAGS = /GF /EHsc /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
OBJDIR = Debug
LINKDBG = /DEBUG
!ELSE
CPPDEFS = /D"OW_HTTP_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /EHsc /RTC1 /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
OBJDIR = Release
LINKDBG = 
!ENDIF

COMMONDIR = $(SRCDIR)\http\common
CLIENTSRCDIR = $(SRCDIR)\http\client

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

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(COMMONOBJS) $(CLIENTOBJS) $(MAP2DEF)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" $(COMMONOBJS) $(CLIENTOBJS) $(LIBS)

#	$(MAP2DEF) $(TARGETDIR)\$(LIBNAME).map $(TARGETDIR)\$(LIBNAME).def
#	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
#		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
#		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" /DEF:"$(TARGETDIR)\$(LIBNAME).def" \
#		$(COMMONOBJS) $(CLIENTOBJS) $(LIBS)

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

COMMONSRCDIR = $(SRCDIR)\http\common
CLIENTSRCDIR = $(SRCDIR)\http\client

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

