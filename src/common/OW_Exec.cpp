/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_Exec.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_Array.hpp"
#include "OW_IOException.hpp"
#include "OW_Thread.hpp"
#include "OW_Select.hpp"
extern "C"
{
#ifdef OW_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h> // for perror
#include <signal.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}
#include <iostream>	// for cerr

namespace OpenWBEM
{

using std::cerr;
using std::endl;
OW_DEFINE_EXCEPTION(ExecTimeout);
OW_DEFINE_EXCEPTION(ExecBufferFull);
OW_DEFINE_EXCEPTION(ExecError);

class PopenStreamsImpl
{
public:
	PopenStreamsImpl();
	~PopenStreamsImpl();
	UnnamedPipeRef in();
	void in(UnnamedPipeRef pipe);
	UnnamedPipeRef out();
	void out(UnnamedPipeRef pipe);
	UnnamedPipeRef err();
	void err(UnnamedPipeRef pipe);
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
	: m_pid(0)
	, m_processstatus(-1)
{
}
//////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreamsImpl::in()
{
	return m_in;
}
//////////////////////////////////////////////////////////////////////////////
void PopenStreamsImpl::in(UnnamedPipeRef pipe)
{
	m_in = pipe;
}
//////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreamsImpl::out()
{
	return m_out;
}
//////////////////////////////////////////////////////////////////////////////
void PopenStreamsImpl::out(UnnamedPipeRef pipe)
{
	m_out = pipe;
}
//////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreamsImpl::err()
{
	return m_err;
}
//////////////////////////////////////////////////////////////////////////////
void PopenStreamsImpl::err(UnnamedPipeRef pipe)
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
#ifdef OW_USE_PTH
	return pth_waitpid(pid, status, options);
#else
	return ::waitpid(pid, status, options);
#endif
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
	in()->close();
	out()->close();
	err()->close();
	// Now make sure the process has exited. We do everything possible to make 
	// sure the sub-process dies.
	if (m_pid != -1) // it's set to -1 if we already sucessfully waited for it.
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
						OW_THROW(ExecErrorException, format("Failed sending SIGKILL to process %1. errno = %2(%3)\n", m_pid, errno, strerror(errno)).c_str());
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
						OW_THROW(ExecErrorException, format("Child process has not exited after sending it a SIGKILL. errno = %1(%2)\n", errno, strerror(errno)).c_str());
					}
				}
				else if (waitpidrv > 0)
				{
					m_pid = -1;
				}
				else
				{
					OW_THROW(ExecErrorException, format("PopenStreamsImpl::getExitStatus: 1- waitpid failed.  errno = %1(%2)\n", errno, strerror(errno)).c_str());
				}
			}
			else
			{
				// call waitpid in case the thing has turned into a zombie, which would cause kill() to fail.
				waitpidNoINTR(m_pid, &m_processstatus, WNOHANG);
				OW_THROW(ExecErrorException, format("Failed sending SIGTERM to process %1. errno = %2(%3)\n", m_pid, errno, strerror(errno)).c_str());
			}
		}
		else if (waitpidrv > 0)
		{
			m_pid = -1;
		}
		else
		{
			OW_THROW(ExecErrorException, format("PopenStreamsImpl::getExitStatus: 2- waitpid failed.  errno = %1(%2)\n", errno, strerror(errno)).c_str());
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
Reference<UnnamedPipe> PopenStreams::in() const
{
	return m_impl->in();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::in(UnnamedPipeRef pipe)
{
	m_impl->in(pipe);
}
/////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreams::out() const
{
	return m_impl->out();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::out(UnnamedPipeRef pipe)
{
	m_impl->out(pipe);
}
/////////////////////////////////////////////////////////////////////////////
UnnamedPipeRef PopenStreams::err() const
{
	return m_impl->err();
}
/////////////////////////////////////////////////////////////////////////////
void PopenStreams::err(UnnamedPipeRef pipe)
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
namespace Exec
{

int
safeSystem(const Array<String>& command)
{
	int status;
	pid_t pid;
	if (command.size() == 0)
		return 1;
#ifdef OW_USE_GNU_PTH
	pid = pth_fork();
#else
	pid = fork();
#endif
	if (pid == -1)
		return -1;
	if (pid == 0)
	{
		// Close all file handle from parent process
		rlimit rl;
		int i = sysconf(_SC_OPEN_MAX);
		if (getrlimit(RLIMIT_NOFILE, &rl) != -1)
		{
		  if( i < 0 )
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
		cerr << format( "Platform::safeSystem: execv failed for program "
				"%1, rval is %2", argv[0], rval);
		_exit(1);
	}
	do
	{
		Thread::testCancel();
#ifdef OW_USE_GNUPTH
		if (pth_waitpid(pid, &status, 0) == -1)
#else
		if (waitpid(pid, &status, 0) == -1)
#endif
		{
			if (errno != EINTR)
				return -1;
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
	PopenStreams retval;
	retval.in( UnnamedPipe::createUnnamedPipe() );
	UnnamedPipeRef upipeOut = UnnamedPipe::createUnnamedPipe();
	retval.out( upipeOut );
	UnnamedPipeRef upipeErr = UnnamedPipe::createUnnamedPipe();
	retval.err( upipeErr );
	if (initialInput != "")
	{
		if (retval.in()->write(initialInput.c_str(), initialInput.length()) == -1)
		{
			OW_THROW(IOException, "Failed writing input to process");
		}
	}
	if (command.size() == 0)
	{
		OW_THROW(ExecErrorException, "command is empty");
	}
#ifdef OW_USE_GNU_PTH
	retval.pid ( pth_fork() );
#else
	retval.pid ( fork() );
#endif
	if (retval.pid() == -1)
		OW_THROW(ExecErrorException, "fork() failed");
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
		
		OW_ASSERT(in); OW_ASSERT(out); OW_ASSERT(err);
		// connect stdin, stdout, and stderr to the return pipes.
		dup2(in->getInputHandle(), 0);
		dup2(out->getOutputHandle(), 1);
		dup2(err->getOutputHandle(), 2);
		// Close all other file handle from parent process
		rlimit rl;
		int i = sysconf(_SC_OPEN_MAX);
		if (getrlimit(RLIMIT_NOFILE, &rl) != -1)
		{
		  if( i < 0 )
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
		cerr << format( "Platform::safePopen: execv failed for program "
				"%1, rval is %2", argv[0], rval);
		_exit(1);
	}
	// this should only fail because of programmer error.
	UnnamedPipeRef foo1 = retval.in();
	PosixUnnamedPipeRef in = foo1.cast_to<PosixUnnamedPipe>();
	UnnamedPipeRef foo2 = retval.out();	
	PosixUnnamedPipeRef out = foo2.cast_to<PosixUnnamedPipe>();
	UnnamedPipeRef foo3 = retval.err();	
	PosixUnnamedPipeRef err = foo3.cast_to<PosixUnnamedPipe>();
	OW_ASSERT(in); OW_ASSERT(out); OW_ASSERT(err);
	// prevent the parent from using the child's end of the pipes.
	in->closeInputHandle();
	out->closeOutputHandle();
	err->closeOutputHandle();
	return retval;
}

/////////////////////////////////////////////////////////////////////////////
void 
executeProcessAndGatherOutput(const Array<String>& command,
	String& output, int& processstatus,
	int timeoutsecs, int outputlimit)
{
	processstatus = -1;
	PopenStreams streams(safePopen(command));
	gatherOutput(output, streams, processstatus,
		timeoutsecs, outputlimit);
	if (processstatus == -1)
	{
		processstatus = streams.getExitStatus();
	}
}

/////////////////////////////////////////////////////////////////////////////
void 
gatherOutput(String& output, PopenStreams& streams, int& processstatus, int timeoutsecs, int outputlimit)
{
	bool outIsOpen = true;
	bool errIsOpen = true;
	bool got_child_return_code = false;
	int cumulative_timeout = 0;
	while(outIsOpen || errIsOpen)
	{
		SelectTypeArray fdset;
		if (outIsOpen)
		{
			fdset.push_back(streams.out()->getSelectObj());
		}
		if (errIsOpen)
		{
			fdset.push_back(streams.err()->getSelectObj());
		}
		
		// check if the child has exited
		if (!got_child_return_code && streams.pid() != -1)
		{
			pid_t waitpidrv;
			waitpidrv = waitpidNoINTR(streams.pid(), &processstatus, WNOHANG);
			if (waitpidrv == -1)
			{
				OW_THROW(ExecErrorException, format("Exec::gatherOutput: waitpid failed errno = %1(%2)\n", errno, strerror(errno)).c_str());
			}
			else if (waitpidrv != 0)
			{
				got_child_return_code = 1;
				streams.pid(-1);
				streams.setProcessStatus(processstatus);
			}
		}
		const int mstimeout = 100; // use 1/10 of a second
		int selectrval = Select::select(fdset, mstimeout);
		switch (selectrval)
		{
			case Select::SELECT_INTERRUPTED:
				// if we got interrupted, just try again
				break;
			case Select::SELECT_ERROR:
			{
				OW_THROW(ExecErrorException, format("error selecting on stdout and stderr: %1(%2)", errno, strerror(errno)).c_str());
			}
			break;
			case Select::SELECT_TIMEOUT:
			{
				if (got_child_return_code)
				{
					// pretend the child output is closed, since they've exited
					// and select timed out.
					outIsOpen = false;
					errIsOpen = false;
				}
				else
				{
					++cumulative_timeout;
					if (timeoutsecs >= 0 && cumulative_timeout >= (timeoutsecs * 10))
					{
						OW_THROW(ExecTimeoutException, "timedout");
					}
				}
			}
			break;
			default:
			{
				cumulative_timeout = 0;
				UnnamedPipeRef readstream;
				// if both have output, we'll get error first.
				if (streams.err()->getSelectObj() == fdset[selectrval])
				{
					readstream = streams.err();
				}
				else if (streams.out()->getSelectObj() == fdset[selectrval])
				{
					readstream = streams.out();
				}
				if (readstream)
				{
					char buff[1024];
					int readrc = readstream->read(buff, sizeof(buff) - 1);
					if (readrc == 0)
					{
						if (readstream == streams.out())
							outIsOpen = false;
						else
							errIsOpen = false;
					}
					else if (readrc == -1)
					{
						OW_THROW(ExecErrorException, format("Error reading stdout from lwcclient: %1(%2)", errno, strerror(errno)).c_str());
					}
					else
					{
						buff[readrc] = 0;
						if (outputlimit >= 0 && output.length() + readrc > static_cast<size_t>(outputlimit))
						{
							// the process output too much, so just copy what we can and return error
							int lentocopy = outputlimit - output.length();
							if (lentocopy >= 0 && static_cast<size_t>(lentocopy) < sizeof(buff))
							{
								buff[lentocopy] = '\0';
								output += buff;
							}
							OW_THROW(ExecBufferFullException, "");
						}
						output += buff;
					}
				}
			}
			break;
		}
	}
}

} // end namespace Exec
} // end namespace OpenWBEM

