!include ..\common.mk

LIBNAME = libowclient$(BLDCHAR)
TARGET = $(LIBNAME).dll
CPPDEFS = $(CPPDEFS) /D"OW_CLIENT_BUILD"

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


LIBPATHS = \
/LIBPATH:$(ZLIBBASE)\lib \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem$(BLDCHAR).lib \
libowhttp$(BLDCHAR).lib \
libowxml$(BLDCHAR).lib \
zdll.lib

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
	-@del /Q $(OBJDIR)\*.*
	-@del /Q $(TARGETDIR)\$(LIBNAME).*

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

