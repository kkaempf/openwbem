#!/bin/sh

ulimit -c unlimited

#OPENWBEMSRCDIR=/usr/local/code/openwbem20
#STAGEDIR=$OPENWBEMSRCDIR/test/acceptance/stage
STAGEDIR=`dirname $0`/stage

. `dirname $0`/env.sh

if [ "$1" = "vg" ]; then
  valgrind \
	--tool=memcheck \
	--leak-check=yes \
	--track-fds=yes \
	--show-reachable=yes \
	--error-limit=no \
	--num-callers=99 \
	--log-file=valgrind.out \
	--suppressions=OpenSSL.supp \
	$STAGEDIR/usr/local/sbin/owcimomd -d \
	-c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t 
elif [ "x$1" = "xgdb" ]; then
#	gdb $STAGEDIR/usr/local/sbin/owcimomd << EOS
	gdb $STAGEDIR/usr/local/sbin/owcimomd 
#b main
#r -d -c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t
#EOS
elif [ "x$1" = "xinsight" ]; then
	insight $STAGEDIR/usr/local/sbin/owcimomd << EOS
b main
r -d -c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t
EOS
elif [ "x$1" = "xstrace" ]; then
  strace -f -o /tmp/strace.out \
    $STAGEDIR/usr/local/sbin/owcimomd -d \
	  -c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t
else
  $STAGEDIR/usr/local/sbin/owcimomd -d \
	-c $STAGEDIR/usr/local/etc/openwbem/openwbem.conf-t
fi


