dnl @synopsis AC_CREATE_PREFIX_CONFIG_H [(OUTPUT-HEADER [,PREFIX [,ORIG-HEADER]])]
dnl
dnl This is a new variant from ac_prefix_config_
dnl This one will prefix all defines starting with an uppercase character with
dnl the supplied prefix.  Lowercase defines and defines starting with an
dnl underscore will be wrapped in an ifdef but will not be prefixed.
dnl
dnl An added bonus is a sanity check on all non-prefixed numeric constants.  If
dnl the value is used from a -D<value> option or comes from a system header, it
dnl is checked to ensure that the value has not changed, causing problems.
dnl An example of such a problem would be _FILE_OFFSET_BITS not matching what the
dnl library was compiled to use, causing structures to have incorrect sizes and
dnl bizarre runtime errors.
dnl
dnl
dnl takes the usual config.h generated header file; looks for each of
dnl the generated "#define SOMEDEF" lines, and prefixes the defined name
dnl (ie. makes it "#define PREFIX_SOMEDEF". The result is written to
dnl the output config.header file. The PREFIX is converted to uppercase
dnl for the conversions.
dnl
dnl default OUTPUT-HEADER = $PACKAGE-config.h
dnl default PREFIX = $PACKAGE
dnl default ORIG-HEADER, derived from OUTPUT-HEADER
dnl         if OUTPUT-HEADER has a "/", use the basename
dnl         if OUTPUT-HEADER has a "-", use the section after it.
dnl         otherwise, just config.h
dnl
dnl In most cases, the configure.in will contain a line saying
dnl         AC_CONFIG_HEADER(config.h)
dnl somewhere *before* AC_OUTPUT and a simple line saying
dnl        AC_PREFIX_CONFIG_HEADER
dnl somewhere *after* AC_OUTPUT.
dnl
dnl example:
dnl   AC_INIT(config.h.in)        # config.h.in as created by "autoheader"
dnl   AM_INIT_AUTOMAKE(testpkg, 0.1.1)   # "#undef VERSION" and "PACKAGE"
dnl   AM_CONFIG_HEADER(config.h)         #                in config.h.in
dnl   AC_MEMORY_H                        # "#undef NEED_MEMORY_H"
dnl   AC_C_CONST_H                       # "#undef const"
dnl   AC_OUTPUT(Makefile)                # creates the "config.h" now
dnl   AC_CREATE_PREFIX_CONFIG_H          # creates "testpkg-config.h"
dnl         and the resulting "testpkg-config.h" contains lines like
dnl   #ifndef TESTPKG_VERSION
dnl   #define TESTPKG_VERSION "0.1.1"
dnl   #endif
dnl   #ifndef TESTPKG_NEED_MEMORY_H
dnl   #define TESTPKG_NEED_MEMORY_H 1
dnl   #endif
dnl   #ifndef testpkg_const
dnl   #define testpkg_const const
dnl   #endif
dnl   #ifndef _SOME_DEFINE
dnl   #define _SOME_DEFINE 1234
dnl   #elif _SOME_DEFINE != 1234
dnl   #error Sanity check fail, _SOME_DEFINE != 1234
dnl   #endif
dnl
dnl   and this "testpkg-config.h" can be installed along with other
dnl   header-files, which is most convenient when creating a shared
dnl   library (that has some headers) where some functionality is
dnl   dependent on the OS-features detected at compile-time. No
dnl   need to invent some "testpkg-confdefs.h.in" manually. :-)
dnl
dnl @author Guido Draheim <guidod@gmx.de>
dnl Modified by Kevin Harris

AC_DEFUN([AC_CREATE_PREFIX_CONFIG_H],
[
	changequote({, })dnl
	ac_prefix_conf_OUT=`echo ifelse($1, , $PACKAGE-config.h, $1)`
	ac_prefix_conf_OUTTMP="$ac_prefix_conf_OUT"tmp
	ac_prefix_conf_DEF=`echo $ac_prefix_conf_OUT | sed -e 'y:abcdefghijklmnopqrstuvwxyz./,-:ABCDEFGHIJKLMNOPQRSTUVWXYZ____:'`
	ac_prefix_conf_PKG=`echo ifelse($2, , $PACKAGE, $2)`
	ac_prefix_conf_UPP=`echo $ac_prefix_conf_PKG | sed -e 'y:abcdefghijklmnopqrstuvwxyz-:ABCDEFGHIJKLMNOPQRSTUVWXYZ_:'  -e '/^[0-9]/s/^/_/'`
	ac_prefix_conf_INP=`echo ifelse($3, , _, $3)`
	ac_prefix_conf_SED=conftest.sed
	if test "$ac_prefix_conf_INP" = "_"; then
		case $ac_prefix_conf_OUT in
			*/*) _INP=`basename $ac_prefix_conf_OUT` ;;
			*-*) _INP=`echo $ac_prefix_conf_OUT | sed -e 's/[a-zA-Z0-9_]*-//'` ;;
			*) _INP=config.h ;;
		esac
	fi
	changequote([, ])dnl
	if test -z "$ac_prefix_conf_PKG" ; then
		AC_MSG_ERROR([no prefix for _PREFIX_PKG_CONFIG_H])
	else
		AC_MSG_RESULT(creating $ac_prefix_conf_OUT - prefix $ac_prefix_conf_UPP for $ac_prefix_conf_INP defines)
		if test -f $ac_prefix_conf_INP ; then
			changequote({, })dnl
			printf '%s\n' '/* automatically generated */' > $ac_prefix_conf_OUTTMP
			printf '%s\n' '#ifndef '$ac_prefix_conf_DEF >>$ac_prefix_conf_OUTTMP
			printf '%s\n' '#define '$ac_prefix_conf_DEF' 1' >>$ac_prefix_conf_OUTTMP
			printf '\n' >>$ac_prefix_conf_OUTTMP
			printf '%s\n' "/* $ac_prefix_conf_OUT. Generated automatically at end of configure. */" >>$ac_prefix_conf_OUTTMP

			printf '%s\n' 's/#undef  *\([A-Z]\)/#undef '$ac_prefix_conf_UPP'_\1/' >$ac_prefix_conf_SED

			#
			# Prefixed #define tweaking (no lowercase or leading underscores)
			#
			printf '%s\n' 's/#define  *\([A-Z][A-Z0-9_]*\)\(.*\)/#ifndef '"$ac_prefix_conf_UPP"'_\1\' >>$ac_prefix_conf_SED
			printf '%s\n' '#define '"$ac_prefix_conf_UPP"'_\1\2\' >>$ac_prefix_conf_SED
			printf '%s\n' '#endif/' >>$ac_prefix_conf_SED

			#
			# Non-prefixed #define tweaking (for lowercase identifiers or leading underscores)
			#

			# Wrapped, no defined value.
			printf '%s\n' 's/#define[ 	]*\([a-z_][a-zA-Z0-9_]*\)[ 	]*$/#ifndef \1\' >> $ac_prefix_conf_SED
			printf '%s\n' '#define \1\' >> $ac_prefix_conf_SED
			printf '%s\n' '#endif/' >> $ac_prefix_conf_SED

			# Wrapped in #ifndef, but no sanity check (non-numeric)
			printf '%s\n' 's/#define[ 	]*\([a-z_][a-zA-Z0-9_]*\)[ 	]\+\([^0-9].*\)/#ifndef \1\' >> $ac_prefix_conf_SED
			printf '%s\n' '#define \1 \2\' >> $ac_prefix_conf_SED
			printf '%s\n' '#endif/' >> $ac_prefix_conf_SED

			# Wrap with a sanity checked version
			printf '%s\n' 's/#define[ 	]*\([a-z_][a-zA-Z0-9_]*\)[ 	]\+\([0-9]\+\)/#ifndef \1\' >> $ac_prefix_conf_SED
			printf '%s\n' '#define \1 \2\' >> $ac_prefix_conf_SED
			printf '%s\n' '#elif \1 != \2\' >> $ac_prefix_conf_SED
			printf '%s\n' '#error Sanity check fail, \1 != \2\' >> $ac_prefix_conf_SED
			printf '%s\n' '#endif/' >> $ac_prefix_conf_SED

			sed -f $ac_prefix_conf_SED $ac_prefix_conf_INP >>$ac_prefix_conf_OUTTMP
			printf '\n' >>$ac_prefix_conf_OUTTMP
			printf '%s\n' "/* $ac_prefix_conf_DEF */" >>$ac_prefix_conf_OUTTMP
			printf '%s\n' '#endif' >>$ac_prefix_conf_OUTTMP
			if cmp -s $ac_prefix_conf_OUT $ac_prefix_conf_OUTTMP 2>/dev/null ; then
				echo "$ac_prefix_conf_OUT is unchanged"
				rm -f $ac_prefix_conf_OUTTMP
			else
				rm -f $ac_prefix_conf_OUT
				mv $ac_prefix_conf_OUTTMP $ac_prefix_conf_OUT
			fi
			changequote([, ])dnl
		else # ! -f $ac_prefix_conf_INP
			AC_MSG_ERROR([input file $ac_prefix_conf_IN does not exist, skip generating $ac_prefix_conf_OUT])
		fi
		rm -f conftest.*
	fi # ! -z $ac_prefix_conf_PKG
])

AC_DEFUN([OW_APPLY_PREFIX_TO_CONFIG_H],
	[
		AC_CONFIG_COMMANDS(
			[$1],
			[AC_CREATE_PREFIX_CONFIG_H($1,$2,$3)],
			[PACKAGE=$PACKAGE]
		)
	]
)
