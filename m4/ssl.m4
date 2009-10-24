# An internal portion of the OW_SSL_SUPPORT macro that is used multiple times.
AC_DEFUN([OW_SSL_INTERNAL_DIRCHECK],
[
	ssldir="$1"
	if test ! -z "$ssldir" -a "x$ssldir" != "x/usr"; then
		# Try to use $ssldir/lib if it exists, otherwise
		# $ssldir
		ssl_libdir="$ssldir"
		if test -d "$ssldir/lib" ; then
			ssl_libdir="$ssldir/lib"
		fi
		LDFLAGS="$saved_LDFLAGS -L$ssl_libdir"

		# Try to use $ssldir/include if it exists, otherwise
		# $ssldir
		ssl_include_dir="$ssldir"
		if test -d "$ssldir/include" ; then
			ssl_include_dir="$ssldir/include"
		fi
		CPPFLAGS="$saved_CPPFLAGS -I$ssl_include_dir"
	fi
])

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
				OW_SSL_INTERNAL_DIRCHECK($ssldir)
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
				ssl_lib_dir=""
				ssl_include_dir=""
			fi
			if test -z "$ssldir" && test ! -z "$found_crypto"; then
				ssldir="(system)"
			fi

			ac_cv_openssldir=$ssldir
		])

		if (test ! -z "$ac_cv_openssldir") ; then
			AC_DEFINE(HAVE_OPENSSL)
			SSL_LIBS="-lssl -lcrypto"
			SSL_LIB_DIR="$ssl_libdir"
			LIBS="$saved_LIBS"
		else
			AC_MSG_ERROR([OpenSSL not found])
		fi

		if (test ! -z "$ac_cv_openssldir" && test "x$ac_cv_openssldir" != "x(system)") ; then
			dnl Need to recover ssldir - test above runs in subshell
			ssldir=$ac_cv_openssldir

			OW_SSL_INTERNAL_DIRCHECK($ssldir)
			SSL_LIB_DIR="$ssl_libdir"
			SSL_LIBS="-lssl -lcrypto"
		fi

		AC_ARG_WITH(ssl-rpath,
			OW_HELP_STRING(--with-ssl-rpath=PATH,add the OpenSSL directory to the runtime link path),
		[
			if test "x$withval" != "xno" ; then
				use_ssl_rpath_dir=$withval
				if test "x$withval" = "xyes" ; then
					# Use the path we have already located.
					use_ssl_rpath_dir="${ssl_lib_dir}"
				fi
				# If the rpath link flags have already been set, append to them.
				if test "x$RPATH_LINK_FLAG_WITH_PATH" != "x"; then
					RPATH_LINK_FLAG_WITH_PATH="${RPATH_LINK_FLAG_WITH_PATH}${RPATH_SEPARATOR}${use_ssl_rpath_dir}"
				fi
				# Alter the default rpath.
				if test "x${RPATH_DEFAULT_PATH}" != "x" ; then
					RPATH_DEFAULT_PATH="${use_ssl_rpath_dir}${RPATH_SEPARATOR}${RPATH_DEFAULT_PATH}"
				else
					RPATH_DEFAULT_PATH="${use_ssl_rpath_dir}"
				fi
			fi
		]
		)

		AC_SUBST(SSL_LIBS)
		AC_SUBST(SSL_LIB_DIR)

		dnl End of the ssl check
	]
)
