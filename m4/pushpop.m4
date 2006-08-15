AC_DEFUN([OW_PUSH_VARIABLE],
	[
		ow_stack_count_$1=${ow_stack_count_$1:-0}
		eval ow_stack_entries_${ow_stack_count_$1}="\"${$1}\""
		ow_stack_count_$1=`expr ${ow_stack_count_$1} \+ 1`
	]
)

AC_DEFUN([OW_POP_VARIABLE],
	[
		ow_stack_count_$1=${ow_stack_count_$1:-0}
		if test ${ow_stack_count_$1} -gt 0; then
			ow_stack_count_$1=`expr ${ow_stack_count_$1} \- 1`
			eval $1="\"\${ow_stack_entries_${ow_stack_count_$1}}\""
			unset ow_stack_entries_${ow_stack_count_$1}
		else
			AC_MSG_ERROR(Attempt to pop empty variable $1)
		fi
	]
)

AC_DEFUN([OW_SAVE_FLAGS],
	[
		OW_PUSH_VARIABLE(CFLAGS)
		OW_PUSH_VARIABLE(CXXFLAGS)
		OW_PUSH_VARIABLE(LDFLAGS)
	]
)

AC_DEFUN([OW_RESTORE_FLAGS],
	[
		OW_POP_VARIABLE(LDFLAGS)
		OW_POP_VARIABLE(CXXFLAGS)
		OW_POP_VARIABLE(CFLAGS)
	]
)
