DEBUG = 1
WARNLEVEL = /W0
OPENSSLBASE = c:\openssl
ZLIBBASE = c:\zlib

BASEDIR = ..

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src
TOOLSRCDIR = $(SRCDIR)\tools

!IFDEF DEBUG
BUILDTYPE = Debug
LINKDBG = /DEBUG
CPPDEFS = /D"WIN32" /D"_DEBUG" /D"_CONSOLE" /D"OW_WIN32" /D"_MBCS"
#CPPDEFS = /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
#CPPFLAGS = /GF /GX /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /MDd /c /Fo"$@"
CPPFLAGS = /Od /Gm /RTC1 /MDd /Zc:forScope /GR /Fo"$@" /Fd"$(TARGETDIR)\$(@B).pdb" /W2 /Zi /Wp64 /TP /c /GX
!ELSE
BUILDTYPE = Release
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)
CPPDEFS = /D"WIN32" /D"_CONSOLE" /D"OW_WIN32" /D"_MBCS"
#CPPDEFS = /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
#CPPFLAGS = /GF /GX /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
CPPFLAGS = /MD /Zc:forScope /GR /Fo"$@" /W2 /Wp64 /TP /c /GX
LINKDBG = 
!ENDIF

OBJDIR = $(BUILDTYPE)
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)


INCS = \
/I".." \
/I$(SRCDIR)\common \
/I$(SRCDIR)\common\socket \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\http\common \
/I$(SRCDIR)\http\client \
/I$(SRCDIR)\xml \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\client \
/I$(SRCDIR)\cimxmllistener \
/I$(SRCDIR)\repositories\hdb \
/I$(OPENSSLBASE)\include

LIBPATH = \
/LIBPATH:$(OPENSSLBASE)\lib\vc \
/LIBPATH:$(ZLIBBASE)\lib \
/LIBPATH:$(TARGETDIR)

#ENUMCLSNAMESEXE = $(TARGETDIR)\owenumclassnames.exe
#ENUMNSEXE = $(TARGETDIR)\owenumnamespace.exe
#ENUMQUALSEXE = $(TARGETDIR)\owenumqualifiers.exe
#EXECWQLEXE = $(TARGETDIR)\owexecwql.exe
#REPDUMPEXE = $(TARGETDIR)\owrepositorydump.exe

CIMLISTENER = owcimindicationlistener
CIMLISTENEREXE = $(TARGETDIR)\$(CIMLISTENER).exe
CREATENS = owcreatenamespace
CREATENSEXE = $(TARGETDIR)\$(CREATENS).exe
DELETENS = owdeletenamespace
DELETENSEXE = $(TARGETDIR)\$(DELETENS).exe
ENUMCLS = owenumclasses
ENUMCLSEXE = $(TARGETDIR)\$(ENUMCLS).exe


EXES = \
$(CREATENSEXE) \
$(DELETENSEXE) \
$(ENUMCLSEXE) \
$(CIMLISTENEREXE)

ALL: $(EXES)

#
# owcimindicationlistener
#
$(CIMLISTENEREXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) /DEBUG \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj \
	libowhttp.lib libowclient.lib libowxml.lib libopenwbem.lib \
	libcimxmllistener.lib zdll.lib libeay32.lib ssleay32.lib
	
$(OBJDIR)\$(CIMLISTENER).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owcreatenamespace
#
$(CREATENSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) /DEBUG \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj libowclient.lib \
	libowxml.lib libowhttp.lib libopenwbem.lib zdll.lib libeay32.lib \
	ssleay32.lib
	
$(OBJDIR)\$(CREATENS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owdeletenamespace
#
$(DELETENSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) /DEBUG \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj libowclient.lib \
	libowxml.lib libowhttp.lib libopenwbem.lib zdll.lib libeay32.lib \
	ssleay32.lib

$(OBJDIR)\$(DELETENS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owenumclasses
#
$(ENUMCLSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) /DEBUG \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj libowclient.lib \
	libowxml.lib libowhttp.lib libopenwbem.lib zdll.lib libeay32.lib \
	ssleay32.lib

$(OBJDIR)\$(ENUMCLS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

clean:
	-@del $(OBJDIR)\*.obj
	-@del $(EXES)

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))

$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))

