#!/bin/sh
SIZESTAGE=`pwd`/sizestage
rm -rf $SIZESTAGE
make DESTDIR="$SIZESTAGE" install >/dev/null 2>/dev/null || exit 1
cd "$SIZESTAGE" || exit 1
find . -name '*.hpp' -or -name '*.mof' -or -name '*.pm' -or -name '*.bs' -or -name '*.h' -or -name '*.3pm' -or -name '*.conf' | xargs rm -f
find . -type f | xargs strip --strip-unneeded
du -ak . | grep -v ^0
