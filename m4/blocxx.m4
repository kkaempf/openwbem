# This is a macro that should contain pretty much everything needed to check
# for blocxx and set up the BLOCXX_LIBS autoconf substitution.

AC_DEFUN(
	[OW_BLOCXX_SUPPORT],
	[
		BLOCXX_LOCATION=
		AC_ARG_WITH(blocxx,
			OW_HELP_STRING(--with-blocxx=PATH,specify the directory to look for BloCxx include/ and lib/ dirs),
			[
				if test "x$withval" != "xno" ; then
					BLOCXX_LOCATION=$withval
					CPPFLAGS="$CPPFLAGS -I$BLOCXX_LOCATION/include"
				else
					AC_MSG_ERROR([The blocxx requirement cannot be disabled.])
				fi
			]
		)

		AC_CHECK_HEADER([blocxx/BLOCXX_config.h], [], AC_MSG_ERROR([You must install BloCxx]))

		dnl If we have got a blocxx location, we can snag the required libs from its pc file.

		dnl Note: This snags it directly from the pc file instead of from
		dnl pkg-config, because blocxx installs the pc file on all platforms, but
		dnl pkg-config is mostly linux-specific.
		if test "x$BLOCXX_LOCATION" != x && test -f "$BLOCXX_LOCATION/lib/pkgconfig/blocxx.pc"; then
			BLOCXX_LIBS="`cat $BLOCXX_LOCATION/lib/pkgconfig/blocxx.pc | grep -i \"^libs:\" | sed \"s/^[^:]*://g\"`"
		fi

		dnl If we don't know the libs, we'll need to figure them out.
		if test "x$BLOCXX_LIBS" = "x"; then
			dnl Find out if blocxx was built with iconv support.
			BLOCXX_USES_ICONV=0
			AC_MSG_CHECKING(if blocxx has iconv support)
			AC_RUN_IFELSE(
				AC_LANG_SOURCE(
					[
						#include <blocxx/BLOCXX_config.h>
						int main()
						{
						#ifdef BLOCXX_HAVE_ICONV_SUPPORT
							return 0;
						#else
							return 1;
						#endif
						}
					]
				),
				[
					AC_MSG_RESULT(yes)
					BLOCXX_USES_ICONV=1
				],
				[
					AC_MSG_RESULT(no)
					BLOCXX_USES_ICONV=0
				]
			)

			if test "x$BLOCXX_USES_ICONV" = x1; then
				dnl Check the libraries needed for iconv.  On many (most?) platforms, this will
				dnl be nothing, and the -lc will not be used.  On other platforms, -liconv is
				dnl needed.
				LIBS_icsaved="$LIBS"
				AC_CHECK_LIB(c, iconv_open,
					[
						dnl Use whatever was defined previously for ICONV_LIB.  This should be
						dnl empty, unless explicitly overridden.
						ICONV_LIB="$ICONV_LIB"
					],
					[
						AC_CHECK_LIB(iconv, iconv_open,
							[ ICONV_LIB="-liconv" ],
						)
					]
				)
				LIBS="$LIBS_icsaved"
			else
				dnl It was not linked with iconv, so we do not need it either.
				ICONV_LIB=""
			fi
			BLOCXX_LIBS="-lblocxx $ICONV_LIB"
		fi
		AC_SUBST(ICONV_LIB)
		AC_SUBST(BLOCXX_LIBS)
	]
)
