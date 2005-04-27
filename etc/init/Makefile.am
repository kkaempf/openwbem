noinst_DATA = $(PACKAGE_PREFIX_D)owcimomd
#openwbemdir = $(sysconfdir)/rc.d/init.d

EXTRA_DIST = owcimomd.sh

BUILT_SOURCES = ${PACKAGE_PREFIX_D}owcimomd.in
CLEANFILES = $(PACKAGE_PREFIX_D)owcimomd $(PACKAGE_PREFIX_D)owcimomd.in

include ${top_srcdir}/Makefile-replacement.inc

global_substitutions= \
-e 's!@product_abbreviation@!openwbem!g'

# Define a rule so the automatic replacement can happen magically.
$(PACKAGE_PREFIX_D)owcimomd.in: owcimomd.sh Makefile
	rm -f $@
	cp $< $@

# The replacement rule for converting from a <foo>.in to <foo> will be applied
# to create $(PACKAGE_PREFIX_D)owcimomd
