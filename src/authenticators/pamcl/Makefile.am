if USE_PAM
include $(top_srcdir)/Makefile-shared-lib.inc

owpamauthbin_PROGRAMS = OW_PAMAuth

OW_PAMAuth_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o OW_PAMAuth

owpamauthbindir = $(libexecdir)/openwbem
OW_PAMAuth_SOURCES = OW_PAMAuthSA.cpp
OW_PAMAuth_LDADD = -lpam

OW_SHARED_LIBS = libpamclauthentication.so
SHAREDLIBADD = -lpam

noinst_LIBRARIES = \
	libpamclauthentication.a 

thelibdir = $(pkglibdir)/authentication
all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done

libpamclauthentication_a_SOURCES = \
OW_LinuxPAMAuthenticationCL.cpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/ifcs

else
clean-local:

endif

