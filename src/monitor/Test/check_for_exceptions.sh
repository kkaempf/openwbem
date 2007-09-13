#!/bin/sh

# This script doesn't allow proper testing of multiple exceptions output by a
# single program.  Most of the tests in this directory wouldn't care, but a few
# do have multiple exceptions.

set -u
set -e
# set -x

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

awk -f ./mgrep.awk '.*Exception:.*\n.*type:.*\n.*msg:.*' ${output_file} > ${output_file}.cfe1 || true

bn='\\n'

missing_exception=0

while read exception_type exception_message; do
	# Strip whitespace from the exception type...
	exception_type=`echo ${exception_type} | awk '{ print $1; }'`
	if [ x${exception_type:+set} != xset ]; then
		continue
	fi
	# Ignore comments.
	case $exception_type in
		'#' | '#*') continue ;;
	esac

	if [ x${exception_message:+set} = xset ]; then
		exception_regex="^.*Exception:.*${bn}[ 	]*type:[ 	]*${exception_type}.*${bn}[ 	]*msg:.*${exception_message}.*"
		if grep -i "${exception_regex}" ${output_file}.cfe1 >/dev/null; then
			grep -i -v "${exception_regex}" ${output_file}.cfe1 > ${output_file}.cfe2 || true
			mv ${output_file}.cfe2 ${output_file}.cfe1
		else
			echo "Execution output did not include exception of type ${exception_type} with message \"${exception_message}\"" >&2
			missing_exception=1
		fi
	else
		exception_regex="^.*Exception:.*${bn}[ 	]*type:[ 	]*${exception_type}.*"
		if grep -i "${exception_regex}" ${output_file}.cfe1 >/dev/null; then
			grep -i -v "${exception_regex}" ${output_file}.cfe1 > ${output_file}.cfe2 || true
			mv ${output_file}.cfe2 ${output_file}.cfe1
		else
			echo "Execution output did not include exception of type \"${exception_type}\"" >&2
			missing_exception=1
		fi
	fi
done < ${exception_list}

exit_code=0

if grep -i exception ${output_file}.cfe1 >/dev/null; then
	echo
	echo "ERROR: One or more unexpected exceptions were located:" >&2
	cat ${output_file}.cfe1 | sed 's/\\n/\n/g'
	echo
	exit_code=2
fi
if [ ${missing_exception} = 1 ]; then
	echo
	echo "ERROR: Missing one or more expected exceptions (look at above output for details)." >&2
	echo
	exit_code=1
fi

rm -f ${output_file}.cfe1
exit ${exit_code}
