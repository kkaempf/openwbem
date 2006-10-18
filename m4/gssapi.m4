dnl Check for gssapi libs with spnego support (just a helper)
dnl $1=checking message
dnl $2=link flags
AC_DEFUN(
	[OW_GSSAPI_LIB_HELPER],
	[
		if test -z "$GSSAPI_LIBS"; then
			OW_SAVE_FLAGS
			OW_PUSH_VARIABLE(LIBS)
			AC_MSG_CHECKING(for $1 gssapi libs)
			LIBS="$LIBS $2 $LIB_resolv"
			AC_RUN_IFELSE(
				AC_LANG_PROGRAM(
					[
						#include <string.h>
						#include <krb5.h>
						#ifdef HEIMDAL
						#include <gssapi.h>
						#else
						#include <gssapi/gssapi.h>
						#endif
 					],
					[
						OM_uint32 major_status;
						OM_uint32 minor_status;
						gss_OID_set mech_set;

						/* The [,] is needed on the next line because m4
							incorrectly determines that the comma belongs to
							the outer test and ends the program there.  This
							causes both compilation and configure
							problems. Be careful if you change this.
						*/
						gss_OID_desc spnego_oid_desc = {6 [,] (void *)"\x2b\x06\x01\x05\x05\x02"};
						int SPNEGO = 0;

						major_status = gss_indicate_mechs(&minor_status, &mech_set);
						if (GSS_ERROR(major_status))
						{
							return 1;
						}
						else
						{
							unsigned int i;
							for (i=0; i < mech_set->count && !SPNEGO; i++)
							{
								gss_OID tmp_oid = &mech_set->elements[[[i]]];
								if (tmp_oid->length == spnego_oid_desc.length &&
									!memcmp(tmp_oid->elements, spnego_oid_desc.elements,
										tmp_oid->length))
								{
									SPNEGO = 1;
									break;
								}
							}
							gss_release_oid_set(&minor_status, &mech_set);
							if (SPNEGO)
							{
								return 0;
							}
							else
							{
								return 1;
							}
						}
					]
				),
				[
					dnl Note the intential spaces in here to cause the flags to be
					dnl non-empty even if the argument is empty.
					GSSAPI_LIBS=" $2 "
					AC_MSG_RESULT(yes... $2)
					AC_DEFINE([GSSAPI_SUPPORTS_SPNEGO], TRUE, [GSSAPI_SUPPORTS_SPNEGO])
					gssapi_supports_spnego=yes
				],
				[
					AC_MSG_RESULT([no (tried $2)])
				],
				[
					AC_MSG_RESULT(no)
				]
			)
			OW_POP_VARIABLE(LIBS)
			OW_RESTORE_FLAGS
		fi
	]
)

AC_DEFUN(
	[OW_GSSAPI_SUPPORT],
	[
		dnl Save flags for later cleanup
		OW_SAVE_FLAGS
		CPPFLAGS="$CPPFLAGS $KRB5_CPPFLAGS"

		dnl
		dnl Find gssapi headers.
		dnl
		if test -z "$GSSAPI_CPPFLAGS"; then
			TMP_GSSAPI_CPPFLAGS=""
			if test -n "$krb5_config_command"; then
				TMP_GSSAPI_CPPFLAGS=" `$krb5_config_command --cflags gssapi 2>/dev/null` "
			else
				for possible_location in $KRB5_LOCATION /usr/include /usr/local/include; do
					if test -z "$TMP_GSSAPI_CPPFLAGS"; then
						OW_PUSH_VARIABLE(CPPFLAGS)
						CPPFLAGS="$CPPFLAGS -I${possible_location}"
						AC_CHECK_HEADER(gssapi.h, [ TMP_GSSAPI_CPPFLAGS="-I$possible_location" ])
						OW_POP_VARIABLE(CPPFLAGS)
					fi
				done
			fi
			if test -z "$TMP_GSSAPI_CPPFLAGS" -a -n "$GSSAPI_location"; then
				TMP_GSSAPI_CPPFLAGS="-I$GSSAPI_LOCATION/include"
			fi
			CPPFLAGS="$CPPFLAGS $TMP_GSSAPI_CPPFLAGS"
			AC_CHECK_HEADER(gssapi.h, [ GSSAPI_CPPFLAGS="$TMP_GSSAPI_CPPFLAGS" ])
		fi

		AC_MSG_CHECKING(for gssapi preprocessor flags)
		AC_MSG_RESULT(${GSSAPI_CPPFLAGS:-NONE})

		dnl
		dnl Find gssapi libraries
		dnl

		dnl Using the krb5-config command
		if test -n "$krb5_config_command"; then
			TMP_GSSAPI_LIBS=`$krb5_config_command --libs gssapi 2>/dev/null`
			OW_GSSAPI_LIB_HELPER(krb5-config,$TMP_GSSAPI_LIBS)
		fi

		if test -n "$KRB5_LOCATION"; then
			dnl Using MIT kerberos libs
			OW_GSSAPI_LIB_HELPER(mit-style,-L$KRB5_LOCATION/lib -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err)
			dnl Using Heimdal
			OW_GSSAPI_LIB_HELPER(Heimdal,-L$KRB5_LOCATION/lib -lgssapi -lkrb5 -lasn1 -lcrypto -lroken -lcrypt)
			dnl Heimdal on OpenBSD
			OW_GSSAPI_LIB_HELPER(Heimdal (OpenBSD style),-L$KRB5_LOCATION/lib -lgssapi -lkrb5 -lasn1 -ldes -lcrypto)
		fi

		AC_MSG_CHECKING(gssapi link flags)
		AC_MSG_RESULT(${GSSAPI_LIBS:-NONE})

		dnl Clean up
		OW_RESTORE_FLAGS
		AC_SUBST(GSSAPI_CPPFLAGS)
		AC_SUBST(GSSAPI_LIBS)

		AM_CONDITIONAL(HAVE_SPNEGO_SUPPORT, test x"$gssapi_supports_spnego" = xyes)
	]
)
