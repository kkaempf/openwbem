!include ..\common.mk

LIBNAME = libowhttp$(BLDCHAR)
TARGET = $(LIBNAME).dll
CPPDEFS = $(CPPDEFS) /D"OW_HTTP_BUILD"

INCS = \
/I$(BASEDIR) \
/I$(SRCDIR)\common \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\common\socket \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\client \
/I$(SRCDIR)\http\common \
/I$(OPENSSLBASE)\include \
/I$(ZLIBBASE)\include

LIBPATHS = \
/LIBPATH:$(OPENSSLBASE)\lib\vc \
/LIBPATH:$(ZLIBBASE)\lib \
/LIBPATH:$(TARGETDIR)

LIBS = \
ws2_32.lib \
advapi32.lib \
libeay32.lib \
ssleay32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
zdll.lib \
libopenwbem$(BLDCHAR).lib 

COMMONOBJS = \
	$(OBJDIR)\OW_HTTPChunkedIStream.obj \
	$(OBJDIR)\OW_HTTPChunkedOStream.obj \
	$(OBJDIR)\OW_HTTPChunkException.obj \
	$(OBJDIR)\OW_HTTPCounter.obj \
	$(OBJDIR)\OW_HTTPDeflateIStream.obj \
	$(OBJDIR)\OW_HTTPDeflateOStream.obj \
	$(OBJDIR)\OW_HTTPException.obj \
	$(OBJDIR)\OW_HTTPLenLimitIStream.obj \
	$(OBJDIR)\OW_HTTPUtils.obj

CLIENTOBJS = \
	$(OBJDIR)\OW_HTTPClient.obj

SERVICEOBJS = \
	$(OBJDIR)\OW_HTTPServer.obj \
	$(OBJDIR)\OW_HTTPSvrConnection.obj

OBJS = $(COMMONOBJS) $(CLIENTOBJS) $(SERVICEOBJS)

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

COMMONSRCDIR = $(SRCDIR)\http\common
CLIENTSRCDIR = $(SRCDIR)\http\client
SERVICESRCDIR = $(SRCDIR)\services\http


$(OBJDIR)\OW_HTTPChunkedIStream.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPChunkedOStream.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPChunkException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPCounter.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPDeflateIStream.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPDeflateOStream.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPException.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPLenLimitIStream.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPUtils.obj: $(COMMONSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(COMMONSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPClient.obj: $(CLIENTSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(CLIENTSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPServer.obj: $(SERVICESRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SERVICESRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPSvrConnection.obj: $(SERVICESRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(SERVICESRCDIR)\$(@B).cpp

