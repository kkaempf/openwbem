dist-hook:
	mkdir $(distdir)/perlNPI
	cp $(srcdir)/perlNPI/{Changes,MANIFEST,Makefile.PL,README.1ST,perlNPI.pm,perlNPI.xs,test.pl,typemap} $(distdir)/perlNPI


if HAVE_PERL_EMBED
#include $(top_srcdir)/Makefile-shared-lib.inc

# contents of Makefile-shared-lib.inc slightly modified for this special dir.
SO_EXT = $(OPENWBEM_VERSION)
SOFILENAME = $@.$(SO_EXT)
TMPLIBDIR = .$@.libs


#.a.so:
#$(LIBNAME).so : $(LIBNAME).a

%.so : %.a
	rm -rf $(TMPLIBDIR)
	mkdir $(TMPLIBDIR)
	cd $(TMPLIBDIR) && \
	$(AR) x ../$< 
	$(CXXLINK) $(CXXSHAREDLIBLINKFLAGS) -o $(SOFILENAME) $(TMPLIBDIR)/*.o $(SHAREDLIBADD)
	rm -rf $@ $(TMPLIBDIR)
	ln -s $(SOFILENAME) $@
	chmod +x $(SOFILENAME)

noinst_LIBRARIES = libperlProvider.a
OW_SHARED_LIBS = libperlProvider.$(LIB_EXT)
SHAREDLIBADD = $(perl_embed_ldflags)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(pkglibdir)/perlproviders

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core $(top_srcdir)/src/providerifcs/perl/perlProvider/perlxsi.c
	-rm -f perlNPI.marker

all-local: perlNPI.marker $(OW_SHARED_LIBS)

perlNPI.marker:
	touch perlNPI.marker
	if test -e $(top_builddir)/src/providerifcs/perl/perlProvider/perlNPI; then true; else cp -a $(top_srcdir)/src/providerifcs/perl/perlProvider/perlNPI $(top_builddir)/src/providerifcs/perl/perlProvider; fi
	chmod -R +w perlNPI
	if test -e perlNPI/Makefile; then $(MAKE) -C perlNPI clean;fi
	cd perlNPI;$(PERL) Makefile.PL;cd ..
# need to fix up the makefile to honor $(DESTDIR)
#	echo "look at the makefile!"
#	false
# hmm, it seems to be okay with the perl on Fedora Core 1
	$(MAKE) -e TOP_SRCDIR=$(top_srcdir)/../ -C perlNPI

perlxsi.c: Makefile
	$(PERL) -MExtUtils::Embed -e xsinit -- -o $(top_srcdir)/src/providerifcs/perl/perlProvider/perlxsi.c

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		PREFIX=$(DESTDIR); \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	$(MAKE) -e TOP_SRCDIR=$(top_srcdir)/../ -C perlNPI install_vendor

uninstall-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -f $(DESTDIR)$(thelibdir)/$$i.$(SO_EXT); \
		rm -f $(DESTDIR)$(thelibdir)/$$i; \
	done
	$(MAKE) -e TOP_SRCDIR=$(top_srcdir)/../ -C perlNPI uninstall

clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done
	-$(MAKE) -e TOP_SRCDIR=$(top_srcdir)/../ -C perlNPI clean
	if [ "$(top_srcdir)" != "$(top_builddir)" ]; then rm -rf perlNPI perlNPI.marker; fi

distclean-local:
	-$(MAKE) -e TOP_SRCDIR=$(top_srcdir)/../ -C perlNPI distclean

libperlProvider_a_SOURCES = \
perlProvider.c \
perlProvider.h \
perlargs.h

#nodist_libperlProvider_a_SOURCES = \
#perlxsi.c

CLEANFILES = \
perlxsi.c

INCLUDES = -I$(top_srcdir)/src/providerifcs/perl \
-I$(top_srcdir)/src/providerifcs/npi/common \
-I$(top_srcdir)/src/providerifcs/npi \
-I$(perl_embed_ccflags)

endif

EXTRA_DIST = \
perlProvider.c \
perlProvider.h \
perlargs.h

