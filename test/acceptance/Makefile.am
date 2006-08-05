SUBDIRS = testfiles progs oop

check_SCRIPTS = acceptance.sh

EXTRA_DIST = acceptance.sh.fb

clean-local:
	@echo "Cleanup Warning: There may be another stage directory: /ow_test"
	rm -rf stage

CLEANFILES = acceptance.sh
SUFFIXES = .fb

# Note that the top_srcdir and top_builddir are defined here instead as a
# replacement.  This is because the replacement will replace them as nothing.
%.in: %.fb Makefile
	rm -f $@
	cp $< $@
	if [ "x$(STATIC_SERVICES)" != "x" ]; then \
		@SED@ \
			-e 's!OWSYMLINKS=1!OWSYMLINKS=0!g' \
			-e 's!STATIC_SERVICES=0!STATIC_SERVICES=1!g' \
			-e 's!# turn on symlinks!# disable symlinks: everything is static.!g' \
			 $@ > $@-t; \
		@MV@	$@-t $@; \
	fi

include ${top_srcdir}/Makefile-replacement.inc

acceptance.sh_extra_substitutions = \
	-e 's!@''absolute_srcdir''@!$(shell pwd)/$(top_srcdir)!g' \
	-e 's!@''absolute_builddir''@!$(shell pwd)/$(top_builddir)!g' \
	-e 's!@''pkglibdir''@!$(pkglibdir)!g' \
	-e 's!@''lib_ext''@!$(LIB_EXT)!g'

all-local: $(check_SCRIPTS)
	chmod +x $^

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

noinst_PROGRAMS = check_security
check_security_SOURCES = check_security.cpp
check_security_LDFLAGS = -o $@ -L$(top_builddir)/src/common -lopenwbem
