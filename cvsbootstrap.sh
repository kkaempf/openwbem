#! /bin/sh

rm -rf config.cache config.guess config.sub depcomp missing mkinstalldirs autom4te.cache

aclocal \
&& autoconf \
&& autoheader \
&& automake --foreign --add-missing 
