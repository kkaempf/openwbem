#!/bin/bash

# These are common functions that may be used from with other scripts after
# having sourced this script.

# A function to check to see if a variable is set without causing undefined var
# errors.
#
# This function will accept multiple parameters, and return a 0 (true) if all
# are set, or 1 (false) if any one of them is not set.
function var_is_set()
{
	if [ $# -gt 0 ]
	then
		local_return_value=0
		while [ $# -gt 0 ]
		do
			var_is_set_local_flags=$-
			set +u
			if eval test "\"x\$${1}\"" = "x"
			then
				local_return_value=1
			fi
			set +$-
			set -${var_is_set_local_flags}
			unset var_is_set_local_flags
			shift
		done
		if [ ${local_return_value} = 0 ]
		then
			unset local_return_value
			return 0
		fi
		unset local_return_value
		return 1
	fi
	return 1
}

# A function to check to see if a function is defined without causing horrible
# errors when calling undefined functions.
#
# This function will accept multiple parameters, and return a 0 (true) if all
# are functions, or 1 (false) if any one of them is not defined as a function.
function is_function()
{
	if [ $# -gt 0 ] ; then
		for function_name in "$@"; do
			if type "${1}" 2>/dev/null | grep -i 'function' >/dev/null; then
				:
			else
				return 1
			fi
		done
		return 0
	else
		echo "is_function() requires some arguments." >&2
		return 127
	fi
}

function pushf()
{
	if eval "INTERNAL_flags_${1}=\"$-\""; then
		return 0
	fi
	return 1
}

function popf()
{
	if var_is_set INTERNAL_flags_${1}; then
		popf_local_flag_value=`eval "echo \"\\\${INTERNAL_flags_}${1}\""`
		set +$-
		set -${popf_local_flag_value}
		unset INTERNAL_flags_${1} popf_local_flag_value
		return 0
	fi
	return 1
}

# Fix up links to libraries.  Since we can't put symlinks into CVS,
# we have to do this so that the linker will work correctly with the
# pre-built libs that may be stored in CVS.
function create_library_links()
{
	CLL_RETURN_VALUE=0
	while [ $# -gt 0 ]; do
		DIR=${1}
		OLDPWD=`pwd`
		for EXT in sl so dylib; do
			if [ -d ${DIR} ]; then
				for x in `find ${DIR} -name "*.${EXT}.*"`; do
					dir=`dirname ${x}`
					fname=`basename ${x}`
					link_name1=`echo ${fname} | sed "s/\(.*\)\.${EXT}\.\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)/\1.${EXT}.\2/g"`
					link_name2=`echo ${fname} | sed "s/\(.*\)\.${EXT}\.\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)/\1.${EXT}/g"`
					cd ${dir}
					if [ "${link_name1}" != "${fname}" ]; then
						if ln -sf ${fname} ${link_name1} ; then
							:
						else
							echo "Failed creating link ${link_name1} --> ${fname}"
							CLL_RETURN_VALUE=1
						fi
					fi
					if [ "${link_name2}" != "${fname}" ]; then
						if ln -sf ${fname} ${link_name2} ; then
							:
						else
							echo "Failed creating link ${link_name2} --> ${fname}"
							CLL_RETURN_VALUE=1
						fi
					fi
					cd ${OLDPWD}
				done
			fi
		done
		# FIXME! Support osx dylib libraries in their normal naming order: ".*\.[0-9]+\.[0-9]+\.[0-9]+\.dylib"
		shift
	done
	return ${CLL_RETURN_VALUE}
}

function build_release()
{
	if var_is_set BUILD_RELEASE && [ "${BUILD_RELEASE}" = "1" ]; then
		return 0
	else
		return 1
	fi
}

function build_debug()
{
	if var_is_set BUILD_DEBUG && [ "${BUILD_DEBUG}" = "1" ]; then
		return 0
	else
		return 1
	fi
}

# This will create a 'mutex' as given by ${1}, or wait for the mutex to become
# available (up to ${2} seconds, or 10 minutes if ${2} is not given).  If the given
# number is 0, then only one attempt will be made.
function mutex_create()
{
	pushf mutex_temp_flags
	local mutex_name="${1}"
	local seconds_to_wait=600

	if [ $# -ge 2 ]; then
		seconds_to_wait=${2}
	fi

	# do not exit on error.
	set +e

	# A local function to grab a mutex.
	function grab_mutex()
	{
		pushf grab_mutex_temp_flags
		set -x
		local rval=1

		# Use the bash noclobber feature as an atomic set.
		set -o noclobber
		if echo $$ > "${1}" 2>/dev/null; then
			# We snagged it.
			rval=0
		elif cat "${1}" | grep "^$$\$" >/dev/null 2>&1 ; then
			# This pid already owns the mutex.
			rval=0
		else
			# Can't acquire it.
			rval=1
		fi
		popf grab_mutex_temp_flags
		return ${rval}
	}
	mkdir -p `dirname "${mutex_name}"`

	local return_value=1

	if grab_mutex "${mutex_name}" 2>/dev/null; then
		# We've got the mutex... Do nothing.
		return_value=0
	else
		# The file already exists... Wait for it...
		while [ ${seconds_to_wait} -gt 0 ]; do
			let "seconds_to_wait=${seconds_to_wait}-1"
			sleep 1 || true
			if grab_mutex "${mutex_name}" 2>/dev/null ; then
				return_value=0
				break
			fi
		done
	fi
	popf mutex_temp_flags
	return ${return_value}
}

function mutex_release()
{
	local mutex_name="${1}"
	local return_value=1

	if [ -f "${mutex_name}" ]; then
		# verify the mutex is owned by this pid...
		if cat "${mutex_name}" 2>/dev/null | grep "^$$\$" >/dev/null 2>&1 ; then
			# If we're here, then the grep found our pid in the file.
			if rm -f "${mutex_name}" ; then
				return_value=0
			else
				echo "The mutex (${mutex_name}) belongs to me, but I can't remove it."
			fi
		else
			# echo "The mutex (${mutex_name}) belongs to another pid (`cat \"${mutex_name}\"`).  I am $$."
			return_value=2
		fi
	else
		# echo "No such mutex (${mutex_name})"
		return_value=0
	fi
	return ${return_value}
}

function build_history_entry()
{
	pushf bhe_flags
	set +x
	local separator="|"
	local history_file=${HOME}/build_history.txt
	local history_mutex=${history_file}.mutex
	local process_id=$$
	local current_time=`time_in_seconds.sh`
	local machine=
	local build=

	if var_is_set MACHINE_NAME ; then
		machine="${MACHINE_NAME}"
	else
		machine=`hostname`
	fi

	if var_is_set OW_BUILD_ID ; then
		build="${OW_BUILD_ID}"
	else
		build="unknown"
	fi


	if mutex_create ${history_mutex}; then

		echo "${current_time}${separator}${machine}${separator}${build}${separator}" $@ >> ${history_file}

		mutex_release ${history_mutex}	|| true
	else
		echo "Unable to create history entry."
	fi
	popf bhe_flags
}
