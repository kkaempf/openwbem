#/bin/sh
case $# in
1)
	if [ -f $1TestCases.hpp ]
	then
		echo "Test Case for $1 already exists" 1>&2
		exit 1
	fi
   sed "s/Generic/$1/g" < GenericTestCases.hpp.tmpl > $1TestCases.hpp
   sed "s/Generic/$1/g" < GenericTestCases.cpp.tmpl > $1TestCases.cpp
   sed 's~\(// test cases includes -- DO NOT EDIT THIS COMMENT\)~\1\
#include "'"$1"'TestCases.hpp"~g' < UnitMain.cpp > tmp
   mv tmp UnitMain.cpp
   sed 's~\(	// add tests to runner -- DO NOT EDIT THIS COMMENT\)~\1\
	runner.addTest( "'"$1"'", '"$1"'TestCases::suite());~g' < UnitMain.cpp > tmp
   mv tmp UnitMain.cpp
   sed 's~^unitMain_SOURCES = \\$~unitMain_SOURCES = \\\n'"$1"'TestCases.cpp \\\n'"$1"'TestCases.hpp \\~g' < Makefile.am > tmp
   mv tmp Makefile.am
   cvs add $1TestCases.*
   ;;
*) echo "Usage: `basename $0` ClassName" 1>&2 ;;
esac

