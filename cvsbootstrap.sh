#! /bin/sh

rm -rf config.cache config.guess config.sub depcomp missing mkinstalldirs autom4te.cache

# Make sure automake is at least as new as 1.6.3 . Older automakes
# will set AR=ar, preventing openwbem from using ow_ar.sh , resulting
# in nested archives which confuse ld, and cause lots of 'undefined
# reference to foo' errors.

AUTOMAKE_VERSION=`automake --version | head -1 | sed -e "s/[^0-9.]//g" `

AUTOMAKE_MAJOR=`echo $AUTOMAKE_VERSION | cut -f1 -d'.'`
AUTOMAKE_MINOR=`echo $AUTOMAKE_VERSION | cut -f2 -d'.'`
AUTOMAKE_MICRO=`echo $AUTOMAKE_VERSION | cut -f3 -d'.'`

if [ ! $AUTOMAKE_MICRO ] ; then AUTOMAKE_MICRO="0"
fi

AUTOMAKE_NUMBER=`expr $AUTOMAKE_MAJOR \* 10000 \+ $AUTOMAKE_MINOR \* 100 \+ $AUTOMAKE_MICRO`
if [ $AUTOMAKE_NUMBER -ge 10603 ] ; then echo 'automake version 1.6.3 or greater, good.'
else echo 'automake 1.6.3 or newer required.'
fi

aclocal \
&& autoconf \
&& autoheader \
&& automake --foreign --add-missing 
