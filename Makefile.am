SUBDIRS = db src etc test mof doc schemas

noinst_SCRIPTS = $(PACKAGE_PREFIX_D)openwbem.spec

EXTRA_DIST = \
Doxyfile.sh \
ow_ar.sh \
Makefile-shared-lib.inc \
LICENSE \
ow_ar.sh \
Makefile-shared-lib-inst.inc \
openwbem.spec \
fake_library_creator.sh \
generic-company-openwbem.spec.in

CLEANFILES = Doxyfile

clean-local:
	if [ -n "$(PACKAGE_PREFIX_D)" ]; then \
		rm -f $(PACKAGE_PREFIX_D)openwbem.spec; \
	fi

SUFFIXES = .sh 


.sh: Makefile
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''srcdir''@!$(srcdir)!g' \
	$< > $@-t
	@MV@	$@-t $@

$(PACKAGE_PREFIX_D)openwbem.spec: generic-company-openwbem.spec.in Makefile
	if [ -n "$(PACKAGE_PREFIX_D)" ]; then \
		@RM@ -f $@ $@-t; \
		@SED@ \
			-e 's!@''GENERIC_D''@!$(PACKAGE_PREFIX_D)!g' \
			-e 's!@''GENERIC''@!$(PACKAGE_PREFIX)!g' \
			-e 's!@''OWVERSION''@!$(OPENWBEM_VERSION)!g' \
		$< > $@-t; \
		@MV@    $@-t $@; \
	fi

docs: Doxyfile
	doxygen Doxyfile

# This will ensure the perl provider doesn't get build during make distcheck,
# since it tries to install itself into /usr/lib/blah, and ignores $prefix
# (which is correct, even if it doesn't seem so at first thought).
DISTCHECK_CONFIGURE_FLAGS= --with-perl=no

rpm: dist
	rpmbuild -ta $(distdir).tar.gz


