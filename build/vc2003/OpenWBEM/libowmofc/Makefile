DEBUG = 1
WARNLEVEL = /W0

BUILDTYPE = Debug
LIBNAME = libowmofc

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
CPPDEFS = /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /EHsc /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
OBJDIR = Debug
LINKDBG = /DEBUG
!ELSE
CPPDEFS = /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /EHsc /RTC1 /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
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

$(TARGETDIR)\$(TARGET): $(OBJDIR) $(TARGETDIR) $(OBJS) $(MAP2DEF)
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" $(OBJS) $(LIBS)
	$(MAP2DEF) $(TARGETDIR)\$(LIBNAME).map $(TARGETDIR)\$(LIBNAME).def
	link /OUT:"$(TARGETDIR)\$(TARGET)" /NOLOGO $(LIBPATHS) /DLL /DEBUG \
		/MAP:"$(TARGETDIR)\$(LIBNAME).map" /MAPINFO:EXPORTS /SUBSYSTEM:WINDOWS \
		/IMPLIB:"$(TARGETDIR)\$(LIBNAME).lib" /DEF:"$(TARGETDIR)\$(LIBNAME).def" \
		$(OBJS) $(LIBS)

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

$(MAP2DEF): $(TOOLSDIR)\owmap2def.cpp
	cl /EHsc /Fe"$(TOOLSDIR)\owmap2def.exe" $(TOOLSDIR)\owmap2def.cpp dbghelp.lib

###############################################################################

$(OBJDIR)\OW_MOFCIMOMVisitor.obj: $(MOFSRCDIR)\OW_MOFCIMOMVisitor.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\OW_MOFCIMOMVisitor.cpp

$(OBJDIR)\OW_MOFCompiler.obj: $(MOFSRCDIR)\OW_MOFCompiler.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\OW_MOFCompiler.cpp

$(OBJDIR)\OW_MOFParseFatalErrorException.obj: $(MOFSRCDIR)\OW_MOFParseFatalErrorException.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\OW_MOFParseFatalErrorException.cpp

$(OBJDIR)\OW_MOFParserErrorHandlerIFC.obj: $(MOFSRCDIR)\OW_MOFParserErrorHandlerIFC.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\OW_MOFParserErrorHandlerIFC.cpp

$(OBJDIR)\OW_MOFVisitor.obj: $(MOFSRCDIR)\OW_MOFVisitor.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(MOFSRCDIR)\OW_MOFVisitor.cpp

$(OBJDIR)\OW_MOFParser.obj: OW_MOFParser.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) OW_MOFParser.cpp

$(OBJDIR)\OW_MOFLexer.obj: OW_MOFLexer.cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) OW_MOFLexer.cpp

OW_MOFParser.cpp: $(MOFSRCDIR)\OW_MOFParser.yy
	-@del OW_MOFParser.cpp
	-@del OW_MOFParser.h
	bison -y -d -powmof $(MOFSRCDIR)\OW_MOFParser.yy
	rename y.tab.c OW_MOFParser.cpp
	rename y.tab.h OW_MOFParser.h

OW_MOFLexer.cpp: $(MOFSRCDIR)\OW_MOFLexer.ll
	-@del OW_MOFLexer.cpp
	-@echo #define YY_NEVER_INTERACTIVE 1 > OW_MOFLexer.cpp
	flex -it -Cfe -Powmof $(MOFSRCDIR)\OW_MOFLexer.ll | sed "/unistd/d"	>> OW_MOFLexer.cpp

#	flex -i -Cfe -Powmof -oOW_MOFLexer.cpp $(MOFSRCDIR)\OW_MOFLexer.ll

