include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libowserviceipc.so
SHAREDLIBADD =

noinst_LIBRARIES = \
libowserviceipc.a

thelibdir = $(pkglibdir)/services
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

libowserviceipc_a_SOURCES = \
OW_DomainSocketServer.cpp \
OW_DomainSocketServer.hpp \
OW_IPCService.cpp \
OW_IPCService.hpp \
OW_IPCServer.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

