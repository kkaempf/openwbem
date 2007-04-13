dnl @synopsis OW_CLEANUP_DIRECTORY_NAME(output_variable, [input])
dnl @author Kevin Harris
dnl
dnl Example:
dnl foo="///foo/bar/../baz/quux/../ick"
dnl OW_CLEANUP_DIRECTORY_NAME(cleaned,$foo)
dnl
dnl The variable "cleaned" would then contain "/foo/baz/ick"
AC_DEFUN([OW_CLEANUP_DIRECTORY_NAME],
	[
		if test "x$2" != "x"; then
			$1="$2"
		fi
		cdn_dir_name="`echo \"[$]$1\" | sed -e 's~/\./~/~g' -e 's~//~/~g'`"
		cdn_temp_dir=""
		until test "${cdn_temp_dir}" = "${cdn_dir_name}"; do
			cdn_temp_dir="${cdn_dir_name}"
			cdn_dir_name="`echo \"${cdn_dir_name}\" | sed -e 's~/[[^/]]*/\.\.~~'`"
		done
		$1="${cdn_dir_name}"
		unset cdn_dir_name
		unset cdn_temp_dir
	]
)

AC_DEFUN(
	[OW_SET_DEFAULT_DIRS],
	[
		pfx=$prefix
		if test "x$pfx" = "xNONE"; then
			pfx=${ac_default_prefix}
		fi
		execpfx=$exec_prefix
		if test "x$execpfx" = "xNONE"; then
			execpfx=$pfx
		fi

		#####################################################
		# Set the location of the state dir
		if test "x$localstatedir" != "x\${prefix}/var"; then
			tempDir=$localstatedir
		else
			tempDir=$pfx/var
		fi
		AC_DEFINE_UNQUOTED(DEFAULT_STATE_DIR, "${tempDir}")

		#####################################################
		# Set the location of the lib dir
		if test "x$libdir" != "x\${exec_prefix}/lib"; then
			tempDir=$libdir
		else
			tempDir=$execpfx/lib
		fi
		AC_DEFINE_UNQUOTED(DEFAULT_LIB_DIR, "${tempDir}")


		#####################################################
		# Set the location of the bin dir
		if test "x$bindir" != "x\${exec_prefix}/bin"; then
			tempDir=$bindir
		else
			tempDir=$execpfx/bin
		fi
		AC_DEFINE_UNQUOTED(DEFAULT_BIN_DIR, "${tempDir}", "The bin dir")


		#####################################################
		# Set the location of the sbin dir
		if test "x$sbindir" != "x\${exec_prefix}/sbin"; then
			tempDir=$sbindir
		else
			tempDir=$execpfx/sbin
		fi
		AC_DEFINE_UNQUOTED(DEFAULT_SBIN_DIR, "${tempDir}", "The sbin dir")


		#####################################################
		# Set the location of the libexec dir
		if test "x$libexecdir" != "x\${exec_prefix}/libexec"; then
			tempDir=$libexecdir
		else
			tempDir=$execpfx/libexec
		fi
		AC_DEFINE_UNQUOTED(DEFAULT_LIBEXEC_DIR, "${tempDir}")

		#####################################################
		# Set the location of the sysconf dir
		if test "x$sysconfdir" != "x\${prefix}/etc"; then
			tempDir=$sysconfdir
		else
			tempDir=$pfx/etc
		fi
		AC_DEFINE_UNQUOTED(DEFAULT_SYSCONF_DIR, "${tempDir}")

	]
)
