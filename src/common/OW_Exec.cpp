/*******************************************************************************
* Copyright (C) 2004-2005 Quest Software, Inc. All rights reserved.
* Copyright (C) 2005 Novell, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Vintela, Inc., Novell, Inc., nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc., Novell, Inc., OR THE 
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_Exec.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_Array.hpp"
#include "OW_IOException.hpp"
#include "OW_Thread.hpp"
#include "OW_Select.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_DateTime.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_TimeoutTimer.hpp"
#include "OW_Process.hpp"
#include "OW_SafeCString.hpp"
#include "OW_Paths.hpp"
#include "OW_ExecMockObject.hpp"
#include "OW_GlobalPtr.hpp"

#include <map>
#include <limits>

extern "C"
{
#ifdef OW_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifndef OW_WIN32
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif
#include <errno.h>
#include <stdio.h> // for perror
#include <signal.h>
}

#include <cerrno>
#include <iostream>	// for cerr

// NSIG may be defined by signal.h, otherwise 64 should be plenty.
#ifndef NSIG
#define NSIG 64
#endif

#if defined(sigemptyset)
// We want to use the function instead of the macro (for scoping reasons).
#undef sigemptyset
#endif // sigemptyset

namespace OW_NAMESPACE
{

using std::cerr;
using std::endl;
OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID(ExecTimeout, ExecErrorException);
OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID(ExecBufferFull, ExecErrorException);
OW_DEFINE_EXCEPTION_WITH_ID(ExecError);


//////////////////////////////////////////////////////////////////////////////
namespace Exec
{
::OW_NAMESPACE::GlobalPtr<ExecMockObject, NullFactory> g_execMockObject = OW_GLOBAL_PTR_INIT;

namespace
{

class SystemPreExec : public PreExec
{
public:

	SystemPreExec()
	: PreExec(true)
	{
	}

	/// @return true
	//
	virtual bool keepStd(int d) const
	{
		return true; // want them all unchanged
	}
	
	/**
	* Resets all signals to their default actions and sets to
	* close-on-exec all descriptors except the standard descriptors.
	*/
	virtual void call(pipe_pointer_t const pparr[])
	{
		std::vector<bool> empty;
		PreExec::resetSignals();
		PreExec::closeDescriptorsOnExec(empty);
	}
};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
Process::Status
system(const Array<String>& command, const char* const envp[], const Timeout& timeout)
{
	SystemPreExec spe;
	ProcessRef proc = Exec::spawn(command[0], command, envp, spe);

	proc->waitCloseTerm(Timeout::relative(0), timeout, Timeout::relative(0));
	return proc->processStatus();

}

namespace // anonymous
{
	long getMaxOpenFiles()
	{
		long sysconfValue = sysconf(_SC_OPEN_MAX);
		long maxOpen = sysconfValue;
		rlimit rl;
		rl.rlim_cur = rlim_t(0);
		if( getrlimit(RLIMIT_NOFILE, &rl) != -1 )
		{
			if( sysconfValue < 0 )
			{
				maxOpen = rl.rlim_cur;
			}
			else
			{
				maxOpen = std::min<rlim_t>(rl.rlim_cur, sysconfValue);
			}
		}
		// Check for a value of maxOpen that really is reasonable.
		// This checks the maximum value to make sure it will fit in an int
		// (required for close).
		OW_ASSERT( (maxOpen > 2) && (maxOpen <= long(std::numeric_limits<int>::max())) );
		return maxOpen;
	}

	unsigned const IN       = 0;
	unsigned const OUT      = 1;
	unsigned const ERR      = 2;
	unsigned const EXEC_ERR = 3;
	unsigned const NPIPE = 4;

	void throw_child_error(Exec::PreExec::Error const & err, const String& process_path)
	{
		Format msg("Exec::spawn(%1): child startup failed: %2", process_path, err.message);
		if (err.error_num != 0)
		{
			OW_THROW_ERRNO_MSG1(
				ExecErrorException,	msg.c_str(), err.error_num);
		}
		else
		{
			OW_THROW(ExecErrorException, msg.c_str());
		}
	}

	void check(bool b, char const * message, bool use_errno = true)
	{
		if (!b)
		{
			Exec::PreExec::Error x;
			SafeCString::strcpy_trunc(x.message, message);
			x.error_num = use_errno ? errno : 0;
			throw x;
		}
	}

	void parent_check(bool b, char const * msg)
	{
		if (!b)
		{
			OW_THROW(ExecErrorException, msg);
		}
	}

	void close_on_exec(int descr, bool may_be_bad)
	{
		int e = ::fcntl(descr, F_SETFD, FD_CLOEXEC);
		check(e == 0 || may_be_bad && errno == EBADF, "fcntl");
	}

	void init_child(
		char const * exec_path, 
		char const * const argv[], char const * const envp[],
		Exec::PreExec & pre_exec, PosixUnnamedPipe * ppipe[NPIPE]
	)
	{
		// This code must be careful not to allocate memory, as this can
		// cause a deadlock on some platforms when there are multiple
		// threads running at the time of the fork().

		int exec_err_desc = -1;
		Exec::PreExec::Error err;
		err.error_num = 0;      // should be unnecessary, but just in case...
		err.message[0] = '\0';  // should be unnecessary, but just in case...
		try
		{
			int rc;
			exec_err_desc = ppipe[EXEC_ERR]->getOutputHandle();
			pre_exec.call(ppipe);

			int rval = 0;
			char * const * cc_argv = const_cast<char * const *>(argv);
			char * const * cc_envp = const_cast<char * const *>(envp);
			if (envp)
			{
				check(::execve(exec_path, cc_argv, cc_envp) != -1, "execve");
			}
			else
			{
				check(::execv(exec_path, cc_argv) != -1, "execv");
			}
		}
		catch (Exec::PreExec::Error & e)
		{
			err = e;
		}
		catch (std::exception & e)
		{
			SafeCString::strcpy_trunc(err.message, e.what());
			err.error_num = 0;
		}
		catch (Exec::PreExec::DontCatch & e)
		{
			throw;
		}
		catch (...)
		{
			SafeCString::strcpy_trunc(err.message, "unknown exception");
			err.error_num = 0;
		}
		::write(exec_err_desc, &err, sizeof(err));
		::_exit(127);
	}

	void close_child_ends(PosixUnnamedPipe * ppipe[NPIPE])
	{
		// prevent the parent from using the child's end of the pipes.
		if (ppipe[IN])
		{
			ppipe[IN]->closeInputHandle();
		}
		if (ppipe[OUT])
		{
			ppipe[OUT]->closeOutputHandle();
		}
		if (ppipe[ERR])
		{
			ppipe[ERR]->closeOutputHandle();
		}
		ppipe[EXEC_ERR]->closeOutputHandle();
	}

	void handle_child_error(
		int rc, Exec::PreExec::Error const & ce, Process & proc, const String& process_path)
	{
		if (rc < 0) // read of error status from child failed
		{
			int errnum = errno;
			// For some reason child initialization failed; kill it.
			proc.waitCloseTerm(Timeout::relative(0.0), Timeout::relative(0.0), Timeout::relative(0.0));
			if (errnum == ETIMEDOUT)
			{
				OW_THROW(ExecErrorException,
					Format("Exec::spawn(%1): timed out waiting for child to exec()",process_path).c_str());
			}
			OW_THROW_ERRNO_MSG1(ExecErrorException,
				Format("Exec::spawn(%1): error reading init status from child",process_path).c_str(), errnum);
		}
		if (rc > 0) // child sent an initialization error message
		{
			throw_child_error(ce, process_path);
		}
		// If rc == 0, initialization succeeded
	}

} // end anonymous namespace

ProcessRef spawn(
	char const * exec_path,
	char const * const argv[], char const * const envp[],
	PreExec & pre_exec
)
{
	// It's important that this code be exception-safe (proper release
	// of resources when exception thrown), as at least one caller
	// (the monitor code) relies on being able to throw a DontCatch-derived
	// exception from pre_exec.call() in the child process and have
	// it propagate out of the spawn call.
	//
	parent_check(exec_path, "Exec::spawn: null exec_path");
	char const * default_argv[2] = { exec_path, 0 };
	if (!argv || !*argv)
	{
		argv = default_argv;
	}

	// Check this here so that any exceptions or core files caused by it can
	// be traced to a real problem instead of the child processes just
	// failing for an unreportable reason.
	getMaxOpenFiles();

	UnnamedPipeRef upipe[NPIPE];
	PosixUnnamedPipe * ppipe[NPIPE];
	for (unsigned i = 0; i < NPIPE; ++i)
	{
		if (i == EXEC_ERR || pre_exec.keepStd(i))
		{
			upipe[i] = UnnamedPipe::createUnnamedPipe();
		}
		ppipe[i] = dynamic_cast<PosixUnnamedPipe *>(upipe[i].getPtr());
	}

	::pid_t child_pid = ::fork();
	if (child_pid == 0) // child process
	{
		init_child(exec_path, argv, envp, pre_exec, ppipe); // never returns
	}
	parent_check(child_pid >= 0, Format("Exec::spawn(%1): fork() failed", exec_path).c_str());
	close_child_ends(ppipe);

	const Timeout SECONDS_TO_WAIT_FOR_CHILD_TO_EXEC = Timeout::relative(10);
	// 10 seconds should be plenty for the child to go from fork() to execv()
	upipe[EXEC_ERR]->setReadTimeout(SECONDS_TO_WAIT_FOR_CHILD_TO_EXEC);

	ProcessRef retval(new Process(upipe[0], upipe[1], upipe[2], child_pid));

	PreExec::Error child_error;
	int nread = upipe[EXEC_ERR]->read(&child_error, sizeof(child_error));
	handle_child_error(nread, child_error, *retval, exec_path);

	return retval;
}

void PreExec::resetSignals()
{
	/*
	according to susv3:
	
	This  volume  of  IEEE Std 1003.1-2001  specifies  that signals set to
	SIG_IGN remain set to SIG_IGN, and that  the  process  signal  mask be
	unchanged across an exec. This is consistent with historical implemen-
	tations, and it permits some useful functionality, such  as  the nohup
	command.  However,  it should be noted that many existing applications
	wrongly assume that they start with certain signals set to the default
	action  and/or  unblocked.  In particular, applications written with a
	simpler signal model that does not include blocking of signals, such as
	the one in the ISO C standard, may not behave properly if invoked with
	some signals blocked. Therefore, it is best not to block or ignore sig-
	nals across execs without explicit reason to do so, and especially not
	to block signals across execs of arbitrary (not  closely co-operating)
	programs.
	
	so we'll reset the signal mask and all signal handlers to SIG_DFL.
	We set them all just in case the current handlers may misbehave now
	that we've fork()ed.
	*/
	int rc;
	::sigset_t emptymask;
	check(::sigemptyset(&emptymask) == 0, "sigemptyset");
	check(::sigprocmask(SIG_SETMASK, &emptymask, 0) == 0, "sigprocmask");

	for (std::size_t sig = 1; sig <= NSIG; ++sig)
	{
		if (sig == SIGKILL || sig == SIGSTOP)
		{
			continue;
		}
		struct sigaction temp;
		int e = ::sigaction(sig, 0, &temp);
		check(e == 0 || errno == EINVAL, "sigaction [1]");
		if (e == 0 && temp.sa_handler != SIG_DFL) // valid signal
		{
			temp.sa_handler = SIG_DFL;
			// note that we don't check the return value because there are signals 
			// (e.g. SIGGFAULT on HP-UX), which are gettable, but not settable.
			::sigaction(sig, &temp, 0);
		}
	}
}

void PreExec::closeDescriptorsOnExec(std::vector<bool> const & keep)
{
	long numd = m_max_descriptors ? m_max_descriptors : getMaxOpenFiles();
	for (int d = 3; d < int(numd); ++d) // Don't close standard descriptors
	{
		if (size_t(d) >= keep.size() || !keep[d])
		{
			close_on_exec(d, true);
		}
	}
}

void PreExec::setupStandardDescriptors(pipe_pointer_t const ppipe[])
{
	int nulld = 0;
	if (!(ppipe[0] && ppipe[1] && ppipe[2]))
	{
		nulld = ::open(_PATH_DEVNULL, O_RDWR);
		check(nulld >= 0, "open");
		close_on_exec(nulld, false);
	}
	for (unsigned d = 0; d < 3; ++d)
	{
		PosixUnnamedPipe * p = ppipe[d];
		int ddup =
			!p ? nulld : d==IN ? p->getInputHandle() : p->getOutputHandle();
		check(::dup2(ddup, d) != -1, "dup2");
	}
}

void PreExec::closePipesOnExec(pipe_pointer_t const ppipe[])
{
	for (unsigned d = 0; d < NPIPE; ++d)
	{
		PosixUnnamedPipe * p = ppipe[d];
		if (p)
		{
			close_on_exec(p->getInputHandle(), false);
			close_on_exec(p->getOutputHandle(), false);
		}
	}
}

PreExec::PreExec(bool precompute_max_descriptors)
: m_max_descriptors(precompute_max_descriptors ? getMaxOpenFiles() : 0)
{
}

PreExec::~PreExec()
{
}

PreExec::DontCatch::~DontCatch()
{
}

StandardPreExec::StandardPreExec()
: PreExec(true)
{
}

bool StandardPreExec::keepStd(int) const
{
	return true;
}

void StandardPreExec::call(pipe_pointer_t const pparr[])
{
	std::vector<bool> empty;
	PreExec::resetSignals();
	PreExec::setupStandardDescriptors(pparr);
	PreExec::closeDescriptorsOnExec(empty);
}

ProcessRef spawn(
	char const * const argv[], char const * const envp[]
)
{
	StandardPreExec pre_exec;
	return spawn(argv[0], argv, envp, pre_exec);
}

namespace
{

#ifndef OW_MIN
#define OW_MIN(x, y) (x) < (y) ? (x) : (y)
#endif

/////////////////////////////////////////////////////////////////////////////
class StringOutputGatherer : public OutputCallback
{
public:
	StringOutputGatherer(String& output, int outputLimit)
		: m_output(output)
		, m_outputLimit(outputLimit)
	{
	}
private:
	virtual void doHandleData(const char* data, size_t dataLen, EOutputSource outputSource, const ProcessRef& theProc, size_t streamIndex, Array<char>& inputBuffer)
	{
		if (m_outputLimit >= 0 && m_output.length() + dataLen > static_cast<size_t>(m_outputLimit))
		{
			// the process output too much, so just copy what we can and return error
			int lentocopy = OW_MIN(m_outputLimit - m_output.length(), dataLen);
			if (lentocopy >= 0)
			{
				m_output += String(data, lentocopy);
			}
			OW_THROW(ExecBufferFullException, "Exec::StringOutputGatherer::doHandleData(): buffer full");
		}

		m_output += data;
	}
	String& m_output;
	int m_outputLimit;
};

/////////////////////////////////////////////////////////////////////////////
class SingleStringInputCallback : public InputCallback
{
public:
	SingleStringInputCallback(const String& s)
		: m_s(s)
	{
	}
private:
	virtual void doGetData(Array<char>& inputBuffer, const ProcessRef& theProc, size_t streamIndex)
	{
		if (m_s.length() > 0)
		{
			inputBuffer.insert(inputBuffer.end(), m_s.c_str(), m_s.c_str() + m_s.length());
			m_s.erase();
		}
		else if (theProc->in()->isOpen())
		{
			theProc->in()->close();
		}
	}
	String m_s;
};

}// end anonymous namespace

/////////////////////////////////////////////////////////////////////////////
Process::Status executeProcessAndGatherOutput(
	char const * const command[],
	String& output,
	char const * const envVars[],
	const Timeout& timeout,
	int outputLimit,
	char const * input)
{
	if (g_execMockObject.get())
	{
		return g_execMockObject.get()->executeProcessAndGatherOutput(command, output, envVars, timeout, outputLimit, input);
	}
	return feedProcessAndGatherOutput(spawn(command, envVars),
		output, timeout, outputLimit, input);
}

/////////////////////////////////////////////////////////////////////////////
Process::Status feedProcessAndGatherOutput(
	ProcessRef const & proc,
	String & output,
	Timeout const & timeout, 
	int outputLimit, 
	String const & input)
{
	Array<ProcessRef> procarr(1, proc);
	SingleStringInputCallback singleStringInputCallback(input);

	StringOutputGatherer gatherer(output, outputLimit);
	processInputOutput(gatherer, procarr, singleStringInputCallback, timeout);
	proc->waitCloseTerm();
	return proc->processStatus();
}

/////////////////////////////////////////////////////////////////////////////
void
gatherOutput(String& output, const ProcessRef& proc, int timeoutSecs, int outputLimit)
{
	gatherOutput(output, proc, Timeout::relativeWithReset(timeoutSecs), outputLimit);
}
/////////////////////////////////////////////////////////////////////////////
void
gatherOutput(String& output, const ProcessRef& proc, const Timeout& timeout, int outputLimit)
{
	Array<ProcessRef> procs(1, proc);

	StringOutputGatherer gatherer(output, outputLimit);
	SingleStringInputCallback singleStringInputCallback = SingleStringInputCallback(String());
	processInputOutput(gatherer, procs, singleStringInputCallback, timeout);
}

/////////////////////////////////////////////////////////////////////////////
OutputCallback::~OutputCallback()
{

}

/////////////////////////////////////////////////////////////////////////////
void
OutputCallback::handleData(const char* data, size_t dataLen, EOutputSource outputSource, const ProcessRef& theProc, size_t streamIndex, Array<char>& inputBuffer)
{
	doHandleData(data, dataLen, outputSource, theProc, streamIndex, inputBuffer);
}

/////////////////////////////////////////////////////////////////////////////
InputCallback::~InputCallback()
{
}

/////////////////////////////////////////////////////////////////////////////
void
InputCallback::getData(Array<char>& inputBuffer, const ProcessRef& theProc, size_t streamIndex)
{
	doGetData(inputBuffer, theProc, streamIndex);
}

namespace
{
	struct ProcessOutputState
	{
		bool inIsOpen;
		bool outIsOpen;
		bool errIsOpen;
		size_t availableDataLen;

		ProcessOutputState()
			: inIsOpen(true)
			, outIsOpen(true)
			, errIsOpen(true)
			, availableDataLen(0)
		{
		}
	};

}

/////////////////////////////////////////////////////////////////////////////
void
processInputOutput(OutputCallback& output, Array<ProcessRef>& procs, InputCallback& input, const Timeout& timeout)
{
	TimeoutTimer timer(timeout);

	Array<ProcessOutputState> processStates(procs.size());
	int numOpenPipes(procs.size() * 2); // count of stdout & stderr. Ignore stdin for purposes of algorithm termination.

	Array<Array<char> > inputs(processStates.size());
	for (size_t i = 0; i < processStates.size(); ++i)
	{
		input.getData(inputs[i], procs[i], i);
		processStates[i].availableDataLen = inputs[i].size();
		if (!procs[i]->out()->isOpen())
		{
			processStates[i].outIsOpen = false;
		}
		if (!procs[i]->err()->isOpen())
		{
			processStates[i].errIsOpen = false;
		}
		if (!procs[i]->in()->isOpen())
		{
			processStates[i].inIsOpen = false;
		}

	}

	timer.start();

	while (numOpenPipes > 0)
	{
		Select::SelectObjectArray selObjs; 
		std::map<int, int> inputIndexProcessIndex;
		std::map<int, int> outputIndexProcessIndex;
		for (size_t i = 0; i < procs.size(); ++i)
		{
			if (processStates[i].outIsOpen)
			{
				Select::SelectObject selObj(procs[i]->out()->getReadSelectObj()); 
				selObj.waitForRead = true; 
				selObjs.push_back(selObj); 
				inputIndexProcessIndex[selObjs.size() - 1] = i;
			}
			if (processStates[i].errIsOpen)
			{
				Select::SelectObject selObj(procs[i]->err()->getReadSelectObj()); 
				selObj.waitForRead = true; 
				selObjs.push_back(selObj); 
				inputIndexProcessIndex[selObjs.size() - 1] = i;
			}
			if (processStates[i].inIsOpen && processStates[i].availableDataLen > 0)
			{
				Select::SelectObject selObj(procs[i]->in()->getWriteSelectObj()); 
				selObj.waitForWrite = true; 
				selObjs.push_back(selObj); 
				outputIndexProcessIndex[selObjs.size() - 1] = i;
			}

		}

		int selectrval = Select::selectRW(selObjs, timer.asRelativeTimeout(0.1));
		switch (selectrval)
		{
			case Select::SELECT_ERROR:
			{
				OW_THROW_ERRNO_MSG(ExecErrorException, "Exec::gatherOutput: error selecting on stdout and stderr");
			}
			break;
			case Select::SELECT_TIMEOUT:
			{
				// Check all processes and see if they've exited but the pipes are still open. If so, close the pipes,
				// since there's nothing to read from them.
				for (size_t i = 0; i < procs.size(); ++i)
				{
					if (procs[i]->processStatus().terminated())
					{
						if (processStates[i].inIsOpen)
						{
							processStates[i].inIsOpen = false;
							procs[i]->in()->close();
						}
						if (processStates[i].outIsOpen)
						{
							processStates[i].outIsOpen = false;
							procs[i]->out()->close();
							--numOpenPipes;
						}
						if (processStates[i].errIsOpen)
						{
							processStates[i].errIsOpen = false;
							procs[i]->err()->close();
							--numOpenPipes;
						}
					}
				}

				timer.loop();
				if (timer.expired())
				{
					OW_THROW(ExecTimeoutException, "Exec::gatherOutput: timedout");
				}
			}
			break;
			default:
			{
				int availableToFind = selectrval;
				
				// reset the timeout counter
				timer.resetOnLoop();

				for (size_t i = 0; i < selObjs.size() && availableToFind > 0; ++i)
				{
					if (!selObjs[i].readAvailable)
					{
						continue;
					}
					else
					{
						--availableToFind;
					}
					int streamIndex = inputIndexProcessIndex[i];
					UnnamedPipeRef readstream;
					if (processStates[streamIndex].outIsOpen)
					{
						if (procs[streamIndex]->out()->getReadSelectObj() == selObjs[i].s)
						{
							readstream = procs[streamIndex]->out();
						}
					}

					if (!readstream && processStates[streamIndex].errIsOpen)
					{
						if (procs[streamIndex]->err()->getReadSelectObj() == selObjs[i].s)
						{
							readstream = procs[streamIndex]->err();
						}
					}

					if (!readstream)
					{
						continue; // for loop
					}

					char buff[1024];
					int readrc = readstream->read(buff, sizeof(buff) - 1);
					if (readrc == 0)
					{
						if (readstream == procs[streamIndex]->out())
						{
							processStates[streamIndex].outIsOpen = false;
							procs[streamIndex]->out()->close();
						}
						else
						{
							processStates[streamIndex].errIsOpen = false;
							procs[streamIndex]->err()->close();
						}
						--numOpenPipes;
					}
					else if (readrc == -1)
					{
						OW_THROW_ERRNO_MSG(ExecErrorException, "Exec::gatherOutput: read error");
					}
					else
					{
						buff[readrc] = '\0';
						output.handleData(buff, readrc, readstream == procs[streamIndex]->out() ? E_STDOUT : E_STDERR, procs[streamIndex],
							streamIndex, inputs[streamIndex]);
					}
				}

				// handle stdin for all processes which have data to send to them.
				for (size_t i = 0; i < selObjs.size() && availableToFind > 0; ++i)
				{
					if (!selObjs[i].writeAvailable)
					{
						continue;
					}
					else
					{
						--availableToFind;
					}
					int streamIndex = outputIndexProcessIndex[i];
					UnnamedPipeRef writestream;
					if (processStates[streamIndex].inIsOpen)
					{
						writestream = procs[streamIndex]->in();
					}

					if (!writestream)
					{
						continue; // for loop
					}

					size_t offset = inputs[streamIndex].size() - processStates[streamIndex].availableDataLen;
					int writerc = writestream->write(&inputs[streamIndex][offset], processStates[streamIndex].availableDataLen);
					if (writerc == 0)
					{
						processStates[streamIndex].inIsOpen = false;
						procs[streamIndex]->in()->close();
					}
					else if (writerc == -1)
					{
						OW_THROW_ERRNO_MSG(ExecErrorException, "Exec::gatherOutput: write error");
					}
					else
					{
						inputs[streamIndex].erase(inputs[streamIndex].begin(), inputs[streamIndex].begin() + writerc);
						input.getData(inputs[streamIndex], procs[streamIndex], streamIndex);
						processStates[streamIndex].availableDataLen = inputs[streamIndex].size();
					}
				}
			}
			break;
		}
	}
}

void processInputOutput(const String& input, String& output, const ProcessRef& process, 
	const Timeout& timeout, int outputLimit)
{
	Array<ProcessRef> procs;
	procs.push_back(process);

	StringOutputGatherer gatherer(output, outputLimit);
	SingleStringInputCallback singleStringInputCallback = SingleStringInputCallback(input);
	processInputOutput(gatherer, procs, singleStringInputCallback, timeout);
}


} // end namespace Exec

} // end namespace OW_NAMESPACE

