man8_MAN = owcimomd.8 owdigestgenpass.8
man8dir = $(mandir)/man8

EXTRA_DIST = $(man8_MAN)

CLEANFILES = owcimomd.8 

SUFFIXES = .fb

.fb:
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''sysconfdir''@!$(sysconfdir)!g' \
	$< > $@-t
	@MV@	$@-t $@

