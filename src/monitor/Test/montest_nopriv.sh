#!/bin/bash

set -e
set -o nounset # treat undefined variables as errors, not as empty string

srcdir=$1

base_dir=/montest-689acb0e1ec89f45-7085a2a1780f5f42
cfgpath=$srcdir/monitor_test.nopriv.cfg
testtgz=$srcdir/monitor_test.nopriv.test.tgz
goldtgz=$testtgz

for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do
	echo "mtnopriv.$i.inp"
	$srcdir/montest.sh $cfgpath root 700 $testtgz $srcdir/mtnopriv.$i.inp $srcdir/mtnopriv.$i.exceptions $goldtgz 1
done
