include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libowrequesthandlerbinary.so

noinst_LIBRARIES = \
libowrequesthandlerbinary.a

thelibdir = $(pkglibdir)/service
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

libowrequesthandlerbinary_a_SOURCES = \
OW_BinaryRequestHandler.cpp \
OW_BinaryRequestHandler.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/cim

