#! /bin/sh

rm -f config.cache config.guess config.sub depcomp missing mkinstalldirs

aclocal \
&& autoconf \
&& autoheader \
&& automake --gnu --add-missing 
