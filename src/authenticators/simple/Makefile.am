include $(top_srcdir)/Makefile-shared-lib.inc
include $(top_srcdir)/Makefile-shared-lib-inst.inc

OW_SHARED_LIBS = libsimpleauthentication.so
SHAREDLIBADD = 

noinst_LIBRARIES = \
	libsimpleauthentication.a 

thelibdir = $(pkglibdir)/authentication

libsimpleauthentication_a_SOURCES = \
OW_SimpleAuthenticator.cpp 

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/ifcs

