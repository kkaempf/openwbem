#!/bin/sh

#OPENWBEMSRCDIR=/usr/local/code/openwbem20
#STAGEDIR=$OPENWBEMSRCDIR/test/acceptance/stage
STAGEDIR=`dirname $0`/stage

LD_LIBRARY_PATH=$STAGEDIR/usr/local/lib:$STAGEDIR/usr/local/lib/openwbem/services:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

#gdb $STAGEDIR/usr/local/sbin/owcimomd << EOS
#b main
#r -d -c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t
#EOS

if [ "$1" = "vg" ]; then
  valgrind \
	--leak-check=yes \
	--gdb-attach=yes \
	--error-limit=no \
	--num-callers=9 \
	$STAGEDIR/usr/local/sbin/owcimomd -d \
	-c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t 
#	--logfile-fd=9 \
#	9>> valgrind.out
else
  $STAGEDIR/usr/local/sbin/owcimomd -d \
	-c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t
fi


