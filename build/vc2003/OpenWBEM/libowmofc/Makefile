!include ..\common.mk

LIBNAME = libowmofc
BASEDIR = ..

TARGET = $(LIBNAME).dll
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)

SRCDIR = ..\..\..\..\src

INCS = \
/I".." \
/I$(SRCDIR)\mof \
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

!IFDEF DEBUG
CPPDEFS = /D"OW_MOF_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Z7 /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
OBJDIR = Debug
LINKDBG = /DEBUG
!ELSE
CPPDEFS = /D"OW_MOF_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR /W$(WARNLEVEL) /nologo /Wp64 /MD /c
OBJDIR = Release
LINKDBG = 
!ENDIF

MOFSRCDIR = $(SRCDIR)\mof

OBJS = \
	$(OBJDIR)\OW_MOFCIMOMVisitor.obj \
	$(OBJDIR)\OW_MOFCompiler.obj \
	$(OBJDIR)\OW_MOFParseFatalErrorException.obj \
	$(OBJDIR)\OW_MOFParserErrorHandlerIFC.obj \
	$(OBJDIR)\OW_MOFVisitor.obj \
	$(OBJDIR)\OW_MOFParser.obj \
	$(OBJDIR)\OW_MOFLexer.obj

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(OBJS)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" $(OBJS) $(LIBS)

clean:
	-@del OW_MOFParser.cpp
	-@del OW_MOFParser.h
	-@del OW_MOFLexer.cpp
	-@del $(OBJDIR)\*.obj
	-@del $(OBJDIR)\*.map
	-@del $(OBJDIR)\*.def
	-@del $(OBJDIR)\*.dll

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))

$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))

###############################################################################

$(OBJDIR)\OW_MOFCIMOMVisitor.obj: $(MOFSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MOFCompiler.obj: $(MOFSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MOFParseFatalErrorException.obj: $(MOFSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MOFParserErrorHandlerIFC.obj: $(MOFSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MOFVisitor.obj: $(MOFSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MOFParser.obj: $(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(@B).cpp

$(OBJDIR)\OW_MOFLexer.obj: $(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(@B).cpp

OW_MOFParser.cpp: $(MOFSRCDIR)\OW_MOFParser.yy
	-@del OW_MOFParser.cpp
	-@del OW_MOFParser.h
	bison -y -d -powmof $(MOFSRCDIR)\OW_MOFParser.yy
	sed "/\#line/d" y.tab.c >> OW_MOFParser.cpp
	rename y.tab.h OW_MOFParser.h
	-@del y.tab.c

OW_MOFLexer.cpp: $(MOFSRCDIR)\OW_MOFLexer.ll
	-@del OW_MOFLexer.cpp
	-@echo #define YY_NEVER_INTERACTIVE 1 > OW_MOFLexer.cpp
	flex -it -Cfe -Powmof $(MOFSRCDIR)\OW_MOFLexer.ll | sed "/unistd/d"	>> OW_MOFLexer.cpp

#	flex -i -Cfe -Powmof -oOW_MOFLexer.cpp $(MOFSRCDIR)\OW_MOFLexer.ll

