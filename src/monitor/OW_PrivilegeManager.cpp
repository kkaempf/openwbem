/*******************************************************************************
* Copyright © 2002  Networks Associates Technology, Inc.  All rights reserved.
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
#include "OW_PrivilegeManagerMockObject.hpp"
#include "OW_Process.hpp"
#include "OW_Reference.hpp"
#include "OW_Secure.hpp"
#include "OW_String.hpp"
#include "OW_Environ.hpp"
#include "OW_Exec.hpp"
#include "OW_File.hpp"
#include "OW_Logger.hpp"
#include "blocxx/WaitpidThreadFix.hpp"

#ifndef BLOCXX_WIN32
#include "blocxx/PosixFileSystem.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <cstdlib>
#include <cstring>
#include <memory> // for auto_ptr

#ifndef OW_WIN32
#include <pwd.h>
#include <sys/socket.h>
#include <unistd.h>
#else
#include <io.h> // for dup()
#endif

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

	template <typename extype>
	void check_result(IPCIO& conn, IPCIO::EBuffering eb = IPCIO::E_BUFFERED)
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

			if( errcode < PrivilegeManager::MONITOR_ERROR_START)
			{
				// Anything with an error code should be rethrown as the desired
				// exception type.
				OW_THROW_ERR(extype, errmsg.c_str(), errcode);
			}
			else if( errcode < PrivilegeManager::MONITOR_FATAL_ERROR_START )
			{
				// Non-fatal errors and errno errors can just be forwarded on as a
				// generic PrivilegeManagerException.
				OW_THROW_ERR(PrivilegeManagerException, errmsg.c_str(), errcode);
			}

			// This switch is using nasty macros that would be nice to avoid but
			// we need to do different actions for debug/non debug compilations.
			switch(errcode)
			{
				case PrivilegeManager::E_INSUFFICIENT_PRIVILEGES:
				{
#if defined(OW_DEBUG)
					if( !::getenv("OW_PRIVMAN_NO_ABORT") )
					{
						Logger logger("PrivilegeManager");
						OW_LOG_FATAL_ERROR(logger, Format("check_result got an INSUFFICIENT_PRIVILEGES error: %1:%2, aborting", errcode, errmsg));
						abort();
					}
#endif
					OW_THROW_ERR(InsufficientPrivilegesException, errmsg.c_str(), errcode);
				}
				default:
				{
#if defined(OW_DEBUG)
					if( !::getenv("OW_PRIVMAN_NO_ABORT") )
					{
						Logger logger("PrivilegeManager");
						OW_LOG_FATAL_ERROR(logger, Format("check_result got an error: %1:%2, aborting", errcode, errmsg));
						abort();
					}
#endif
					OW_THROW_ERR(FatalPrivilegeManagerException, errmsg.c_str(), errcode);
				}
			}
		}
	}

	void check_result(IPCIO& conn, IPCIO::EBuffering eb = IPCIO::E_BUFFERED)
	{
		check_result<PrivilegeManagerException>(conn, eb);
	}

	std::size_t cstr_arr_len(char const * const * arr)
	{
		char const * const * p;
		for (p = arr; *p; ++p)
		{
		}
		return p - arr;
	}

	void ipcio_put_strarr(IPCIO& conn, char const * const * arr)
	{
		std::size_t arrlen = cstr_arr_len(arr);
		ipcio_put(conn, arrlen);
		for (std::size_t i = 0; i < arrlen; ++i)
		{
			ipcio_put(conn, arr[i]);
		}
	}

	// five minutes
#define MONITOR_TIMEOUT Timeout::relative(300.0)

}

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION(PrivilegeManager);
OW_DEFINE_EXCEPTION2(FatalPrivilegeManager, PrivilegeManagerException);
OW_DEFINE_EXCEPTION2(MonitorCommunication, FatalPrivilegeManagerException)
OW_DEFINE_EXCEPTION2(InsufficientPrivileges, FatalPrivilegeManagerException);

PrivilegeManagerMockObject_t g_privilegeManagerMockObject = OW_GLOBAL_PTR_INIT;

bool PrivilegeManager::use_lib_path = false;

struct PrivilegeManagerImpl : public IntrusiveCountableBase
{
	PrivilegeManagerImpl(AutoDescriptor peer_descriptor, ProcessRef const & pproc)
		: m_conn(new IPCIO(peer_descriptor, MONITOR_TIMEOUT))
		, m_connectionValid(true)
		, m_pproc(pproc)
	{
	}

	PrivilegeManagerImpl(AutoDescriptor peer_descriptor);

	PrivilegeManagerImpl();

	void setLogger(LoggerSpec const * plogspec);

	virtual ~PrivilegeManagerImpl();

	Reference<IPCIO> m_conn;
	bool m_connectionValid;
	ProcessRef m_pproc;
	NonRecursiveMutex m_mutex;

	void verifyValidConnection(const char* text); // throws PrivilegeManagerException
	void invalidateConnection();
};

void PrivilegeManagerImpl::verifyValidConnection(const char* text)
{
	NonRecursiveMutexLock lock(m_mutex);
	CHECK(m_connectionValid, text);
}

void PrivilegeManagerImpl::invalidateConnection()
{
	NonRecursiveMutexLock lock(m_mutex);
	m_connectionValid = false;
}

PrivilegeManagerImpl::PrivilegeManagerImpl()
	: m_connectionValid(false)
{
}

PrivilegeManagerImpl::PrivilegeManagerImpl(AutoDescriptor peer_descriptor)
	: m_conn(new IPCIO(peer_descriptor, MONITOR_TIMEOUT)),
	m_connectionValid(true)
{
	try
	{
		::pid_t monitor_pid;
		ipcio_get(*m_conn, monitor_pid);
		m_conn->get_sync();

		m_pproc = new Process(monitor_pid);
	}
	catch(const IPCIOException& e)
	{
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while initializing privilege manager from peer", e);
	}
}

namespace
{
	class WaitpidThreadFixSettingRestorer
	{
	public:
		WaitpidThreadFixSettingRestorer(bool setting) 
		: m_setting(setting)
		, m_restored(false) 
		{
		}

		~WaitpidThreadFixSettingRestorer() 
		{ 
			if (!m_restored)
			{
				WaitpidThreadFix::setWaitpidThreadFixEnabled(m_setting); 
			}
		}

		void restore()
		{
			WaitpidThreadFix::setWaitpidThreadFixEnabled(m_setting); 
			m_restored = true;
		}
	private:
		bool m_setting;
		bool m_restored;
	};
}

PrivilegeManagerImpl::~PrivilegeManagerImpl()
{
	bool prevSetting = WaitpidThreadFix::setWaitpidThreadFixEnabled(false);
	WaitpidThreadFixSettingRestorer restorer(prevSetting);
	try
	{
		if (m_pproc && m_pproc->processStatus().running())
		{
			ipcio_put(*m_conn, PrivilegeCommon::E_CMD_DONE);
			m_conn->put_sync();
			m_pproc->waitCloseTerm(Timeout::relative(0.0), Timeout::relative(9.0), Timeout::relative(10.0));
		}
	}
	catch (...)
	{
	}
	m_pproc = 0; // do this explicitly while the waitpid thread fix is disabled
}

namespace
{
	IntrusiveReference<PrivilegeManagerImpl> g_pmImpl;
	NonRecursiveMutex g_pmImplGuard;
}

PrivilegeManager::PrivilegeManager(PrivilegeManagerImpl * p_impl)
: m_impl(p_impl)
{
	if( !g_privilegeManagerMockObject )
	{
		OW_ASSERT(g_pmImpl.getPtr() == p_impl);
	}
}

PrivilegeManager::PrivilegeManager()
{
}

PrivilegeManager::PrivilegeManager(const PrivilegeManager& x)
: m_impl(x.m_impl)
{
}

PrivilegeManager& 
PrivilegeManager::operator=(const PrivilegeManager& x)
{
	m_impl = x.m_impl;
	return *this;
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
#ifndef OW_WIN32
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
#endif
	// Not reached, but we need a return value.
	return PrivilegeManager();
}

PrivilegeManager PrivilegeManager::getPrivilegeManager()
{
	if( g_privilegeManagerMockObject )
	{
		return PrivilegeManager(new PrivilegeManagerImpl());
	}

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
	  : PreExec(true)
	  , m_keep(child_desc + 1, false)
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
			Descriptor child_desc, Descriptor parent_desc,
			char const * config_dir, char const * app_name
		);
		char const * m_user_name;
		String m_config_dir;
		std::auto_ptr<PrivilegeManagerImpl> m_pmgr;
	};

#define CTOR "PrivilegeManager"	

	char const * SMPolicy::check_config_dir(char const * config_dir)
	{
#ifdef OW_WIN32
#pragma message(Reminder "TODO: implement it for Win in BloCxx!")
		return "";
#else
		CHECK(config_dir, Format("%1: config_dir must be non-null", CTOR));
		CHECK(config_dir[0] == '/', Format("%1: config_dir: %2 must be an absolute path", CTOR, config_dir));

		using namespace FileSystem::Path;
		std::pair<ESecurity, String> x = security(config_dir, ROOT_UID);
		CHECK(x.first != E_INSECURE, Format("%1: config_dir: %2 is insecure", CTOR, config_dir));
		CHECK(x.first == E_SECURE_DIR, Format("%1: config_dir: %2 is not a directory", CTOR, config_dir));
		m_config_dir = x.second;  // real path
		return m_config_dir.c_str();
#endif
	}

	void SMPolicy::spawn(
		Descriptor child_desc, Descriptor parent_desc,
		char const * config_dir, char const * app_name
	)
	{
#ifdef OW_WIN32
#pragma message(Reminder "TODO: implement it for Win in BloCxx!")
#else
		PrivMonPreExec pre_exec(child_desc); 
		String exec_path = 
			ConfigOpts::installed_owlibexec_dir + "/owprivilegemonitor" + String(OW_OPENWBEM_LIBRARY_VERSION);
		using namespace FileSystem::Path;
		std::pair<ESecurity, String> x = security(exec_path, ROOT_UID);
		CHECK(x.first != E_INSECURE, CTOR ": monitor executable is insecure");
		StringArray argv = 
			monitor_argv(exec_path.c_str(), config_dir, app_name, m_user_name);
		StringArray envp = monitor_envp(child_desc);
		bool prevSetting = WaitpidThreadFix::setWaitpidThreadFixEnabled(false);
		WaitpidThreadFixSettingRestorer restorer(prevSetting);
		ProcessRef p_monitor = Exec::spawn(exec_path, argv, envp, pre_exec);
		restorer.restore();

		// Caller owns parent_desc, so object makes own copy.
		m_pmgr.reset(new PrivilegeManagerImpl(AutoDescriptor(::dup(parent_desc)), p_monitor));
#endif
	}

}

class MonitorChildImpl : public ProcessImpl
{
public:
	MonitorChildImpl(PrivilegeManager const & priv_mgr)
		: m_priv_mgr(priv_mgr)
	{
	}
	virtual int kill(ProcId pid, int sig)
	{
		return m_priv_mgr.kill(pid, sig);
	}
	virtual Process::Status pollStatus(ProcId pid)
	{
		return m_priv_mgr.pollStatus(pid);
	}
private:
	PrivilegeManager m_priv_mgr;

};

class MonitorChild : public Process
{
public:
	MonitorChild(
		UnnamedPipeRef const & in, UnnamedPipeRef const & out,
		UnnamedPipeRef const & err, ProcId pid,
		PrivilegeManager const & priv_mgr
	)
		: Process(ProcessImplRef(new MonitorChildImpl(priv_mgr)), in, out, err, pid)
	{
	}

};

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
#ifdef OW_WIN32
#pragma message(Reminder "TODO: implement it for Win!")
#else
		if (::getuid() == ROOT_UID)
		{
			Secure::runAs(user_name);
		}
#endif
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
		IPCIO& conn = *policy.m_pmgr->m_conn;
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
	try
	{
		verifyValidConnection("setLogger: no connection to the monitor");

		ipcio_put(*m_conn, pspec != 0);
		if (pspec)
		{
			size_t n = pspec->categories.size();
			ipcio_put(*m_conn, n);
			for (size_t i = 0; i < n; ++i)
			{
				ipcio_put(*m_conn, pspec->categories[i]);
			}
			ipcio_put(*m_conn, pspec->message_format);
			ipcio_put(*m_conn, pspec->file_name);
			ipcio_put(*m_conn, pspec->max_file_size);
			ipcio_put(*m_conn, pspec->max_backup_index);
			m_conn->put_sync();

			check_result(*m_conn);
			m_conn->get_sync();
		}
	}
	catch(const IPCIOException& e)
	{
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while obtaining logger information", e);
	}
}

AutoDescriptor PrivilegeManager::open(
	char const * pathname, OpenFlags flags, OpenPerms perms
)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->open(pathname, flags, perms);
	}

	Logger logger("PrivilegeManager.open");

	OW_LOG_DEBUG3(logger, Format("Attempting to open: %1", pathname));
	CHECK(pimpl(), "open: process has no privileges");
	pimpl()->verifyValidConnection("open: no connection to the monitor");
	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);

		IPCIO& conn = *pimpl()->m_conn;
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
#ifdef OW_WIN32
#pragma message(Reminder "TODO: for Win retval.get() should return FileHandle so after correct porting 'reinterpret_cast' need to be removed !")
			CHECK(FileSystem::seek(reinterpret_cast<FileHandle>(retval.get()), 0, SEEK_END) >= 0,
				"Seek to end failed");
#else
			CHECK(FileSystem::seek(retval.get(), 0, SEEK_END) >= 0,
				"Seek to end failed");
#endif
		}
		return retval;
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while opening file", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

namespace // anonymous
{
	FileSystem::FileInformation statImpl(PrivilegeCommon::ECommand statcmd
		, const String& statname
		, PrivilegeManagerImpl* pimpl
		, const char* pathname)
	{
		Logger logger("PrivilegeManager." + statname);

		OW_LOG_DEBUG3(logger, Format("Attempting to %1: %2", statname, pathname));
		CHECK(pimpl, Format("%1: process has no privileges", statname));
		pimpl->verifyValidConnection(Format("%1: no connection to the monitor", statname).c_str());
		try
		{
			NonRecursiveMutexLock lock(pimpl->m_mutex);

			IPCIO& conn = *pimpl->m_conn;
			ipcio_put(conn, statcmd);
			ipcio_put(conn, pathname);
			conn.put_sync();

			check_result<FileSystemException>(conn, IPCIO::E_UNBUFFERED);

#ifdef BLOCXX_WIN32
			OW_THROW(PrivilegeManagerException, Format("Not implemented: %1()", statname));
#else
			struct stat statbuf;
			ipcio_get(conn, statbuf);
			conn.get_sync();

			return FileSystem::statToFileInfo(statbuf);
#endif
		}
		catch(const IPCIOException& e)
		{
			pimpl->invalidateConnection();
			OW_THROW_SUBEX(MonitorCommunicationException, Format("Communication error for %1 on file", statname).c_str(), e);
		}
		catch(const FatalPrivilegeManagerException& e)
		{
			pimpl->invalidateConnection();
			throw;
		}
	}
} // end anonymous namespace

FileSystem::FileInformation PrivilegeManager::stat(const char* pathname)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->stat(pathname);
	}

	return statImpl(PrivilegeCommon::E_CMD_STAT, "stat", pimpl(), pathname);
}

FileSystem::FileInformation PrivilegeManager::lstat(const char* pathname)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->lstat(pathname);
	}

	return statImpl(PrivilegeCommon::E_CMD_LSTAT, "lstat", pimpl(), pathname);
}

StringArray PrivilegeManager::readDirectory(
	char const * pathname, ReadDirOptions opt
)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->readDirectory(pathname, opt);
	}

	CHECK(pimpl(), "readDirectory: process has no privileges");
	pimpl()->verifyValidConnection("readDirectory: no connection to the monitor");
	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);

		IPCIO& conn = *pimpl()->m_conn;
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
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while reading directory", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

String PrivilegeManager::readLink(char const * pathname)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->readLink(pathname);
	}

	CHECK(pimpl(), "readLink: process has no privileges");
	pimpl()->verifyValidConnection("readLink: no connection to the monitor");
	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);

		IPCIO& conn = *pimpl()->m_conn;
		ipcio_put(conn, PrivilegeCommon::E_CMD_READ_LINK);
		ipcio_put(conn, pathname);
		conn.put_sync();

		check_result(conn);
		String s;
		ipcio_get(conn, s, std::size_t(-1));
		conn.get_sync();

		return s;
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while reading link", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

#if 0
bool PrivilegeManager::checkPath(char const * path, char const * user)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->check(path, user);
	}

	CHECK(pimpl(), "checkPath: process has no privileges");
	pimpl()->verifyValidConnection("checkPath: no connection to the monitor");
	try
	{
		IPCIO& conn = *pimpl()->m_conn;
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
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while checking path", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}
#endif

void PrivilegeManager::rename(char const * oldpath, char const * newpath)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->rename(oldpath, newpath);
	}

	CHECK(pimpl(), "rename: process has no privileges");
	pimpl()->verifyValidConnection("rename: no connection to the monitor");
	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);

		IPCIO& conn = *pimpl()->m_conn;
		ipcio_put(conn, PrivilegeCommon::E_CMD_RENAME);
		ipcio_put(conn, oldpath);
		ipcio_put(conn, newpath);
		conn.put_sync();

		check_result(conn);
		conn.get_sync();
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while renaming file", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

bool PrivilegeManager::removeFile(char const * path)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->removeFile(path);
	}

	try
	{
		CHECK(pimpl(), "removeFile: process has no privileges");
		pimpl()->verifyValidConnection("removeFile: no connection to the monitor");
		NonRecursiveMutexLock lock(pimpl()->m_mutex);

		IPCIO& conn = *pimpl()->m_conn;
		ipcio_put(conn, PrivilegeCommon::E_CMD_REMOVE_FILE);
		ipcio_put(conn, path);
		conn.put_sync();

		check_result(conn);
		bool retval;
		ipcio_get(conn, retval);

		int errorcode;
		ipcio_get(conn, errorcode);
		conn.get_sync();

		// Reset errno with the value set by the monitor.
		errno = errorcode;

		return retval;
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while removing file", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

bool PrivilegeManager::removeDirectory(char const * path)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->removeDirectory(path);
	}

	try
	{
		CHECK(pimpl(), "removeDirectory: process has no privileges");
		pimpl()->verifyValidConnection("removeDirectory: no connection to the monitor");
		NonRecursiveMutexLock lock(pimpl()->m_mutex);

		IPCIO& conn = *pimpl()->m_conn;
		ipcio_put(conn, PrivilegeCommon::E_CMD_REMOVE_DIR);
		ipcio_put(conn, path);
		conn.put_sync();

		check_result(conn);
		bool retval;
		ipcio_get(conn, retval);

		int errorcode;
		ipcio_get(conn, errorcode);
		conn.get_sync();

		// Reset errno with the value set by the monitor.
		errno = errorcode;

		return retval;
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while removing directory", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

ProcessRef PrivilegeManager::monitoredSpawn(
	char const * exec_path,
	char const * app_name,
	char const * const argv[], char const * const envp[]
)
{
	char const * default_argv[2] = { exec_path, 0 };
	if (!argv || !*argv)
	{
		argv = default_argv;
	}
	if (!envp)
	{
		envp = environ;
	}

	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->monitoredSpawn(exec_path, app_name, argv, envp);
	}

	CHECK(pimpl(), "monitoredSpawn: process has no privileges");
	pimpl()->verifyValidConnection("monitoredSpawn: no connection to the monitor");

	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);
		IPCIO& conn = *pimpl()->m_conn;

		ipcio_put(conn, PrivilegeCommon::E_CMD_MONITORED_SPAWN);
		ipcio_put(conn, exec_path);
		ipcio_put(conn, app_name);
		ipcio_put_strarr(conn, argv);
		ipcio_put_strarr(conn, envp);
		conn.put_sync();

		check_result(conn, IPCIO::E_UNBUFFERED);
		UnnamedPipeRef in = UnnamedPipe::createUnnamedPipeFromDescriptor(AutoDescriptor(), conn.get_handle());
		UnnamedPipeRef out = UnnamedPipe::createUnnamedPipeFromDescriptor(conn.get_handle(), AutoDescriptor());
		UnnamedPipeRef err = UnnamedPipe::createUnnamedPipeFromDescriptor(conn.get_handle(), AutoDescriptor());
		ProcId pid;
		ipcio_get(conn, pid);
		conn.get_sync();
		return ProcessRef(new MonitorChild(in, out, err, pid, *this));
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while executing monitored spawn", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

ProcessRef PrivilegeManager::monitoredUserSpawn(
	char const * exec_path,
	char const * app_name,
	char const * const argv[], char const * const envp[],
	char const * user
)
{
	char const * default_argv[2] = { exec_path, 0 };
	if (!argv || !*argv)
	{
		argv = default_argv;
	}
	if (!envp)
	{
		envp = environ;
	}

	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->monitoredUserSpawn(exec_path, app_name, argv, envp, user);
	}

	CHECK(pimpl(), "monitoredUserSpawn: process has no privileges");
	pimpl()->verifyValidConnection("monitoredUserSpawn: no connection to the monitor");

	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);
		IPCIO& conn = *pimpl()->m_conn;

		ipcio_put(conn, PrivilegeCommon::E_CMD_MONITORED_USER_SPAWN);
		ipcio_put(conn, exec_path);
		ipcio_put(conn, app_name);
		ipcio_put_strarr(conn, argv);
		ipcio_put_strarr(conn, envp);
		ipcio_put(conn, user);
		conn.put_sync();

		check_result(conn, IPCIO::E_UNBUFFERED);
		UnnamedPipeRef in = UnnamedPipe::createUnnamedPipeFromDescriptor(AutoDescriptor(), conn.get_handle());
		UnnamedPipeRef out = UnnamedPipe::createUnnamedPipeFromDescriptor(conn.get_handle(), AutoDescriptor());
		UnnamedPipeRef err = UnnamedPipe::createUnnamedPipeFromDescriptor(conn.get_handle(), AutoDescriptor());
		ProcId pid;
		ipcio_get(conn, pid);
		conn.get_sync();
		return ProcessRef(new MonitorChild(in, out, err, pid, *this));
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while executing monitored user spawn", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

int PrivilegeManager::kill(ProcId pid, int sig)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->kill(pid, sig);
	}

	CHECK(pimpl(), "kill: process has no privileges");
	pimpl()->verifyValidConnection("kill: no connection to the monitor");

	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);
		IPCIO& conn = *pimpl()->m_conn;

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
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while executing kill", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

Process::Status PrivilegeManager::pollStatus(ProcId pid)
{
	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->pollStatus(pid);
	}

	CHECK(pimpl(), "pollStatus: process has no privileges");
	pimpl()->verifyValidConnection("pollStatus: no connection to the monitor");

	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);
		IPCIO& conn = *pimpl()->m_conn;

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
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while executing poll status", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

ProcessRef PrivilegeManager::userSpawn(
	char const * exec_path,
	char const * const argv[], char const * const envp[],
	char const * user, char const * working_dir
)
{
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

	if( g_privilegeManagerMockObject )
	{
		return g_privilegeManagerMockObject->userSpawn(exec_path, argv, envp, user, working_dir);
	}

	CHECK(pimpl(), "userSpawn: process has no privileges");
	pimpl()->verifyValidConnection("userSpawn: no connection to the monitor");

	try
	{
		NonRecursiveMutexLock lock(pimpl()->m_mutex);
		IPCIO& conn = *pimpl()->m_conn;

		ipcio_put(conn, PrivilegeCommon::E_CMD_USER_SPAWN);
		ipcio_put(conn, exec_path);
		ipcio_put_strarr(conn, argv);
		ipcio_put_strarr(conn, envp);
		ipcio_put(conn, user);
		ipcio_put(conn, working_dir);
		conn.put_sync();

		check_result(conn, IPCIO::E_UNBUFFERED);
		UnnamedPipeRef in = UnnamedPipe::createUnnamedPipeFromDescriptor(AutoDescriptor(), conn.get_handle());
		UnnamedPipeRef out = UnnamedPipe::createUnnamedPipeFromDescriptor(conn.get_handle(), AutoDescriptor());
		UnnamedPipeRef err = UnnamedPipe::createUnnamedPipeFromDescriptor(conn.get_handle(), AutoDescriptor());
		ProcId pid;
		ipcio_get(conn, pid);
		conn.get_sync();
		return ProcessRef(new MonitorChild(in, out, err, pid, *this));
	}
	catch(const IPCIOException& e)
	{
		pimpl()->invalidateConnection();
		OW_THROW_SUBEX(MonitorCommunicationException, "Communication error while executing user spawn", e);
	}
	catch(const FatalPrivilegeManagerException& e)
	{
		pimpl()->invalidateConnection();
		throw;
	}
}

bool
PrivilegeManager::isNull() const
{
	return pimpl() == 0;
}

PrivilegeManagerImpl*
PrivilegeManager::pimpl() const
{
	return m_impl.getPtr();
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
