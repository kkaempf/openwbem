noinst_DATA = $(PACKAGE_PREFIX)owcimomd
#openwbemdir = $(sysconfdir)/rc.d/init.d

EXTRA_DIST = owcimomd.sh

CLEANFILES = $(PACKAGE_PREFIX)owcimomd

SUFFIXES = .sh

.sh:
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''sbindir''@!$(sbindir)!g' \
		-e 's!@''PACKAGE_PREFIX''@!$(PACKAGE_PREFIX)!g' \
	$< > $@-t
	@MV@	$@-t $@

