!include ..\common.mk

LIBNAME = libowmofc$(BLDCHAR)
TARGET = $(LIBNAME).dll
CPPDEFS = $(CPPDEFS) /D"OW_MOF_BUILD"

INCS = \
/I$(BASEDIR) \
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
libopenwbem$(BLDCHAR).lib 

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
	-@del /Q OW_MOFParser.cpp
	-@del /Q OW_MOFParser.h
	-@del /Q OW_MOFLexer.cpp
	-@del /Q $(OBJDIR)\*.*
	-@del /Q $(TARGETDIR)\$(LIBNAME).*

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))

$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))

###############################################################################

MOFSRCDIR = $(SRCDIR)\mof

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

