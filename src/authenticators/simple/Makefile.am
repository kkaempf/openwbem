include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libsimpleauthentication.so

noinst_LIBRARIES = \
	libsimpleauthentication.a 

thelibdir = $(pkglibdir)/auth_mods
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

libsimpleauthentication_a_SOURCES = \
OW_SimpleAuthenticator.cpp 

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/thread -I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/xml		\
-I$(top_srcdir)/src/provider	\
-I$(top_srcdir)/src/hdb		\
-I$(top_srcdir)/src/http	\
-I$(top_srcdir)/src/server	\
-I$(top_srcdir)/src/cim -I$(top_srcdir)/src/net/socket		\
-I$(top_srcdir)/src/cimom -I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/cimom -I$(top_srcdir)/src/server

