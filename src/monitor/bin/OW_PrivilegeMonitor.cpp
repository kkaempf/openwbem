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
#include "blocxx/WaitpidThreadFix.hpp"

#include <algorithm>
#include <deque>
#include <fstream>
#include <fcntl.h>
#include <pwd.h>
#include <set>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace OpenWBEM;
using OpenWBEM::PrivilegeConfig::Privileges;
typedef PrivilegeCommon::ECommand ECommand;

#define CHECK0(tst, msg) CHECK_E(tst, CheckException, msg)
#define CHECK(tst, msg) CHECK0(tst, add_prefix(msg))

#define CHECK0_ERRNO(tst, msg) CHECK_ERRNO_E(tst, CheckException, msg)
#define CHECK_ERRNO(tst, msg) CHECK0_ERRNO(tst, add_prefix(msg))

// CHECKARGS differs from CHECK only in that it also causes the monitor to quit
//
#define CHECKARGS(tst, msg) \
	WRAP_STMT(if (!(tst)) { m_done = true; THROW_MSG_E(CheckException, msg); })

#define PRIVOP_BODY try

#define PRIVOP_FINISH \
	catch (Exception & e) \
	{ \
		report_error(conn(), e); \
	} \
	conn().put_sync()

// There is a call to the SSL function ERR_remove_state in the Thread code.
// It's really only needed if you're using SSL.  To avoid a dependency on
// libssl, we define the function here to do nothing.
//
extern "C" void ERR_remove_state(unsigned long)
{
}

namespace
{
	Logger logger("none", LogAppenderRef(new NullAppender()));

	unsigned const PATH_CACHE_SIZE = 100;
	int const NUM_CMDS = static_cast<int>(PrivilegeCommon::E_NUM_MONITOR_CMDS);
	UserId const ROOT_USERID = 0;

	std::size_t const MAX_PATH_LENGTH = 1024;
	std::size_t const MAX_APPNAME_LENGTH = 1024;
	std::size_t const MAX_ARGV_LENGTH = 1024;
	std::size_t const MAX_ARG_LENGTH = 1024;
	std::size_t const MAX_ENV_LENGTH = 1024;
	std::size_t const MAX_ENVITEM_LENGTH = 1024;
	std::size_t const MAX_STATUS_MESSAGE_LENGTH = 1024;
	std::size_t const MAX_USER_NAME_LENGTH = 1024;
	std::size_t const MAX_CATEGORY_LENGTH = 128;
	std::size_t const MAX_FORMAT_LENGTH = 1024;
	std::size_t const MAX_CATEGORIES = 1024;

	char const * val_LIBPATH = std::getenv(OW_ENV_VAR_LIBPATH);
	bool have_LIBPATH = (val_LIBPATH != 0);
	String entry_LIBPATH(
		have_LIBPATH ? String(OW_ENV_VAR_LIBPATH "=") + val_LIBPATH : String()
	);

	OW_DECLARE_EXCEPTION(Check);
	OW_DEFINE_EXCEPTION(Check);

	void report_error(IPCIO & conn, Exception const & e)
	{
		if (dynamic_cast<IPCIOException const *>(&e) == 0)
		{
			OW_LOG_ERROR(logger, Format("%1", e));
			ipcio_put(conn, PrivilegeCommon::E_ERROR);
			// explicitly specify type, to protect against Exception API changes
			ipcio_put<int>(conn, e.getErrorCode());
			Format fmt("%1: %2", e.type(), e.what());
			ipcio_put(conn, fmt.c_str());
		}
		else
		{
			throw;
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

	using PrivilegeConfig::ExecPatterns;

	inline bool is_special_direntry(String const & s)
	{
		return s == "." || s == "..";
	}

	std::pair<StringArray, bool>
	ipcio_get_strarr(IPCIO & conn, std::size_t max_len, std::size_t max_str_len)
	{
		bool size_ok = true;
		std::size_t arrlen;
		ipcio_get(conn, arrlen);
		StringArray arr;
		for (std::size_t i = 0; i < arrlen; ++i)
		{
			String s;
			ipcio_get(conn, s, max_str_len + 1);
			if (size_ok)
			{
				if (i < max_len && s.length() <= max_str_len)
				{
					arr.push_back(s);
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
		void read_dir();
		void read_link();
#if 0
		void check_path();
#endif
		void rename();
		void unlink();
		void monitoredSpawn();
		void monitoredUserSpawn();
		void kill();
		void pollStatus();
		void userSpawn();
		void done();

		void check_valid_path(String const & path, char const * op);

		void spawn_and_return(
			String const & exec_path,
			StringArray const & argv, StringArray const & envp,
			Exec::PreExec & pre_exec
		);

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

#define CMDCASE(name, fct) \
	case PrivilegeCommon::E_CMD_ ## name : this->fct(); break

	void Monitor::run_body()
	{
		ECommand cmd;
		while (!m_done && ipcio_get(conn(), cmd, IPCIO::E_RETURN_FALSE_ON_EOF))
		{
			switch (cmd)
			{
				CMDCASE(OPEN, open);
				CMDCASE(READ_DIR, read_dir);
				CMDCASE(READ_LINK, read_link);
#if 0
				CMDCASE(CHECK_PATH, check_path);
#endif
				CMDCASE(RENAME, rename);
				CMDCASE(UNLINK, unlink);
				CMDCASE(MONITORED_SPAWN, monitoredSpawn);
				CMDCASE(MONITORED_USER_SPAWN, monitoredUserSpawn);
				CMDCASE(KILL, kill);
				CMDCASE(POLL_STATUS, pollStatus);
				CMDCASE(USER_SPAWN, userSpawn);
				CMDCASE(DONE, done);

			default:
				ipcio_put(conn(), PrivilegeCommon::E_ERROR);
				ipcio_put(conn(), "Unknown operation requested");
				conn().put_sync();
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

	void Monitor::check_valid_path(String const & path, char const * op)
	{
		CHECKARGS(path.length() <= MAX_PATH_LENGTH,
			String(op) + ": path argument too long");
		CHECKARGS(path.startsWith("/"),
			String(op) + ": path argument must be an absolute path");
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
			"set_logger: too many categories");
		for (size_t i = 0; i < ls.categories.size(); ++i)
		{
			CHECKARGS(ls.categories[i].length() <= MAX_CATEGORY_LENGTH,
				"set_logger: category name too long");
		}
		CHECKARGS(ls.message_format.length() <= MAX_FORMAT_LENGTH,
			"set_logger: format too long");
		CHECKARGS(ls.file_name.length() <= MAX_PATH_LENGTH,
			"set_logger: log file path too long");
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

		PRIVOP_BODY
		{
			CHECKARGS(!m_logger_set, "set_logger: logger already set");
			check_logger_spec(ls, m_done);
			this->check_valid_path(ls.file_name, "set_logger");
			String dir_name = FileSystem::Path::dirname(ls.file_name);
			CHECKARGS(m_secure_paths.is_secure(dir_name),
				"set_logger: log file dir " + dir_name + " is insecure");
			// Don't check log file itself, because race conditions we
			// may encounter during log rotation may cause us to throw a
			// "file not found" exception.
			this->set_logger_from_spec(app_name, ls);

			ipcio_put(conn(), PrivilegeCommon::E_OK);
		}
		PRIVOP_FINISH;

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
		PRIVOP_BODY
		{
			this->check_valid_path(path, "open");
			CHECKARGS(has_open_priv(path, flags, priv()),
				"open: insufficient privileges");

			char const * cpath = path.c_str();
			AutoDescriptor d;
			switch (flags & PrivilegeManager::iota)
			{
				case PrivilegeManager::in:
					d.reset(::open(cpath, O_RDONLY));
					break;
				case PrivilegeManager::out:
					d.reset(::open(cpath, O_WRONLY | O_CREAT | O_TRUNC, perms));
					break;
				case PrivilegeManager::out_trunc:
					d.reset(::open(cpath, O_WRONLY | O_CREAT | O_TRUNC, perms));
					break;
				case PrivilegeManager::out_app:
					d.reset(::open(cpath, O_WRONLY | O_CREAT | O_APPEND, perms));
					break;
				case PrivilegeManager::in_out:
					d.reset(::open(cpath, O_RDWR));
					break;
				case PrivilegeManager::in_out_trunc:
					d.reset(::open(cpath, O_RDWR | O_CREAT | O_TRUNC, perms));
					break;
				case PrivilegeManager::in_out_app:
					d.reset(::open(cpath, O_RDWR | O_CREAT | O_APPEND, perms));
					break;
				default:
					CHECKARGS(false, "open: illegal flags parameter");
			}
			CHECK_ERRNO(d.get() >= 0, "open");
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			conn().put_handle(d.get());
		}
		PRIVOP_FINISH;
	}

	void Monitor::read_dir()
	{
		String dirpath;
		PrivilegeManager::ReadDirOptions opt;
		ipcio_get(conn(), dirpath, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), opt);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ readDirectory, path=%1, opt=%2", dirpath, opt).toString());
		PRIVOP_BODY
		{
			this->check_valid_path(dirpath, "readDirectory");
			CHECKARGS(priv().read_dir.match(dirpath),
				"readDirectory: insufficient privileges");

			StringArray dir_entries;
			bool ok = FileSystem::getDirectoryContents(dirpath, dir_entries);
			CHECK(ok, "readDirectory: could not read " + dirpath);
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
		}
		PRIVOP_FINISH;
	}

	void Monitor::read_link()
	{
		String lpath;
		ipcio_get(conn(), lpath, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ readLink, path=%1", lpath).toString());
		PRIVOP_BODY
		{
			this->check_valid_path(lpath, "readLink");
			CHECKARGS(priv().read_link.match(lpath),
				"readLink: insufficient privileges");

			String s = FileSystem::readSymbolicLink(lpath);
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), s);
		}
		PRIVOP_FINISH;
	}

	void Monitor::rename()
	{
		String old_path, new_path;
		ipcio_get(conn(), old_path, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), new_path, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ rename, oldpath=%1, newpath=%2", old_path, new_path).toString());

		PRIVOP_BODY
		{
			this->check_valid_path(old_path, "rename");
			CHECKARGS(priv().rename_from.match(old_path),
				"rename: insufficient privileges");

			this->check_valid_path(new_path, "rename");
			CHECKARGS(priv().rename_to.match(new_path),
				"rename: insufficient privileges");

			bool ok = FileSystem::renameFile(old_path, new_path);
			CHECK(ok,
				"rename: could not rename " + old_path + " to " + new_path);

			ipcio_put(conn(), PrivilegeCommon::E_OK);
		}
		PRIVOP_FINISH;
	}

	void Monitor::unlink()
	{
		String path;
		ipcio_get(conn(), path, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ unlink, path=%1", path).toString());
		PRIVOP_BODY
		{
			this->check_valid_path(path, "unlink");
			CHECKARGS(priv().unlink.match(path),
				"unlink: insufficient privileges");

			bool ok = FileSystem::removeFile(path);

			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), ok);
		}
		PRIVOP_FINISH;
	}

	inline int output_handle(UnnamedPipeRef const & p)
	{
		return p.cast_to<PosixUnnamedPipe>()->getOutputHandle();
	}

	inline int input_handle(UnnamedPipeRef const & p)
	{
		return p.cast_to<PosixUnnamedPipe>()->getInputHandle();
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

	bool env_var_libpath_entry(String const & s)
	{
		return s.startsWith(OW_ENV_VAR_LIBPATH "=");
	}

	bool filter_env(StringArray & env)
	{
		if (reserved_env_var_present(env))
		{
			return false;
		}
		StringArray::iterator i =
			std::find_if(env.begin(), env.end(), env_var_libpath_entry);
		if (have_LIBPATH)
		{
			if (i == env.end())
			{
				env.push_back(entry_LIBPATH);
			}
			else
			{
				*i = entry_LIBPATH;
			}
		}
		else if (i != env.end())
		{
			env.erase(i);
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
		xargv = ipcio_get_strarr(conn(), MAX_ARGV_LENGTH, MAX_ARG_LENGTH);
		xenvp = ipcio_get_strarr(conn(), MAX_ENV_LENGTH, MAX_ENVITEM_LENGTH);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ monitoredSpawn, exec_path=%1, app_name=%2", exec_path, app_name));
		PRIVOP_BODY
		{
			this->check_valid_path(exec_path, "monitoredSpawn");
			CHECKARGS(app_name.length() <= MAX_APPNAME_LENGTH, "monitoredSpawn: app name too long");
			CHECKARGS(xargv.second,	"monitoredSpawn: argv too large");
			CHECKARGS(priv().monitored_exec.match(exec_path, app_name)
				|| priv().monitored_exec_check_args.match(exec_path, xargv.first, app_name),
				"monitoredSpawn: insufficient privileges");
			CHECKARGS(xenvp.second,	"monitoredSpawn: envp too large");
			bool reserved_env_var_absent = filter_env(xenvp.first);
			CHECKARGS(reserved_env_var_absent,
				"monitoredSpawn: reserved env var set in environment argument"
			);
			CHECK(m_secure_paths.is_secure(exec_path),
				"monitoredSpawn: exec path " + exec_path + " is insecure");

			AutoDescriptor desc(::dup(0)); // get some unused descriptor
			CHECK(desc.get() >= 0, "monitoredSpawn: dup failed");
			putenv_monitor_desc(xenvp.first, desc.get());
			ME_PreExec pre_exec(m_config_dir, app_name.c_str(), this, desc);
			this->spawn_and_return(
				exec_path, xargv.first, xenvp.first, pre_exec);
		}
		PRIVOP_FINISH;
	}
	
	void Monitor::monitoredUserSpawn()
	{
		String exec_path;
		String app_name;
		String user_name; 
		std::pair<StringArray, bool> xargv, xenvp;
		ipcio_get(conn(), exec_path, MAX_PATH_LENGTH + 1);
		ipcio_get(conn(), app_name, MAX_APPNAME_LENGTH + 1);
		xargv = ipcio_get_strarr(conn(), MAX_ARGV_LENGTH, MAX_ARG_LENGTH);
		xenvp = ipcio_get_strarr(conn(), MAX_ENV_LENGTH, MAX_ENVITEM_LENGTH);
		ipcio_get(conn(), user_name, MAX_USER_NAME_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ monitoredUserSpawn, exec_path=%1, app_name=%2, user=%3", 
								   exec_path, app_name, user_name));
		PRIVOP_BODY
		{
			this->check_valid_path(exec_path, "monitoredUserSpawn");
			CHECKARGS(user_name.length() <= MAX_USER_NAME_LENGTH,
				"monitoredUserSpawn: user name too long");
			CHECKARGS(app_name.length() <= MAX_APPNAME_LENGTH, "monitoredUserSpawn: app name too long");
			CHECKARGS(xargv.second,	"monitoredUserSpawn: argv too large");
			CHECKARGS(priv().monitored_user_exec.match(exec_path, app_name, user_name)
				|| priv().monitored_user_exec_check_args.match(exec_path, xargv.first, app_name, user_name),
				"monitoredUserSpawn: insufficient privileges");
			CHECKARGS(xenvp.second,	"monitoredUserSpawn: envp too large");
			bool reserved_env_var_absent = filter_env(xenvp.first);
			CHECKARGS(reserved_env_var_absent,
				"monitoredUserSpawn: reserved env var set in environment argument"
			);
			CHECK(m_secure_paths.is_secure(exec_path),
				"monitoredUserSpawn: exec path " + exec_path + " is insecure");

			AutoDescriptor desc(::dup(0)); // get some unused descriptor
			CHECK(desc.get() >= 0, "monitoredUserSpawn: dup failed");
			putenv_monitor_desc(xenvp.first, desc.get());
			ME_PreExec pre_exec(m_config_dir, app_name.c_str(), this, desc, user_name.c_str());
			this->spawn_and_return(
				exec_path, xargv.first, xenvp.first, pre_exec);
		}
		PRIVOP_FINISH;
	}

	void Monitor::kill()
	{
		ProcId pid;
		int sig;
		ipcio_get(conn(), pid);
		ipcio_get(conn(), sig);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ kill, pid=%1, sig=%2", pid, sig).toString());
		PRIVOP_BODY
		{
			CHECKARGS(has(m_proc_map, pid), "kill: unknown process");
			int rv = (::kill(pid, sig) == 0 ? 0 : errno);
			ipcio_put(conn(), PrivilegeCommon::E_OK);
			ipcio_put(conn(), rv);
		}
		PRIVOP_FINISH;
	}

	void Monitor::pollStatus()
	{
		ProcId pid;
		ipcio_get(conn(), pid);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ pollStatus, pid=%1", pid).toString());
		PRIVOP_BODY
		{
			CHECKARGS(has(m_proc_map, pid), "pollStatus: unknown process");
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
		}
		PRIVOP_FINISH;
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
		xargv = ipcio_get_strarr(conn(), MAX_ARGV_LENGTH, MAX_ARG_LENGTH);
		xenvp = ipcio_get_strarr(conn(), MAX_ENV_LENGTH, MAX_ENVITEM_LENGTH);
		ipcio_get(conn(), user_name, MAX_USER_NAME_LENGTH + 1);
		ipcio_get(conn(), working_dir, MAX_PATH_LENGTH + 1);
		conn().get_sync();

		OW_LOG_INFO(logger, Format("REQ userSpawn, exec_path=%1, user=%2", exec_path, user_name));
		PRIVOP_BODY
		{
			this->check_valid_path(exec_path, "userSpawn");
			CHECKARGS(user_name.length() <= MAX_USER_NAME_LENGTH,
				"userSpawn: user name too long");
			CHECKARGS(working_dir.length() <= MAX_PATH_LENGTH,
				"userSpawn: working dir too long");
			CHECKARGS(xargv.second, "userSpawn: argv too large");
			CHECKARGS(priv().user_exec.match(exec_path, user_name) 
				|| priv().user_exec_check_args.match(exec_path, xargv.first, user_name),
				"userSpawn: insufficient privileges");
			CHECKARGS(xenvp.second, "userSpawn: envp too large");
			bool reserved_env_var_absent = filter_env(xenvp.first);
			CHECKARGS(reserved_env_var_absent,
				"userSpawn: reserved env var set in environment argument");
			CHECK(m_secure_paths.is_secure(exec_path),
				"userSpawn: exec path " + exec_path + " is insecure");
			CHECK(working_dir.empty() || working_dir.startsWith("/"),
				"userSpawn: working dir must be an absolute path");

			UE_PreExec pre_exec(user_name, working_dir);
			this->spawn_and_return(
				exec_path, xargv.first, xenvp.first, pre_exec);
		}
		PRIVOP_FINISH;
	}

#if 0
	void Monitor::check_path()
	{
		*** FILL IN ***;
	}
#endif

	void readConfigFile(Privileges& privs, const String& path)
	{
		std::ifstream is(path.c_str());
		CHECK0(is, "Could not open privilege config file " + path);

		openwbem_privconfig_Lexer lexer(is);
		PrivilegeConfig::ParseError err;

		int code = openwbem_privconfig_parse(&privs, &err, &lexer);
		CHECK0(code == 0,
			"Parse error in privilege config file " + path + 
			" (line " + String(err.line) + ", column " + String(err.column) +
			"): " + err.message
		);
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
				readConfigFile(*p_priv, privFile);
			}
		}
		else
		{
			readConfigFile(*p_priv, path);
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
		turnOffWaitpidThreadFix();

		PrivilegeCommon::DescriptorInfo x
			= PrivilegeCommon::monitor_descriptor();
		if (x.errnum != 0)
		{
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
				UserId uid;
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
					OW_LOG_ERROR(logger, Format("Exception caught in setup: %1", e));
					ipcio_put(*p_conn, PrivilegeCommon::E_ERROR);
					ipcio_put(*p_conn, e.what());
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

// argv[1]: config_dir; must have already been verified as secure and must
//   be in canonical form as described for FileSystem::Path::realPath.
// argv[2]: app_name; name of config file for the application we are monitoring.
//
int main(int argc, char * * argv)
{
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
