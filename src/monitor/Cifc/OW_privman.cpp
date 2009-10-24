// This file does not appear to be used anywhere.  I believe this was the
// original implementation, but is no longer used in favor of the
// OW_privman_noexcept.cpp file.  The exceptionless version was created to work
// around bugs (AIX) where (under some circumstances) throwing an exception in
// a shared library (even if it was immediately caught) would cause
// __cxx_terminate() to be called.

#include "OW_config.h"
#include "OW_Exception.hpp"
#include "blocxx/Format.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_SafeCString.hpp"
#include "blocxx/StringStream.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include <map>

extern "C"
{
#include "OW_privman.h"
}

using namespace OpenWBEM;

namespace OW_NAMESPACE
{
	OW_DECLARE_EXCEPTION(PerlProcessMap);
	OW_DEFINE_EXCEPTION(PerlProcessMap);
}

namespace
{
	PrivilegeManager mgr = PrivilegeManager::connectToMonitor();

	typedef std::map<int, ProcessRef> procmap_t;
	procmap_t procmap;

	void copy_errmsg(Exception const & e, char * errbuf, size_t bufsize)
	{
		OStringStream oss;
		oss << e;
		SafeCString::strcpy_trunc(errbuf, bufsize, oss.c_str());
	}

	ProcessRef process(int pid)
	{
		procmap_t::const_iterator it = procmap.find(pid);
		if (it == procmap.end())
		{
			OW_THROW(PerlProcessMapException,
				Format("Process %1 does not exist", pid).c_str());
		}
		else
		{
			return it->second;
		}
	}

	inline Process::Status procstat(int rep1, int rep2)
	{
		return Process::Status(rep1, rep2, Process::Status::Repr());
	}
}

#define BEGIN \
	try

#define END(errbuf, bufsize) \
	catch (Exception & e) \
 	{ \
		copy_errmsg(e, errbuf, bufsize); \
		return e.getErrorCode(); \
	} \
	catch (...) \
	{ \
		SafeCString::strcpy_trunc(errbuf, bufsize, "Unknown exception thrown"); \
		return Exception::UNKNOWN_ERROR_CODE; \
	} \
	return 0;

int owprivman_open(
	char const * pathname, unsigned flags, unsigned perms,
	int * retval, char * errbuf, size_t bufsize
)
{
	BEGIN
	{
		*retval = mgr.open(
			pathname,
			static_cast<PrivilegeManager::OpenFlags>(flags),
			static_cast<PrivilegeManager::OpenPerms>(perms)
		);
	}
	END(errbuf, bufsize)
}

int owprivman_read_directory(
	char const * pathname, int keep_special, string_handler_t h, void * p,
	char * errbuf, size_t bufsize
)
{
	BEGIN
	{
		PrivilegeManager::ReadDirOptions opt =
			keep_special ? PrivilegeManager::E_KEEP_SPECIAL :
			PrivilegeManager::E_OMIT_SPECIAL;
		StringArray results = mgr.readDirectory(pathname, opt);
		for (size_t i = 0; i < results.size(); ++i)
		{
			h(results[i].c_str(), p);
		}
	}
	END(errbuf, bufsize)
}

int owprivman_rename(
	char const * old_path, char const * new_path,
	char * errbuf, size_t bufsize
)
{
	BEGIN
	{
		mgr.rename(old_path, new_path);
	}
	END(errbuf, bufsize)
}

int owprivman_unlink(
	char const * path, int * retval, char * errbuf, size_t bufsize
)
{
	BEGIN
	{
		*retval = static_cast<int>(mgr.unlink(path));
	}
	END(errbuf, bufsize)
}

int owprivman_monitored_spawn(
	char const * exec_path, char const * app_name,
	char const * const * argv, char const * const * envp,
	int * retval, char * errbuf, size_t bufsize
)
{
	BEGIN
	{
		ProcessRef pproc = mgr.monitoredSpawn(exec_path, app_name, argv, envp);
		ProcId pid = pproc->pid();
		procmap[pid] = pproc;
		*retval = pid;
	}
	END(errbuf, bufsize)
}

int owprivman_user_spawn(
	char const * exec_path,
	char const * const * argv, char const * const * envp,
	char const * user,
	int * retval, char * errbuf, size_t bufsize
)
{
	BEGIN
	{
		ProcessRef pproc = mgr.userSpawn(exec_path, argv, envp, user);
		ProcId pid = pproc->pid();
		procmap[pid] = pproc;
		*retval = static_cast<int>(pid);
	}
	END(errbuf, bufsize)
}

int owprivman_child_stdin(int pid, int * retval, char * errbuf, size_t bufsize)
{
	BEGIN
	{
		*retval = static_cast<int>(process(pid)->in()->getOutputDescriptor());
	}
	END(errbuf, bufsize)
}

int owprivman_child_stdout(int pid, int * retval, char * errbuf, size_t bufsize)
{
	BEGIN
	{
		*retval = static_cast<int>(process(pid)->out()->getInputDescriptor());
	}
	END(errbuf, bufsize)
}

int owprivman_child_stderr(int pid, int * retval, char * errbuf, size_t bufsize)
{
	BEGIN
	{
		*retval = static_cast<int>(process(pid)->err()->getInputDescriptor());
	}
	END(errbuf, bufsize)
}

int owprivman_child_status(
	int pid, int * rep1, int * rep2, char * errbuf, size_t bufsize)
{
	BEGIN
	{
		process(pid)->processStatus().repr(*rep1, *rep2);
	}
	END(errbuf, bufsize)
}



int owprivman_status_running(int rep1, int rep2)
{
	return static_cast<int>(procstat(rep1, rep2).running());
}

int owprivman_status_terminated(int rep1, int rep2)
{
	return static_cast<int>(procstat(rep1, rep2).terminated());
}

int owprivman_status_exit_terminated(int rep1, int rep2)
{
	return static_cast<int>(procstat(rep1, rep2).exitTerminated());
}

int owprivman_status_exit_status(int rep1, int rep2)
{
	return procstat(rep1, rep2).exitStatus();
}

int owprivman_status_terminated_successfully(int rep1, int rep2)
{
	return static_cast<int>(procstat(rep1, rep2).terminatedSuccessfully());
}

int owprivman_status_signal_terminated(int rep1, int rep2)
{
	return static_cast<int>(procstat(rep1, rep2).signalTerminated());
}

int owprivman_status_term_signal(int rep1, int rep2)
{
	return procstat(rep1, rep2).termSignal();
}

int owprivman_status_stopped(int rep1, int rep2)
{
	return static_cast<int>(procstat(rep1, rep2).stopped());
}

int owprivman_status_stop_signal(int rep1, int rep2)
{
	return static_cast<int>(procstat(rep1, rep2).stopSignal());
}

int owprivman_child_wait_close_term(
	int pid, float wait_initial, float wait_close, float wait_term,
	int * status_rep1, int * status_rep2, char * errbuf, size_t bufsize
)
{
	BEGIN
	{
		ProcessRef proc = process(pid);
		procmap.erase(pid);
		proc->waitCloseTerm(Timeout::relative(wait_initial), Timeout::relative(wait_close), Timeout::relative(wait_term));
		proc->processStatus().repr(*status_rep1, *status_rep2);
	}
	END(errbuf, bufsize)
}
