noinst_DATA = $(PACKAGE_PREFIX_D)owcimomd
#openwbemdir = $(sysconfdir)/rc.d/init.d

EXTRA_DIST = owcimomd.sh

CLEANFILES = $(PACKAGE_PREFIX_D)owcimomd

$(PACKAGE_PREFIX_D)owcimomd: owcimomd.sh
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''sbindir''@!$(sbindir)!g' \
		-e 's!@''PACKAGE_PREFIX''@!$(PACKAGE_PREFIX)!g' \
	$< > $@-t
	@MV@	$@-t $@

