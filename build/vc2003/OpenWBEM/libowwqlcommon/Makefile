DEBUG = 1
WARNLEVEL = 0
LIBNAME = libowwqlcommon
OPENSSLBASE = C:\openssl

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src

!IFDEF DEBUG
BUILDTYPE = Debug
CPPDEFS = /D"OW_WQLCOMMON_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Z7 /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
LINKDBG = /DEBUG
!ELSE
BUILDTYPE = Release
CPPDEFS = /D"OW_WQLCOMMON_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /MD /c
LINKDBG = 
!ENDIF

BASEDIR = ..
TARGET = $(LIBNAME).dll
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)
OBJDIR = $(BUILDTYPE)

INCS = \
/I$(BASEDIR) \
/I$(SRCDIR)\common \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\cim

LIBPATHS = \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem.lib 

OBJS = \
	$(OBJDIR)\OW_WQLCompile.obj \
	$(OBJDIR)\OW_WQLOperand.obj \
	$(OBJDIR)\OW_WQLPropertySource.obj \
	$(OBJDIR)\OW_WQLSelectStatement.obj \
	$(OBJDIR)\OW_WQLInstancePropertySource.obj

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

WQLCOMSRCDIR = $(SRCDIR)\wql\common

$(OBJDIR)\OW_WQLCompile.obj:
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLCOMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLOperand.obj:
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLCOMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLPropertySource.obj:
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLCOMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLSelectStatement.obj:
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLCOMSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLInstancePropertySource.obj:
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLCOMSRCDIR)\$(@B).cpp

