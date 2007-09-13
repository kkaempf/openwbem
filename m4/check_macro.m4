dnl @synopsis OW_CHECK_MACRO(name, [true], [false])
dnl @author Kevin Harris
dnl check for the macro defined by $1.
dnl if defined, $2 will be executed.
dnl if not, $3 will
AC_DEFUN([OW_CHECK_MACRO],
	[
		AC_RUN_IFELSE(
			[
				int main(int argc, const char** argv)
				{
				#if defined $1
					return 0;
				#else
					return 1;
				#endif
				}
			],
			[
				$2
			],
			[
				$3
			]
		)
	]
)
