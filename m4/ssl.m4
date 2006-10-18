AC_DEFUN(
	[OW_SSL_SUPPORT],
	[
		# The big search for OpenSSL.  Thanks to the openssh configure.in file!
		saved_LDFLAGS="$LDFLAGS"
		saved_CPPFLAGS="$CPPFLAGS"
		saved_LIBS="$LIBS"

		AC_ARG_WITH(ssl-dir,
			OW_HELP_STRING(--with-ssl-dir=PATH,specify path to OpenSSL installation),
		[
			if test "x$withval" != "xno" ; then
				tryssldir=$withval
			fi
		]
		)

		if test "x$prefix" != "xNONE" ; then
			tryssldir="$tryssldir $prefix"
		fi
		AC_CACHE_CHECK([for OpenSSL directory], ac_cv_openssldir, [
			for ssldir in $tryssldir "" /usr/local/openssl /usr/lib/openssl /usr/local/ssl /usr/lib/ssl /usr/local /usr/pkg /opt /opt/openssl ; do
				CPPFLAGS="$saved_CPPFLAGS"
				LDFLAGS="$saved_LDFLAGS"
				SSL_LIBS="-lssl -lcrypto"
				LIBS="$saved_LIBS $SSL_LIBS"

				# Skip directories if they don't exist
				if test ! -z "$ssldir" -a ! -d "$ssldir" ; then
					continue;
				fi
				if test ! -z "$ssldir" -a "x$ssldir" != "x/usr"; then
					# Try to use $ssldir/lib if it exists, otherwise
					# $ssldir
					if test -d "$ssldir/lib" ; then
						LDFLAGS="$saved_LDFLAGS -L$ssldir/lib"
						if test "x$RPATH_LINK_FLAG_REQUIRED" = "x1" ; then
							LDFLAGS="$LDFLAGS ${RPATH_LINK_FLAG}$ssldir/lib"
						fi
					else
						LDFLAGS="$saved_LDFLAGS -L$ssldir"
						if test "x$RPATH_LINK_FLAG_REQUIRED" = "x1" ; then
							LDFLAGS="$LDFLAGS ${RPATH_LINK_FLAG}$ssldir"
						fi
					fi
					# Try to use $ssldir/include if it exists, otherwise
					# $ssldir
					if test -d "$ssldir/include" ; then
						CPPFLAGS="$saved_CPPFLAGS -I$ssldir/include"
					else
						CPPFLAGS="$saved_CPPFLAGS -I$ssldir"
					fi
				fi

				# Basic test to check for compatible version and correct linking
				# *does not* test for RSA - that comes later.
				AC_TRY_COMPILE(
				[
		#include <string.h>
		#include <openssl/rand.h>
				],
				[
			char a[2048];
			memset(a, 0, sizeof(a));
			RAND_add(a, sizeof(a), sizeof(a));
			return(RAND_status() <= 0);
				],
				[
					found_crypto=1
					break;
				],
				[
					did_not_find_crypt=0
				]
				)

				if test ! -z "$found_crypto" ; then
					break;
				fi
			done

			if test -z "$found_crypto" ; then
				#AC_MSG_ERROR([Could not find working OpenSSL library, please install or check config.log])
				ssldir=""
			fi
			if test -z "$ssldir" && test ! -z "$found_crypto"; then
				ssldir="(system)"
			fi

			ac_cv_openssldir=$ssldir
		])

		if (test ! -z "$ac_cv_openssldir") ; then
			AC_DEFINE(HAVE_OPENSSL)
			SSL_LIBS="-lssl -lcrypto"
			LIBS="$saved_LIBS"
		else
			AC_MSG_ERROR([OpenSSL not found])
		fi

		if (test ! -z "$ac_cv_openssldir" && test "x$ac_cv_openssldir" != "x(system)") ; then
			dnl Need to recover ssldir - test above runs in subshell
			ssldir=$ac_cv_openssldir
			if test ! -z "$ssldir" -a "x$ssldir" != "x/usr"; then
				# Try to use $ssldir/lib if it exists, otherwise
				# $ssldir
				if test -d "$ssldir/lib" ; then
					LDFLAGS="$saved_LDFLAGS -L$ssldir/lib"
					if test "x$RPATH_LINK_FLAG_REQUIRED" = "x1" ; then
						LDFLAGS="$LDFLAGS ${RPATH_LINK_FLAG}$ssldir/lib"
					fi
				else
					LDFLAGS="$saved_LDFLAGS -L$ssldir"
					if test "x$RPATH_LINK_FLAG_REQUIRED" = "x1" ; then
						LDFLAGS="$LDFLAGS ${RPATH_LINK_FLAG}$ssldir"
					fi
				fi
				# Try to use $ssldir/include if it exists, otherwise
				# $ssldir
				if test -d "$ssldir/include" ; then
					CPPFLAGS="$saved_CPPFLAGS -I$ssldir/include"
				else
					CPPFLAGS="$saved_CPPFLAGS -I$ssldir"
				fi
			fi
			SSL_LIBS="-lssl -lcrypto"
		fi

		AC_SUBST(SSL_LIBS)

		dnl End of the ssl check
	]
)
