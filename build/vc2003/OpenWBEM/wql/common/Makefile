!include ..\common.mk

LIBNAME = libowwqlcommon$(BLDCHAR)
CPPDEFS = $(CPPDEFS) /D"OW_WQLCOMMON_BUILD"
TARGET = $(LIBNAME).dll

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
libopenwbem$(BLDCHAR).lib 

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
	-@del /Q $(OBJDIR)\*.*
	-@del /Q $(TARGETDIR)\$(LIBNAME).*

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

