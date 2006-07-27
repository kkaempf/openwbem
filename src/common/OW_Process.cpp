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

/**
* @author Kevin S. Van Horn
* @author Dan Nuffer (code modified from OW_Exec.cpp)
*/

#include "OW_config.h"

#include "OW_DateTime.hpp"
#include "OW_Exec.hpp"
  // To get ExecErrorException declaration
#include "OW_Format.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_Process.hpp"
#include "OW_SafeCString.hpp"
#include "OW_String.hpp"
#include "OW_Thread.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Paths.hpp"
#include "OW_TimeoutTimer.hpp"
#include "OW_SignalUtils.hpp"

#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cmath>
#include <algorithm>
#include <limits>

#if defined(sigemptyset)
// We want to use the function instead of the macro (for scoping reasons).
#undef sigemptyset
#endif // sigemptyset

using namespace OpenWBEM;

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION(ProcessError);

// --- Process::Status ---

Process::Status::Status(ProcId pid, int status)
: m_status_available(pid > 0),
  m_status(status)
{
}

Process::Status::Status(int rep1, int rep2, Repr)
: m_status_available(static_cast<bool>(rep1)),
  m_status(rep2)
{
}

Process::Status::Status()
: m_status_available(false),
  m_status(0)
{
}

void Process::Status::repr(int & rep1, int & rep2)
{
	rep1 = static_cast<int>(m_status_available);
	rep2 = m_status;
}

bool Process::Status::running() const
{
	return !m_status_available;
}

bool Process::Status::terminated() const
{
	return m_status_available && (WIFEXITED(m_status) || WIFSIGNALED(m_status));
}

bool Process::Status::exitTerminated() const
{
	return m_status_available && WIFEXITED(m_status);
}

bool Process::Status::terminatedSuccessfully() const
{
	return exitTerminated() && exitStatus() == 0;
}

int Process::Status::exitStatus() const
{
	return WEXITSTATUS(m_status);
}

bool Process::Status::signalTerminated() const
{
	return m_status_available && WIFSIGNALED(m_status);
}

int Process::Status::termSignal() const
{
	return WTERMSIG(m_status);
}

bool Process::Status::stopped() const
{
	return m_status_available && WIFSTOPPED(m_status);
}

int Process::Status::stopSignal() const
{
	return WSTOPSIG(m_status);
}


String
Process::Status::toString() const
{
	if (running())
	{
		return "running";
	}
	else if (stopped())
	{
		return Format("stopped by %1", SignalUtils::signalName(stopSignal()));
	}
	else if (terminated())
	{
		if (exitTerminated())
		{
			return Format("exited with status %1", String(exitStatus()));
		}
		else if (signalTerminated())
		{
			return Format("terminated by signal %1", SignalUtils::signalName(termSignal()));
		}
	}
	return "Unknown";
}

// --- Process ---

Process::Process(
	UnnamedPipeRef const & in, UnnamedPipeRef const & out,
	UnnamedPipeRef const & err, ProcId pid
)
: m_in(in),
  m_out(out),
  m_err(err),
  m_pid(pid),
  m_status()
{
}

Process::Process(ProcId pid)
: m_in(),
  m_out(),
  m_err(),
  m_pid(pid),
  m_status()
{
}

Process::~Process()
{
	if (m_pid < 0)
	{
		return;
	}
	try
	{
		this->waitCloseTerm(Timeout::relative(0.0), Timeout::relative(1.0), Timeout::relative(2.0));
	}
	catch (...)
	{
	}
}

void Process::release()
{
	m_in = 0;
	m_out = 0;
	m_err = 0;
	m_pid = -1;
}

UnnamedPipeRef Process::in() const
{
	return m_in;
}

UnnamedPipeRef Process::out() const
{
	return m_out;
}

UnnamedPipeRef Process::err() const
{
	return m_err;
}

ProcId Process::pid() const
{
	return m_pid;
}

Process::Status Process::processStatus()
{
	// m_pid tested in case this method is called inappropriately
	if (m_pid >= 0 && !m_status.terminated())
	{
		m_status = this->pollStatus(m_pid);
	}
	return m_status;
}

namespace
{
	inline void upr_close(UnnamedPipeRef & x)
	{
		if (x)
		{
			x->close();
		}
	}
}

void Process::waitCloseTerm(float wait_initial, float wait_close, float wait_term)
{
	waitCloseTerm(Timeout::relative(wait_initial), Timeout::relative(wait_close), Timeout::relative(wait_term));
}

void Process::waitCloseTerm(
	const Timeout& wait_initial, const Timeout& wait_close, const Timeout& wait_term)
{
	if (m_pid < 0) // safety check in case called inappropriately
	{
		return;
	}
	if (m_status.terminated())
	{
		return;
	}
	if (m_pid == ::getpid())
	{
		OW_THROW(ProcessErrorException, "Process::m_pid == getpid()");
	}

	TimeoutTimer initialTimer(wait_initial);
	TimeoutTimer closeTimer(wait_close);
	TimeoutTimer termTimer(wait_term);

	if (wait_initial.getType() == Timeout::E_RELATIVE && wait_initial.getRelative() > 0 && this->terminatesWithin(initialTimer.asAbsoluteTimeout()))
	{
		return;
	}

	if (wait_close.getType() == Timeout::E_RELATIVE && wait_close.getRelative() > 0)
	{
		// Close the streams. If the child process is blocked waiting to output,
		// then this will cause it to get a SIGPIPE, and it may be able to clean
		// up after itself.  Likewise, if the child process is blocked waiting
		// for input, it will now detect EOF.
		upr_close(m_in);
		upr_close(m_out);
		upr_close(m_err);
		if (this->terminatesWithin(closeTimer.asAbsoluteTimeout()))
		{
			return;
		}
	}

	if (wait_term.getType() == Timeout::E_RELATIVE && wait_term.getRelative() > 0 && this->killWait(termTimer.asAbsoluteTimeout(), SIGTERM, "SIGTERM"))
	{
		return;
	}
	// Give it a full minute to make sure we don't leave zombies hanging around
	// if the system is heavily loaded
	Timeout const sigkillTimeout = Timeout::relative(60.0);
	if (!killWait(sigkillTimeout, SIGKILL, "SIGKILL"))
	{
		OW_THROW(
			ProcessErrorException, "Child process has not terminated after sending it a SIGKILL."
		);
	}
}

int Process::kill(ProcId pid, int sig)
{
	return ::kill(pid, sig) == 0 ? 0 : errno;
}

Process::Status Process::pollStatus(ProcId pid)
{
	ProcId wpid;
	int status;
	do
	{
		// Use WUNTRACED so that we can detect if process stopped
		wpid = ::waitpid(pid, &status, WNOHANG | WUNTRACED);
	} while (wpid < 0 && errno == EINTR);
	if (wpid < 0)
	{
		OW_THROW_ERRNO_MSG(ProcessErrorException, "waitpid() failed");
	}
	return Status(wpid, status);
}

// Waits wait_time at most wait_time seconds for process to terminate, setting
// m_status.
// RETURNS: whether or not process terminated.
//
bool Process::terminatesWithin(const Timeout& wait_time)
{
	float const mult = 1.20;
	float const max_period = 5000.0; // milliseconds
	float period = 100.0; // milliseconds
	TimeoutTimer timer(wait_time);
	while (!timer.expired() && !m_status.terminated())
	{
		Thread::sleep(static_cast<UInt32>(period));
		period = std::min(max_period, period * mult);
		m_status = this->pollStatus(m_pid);
		timer.loop();
	}
	return m_status.terminated();
}

// Sends signal sig to child process and waits wait_time seconds for it
// to terminate.  If an error occurs, signame is used in constructing the
// error message.
//
bool Process::killWait(const Timeout& wait_time, int sig, char const * signame)
{
	int errnum = this->kill(m_pid, sig);
	if (errnum != 0)
	{
		// maybe kill() failed because child terminated first
		if (this->processStatus().terminated())
		{
			return true;
		}
		else {
			Format fmt("Failed sending %1 to process %2.", signame, m_pid);
			char const * msg = fmt.c_str();
			errno = errnum;
			OW_THROW_ERRNO_MSG(ProcessErrorException, msg);
		}
	}
	return this->terminatesWithin(wait_time);
}


} // namespace OW_NAMESPACE
