dnl See if the C++ compiler supports std::tr1::is_pod<> correctly.
dnl This allows a "true" and "false" block to be included as arguments.
AC_DEFUN([OW_CHECK_TR1_IS_POD],
[
	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	dnl See if the C++ compiler supports std::tr1::is_pod<> correctly
	AC_MSG_CHECKING(if the C++ compiler supports std::tr1::is_pod<> correctly)
	AC_TRY_COMPILE(
	[
		#include <tr1/type_traits>
		template <bool x> struct StaticAssertionFailure;

		template <>
		struct StaticAssertionFailure<true>
		{
		};

		#define OW_STATIC_ASSERT_CAT(a, b) a ## b
		#define OW_STATIC_ASSERT_AUX(a, b) OW_STATIC_ASSERT_CAT(a, b)

		#define OW_STATIC_ASSERT( B ) \
		enum { \
			OW_STATIC_ASSERT_AUX(openwbem_static_assert_enum_, __LINE__) \
			= sizeof( StaticAssertionFailure< (bool)( B ) > ) \
		}

		struct POD
		{
			int x;
			int y;
		};

		OW_STATIC_ASSERT(std::tr1::is_pod<POD>::value == true);

		class nonPOD
		{
		private:
			nonPOD();
			nonPOD(const nonPOD& x);
			int x;
		};

		OW_STATIC_ASSERT(std::tr1::is_pod<nonPOD>::value == false);
	],
	[
	],
	[
		AC_MSG_RESULT(yes)
		$1
	],
	[
		AC_MSG_RESULT(no)
		$2
	])
	AC_LANG_RESTORE
])
