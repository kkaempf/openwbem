if HAVE_PERL_EMBED
include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libperlProvider.a
OW_SHARED_LIBS = libperlProvider.so
SHAREDLIBADD = $(perl_embed_ldflags)

thelibdir = $(pkglibdir)/perlproviders

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core $(top_srcdir)/src/providerifcs/perl/perlProvider/perlxsi.c
	-rm -f perlNPI.marker

all-local: perlNPI.marker $(OW_SHARED_LIBS)

perlNPI.marker:
	touch perlNPI.marker
	if test -e perlNPI; then true; else cp -a $(top_srcdir)/src/providerifcs/perl/perlProvider/perlNPI $(top_builddir)/src/providerifcs/perl/perlProvider; fi
	if test -e perlNPI/Makefile; then make -C perlNPI clean;fi
	cd perlNPI;$(PERL) Makefile.PL;cd ..
	make -e TOP_SRCDIR=$(top_srcdir)/../ -C perlNPI

perlxsi.c:
	$(PERL) -MExtUtils::Embed -e xsinit -- -o $(top_srcdir)/src/providerifcs/perl/perlProvider/perlxsi.c

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		PREFIX=$(DESTDIR); \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	cd perlNPI;$(PERL) Makefile.PL PREFIX=$(DESTDIR)/$(prefix) INSTALLDIRS=vendor;cd ..
	make -e TOP_SRCDIR=$(top_srcdir)/../ -C perlNPI install

clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done

libperlProvider_a_SOURCES = \
perlProvider.c \
perlProvider.h \
perlxsi.c \
perlargs.h

INCLUDES = -I$(top_srcdir)/src/providerifcs/perl \
-I$(top_srcdir)/src/providerifcs/npi/common \
-I$(top_srcdir)/src/providerifcs/npi \
-I$(perl_embed_ccflags)
endif
