BASEDIR = ..
SRCDIR = ..\..\..\..\src
LINK = link
WARNLEVEL = 2
OPENSSLBASE = c:\openssl
ZLIBBASE = c:\zlib
CC = cl

!IFDEF DEBUG

BUILDTYPE = Debug
OBJDIR = Debug
BLDCHAR = D
LINKDBG = /DEBUG
CCDEFS = /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CCFLAGS = /GF /GX /RTC1 /Z7 /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
CPPDEFS = /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Z7 /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"

!ELSE

BUILDTYPE = Release
OBJDIR = Release
BLDCHAR = 
LINKDBG =
CCDEFS = /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CCFLAGS = /GF /GX /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /MD /c /Fo"$@"
CPPDEFS = /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /MD /c /Fo"$@"

!ENDIF

TARGETDIR = $(BASEDIR)\$(BUILDTYPE)
SSLLIBS = libeay32.lib ssleay32.lib
ZLIB = zdll.lib


