/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
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

#include <map>

extern "C"
{
#ifdef OW_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifndef OW_WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif
#include <errno.h>
#include <stdio.h> // for perror
#include <signal.h>
}

#include <cerrno>
#include <iostream>	// for cerr


namespace OW_NAMESPACE
{

using std::cerr;
using std::endl;
OW_DEFINE_EXCEPTION_WITH_ID(ExecTimeout);
OW_DEFINE_EXCEPTION_WITH_ID(ExecBufferFull);
OW_DEFINE_EXCEPTION_WITH_ID(ExecError);

#ifndef OW_WIN32
class PopenStreamsImpl : public IntrusiveCountableBase
{
public:
	PopenStreamsImpl();
	~PopenStreamsImpl();
	UnnamedPipeRef in();
	void in(const UnnamedPipeRef& pipe);
	UnnamedPipeRef out();
	void out(const UnnamedPipeRef& pipe);
	UnnamedPipeRef err();
	void err(const UnnamedPipeRef& pipe);
	pid_t pid();
	void pid(pid_t newPid);
	int getExitStatus();
	void setProcessStatus(int ps)
	{
		m_processstatus = ps;
	}
private:
	UnnamedPipeRef m_in;
	UnnamedPipeRef m_out;
	UnnamedPipeRef m_err;
	pid_t m_pid;
	int m_processstatus;
};
//////////////////////////////////////////////////////////////////////////////
PopenStreamsImpl::PopenStreamsImpl()
	: m_pid(-1)
	, m_processstatus(-1)
{
}
//////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreamsImpl::in()
{
	return m_in;
}
//////////////////////////////////////////////////////////////////////////////
void PopenStreamsImpl::in(const UnnamedPipeRef& pipe)
{
	m_in = pipe;
}
//////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreamsImpl::out()
{
	return m_out;
}
//////////////////////////////////////////////////////////////////////////////
void PopenStreamsImpl::out(const UnnamedPipeRef& pipe)
{
	m_out = pipe;
}
//////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreamsImpl::err()
{
	return m_err;
}
//////////////////////////////////////////////////////////////////////////////
void PopenStreamsImpl::err(const UnnamedPipeRef& pipe)
{
	m_err = pipe;
}
//////////////////////////////////////////////////////////////////////////////
pid_t PopenStreamsImpl::pid()
{
	return m_pid;
}
//////////////////////////////////////////////////////////////////////////////
void PopenStreamsImpl::pid(pid_t newPid)
{
	m_pid = newPid;
}
//////////////////////////////////////////////////////////////////////
static inline pid_t lwaitpid(pid_t pid, int* status, int options)
{
	// The status is not passed directly to waitpid because some implementations
	// store a value there even when the function returns <= 0.
	int localReturnValue = -1;
	pid_t returnedPID = ::waitpid(pid, &localReturnValue, options);
	if( returnedPID > 0 )
	{
		*status = localReturnValue;
	}	
	return returnedPID;
}
//////////////////////////////////////////////////////////////////////
static pid_t
waitpidNoINTR(pid_t pid, int* status, int options)
{
	pid_t waitpidrv;
	do
	{
		Thread::testCancel();
		waitpidrv = lwaitpid(pid, status, options);
	} while (waitpidrv == -1 && errno == EINTR);
	return waitpidrv;
}
//////////////////////////////////////////////////////////////////////////////
static inline void
milliSleep(UInt32 milliSeconds)
{
	Thread::sleep(milliSeconds);
}
//////////////////////////////////////////////////////////////////////////////
static inline void
secSleep(UInt32 seconds)
{
	Thread::sleep(seconds * 1000);
}
//////////////////////////////////////////////////////////////////////////////
int PopenStreamsImpl::getExitStatus()
{
	// Close the streams. If the child process is blocked waiting to output,
	// then this will cause it to get a SIGPIPE, and it may be able to clean
	// up after itself.
	UnnamedPipeRef upr;
	if (upr = in())
	{
		upr->close();
	}
	if (upr = out())
	{
		upr->close();
	}
	if (upr = err())
	{
		upr->close();
	}
	// Now make sure the process has exited. We do everything possible to make
	// sure the sub-process dies.
	if (m_pid > 0 && m_pid != ::getpid()) // it's set to -1 if we already sucessfully waited for it.
	{
		pid_t waitpidrv;
		
		// give it up to 10 seconds to quit
		waitpidrv = waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
		for (int i = 0; i < 100 && waitpidrv == 0; ++i)
		{
			milliSleep(100); // 1/10 of a second
			waitpidrv = waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
		}
		if (waitpidrv == 0)
		{
			if (kill(m_pid, SIGTERM) != -1)
			{
				// give it up to 10 seconds to quit
				waitpidrv = waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
				for (int i = 0; i < 100 && waitpidrv == 0; ++i)
				{
					milliSleep(100); // 1/10 of a second
					waitpidrv = waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
				}
	
				if (waitpidrv == 0)
				{
					/* process still didn't terminate after a SIGTERM, so we'll
					   try sending it SIGKILL */
					if (kill(m_pid, SIGKILL) == -1)
					{
						// call waitpid in case the thing has turned into a zombie, which would cause kill() to fail.
						waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
						OW_THROW_ERRNO_MSG(ExecErrorException, Format("PopenStreamsImpl::getExitStatus: Failed sending SIGKILL to process %1", m_pid).c_str());
					}
					// give the kernel 1 sec to clean it up, otherwise we bail.
					waitpidrv = waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
					for (int i = 0; i < 100 && waitpidrv == 0; ++i)
					{
						milliSleep(10); // 1/100 of a second
						waitpidrv = waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
					}
					if (waitpidrv == 0)
					{
						OW_THROW_ERRNO_MSG(ExecErrorException, "PopenStreamsImpl::getExitStatus: Child process has not exited after sending it a SIGKILL.");
					}
				}
				else if (waitpidrv > 0)
				{
					m_pid = -1;
				}
				else
				{
					OW_THROW_ERRNO_MSG(ExecErrorException, "PopenStreamsImpl::getExitStatus: second waitpid() failed.");
				}
			}
			else
			{
				// call waitpid in case the thing has turned into a zombie, which would cause kill() to fail.
				waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
				OW_THROW_ERRNO_MSG(ExecErrorException, Format("PopenStreamsImpl::getExitStatus: Failed sending SIGTERM to process %1.", m_pid).c_str());
			}
		}
		else if (waitpidrv > 0)
		{
			m_pid = -1;
		}
		else
		{
			OW_THROW_ERRNO_MSG(ExecErrorException, "PopenStreamsImpl::getExitStatus: first waitpid() failed.");
		}
	}
	return m_processstatus;
}
//////////////////////////////////////////////////////////////////////////////
PopenStreamsImpl::~PopenStreamsImpl()
{
	try // can't let exceptions past.
	{
		// This will terminate the process.
		getExitStatus();
	}
	catch (...)
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
PopenStreams::PopenStreams()
	: m_impl(new PopenStreamsImpl)
{
}
/////////////////////////////////////////////////////////////////////////////
PopenStreams::~PopenStreams()
{
}
/////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreams::in() const
{
	return m_impl->in();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::in(const UnnamedPipeRef& pipe)
{
	m_impl->in(pipe);
}
/////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreams::out() const
{
	return m_impl->out();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::out(const UnnamedPipeRef& pipe)
{
	m_impl->out(pipe);
}
/////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreams::err() const
{
	return m_impl->err();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::err(const UnnamedPipeRef& pipe)
{
	m_impl->err(pipe);
}
/////////////////////////////////////////////////////////////////////////////
pid_t PopenStreams::pid() const
{
	return m_impl->pid();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::pid(pid_t newPid)
{
	m_impl->pid(newPid);
}
/////////////////////////////////////////////////////////////////////////////
int PopenStreams::getExitStatus()
{
	return m_impl->getExitStatus();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::setProcessStatus(int ps)
{
	m_impl->setProcessStatus(ps);
}
/////////////////////////////////////////////////////////////////////////////
PopenStreams::PopenStreams(const PopenStreams& src)
	: m_impl(src.m_impl)
{
}
/////////////////////////////////////////////////////////////////////////////
PopenStreams& PopenStreams::operator=(const PopenStreams& src)
{
	m_impl = src.m_impl;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
bool operator==(const PopenStreams& x, const PopenStreams& y)
{
	return x.m_impl == y.m_impl;
}

//////////////////////////////////////////////////////////////////////////////
namespace Exec
{

int
safeSystem(const Array<String>& command)
{
	int status;
	pid_t pid;
	if (command.size() == 0)
	{
		return 1;
	}
	pid = fork();
	if (pid == -1)
	{
		return -1;
	}
	if (pid == 0)
	{
		// Close all file handle from parent process
		rlimit rl;
		int i = sysconf(_SC_OPEN_MAX);
		if (getrlimit(RLIMIT_NOFILE, &rl) != -1)
		{
		  if ( i < 0 )
		  {
		    i = rl.rlim_max;
		  }
		  else
		  {
		    i = std::min<int>(rl.rlim_max, i);
		  }
		}
		while (i > 2)
		{
			close(i);
			i--;
		}
		char** argv = new char*[command.size() + 1];
		for (size_t i = 0; i < command.size(); i++)
		{
			argv[i] = strdup(command[i].c_str());
		}
		argv[command.size()] = 0;
		int rval = execv(argv[0], argv);
		cerr << Format( "Exec::safeSystem: execv failed for program "
				"%1, rval is %2", argv[0], rval);
		_exit(1);
	}
	do
	{
		Thread::testCancel();
		if (waitpid(pid, &status, 0) == -1)
		{
			if (errno != EINTR)
			{
				return -1;
			}
		}
		else
		{
			return WEXITSTATUS(status);
		}
	} while (1);
}

//////////////////////////////////////////////////////////////////////////////
PopenStreams
safePopen(const Array<String>& command,
		const String& initialInput)
{
	PopenStreams retval = safePopen(command);

	if (initialInput != "")
	{
		if (retval.in()->write(initialInput.c_str(), initialInput.length()) == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, "Exec::safePopen: Failed writing input to process");
		}
	}

	return retval;
}

//////////////////////////////////////////////////////////////////////////////
PopenStreams
safePopen(const Array<String>& command, const char* const envp[])
{
	PopenStreams retval;
	retval.in( UnnamedPipe::createUnnamedPipe() );
	UnnamedPipeRef upipeOut = UnnamedPipe::createUnnamedPipe();
	retval.out( upipeOut );
	UnnamedPipeRef upipeErr = UnnamedPipe::createUnnamedPipe();
	retval.err( upipeErr );
	if (command.size() == 0)
	{
		OW_THROW(ExecErrorException, "Exec::safePopen: command is empty");
	}
	retval.pid ( fork() );
	if (retval.pid() == -1)
	{
		OW_THROW_ERRNO_MSG(ExecErrorException, "Exec::safePopen: fork() failed");
	}
	if (retval.pid() == 0)
	{
		// Close stdin, stdout, and stderr.
		close(0);
		close(1);
		close(2);
		// this should only fail because of programmer error.
		UnnamedPipeRef foo1 = retval.in();
		PosixUnnamedPipeRef in = foo1.cast_to<PosixUnnamedPipe>();

		UnnamedPipeRef foo2 = retval.out();
		PosixUnnamedPipeRef out = foo2.cast_to<PosixUnnamedPipe>();

		UnnamedPipeRef foo3 = retval.err();
		PosixUnnamedPipeRef err = foo3.cast_to<PosixUnnamedPipe>();
		
		OW_ASSERT(in);
		OW_ASSERT(out);
		OW_ASSERT(err);
		// connect stdin, stdout, and stderr to the return pipes.
		dup2(in->getInputHandle(), 0);
		dup2(out->getOutputHandle(), 1);
		dup2(err->getOutputHandle(), 2);
		// Close all other file handle from parent process
		rlimit rl;
		int i = sysconf(_SC_OPEN_MAX);
		if (getrlimit(RLIMIT_NOFILE, &rl) != -1)
		{
			if ( i < 0 )
			{
				i = rl.rlim_max;
			}
			else
			{
				i = std::min<int>(rl.rlim_max, i);
			}
		}
		while (i > 2)
		{
			close(i);
			i--;
		}

		// according to susv3:
		//        This  volume  of  IEEE Std 1003.1-2001  specifies  that  signals set to
		//        SIG_IGN remain set to SIG_IGN, and that  the  process  signal  mask  be
		//        unchanged  across an exec. This is consistent with historical implemen-
		//        tations, and it permits some useful functionality, such  as  the  nohup
		//        command.  However,  it  should be noted that many existing applications
		//        wrongly assume that they start with certain signals set to the  default
		//        action  and/or  unblocked.  In  particular, applications written with a
		//        simpler signal model that does not include blocking of signals, such as
		//        the  one in the ISO C standard, may not behave properly if invoked with
		//        some signals blocked. Therefore, it is best not to block or ignore sig-
		//        nals  across execs without explicit reason to do so, and especially not
		//        to block signals across execs of arbitrary (not  closely  co-operating)
		//        programs.

		// so we'll reset the signal mask and all SIG_IGN signal handlers to SIG_DFL
		sigset_t emptymask;
		sigemptyset(&emptymask);
		::sigprocmask(SIG_SETMASK, &emptymask, 0);

// NSIG may be defined by signal.h, otherwise 64 should be plenty.
#ifndef NSIG
#define NSIG 64
#endif
		for (size_t sig = 1; sig <= NSIG; ++sig)
		{
			struct sigaction temp;
			sigaction(sig, 0, &temp);
			if (temp.sa_handler == SIG_IGN)
			{
				temp.sa_handler = SIG_DFL;
				sigaction(sig, &temp, NULL);
			}
		}


		char** argv = new char*[command.size() + 1];
		for (size_t i = 0; i < command.size(); i++)
		{
			argv[i] = strdup(command[i].c_str());
		}
		argv[command.size()] = 0;
		int rval = 0;
		if (envp)
		{
			rval = execve(argv[0], argv, const_cast<char* const*>(envp));
		}
		else
		{
			rval = execv(argv[0], argv);
		}
		cerr << Format( "Exec::safePopen: execv failed for program "
				"%1, rval is %2", argv[0], rval);
		_exit(127);
	}
	// this should only fail because of programmer error.
	UnnamedPipeRef foo1 = retval.in();
	PosixUnnamedPipeRef in = foo1.cast_to<PosixUnnamedPipe>();
	UnnamedPipeRef foo2 = retval.out();	
	PosixUnnamedPipeRef out = foo2.cast_to<PosixUnnamedPipe>();
	UnnamedPipeRef foo3 = retval.err();	
	PosixUnnamedPipeRef err = foo3.cast_to<PosixUnnamedPipe>();
	OW_ASSERT(in);
	OW_ASSERT(out);
	OW_ASSERT(err);
	// prevent the parent from using the child's end of the pipes.
	in->closeInputHandle();
	out->closeOutputHandle();
	err->closeOutputHandle();

	return retval;
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
	virtual void doHandleData(const char* data, size_t dataLen, EOutputSource outputSource, PopenStreams& theStream, size_t streamIndex, Array<char>& inputBuffer)
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
	virtual void doGetData(Array<char>& inputBuffer, PopenStreams& theStream, size_t streamIndex)
	{
		if (m_s.length() > 0)
		{
			inputBuffer.insert(inputBuffer.end(), m_s.c_str(), m_s.c_str() + m_s.length());
			m_s.erase();
		}
		else if (theStream.in()->isOpen())
		{
			theStream.in()->close();
		}
	}
	String m_s;
};

}// end anonymous namespace

/////////////////////////////////////////////////////////////////////////////
void
executeProcessAndGatherOutput(const Array<String>& command,
	String& output, int& processStatus,
	int timeoutSecs, int outputLimit, const String& input)
{
	processStatus = -1;
	Array<PopenStreams> streams;
	streams.push_back(safePopen(command));
	Array<ProcessStatus> processStatuses(1);
	SingleStringInputCallback singleStringInputCallback(input);

	StringOutputGatherer gatherer(output, outputLimit);
	processInputOutput(gatherer, streams, processStatuses, singleStringInputCallback, timeoutSecs);

	if (processStatuses[0].hasExited())
	{
		processStatus = processStatuses[0].getStatus();
	}
	else
	{
		processStatus = streams[0].getExitStatus();
	}
}

/////////////////////////////////////////////////////////////////////////////
void
gatherOutput(String& output, PopenStreams& stream, int& processStatus, int timeoutSecs, int outputLimit)
{
	Array<PopenStreams> streams;
	streams.push_back(stream);
	Array<ProcessStatus> processStatuses(1);

	StringOutputGatherer gatherer(output, outputLimit);
	SingleStringInputCallback singleStringInputCallback = SingleStringInputCallback(String());
	processInputOutput(gatherer, streams, processStatuses, singleStringInputCallback, timeoutSecs);
	if (processStatuses[0].hasExited())
	{
		processStatus = processStatuses[0].getStatus();
	}
}

/////////////////////////////////////////////////////////////////////////////
OutputCallback::~OutputCallback()
{

}

/////////////////////////////////////////////////////////////////////////////
void
OutputCallback::handleData(const char* data, size_t dataLen, EOutputSource outputSource, PopenStreams& theStream, size_t streamIndex, Array<char>& inputBuffer)
{
	doHandleData(data, dataLen, outputSource, theStream, streamIndex, inputBuffer);
}

/////////////////////////////////////////////////////////////////////////////
InputCallback::~InputCallback()
{
}

/////////////////////////////////////////////////////////////////////////////
void
InputCallback::getData(Array<char>& inputBuffer, PopenStreams& theStream, size_t streamIndex)
{
	doGetData(inputBuffer, theStream, streamIndex);
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
processInputOutput(OutputCallback& output, Array<PopenStreams>& streams, Array<ProcessStatus>& processStatuses, InputCallback& input, int timeoutsecs)
{
	processStatuses.clear();
	processStatuses.resize(streams.size());

	Array<ProcessOutputState> processStates(streams.size());
	int numOpenPipes(streams.size() * 2); // count of stdout & stderr. Ignore stdin for purposes of algorithm termination.

	DateTime curTime;
	curTime.setToCurrent();
	DateTime timeoutEnd(curTime);
	timeoutEnd += timeoutsecs;

	Array<Array<char> > inputs(processStates.size());
	for (size_t i = 0; i < processStates.size(); ++i)
	{
		input.getData(inputs[i], streams[i], i);
		processStates[i].availableDataLen = inputs[i].size();
		if (!streams[i].out()->isOpen())
		{
			processStates[i].outIsOpen = false;
		}
		if (!streams[i].err()->isOpen())
		{
			processStates[i].errIsOpen = false;
		}
		if (!streams[i].in()->isOpen())
		{
			processStates[i].inIsOpen = false;
		}

	}

	while (numOpenPipes > 0)
	{
		Select::SelectObjectArray inputSelObjs;
		std::map<int, int> inputIndexProcessIndex;
		Select::SelectObjectArray outputSelObjs;
		std::map<int, int> outputIndexProcessIndex;
		for (size_t i = 0; i < streams.size(); ++i)
		{
			if (processStates[i].outIsOpen)
			{
				inputSelObjs.push_back(streams[i].out()->getSelectObj());
				inputIndexProcessIndex[inputSelObjs.size() - 1] = i;
			}
			if (processStates[i].errIsOpen)
			{
				inputSelObjs.push_back(streams[i].err()->getSelectObj());
				inputIndexProcessIndex[inputSelObjs.size() - 1] = i;
			}
			if (processStates[i].inIsOpen && processStates[i].availableDataLen > 0)
			{
				outputSelObjs.push_back(streams[i].in()->getWriteSelectObj());
				outputIndexProcessIndex[outputSelObjs.size() - 1] = i;
			}

			// check if the child has exited - the pid gets set to -1 once it's exited.
			if (streams[i].pid() != -1)
			{
				pid_t waitpidrv;
				int processStatus(-1);
				waitpidrv = waitpidNoINTR(streams[i].pid(), &processStatus, WNOHANG);
				if (waitpidrv == -1)
				{
					OW_THROW_ERRNO_MSG(ExecErrorException, "Exec::gatherOutput: waitpid() failed");
				}
				else if (waitpidrv != 0)
				{
					streams[i].pid(-1);
					streams[i].setProcessStatus(processStatus);
					processStatuses[i] = ProcessStatus(processStatus);
				}
			}
		}

		const int mstimeout = 100; // use 1/10 of a second
		int selectrval = Select::selectRW(inputSelObjs, outputSelObjs, mstimeout);
		switch (selectrval)
		{
			case Select::SELECT_INTERRUPTED:
				// if we got interrupted, just try again
				break;
			case Select::SELECT_ERROR:
			{
				OW_THROW_ERRNO_MSG(ExecErrorException, "Exec::gatherOutput: error selecting on stdout and stderr");
			}
			break;
			case Select::SELECT_TIMEOUT:
			{
				// Check all processes and see if they've exited but the pipes are still open. If so, close the pipes,
				// since there's nothing to read from them.
				for (size_t i = 0; i < streams.size(); ++i)
				{
					if (streams[i].pid() == -1)
					{
						if (processStates[i].inIsOpen)
						{
							processStates[i].inIsOpen = false;
							streams[i].in()->close();
						}
						if (processStates[i].outIsOpen)
						{
							processStates[i].outIsOpen = false;
							streams[i].out()->close();
							--numOpenPipes;
						}
						if (processStates[i].errIsOpen)
						{
							processStates[i].errIsOpen = false;
							streams[i].err()->close();
							--numOpenPipes;
						}
					}
				}

				curTime.setToCurrent();
				if (timeoutsecs >= 0 && curTime > timeoutEnd)
				{
					OW_THROW(ExecTimeoutException, "Exec::gatherOutput: timedout");
				}
			}
			break;
			default:
			{
				int availableToFind = selectrval;
				// reset the timeout counter
				curTime.setToCurrent();
				timeoutEnd = curTime;
				timeoutEnd += timeoutsecs;

				for (size_t i = 0; i < inputSelObjs.size() && availableToFind > 0; ++i)
				{
					if (!inputSelObjs[i].available)
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
						if (streams[streamIndex].out()->getSelectObj() == inputSelObjs[i].s)
						{
							readstream = streams[streamIndex].out();
						}
					}

					if (!readstream && processStates[streamIndex].errIsOpen)
					{
						if (streams[streamIndex].err()->getSelectObj() == inputSelObjs[i].s)
						{
							readstream = streams[streamIndex].err();
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
						if (readstream == streams[streamIndex].out())
						{
							processStates[streamIndex].outIsOpen = false;
							streams[streamIndex].out()->close();
						}
						else
						{
							processStates[streamIndex].errIsOpen = false;
							streams[streamIndex].err()->close();
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
						output.handleData(buff, readrc, readstream == streams[streamIndex].out() ? E_STDOUT : E_STDERR, streams[streamIndex],
							streamIndex, inputs[streamIndex]);
					}
				}

				// handle stdin for all processes which have data to send to them.
				for (size_t i = 0; i < outputSelObjs.size() && availableToFind > 0; ++i)
				{
					if (!outputSelObjs[i].available)
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
						writestream = streams[streamIndex].in();
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
						streams[streamIndex].in()->close();
					}
					else if (writerc == -1)
					{
						OW_THROW_ERRNO_MSG(ExecErrorException, "Exec::gatherOutput: write error");
					}
					else
					{
						inputs[streamIndex].erase(inputs[streamIndex].begin(), inputs[streamIndex].begin() + writerc);
						input.getData(inputs[streamIndex], streams[streamIndex], streamIndex);
						processStates[streamIndex].availableDataLen = inputs[streamIndex].size();
					}
				}
			}
			break;
		}
	}
}


} // end namespace Exec
#endif
} // end namespace OW_NAMESPACE

