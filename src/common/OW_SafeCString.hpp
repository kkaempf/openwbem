#include <cstring>

namespace OW_NAMESPACE
{
	class String;

namespace SafeCString
{

	/**
	 * Like std::strdup, except that new is used to allocate memory
	 */
	char * str_dup(char const * s);

	/**
	 * Like std::strdup, except that no-throw new is used to allocate memory
	 * and NULL is returned on allocation failure.
	 */
	char * str_dup_nothrow(char const * s);


	/**
	 * RETURNS: s if s is nonnull, otherwise returns empty string.
	 *
	 * WARNING: Do not deallocate the return value.
	 */
	inline char const * nonull(char const * s)
	{
		return s ? s : "";
	}


	/**
	 * PROMISE: copies the first n = min(strlen(src), dstsize - 1) characters
	 * of C-string src to dst, null-terminating the result.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters or an
	 * array of at least dstsize - 1 characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: dst + n.
	 */
	char * strcpy_trunc(char * dst, std::size_t dstsize, char const * src);

	/**
	 * PROMISE: copies the first n = min(strlen(src), srclen, dstsize - 1)
	 * characters of C-string src to dst, null-terminating the result.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters or an array
	 * of at least srclen characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: dst + n.
	 */
	char * strcpy_trunc(
		char * dst, std::size_t dstsize, char const * src, std::size_t srclen
	);

	/**
	 * A variant of strcpy_trunc that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcpy_trunc(char (&dst)[N], char const * src)
	{
		return strcpy_trunc(dst, N, src);
	}

	/**
	 * A variant of strcpy_trunc that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcpy_trunc(char (&dst)[N], char const * src, std::size_t srclen)
	{
		return strcpy_trunc(dst, N, src, srclen);
	}

	/**
	 * Like strcpy_trunc, but the destination is the subarray dst[pos..N-1] and
	 * the return value is an array index instead of a char *.
	 */
	template <std::size_t N> inline
	std::size_t strcpy_to_pos_trunc(
		char (&dst)[N], std::size_t pos, char const * src
	)
	{
		return strcpy_trunc(dst + pos, N - pos, src) - dst;
	}

	/**
	 * Like strcpy_trunc, but the destination is the subarray dst[pos..N-1] and
	 * the return value is an array index instead of a char *.
	 */
	template <std::size_t N> inline
	std::size_t strcpy_to_pos_trunc(
		char (&dst)[N], std::size_t pos, char const * src, std::size_t srclen
	)
	{
		return strcpy_trunc(dst + pos, N - pos, src, srclen) - dst;
	}

	OW_DECLARE_EXCEPTION(Overflow);
	/** Error codes for use with OverflowException class */
	int const RESULT_TRUNCATED = 0;
	int const DEST_UNTERMINATED = 1;

	/**
	 * PROMISE: copies the first n = min(strlen(src), dstsize - 1) chars of the
	 * C-string src to dst and appends a terminating '\0'.
	 *
	 * THROWS: OverflowException if strlen(src) >= dstsize.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters or an array
	 * of at least dstsize characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: Pointer to the terminating '\0' char.
	 */
	char * strcpy_check(char * dst, std::size_t dstsize, char const * src);

	/**
	 * PROMISE: copies the first min(n, dstsize - 1) chars of the C-string src
	 * to dst, where n = min(strlen(src), srclen), and appends a terminating '\0'.
	 *
	 * THROWS: OverflowException if n >= dstsize.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters or an
	 * array of at least srclen characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: Pointer to the terminating '\0' char.
	 */
	char * strcpy_check(
		char * dst, std::size_t dstsize, char const * src, std::size_t srclen
	);

	/**
	 * A variant of strcpy_check that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcpy_check(char (&dst)[N], char const * src)
	{
		return strcpy_check(dst, N, src);
	}

	/**
	 * A variant of strcpy_check that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcpy_check(char (&dst)[N], char const * src, std::size_t srclen)
	{
		return strcpy_check(dst, N, src, srclen);
	}

	/**
	 * Like strcpy_check, but the destination is the subarray dst[pos..N-1] and
	 * the return value is an array index instead of a char *.
	 */
	template <std::size_t N> inline
	std::size_t strcpy_to_pos_check(
		char (&dst)[N], std::size_t pos, char const * src
	)
	{
		return strcpy_check(dst + pos, N - pos, src) - dst;
	}

	/**
	 * Like strcpy_check, but the destination is the subarray dst[pos..N-1] and
	 * the return value is an array index instead of a char *.
	 */
	template <std::size_t N> inline
	std::size_t strcpy_to_pos_check(
		char (&dst)[N], std::size_t pos, char const * src, std::size_t srclen
	)
	{
		return strcpy_check(dst + pos, N - pos, src, srclen) - dst;
	}


	/**
	 * PROMISE: If dst[0..dstsize-1] contains a '\0', appends to C-string dst
	 * the first min(strlen(src), dstsize - 1 - strlen(dst)) characters of
	 * C-string src, and null terminates the result.
	 *
	 * THROWS: OverflowException if dst[0..dstsize-1] contains no '\0'.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: Pointer to the terminating '\0' of the resulting C-string.
	 */
	char * strcat_trunc(char * dst, std::size_t dstsize, char const * src);

	/**
	 * PROMISE: If dst[0..dstsize-1] contains a '\0', appends to C-string dst
	 * the first min(strlen(src), srclen, dstsize - 1 - strlen(dst)) characters of
	 * C-string src, and null terminates the result.
	 *
	 * THROWS: OverflowException if dst[0..dstsize-1] contains no '\0'.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters or an array
	 * of at least srclen characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: Pointer to the terminating '\0' of the resulting C-string
	 */
	char * strcat_trunc(
		char * dst, std::size_t dstsize, char const * src, std::size_t srclen
	);

	/**
	 * A variant of strcat_trunc that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcat_trunc(char (&dst)[N], char const * src)
	{
		return strcat_trunc(dst, N, src);
	}

	/**
	 * A variant of strcat_trunc that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcat_trunc(char (&dst)[N], char const * src, std::size_t srclen)
	{
		return strcat_trunc(dst, N, src, srclen);
	}

	/**
	 * PROMISE: If dst[0..dstsize-1] contains a '\0', appends to C-string dst
	 * the first min(strlen(src), dstsize - 1 - strlen(dst)) characters of
	 * C-string src and null terminates the result.
	 *
 	 * THROWS: OverflowException with getErrorCode() == DEST_UNTERMINATED
	 * if dst[0..dstsize-1] contains no '\0'.
	 *
	 * THROWS: OverflowException with getErrorCode() == RESULT_TRUNCATED
	 * if strlen(dst) + strlen(src) >= dstsize.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: Pointer to the terminating '\0' of the resulting C-string.
	 */
	char * strcat_check(char * dst, std::size_t dstsize, char const * src);

	/**
	 * PROMISE: If dst[0..dstsize-1] contains a '\0', appends to C-string dst
	 * the first min(N, dstsize - 1 - strlen(dst)) characters of C-string src,
	 * where N = min(strlen(src), srclen), and null terminates the result.
	 *
 	 * THROWS: OverflowException with getErrorCode() == DEST_UNTERMINATED
	 * if dst[0..dstsize-1] contains no '\0'.
	 *
	 * THROWS: OverflowException with getErrorCode() == RESULT_TRUNCATED
	 * if strlen(dst) + N >= dstsize.
	 *
	 * REQUIRE: dst != 0, src != 0, dstsize > 0.
	 *
	 * REQUIRE: dst points to an array of at least dstsize characters.
	 *
	 * REQUIRE: src points to a null-terminated array of characters or an array
	 * of at least srclen characters.
	 *
	 * REQUIRE: the source and destination character sequences do not overlap.
	 *
	 * RETURNS: Pointer to the terminating '\0' of the resulting C-string.
	 */
	char * strcat_check(
		char * dst, std::size_t dstsize, char const * src, std::size_t srclen
	);

	/**
	 * A variant of strcat_check that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcat_check(char (&dst)[N], char const * src)
	{
		return strcat_check(dst, N, src);
	}

	/**
	 * A variant of strcat_check that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * strcat_check(char (&dst)[N], char const * src, std::size_t srclen)
	{
		return strcat_check(dst, N, src, srclen);
	}


namespace Impl
{
	inline std::size_t nchars_output(int retval, std::size_t dstsize)
	{
		return (
			retval < 0 || retval >= static_cast<int>(dstsize) ?	dstsize - 1
			: static_cast<std::size_t>(retval)
		);
	}

	std::size_t nchars_check(int retval, std::size_t dstsize);
}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1>
	std::size_t sprintf_trunc(
		char * dst, std::size_t dstsize, char const * fmt, T1 const & x1
	)
	{
		return Impl::nchars_output(snprintf(dst, dstsize, fmt, x1), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2>
	std::size_t sprintf_trunc(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_output(
			snprintf(dst, dstsize, fmt, x1, x2), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2>
	std::size_t sprintf_trunc(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_output(
			snprintf(dst, dstsize, fmt, x1, x2), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2, typename T3>
	std::size_t sprintf_trunc(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3
	)
	{
		return Impl::nchars_output(
			snprintf(dst, dstsize, fmt, x1, x2, x3), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2, typename T3, typename T4>
	std::size_t sprintf_trunc(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4
	)
	{
		return Impl::nchars_output(
			snprintf(dst, dstsize, fmt, x1, x2, x3, x4), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	std::size_t sprintf_trunc(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4, T5 const & x5
	)
	{
		return Impl::nchars_output(
			snprintf(dst, dstsize, fmt, x1, x2, x3, x4, x5), dstsize);
	}

	/**
	 * A variant of sprintf_trunc that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1>
	std::size_t sprintf_trunc(char (&dst)[N], char const * fmt, T1 const & x1)
	{
		return Impl::nchars_output(snprintf(dst, N, fmt, x1), N);
	}

	/**
	 * A variant of sprintf_trunc that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2>
	std::size_t sprintf_trunc(
		char (&dst)[N], char const * fmt, T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_output(snprintf(dst, N, fmt, x1, x2), N);
	}

	/**
	 * A variant of sprintf_trunc that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2>
	std::size_t sprintf_trunc(
		char (&dst)[N], char const * fmt, T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_output(snprintf(dst, N, fmt, x1, x2), N);
	}

	/**
	 * A variant of sprintf_trunc that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2, typename T3>
	std::size_t sprintf_trunc(
		char (&dst)[N], char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3
	)
	{
		return Impl::nchars_output(snprintf(dst, N, fmt, x1, x2, x3), N);
	}

	/**
	 * A variant of sprintf_trunc that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2, typename T3, typename T4>
	std::size_t sprintf_trunc(
		char (&dst)[N], char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4
	)
	{
		return Impl::nchars_output(snprintf(dst, N, fmt, x1, x2, x3, x4), N);
	}

	/**
	 * A variant of sprintf_trunc that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2, typename T3, typename T4,
			  typename T5>
	std::size_t sprintf_trunc(
		char (&dst)[N], char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4, T5 const & x5
	)
	{
		return Impl::nchars_output(
			snprintf(dst, N, fmt, x1, x2, x3, x4, x5), N);
	}


	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * THROWS: OverflowException if the output had to be truncated.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1>
	std::size_t sprintf_check(
		char * dst, std::size_t dstsize, char const * fmt, T1 const & x1
	)
	{
		return Impl::nchars_check(snprintf(dst, dstsize, fmt, x1), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * THROWS: OverflowException if the output had to be truncated.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2>
	std::size_t sprintf_check(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_check(
			snprintf(dst, dstsize, fmt, x1, x2), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * THROWS: OverflowException if the output had to be truncated.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2>
	std::size_t sprintf_check(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_check(
			snprintf(dst, dstsize, fmt, x1, x2), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * THROWS: OverflowException if the output had to be truncated.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2, typename T3>
	std::size_t sprintf_check(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3
	)
	{
		return Impl::nchars_check(
			snprintf(dst, dstsize, fmt, x1, x2, x3), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * THROWS: OverflowException if the output had to be truncated.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2, typename T3, typename T4>
	std::size_t sprintf_check(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4
	)
	{
		return Impl::nchars_check(
			snprintf(dst, dstsize, fmt, x1, x2, x3, x4), dstsize);
	}

	/**
	 * PROMISE: Behaves like sprintf(dst, fmt, ...) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * THROWS: OverflowException if the output had to be truncated.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	std::size_t sprintf_check(
		char * dst, std::size_t dstsize, char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4, T5 const & x5
	)
	{
		return Impl::nchars_check(
			snprintf(dst, dstsize, fmt, x1, x2, x3, x4, x5), dstsize);
	}

	/**
	 * A variant of sprintf_check that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1>
	std::size_t sprintf_check(char (&dst)[N], char const * fmt, T1 const & x1)
	{
		return Impl::nchars_check(snprintf(dst, N, fmt, x1), N);
	}

	/**
	 * A variant of sprintf_check that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2>
	std::size_t sprintf_check(
		char (&dst)[N], char const * fmt, T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_check(snprintf(dst, N, fmt, x1, x2), N);
	}

	/**
	 * A variant of sprintf_check that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2>
	std::size_t sprintf_check(
		char (&dst)[N], char const * fmt, T1 const & x1, T2 const & x2
	)
	{
		return Impl::nchars_check(snprintf(dst, N, fmt, x1, x2), N);
	}

	/**
	 * A variant of sprintf_check that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2, typename T3>
	std::size_t sprintf_check(
		char (&dst)[N], char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3
	)
	{
		return Impl::nchars_check(snprintf(dst, N, fmt, x1, x2, x3), N);
	}

	/**
	 * A variant of sprintf_check that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2, typename T3, typename T4>
	std::size_t sprintf_check(
		char (&dst)[N], char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4
	)
	{
		return Impl::nchars_check(snprintf(dst, N, fmt, x1, x2, x3, x4), N);
	}

	/**
	 * A variant of sprintf_check that infers the destination buffer size.
	*/
	template <std::size_t N, typename T1, typename T2, typename T3, typename T4,
			  typename T5>
	std::size_t sprintf_check(
		char (&dst)[N], char const * fmt,
		T1 const & x1, T2 const & x2, T3 const & x3, T4 const & x4, T5 const & x5
	)
	{
		return Impl::nchars_check(snprintf(dst, N, fmt, x1, x2, x3, x4, x5), N);
	}


	/**
	 * PROMISE: Behaves like vprintf(dst, fmt, ap) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
	template <typename T1>
	std::size_t vprintf_trunc(
		char * dst, std::size_t dstsize, char const * fmt, va_list ap
	)
	{
		return Impl::nchars_output(vsnprintf(dst, dstsize, fmt, ap), dstsize);
	}

	/**
	 * A variant of vprintf_trunc that infers the destination buffer size.
	 */
 	template <std::size_t N> inline
	std::size_t vprintf_trunc(char (&dst)[N], char const * fmt, va_list ap)
	{
		return vprintf_trunc(dst, N, fmt, ap);
	}


	/**
	 * PROMISE: Behaves like vprintf(dst, fmt, ap) except that output stops
	 * at dstsize - 1 characters.  Always outputs a terminating '\0'.
	 *
	 * THROWS: OverflowException if the output had to be truncated.
	 *
	 * RETURNS: the number of chars output, not including the terminating '\0'
	*/
 	inline std::size_t vprintf_check(
		char * dst, std::size_t dstsize, char const * fmt, va_list ap
	)
	{
		return Impl::nchars_check(vsnprintf(dst, dstsize, fmt, ap), dstsize);
	}

	/**
	 * A variant of vprintf_check that infers the destination buffer size.
	 */
 	template <std::size_t N> inline
	std::size_t vprintf_check(char (&dst)[N], char const * fmt, va_list ap)
	{
		return vprintf_check(dst, N, fmt, ap);
	}


	struct FILE;

	/**
	 * PROMISE: Equivalent to std::fgets, except that I/O errors are reported by
	 * throwing an exception.
	 *
	 * THROWS: OpenWBEM::IOException if there is a read error.
	 * 
	 * REQUIRES: dst nonnull, fp nonnull, dstsize > 0.
	 *
	 * RETURNS: NULL if no characters read because of reaching EOF; dst otherwise.
	 */
	char * fgets_trunc(char * dst, std::size_t dstsize, FILE * fp);

	/**
	 * A variant of fgets_trunc that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * fgets_trunc(char (&dst)[N], FILE * fp)
	{
		return fgets_trunc(dst, N, fp);
	}

	/**
	 * PROMISE: Equivalent to std::fgets, except that I/O errors and truncated
	 * lines are reported by throwing an exception.
	 * 
	 * THROWS: OpenWBEM::IOException if there is a read error.
	 *
	 * THROWS: OverflowException if the input line (including terminating '\0')
	 * must be truncated to fit into dstsize chars.
	 *
	 * REQUIRES: dst nonnull, fp nonnull, dstsize > 0.
	 *
	 * RETURNS: NULL if no characters read because of reaching EOF; dst otherwise.
	 */
	char * fgets_check(char * dst, std::size_t dstsize, FILE * fp);

	/**
	 * A variant of fgets_check that infers the destination buffer size.
	 */
	template <std::size_t N> inline
	char * fgets_check(char (&dst)[N], FILE * fp)
	{
		return fgets_check(dst, N, fp);
	}

	/**
	* RETURNS: A String created by reading from fp until either EOF or a
	* newline is encountered.  If a newline is found it is included in the
	* return String.
	*
	* THROWS: OpenWBEM::IOException if there is a read error.
	*
	* THROWS: OpenWBEM::StringConversionException if neither EOF nor newline
	* is found within max_chars characters.  (This is a protection against
	* unreasonably long inputs.)
	*/
	String fget_string(FILE * fp, std::size_t const max_chars);


#define OW_INTSTR_AUX(x) # x
	/*
	 * Use the OW_INTSTR macro to insert symbolic constants into format strings.
	 * A typical usage would be for a maximum field width when using the
	 * scanf family of functions.  For example, suppose that buf is an array of
	 * STRMAX + 1 characters, and STRMAX is a manifest constant.  Then one
	 * might write
	 *
	 *   sscanf(inpstr, "Name: %." OW_INTSTR(STRMAX) "s\n", buf);
	*/
#define OW_INTSTR(x) OW_INTSTR_AUX(x)

} // namespace SafeCString
} // namespace OW_NAMESPACE
