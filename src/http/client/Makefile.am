include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowhttpclient.a

OW_SHARED_LIBS = libowhttpclient.so
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

libowhttpclient_a_SOURCES =	\
OW_HTTPClient.cpp 

pkginclude_HEADERS =	\
OW_HTTPClient.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common

