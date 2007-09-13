dnl @synopsis OW_SELECT_ONE([varname], [action1], [action2], ...)
dnl @author Kevin Harris
dnl
dnl Execute the arguments given by $2..$n until ${$1} is nonzero.
dnl
dnl This is the same as a bunch of nested ifs, but makes writing the
dnl tests easier.
dnl
dnl How it works:
dnl It will repeatedly check the value of the variable given by $1.  If
dnl this is zero, then the $2 will be executed and it will recurse with
dnl $3...$n still remaining.
dnl
dnl All of the builtin([foo]) junk is used to prevent m4 from
dnl entering an infinite loop and consuming all available memory.
dnl
dnl $1=variable name
dnl $2...$n = actions to perform until the variable specified in $1 is non-zero.
dnl
dnl Example:
dnl value=0
dnl OW_SELECT_ONE([value], [echo 1], [echo 2; value=1], [echo 3])
dnl
dnl would result in:
dnl   echo 1
dnl   echo 2
dnl at which point no further evaluation would be done.
AC_DEFUN([OW_SELECT_ONE],
	[
		builtin([ifelse],
			[$2], [], [],
			[
				if eval test [$]{$1:-0} = 0; then
					$2
				fi
				OW_SELECT_ONE($1, builtin([shift],builtin([shift], $@)))
			]
		)
	]
)