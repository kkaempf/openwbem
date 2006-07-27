#!/bin/sh

ulimit -c unlimited

#OPENWBEMSRCDIR=/usr/local/code/openwbem20
#STAGEDIR=$OPENWBEMSRCDIR/test/acceptance/stage
STAGEDIR=`dirname $0`/stage

LD_LIBRARY_PATH=$STAGEDIR/usr/local/lib:$STAGEDIR/usr/local/lib/openwbem/services:$LD_LIBRARY_PATH
# AIX
LIBPATH="$LD_LIBRARY_PATH:$LIBPATH" 

# HP-UX
SHLIB_PATH="$LD_LIBRARY_PATH:$SHLIB_PATH"

# Mac OSX
DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH

export LD_LIBRARY_PATH LIBPATH SHLIB_PATH DYLD_LIBRARY_PATH

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


