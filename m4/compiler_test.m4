dnl @synopsis OW_COMPILER_TEST
dnl @author Kevin Harris
dnl Check for the compiler type and version.
dnl
dnl This will define one of (names before transform):
dnl   USING_ACC
dnl   USING_XLC
dnl   USING_SUNC
dnl   USING_GCC
dnl And will define:
dnl   COMPILER_VERSION
dnl   COMPILER_MAJOR
dnl   COMPILER_MINOR
dnl And will set the variables:
dnl   OW_COMPILER_VERSION
dnl   OW_COMPILER_MAJOR
dnl   OW_COMPILER_MINOR
dnl   OW_COMPILER_TYPE
dnl   OW_COMPILER_LIBDIR
dnl And will export to Makefiles:
dnl   OW_COMPILER_LIBDIR
dnl
dnl Among the variables it sets, this could be used (in the future) to
dnl store in the OW_config.h the compiler type that was used to build
dnl OW.  This could be important when building something that is using
dnl a binary-incompatible compiler.

AC_DEFUN([OW_COMPILER_TEST],
	[
		OW_COMPILER_LIBDIR=
		found_compiler_type=0
		OW_SELECT_ONE([found_compiler_type],
			# Check for aCC
			[
				AC_MSG_CHECKING([C++ compiler is aCC])
				OW_CHECK_MACRO([__HP_aCC],
					[
						AC_MSG_RESULT([yes])
						AC_DEFINE(USING_ACC, [], [OW was built with aCC])
						OW_COMPILER_TYPE=aCC
						found_compiler_type=1
					],
					[
						AC_MSG_RESULT([no])
					]
				)
			],
			# Check for xlC
			[
				AC_MSG_CHECKING([C++ compiler is xlC])
				OW_CHECK_MACRO([__xlC__],
					[
						AC_MSG_RESULT([yes])
						AC_DEFINE(USING_XLC, [], [OW was built with xlC])
						OW_COMPILER_TYPE=xlC
						found_compiler_type=1
					],
					[
						AC_MSG_RESULT([no])
					]
				)
			],
			# Check for Sun C++
			[
				AC_MSG_CHECKING([C++ compiler is Sun C++])
				# Sun C++ apparently defines no macros that could be used
				# to identify it.  Use the -V option and a grep instead.
				if $CXX -V 2>&1 | grep 'Sun C++' >/dev/null 2>&1; then
					AC_MSG_RESULT([yes])
					AC_DEFINE(USING_SUNC, [], [OW was built with Sun C++])
					OW_COMPILER_TYPE=sunc
					found_compiler_type=1
				else
					AC_MSG_RESULT([no])
				fi
			],
			# Check for gcc
			[
				AC_MSG_CHECKING([C++ compiler is gcc])
				OW_CHECK_MACRO([__GNUC__],
					[
						AC_MSG_RESULT([yes])
						AC_DEFINE(USING_GCC, [], [OW was built with gcc (g++)])
						OW_COMPILER_TYPE=gcc
						found_compiler_type=1
					],
					[
						AC_MSG_RESULT([no])
					]
				)
			],
			[
				AC_MSG_WARN(Unable to detect the compiler type)
				OW_COMPILER_TYPE=unknown
			]
		)

		# Get the version...
		# gcc:
		# $ g++ --version
		# g++ (GCC) 4.1.2 (Ubuntu 4.1.2-0ubuntu4)
		# ...
		# -->  g++ --version 2>&1 | grep '[0-9][0-9]*\.[0-9][0-9]*' | sed -e 's/[(][^)]*[)]//g' -e 's/[^0-9.]//g'
		#
		# xlC_r:
		# $ xlC_r -qversion
		# IBM XL C/C++ Enterprise Edition V8.0 for AIX
		# Version: 08.00.0000.0010
		# --> xlC_r -qversion 2>&1 | sed -e 's/[^0-9.]//g' | head -1
		#
		# aCC:
		# $ aCC --version
		# aCC: HP ANSI C++ B3910B A.03.67
		# --> aCC --version 2>&1 | sed 's/[^.]*\.//'
		#
		# Sun C:
		# $ CC -V
		# CC: Sun C++ 5.9 SunOS_i386 2007/05/03
		# --> CC -V 2>&1 | grep 'Sun C++' | sed -e 's/.*C++[ ]*//' -e 's/ .*//'
		case $OW_COMPILER_TYPE in
			aCC)
				OW_COMPILER_VERSION=`$CXX --version 2>&1 | sed 's/[[^.]]*\.//'`
				;;
			xlC)
				OW_COMPILER_VERSION=`$CXX -qversion 2>&1 | sed -e 's/[[^0-9.]]//g' | head -1`
				;;
			sunc)
				OW_COMPILER_VERSION=`$CXX -V 2>&1 | grep 'Sun C++' | sed -e 's/.*C++[[ ]]*//' -e 's/ .*//'`
				;;
			gcc)
				OW_COMPILER_VERSION=`$CXX --version 2>&1 | grep '[[0-9]][[0-9]]*\.[[0-9]][[0-9]]*' | sed -e 's/[[(]][[^)]]*[[)]]//g' -e 's/[[^0-9.]]//g'`
				OW_COMPILER_LIBDIR=`$CXX --print-file libstdc++.so`
				OW_CLEANUP_DIRECTORY_NAME(OW_COMPILER_LIBDIR, `dirname $OW_COMPILER_LIBDIR`)
				;;
			*)
				AC_MSG_WARN(Unable to detect compiler version)
				;;
		esac

		if test "x$OW_COMPILER_VERSION" != x; then
			AC_DEFINE(COMPILER_VERSION, [$OW_COMPILER_VERSION], [The compiler version used to compile OpenWBEM])
			OW_COMPILER_MAJOR=`printf '%s' "${OW_COMPILER_VERSION}" | sed 's/[[^0-9]].*//g'`
			OW_COMPILER_MINOR=`printf '%s' "${OW_COMPILER_VERSION}" | sed -e 's/^[[^0-9]]*[[0-9]]*\.//' -e 's/[[^0-9]].*//'`
			if test "x$OW_COMPILER_MINOR" = x; then
				OW_COMPILER_MINOR=0
			fi
			AC_DEFINE_UNQUOTED(COMPILER_MAJOR, [$OW_COMPILER_MAJOR], [Major version of the compiler])
			AC_DEFINE_UNQUOTED(COMPILER_MINOR, [$OW_COMPILER_MINOR], [Minor version of the compiler])
		fi
		AC_SUBST(OW_COMPILER_LIBDIR)
	]
)