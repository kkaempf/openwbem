!include ..\common.mk

LIBNAME = librepositoryhdb$(BLDCHAR)
TARGET = $(LIBNAME).dll
CPPDEFS = $(CPPDEFS) /D"OW_REPHDB_BUILD"

INCS = \
/I$(BASEDIR) \
/I$(SRCDIR)\common \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\..\db\include

LIBPATHS = \
/LIBPATH:$(OPENSSLBASE)\lib\vc \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem$(BLDCHAR).lib \
libowdb$(BLDCHAR).lib

OBJS = \
	$(OBJDIR)\OW_AssocDb.obj \
	$(OBJDIR)\OW_CIMRepository.obj \
	$(OBJDIR)\OW_GenericHDBRepository.obj \
	$(OBJDIR)\OW_HDB.obj \
	$(OBJDIR)\OW_HDBException.obj \
	$(OBJDIR)\OW_HDBNode.obj \
	$(OBJDIR)\OW_Index.obj \
	$(OBJDIR)\OW_IndexImpl.obj \
	$(OBJDIR)\OW_InstanceRepository.obj \
	$(OBJDIR)\OW_MetaRepository.obj

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

HDBSRCDIR = $(SRCDIR)\repositories\hdb

$(OBJDIR)\OW_AssocDb.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMRepository.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_GenericHDBRepository.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HDB.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HDBException.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HDBNode.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_Index.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_IndexImpl.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_InstanceRepository.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_MetaRepository.obj: $(HDBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(HDBSRCDIR)\$(@B).cpp

