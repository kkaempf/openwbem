dnl @synopsis OW_REPLACEMENT
dnl @author Kevin Harris
dnl
dnl Create replacement sed scripts for all substituted variables
dnl
dnl There is a lot of junk in here to work around quoting.
dnl
dnl Since some of the variables being substituted will be makefile variables
dnl (cannot be evaluated now), some processing must be done to prevent the
dnl shell from interpreting them and puking (hence the ugly sed expression).
dnl
dnl Some variables have backslashes in them.  These need to be preserved.
AC_DEFUN([OW_REPLACEMENT_SCRIPT],
	[
		AC_MSG_NOTICE([Generating replacement script "$1"])

		rm -f $1
		touch $1

		replacement_at_expression='s/[[$]]@/\\$\@/g'
		replacement_makefile_expression='s/[[$]][[(]]/MAKEFILE_VARIABLE(/g'
		replacement_backslash_expression='s/\\/\\\\/g'

		for replacement_variable in ${ac_subst_vars}; do
			# Evaluate (repeatedly) the variable value until it no longer changes.
			replacement_value1=
			replacement_value=`eval printf '%s' "\"\\\${$replacement_variable}\"" | sed -e "${replacement_makefile_expression}" -e "${replacement_backslash_expression}" -e "${replacement_at_expression}"`
			while test "x${replacement_value1}" != "x${replacement_value}"; do
				replacement_value1="${replacement_value}"
				replacement_value=`eval printf '%s' "\"${replacement_value}\"" | sed -e "${replacement_makefile_expression}" -e "${replacement_backslash_expression}" -e "${replacement_at_expression}"`
			done
			# Find a valid sed separator that is not present in the replacement expression.
			replacement_separator=
			for sep in ',' '/' '|' '?' '!'; do
				# If you ever find horrible shell breakage while executing the
				# configure script, check this next line first.
				replacement_without_separator=`eval printf '%s' "\"\\\${replacement_value%${sep}*}\""`
				if test "x${replacement_value}" = "x${replacement_without_separator}"; then
					replacement_separator=${sep}
					break
				fi
			done
			dnl AC_MSG_NOTICE([Substituting variable "${replacement_variable}" as "${replacement_value}"])
			printf '%s\n' "s${replacement_separator}@${replacement_variable}@${replacement_separator}${replacement_value}${replacement_separator}g" >> $1
		done
	]
)
