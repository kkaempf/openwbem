SUBDIRS = db src etc test mof doc schemas

EXTRA_DIST = \
Doxyfile.sh \
ow_ar.sh \
Makefile-shared-lib.inc \
AuthModule.HOWTO \
ACL.HOWTO \
PORTING.txt \
ProviderInterface.HOWTO \
LICENSE \
ow_ar.sh \
Makefile-shared-lib.inc \
Makefile-shared-lib-inst.inc \
openwbem.spec \
openwbem-faq.html

CLEANFILES = Doxyfile

SUFFIXES = .sh

.sh:
	@RM@ -f $@ $@-t
	@SED@ \
		-e 's!@''srcdir''@!$(srcdir)!g' \
	$< > $@-t
	@MV@	$@-t $@

docs: Doxyfile
	doxygen Doxyfile
