#!/bin/sh
#
# run_tests.sh - test openwbem
#
# This script assumes the current directory is the OW root.
# 

unset OWLONGTEST

set -a
set -x
set -u

killowcimomd()
{
	kill $(ps -C owcimomd -o pid=) || true
}

doMakeDistCheck()
{
	killowcimomd
	make -j3 distcheck
	RVAL=$?
	if [ $RVAL != 0 ]; then
		echo "doMakeDistCheck failed!"
	fi
	return $RVAL
}

doATest()
{
# TODO: test make rpm in here.
	CONFIGOPTS=$1
	make distclean
	./cvsbootstrap.sh
	./configure $CONFIGOPTS
	killowcimomd
	make -j3 \
		&& make -j3 check \
		&& OWLONGTEST=1 make -j3 check \
		&& make clean
	RVAL=$?
	if [ $RVAL != 0 ]; then
		echo "doATest failed!  CONFIGOPTS=$CONFIGOPTS"
	fi
	return $RVAL
}

doACompileOnlyTest()
{
	CONFIGOPTS=$1
	make distclean
	./cvsbootstrap.sh
	./configure $CONFIGOPTS
	killowcimomd
	make -j3 \
		&& make clean
	RVAL=$?
	if [ $RVAL != 0 ]; then
		echo "doACompileOnlyTest failed!  CONFIGOPTS=$CONFIGOPTS"
	fi
	return $RVAL
}

##############################################################################
doTests()
{
	doATest "" || return 1
	doATest "--enable-debug-mode --enable-stack-trace --enable-maintainer-mode" || return 1
	doMakeDistCheck || return 1
	doATest "--disable-zlib" || return 1
	doATest "--disable-openslp" || return 1
	doATest "--disable-acls" || return 1
	doACompileOnlyTest "--enable-memory-debug-mode" || return 1
	doATest "--enable-func-name-debug-mode" || return 1
	doATest "--disable-check-null-references --disable-check-array-indexing" || return 1
	doATest "--disable-digest" || return 1
	doATest "--disable-ssl" || return 1
	doATest "--with-package-prefix=foo" || return 1
	doACompileOnlyTest "--enable-perl-providerifc" || return 1
	doACompileOnlyTest "--disable-association-traversal" || return 1
	doACompileOnlyTest "--disable-qualifier-declaration" || return 1
	doACompileOnlyTest "--disable-schema-manipulation" || return 1
	doACompileOnlyTest "--disable-instance-manipulation" || return 1
	doACompileOnlyTest "--disable-association-traversal --disable-qualifier-declaration --disable-schema-manipulation --disable-instance-manipulation" || return 1
}

## MAIN ######################################################################
if doTests; then
	echo "Success"
	exit 0
else
	echo "Fail"
	exit 1
fi

