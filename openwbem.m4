dnl
dnl Parts of this script are Originally 
dnl from kim-browser by mhk@kde.org 
dnl and
dnl owperlprovider by Jason Long <jason@long.name>
dnl
dnl modified by Anas Nashif <nashif@suse.de>
dnl modified by Dan Nuffer <nuffer@users.sourceforge.net>
dnl

dnl ------------------------------------------------------------------------
dnl Find a file (or one of more files in a list of dirs)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_FIND_FILE],
[
$3=NO
for i in $2;
do
	for j in $1;
	do
		echo "configure: __oline__: $i/$j" >&AC_FD_CC
		if test -r "$i/$j"; then
			echo "taking that" >&AC_FD_CC
			$3=$i
			break 2
		fi
	done
done
])

dnl ------------------------------------------------------------------------
dnl Try to find the OpenWBEM headers and libraries.
dnl ------------------------------------------------------------------------
dnl   Usage:   CHECK_OPENWBEM([REQUIRED-VERSION
dnl                      [,ACTION-IF-FOUND[,ACTION-IF-NOT-FOUND]]])
dnl
AC_DEFUN(CHECK_OPENWBEM,
[
OPENWBEM_CLIENT_LIBS="-lopenwbem -lowclient -lowxml -lowhttpcommon -lowhttpclient"
OPENWBEM_PROVIDER_LIBS="-lopenwbem -lowprovider -lowcppprovifc"

ac_OPENWBEM_includes=NO ac_OPENWBEM_libraries=NO
OPENWBEM_libraries=""
OPENWBEM_includes=""
openwbem_error=""
AC_ARG_WITH(openwbem-dir,
	[  --with-openwbem-dir=DIR      where the root of OpenWBEM is installed],
	[  ac_OPENWBEM_includes="$withval"/include
		ac_OPENWBEM_libraries="$withval"/lib${libsuff}
	])

want_OPENWBEM=yes
if test $want_OPENWBEM = yes; then

	AC_MSG_CHECKING(for OpenWBEM)

	AC_CACHE_VAL(ac_cv_have_OPENWBEM,
	[#try to guess OpenWBEM locations

		OPENWBEM_incdirs="/usr/include /usr/local/include /usr/openwbem/include /usr/local/openwbem/include $prefix/include"
		OPENWBEM_incdirs="$ac_OPENWBEM_includes $OPENWBEM_incdirs"
		AC_FIND_FILE(openwbem/OW_config.h, $OPENWBEM_incdirs, OPENWBEM_incdir)
		ac_OPENWBEM_includes="$OPENWBEM_incdir"

		OPENWBEM_libdirs="/usr/lib${libsuff} /usr/local/lib /usr/openwbem/lib /usr/local/openwbem/lib $prefix/lib $exec_prefix/lib $kde_extra_libs"
		if test ! "$ac_OPENWBEM_libraries" = "NO"; then
			OPENWBEM_libdirs="$ac_OPENWBEM_libraries $OPENWBEM_libdirs"
		fi

		test=NONE
		OPENWBEM_libdir=NONE
		for dir in $OPENWBEM_libdirs; do
			try="ls -1 $dir/libopenwbem*"
			if test=`eval $try 2> /dev/null`; then OPENWBEM_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
		done

		ac_OPENWBEM_libraries="$OPENWBEM_libdir"

		if test "$ac_OPENWBEM_includes" = NO || test "$ac_OPENWBEM_libraries" = NO; then
			have_OPENWBEM=no
			openwbem_error="OpenWBEM not found"
		else
			have_OPENWBEM=yes;
		fi

	])

	eval "$ac_cv_have_OPENWBEM"

	AC_MSG_RESULT([libraries $ac_OPENWBEM_libraries, headers $ac_OPENWBEM_includes])

else
	have_OPENWBEM=no
	openwbem_error="OpenWBEM not found"
fi

if test "$ac_OPENWBEM_includes" = "/usr/include" || test  "$ac_OPENWBEM_includes" = "/usr/local/include" || test -z "$ac_OPENWBEM_includes"; then
	OPENWBEM_INCLUDES="";
else
	OPENWBEM_INCLUDES="-I$ac_OPENWBEM_includes"
fi

if test "$ac_OPENWBEM_libraries" = "/usr/lib" || test "$ac_OPENWBEM_libraries" = "/usr/local/lib" || test -z "$ac_OPENWBEM_libraries"; then
	OPENWBEM_LDFLAGS=""
else
	OPENWBEM_LDFLAGS="-L$ac_OPENWBEM_libraries -R$ac_OPENWBEM_libraries"
fi


dnl check REQUIRED-VERSION
ifelse([$1], , [], [

	# only check version if OpenWBEM has been found
	if test "x$openwbem_error" = "x" ; then
		OPENWBEM_REQUEST_VERSION="$1"
		AC_MSG_CHECKING(OpenWBEM version)

		AC_REQUIRE([AC_PROG_EGREP])

		changequote(<<, >>)
		openwbem_version=`$EGREP "define OW_VERSION" $ac_OPENWBEM_includes/openwbem/OW_config.h 2>&1 | sed 's/.* "\([^"]*\)".*/\1/p; d'`
		openwbem_major_ver=`expr $openwbem_version : '\([0-9]\+\)[0-9.]*'`
		openwbem_minor_ver=`expr $openwbem_version : '[0-9]\+\.\([0-9]\+\)[0-9.]*'`
		openwbem_micro_ver=`expr $openwbem_version : '[0-9]\+\.[0-9]\+\.\([0-9]\+\)' "|" 0`

		openwbem_major_req=`expr $OPENWBEM_REQUEST_VERSION : '\([0-9]\+\)[0-9.]*'`
		openwbem_minor_req=`expr $OPENWBEM_REQUEST_VERSION : '[0-9]\+\.\([0-9]\+\)[0-9.]*'`
		openwbem_micro_req=`expr $OPENWBEM_REQUEST_VERSION : '[0-9]\+\.[0-9]\+\.\([0-9]\+\)' '|' 0`
		changequote([, ])
		AC_MSG_RESULT($openwbem_version)
		#echo "openwbem_major_ver=$openwbem_major_ver"
		#echo "openwbem_minor_ver=$openwbem_minor_ver"
		#echo "openwbem_micro_ver=$openwbem_micro_ver"
		#echo "openwbem_major_req=$openwbem_major_req"
		#echo "openwbem_minor_req=$openwbem_minor_req"
		#echo "openwbem_micro_req=$openwbem_micro_req"

		AC_MSG_CHECKING(requested OpenWBEM version ($OPENWBEM_REQUEST_VERSION))
		if test $openwbem_major_ver -gt $openwbem_major_req
		then
			AC_MSG_RESULT(yes)
			openwbem_version_ok=yes
		elif test $openwbem_major_ver -eq $openwbem_major_req &&
			test $openwbem_minor_ver -gt $openwbem_minor_req
		then
			AC_MSG_RESULT(yes)
			openwbem_version_ok=yes
		elif test $openwbem_major_ver -eq $openwbem_major_req &&
			test $openwbem_minor_ver -eq $openwbem_minor_req &&
			test $openwbem_micro_ver -ge $openwbem_micro_req
		then
			AC_MSG_RESULT(yes)
			openwbem_version_ok=yes
		else
			AC_MSG_RESULT(no)
			openwbem_error="Installed version of OpenWBEM header files is too old"
		fi
	fi
]) dnl End of Ifdef REQUIRED-VERSION

if test "x$openwbem_error" = "x" ; then
	dnl Successfully found,
	dnl do ACTION-IF-FOUND
	ifelse([$2], , :, [$2])
else
	dnl do ACTION-IF-NOT-FOUND
	ifelse([$3], ,
		AC_MSG_ERROR($openwbem_error),
		[$3])
fi

AC_SUBST(OPENWBEM_INCLUDES)
AC_SUBST(OPENWBEM_LDFLAGS)
AC_SUBST(OPENWBEM_CLIENT_LIBS)
AC_SUBST(OPENWBEM_PROVIDER_LIBS)
])



