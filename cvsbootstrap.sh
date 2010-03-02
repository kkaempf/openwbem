#! /bin/sh

#set -x
set -e

rm -rf config.cache config.guess config.sub depcomp missing mkinstalldirs autom4te.cache

if bison --version >/dev/null; then
	BISON_VERSION_LINE=`bison --version | head -n 1`
	BISON_VERSION=`echo $BISON_VERSION_LINE | sed 's/^[^0-9]*//'`
	BISON_MAJOR_VERSION=`echo $BISON_VERSION | sed 's/\..*$//'`
	BISON_MINOR_VERSION=`echo $BISON_VERSION | sed 's/[^.]*\.//'`
else
	echo "ERROR: bison not found"
	exit 1
fi

if [ "z$BISON_VERSION" = "z" ]; then
	echo "ERROR: bison version cannot be determined"
	exit 1
fi

if [ $BISON_MAJOR_VERSION -lt 2 ] || [ $BISON_MINOR_VERSION -lt 1 ]; then
	echo "ERROR: bison version is < 2.1"
#	exit 1
fi

aclocal-1.9
autoconf
autoheader
automake-1.9 --foreign --add-missing
