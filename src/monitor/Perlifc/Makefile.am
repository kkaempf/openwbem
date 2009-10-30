if ENABLE_MONITORED_PERL_IFC

all-local: perlxsbuild

clean-local:
	rm -rf perlxsbuild

mpdir = $(DESTDIR)$(libdir)/openwbem/monitoredPerl
lperl = $(DESTDIR)$(libexecdir)/openwbem/perl

install-exec-local:
	rm -rf $(mpdir)
	mkdir -p $(mpdir)
	cp -R perlxsbuild/blib/* $(mpdir)
	rm -rf $(mpdir)/man3
	chmod -R og-w $(mpdir)
	chmod -R og+rx $(mpdir)
	ln -sf $(PERL) $(lperl)

uninstall-local:
	rm -rf $(mpdir)
	rm -f $(lperl)

PERLXS_LIB_SOURCES = Privman.pm
PERLXS_TOP_SOURCES = Privman.xs ppport.h
PERLXSBUILD_SOURCES = $(PERLXS_LIB_SOURCES) $(PERLXS_TOP_SOURCES) Makefile.PL

EXTRA_DIST = $(PERLXSBUILD_SOURCES)

PMDIR = $(libdir)/openwbem


if DARWIN
if ARCH_PPC
# The perlxsbuild process on MacOS will barf if
# (${MACOSX_DEPLOYMENT_TARGET:-10.1} < 10.3).  Since Apple doesn't release
# patches for 10.2 anymore, (nor do they for 10.3), we're probably safe in
# assuming that people won't need the perlifc on any version of OSX earlier
# than 10.3.
#
# TODO: Find out what, if anything, needs to be done to make the perl modules work in MacOS x86 and x86_64.
export MACOSX_DEPLOYMENT_TARGET=10.3
endif # ARCH_PPC
endif DARWIN

perlxsbuild: $(PERLXSBUILD_SOURCES) Makefile
	$(RM) -rf $@
	mkdir $@
	for f in $(PERLXS_TOP_SOURCES); do cp $(srcdir)/$$f $@; done
	mkdir $@/lib
	for f in $(PERLXS_LIB_SOURCES); do cp $(srcdir)/$$f $@/lib; done
	cat $(srcdir)/Makefile.PL | \
		$(SED) -e 's|@rpath@|$(RPATH_LINK_FLAG_WITH_PATH)|g' \
		-e 's|@owcprivman_install@|$(PMDIR)|g' \
		-e 's|@owcprivman_build@|$(top_builddir)/src/monitor/Cifc|g' \
		-e 's|@monitor_source@|$(top_srcdir)/src/monitor|g' \
		-e 's|@monitor_build@|$(top_builddir)/src/monitor|g' \
		-e 's|@owconfigdir@|$(top_builddir)|g' \
		-e 's|@OWLDFLAGS@|$(LDFLAGS)|g' \
		-e 's|@OWCC@|$(CC)|g' \
		-e 's|@SSL_LIB_FLAGS@|$(SSL_LIBS)|g' \
		-e 's|@OWCPPFLAGS@|$(CPPFLAGS)|g' \
		> $@/Makefile.PL
	cd $@; $(PERL) Makefile.PL; $(MAKE)

endif
