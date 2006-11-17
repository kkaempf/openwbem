#!/bin/sh

# This script doesn't allow proper testing of multiple exceptions output by a
# single program.  Most of the tests in this directory wouldn't care, but a few
# do have multiple exceptions.

set -u
set -e

exception_list="$1"
output_file="$2"

if [ -f ${exception_list} -o -h ${exception_list} ]; then
	:
else
	echo "Exception list file does not exist: ${exception_list}" >&2
	exit 1
fi

if [ -f ${output_file} -o -h ${output_file} ]; then
	:
else
	echo "Output file does not exist: ${output_file}" >&2
	exit 1
fi

while read exception_type exception_message; do
	case $exception_type in
		'#' | '#*') continue ;;
	esac
	if [ x${exception_type:+set} = xset ]; then
		if grep -iq "^[ 	]*type:[ 	]*${exception_type}$" ${output_file}; then
			:
		else
			foo=$?
			echo "Execution output did not include exception \"${exception_type}\"" >&2
			exit ${foo}
		fi
	fi
	if [ x${exception_message:+set} = xset ]; then
		if grep -iq "^[ 	]*msg:.*${exception_message}.*" ${output_file}; then
			:
		else
			foo=$?
			echo "Execution output did not include exception message \"${exception_message}\"" >&2
		   exit ${foo}
		fi
	fi
done < ${exception_list}
