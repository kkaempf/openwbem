#!/bin/bash
set -e
set -x
if [ "$#" != "1" ]; then
	echo "Usage: $(basename $0) ClassName"
fi

originals=`find ../../ -name OW_$1.\?pp`
echo "Found $originals"

for file in $originals; do
	newname=$(basename $file)
	newname=${newname/OW_/OWBI1_}
	newdirname=$(basename $(dirname $file))
	echo "will create $newdirname/$newname"
	if [ ! -d $newdirname ]; then
		mkdir $newdirname
	fi
	sed \
		-e 's/OW_NAMESPACE/OWBI1/g' \
		-e 's/OW_[A-Z]*_API /OWBI1_OWBI1PROVIFC_API /g' \
		-e 's/OW_/OWBI1_/g' \
		< $file > $newdirname/$newname
	if echo $newname | grep -q '\.cpp$'; then
		sed 's~^libowowbi1provifc_a_SOURCES = \\$~libowowbi1provifc_a_SOURCES = \\\n'"$newdirname/$newname"' \\~g' < Makefile.am > tmp
		mv tmp Makefile.am
	else
		sed 's~^pkginclude_HEADERS = \\$~pkginclude_HEADERS = \\\n'"$newdirname/$newname"' \\~g' < Makefile.am > tmp
		mv tmp Makefile.am
	fi
done

