!include ..\common.mk

LIBNAME = libcimxmllistener$(BLDCHAR)
TARGET = $(LIBNAME).dll
CPPDEFS = $(CPPDEFS) /D"OW_LISTENER_BUILD"

INCS = \
/I".." \
/I$(SRCDIR)\common \
/I$(SRCDIR)\common\socket \
/I$(SRCDIR)\cim \
/I$(SRCDIR)\ifcs \
/I$(SRCDIR)\http\common \
/I$(SRCDIR)\http\client \
/I$(SRCDIR)\services\http \
/I$(SRCDIR)\client \
/I$(SRCDIR)\xml \
/I$(SRCDIR)\requesthandlers\cimxml \
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
libopenwbem$(BLDCHAR).lib \
libowhttp$(BLDCHAR).lib \
libowclient$(BLDCHAR).lib \
libowxml$(BLDCHAR).lib 

OBJS = \
	$(OBJDIR)\OW_CIMListenerCallback.obj \
	$(OBJDIR)\OW_XMLListener.obj \
	$(OBJDIR)\OW_ListenerAuthenticator.obj \
	$(OBJDIR)\OW_HTTPXMLCIMListener.obj

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

LIBSRCDIR = $(SRCDIR)\cimxmllistener

$(OBJDIR)\OW_CIMListenerCallback.obj: $(LIBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_XMLListener.obj: $(LIBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_ListenerAuthenticator.obj: $(LIBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\$(@B).cpp

$(OBJDIR)\OW_HTTPXMLCIMListener.obj: $(LIBSRCDIR)\$(@B).cpp
	$(CPP) $(CPPDEFS) $(INCS) $(CPPFLAGS) $(LIBSRCDIR)\$(@B).cpp

