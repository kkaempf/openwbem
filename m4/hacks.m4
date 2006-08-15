# This is a hack to alter autoconf to always treat all files as obsolete.  This
# causes maintainer mode to force a rebuild of configure every time autoconf is
# executed (autoconf normally tries to be too smart for its own good).  Note
# that this is used with another hack in acinclude.m4 where OW_INCLUDE_M4 hacks
# the configure and config.status dependencies so that things are rebuilt when
# any of the included m4 files are touched.  Without this additional hack,
# autoconf will (incorrectly) assume that configure does not need to be
# recreated, as it doesn't properly understand m4 includes.
AC_DEFUN([OW_APPLY_AUTOCONF_REEXECUTION_HACK],
	AUTOCONF="${AUTOCONF-${am_missing_run}autoconf} -f"
)
