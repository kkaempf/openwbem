include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libsimpleauthentication.so
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem

noinst_LIBRARIES = \
	libsimpleauthentication.a 

thelibdir = $(pkglibdir)/authentication

libsimpleauthentication_a_SOURCES = \
OW_SimpleAuthenticator.cpp 

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/ifcs

