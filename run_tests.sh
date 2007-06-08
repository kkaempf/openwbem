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
	let NUM_PROCS_PLUS_1=$NUM_PROCS+1
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
	pushd $BLOCXX_LOCATION
	make distclean
	./bootstrap.sh
	./configure $CONFIGOPTS
	make $MAKE_PARALLEL \
		&& make $MAKE_PARALLEL check \
		&& make $MAKE_PARALLEL distcheck DISTCHECK_CONFIGURE_FLAGS="$CONFIGOPTS"
	RVAL=$?
	if [ $RVAL != 0 ]; then
		echo "doATest failed!  CONFIGOPTS=$CONFIGOPTS"
		return $RVAL
	fi

	popd

	make distclean
	./cvsbootstrap.sh
	./configure $CONFIGOPTS --with-blocxx="$BLOCXX_LOCATION"
	killowcimomd
	make $MAKE_PARALLEL \
		&& OWLONGTEST=1 make $MAKE_PARALLEL check \
		&& make $MAKE_PARALLEL distcheck DISTCHECK_CONFIGURE_FLAGS="$CONFIGOPTS --with-blocxx=\"$BLOCXX_LOCATION\""

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
	doATest "--disable-pam" || return 1
	doATest "--with-package-prefix=foo" || return 1
	doACompileOnlyTest "--disable-association-traversal" || return 1
	doACompileOnlyTest "--disable-qualifier-declaration" || return 1
	doACompileOnlyTest "--disable-schema-manipulation" || return 1
	doACompileOnlyTest "--disable-instance-manipulation" || return 1
	doACompileOnlyTest "--disable-property-operations" || return 1
	doACompileOnlyTest "--disable-namespace-manipulation" || return 1
	doACompileOnlyTest "--disable-association-traversal --disable-qualifier-declaration --disable-schema-manipulation --disable-instance-manipulation --disable-property-operations --disable-namespace-manipulation" || return 1
	doATest "--prefix=/opt/some/other/prefix --enable-rpath-link" || return 1
	doATest "--prefix=/opt/some/other/prefix --with-runtime-link-path=/opt/some/other/prefix/some/other/lib/dir" || return 1
	doATest "--enable-non-thread-safe-exception-handling-workaround" || return 1
	doATest "--enable-static-services" || { echo "--enable-static-services failed"; }
	doATest "--enable-monitored-perl-ifc" || return 1
	doATest "--enable-64-bit-build" || return 1
	doATest "--disable-cmpi" || return 1
	doATest "--enable-vas" || return 1
	doATest "--enable-ipv6" || return 1
	doATest "--disable-ipv6" || return 1
}

## MAIN ######################################################################
BLOCXX_LOCATION=$1
if doTests; then
	echo "Success"
	exit 0
else
	echo "Fail"
	exit 1
fi

