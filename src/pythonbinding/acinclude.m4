dnl AC_PATH_BOOST([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl Test for the Boost C++ libraries of a particular version (or newer)
dnl Defines:
dnl   BOOST_CXXFLAGS to the set of flags required to compiled Boost
AC_DEFUN([AC_PATH_BOOST], 
[
  BOOST_CXXFLAGS=""
  path_given="no"

dnl Extract the path name from a --with-boost=PATH argument
  AC_ARG_WITH(boost,
    [  --with-boost=PATH absolute path name where the Boost C++ libraries
    reside. Alternatively, the BOOST_ROOT environment variable will be used],
    if test "$withval" = no ; then
	path_given="no"
	BOOST_CXXFLAGS=""
    else
      if test "$withval" != yes ; then
        path_given="yes"
        BOOST_CXXFLAGS="-I$withval -DHAVE_BOOST"
        BOOST_ROOT=$withval
      fi
    fi    
  )

dnl If no path with given and there is a BOOST_ROOT environment variable,
dnl use it
  if test "$path_given" = "no"; then
    if test "x$BOOST_ROOT" = "x"; then
      BOOST_CXXFLAGS=""
    else
      BOOST_CXXFLAGS="-I$BOOST_ROOT -DHAVE_BOOST"
    fi
  fi

  boost_min_version=ifelse([$1], ,1.20.0,$1)

  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  OLD_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $BOOST_CXXFLAGS"
  AC_MSG_CHECKING([for the Boost C++ libraries, version $boost_min_version or newer])
  AC_TRY_COMPILE(
    [
#include <boost/version.hpp>
],
    [],
    [
      have_boost="yes"
    ],
    [
      AC_MSG_RESULT(no)
      have_boost="no"
      ifelse([$3], , :, [$3])
    ])

  if test "$have_boost" = "yes"; then
    WANT_BOOST_MAJOR=`expr $boost_min_version : '\([[0-9]]\+\)'`
    WANT_BOOST_MINOR=`expr $boost_min_version : '[[0-9]]\+\.\([[0-9]]\+\)'`
    WANT_BOOST_SUB_MINOR=`expr $boost_min_version : '[[0-9]]\+\.[[0-9]]\+\.\([[0-9]]\+\)'`
    WANT_BOOST_VERSION=`expr $WANT_BOOST_MAJOR \* 100000 \+ $WANT_BOOST_MINOR \* 100 \+ $WANT_BOOST_SUB_MINOR`

    AC_TRY_COMPILE(
      [
#include <boost/version.hpp>
],
      [
#if BOOST_VERSION >= $WANT_BOOST_VERSION
// Everything is okay
#else
#  error Boost version is too old
#endif

],
      [
        AC_MSG_RESULT(yes)
        ifelse([$2], , :, [$2])
      ],
      [
        AC_MSG_RESULT([no, version of installed Boost libraries is too old])
        ifelse([$3], , :, [$3])
      ])
  fi
  CXXFLAGS="$OLD_CXXFLAGS"
  AC_LANG_RESTORE
])



dnl AC_PATH_PYTHON([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl Test for the Python headers of a particular version (or newer)
dnl Defines:
dnl   PYTHON_CXXFLAGS to the set of flags required to compile code that uses Python
AC_DEFUN([AC_PATH_PYTHON], 
[
  PYTHON_CXXFLAGS=""
  path_given="no"

dnl Extract the path name from a --with-python=PATH argument
  AC_ARG_WITH(python,
    [  --with-python=PATH absolute path name where the Python library
    reside. Alternatively, the PYTHON_ROOT environment variable will be used],
    if test "$withval" = no ; then
	path_given="no"
	PYTHON_CXXFLAGS=""
    else
      if test "$withval" != yes ; then
        path_given="yes"
        PYTHON_CXXFLAGS="-I$withval -DHAVE_PYTHON"
        PYTHON_ROOT=$withval
      fi
    fi    
  )

dnl If no path with given and there is a PYTHON_ROOT environment variable,
dnl use it
  if test "$path_given" = "no"; then
    if test "x$PYTHON_ROOT" = "x"; then
      PYTHON_CXXFLAGS=""
    else
      PYTHON_CXXFLAGS="-I$PYTHON_ROOT -DHAVE_PYTHON"
    fi
  fi

  python_min_version=ifelse([$1], ,2.0.0,$1)

  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  OLD_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $PYTHON_CXXFLAGS"
  AC_MSG_CHECKING([for the Python library, version $python_min_version or newer])
  AC_TRY_COMPILE(
    [
#include <Python.h>
],
    [],
    [
      have_python="yes"
    ],
    [
      AC_MSG_RESULT(no)
      have_python="no"
      ifelse([$3], , :, [$3])
    ])

  if test "$have_python" = "yes"; then
    WANT_PYTHON_MAJOR=`expr $python_min_version : '\([[0-9]]\+\)'`
    WANT_PYTHON_MINOR=`expr $python_min_version : '[[0-9]]\+\.\([[0-9]]\+\)'`
    WANT_PYTHON_SUB_MINOR=`expr $python_min_version : '[[0-9]]\+\.[[0-9]]\+\.\([[0-9]]\+\)'`

    AC_TRY_COMPILE(
      [
#include <patchlevel.h>
],
      [
#define WANT_PYTHON_VERSION (($WANT_PYTHON_MAJOR << 24) | \
    ($WANT_PYTHON_MINOR << 16 ) | \
    ($WANT_PYTHON_SUB_MINOR << 8 ))
#if PY_VERSION_HEX >= WANT_PYTHON_VERSION
// Everything is okay
#else
#  error Python version is too old
#endif

],
      [
        AC_MSG_RESULT(yes)
        ifelse([$2], , :, [$2])
      ],
      [
        AC_MSG_RESULT([no, version of installed Python library is too old])
        ifelse([$3], , :, [$3])
      ])
  fi
  CXXFLAGS="$OLD_CXXFLAGS"
  AC_LANG_RESTORE
])
