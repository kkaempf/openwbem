man8_MANS = owcimomd.8 owdigestgenpass.8
man8dir = $(mandir)/man8

EXTRA_DIST = $(man8_MANS) owcimomd.8.fb

BUILT_SOURCES = owcimomd.8

SUFFIXES = .fb

.fb:
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''sysconfdir''@!$(sysconfdir)!g' \
	$< > $@-t
	@MV@	$@-t $@

