if USE_PAM
include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libpamauthentication.so
SHAREDLIBADD = -lpam

noinst_LIBRARIES = \
	libpamauthentication.a 

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

libpamauthentication_a_SOURCES = \
OW_LinuxPAMAuthentication.cpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/ifcs

else
clean-local:

endif
