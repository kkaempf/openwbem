SUBDIRS = init sysconfig pam.d

openwbemconf_DATA = openwbem.conf ssleay.cnf owgencert
openwbemconfdir = $(sysconfdir)/openwbem

EXTRA_DIST = openwbem.conf.sh owgencert.sh ssleay.cnf

CLEANFILES = openwbem.conf owgencert

SUFFIXES = .sh

.sh : $(top_builddir)/replacement_script.sed Makefile
	rm -f $@ $@.partial
	@echo "Creating $@ from $<";
	sed -f $(top_builddir)/replacement_script.sed < $< > $@.partial
	mv $@.partial $@

install-exec-hook:
	install -d $(DESTDIR)/$(openwbemconfdir)/openwbem.conf.d

