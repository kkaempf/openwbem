#! /bin/sh

rm -f config.sub config.guess ltmain.sh ltconfig

aclocal \
&& autoconf \
&& automake --foreign --add-missing 
