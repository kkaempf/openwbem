noinst_DATA = owcimomd
#openwbemdir = $(sysconfdir)/rc.d/init.d

EXTRA_DIST = owcimomd.sh

CLEANFILES = owcimomd

SUFFIXES = .sh

.sh:
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''sbindir''@!$(sbindir)!g' \
	$< > $@-t
	@MV@	$@-t $@

