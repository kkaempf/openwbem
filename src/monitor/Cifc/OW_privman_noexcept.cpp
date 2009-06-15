#include "OW_config.h"

#include <cstdlib>
#include <cstring>
#include <vector>

// Get ECommand, EStatus, DescriptorInfo
#include "OW_PrivilegeCommon.hpp"

// Get PrivilegeManager::ReadDirOptions
#include "OW_PrivilegeManager.hpp"

#include "OW_IPCIO_noexcept.hpp"

#if 0
#include "OW_Exception.hpp"
#include "blocxx/Format.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_SafeCString.hpp"
#include "blocxx/StringStream.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include <map>
#endif

#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif

extern "C"
{
#include "OW_privman_noexcept.h"
}

using namespace OpenWBEM;

namespace
{
	typedef IPCIO_noexcept::EBuffering EBuffering;

	inline char const * strend(char const * s, std::size_t n)
	{
		return static_cast<char const *>(std::memchr(s, '\0', n));
	}
 
	// PROMISE: Copies first m = min(n, strlen(src) + 1) chars of src to dst.
	// RETURNS: dst + strlen(src) if strlen(src) < n, NULL otherwise.
	//
	inline char * safe_strcpy(char * dst, char const * src, std::size_t n)
	{
#ifdef OW_HAS_MEMCCPY
		char * rv = static_cast<char *>(std::memccpy(dst, src, '\0', n));
		return rv ? rv - 1 : 0;
#else
		char const * end = strend(src, n);
		if (end) // '\0' found
		{
			size_t len = end - src;
			std::memcpy(dst, src, len + 1);  // include terminating '\0'
			return dst + len;
		}
		else
		{
			std::memcpy(dst, src, n);
			return 0;
		}
#endif
	}

	// PROMISE: copies the first n = min(strlen(src), dstsize - 1) characters
	// of C-string src to dst, null-terminating the result.
	//
	// REQUIRE: dst != 0, src != 0, dstsize > 0.
	//
	// REQUIRE: dst points to an array of at least dstsize characters.
	//
	// REQUIRE: src points to a null-terminated array of characters or an
	// array of at least dstsize - 1 characters.
	//
	// REQUIRE: the source and destination character sequences do not overlap.
	//
	// RETURNS: dst + n.
	//
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

	int monitor_descriptor()
	{
		char const * s_desc = std::getenv("OW_PRIVILEGE_MONITOR_DESCRIPTOR");
		if (!s_desc)
		{
			return -1;
		}
		int fd = std::atoi(s_desc);
		return fd <= 0 ? -1 : fd;
	}

	IPCIO_noexcept g_conn;

	IPCIO_noexcept & connection()
	{
		if (!g_conn.connected())
		{
			g_conn.connect(monitor_descriptor());
			::pid_t monitor_pid;
			ipcio_get(g_conn, monitor_pid);
			g_conn.get_sync();
			// We discard the monitor pid, as we don't use it.
		}
		return g_conn;
	}

	// Directly copies the error message to the buffer, truncating if needed.
	int copy_errmsg(char const * msg, char * errbuf, size_t bufsize)
	{
		strcpy_trunc(errbuf, bufsize, msg);
		return -1;
	}

	int copy_errmsg2(
		char const * operation, char const * msg, char * errbuf, size_t bufsize)
	{
		char * end = errbuf + bufsize;
		char * pos = strcpy_trunc(errbuf, bufsize, operation);
		if (pos + 1 >= end)
		{
			return -1;
		}

		pos = strcpy_trunc(pos, end - pos, ": ");
		if (pos + 1 >= end)
		{
			return -1;
		}

		strcpy_trunc(pos, end - pos, msg);
		return -1;
	}

	int copy_unimplemented_errmsg(char * errbuf, size_t bufsize)
	{
		return copy_errmsg("Unimplemented monitor function", errbuf, bufsize);
	}

	const char* get_exception_for_code(int errcode)
	{
		if( errcode < PrivilegeManager::MONITOR_FATAL_ERROR_START )
		{
			return "PrivilegeManagerException";
		}
		else if( errcode == PrivilegeManager::E_INSUFFICIENT_PRIVILEGES )
		{
			return "InsufficientPrivilegesException";
		}
		else
		{
			return "FatalPrivilegeManagerException";
		}
	}

	int check_result(
		IPCIO_noexcept & conn, char * errbuf, size_t bufsz,
		char const * op, EBuffering eb)
	{
		PrivilegeCommon::EStatus status;
		ipcio_get(conn, status, eb);
		if (!conn.ok())
		{
			copy_errmsg2(op, "no response from monitor", errbuf, bufsz);
			return -1;
		}
		if (status == PrivilegeCommon::E_ERROR)
		{
			int errcode;
			ipcio_get(conn, errcode);

			std::vector<char> errmsg;
			ipcio_get_str(conn, errmsg);

			if (!conn.ok())
			{
				copy_errmsg2(
					op, "failed reading error message and code from monitor",
					errbuf, bufsz);
				return -1;
			}

			conn.get_sync();
			if (!conn.ok())
			{
				copy_errmsg2(op,
					"get_sync failure (reading error message and code)",
					errbuf, bufsz);
			}

			char* p = errbuf;
			char* buf_end = errbuf + bufsz;
			// All this does is formats the string similar to this:
			// Format("%1: %2", get_exception_for_code(errcode), &errmsg[0])
			p = strcpy_trunc(p, buf_end - p, get_exception_for_code(errcode));
			if( p < buf_end )
			{
				p = strcpy_trunc(p, buf_end - p, ": ");
				if( p < buf_end )
				{
					p = strcpy_trunc(p, buf_end - p, &errmsg[0]);
				}
			}

			return errcode;
		}
		return 0;
	}

	// Must match opname array
	enum EOperations
	{
		E_OPEN, E_READ_DIR
	};

	// Must match EOperations
	char const * const opname[] =
	{
		"owprivman_open", "owprivman_read_directory"
	};
}

#define CHECK(tst, op, msg) \
if (!(tst)) \
{ \
  return copy_errmsg2(opname[(op)], (msg), errbuf, bufsize); \
}

#define CHECK_RESULT(conn, op, eb) \
do { \
	int errc2344e7187f5b7f85bce00b17ce47fc25 = \
		check_result((conn), errbuf, bufsize, opname[(op)], (eb)); \
	if (errc2344e7187f5b7f85bce00b17ce47fc25 != 0) \
	{ \
		return errc2344e7187f5b7f85bce00b17ce47fc25; \
	} \
} while (false)

int owprivman_open(
	char const * pathname, unsigned flags, unsigned perms,
	int * retval, char * errbuf, size_t bufsize
)
{
	IPCIO_noexcept & conn = connection();
	CHECK(conn.ok(), E_OPEN, "no monitor connection");

	ipcio_put(conn, PrivilegeCommon::E_CMD_OPEN);
	ipcio_put_str(conn, pathname);
	ipcio_put(conn, flags);
	ipcio_put(conn, perms);
	CHECK(conn.ok(), E_OPEN, "write to monitor failed");
	conn.put_sync();
	CHECK(conn.ok(), E_OPEN, "put_sync failure");

	CHECK_RESULT(conn, E_OPEN, IPCIO_noexcept::E_UNBUFFERED);
	FileHandle fh = conn.get_handle();
	CHECK(conn.ok(), E_OPEN, "failed reading descriptor from monitor");
	conn.get_sync();
	if (!conn.ok())
	{
		::close(fh);
		CHECK(false, E_OPEN, "get_sync failure");
	}

	// The Perl API does not support the ate (seek to end) flag, so we're done.

	*retval = fh;
	return 0;
}

int owprivman_read_directory(
	char const * pathname, int keep_special, string_handler_t h, void * p,
	char * errbuf, size_t bufsize
)
{
	IPCIO_noexcept & conn = connection();
	CHECK(conn.ok(), E_READ_DIR, "no monitor connection");

	PrivilegeManager::ReadDirOptions opt =
		keep_special ? PrivilegeManager::E_KEEP_SPECIAL :
		PrivilegeManager::E_OMIT_SPECIAL;
	ipcio_put(conn, PrivilegeCommon::E_CMD_READ_DIR);
	ipcio_put_str(conn, pathname);
	ipcio_put(conn, opt);
	CHECK(conn.ok(), E_READ_DIR, "write to monitor failed");
	conn.put_sync();
	CHECK(conn.ok(), E_READ_DIR, "put_sync failure");

	CHECK_RESULT(conn, E_READ_DIR, IPCIO_noexcept::E_BUFFERED);
	size_t arrlen;
	ipcio_get(conn, arrlen);
	CHECK(conn.ok(), E_READ_DIR, "read from monitor failed (arrlen)");
	for (size_t i = 0; i < arrlen; ++i)
	{
		std::vector<char> entry;
		ipcio_get_str(conn, entry);
		h(&entry[0], p);
	}

	CHECK(conn.ok(), E_READ_DIR, "read from monitor failed (entry)");
	conn.get_sync();
	CHECK(conn.ok(), E_READ_DIR, "get_sync failure");

	return 0;
}

int owprivman_rename(
	char const * old_path, char const * new_path,
	char * errbuf, size_t bufsize
)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_remove_file(
	char const * path, int * retval, char * errbuf, size_t bufsize
)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_remove_dir(
	char const * path, int * retval, char * errbuf, size_t bufsize
)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_monitored_spawn(
	char const * exec_path, char const * app_name,
	char const * const * argv, char const * const * envp,
	int * retval, char * errbuf, size_t bufsize
)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_user_spawn(
	char const * exec_path,
	char const * const * argv, char const * const * envp,
	char const * user,
	int * retval, char * errbuf, size_t bufsize
)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_child_stdin(int pid, int * retval, char * errbuf, size_t bufsize)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_child_stdout(int pid, int * retval, char * errbuf, size_t bufsize)
{	
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_child_stderr(int pid, int * retval, char * errbuf, size_t bufsize)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}

int owprivman_child_status(
	int pid, int * rep1, int * rep2, char * errbuf, size_t bufsize)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}



int owprivman_status_running(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_terminated(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_exit_terminated(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_exit_status(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_terminated_successfully(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_signal_terminated(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_term_signal(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_stopped(int rep1, int rep2)
{
	return 0;
}

int owprivman_status_stop_signal(int rep1, int rep2)
{
	return 0;
}

int owprivman_child_wait_close_term(
	int pid, float wait_initial, float wait_close, float wait_term,
	int * status_rep1, int * status_rep2, char * errbuf, size_t bufsize
)
{
	return copy_unimplemented_errmsg(errbuf, bufsize);
}
