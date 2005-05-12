#include "OW_SafeCString.hpp"

#include <cstring>
#include <new>

namespace
{
	inline char * strend(char * s, std::size_t n)
	{
		return static_cast<char *>(std::memchr(s, '\0', n));
	}

	char * strend_checked(char * s, std::size_t n)
	{
		char * retval = strend(s, n);
		if (retval)
		{
			return retval;
		}
		else
		{
			OW_THROW_ERR(
				OverflowException,
				"cstring catenation first operand unterminated",
				DEST_UNTERMINATED
			);
		}
	}

	// PROMISE: Copies first m = min(n, strlen(src) + 1) chars of src to dst.
	// RETURNS: dst + m if strlen(src) < n, NULL otherwise.
	//
	inline char * safe_strcpy(char * dst, char const * src, std::size_t n)
	{
#ifdef OW_HAS_MEMCCPY
		return static_cast<char *>(std::memccpy(dst, src, '\0', n));
#else
		char const * end = strend(src, n);
		if (end) // '\0' found
		{
			n = (end - src) + 1;
			std::memcpy(dst, src, n);
			return dst + n;
		}
		else
		{
			std::memcpy(dst, src, n);
			return 0;
		}
#endif
	}
}

namespace OW_NAMESPACE
{
namespace SafeCString
{

OW_DEFINE_EXCEPTION(Overflow);

char * str_dup(char const * s);
{
	char * retval = new char[std::strlen(s) + 1];
	return std::strcpy(retval, s);
}

char * str_dup_nothrow(char const * s);
{
	char * retval = new (std::nothrow) char[std::strlen(s)];
	if (retval)
	{
		std::strcpy(retval, s);
	}
	return retval;
}

char * strcpy_trunc(char * dst, std::size_t dstsize, char const * src)
{
	std::size_t n = dstsize - 1;
	char * retval = safe_strcpy(dst, src, n);
	if (retval)
	{
		return retval;
	}
	else
	{
		dst[n] = '\0';
		return dst + n;
	}
}

char * strcpy_trunc(
	char * dst, std::size_t dstsize, char const * src, std::size_t srclen
)
{
	std::size_t n = (srclen < dstsize ? srclen : dstsize - 1);
	char * retval = safe_strcpy(dst, src, n);
	if (retval)
	{
		return retval;
	}
	else
	{
		dst[n] = '\0';
		return dst + n;
	}
}

char * strcpy_check(char * dst, std::size_t dstsize, char const * src)
{
	char * retval = safe_strcpy(dst, src, dstsize);
	if (retval)
	{
		return retval;
	}
	else
	{
		dst[dstsize - 1] = '\0';
		OW_THROW_ERR(
			OverflowException, "cstring copy overflow", RESULT_TRUNCATED);
	}
}

char * strcpy_check(
	char * dst, std::size_t dstsize, char const * src, std::size_t srclen
)
{
	if (srclen >= dstsize)
	{
		return strcpy_check(dst, dstsize, src);
	}
	else // srclen < dstsize
	{
		return strcpy_trunc(dst, srclen + 1, src);
	}
}

char * strcat_trunc(char * dst, std::size_t dstsize, char const * src)
{
	char * dstend = strend_checked(dst, dstsize);
	return strcpy_trunc(dstend, (dst + dstsize) - dstend, src);
}

char * strcat_trunc(
	char * dst, std::size_t dstsize, char const * src, std::size_t srclen
)
{
	char * dstend = strend_checked(dst, dstsize);
	return strcpy_trunc(dstend, (dst + dstsize) - dstend, src, srclen);
}

char * strcat_check(char * dst, std::size_t dstsize, char const * src)
{
	char * dstend = strend_checked(dst, dstsize);
	return strcpy_check(dstend, (dst + dstsize) - dstend, src);
}

char * strcat_check(
	char * dst, std::size_t dstsize, char const * src, std::size_t srclen
)
{
	char * dstend = strend_checked(dst, dstsize);
	return strcpy_check(dstend, (dst + dstsize) - dstend, src, srclen);
}

namespace Impl
{
	std::size_t nchars_check(int retval, std::size_t dstsize)
	{
		if (retval < 0 || retval >= static_cast<int>(dstsize))
		{
			OW_THROW_ERR(
				OverflowException, "sprintf overflow", RESULT_TRUNCATED);
		}
		return static_cast<std::size_t>(retval);
	}
}

char * fgets_trunc(char * dst, std::size_t dstsize, FILE * fp)
{
	char * res = std::fgets(dst, dstsize, fp);
	if (!res)
	{
		if (std::feof(fp))
		{
			return 0;
		}
		OW_THROW(OpenWBEM::IOException, "read error");
	}
	return res;
}

char * fgets_check(char * dst, std::size_t dstsize, FILE * fp)
{
	std::size_t const end = dstsize - 1;
	char savechar = dst[end];
	dst[end] = ' '; // anything but '\0'
	char * res = std::fgets(dst, dstsize, fp);
	char endchar = dst[end];
	dst[end] = savechar;
	if (res)
	{
		if (endchar == '\0' && (end == 0 || dst[end - 1] != '\n'))
		{
			// No newline at end.  Either the input line was truncated, or
			// we read the last line of the file and it had no newline.
			// We test for EOF to distinguish the two cases.  Since the EOF
			// marker doesn't get set until we actually try to read past EOF,
			// we first peek one character ahead.
			std::ungetc(std::fgetc(fp), fp);
			if (!std::feof(fp))
			{
				OW_THROW_ERR(
					OverflowException, "fgets overflow", RESULT_TRUNCATED);
			}
		}
		return res;
	}
	else
	{
		if (std::feof(fp))
		{
			return 0;
		}
		OW_THROW(OpenWBEM::IOException, "read error");
	}
}

String fget_string(FILE * fp, std::size_t const max_chars)
{
	// This could perhaps be made more efficient by reading directly into a
	// custom resizable character array.
	std::size_t const BUFSIZE = 8 * 1024;
	StringBuffer sb;
	char buf[BUFSIZE];
	while (!sb.endsWith('\n') && sb.length() <= max_chars &&
		fgets_trunc(buf, fp))
	{
		sb.append(buf);
	}
	if (sb.length() > max_chars)
	{
		OW_THROW(StringConversionException, "input line too long");
	}
	return sb.releaseString();
}

} // namespace SafeCString
} // namespace OW_NAMESPACE
