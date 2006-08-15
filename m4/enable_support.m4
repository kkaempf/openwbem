dnl OW_ENABLE_SUPPORT -- Check for the --enable-FOO or --disable-FOO argument
dnl
dnl Allows up to 4 arguments (should have at least one)
dnl $1 = the name of the item to support
dnl $2 = action when "force-enabled"
dnl $3 = action when "force-disabled"
dnl $4 = action when unspecified (autodetect)
AC_DEFUN([OW_ENABLE_SUPPORT],
	[
		enable_support_variable=`echo $1 | tr '[-]' '[_]'`
		AC_MSG_CHECKING([if support for $1 is enabled])
		AC_ARG_ENABLE($1,
			OW_HELP_STRING([--enable-$1],[enable support for $1]),
			[
				if eval test "x\$enable_${enable_support_variable}" = "xyes"; then
					eval enable_${enable_support_variable}=force_enable
					AC_MSG_RESULT([force-enabled])
					$2
				else
					eval enable_${enable_support_variable}=force_disable
					AC_MSG_RESULT([force-disabled])
					$3
				fi
			],
			[
				eval enable_${enable_support_variable}=autodetect
				AC_MSG_RESULT([(autodetect)])
				$4
			]
		)
	]
)

dnl Check for support of some feature with autodetection.  This may
dnl seem complex, but for most use, it should be rather simple.
dnl
dnl $1 = Name of the option
dnl $2 = <program> (see AC_LANG_PROGRAM)
dnl $3 = executed when force-enabled and available.
dnl $4 = executed when force-disabled
dnl $5 = executed when autodetect available
dnl $6 = executed when autodetect unavailable.
dnl
dnl Most of the time, $3 through $6 should not be needed.
AC_DEFUN([OW_VERIFY_ENABLE_SUPPORT],
	[
		enable_support_variable=`echo $1 | tr '[-]' '[_]'`
		OW_ENABLE_SUPPORT([$1],
			[
				AC_MSG_CHECKING([if support for $1 is possible])
				AC_LINK_IFELSE(
					[ $2 ],
					[
						AC_MSG_RESULT(yes)
						eval enable_${enable_support_variable}=yes
						$3
					],
					AC_MSG_ERROR(Support for $1 is not possible)
				)
			],
			[
				eval enable_${enable_support_variable}=no
				$4
			],
			[
				AC_MSG_CHECKING([(autodetecting) possible support for $1])
				AC_LINK_IFELSE(
					[ $2 ],
					[
						AC_MSG_RESULT(yes)
						eval enable_${enable_support_variable}=yes
						$5
					],
					[
						AC_MSG_RESULT(no)
						eval enable_${enable_support_variable}=no
						$6
					]
				)
			]
		)
	]
)

dnl Check for support of some feature with autodetection (execution
dnl required).  This may seem complex, but for most use, it should
dnl be rather simple.
dnl
dnl $1 = Name of the option
dnl $2 = <program> (see AC_LANG_PROGRAM)
dnl $3 = executed when force-enabled and available.
dnl $4 = executed when force-disabled
dnl $5 = executed when autodetect available
dnl $6 = executed when autodetect unavailable.
dnl
dnl Most of the time, $3 through $6 should not be needed.
AC_DEFUN([OW_EXEC_VERIFY_ENABLE_SUPPORT],
	[
		enable_support_variable=`echo $1 | tr '[-]' '[_]'`
		OW_ENABLE_SUPPORT([$1],
			[
				AC_MSG_CHECKING([if support for $1 is possible])
				AC_RUN_IFELSE(
					[ $2 ],
					[
						AC_MSG_RESULT(yes)
						eval enable_${enable_support_variable}=yes
						$3
					],
					AC_MSG_ERROR(Support for $1 is not possible)
				)
			],
			[
				eval enable_${enable_support_variable}=no
				$4
			],
			[
				AC_MSG_CHECKING([(autodetecting) possible support for $1])
				AC_RUN_IFELSE(
					[ $2 ],
					[
						AC_MSG_RESULT(yes)
						eval enable_${enable_support_variable}=yes
						$5
					],
					[
						AC_MSG_RESULT(no)
						eval enable_${enable_support_variable}=no
						$6
					]
				)
			]
		)
	]
)
