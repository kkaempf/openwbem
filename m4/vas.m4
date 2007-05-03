AC_DEFUN(
	[OW_VAS_SUPPORT],
	[
		AC_ARG_ENABLE(vas,
		OW_HELP_STRING(--enable-vas,enable VAS (Vintela Authentication Services) integration),
		[
			USE_VAS=1
			AC_DEFINE(USE_VAS, 1, VAS integration is enabled)
		])

		AC_ARG_WITH(vas2,
			OW_HELP_STRING( --with-vas2=PATH,Specify the directory in which the vas 2
				headers and libs reside),
			[
				# This fancy thing just erases ./ and foo/.. since aCC on HPUX doesn't work
				# if include dirs have that!
				# We use [[ and ]] because m4 eats [ and ]

				# Note that this is a loop instead of global substitution, because items
				# like "/foo/bar/baz/quux/../../.." don't convert correctly using the
				# simple sed expression.  It turns it into "foo/bar/baz" instead of "/foo".
				# This loop ensures left-to-right eating of one 'DIR/..' at a time.
				# After trying to come up with a functional (and portable) sed expression,
				# this loop looked very appetizing (less error prone and it actually
				# works).
				VAS2_DIR=`echo $withval | sed -e 's~/\\./~/~g'`
				temp_dir=""
				until [[ "$temp_dir" = "$VAS2_DIR" ]]; do
					temp_dir="$VAS2_DIR"
					VAS2_DIR=`echo $VAS2_DIR | sed -e 's~/[[^/]]*/\\.\\.~~'`
				done
				VAS2_INCLUDE_DIR=$VAS2_DIR/include

				if test -d "$VAS2_DIR/lib64" ; then
					VAS2_LIB_DIR=$VAS2_DIR/lib64
				else
					VAS2_LIB_DIR=$VAS2_DIR/lib
				fi
			]
		)
		AC_ARG_WITH(vas2-rpath,
			OW_HELP_STRING(--with-vas2-rpath=PATH,Specify the directory in which the vas
				2 runtime libs reside),
			[
				VAS2_RPATH=$withval
			]
		)
		AC_SUBST(VAS2_INCLUDE_DIR)
		AC_SUBST(VAS2_LIB_DIR)
		AC_SUBST(VAS2_RPATH)
		AM_CONDITIONAL(HAVE_VAS2, test x$VAS2_DIR != x)

		AC_ARG_WITH(vas3,
			OW_HELP_STRING(--with-vas3=PATH,Specify the directory in which the vas 3
				headers and libs reside),
			[
				# This fancy thing just erases ./ and foo/.. since aCC on HPUX doesn't work
				# if include dirs have that!
				# We use [[ and ]] because m4 eats [ and ]
				# See the comments for VAS2_DIR above.
				VAS3_DIR=`echo $withval | sed -e 's~/\\./~/~g'`
				temp_dir=""
				until [[ "$temp_dir" = "$VAS3_DIR" ]]; do
					temp_dir="$VAS3_DIR"
					VAS3_DIR=`echo $VAS3_DIR | sed -e 's~/[[^/]]*/\\.\\.~~'`
				done
				VAS3_INCLUDE_DIR=$VAS3_DIR/include

				if test -d "$VAS3_DIR/lib64" ; then
					VAS3_LIB_DIR=$VAS3_DIR/lib64
				else
					VAS3_LIB_DIR=$VAS3_DIR/lib
				fi
			]
		)
		AC_ARG_WITH(vas3-rpath,
			OW_HELP_STRING(--with-vas3-rpath=PATH,Specify the directory in which the vas
				3 runtime libs reside),
			[
				VAS3_RPATH=$withval
			]
		)
		AC_SUBST(VAS3_INCLUDE_DIR)
		AC_SUBST(VAS3_LIB_DIR)
		AC_SUBST(VAS3_RPATH)
		AM_CONDITIONAL(HAVE_VAS3, test x$VAS3_DIR != x)
	]
)
