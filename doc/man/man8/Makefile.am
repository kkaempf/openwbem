man8_DATA = owcimomd.8 owdigestgenpass.8
man8dir = $(mandir)/man8

EXTRA_DIST = owcimomd.8.fb owdigestgenpass.8

CLEANFILES = owcimomd.8 

SUFFIXES = .fb

.fb:
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''sysconfdir''@!$(sysconfdir)!g' \
	$< > $@-t
	@MV@	$@-t $@

