/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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

extern "C"
{
#include <sys/resource.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}


using std::cerr;
using std::endl;

//////////////////////////////////////////////////////////////////////////////
int
OW_Exec::safeSystem(const OW_Array<OW_String>& command)
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
			i = rl.rlim_max;

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
		cerr << format( "OW_Platform::safePopen: execv failed for program "
				"%1, rval is %2", argv[0], rval);
		_exit(1);
	}

	do
	{
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


class OW_PopenStreamsImpl
{
	public:
		OW_UnnamedPipeRef in();
		void in(OW_UnnamedPipeRef pipe);
		OW_UnnamedPipeRef out();
		void out(OW_UnnamedPipeRef pipe);
		OW_UnnamedPipeRef err();
		void err(OW_UnnamedPipeRef pipe);
		pid_t pid();
		void pid(pid_t newPid);
		int getExitStatus();
		~OW_PopenStreamsImpl();

	private:
		OW_UnnamedPipeRef m_in;
		OW_UnnamedPipeRef m_out;
		OW_UnnamedPipeRef m_err;
		pid_t m_pid;
};

//////////////////////////////////////////////////////////////////////////////
OW_UnnamedPipeRef OW_PopenStreamsImpl::in()
{
	return m_in;
}
//////////////////////////////////////////////////////////////////////////////
void OW_PopenStreamsImpl::in(OW_UnnamedPipeRef pipe)
{
	m_in = pipe;
}
//////////////////////////////////////////////////////////////////////////////
OW_UnnamedPipeRef OW_PopenStreamsImpl::out()
{
	return m_out;
}
//////////////////////////////////////////////////////////////////////////////
void OW_PopenStreamsImpl::out(OW_UnnamedPipeRef pipe)
{
	m_out = pipe;
}
//////////////////////////////////////////////////////////////////////////////
OW_UnnamedPipeRef OW_PopenStreamsImpl::err()
{
	return m_err;
}
//////////////////////////////////////////////////////////////////////////////
void OW_PopenStreamsImpl::err(OW_UnnamedPipeRef pipe)
{
	m_err = pipe;
}

//////////////////////////////////////////////////////////////////////////////
pid_t OW_PopenStreamsImpl::pid()
{
	return m_pid;
}

//////////////////////////////////////////////////////////////////////////////
void OW_PopenStreamsImpl::pid(pid_t newPid)
{
	m_pid = newPid;
}

//////////////////////////////////////////////////////////////////////////////
int OW_PopenStreamsImpl::getExitStatus()
{
	int status;
	do
	{
#ifdef OW_USE_GNUPTH
		if (pth_waitpid(m_pid, &status, 0) == -1)
#else
		if (waitpid(m_pid, &status, 0) == -1)
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
OW_PopenStreamsImpl::~OW_PopenStreamsImpl()
{
	// prevent zombie children
	getExitStatus();
}


//////////////////////////////////////////////////////////////////////////////
OW_PopenStreams
OW_Exec::safePopen(const OW_Array<OW_String>& command,
		const OW_String& initialInput)
{
	OW_PopenStreams retval;
	retval.in( OW_UnnamedPipe::createUnnamedPipe() );
	OW_UnnamedPipeRef upipeOut = OW_UnnamedPipe::createUnnamedPipe();
	upipeOut->setOutputBlocking(true);
	retval.out( upipeOut );
	OW_UnnamedPipeRef upipeErr = OW_UnnamedPipe::createUnnamedPipe();
	upipeErr->setOutputBlocking(true);
	retval.err( upipeErr );

	if (initialInput != "")
	{
		retval.in()->write(initialInput.c_str(), initialInput.length());
	}

	if (command.size() == 0)
	{
		retval.pid( -1 );
		return retval;
	}

#ifdef OW_USE_GNU_PTH
	retval.pid ( pth_fork() );
#else
	retval.pid ( fork() );
#endif

	if (retval.pid() == -1)
		return retval;

	if (retval.pid() == 0)
	{
		// Close stdin, stdout, and stderr.
		close(0);
		close(1);
		close(2);

		// this should only fail because of programmer error.
		OW_PosixUnnamedPipe* in = dynamic_cast<OW_PosixUnnamedPipe*>(retval.in().getPtr());
		OW_PosixUnnamedPipe* out = dynamic_cast<OW_PosixUnnamedPipe*>(retval.out().getPtr());
		OW_PosixUnnamedPipe* err = dynamic_cast<OW_PosixUnnamedPipe*>(retval.err().getPtr());
		OW_ASSERT(in); OW_ASSERT(out); OW_ASSERT(err);

		// connect stdin, stdout, and stderr to the return pipes.
		dup(in->getInputHandle());
		dup(out->getOutputHandle());
		dup(err->getOutputHandle());

		// Close all other file handle from parent process
		rlimit rl;
		int i = sysconf(_SC_OPEN_MAX);
		if (getrlimit(RLIMIT_NOFILE, &rl) != -1)
			i = rl.rlim_max;

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
		cerr << format( "OW_Platform::safePopen: execv failed for program "
				"%1, rval is %2", argv[0], rval);
		_exit(1);
	}

	// this should only fail because of programmer error.
	OW_PosixUnnamedPipe* in = dynamic_cast<OW_PosixUnnamedPipe*>(retval.in().getPtr());
	OW_PosixUnnamedPipe* out = dynamic_cast<OW_PosixUnnamedPipe*>(retval.out().getPtr());
	OW_PosixUnnamedPipe* err = dynamic_cast<OW_PosixUnnamedPipe*>(retval.err().getPtr());
	OW_ASSERT(in); OW_ASSERT(out); OW_ASSERT(err);

	// prevent the parent from using the child's end of the pipes.
	in->closeInputHandle();
	out->closeOutputHandle();
	err->closeOutputHandle();

	return retval;
}

OW_PopenStreams::OW_PopenStreams()
	: m_impl(new OW_PopenStreamsImpl)
{
}

OW_PopenStreams::~OW_PopenStreams()
{
}

OW_Reference<OW_UnnamedPipe> OW_PopenStreams::in() const
{
	return m_impl->in();
}

void OW_PopenStreams::in(OW_UnnamedPipeRef pipe)
{
	m_impl->in(pipe);
}

OW_UnnamedPipeRef OW_PopenStreams::out() const
{
	return m_impl->out();
}

void OW_PopenStreams::out(OW_UnnamedPipeRef pipe)
{
	m_impl->out(pipe);
}

OW_UnnamedPipeRef OW_PopenStreams::err() const
{
	return m_impl->err();
}

void OW_PopenStreams::err(OW_UnnamedPipeRef pipe)
{
	m_impl->err(pipe);
}

pid_t OW_PopenStreams::pid() const
{
	return m_impl->pid();
}

void OW_PopenStreams::pid(pid_t newPid)
{
	m_impl->pid(newPid);
}

int OW_PopenStreams::getExitStatus()
{
	return m_impl->getExitStatus();
}

OW_PopenStreams::OW_PopenStreams(const OW_PopenStreams& src)
	: m_impl(src.m_impl)
{
}

OW_PopenStreams& OW_PopenStreams::operator=(const OW_PopenStreams& src)
{
	m_impl = src.m_impl;
	return *this;
}
