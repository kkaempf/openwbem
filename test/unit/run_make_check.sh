#!/bin/sh

wrapper=./set_test_libpath.sh
MAKE=${MAKE:-make}

# Build the wrapper script if needed.
if [ -f ${wrapper} ]; then
	:
else
	${MAKE} local-scripts
fi

if [ $# -eq 0 ] || [ x$1 = xall ]; then
	${MAKE} check
	exit $?
fi

for testname in "$@"; do
	if [ -f ${testname}TestCases ]; then
		${wrapper} ./${testname}TestCases || exit $?
	elif [ -f ${testname} ]; then
		${wrapper} ./${testname} || exit $?
	elif [ -f ./${testname}TestCases.cpp ] || [ -f ./${testname}Test.cpp ]; then
		# Build and rerun...
		${MAKE} || exit $?
		exec "$0" "$@"
	else
		echo "No test for ${testname}" >&2
		exit 1
	fi
done


