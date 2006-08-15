dnl Include another m4 macro file into the configure script.
dnl $1 = filename (relative to the top source directory)
dnl
dnl Note the hack that inserts the included m4 as a dependency so
dnl maintainer-mode rebuilds will re-execute autoconf whenever any of
dnl these m4 files is touched.
AC_DEFUN([OW_INCLUDE_M4],
	[
		AC_MSG_NOTICE([Including $1])
		builtin([include],[$1])
		CONFIGURE_DEPENDENCIES="${CONFIGURE_DEPENDENCIES} $1"
		AC_SUBST(CONFIGURE_DEPENDENCIES)
		CONFIG_STATUS_DEPENDENCIES="${CONFIG_STATUS_DEPENDENCIES} $1"
		AC_SUBST(CONFIG_STATUS_DEPENDENCIES)
	]
)


