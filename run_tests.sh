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

doMakeDistCheck()
{
	kill $(ps -C owcimomd -o pid=)
	make -j3 distcheck
	RVAL=$?
	if [ $RVAL != 0 ]; then
		echo "doMakeDistCheck failed!  CONFIGOPTS=$CONFIGOPTS"
	fi
	return $RVAL
}

doATest()
{
	CONFIGOPTS=$1
	./configure $CONFIGOPTS
	kill $(ps -C owcimomd -o pid=)
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

##############################################################################
doTest()
{
	doATest "" || return 1
	doATest "--enable-debug-mode --enable-stack-trace --enable-maintainer-mode" || return 1
	doATest "--disable-zlib" || return 1
	doATest "--disable-openslp" || return 1
	doATest "--disable-acls" || return 1
	doATest "--enable-memory-debug-mode" || return 1
	doATest "--enable-func-name-debug-mode" || return 1
	doATest "--disable-check-null-references --disable-check-array-indexing" || return 1
	doATest "--disable-digest" || return 1
	doATest "--disable-ssl" || return 1
	doMakeDistCheck || return 1
}

## MAIN ######################################################################
if doTest; then
	echo "Success"
	exit 0
else
	echo "Fail"
	exit 1
fi

