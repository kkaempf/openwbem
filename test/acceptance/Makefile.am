SUBDIRS = testfiles progs oop

check_SCRIPTS = acceptance.sh

EXTRA_DIST = acceptance.sh.fb

clean-local: 
	rm -rf stage

CLEANFILES = acceptance.sh
SUFFIXES = .fb

.fb: Makefile
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''sysconfdir''@!$(sysconfdir)!g' \
		-e 's!@''top_srcdir''@!$(top_srcdir)!g' \
		-e 's!@''sbindir''@!$(sbindir)!g' \
		-e 's!@''bindir''@!$(bindir)!g' \
		-e 's!@''libdir''@!$(libdir)!g' \
		-e 's!@''includedir''@!$(includedir)!g' \
		-e 's!@''pkglibdir''@!$(pkglibdir)!g' \
		-e 's!@''libexecdir''@!$(libexecdir)!g' \
		-e 's!@''localstatedir''@!$(localstatedir)!g' \
		-e 's!@''PACKAGE_PREFIX_D''@!$(PACKAGE_PREFIX_D)!g' \
		-e 's!@''PACKAGE_PREFIX''@!$(PACKAGE_PREFIX)!g' \
		-e 's!@''lib_ext''@!$(LIB_EXT)!g' \
		$< > $@-t
	@MV@	$@-t $@
	if [ "x$(STATIC_SERVICES)" != "x" ]; then \
		@SED@ \
			-e 's!OWSYMLINKS=1!OWSYMLINKS=0!g' \
			-e 's!STATIC_SERVICES=0!STATIC_SERVICES=1!g' \
			-e 's!# turn on symlinks!# disable symlinks: everything is static.!g' \
			 $@ > $@-t; \
		@MV@	$@-t $@; \
	fi

	chmod +x $@

INCLUDES = \
-I$(top_srcdir)/src \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/test/CppUnit \
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/client

TESTS = acceptance.sh
