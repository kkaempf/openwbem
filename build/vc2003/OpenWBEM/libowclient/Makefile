DEBUG = 1
WARNLEVEL = /W0
LIBNAME = libowclient

ZLIBBASE = "c:\zlib"
OPENSSLBASE = C:\openssl

CPP = cl
LINK = link

SRCDIR = ..\..\..\..\src
TOOLSDIR = ..\tools

INCS = \
/I$(BASEDIR) \
/I$(SRCDIR)\common \
/I$(SRCDIR)\common\socket \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\xml \
/I$(SRCDIR)\http\common \
/I$(SRCDIR)\http\client \
/I$(ZLIBBASE)\include \
/I$(OPENSSLBASE)\include


!IFDEF DEBUG
BUILDTYPE = Debug
CPPDEFS = /D"OW_CLIENT_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CPPFLAGS = /GF /GX /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
LINKDBG = /DEBUG
!ELSE
BUILDTYPE = Release
CPPDEFS = /D"OW_CLIENT_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CPPFLAGS = /GF /GX /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
LINKDBG = 
!ENDIF

BASEDIR = ..
TARGET = $(LIBNAME).dll
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)
OBJDIR = $(BUILDTYPE)

LIBPATHS = \
/LIBPATH:$(ZLIBBASE)\lib \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem.lib \
libowhttp.lib \
libowxml.lib \
zdll.lib

CLIENTSRCDIR = $(SRCDIR)\client

OBJS = \
	$(OBJDIR)\OW_CIMOMLocator.obj \
	$(OBJDIR)\OW_CIMOMInfo.obj \
	$(OBJDIR)\OW_CIMClient.obj \
	$(OBJDIR)\OW_CIMOMLocatorSLP.obj \
	$(OBJDIR)\OW_ClientCIMOMHandle.obj \
	$(OBJDIR)\OW_CIMXMLCIMOMHandle.obj \
	$(OBJDIR)\OW_BinaryCIMOMHandle.obj \
	$(OBJDIR)\OW_ClientCIMOMHandleConnectionPool.obj

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

CLIENTSRCDIR = $(SRCDIR)\client

$(OBJDIR)\OW_CIMOMLocator.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMOMInfo.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMClient.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMOMLocatorSLP.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ClientCIMOMHandle.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_CIMXMLCIMOMHandle.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp
	
$(OBJDIR)\OW_BinaryCIMOMHandle.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ClientCIMOMHandleConnectionPool.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

