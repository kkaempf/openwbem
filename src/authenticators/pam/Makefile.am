if USE_PAM
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libpamauthentication.so
SHAREDLIBADD = -lpam

noinst_LIBRARIES = \
	libpamauthentication.a 

thelibdir = $(pkglibdir)/authentication

libpamauthentication_a_SOURCES = \
OW_LinuxPAMAuthentication.cpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/ifcs

else
clean-local:

endif
