/*******************************************************************************
* Copyright (c) 2002, Networks Associates, Inc. All rights reserved.
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, 
*       nor Quest Software, Inc., nor Novell, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_AppenderLogger.hpp"
#include "OW_Array.hpp"
#include "OW_Assertion.hpp"
#include "OW_AutoDescriptor.hpp"
#include "OW_Exception.hpp"
#include "OW_Exec.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Format.hpp"
#include "OW_IPCIO.hpp"
#include "OW_Logger.hpp"
#include "OW_LoggerSpec.hpp"
#include "OW_MultiProcessFileAppender.hpp"
#include "OW_NullAppender.hpp"
#include "OW_PrivilegeCommon.hpp"
#include "OW_PrivilegeConfig.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_Secure.hpp"
#include "OW_String.hpp"
#include "OW_Timeout.hpp"
#include "OW_SyslogAppender.hpp"
#include "blocxx/WaitpidThreadFix.hpp"

#include <algorithm>
#include <iterator>
#include <deque>
#include <fstream>
#include <fcntl.h>
#include <pwd.h>
#include <set>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits>
#include <memory>

using namespace OpenWBEM;
using OpenWBEM::PrivilegeConfig::Privileges;
typedef PrivilegeCommon::ECommand ECommand;

#define CHECK0(tst, msg) CHECK_E(tst, CheckException, msg)
#define CHECK0_ERR(tst, msg, err) CHECK_E_ERR(tst, CheckException, msg, err)
#define CHECK(tst, msg, err) CHECK0_ERR(tst, add_prefix(msg), err)

#define CHECK0_ERRNO(tst, msg) CHECK_ERRNO_E(tst, CheckException, msg)
#define CHECK_ERRNO(tst, msg) CHECK0_ERRNO(tst, add_prefix(msg))

// CHECKARGS differs from CHECK only in that it also causes the monitor to quit
//
#define CHECKARGS(tst, msg, err) \
	WRAP_STMT(if (!(tst)) { if( (err) >= PrivilegeManager::MONITOR_FATAL_ERROR_START) {  m_done = true; } THROW_MSG_ERR(CheckException, msg, err); })



// There is a call to the SSL function ERR_remove_state in the Thread code.
// It's really only needed if you're using SSL.  To avoid a dependency on
// libssl, we define the function here to do nothing.
//
extern "C" void ERR_remove_state(unsigned long)
{
}

namespace
{
	LogAppenderRef getSyslogAppender()
	{
		LogAppenderRef logappender = 
			new SyslogAppender(
				LogAppender::ALL_COMPONENTS, 
				LogAppender::ALL_CATEGORIES,
				SyslogAppender::STR_DEFAULT_MESSAGE_PATTERN,
				"ow.mon.tempsyslogger", "daemon"
				);

		return logappender;
	}

	Logger logger("ow.mon.none", getSyslogAppender());

	unsigned const PATH_CACHE_SIZE = 100;
	int const NUM_CMDS = static_cast<int>(PrivilegeCommon::E_NUM_MONITOR_CMDS);
	UserId const ROOT_USERID = 0;

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#ifndef ARG_MAX
#define ARG_MAX 1024
#endif

	/**
	 * The intention of these values is to limit the maximum amount of memory
	 * consumed by the monitor in the case bad values are provided.  The system
	 * may impose further limits on the combined total size of environment,
	 * path, and arguments, which would then be reported as an error when the
	 * appropriate system call is attempted.
	 */
	std::size_t const MAX_PATH_LENGTH = PATH_MAX;
	std::size_t const MAX_APPNAME_LENGTH = PATH_MAX;
	// The maximum total length of all command-line arguments combined
	// (including trailing nulls).
	std::size_t const MAX_ARG_LENGTH = ARG_MAX;
	// The total size of the environment (including trailing nulls).
	std::size_t const MAX_ENV_LENGTH = ARG_MAX;
	std::size_t const MAX_STATUS_MESSAGE_LENGTH = 1024;
	std::size_t const MAX_USER_NAME_LENGTH = 1024;
	std::size_t const MAX_CATEGORY_LENGTH = 128;
	std::size_t const MAX_FORMAT_LENGTH = 1024;
	std::size_t const MAX_CATEGORIES = 1024;

	OW_DECLARE_EXCEPTION(Check);
	OW_DEFINE_EXCEPTION(Check);

	String simpleUntokenize(const StringArray& arr, const String& separator = ", ")
	{
		String output;
		StringArray::const_iterator iter = arr.begin();
		if( iter != arr.end() )
		{
			output = *iter;
			for( ++iter; iter != arr.end(); ++iter )
			{
				output += separator + *iter;
			}
		}
		return output;
	}

	void sendError(IPCIO& conn, const String& msg, int errorCode)
	{
		ipcio_put(conn, PrivilegeCommon::E_ERROR);
		ipcio_put(conn, errorCode);
		ipcio_put(conn, msg);
		conn.put_sync();
	}

	void reportError(IPCIO& conn, const String& msg, int errorCode)
	{
		OW_LOG_ERROR(logger, Format("Error (code %2): %1", msg, errorCode));
		sendError(conn, msg, errorCode);
	}

	// Report an exception.  If it is an ipcio exception, it will be rethrown,
	// as these should not be hidden from main() because it returns different
	// error codes for IPCIO problems.
	void reportExceptionAux(IPCIO & conn, const String& prefix, const Exception& e, bool logAsError)
	{
		if (logAsError)
		{
			OW_LOG_ERROR(logger, Format("%1%2", prefix, e));
		}
		else
		{
			OW_LOG_DEBUG(logger, Format("%1%2", prefix, e));
		}
		
		sendError(conn, Format("%1%2: %3", prefix, e.type(), e.what()), e.getErrorCode());

		if (dynamic_cast<const IPCIOException*>(&e))
		{
			e.rethrow();
		}
	}

	template <typename Container, typename T>
	inline bool has(Container const & c, T const & val)
	{
		return c.find(val) != c.end();
	}

	String add_prefix(String const & s)
	{
		return "PrivilegeManager::" + s;
	}

	UserId getuid(String const & username)
	{
		errno = 0;
		struct passwd * pwent = ::getpwnam(username.c_str());
		CHECK0(pwent || errno != 0, "user name " + username + " not found");
		CHECK0_ERRNO(pwent || errno == 0, Format("getpwnam(\"%1\")", username).c_str());
		return pwent->pw_uid;
	}

	bool has_open_priv(
		String const & path, PrivilegeManager::OpenFlags flags,
		Privileges const & priv
	)
	{
		if ((flags & PrivilegeManager::in) && !priv.open_read.match(path))
		{
			return false;
		}
		if (flags & PrivilegeManager::app)
		{
			if (!priv.open_append.match(path) && !priv.open_write.match(path))
			{
				return false;
			}
		}
		else if ((flags & PrivilegeManager::out) &&	!priv.open_write.match(path))
		{
			return false;
		}
		return true;
	}

	bool has_stat_priv(String const & path, Privileges const & priv)
	{
		return priv.stat.match(path);
	}

	using PrivilegeConfig::ExecPatterns;

	inline bool is_special_direntry(String const & s)
	{
		return s == "." || s == "..";
	}

	/*
	 * Get a string array from the monitor connection.
	 *
	 * @param max_len The maximum length of all arguments (including null
	 * terminators).
	 *
	 * @returns The generated StringArray and true if all arguments were read
	 * and less than the maximum length.  If the maximum size was exceeded, an
	 * empty StringArray and false are returned.
	 *
	 * @throws IPCIOException if not all arguments could be read
	 */
	std::pair<StringArray, bool>
	ipcio_get_strarr(IPCIO & conn, std::size_t max_len)
	{
		bool size_ok = true;
		std::size_t arrlen;
		ipcio_get(conn, arrlen);
		StringArray arr;
		std::size_t size_available = max_len;
		for (std::size_t i = 0; i < arrlen; ++i)
		{
			String s;
			// A single string may consume the entire available size.  This is
			// acceptable, but further arguments will cause the size to exceed the
			// maximum.
			ipcio_get(conn, s, max_len);
			if (size_ok)
			{
				// These sizes have one added because the trailing nul byte counts
				// as part of the total length limit.
				if( (s.length() + 1) < size_available )
				{
					arr.push_back(s);
					size_available -= s.length() + 1;
				}
				else
				{
					StringArray().swap(arr);
					size_ok = false;
				}
			}
		}
		return std::make_pair(arr, size_ok);
	}

	class StringCache
	{
	public:
		StringCache(unsigned max_size)
		: m_max_size(max_size == 0 ? 1 : max_size)
		{
		}

		bool has(String const & s) const
		{
			return m_set.find(s) != m_set.end();
		}

		void insert(String const & s);

	private:
		typedef std::set<String> set_t;
		set_t m_set;
		std::deque<set_t::iterator> m_fifo;
		unsigned m_max_size;
	};

	void StringCache::insert(String const & s)
	{
		std::pair<set_t::iterator, bool> x = m_set.insert(s);
		if (x.second)
		{
			// If necessary to make room in cache, toss out least-recently
			// inserted entry (FIFO replacement strategy).
			while (m_set.size() > m_max_size)
			{
				m_set.erase(m_fifo.front());
				m_fifo.pop_front();
			}
			m_fifo.push_back(x.first);
		}
	}

	class SecurePathCache
	{
	public:
		SecurePathCache(unsigned max_size, UserId uid)
		: m_secure_paths(max_size),
		  m_secure_dirs(max_size),
		  m_uid(uid)
		{
		}

		bool is_secure(String const & path);

	private:
		// Paths known to be secure
		StringCache m_secure_paths;

		// Directories known to be acceptable for containing secure files
		// (either only root can modify directory contents, or directory has
		// sticky bit set so that users can only rename/unlink their own files).
		StringCache m_secure_dirs;

		UserId m_uid;
	};

	bool SecurePathCache::is_secure(String const & path)
	{
		using namespace FileSystem::Path;
		if (m_secure_paths.has(path))
		{
			return true;
		}
		std::size_t pos = path.lastIndexOf('/');
		OW_ASSERT(pos != String::npos);
		String dir_name = path.substring(0, pos == 0 ? 1 : pos);
		ESecurity sec;
		if (m_secure_dirs.has(dir_name))
		{
			String relpath = path.substring(pos + 1);
			sec = security(dir_name, relpath, m_uid).first;
		}
		else
		{
			sec = security(path, m_uid).first;
			if (sec != E_INSECURE)
			{
				m_secure_dirs.insert(dir_name);
			}
		}
		if (sec == E_INSECURE)
		{
			return false;
		}
		m_secure_paths.insert(path);
		if (sec == E_SECURE_DIR)
		{
			m_secure_dirs.insert(path);
		}
		return true;
	}

	class Monitor
	{
	public:
		Monitor(
			std::auto_ptr<IPCIO> & p_conn,
			std::auto_ptr<Privileges> & p_priv,
			char const * monitor_path, char const * config_dir,
			UserId uid
		);

		void run();
		bool set_logger_from_ipc(String const & app_name, LoggerSpec & lspec);
		void set_logger_from_spec(String const & app_name, LoggerSpec const & lspec);
		void release();

		struct Restart : public Exec::PreExec::DontCatch
		{
			int errnum;
			int monitor_desc;
			String app_name;
			String user_name; 
		};

	private:
		void run_body();

		void open();
		void stat();
		void lstat();
		void read_dir();
		void read_link();
#if 0
		void check_path();
#endif
		void rename();
		void removeFile();
		void removeDirectory();
		void monitoredSpawn();
		void monitoredUserSpawn();
		void kill();
		void pollStatus();
		void userSpawn();
		void done();

		// Check that the given path is valid.  This will clean the path if it
		// needs it.  If the path is not valid for any reason, a CheckException
		// is thrown with an error code of PrivilegeManager::E_INVALID_PATH
		void force_valid_path(String& path, char const * op);

		void spawn_and_return(
			String const & exec_path,
			StringArray const & argv, StringArray const & envp,
			Exec::PreExec & pre_exec
		);

		void reportException(const String& prefix, const Exception& e);

		IPCIO & conn()
		{
			return *m_pconn;
		}

		Privileges const & priv() const
		{
			return *m_ppriv;
		}

		typedef std::map<ProcId, ProcessRef> proc_map_t;

		std::auto_ptr<IPCIO> m_pconn;
		std::auto_ptr<Privileges> m_ppriv;
		bool m_logger_set;
		char const * m_monitor_path;
		char const * m_config_dir;
		proc_map_t m_proc_map;
		SecurePathCache m_secure_paths;
		bool m_done;
	}; // end class Monitor

	Monitor::Monitor(
		std::auto_ptr<IPCIO> & p_conn,
		std::auto_ptr<Privileges> & p_priv,
		char const * monitor_path, char const * config_dir,
		UserId uid
	)
	: m_pconn(p_conn),
	  m_ppriv(p_priv),
	  m_logger_set(false),
	  m_monitor_path(monitor_path),
	  m_config_dir(config_dir),
	  m_proc_map(),
	  m_secure_paths(PATH_CACHE_SIZE, uid),
	  m_done(false)
	{
	}

	struct ReleaseProc
	{
		template <typename KV>
		void operator()(KV const & kv) const
		{
			kv.second->release();
		}
	};

	void Monitor::release()
	{
		std::for_each(m_proc_map.begin(), m_proc_map.end(), ReleaseProc());
		conn().close();
	}

	void Monitor::run_body()
	{
		ECommand cmd;
		while (!m_done && ipcio_get(conn(), cmd, IPCIO::E_RETURN_FALSE_ON_EOF))
		{
			switch (cmd)
			{
				case PrivilegeCommon::E_CMD_OPEN :                 this->open(); break;
				case PrivilegeCommon::E_CMD_STAT :                 this->stat(); break;
				case PrivilegeCommon::E_CMD_LSTAT :                this->lstat(); break;
				case PrivilegeCommon::E_CMD_READ_DIR :             this->read_dir(); break;
				case PrivilegeCommon::E_CMD_READ_LINK :            this->read_link(); break;
#if 0
				case PrivilegeCommon::E_CMD_CHECK_PATH :           this->check_path(); break;
#endif
				case PrivilegeCommon::E_CMD_RENAME :               this->rename(); break;
				case PrivilegeCommon::E_CMD_REMOVE_FILE :          this->removeFile(); break;
				case PrivilegeCommon::E_CMD_REMOVE_DIR :           this->removeDirectory(); break;
				case PrivilegeCommon::E_CMD_MONITORED_SPAWN :      this->monitoredSpawn(); break;
				case PrivilegeCommon::E_CMD_MONITORED_USER_SPAWN : this->monitoredUserSpawn(); break;
				case PrivilegeCommon::E_CMD_KILL :                 this->kill(); break;
				case PrivilegeCommon::E_CMD_POLL_STATUS :          this->pollStatus(); break;
				case PrivilegeCommon::E_CMD_USER_SPAWN :           this->userSpawn(); break;
				case PrivilegeCommon::E_CMD_DONE :                 this->done(); break;

			default:
				reportError(conn(), "Unknown operation requested", PrivilegeManager::E_INVALID_OPERATION);
				m_done = true;
				OW_LOG_FATAL_ERROR(logger, "Unknown operation requested; monitor exiting");
			}
		}
		OW_LOG_INFO(logger, Format("Exiting Monitor::run_body, m_done = %1", m_done));
	}

	void Monitor::run()
	{
		OW_LOG_INFO(logger, "Entering Monitor::run");
		try
		{
			this->run_body();
		}
		catch (Exception & e)
		{
			OW_LOG_FATAL_ERROR(logger, Format("Uncaught exception: %1", e).toString());
		}
	}

	void Monitor::force_valid_path(String& path, char const * op)
	{
		CHECKARGS(path.length() <= MAX_PATH_LENGTH,
			Format("%1: path argument too long", op),
			PrivilegeManager::E_INVALID_PATH);
		CHECKARGS(path.startsWith("/"),
			Format("%1: path argument must be an absolute path.  path=\"%2\"", op, path),
			PrivilegeManager::E_INVALID_PATH);

		String temppath = PrivilegeConfig::normalizePath(path);

		// If the path was altered, log it and use the altered version.
		if( temppath != path )
		{
			OW_LOG_DEBUG3(logger, Format("Cleaned path provided to monitor. Original: \"%1\", cleaned: \"%2\"", path, temppath));
			path = temppath;
		}

		CHECKARGS((path.indexOf("/../") == String::npos) && !path.endsWith("/.."),
			Format("%1: path argument must not contain \"..\".  path=\"%2\"", op, path),
			PrivilegeManager::E_INVALID_PATH);
	}

	void Monitor::done()
	{
		OW_LOG_INFO(logger, "REQ done");
		m_done = true;
	}

	bool get_logger_spec(IPCIO & io, LoggerSpec & ls)
	{
		bool do_logging;
		ipcio_get(io, do_logging);
		if (!do_logging)
		{
			return false;
		}
		size_t num_categories;
		ipcio_get(io, num_categories);
		for (size_t i = 0; i < num_categories; ++i)
		{
			String cat;
			ipcio_get(io, cat, MAX_CATEGORY_LENGTH + 1);
			if (i <= MAX_CATEGORIES)
			{
				ls.categories.push_back(cat);
			}
		}
		ipcio_get(io, ls.message_format, MAX_FORMAT_LENGTH + 1);
		ipcio_get(io, ls.file_name, MAX_PATH_LENGTH + 1);
		ipcio_get(io, ls.max_file_size);
		ipcio_get(io, ls.max_backup_index);
		io.get_sync();
		return true;
	}

	void check_logger_spec(LoggerSpec const & ls, bool & m_done)
	{
		// The name of the second argument (m_done) must match the
		// name of the same data member in the Monitor class, for
		// the CHECKARGS macro to work correctly.

		CHECKARGS(ls.categories.size() <= MAX_CATEGORIES,
			"set_logger: too many categories",
			PrivilegeManager::E_INVALID_SIZE);
		for (size_t i = 0; i < ls.categories.size(); ++i)
		{
			CHECKARGS(ls.categories[i].length() <= MAX_CATEGORY_LENGTH,
				"set_logger: category name too long",
				PrivilegeManager::E_INVALID_SIZE);
		}
		CHECKARGS(ls.message_format.length() <= MAX_FORMAT_LENGTH,
			"set_logger: format too long",
			PrivilegeManager::E_INVALID_SIZE);
		CHECKARGS(ls.file_name.length() <= MAX_PATH_LENGTH,
			"set_logger: log file path too long",
			PrivilegeManager::E_INVALID_SIZE);
	}

	void Monitor::set_logger_from_spec(
		String const & app_name, LoggerSpec const & ls)
	{
		StringArray components;
		components.push_back("*");
		LogAppenderRef log_appender = new MultiProcessFileAppender(
			components, ls.categories, ls.file_name, ls.message_format,
			ls.max_file_size, ls.max_backup_index
		);
		LogAppender::setDefaultLogAppender(log_appender);
		logger = Logger("ow.mon." + app_name);
		m_logger_set = true;
	}

	bool Monitor::set_logger_from_ipc(String const & app_name, LoggerSpec & ls)
	{
		if (!get_logger_spec(conn(), ls))
		{
			return false;
		}

		try
		{
			CHECKARGS(!m_logger_set, "set_logger: logger already set", PrivilegeManager::E_ALREADY_INITIALIZED);
			check_logger_spec(ls, m_done);
			this->force_valid_path(ls.file_name, "set_logger");
			String dir_name = FileSystem::Path::dirname(ls.file_name);
			CHECKARGS(m_secure_paths.is_secure(dir_name),
				"set_logger: log file dir " + dir_name + " is insecure",
				PrivilegeManager::E_INVALID_SECURITY);
			// Don't check log file itself, because race conditions we
			// may encounter during log rotation may cause us to throw a
			// "file not found" exception.
			this->set_logger_from_spec(app_name, ls);

			ipcio_put(conn(), PrivilegeCommon::E_OK);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("setLogger: ", e);
		}
		return true;
	}

	void Monitor::open()
	{
		String path;
		PrivilegeManager::OpenFlags flags;
		PrivilegeManager::OpenPerms perms;
		ipcio_get(conn(), path, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), flags);
		ipcio_get(conn(), perms);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ open, path=%1, flags=%2, perms=%3", path, flags, perms).toString());
		try
		{
			this->force_valid_path(path, "open");
			CHECKARGS(has_open_priv(path, flags, priv()),
				Format("open: insufficient privileges: file=\"%1\" flags=%2, perms=%3", path, flags, perms).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			char const * cpath = path.c_str();
			AutoDescriptor d;

			int pflags = 0;
			// If any other posix flags are added, add them here.
			if( flags & PrivilegeManager::posix_nonblock )
			{
				pflags |= O_NONBLOCK;
			}

			switch (flags & PrivilegeManager::iota)
			{
				case PrivilegeManager::in:
					d.reset(::open(cpath, pflags | O_RDONLY));
					break;
				case PrivilegeManager::out:
					d.reset(::open(cpath, pflags | O_WRONLY | O_CREAT | O_TRUNC, perms));
					break;
				case PrivilegeManager::out_trunc:
					d.reset(::open(cpath, pflags | O_WRONLY | O_CREAT | O_TRUNC, perms));
					break;
				case PrivilegeManager::out_app:
					d.reset(::open(cpath, pflags | O_WRONLY | O_CREAT | O_APPEND, perms));
					break;
				case PrivilegeManager::in_out:
					d.reset(::open(cpath, pflags | O_RDWR));
					break;
				case PrivilegeManager::in_out_trunc:
					d.reset(::open(cpath, pflags | O_RDWR | O_CREAT | O_TRUNC, perms));
					break;
				case PrivilegeManager::in_out_app:
					d.reset(::open(cpath, pflags | O_RDWR | O_CREAT | O_APPEND, perms));
					break;
				default:
					CHECKARGS(false, Format("open: illegal flags parameter: %1", perms).c_str(), PrivilegeManager::E_INVALID_PARAMETER);
			}
			CHECK_ERRNO(d.get() >= 0, Format("open: \"%1\"", path));
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			conn().put_handle(d.get());
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("open: ", e);
		}
	}

	void Monitor::stat()
	{
		String path;
		ipcio_get(conn(), path, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ stat, path=%1", path).toString());
		try
		{
			this->force_valid_path(path, "stat");
			CHECKARGS(has_stat_priv(path, priv()),
				Format("stat: insufficient privileges: file=\"%1\"", path).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			char const * cpath = path.c_str();

			struct stat statbuf;

			int result = ::stat(cpath, &statbuf);

			CHECK_ERRNO(result == 0, Format("stat: \"%1\"", path));
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), statbuf);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("stat: ", e);
		}
	}

	void Monitor::lstat()
	{
		String path;
		ipcio_get(conn(), path, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ lstat, path=%1", path).toString());
		try
		{
			this->force_valid_path(path, "lstat");
			CHECKARGS(has_stat_priv(path, priv()),
				Format("lstat: insufficient privileges: file=\"%1\"", path).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			char const * cpath = path.c_str();

			struct stat statbuf;

			int result = ::lstat(cpath, &statbuf);

			CHECK_ERRNO(result == 0, Format("lstat: \"%1\"", path));
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), statbuf);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("lstat: ", e);
		}
	}

	void Monitor::read_dir()
	{
		String dirpath;
		PrivilegeManager::ReadDirOptions opt;
		ipcio_get(conn(), dirpath, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), opt);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ readDirectory, path=%1, opt=%2", dirpath, opt).toString());
		try
		{
			this->force_valid_path(dirpath, "readDirectory");
			CHECKARGS(priv().read_dir.match(dirpath),
				Format("readDirectory: insufficient privileges: dirpath=\"%1\" opt=%2", dirpath, opt).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			StringArray dir_entries;
			bool ok = FileSystem::getDirectoryContents(dirpath, dir_entries);
			CHECK(ok, "readDirectory: could not read " + dirpath, PrivilegeManager::E_OPERATION_FAILED);
			if (opt == PrivilegeManager::E_OMIT_SPECIAL)
			{
				StringArray::iterator end = dir_entries.end();
				StringArray::iterator beg = dir_entries.begin();
				dir_entries.erase(
					std::remove_if(beg, end, is_special_direntry), end);
			}

			ipcio_put(conn(), PrivilegeCommon::E_OK);
			std::size_t n = dir_entries.size();
			ipcio_put(conn(), n);
			for (std::size_t i = 0; i < n; ++i)
			{
				ipcio_put(conn(), dir_entries[i]);
			}
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("readDir: ", e);
		}
	}

	void Monitor::read_link()
	{
		String lpath;
		ipcio_get(conn(), lpath, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ readLink, path=%1", lpath).toString());
		try
		{
			this->force_valid_path(lpath, "readLink");
			CHECKARGS(priv().read_link.match(lpath),
				Format("readLink: insufficient privileges: path=\"%1\"", lpath).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			String s = FileSystem::readSymbolicLink(lpath);
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), s);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("readLink: ", e);
		}
	}

	void Monitor::rename()
	{
		String old_path, new_path;
		ipcio_get(conn(), old_path, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), new_path, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ rename, oldpath=%1, newpath=%2", old_path, new_path).toString());

		try
		{
			this->force_valid_path(old_path, "rename");
			CHECKARGS(priv().rename_from.match(old_path),
				Format("rename: insufficient privileges for source path: %1", old_path).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			this->force_valid_path(new_path, "rename");
			CHECKARGS(priv().rename_to.match(new_path),
				Format("rename: insufficient privileges for dest path: %1", new_path).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			bool ok = FileSystem::renameFile(old_path, new_path);
			CHECK(ok,
				"rename: could not rename " + old_path + " to " + new_path, PrivilegeManager::E_OPERATION_FAILED);

			ipcio_put(conn(), PrivilegeCommon::E_OK);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("rename: ", e);
		}
	}

	void Monitor::removeFile()
	{
		String path;
		ipcio_get(conn(), path, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ removeFile, path=%1", path).toString());
		try
		{
			this->force_valid_path(path, "removeFile");
			CHECKARGS(priv().remove_file.match(path),
				Format("removeFile: insufficient privileges: path=\"%1\"", path).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			bool ok = FileSystem::removeFile(path);

			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), ok);
			ipcio_put(conn(), errno);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("removeFile: ", e);
		}
	}

	void Monitor::removeDirectory()
	{
		String path;
		ipcio_get(conn(), path, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ removeDirectory, path=%1", path).toString());
		try
		{
			this->force_valid_path(path, "removeDirectory");
			CHECKARGS(priv().remove_dir.match(path),
				Format("removeDirectory: insufficient privileges: path=\"%1\"", path).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);

			bool ok = FileSystem::removeDirectory(path);
			OW_LOG_INFO(logger, Format("removeDirectory: retval=%1", ok));

			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), ok);
			ipcio_put(conn(), errno);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("removeDirectory: ", e);
		}
	}

	inline int output_handle(UnnamedPipeRef const & p)
	{
		return p->getOutputDescriptor();
	}

	inline int input_handle(UnnamedPipeRef const & p)
	{
		return p->getInputDescriptor();
	}

	struct SMPolicy : public PrivilegeCommon::SpawnMonitorPolicy
	{
		SMPolicy(char const * user_name = 0)
		  : m_user_name(user_name) {}
		virtual char const * check_config_dir(char const * config_dir);

		virtual void spawn(
			int child_desc, int parent_desc,
			char const * config_dir, char const * app_name
		);
        char const * m_user_name; 
		int m_monitor_desc;
	};

	char const * SMPolicy::check_config_dir(char const * config_dir)
	{
		// We've already checked config_dir
		return config_dir;
	}

	void SMPolicy::spawn(
		int child_desc, int parent_desc,
		char const * config_dir, char const * app_name
	)
	{
		// Caller owns child_desc and parent_desc
		::pid_t child_pid = ::fork();
		CHECK_ERRNO(child_pid >= 0, "fork");
		if (child_pid == 0) // child (monitor)
		{
			Monitor::Restart r;
			r.errnum = ::dup2(child_desc, m_monitor_desc) >= 0 ? 0 : errno;
			r.monitor_desc = m_monitor_desc;
			r.app_name = app_name;
			r.user_name = m_user_name; 
			throw r;
		}
		else // parent (monitored process)
		{
			int e = ::dup2(parent_desc, m_monitor_desc);
			CHECK_ERRNO(e >= 0, "dup2");
		}
	}

	class ME_PreExec : public Exec::PreExec
	{
	public:
		// REQUIRE: monitor_desc is an unused descriptor, and
		// the environment we pass to Exec::spawn contains an entry
		// of the form <s>=<d>, where <s> is the value of
		// PrivilegeCommon::CONN_DESC_ENV_VAR, and <d> is the character
		// sequence representing the integer monitor_desc.get().
		//
		ME_PreExec(
			char const * config_dir, char const * app_name,
			Monitor * p_monitor, AutoDescriptor & monitor_desc,
			char const * user_name = 0
		);
		virtual bool keepStd(int d) const;
		virtual void call(pipe_pointer_t const pparr[]);

	private:
		char const * m_config_dir;
		char const * m_app_name;
		char const * m_user_name; 
		Monitor * m_p_monitor;
		AutoDescriptor m_monitor_desc;
	};

	ME_PreExec::ME_PreExec(
		char const * config_dir, char const * app_name,
		Monitor * p_monitor, AutoDescriptor & monitor_desc,
		char const * user_name
	)
	: m_config_dir(config_dir),
	  m_app_name(app_name),
	  m_user_name(user_name),
	  m_p_monitor(p_monitor),
	  m_monitor_desc(monitor_desc)
	{
	}

	bool ME_PreExec::keepStd(int) const
	{
		return true;
	}

#if 0
	void check_pre_exec(bool b, char const * errmsg, bool use_errno = true)
	{
		if (!b)
		{
			Exec::PreExec::Error e;
			e.error_num = use_errno ? errno : 0;
			SafeCString::strcpy_trunc(e.message, errmsg);
			throw e;
		}
	}
#endif

	void ME_PreExec::call(pipe_pointer_t const pparr[])
	{
		// Make sure processes managed by monitor aren't killed when
		// monitor object dtor is called in forked monitor process, and
		// close descriptor that the (parent) monitor uses to communicate with
		// its client.
		//
		m_p_monitor->release();

		// Create new monitor process
		SMPolicy policy(m_user_name);
		policy.m_monitor_desc = m_monitor_desc.release();
		PrivilegeCommon::spawn_monitor(m_config_dir, m_app_name, policy); 

		// Signals already set to defaults

		// This is after the new monitor is created. We don't want it in the new process group.
		PreExec::setNewProcessGroup();

		// Close unneeded descriptors that aren't already being auto-closed.
		PreExec::closePipesOnExec(pparr);

		PreExec::setupStandardDescriptors(pparr);
	}

	void putenv_monitor_desc(StringArray & envarr, int desc)
	{
		Format fmt("%1=%2", PrivilegeCommon::CONN_DESC_ENV_VAR, desc);
		envarr.push_back(fmt.toString());
	}

	bool bad_env_entry(String const & s)
	{
		return s.startsWith(PrivilegeCommon::CONN_DESC_ENV_ENTRY_PFX);
	}

	bool reserved_env_var_present(StringArray const & env)
	{
		return std::find_if(env.begin(), env.end(), bad_env_entry) != env.end();
	}

	bool filter_env(StringArray & env)
	{
		if (reserved_env_var_present(env))
		{
			return false;
		}
		return true;
	}

	void Monitor::spawn_and_return(
		String const & exec_path,
		StringArray const & argv, StringArray const & envp,
		Exec::PreExec & pre_exec
	)
	{
		ProcessRef p_proc =	Exec::spawn(exec_path, argv, envp, pre_exec);
		::pid_t child_pid = p_proc->pid();
		OW_LOG_INFO(logger, Format("Spawned child pid: %1", child_pid));
		m_proc_map.insert(std::make_pair(child_pid, p_proc));

		ipcio_put(conn(), PrivilegeCommon::E_OK);
		conn().put_handle(output_handle(p_proc->in()));
		conn().put_handle(input_handle(p_proc->out()));
		conn().put_handle(input_handle(p_proc->err()));
		ipcio_put(conn(), p_proc->pid());
		p_proc->in()->close();
		p_proc->out()->close();
		p_proc->err()->close();
	}

	void Monitor::monitoredSpawn()
	{
		String exec_path;
		String app_name;
		std::pair<StringArray, bool> xargv, xenvp;
		ipcio_get(conn(), exec_path, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), app_name, MAX_APPNAME_LENGTH + 1);
		xargv = ipcio_get_strarr(conn(), MAX_ARG_LENGTH);
		xenvp = ipcio_get_strarr(conn(), MAX_ENV_LENGTH);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ monitoredSpawn, exec_path=%1, app_name=%2", exec_path, app_name));
		try
		{
			this->force_valid_path(exec_path, "monitoredSpawn");
			CHECKARGS(app_name.length() <= MAX_APPNAME_LENGTH, "monitoredSpawn: app name too long", PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(xargv.second, "monitoredSpawn: argv too large", PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(xenvp.second,	"monitoredSpawn: envp too large", PrivilegeManager::E_INVALID_SIZE);
			bool reserved_env_var_absent = filter_env(xenvp.first);
			CHECKARGS(reserved_env_var_absent,
				"monitoredSpawn: reserved env var set in environment argument",
				PrivilegeManager::E_INVALID_PARAMETER);
			CHECKARGS(priv().monitored_exec.match(exec_path, xenvp.first, app_name)
				|| priv().monitored_exec_check_args.match(exec_path, xargv.first, xenvp.first, app_name),
				Format("monitoredSpawn: insufficient privileges: path=\"%1\" app=\"%2\" args={%3}, env={%4}", exec_path, app_name, simpleUntokenize(xargv.first), simpleUntokenize(xenvp.first)).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);
			CHECK(m_secure_paths.is_secure(exec_path),
				"monitoredSpawn: exec path " + exec_path + " is insecure",
				PrivilegeManager::E_INVALID_SECURITY);

			AutoDescriptor desc(::dup(0)); // get some unused descriptor
			CHECK(desc.get() >= 0, "monitoredSpawn: dup failed", PrivilegeManager::E_OPERATION_FAILED);
			putenv_monitor_desc(xenvp.first, desc.get());
			ME_PreExec pre_exec(m_config_dir, app_name.c_str(), this, desc);
			this->spawn_and_return(
				exec_path, xargv.first, xenvp.first, pre_exec);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("monitoredSpawn: ", e);
		}
	}
	
	void Monitor::monitoredUserSpawn()
	{
		String exec_path;
		String app_name;
		String user_name; 
		std::pair<StringArray, bool> xargv, xenvp;
		ipcio_get(conn(), exec_path, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), app_name, MAX_APPNAME_LENGTH + 1);
		xargv = ipcio_get_strarr(conn(), MAX_ARG_LENGTH);
		xenvp = ipcio_get_strarr(conn(), MAX_ENV_LENGTH);
		ipcio_get(conn(), user_name, MAX_USER_NAME_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ monitoredUserSpawn, exec_path=%1, app_name=%2, user=%3", 
								   exec_path, app_name, user_name));
		try
		{
			this->force_valid_path(exec_path, "monitoredUserSpawn");
			CHECKARGS(user_name.length() <= MAX_USER_NAME_LENGTH,
				"monitoredUserSpawn: user name too long",
				PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(app_name.length() <= MAX_APPNAME_LENGTH, "monitoredUserSpawn: app name too long", PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(xargv.second, "monitoredUserSpawn: argv too large", PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(xenvp.second, "monitoredUserSpawn: envp too large", PrivilegeManager::E_INVALID_SIZE);
			bool reserved_env_var_absent = filter_env(xenvp.first);
			CHECKARGS(reserved_env_var_absent,
				"monitoredUserSpawn: reserved env var set in environment argument",
				PrivilegeManager::E_INVALID_PARAMETER
			);
			CHECKARGS(priv().monitored_user_exec.match(exec_path, xenvp.first, app_name, user_name)
				|| priv().monitored_user_exec_check_args.match(exec_path, xargv.first, xenvp.first, app_name, user_name),
				Format("monitoredUserSpawn: insufficient privileges: user=%1 path=\"%2\" app=\"%3\" args={%4} env={%5}", user_name, exec_path, app_name, simpleUntokenize(xargv.first), simpleUntokenize(xenvp.first)).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);
			CHECK(m_secure_paths.is_secure(exec_path),
				"monitoredUserSpawn: exec path " + exec_path + " is insecure",
				PrivilegeManager::E_INVALID_SECURITY);

			AutoDescriptor desc(::dup(0)); // get some unused descriptor
			CHECK(desc.get() >= 0, "monitoredUserSpawn: dup failed", PrivilegeManager::E_OPERATION_FAILED);
			putenv_monitor_desc(xenvp.first, desc.get());
			ME_PreExec pre_exec(m_config_dir, app_name.c_str(), this, desc, user_name.c_str());
			this->spawn_and_return(
				exec_path, xargv.first, xenvp.first, pre_exec);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("monitoredUserSpawn: ", e);
		}
	}

	void Monitor::kill()
	{
		ProcId pid;
		int sig;
		ipcio_get(conn(), pid);
		ipcio_get(conn(), sig);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ kill, pid=%1, sig=%2", pid, sig).toString());
		try
		{
			// allow either a process or a process group (-pid)
			CHECKARGS((pid > 0) ? has(m_proc_map, pid) : has(m_proc_map, -pid), Format("kill: unknown process: %1", pid), PrivilegeManager::E_INVALID_PARAMETER);
			int rv = (::kill(pid, sig) == 0 ? 0 : errno);
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), rv);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("kill: ", e);
		}

	}

	void Monitor::pollStatus()
	{
		ProcId pid;
		ipcio_get(conn(), pid);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ pollStatus, pid=%1", pid).toString());
		try
		{
			CHECKARGS(has(m_proc_map, pid), Format("pollStatus: unknown process: %1", pid), PrivilegeManager::E_INVALID_PARAMETER);
			ProcId wpid;
			int status;
			do
			{
				wpid = ::waitpid(pid, &status, WNOHANG | WUNTRACED);
			} while (wpid < 0 && errno == EINTR);
			CHECK_ERRNO(wpid >= 0, "wait_pid");
			if (Process::Status(wpid, status).terminated())
			{
				// Prevent future waitpids from being called when the destructor happens.
				m_proc_map[pid]->release();

				// Remove the process from the selection of processes currently maintained by this instance of the monitor.
				m_proc_map.erase(pid);
			}
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), wpid);
			ipcio_put(conn(), status);
			conn().put_sync();
		}
		catch (const Exception& e)
		{
			reportException("pollStatus: ", e);
		}
	}

	class UE_PreExec : public Exec::PreExec
	{
	public:
		UE_PreExec(String const & user_name, String const & working_dir);
		virtual bool keepStd(int d) const;
		virtual void call(pipe_pointer_t const pparr[]);

	private:
		String m_user_name;
		String m_working_dir;
	};

	UE_PreExec::UE_PreExec(String const & user_name, String const & working_dir)
	: m_user_name(user_name),
	  m_working_dir(working_dir)
	{
	}

	bool UE_PreExec::keepStd(int) const
	{
		return true;
	}

	void UE_PreExec::call(pipe_pointer_t const pparr[])
	{
		// Change identity to the specified user
		Secure::runAs(m_user_name.c_str());

		// Signals already set to defaults

		PreExec::setNewProcessGroup();

		// Close unneeded descriptors that aren't already being auto-closed.
		PreExec::closePipesOnExec(pparr);

		// Connect stdin, stdout, and stderr
		PreExec::setupStandardDescriptors(pparr);

		// set up
		if (!m_working_dir.empty() && ::chdir(m_working_dir.c_str()) != 0)
		{
			int errnum = errno;
			OW_THROW_ERRNO_MSG1(FileSystemException,
				Format("Cannot chdir to %1", m_working_dir), errnum);
		}
	}

	void Monitor::userSpawn()
	{
		String user_name;
		String exec_path;
		String working_dir;
		std::pair<StringArray, bool> xargv, xenvp;

		ipcio_get(conn(), exec_path, MAX_PATH_LENGTH + 1);
		xargv = ipcio_get_strarr(conn(), MAX_ARG_LENGTH);
		xenvp = ipcio_get_strarr(conn(), MAX_ENV_LENGTH);
		ipcio_get(conn(), user_name, MAX_USER_NAME_LENGTH + 1);
		ipcio_get(conn(), working_dir, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ userSpawn, exec_path=%1, user=%2", exec_path, user_name));
		try
		{
			this->force_valid_path(exec_path, "userSpawn");
			CHECKARGS(user_name.length() <= MAX_USER_NAME_LENGTH,
				"userSpawn: user name too long",
				PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(working_dir.length() <= MAX_PATH_LENGTH,
				"userSpawn: working dir too long",
				PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(xargv.second, "userSpawn: argv too large", PrivilegeManager::E_INVALID_SIZE);
			CHECKARGS(xenvp.second, "userSpawn: envp too large", PrivilegeManager::E_INVALID_SIZE);
			bool reserved_env_var_absent = filter_env(xenvp.first);
			CHECKARGS(reserved_env_var_absent,
				"userSpawn: reserved env var set in environment argument",
				PrivilegeManager::E_INVALID_PARAMETER);
			CHECKARGS(priv().user_exec.match(exec_path, xenvp.first, user_name)
				|| priv().user_exec_check_args.match(exec_path, xargv.first, xenvp.first, user_name),
				Format("userSpawn: insufficient privileges: user=%1 path=\"%2\" dir=\"%3\" args={%4} env={%5}", user_name, exec_path, working_dir, simpleUntokenize(xargv.first), simpleUntokenize(xenvp.first)).c_str(),
				PrivilegeManager::E_INSUFFICIENT_PRIVILEGES);
			CHECK(m_secure_paths.is_secure(exec_path),
				"userSpawn: exec path " + exec_path + " is insecure",
				PrivilegeManager::E_INVALID_SECURITY);
			CHECK(working_dir.empty() || working_dir.startsWith("/"),
				"userSpawn: working dir must be an absolute path",
				PrivilegeManager::E_INVALID_PATH);

			UE_PreExec pre_exec(user_name, working_dir);
			this->spawn_and_return(
				exec_path, xargv.first, xenvp.first, pre_exec);

			conn().put_sync();
		}
		catch (Exception & e)
		{
			reportException("userSpawn: ", e);
		}
	}

	void Monitor::reportException(const String& prefix, const Exception& e)
	{
		bool logAsError = false;
		if (m_done)
		{
			logAsError = true;
		}
		reportExceptionAux(conn(), prefix, e, logAsError);
	}

#if 0
	void Monitor::check_path()
	{
		*** FILL IN ***;
	}
#endif

	class MonitorIncludeHandler : public OpenWBEM::PrivilegeConfig::IncludeHandler
	{
	public:
		MonitorIncludeHandler(const String& configDir)
		: m_configDir(configDir)
		{
		}

		virtual std::istream* getInclude(const String& includeParam)
		{
			String cd = m_configDir.endsWith(OW_FILENAME_SEPARATOR) ? m_configDir : m_configDir + OW_FILENAME_SEPARATOR;
			String path = cd + includeParam;
			using namespace FileSystem::Path;
			CHECK0(security(m_configDir, includeParam, ROOT_USERID).first != E_INSECURE,
				"Config file " + path + " insecure");

			Reference<std::ifstream> is(new std::ifstream(path.c_str()));
			CHECK0(*is, "Could not open privilege config file " + path);
			m_includeStorage.push_back(is);
			return is.getPtr();
		}

		virtual void endInclude()
		{
			m_includeStorage.pop_back();
		}
	private:
		String m_configDir;
		// This class has to maintain ownership of the ifstreams.
		std::vector<Reference<std::ifstream> > m_includeStorage;
	};

	void readConfigFile(Privileges& privs, const String& path, const String& configDir)
	{
		std::ifstream is(path.c_str());
		CHECK0(is, "Could not open privilege config file " + path);

		OW_LOG_INFO(logger, Format("Reading privileges from %1(%2)", path, FileSystem::Path::realPath(path)));

		MonitorIncludeHandler mih(configDir);
		PrivilegeConfig::openwbem_privconfig_Lexer lexer(is, mih, path);
		PrivilegeConfig::ParseError err;

		int code = openwbem_privconfig_parse(&privs, &err, &lexer);
		CHECK0(code == 0,
			Format("Parse error in privilege config file %1 (line %2, column %3): %4",
				path, err.line, err.column, err.message));

	}

	// REQUIRE:	config_dir has already been verified to be a secure directory
	// and is in canonical form (see FileSystem::Path::security).
	//
	std::auto_ptr<Privileges> read_config(
		String const & config_dir, String const & app_name
	)
	{
		String cd = config_dir.endsWith(OW_FILENAME_SEPARATOR) ? config_dir : config_dir + OW_FILENAME_SEPARATOR;
		String path = cd + app_name;
		using namespace FileSystem::Path;
		CHECK0(security(config_dir, app_name, ROOT_USERID).first != E_INSECURE,
			"Config file " + path + " insecure");

		std::auto_ptr<Privileges> p_priv(new Privileges());
		if (FileSystem::isDirectory(path))
		{
			StringArray privFiles;
			if (!FileSystem::getDirectoryContents(path, privFiles))
			{
				OW_THROW_ERRNO_MSG(CheckException, Format("Failed to get contents of directory: %1", path));
			}
			for (size_t i = 0; i < privFiles.size(); ++i)
			{
				const String& s(privFiles[i]);
				if (s == "." || s == "..")
				{
					continue;
				}

				String privFile = path + OW_FILENAME_SEPARATOR + s;
				CHECK0(security(path, s, ROOT_USERID).first != E_INSECURE, "Config file " + privFile + " insecure");
				readConfigFile(*p_priv, privFile, config_dir);
			}
		}
		else
		{
			readConfigFile(*p_priv, path, config_dir);
		}
		return p_priv;
	}

	String username(
		String const & cfg_user, char const * arg_user, String const & last_user
	)
	{
		if (!last_user.empty()) // we're doing a monitoredSpawn
		{
			return cfg_user.empty() ? last_user : cfg_user;
		}
		bool have_arg_user = arg_user && *arg_user;
		bool have_cfg_user = !cfg_user.empty();
		CHECK0(have_cfg_user || have_arg_user, "must specify user name");
		CHECK0(!have_cfg_user || !have_arg_user || cfg_user == arg_user,
			"user name specified as both " + cfg_user + " and " + arg_user);
		return have_cfg_user ? cfg_user : String(arg_user);
	}

	int aux_main(int argc, char * * argv)
	{
		::umask(0);

		// Create a new process group so that any signals sent by the shell to owcimomd will not be sent to owprivilegemonitor as well.
		// This is useful when running owcimomd in debug mode and using Ctrl-C to shut it down.
		::setpgid(0, 0);

		// until the real logger is created we will log all errors to syslog
		LogAppender::setDefaultLogAppender(getSyslogAppender());

		WaitpidThreadFix::setWaitpidThreadFixEnabled(false);

		PrivilegeCommon::DescriptorInfo x
			= PrivilegeCommon::monitor_descriptor();
		if (x.errnum != 0)
		{
			OW_LOG_ERROR(logger, Format("monitor_descriptor() returned an error: %1", x.errnum));
			return x.errnum;
		}
		int client_descriptor = x.descriptor;
		int exit_status;
		String config_dir, app_name, user_name, mon_user_name;
		LoggerSpec ls;
		bool has_logger = false;
		enum { E_FIRST, E_REPEAT, E_DONE } iteration = E_FIRST;
		// All iterations after the first are the result of a monitoredSpawn
		// call.
		while (iteration != E_DONE)
		{
			try
			{
				std::auto_ptr<IPCIO> p_conn(
					new IPCIO(AutoDescriptor(::dup(client_descriptor)), Timeout::infinite));
				std::auto_ptr<Privileges> p_priv;
				char const * monitor_path = NULL;
				exit_status = PrivilegeCommon::EXIT_FAILED_INIT;
				UserId uid = 0;
				try
				{
					CHECK0(argc == 3 || argc == 4,
						"Wrong number of arguments for monitor executable");
					char const * monitor_path = argv[0];
					config_dir = argv[1];
					if (iteration == E_FIRST)
					{
						app_name = argv[2];
					}
					p_priv = read_config(config_dir, app_name);
					char const * arg_user = (argc == 3 ? 0 : argv[3]);
					user_name =
						username(p_priv->unpriv_user, arg_user, user_name);
					uid = getuid(user_name);
					ipcio_put(*p_conn, PrivilegeCommon::E_OK);
					ipcio_put(*p_conn, user_name);
					ipcio_put(*p_conn, ::getpid());
					exit_status = 0;
				}
				catch (CheckException & e)
				{
					reportExceptionAux(*p_conn, "Setup Exception: ", e, true);
				}
				p_conn->put_sync();

				if (exit_status == 0)
				{
					Monitor mon(
						p_conn, p_priv, monitor_path, config_dir.c_str(), uid);
					if (iteration == E_FIRST)
					{
						has_logger = mon.set_logger_from_ipc(app_name, ls);
					}
					else if (has_logger)
					{
						mon.set_logger_from_spec(app_name, ls);
					}
					if (iteration == E_REPEAT && !mon_user_name.empty())
					{
						Secure::runAs(mon_user_name.c_str()); 
					}
					mon.run();
				}
				iteration = E_DONE;
			}
			catch (Monitor::Restart & e)
			{
				if (e.errnum != 0)
				{
					OW_LOG_ERROR(logger, Format("Monitor restart failed, errnum=%1", e.errnum));
					return PrivilegeCommon::EXIT_FAILED_INIT;
				}
				client_descriptor = e.monitor_desc;
				app_name = e.app_name;
				mon_user_name = e.user_name; 
				iteration = E_REPEAT;
			}
		}

		return exit_status;
	}
}

extern int openwbem_privconfig_debug;


// argv[1]: config_dir; must have already been verified as secure and must
//   be in canonical form as described for FileSystem::Path::realPath.
// argv[2]: app_name; name of config file for the application we are monitoring.
//
int main(int argc, char * * argv)
{
#ifdef OW_DEBUG
	if( getenv("yydebug") != NULL )
	{
		openwbem_privconfig_debug = 1;
	}
#endif

	try
	{
		int ec = aux_main(argc, argv);
		if (ec != 0)
		{
			OW_LOG_ERROR(logger, Format("Monitor exited with error code %1", ec));
		}
		else
		{
			OW_LOG_INFO(logger, "Monitor exited normally");
		}
		return ec;
	}
	catch (IPCIOException & e)
	{
		OW_LOG_ERROR(logger, Format("Monitor threw exception: %1", e));
		return PrivilegeCommon::EXIT_IPCIO_ERROR;
	}
	catch (Exception & e)
	{
		OW_LOG_ERROR(logger, Format("Monitor threw exception: %1", e));
		return PrivilegeCommon::EXIT_UNCAUGHT_EXCEPTION;
	}
	catch (...)
	{
		OW_LOG_ERROR(logger, "Monitor threw unknown exception");
		return PrivilegeCommon::EXIT_UNCAUGHT_EXCEPTION;
	}
}
