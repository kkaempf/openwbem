DEBUG = 1
WARNLEVEL = /W0
LIBNAME = librepositoryhdb

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src

INCS = \
/I".." \
/I$(SRCDIR)\common \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\..\db\include

!IFDEF DEBUG
BUILDTYPE = Debug
CPPDEFS = /D"OW_REPHDB_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
LINKDBG = /DEBUG
!ELSE
BUILDTYPE = Release
CPPDEFS = /D"OW_REPHDB_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
LINKDBG = 
!ENDIF

BASEDIR = ..
TARGET = $(LIBNAME).dll
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)
OBJDIR = $(BUILDTYPE)

LIBPATHS = \
/LIBPATH:$(OPENSSLBASE)\lib\vc \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem.lib \
libowdb.lib

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
	-@del $(OBJDIR)\*.obj
	-@del $(OBJDIR)\*.map
	-@del $(OBJDIR)\*.def
	-@del $(OBJDIR)\*.dll

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

