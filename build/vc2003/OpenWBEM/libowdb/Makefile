DEBUG = 1
WARNLEVEL = /W0
BASEDIR = ..

BUILDTYPE = Debug
LIBNAME = libowdb

TARGET = $(LIBNAME).dll
BASEDIR = ..
TARGETDIR = $(BASEDIR)\$(BUILDTYPE)

CC = cl
LINK = link

SRCDIR = ..\..\..\..\src
TOOLSDIR = ..\tools

INCS = \
/I".." \
/I$(SRCDIR)\..\db\include \
/I$(SRCDIR)\common 

LIBPATHS = \
/LIBPATH:$(TARGETDIR)

LIBS = \
advapi32.lib \
kernel32.lib \
user32.lib \
uuid.lib \
libopenwbem.lib 

!IFDEF DEBUG
CCDEFS = /D"OW_DB_BUILD" /D"_DEBUG" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS" 
CCFLAGS = /GF /GX /RTC1 /Zi /Zc:forScope /GR $(WARNLEVEL) /nologo /Wp64 /D"_DEBUG" /MDd /c /Fo"$@"
OBJDIR = Debug
LINKDBG = /DEBUG
!ELSE
CCDEFS = /D"OW_DB_BUILD" /D"WIN32" /D"_WINDOWS" /D"_USRDLL" /D"_LIB" /D"_WINDLL" /D"_MBCS"
CCFLAGS = /GF /GX /Zc:forScope /GR $(WARNLEVL) /nologo /Wp64 /MD /c
OBJDIR = Release
LINKDBG = 
!ENDIF

OBJS = \
	$(OBJDIR)\bt_close.obj \
	$(OBJDIR)\bt_conv.obj \
	$(OBJDIR)\bt_debug.obj \
	$(OBJDIR)\bt_delete.obj \
	$(OBJDIR)\bt_get.obj \
	$(OBJDIR)\bt_open.obj \
	$(OBJDIR)\bt_overflow.obj \
	$(OBJDIR)\bt_page.obj \
	$(OBJDIR)\bt_put.obj \
	$(OBJDIR)\bt_search.obj \
	$(OBJDIR)\bt_seq.obj \
	$(OBJDIR)\bt_split.obj \
	$(OBJDIR)\bt_utils.obj \
	$(OBJDIR)\db.obj \
	$(OBJDIR)\mpool.obj

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

BTSRCDIR = $(SRCDIR)\..\db\btree
DBSRCDIR = $(SRCDIR)\..\db\db
MPOOLSRCDIR = $(SRCDIR)\..\db\mpool

$(OBJDIR)\bt_close.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_conv.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_debug.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_delete.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_get.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_open.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_overflow.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_page.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_put.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_search.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_seq.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c
	
$(OBJDIR)\bt_split.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\bt_utils.obj: $(BTSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(BTSRCDIR)\$(@B).c

$(OBJDIR)\db.obj: $(DBSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(DBSRCDIR)\$(@B).c
	
$(OBJDIR)\mpool.obj: $(MPOOLSRCDIR)\$(@B).c
	$(CC) $(CCDEFS) $(INCS) $(CCFLAGS) $(MPOOLSRCDIR)\$(@B).c

