include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowserver.a
SHAREDLIBADD = \
../provider/libowprovider.a

OW_SHARED_LIBS = libowserver.so
thelibdir = $(libdir)

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -s $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs

libowserver_a_SOURCES =	\
OW_AuthManager.cpp \
OW_AuthManager.hpp \
OW_ConfigException.hpp \
OW_ConfigException.cpp \
OW_Logger.cpp \
OW_PidFile.cpp \
OW_PidFile.hpp \
OW_Platform.cpp \
OW_Platform.hpp

