!include ..\common.mk

TOOLSRCDIR = $(SRCDIR)\tools

!IFDEF DEBUG
CPPDEFS = /D"WIN32" /D"_DEBUG" /D"_CONSOLE" /D"OW_WIN32" /D"_MBCS"
CPPFLAGS = /Od /RTC1 /MDd /Zc:forScope /GR /Fo"$@" /Fd"$(TARGETDIR)\$(@B).pdb" /W2 /Z7 /Wp64 /TP /c /GX
!ELSE
CPPDEFS = /D"WIN32" /D"_CONSOLE" /D"OW_WIN32" /D"_MBCS"
CPPFLAGS = /MD /Zc:forScope /GR /Fo"$@" /W$(WARNLEVEL) /Wp64 /TP /c /GX
!ENDIF

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
/I$(SRCDIR)\services\http \
/I$(SRCDIR)\repositories\hdb \
/I$(SRCDIR)\mof \
/I$(SRCDIR)\repositories\hdb \
/I$(OPENSSLBASE)\include

LIBPATH = \
/LIBPATH:$(OPENSSLBASE)\lib\vc \
/LIBPATH:$(ZLIBBASE)\lib \
/LIBPATH:$(TARGETDIR)

#REPDUMPEXE = $(TARGETDIR)\owrepositorydump.exe

ENUMCLSNAMES = owenumclassnames
ENUMCLSNAMESEXE = $(TARGETDIR)\$(ENUMCLSNAMES).exe
CIMLISTENER = owcimindicationlistener
CIMLISTENEREXE = $(TARGETDIR)\$(CIMLISTENER).exe
CREATENS = owcreatenamespace
CREATENSEXE = $(TARGETDIR)\$(CREATENS).exe
DELETENS = owdeletenamespace
DELETENSEXE = $(TARGETDIR)\$(DELETENS).exe
ENUMCLS = owenumclasses
ENUMCLSEXE = $(TARGETDIR)\$(ENUMCLS).exe
MOFC = owmofc
MOFCEXE = $(TARGETDIR)\$(MOFC).exe
EXECWQL = owexecwql
EXECWQLEXE = $(TARGETDIR)\$(EXECWQL).exe
ENUMNS = owenumnamespace
ENUMNSEXE = $(TARGETDIR)\$(ENUMNS).exe
ENUMQUALS = owenumqualifiers
ENUMQUALSEXE = $(TARGETDIR)\$(ENUMQUALS).exe

EXES = \
$(CREATENSEXE) \
$(DELETENSEXE) \
$(ENUMCLSEXE) \
$(CIMLISTENEREXE) \
$(MOFCEXE) \
$(EXECWQLEXE) \
$(ENUMCLSNAMESEXE) \
$(ENUMNSEXE) \
$(ENUMQUALSEXE)


LIBOWXML = libowxml$(BLDCHAR).lib
LIBOWCLIENT = libowclient$(BLDCHAR).lib
LIBOWHTTP = libowhttp$(BLDCHAR).lib
LIBOPENWBEM = libopenwbem$(BLDCHAR).lib
LIBOWMOFC = libowmofc$(BLDCHAR).lib
LIBOWREP = librepositoryhdb$(BLDCHAR).lib
LIBOWDB = libowdb$(BLDCHAR).lib
LIBOWLISTENER = libcimxmllistener$(BLDCHAR).lib


ALL: $(EXES)

#
# owenumqualifiers
#
$(ENUMQUALSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(LIBOWCLIENT) \
	$(LIBOWXML) $(LIBOWHTTP) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)

$(OBJDIR)\$(ENUMQUALS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owenumnamespace
#
$(ENUMNSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(LIBOWCLIENT) \
	$(LIBOWXML) $(LIBOWHTTP) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)

$(OBJDIR)\$(ENUMNS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owenumclassnames
#
$(ENUMCLSNAMESEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(LIBOWCLIENT) \
	$(LIBOWXML) $(LIBOWHTTP) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)

$(OBJDIR)\$(ENUMCLSNAMES).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owexecwql
#
$(EXECWQLEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(LIBOWCLIENT) \
	$(LIBOWXML) $(LIBOWHTTP) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)
	
$(OBJDIR)\$(EXECWQL).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owmofc
#
$(MOFCEXE): $(OBJDIR)\$(@B).obj $(OBJDIR)\OW_MOFCompCIMOMHandle.obj $(TARGETDIR)
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(OBJDIR)\OW_MOFCompCIMOMHandle.obj \
	$(LIBOWMOFC) $(LIBOWCLIENT) $(LIBOWXML) $(LIBOWHTTP) \
	$(LIBOWREP) $(LIBOWDB) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)
	
$(OBJDIR)\$(MOFC).obj: $(TOOLSRCDIR)\owmofc\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\owmofc\$(@B).cpp

$(OBJDIR)\OW_MOFCompCIMOMHandle.obj: $(TOOLSRCDIR)\owmofc\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\owmofc\$(@B).cpp

#
# owcimindicationlistener
#
$(CIMLISTENEREXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj \
	$(LIBOWHTTP) $(LIBOWCLIENT) $(LIBOWXML) $(LIBOPENWBEM) \
	$(LIBOWLISTENER) $(ZLIB) $(SSLLIBS)
	
$(OBJDIR)\$(CIMLISTENER).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owcreatenamespace
#
$(CREATENSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(LIBOWCLIENT) \
	$(LIBOWXML) $(LIBOWHTTP) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)
	
$(OBJDIR)\$(CREATENS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owdeletenamespace
#
$(DELETENSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(LIBOWCLIENT) \
	$(LIBOWXML) $(LIBOWHTTP) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)

$(OBJDIR)\$(DELETENS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

#
# owenumclasses
#
$(ENUMCLSEXE): $(OBJDIR)\$(@B).obj $(TARGETDIR) 
	link /OUT:$@ /nologo $(LIBPATH) $(LINKDBG) \
	/PDB:"$(TARGETDIR)\$(@B).pdb" /MAP /SUBSYSTEM:CONSOLE \
	/MACHINE:X86 $(OBJDIR)\$(@B).obj $(LIBOWCLIENT) \
	$(LIBOWXML) $(LIBOWHTTP) $(LIBOPENWBEM) $(ZLIB) $(SSLLIBS)

$(OBJDIR)\$(ENUMCLS).obj: $(TOOLSRCDIR)\$(@B).cpp $(OBJDIR)
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(TOOLSRCDIR)\$(@B).cpp

clean:
	-@del /Q $(OBJDIR)\*.*
	-@del /Q $(TARGETDIR)\$(ENUMCLSNAMES).*
	-@del /Q $(TARGETDIR)\$(CIMLISTENER).*
	-@del /Q $(TARGETDIR)\$(CREATENS).*
	-@del /Q $(TARGETDIR)\$(DELETENS).*
	-@del /Q $(TARGETDIR)\$(ENUMCLS).*
	-@del /Q $(TARGETDIR)\$(MOFC).*
	-@del /Q $(TARGETDIR)\$(EXECWQL).*
	-@del /Q $(TARGETDIR)\$(ENUMNS).*
	-@del /Q $(TARGETDIR)\$(ENUMQUALS).*

$(TARGETDIR):
	IF NOT EXIST $(TARGETDIR) (md $(TARGETDIR))

$(OBJDIR):
	IF NOT EXIST $(OBJDIR) (md $(OBJDIR))

