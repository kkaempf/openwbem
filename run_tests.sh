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

# echo is to get rid of extra spaces
NUM_PROCS=`echo \`cat /proc/cpuinfo | grep processor | wc -l\``
if [ -n "$NUM_PROCS" ] && [ "x$NUM_PROCS" != "x1" ]; then
	let NUM_PROCS_PLUS_1=$NUM_PROCS + 1
	MAKE_PARALLEL="-j$NUM_PROCS_PLUS_1"
else
	MAKE_PARALLEL=""
fi

killowcimomd()
{
	kill $(ps -C owcimomd -o pid=) || true
}

doMakeDistCheck()
{
	killowcimomd
	make $MAKE_PARALLEL distcheck
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
	make $MAKE_PARALLEL \
		&& make $MAKE_PARALLEL check \
		&& OWLONGTEST=1 make $MAKE_PARALLEL check \
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
	make $MAKE_PARALLEL \
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
	doATest "--enable-debug-mode --enable-stack-trace --enable-maintainer-mode --enable-valgrind-support" || return 1
	doMakeDistCheck || return 1
	doATest "--disable-zlib" || return 1
	doATest "--disable-openslp" || return 1
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
	doACompileOnlyTest "--disable-property-operations" || return 1
	doACompileOnlyTest "--disable-association-traversal --disable-qualifier-declaration --disable-schema-manipulation --disable-instance-manipulation --disable-property-operations" || return 1
	doATest "--enable-static-services" || return 1
}

## MAIN ######################################################################
if doTests; then
	echo "Success"
	exit 0
else
	echo "Fail"
	exit 1
fi

