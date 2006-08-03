/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, nor Quest Software, Inc., nor the
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
#include "OW_Array.hpp"
#include "OW_Assertion.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Format.hpp"
#include "OW_IPCIO.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_PrivilegeCommon.hpp"
#include "OW_PrivilegeConfig.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_Process.hpp"
#include "OW_Reference.hpp"
#include "OW_Secure.hpp"
#include "OW_String.hpp"
#include "OW_Environ.hpp"
#include "OW_Exec.hpp"

#include <cstdlib>
#include <cstring>
#include <pwd.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace OpenWBEM;
using PrivilegeConfig::Privileges;

#define CHECKPREFIX String("PrivilegeManager::")
#define CHECK(tst, msg) \
	CHECK_E((tst), PrivilegeManagerException, CHECKPREFIX + (msg))
#define CHECK_ERRNO(tst, msg) \
	CHECK_ERRNO_E((tst), PrivilegeManagerException, CHECKPREFIX + (msg))

namespace
{
	::uid_t const ROOT_UID = 0;

	void check_result(IPCIO & conn, IPCIO::EBuffering eb = IPCIO::E_BUFFERED)
	{
		PrivilegeCommon::EStatus status;
		ipcio_get(conn, status, IPCIO::E_THROW_ON_EOF, eb);
		if (status == PrivilegeCommon::E_ERROR)
		{
			String errmsg;
			int errcode;
			ipcio_get(conn, errcode);
			ipcio_get(conn, errmsg);
			conn.get_sync();
			OW_THROW_ERR(PrivilegeManagerException, errmsg.c_str(), errcode);
		}
	}

	std::size_t cstr_arr_len(char const * const * arr)
	{
		char const * const * p;
		for (p = arr; *p; ++p)
		{
		}
		return p - arr;
	}

	void ipcio_put_strarr(IPCIO & conn, char const * const * arr)
	{
		std::size_t arrlen = cstr_arr_len(arr);
		ipcio_put(conn, arrlen);
		for (std::size_t i = 0; i < arrlen; ++i)
		{
			ipcio_put(conn, arr[i]);
		}
	}

	Timeout monitor_timeout(Timeout::relative(300.0)); // five minutes
}

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION(PrivilegeManager);

bool PrivilegeManager::use_lib_path = false;

struct PrivilegeManagerImpl : public IntrusiveCountableBase
{
	PrivilegeManagerImpl(AutoDescriptor peer_descriptor, ProcessRef const & pproc)
	: m_conn(peer_descriptor, monitor_timeout),
	  m_pproc(pproc)
	{
	}

	PrivilegeManagerImpl(AutoDescriptor peer_descriptor);

	void setLogger(LoggerSpec const * plogspec);

	virtual ~PrivilegeManagerImpl();

	IPCIO m_conn;
	ProcessRef m_pproc;
	NonRecursiveMutex m_mutex;
};

PrivilegeManagerImpl::PrivilegeManagerImpl(AutoDescriptor peer_descriptor)
: m_conn(peer_descriptor, monitor_timeout)
{
	::pid_t monitor_pid;
	ipcio_get(m_conn, monitor_pid);
	m_conn.get_sync();

	m_pproc = new Process(monitor_pid);
}

PrivilegeManagerImpl::~PrivilegeManagerImpl()
{
	try
	{
		if (m_pproc->processStatus().running())
		{
			ipcio_put(m_conn, PrivilegeCommon::E_CMD_DONE);
			m_conn.put_sync();
			m_pproc->waitCloseTerm(Timeout::relative(0.0), Timeout::relative(9.0), Timeout::relative(10.0));
		}
	}
	catch (...)
	{
	}
}

namespace
{
	IntrusiveReference<PrivilegeManagerImpl> g_pmImpl;
	NonRecursiveMutex g_pmImplGuard;
}

PrivilegeManager::PrivilegeManager(PrivilegeManagerImpl * p_impl)
: RefCountedPimpl<PrivilegeManagerImpl>(p_impl)
{
	OW_ASSERT(g_pmImpl.getPtr() == p_impl);
}

PrivilegeManager::~PrivilegeManager()
{
	// All the action happens in the PrivilegeManagerImpl dtor, and
	// HAS to happen there, so that we can copy a PrivilegeManager object
	// -- as a resource handle -- without shutting down the monitor.
}

// static
PrivilegeManager PrivilegeManager::connectToMonitor()
{
	PrivilegeCommon::DescriptorInfo x(PrivilegeCommon::monitor_descriptor());
	switch (x.errnum)
	{
		case 0:
			{
				NonRecursiveMutexLock lock(g_pmImplGuard);
				if (!g_pmImpl)
				{
					g_pmImpl = new PrivilegeManagerImpl(AutoDescriptor(::dup(x.descriptor)));
				}
				return PrivilegeManager(g_pmImpl.getPtr());
			}
		case PrivilegeCommon::EXIT_NO_DESCRIPTOR_STRING:
			return PrivilegeManager(); // no monitor exists, so they get a NULL PrivilegeManager
		case PrivilegeCommon::EXIT_BAD_DESCRIPTOR_STRING:
			OW_THROW(
				PrivilegeManagerException,
				"PrivilegeManager::get_privilege_manager: environment specifies bad monitor descriptor"
			);
		default:
			OW_ASSERT(false);
	}
	// Not reached, but we need a return value.
	return PrivilegeManager();
}

PrivilegeManager PrivilegeManager::getPrivilegeManager()
{
	NonRecursiveMutexLock lock(g_pmImplGuard);
	if (g_pmImpl)
	{
		return PrivilegeManager(g_pmImpl.getPtr());
	}
	return PrivilegeManager();
}

namespace
{
	
	StringArray monitor_argv(
		char const * exec_path,	char const * config_dir, char const * app_name,
		char const * user_name
	)
	{
		StringArray retval;
		retval.push_back(exec_path);
		retval.push_back(config_dir);
		retval.push_back(app_name);
		if (user_name && *user_name)
		{
			retval.push_back(user_name);
		}
		return retval;
	}

	void copy_env_var(char const * varname, StringArray & env)
	{
		char const * lib_path= ::getenv(varname);
		if (lib_path)
		{
			Format fmt("%1=%2", varname, lib_path);
			env.push_back(fmt.toString());
		}
	}

	// A minimal environment
	//
	StringArray monitor_envp(int child_desc)
	{
		StringArray retval = Secure::minimalEnvironment();
		Format fmt("%1=%2", PrivilegeCommon::CONN_DESC_ENV_VAR, child_desc);
		retval.push_back(fmt.toString());
		if (PrivilegeManager::use_lib_path)
		{
			copy_env_var(OW_ENV_VAR_LIBPATH, retval);
		}
		return retval;
	}

	class PrivMonPreExec : public Exec::PreExec
	{
	public:
		// REQUIRE: child_desc >= 3
		PrivMonPreExec(int child_desc);
		virtual bool keepStd(int d) const;
		virtual void call(pipe_pointer_t const pparr[]);

	private:
		std::vector<bool> m_keep;
	};

	PrivMonPreExec::PrivMonPreExec(int child_desc)
	: PreExec(true),
	  m_keep(child_desc + 1, false)
	{
		m_keep[child_desc] = true;
	}

	bool PrivMonPreExec::keepStd(int) const
	{
		return false;
	}

	void PrivMonPreExec::call(pipe_pointer_t const pparr[])
	{
		PreExec::resetSignals();
		PreExec::setupStandardDescriptors(pparr);
		PreExec::closeDescriptorsOnExec(m_keep);
	}

	struct SMPolicy : public PrivilegeCommon::SpawnMonitorPolicy
	{
		virtual char const * check_config_dir(char const * config_dir);
		virtual void spawn(
			int child_desc, int parent_desc,
			char const * config_dir, char const * app_name
		);
		char const * m_user_name;
		String m_config_dir;
		std::auto_ptr<PrivilegeManagerImpl> m_pmgr;
	};

#define CTOR "PrivilegeManager"	

	char const * SMPolicy::check_config_dir(char const * config_dir)
	{
		CHECK(config_dir, CTOR ": config_dir must be non-null");
		CHECK(config_dir[0] == '/', CTOR ": config_dir must be absolute path");
		// CHECK(config_dir[std::strlen(config_dir) - 1] == '/',
			// CTOR ": config_dir must end in '/'");

		using namespace FileSystem::Path;
		std::pair<ESecurity, String> x = security(config_dir, ROOT_UID);
		CHECK(x.first != E_INSECURE, CTOR ": config_dir is insecure");
		CHECK(x.first == E_SECURE_DIR, CTOR ": config_dir is not a directory");
		m_config_dir = x.second;  // real path
		return m_config_dir.c_str();
	}

	void SMPolicy::spawn(
		int child_desc, int parent_desc,
		char const * config_dir, char const * app_name
	)
	{
		PrivMonPreExec pre_exec(child_desc);
		String exec_path = 
			ConfigOpts::installed_owlibexec_dir + "/owprivilegemonitor" + String(OW_OPENWBEM_LIBRARY_VERSION);
		using namespace FileSystem::Path;
		std::pair<ESecurity, String> x = security(exec_path, ROOT_UID);
		CHECK(x.first != E_INSECURE, CTOR ": monitor executable is insecure");
		StringArray argv = 
			monitor_argv(exec_path.c_str(), config_dir, app_name, m_user_name);
		StringArray envp = monitor_envp(child_desc);
		ProcessRef p_monitor = Exec::spawn(exec_path, argv, envp, pre_exec);

		// Caller owns parent_desc, so object makes own copy.
		m_pmgr.reset(new PrivilegeManagerImpl(AutoDescriptor(::dup(parent_desc)), p_monitor));
	}

}

class MonitorChild : public Process
{
public:
	MonitorChild(
		UnnamedPipeRef const & in, UnnamedPipeRef const & out,
		UnnamedPipeRef const & err, ProcId pid,
		PrivilegeManager const & priv_mgr
	)
	: Process(in, out, err, pid),
	  m_priv_mgr(priv_mgr)
	{
	}

private:
	virtual int kill(ProcId pid, int sig);
	virtual Status pollStatus(ProcId pid);
	PrivilegeManager m_priv_mgr;
};

int MonitorChild::kill(ProcId pid, int sig)
{
	return m_priv_mgr.kill(pid, sig);
}

Process::Status MonitorChild::pollStatus(ProcId pid)
{
	return m_priv_mgr.pollStatus(pid);
}

IntrusiveCountableBase *
PrivilegeManager::init(
	char const * config_dir, char const * app_name, char const * user_name,
	LoggerSpec const * plogspec)
{
	if (!config_dir || !app_name || !*config_dir || !*app_name ||
		!FileSystem::exists(String(config_dir) + "/" + app_name)
	)
	{
		CHECK(user_name && *user_name,
			"PrivilegeManager: no privilege config file and no user name");
		// only root can setuid() to another user
		if (::getuid() == ROOT_UID)
		{
			Secure::runAs(user_name);
		}
		return 0; // no privileges
	}
	SMPolicy policy;
	policy.m_user_name = user_name;
	try
	{
		PrivilegeCommon::spawn_monitor(config_dir, app_name, policy);
	}
	catch (PrivilegeCommon::SpawnMonitorException & e)
	{
		Format format("PrivilegeManager::init() failed to spawn monitor: %1", e.what());
		OW_THROW_SUBEX(PrivilegeManagerException, format.c_str(), e);
	}

	if (policy.m_pmgr.get())
	{
		::pid_t monitor_pid;
		IPCIO & conn = policy.m_pmgr->m_conn;
		ipcio_get(conn, monitor_pid);
		CHECK(monitor_pid == policy.m_pmgr->m_pproc->pid(),
			"inconsistent pid received from monitor");
		conn.get_sync();

		policy.m_pmgr->setLogger(plogspec);
	}

	return policy.m_pmgr.release();
}

void PrivilegeManagerImpl::setLogger(LoggerSpec const * pspec)
{
	ipcio_put(m_conn, pspec != 0);
	if (pspec)
	{
		size_t n = pspec->categories.size();
		ipcio_put(m_conn, n);
		for (size_t i = 0; i < n; ++i)
		{
			ipcio_put(m_conn, pspec->categories[i]);
		}
		ipcio_put(m_conn, pspec->message_format);
		ipcio_put(m_conn, pspec->file_name);
		ipcio_put(m_conn, pspec->max_file_size);
		ipcio_put(m_conn, pspec->max_backup_index);
		m_conn.put_sync();

		check_result(m_conn);
		m_conn.get_sync();
	}
}

AutoDescriptor PrivilegeManager::open(
	char const * pathname, OpenFlags flags, OpenPerms perms
)
{
	CHECK(pimpl(), "open: process has no privileges");
	NonRecursiveMutexLock lock(pimpl()->m_mutex);

	IPCIO & conn = pimpl()->m_conn;
	ipcio_put(conn, PrivilegeCommon::E_CMD_OPEN);
	ipcio_put(conn, pathname);
	ipcio_put(conn, flags);
	ipcio_put(conn, perms);
	conn.put_sync();

	check_result(conn, IPCIO::E_UNBUFFERED);
	AutoDescriptor retval = conn.get_handle();
	conn.get_sync();
	if (flags & ate)
	{
		CHECK(FileSystem::seek(retval.get(), 0, SEEK_END) >= 0,
			"Seek to end failed");
	}
	return retval;
}

StringArray PrivilegeManager::readDirectory(
	char const * pathname, ReadDirOptions opt
)
{
	CHECK(pimpl(), "readDirectory: process has no privileges");
	NonRecursiveMutexLock lock(pimpl()->m_mutex);

	IPCIO & conn = pimpl()->m_conn;
	ipcio_put(conn, PrivilegeCommon::E_CMD_READ_DIR);
	ipcio_put(conn, pathname);
	ipcio_put(conn, opt);
	conn.put_sync();

	check_result(conn);
	std::size_t arrlen;
	ipcio_get(conn, arrlen);
	StringArray retval;
	for (std::size_t i = 0; i < arrlen; ++i)
	{
		String s;
		ipcio_get(conn, s, std::size_t(-1));
		retval.push_back(s);
	}
	conn.get_sync();

	return retval;
}

String PrivilegeManager::readLink(char const * pathname)
{
	CHECK(pimpl(), "readLink: process has no privileges");
	NonRecursiveMutexLock lock(pimpl()->m_mutex);

	IPCIO & conn = pimpl()->m_conn;
	ipcio_put(conn, PrivilegeCommon::E_CMD_READ_LINK);
	ipcio_put(conn, pathname);
	conn.put_sync();

	check_result(conn);
	String s;
	ipcio_get(conn, s, std::size_t(-1));
	conn.get_sync();

	return s;
}

#if 0
bool PrivilegeManager::checkPath(char const * path, char const * user)
{
	IPCIO & conn = pimpl()->m_conn;
	ipcio_put(conn, PrivilegeCommon::E_CMD_CHECK_PATH);
	ipcio_put(conn, path);
	ipcio_put(conn, user);
	conn.put_sync();

	check_result(conn);
	bool retval;
	ipcio_get(conn, retval);
	conn.get_sync();

	return retval;
}
#endif

void PrivilegeManager::rename(char const * oldpath, char const * newpath)
{
	CHECK(pimpl(), "rename: process has no privileges");
	NonRecursiveMutexLock lock(pimpl()->m_mutex);

	IPCIO & conn = pimpl()->m_conn;
	ipcio_put(conn, PrivilegeCommon::E_CMD_RENAME);
	ipcio_put(conn, oldpath);
	ipcio_put(conn, newpath);
	conn.put_sync();

	check_result(conn);
	conn.get_sync();
}

bool PrivilegeManager::unlink(char const * path)
{
	CHECK(pimpl(), "unlink: process has no privileges");
	NonRecursiveMutexLock lock(pimpl()->m_mutex);

	IPCIO & conn = pimpl()->m_conn;
	ipcio_put(conn, PrivilegeCommon::E_CMD_UNLINK);
	ipcio_put(conn, path);
	conn.put_sync();

	check_result(conn);
	bool retval;
	ipcio_get(conn, retval);
	conn.get_sync();

	return retval;
}

ProcessRef PrivilegeManager::monitoredSpawn(
	char const * exec_path,
	char const * app_name,
	char const * const argv[], char const * const envp[]
)
{
	CHECK(pimpl(), "monitoredSpawn: process has no privileges");
	NonRecursiveMutexLock lock(pimpl()->m_mutex);
	IPCIO & conn = pimpl()->m_conn;

	char const * default_argv[2] = { exec_path, 0 };
	if (!argv || !*argv)
	{
		argv = default_argv;
	}
	if (!envp)
	{
		envp = environ;
	}

	ipcio_put(conn, PrivilegeCommon::E_CMD_MONITORED_SPAWN);
	ipcio_put(conn, exec_path);
	ipcio_put(conn, app_name);
	ipcio_put_strarr(conn, argv);
	ipcio_put_strarr(conn, envp);
	conn.put_sync();

	check_result(conn, IPCIO::E_UNBUFFERED);
	UnnamedPipeRef in(new PosixUnnamedPipe(AutoDescriptor(), conn.get_handle()));
	UnnamedPipeRef out(new PosixUnnamedPipe(conn.get_handle(), AutoDescriptor()));
	UnnamedPipeRef err(new PosixUnnamedPipe(conn.get_handle(), AutoDescriptor()));
	ProcId pid;
	ipcio_get(conn, pid);
	conn.get_sync();
	return ProcessRef(new MonitorChild(in, out, err, pid, *this));
}

int PrivilegeManager::kill(ProcId pid, int sig)
{
	NonRecursiveMutexLock lock(pimpl()->m_mutex);
	IPCIO & conn = pimpl()->m_conn;

	ipcio_put(conn, PrivilegeCommon::E_CMD_KILL);
	ipcio_put(conn, pid);
	ipcio_put(conn, sig);
	conn.put_sync();

	check_result(conn);
	int retval;
	ipcio_get(conn, retval);
	conn.get_sync();
	return retval;
}

Process::Status PrivilegeManager::pollStatus(ProcId pid)
{
	NonRecursiveMutexLock lock(pimpl()->m_mutex);
	IPCIO & conn = pimpl()->m_conn;

	ipcio_put(conn, PrivilegeCommon::E_CMD_POLL_STATUS);
	ipcio_put(conn, pid);
	conn.put_sync();

	check_result(conn);
	ProcId wpid;
	int wstatus;
	ipcio_get(conn, wpid);
	ipcio_get(conn, wstatus);
	conn.get_sync();
	return Process::Status(wpid, wstatus);
}

ProcessRef PrivilegeManager::userSpawn(
	char const * exec_path,
	char const * const argv[], char const * const envp[],
	char const * user, char const * working_dir
)
{
	CHECK(pimpl(), "userSpawn: process has no privileges");

	char const * default_argv[2] = { exec_path, 0 };
	if (!argv || !argv[0])
	{
		argv = default_argv;
	}
	if (!envp)
	{
		envp = environ;
	}
	if (!user)
	{
		user = "";
	}
	if (!working_dir)
	{
		working_dir = "";
	}

	NonRecursiveMutexLock lock(pimpl()->m_mutex);
	IPCIO & conn = pimpl()->m_conn;

	ipcio_put(conn, PrivilegeCommon::E_CMD_USER_SPAWN);
	ipcio_put(conn, exec_path);
	ipcio_put_strarr(conn, argv);
	ipcio_put_strarr(conn, envp);
	ipcio_put(conn, user);
	ipcio_put(conn, working_dir);
	conn.put_sync();

	check_result(conn, IPCIO::E_UNBUFFERED);
	UnnamedPipeRef in(new PosixUnnamedPipe(AutoDescriptor(), conn.get_handle()));
	UnnamedPipeRef out(new PosixUnnamedPipe(conn.get_handle(), AutoDescriptor()));
	UnnamedPipeRef err(new PosixUnnamedPipe(conn.get_handle(), AutoDescriptor()));
	ProcId pid;
	ipcio_get(conn, pid);
	conn.get_sync();
	return ProcessRef(new MonitorChild(in, out, err, pid, *this));
}

bool
PrivilegeManager::isNull() const
{
	return pimpl() == 0;
}

// static 
PrivilegeManager PrivilegeManager::setInstance(IntrusiveCountableBase * p_impl)
{
	NonRecursiveMutexLock lock(g_pmImplGuard);
	OW_ASSERT(!g_pmImpl);
	g_pmImpl = static_cast<PrivilegeManagerImpl*>(p_impl);
	return PrivilegeManager(g_pmImpl.getPtr());
}

} // namespace OW_NAMESPACE