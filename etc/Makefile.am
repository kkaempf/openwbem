SUBDIRS = init sysconfig pam.d

openwbemconf_DATA = openwbem.conf
openwbemconfdir = $(sysconfdir)/openwbem

EXTRA_DIST = openwbem.conf.sh

CLEANFILES = openwbem.conf

SUFFIXES = .sh

.sh: Makefile
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''libdir''@!$(libdir)!g' \
		-e 's!@''libexecdir''@!$(libexecdir)!g' \
		-e 's!@''sysconfdir''@!$(sysconfdir)!g' \
		-e 's!@''localstatedir''@!$(localstatedir)!g' \
	$< > $@-t
	@MV@	$@-t $@

