SUBDIRS = init sysconfig pam.d

openwbemconf_DATA = openwbem.conf ssleay.cnf owgencert
openwbemconfdir = $(sysconfdir)/openwbem

EXTRA_DIST = openwbem.conf.sh owgencert.sh ssleay.cnf

CLEANFILES = openwbem.conf owgencert

SUFFIXES = .sh

.sh: Makefile
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''libdir''@!$(libdir)!g' \
		-e 's!@''libexecdir''@!$(libexecdir)!g' \
		-e 's!@''sysconfdir''@!$(sysconfdir)!g' \
		-e 's!@''localstatedir''@!$(localstatedir)!g' \
		-e 's!@''lib_ext''@!$(LIB_EXT)!g' \
		-e 's!@''PACKAGE_PREFIX''@!$(PACKAGE_PREFIX)!g' \
	$< > $@-t
	@MV@	$@-t $@

