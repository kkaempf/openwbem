!include ..\common.mk

LIBNAME = libowwql$(BLDCHAR)
TARGET = $(LIBNAME).dll
CPPDEFS = $(CPPDEFS) /D"OW_WQL_BUILD"

WQLSRCDIR = $(SRCDIR)\wql

INCS = \
/I$(BASEDIR) \
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
libopenwbem$(BLDCHAR).lib \
libowwqlcommon$(BLDCHAR).lib

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
	-@del /Q OW_WQLParser.cpp
	-@del /Q OW_WQLParser.h
	-@del /Q OW_WQLLexer.cpp
	-@del /Q $(OBJDIR)\*.*
	-@del /Q $(TARGETDIR)\$(LIBNAME).*

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
	-@echo #include "OW_config.h" > OW_WQLParser.cpp
	-@echo #ifdef DELETE >> OW_WQLParser.cpp
	-@echo #undef DELETE >> OW_WQLParser.cpp
	-@echo #endif >> OW_WQLParser.cpp
	-@echo #ifdef IN >> OW_WQLParser.cpp
	-@echo #undef IN >> OW_WQLParser.cpp
	-@echo #endif >> OW_WQLParser.cpp
	bison -y -d -powwql $(WQLSRCDIR)\OW_WQLParser.yy
	-@echo #include "OW_config.h" > OW_WQLParser.h
	-@echo #ifdef DELETE >> OW_WQLParser.h
	-@echo #undef DELETE >> OW_WQLParser.h
	-@echo #endif >> OW_WQLParser.h
	-@echo #ifdef IN >> OW_WQLParser.h
	-@echo #undef IN >> OW_WQLParser.h
	-@echo #endif >> OW_WQLParser.h
	cat y.tab.h >> OW_WQLParser.h
	-@del y.tab.h
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

