#/bin/sh

if [ x${1:-nothing} = x--full ]; then
	FULL_TESTS=1
	shift
else
	FULL_TESTS=0
fi

if [ $# -le 0 ]; then
	echo "Usage: `basename $0` [option] ClassName" >&2
	echo "Where option can be --full to produce a full (non-auto) test." >&2
	exit 1
fi

while [ $# -gt 0 ]; do
	if [ -f $1TestCases.cpp -o -f $1TestCases.hpp ]; then
		echo "Test Case for $1 already exist" >&2
		exit 1
	fi

	if [ x${FULL_TESTS} = x0 ]; then
		sed "s/Generic/$1/g" < GenericAutoTestCases.cpp.tmpl > $1TestCases.cpp
		SOURCE_FILES=$1TestCases.cpp
	else
		sed "s/Generic/$1/g" < GenericTestCases.hpp.tmpl > $1TestCases.hpp
		sed "s/Generic/$1/g" < GenericTestCases.cpp.tmpl > $1TestCases.cpp
		SOURCE_FILES="$1TestCases.cpp $1TestCases.hpp"
	fi
	sed -e 's~\(BUILT_TESTS = \\\)$~\1\n'"$1"'TestCases \\~g' \
		-e 's~\(# Test case definitions -- DO NOT EDIT THIS COMMENT\)~\1\n'"$1"'TestCases_SOURCES = '"${SOURCE_FILES}"'\n~g' < Makefile.am > tmp && mv tmp Makefile.am
		echo "$1TestCases" >> .cvsignore

	if [ -d CVS ]; then
		cvs add ${SOURCE_FILES}
	else
		svn add ${SOURCE_FILES}
	fi

	shift
done

