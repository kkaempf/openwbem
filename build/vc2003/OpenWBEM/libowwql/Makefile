DEBUG = 1
WARNLEVEL = 0

BUILDTYPE = Debug
LIBNAME = libowwql

TARGET = $(LIBNAME).dll
BASEDIR = ..
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src
WQLSRCDIR = $(SRCDIR)\wql

INCS = \
/I".." \
/I$(WQLSRCDIR) \
/I$(SRCDIR)\common \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\client \
/I$(SRCDIR)\wql\common

LIBPATHS = \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem.lib \
libowwqlcommon.lib

!IFDEF DEBUG
CPPDEFS = /D"OW_WQL_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Z7 /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
OBJDIR = Debug
LINKDBG = /DEBUG
!ELSE
CPPDEFS = /D"OW_WQL_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /MD /c
OBJDIR = Release
LINKDBG = 
!ENDIF

OBJS = \
	$(OBJDIR)\OW_WQLAst.obj \
	$(OBJDIR)\OW_WQLImpl.obj \
	$(OBJDIR)\OW_WQLParser.obj \
	$(OBJDIR)\OW_WQLLexer.obj \
	$(OBJDIR)\OW_WQLProcessor.obj \
	$(OBJDIR)\OW_WQLScanUtils.obj \
	$(OBJDIR)\OW_WQLSelectStatementGen.obj

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(OBJS)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" $(OBJS) $(LIBS)

clean:
	-@del OW_WQLParser.cpp
	-@del OW_WQLParser.h
	-@del OW_WQLLexer.cpp
	-@del $(OBJDIR)\*.obj
	-@del $(OBJDIR)\*.map
	-@del $(OBJDIR)\*.def
	-@del $(OBJDIR)\*.dll

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))

$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))

###############################################################################
$(OBJDIR)\OW_WQLAst.obj: $(WQLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLImpl.obj: $(WQLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLParser.obj: $(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(@B).cpp

OW_WQLParser.cpp: $(WQLSRCDIR)\OW_WQLParser.yy
	-@del OW_WQLParser.cpp
	-@del OW_WQLParser.h
	-@echo #include "OW_config.h" 1 > OW_WQLParser.cpp
	bison -y -d -powwql $(WQLSRCDIR)\OW_WQLParser.yy
	rename y.tab.h OW_WQLParser.h
	sed "/\#line/d" y.tab.c >> OW_WQLParser.cpp
	-@del y.tab.c

#	sed "s/\\/\\\\/g" y.tab.c >> OW_WQLParser.cpp
#	sed "/\#line/d" y.tab.c >> OW_WQLParser.cpp
#	rename y.tab.c OW_WQLParser.cpp

$(OBJDIR)\OW_WQLLexer.obj: $(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(@B).cpp

OW_WQLLexer.cpp: $(WQLSRCDIR)\OW_WQLLexer.ll
	-@del OW_WQLLexer.cpp
	-@echo #define YY_NEVER_INTERACTIVE 1 > OW_WQLLexer.cpp
	flex -it -Cfe -Powwql $(WQLSRCDIR)\OW_WQLLexer.ll | sed "/unistd/d"	>> OW_WQLLexer.cpp

$(OBJDIR)\OW_WQLProcessor.obj: $(WQLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLScanUtils.obj: $(WQLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_WQLSelectStatementGen.obj: $(WQLSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(WQLSRCDIR)\$(@B).cpp

