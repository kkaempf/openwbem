
dnl $1=checking message
dnl $2=link flags
AC_DEFUN(
	[OW_KRB5_LIB_HELPER],
	[
		if test -z "$KRB5_LDFLAGS"; then
			OW_PUSH_VARIABLE(LIBS)
			AC_MSG_CHECKING(for $1 krb5 libs)
			LIBS="$LIBS $2 $LIB_resolv"
			AC_RUN_IFELSE(
				AC_LANG_PROGRAM(
					[
						#include <krb5.h>
					],
					[
						krb5_context foo;
						int val = krb5_init_context(&foo);
						if( val == 0 )
						{
							krb5_free_context(foo);
						}
						return val;
					]
				),
				[
					dnl Note the intential spaces in here to cause the flags to be
					dnl non-empty even if the argument is empty.
					KRB5_LDFLAGS=" $2 "
					AC_MSG_RESULT(yes... $2)
				],
				[
					AC_MSG_RESULT([no (tried $2)])
				],
				[
					AC_MSG_RESULT(no)
				]
			)
			OW_POP_VARIABLE(LIBS)
		fi
	]
)

AC_DEFUN(
	[OW_KRB5_SUPPORT],
	[
		dnl Save flags for later cleanup
		OW_SAVE_FLAGS

		KRB5_DISABLED=0
		KRB5_LOCATION=""
		AC_ARG_WITH(krb5,
			AC_HELP_STRING([--with-krb5=dir],[use krb5 in dir]),
			[
				if test "x$withval" != "xno" && test "x$withval" != xyes; then
					KRB5_LOCATION="$withval"
				elif test "x$withval" = xno; then
					dnl Specified with the --without syntax.
					KRB5_DISABLED=1
				fi
			]
		)

		if test x$KRB5_DISABLED != x1; then
			dnl
			dnl Find krb5 headers.
			dnl
			krb5_config_command=""
			AC_MSG_CHECKING(for krb5-config)
			if test -n "$KRB5_LOCATION" -a -x "$KRB5_LOCATION/bin/krb5-config"; then
				krb5_config_command="$KRB5_LOCATION/bin/krb5-config"
			elif which krb5-config >/dev/null 2>&1; then
				if test -x "`which krb5-config`"; then
					krb5_config_command="`which krb5-config`"
				fi
			fi
			AC_MSG_RESULT(${krb5_config_command:-NONE})

			if test -z "$KRB5_CPPFLAGS"; then
				TMP_KRB5_CPPFLAGS=""
				if test -n "$krb5_config_command"; then
					TMP_KRB5_CPPFLAGS=" `$krb5_config_command --cflags krb5 2>/dev/null` "
				else
					for possible_location in /usr/include /usr/local/include; do
						if test -z "$TMP_KRB5_CPPFLAGS"; then
							OW_PUSH_VARIABLE(CPPFLAGS)
							CPPFLAGS="$CPPFLAGS -I${possible_location}"
							AC_CHECK_HEADER(krb5.h, [ TMP_KRB5_CPPFLAGS="-I$possible_location" ])
							OW_POP_VARIABLE(CPPFLAGS)
						fi
					done
				fi
				if test -z "$TMP_KRB5_CPPFLAGS" -a -n "$KRB5_location"; then
					TMP_KRB5_CPPFLAGS="-I$KRB5_LOCATION/include"
				fi
				CPPFLAGS="$CPPFLAGS $TMP_KRB5_CPPFLAGS"
				AC_CHECK_HEADER(krb5.h, [ KRB5_CPPFLAGS="$TMP_KRB5_CPPFLAGS" ])
			fi

			dnl
			dnl Check if the krb5 is heimdal
			dnl
			AC_CHECK_DECLS(heimdal_version,
				[AC_DEFINE_UNQUOTED(HEIMDAL, 1, [Heimdal is used for krb5])],
				[],
				[#include <krb5.h>]
			)

			dnl Print out some useful stuff
			AC_MSG_CHECKING(for krb5 preprocessor flags)
			AC_MSG_RESULT(${KRB5_CPPFLAGS:-NONE})

			dnl
			dnl Find krb5 libraries
			dnl

			dnl Using the krb5-config command
			if test -n "$krb5_config_command"; then
				TMP_KRB5_LDFLAGS=`$krb5_config_command --libs krb5 2>/dev/null`
				OW_KRB5_LIB_HELPER(krb5-config,$TMP_KRB5_LDFLAGS)
			fi

			if test -n "$KRB5_LOCATION"; then
				dnl Using MIT kerberos libs
				OW_KRB5_LIB_HELPER(mit-style,-L$KRB5_LOCATION/lib -lkrb5 -lk5crypto -lcom_err)
				dnl Using Heimdal
				OW_KRB5_LIB_HELPER(Heimdal,-L$KRB5_LOCATION/lib -lkrb5 -lasn1 -lcrypto -lroken -lcrypt)
				dnl Heimdal on OpenBSD
				OW_KRB5_LIB_HELPER(Heimdal (OpenBSD style),-L$KRB5_LOCATION/lib -lkrb5 -lasn1 -ldes -lcrypto)
			fi

			AC_MSG_CHECKING(krb5 link flags)
			AC_MSG_RESULT(${KRB5_LDFLAGS:-NONE})
		fi

		dnl Clean up
		OW_RESTORE_FLAGS
		AC_SUBST(KRB5_CPPFLAGS)
		AC_SUBST(KRB5_LDFLAGS)

		HAVE_KRB5=0
		if test x"$KRB5_CPPFLAGS" != x || test x"$KRB5_LDFLAGS" != x; then
			HAVE_KRB5=1
			AC_DEFINE_UNQUOTED(HAVE_KRB5, 1, [Have krb5])
		fi
		AM_CONDITIONAL(HAVE_KRB, test x$HAVE_KRB5 = x1)
	]
)
