#!/bin/bash -x

# This shell script, which is meant to be sourced at the TOP of another script,
# will set environment variables based on parameters.  ie. if a parameter like
# "FOO=yes" is given, then the environment variable "FOO" will be set to "yes".
# All other parameters will be placed in an array called PARAMETERS.
# 
# Should you want the parameters back on your command line, use this:
#   set -- ${PARAMETERS[@]}
#
#
# The main use of this script, is to allow the environment to be easily set
# from a command line (especially from a remote login).

# Although this may work on different shells, YOU REALLY SHOULD use bash.

# Since a -e may be wrapped around this script, PARAMETERS must be set before
# unsetting it.  Thankfully, an array can have an unnumbered element,
# accessible by its name.  After setting it, unsetting it will unset the entire
# array without causing the shell to abort.
PARAMETERS=unused
unset PARAMETERS
declare -a PARAMETERS

recreate_environment()
{
	if [ $# -gt 0 ]
	then
		PARAMETERS[0]=
		let "current_param_number=1"
		while [ $# -gt 0 ]
		do
			if echo "${1}" | grep "=" > /dev/null
			then
				LOCAL_VAR="`echo \"${1}\" | cut -f1 -d'='`"
				LOCAL_VALUE="`echo \"${1}\" | cut -f2- -d'='`"
# FIXME! Debug output
				echo "Setting ${LOCAL_VAR} to ${LOCAL_VALUE}"
####
				eval "${LOCAL_VAR}=\"${LOCAL_VALUE}\""
				export ${LOCAL_VAR}
				unset LOCAL_VAR
				unset LOCAL_VALUE
			else
				PARAMETERS[${current_param_number}]="${1}"
				let "current_param_number+=1"
			fi  
			shift
		done	
		unset current_param_number
	fi
}

recreate_environment "$@"
echo "Still have parameters: ${PARAMETERS[@]}"
