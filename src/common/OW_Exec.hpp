/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_EXEC_HPP_
#define OW_EXEC_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_Reference.hpp"
#include "OW_String.hpp"
#include "OW_ArrayFwd.hpp"

namespace OpenWBEM
{

DECLARE_EXCEPTION(ExecTimeout);
DECLARE_EXCEPTION(ExecBufferFull);
DECLARE_EXCEPTION(ExecError);
class UnnamedPipe;
typedef Reference<UnnamedPipe> UnnamedPipeRef;
class PopenStreamsImpl;
/**
 * This class represents a connection to a process.
 */
class PopenStreams
{
public:
	PopenStreams();
	~PopenStreams();
	PopenStreams(const PopenStreams& src);
	PopenStreams& operator=(const PopenStreams& src);
	/**
	 * Get a write-only pipe to the process's stdin.
	 */
	UnnamedPipeRef in() const;
	
	/**
	 * Set a pipe to the process's stdin
	 */
	void in(UnnamedPipeRef pipe);
	/**
	 * Get a read-only pipe to the process's stdout
	 */
	UnnamedPipeRef out() const;
	/**
	 * Set a pipe to the process's stdout
	 */
	void out(UnnamedPipeRef pipe);
	/**
	 * Get a read-only pipe to the process's stderr
	 */
	UnnamedPipeRef err() const;
	/**
	 * Set a pipe to the process's stderr
	 */
	void err(UnnamedPipeRef pipe);
	/**
	 * Get the process's pid.  If the process's exit status has already been
	 * read by calling getExitStatus(), then this will return -1
	 */
	ProcId pid() const;
	/**
	 * Set the process's pid.  This is only usefule when constructing an 
	 * instance of this class.
	 */
	void pid(ProcId newPid);
	/**
	 * Get the process's exit status.  
	 * If the child process is still running, this function will do everything
	 * possible to terminate it.
	 * The following steps will be taken to attempt to terminate the child
	 * process.
	 * 1. The input and output pipes will be closed.  This may cause the
	 *    child to get a SIGPIPE which may terminate it.
	 * 2. If the child still hasn't terminated after 10 seconds, a SIGTERM 
	 *    is sent.
	 * 3. If the child still hasn't terminated after 10 seconds, a SIGKILL
	 *    is sent.
	 * After calling this function, the pipes to will be closed, and this object
	 * is basically useless.
	 * @return The exit status of the process.  This should be evaluated using
	 * the family of macros (WIFEXITED(), WEXITSTATUS(), etc.) from "sys/wait.h"
	 */
	int getExitStatus();
	/**
	 * Sets the process's exit status.
	 * This function is used by Exec::gatherOutput()
	 */
	void setProcessStatus(int ps);
private:
	Reference<PopenStreamsImpl> m_impl;
};

/////////////////////////////////////////////////////////////////////////////
namespace Exec
{
	/**
	 * Execute a command.
	 * The command will inherit stdin, stdout, and stderr from the parent
	 * process.  This function will not search the path for command[0], so
	 * the absolute path to the binary should be specified.  If the path needs
	 * to be searched, you can set command[0] = "/bin/sh"; command[1] = "-c";
	 * and then fill in the rest of the array with the command you wish to 
	 * execute.
	 * This function blocks until the child process exits.  Be careful that
	 * the command you run doesn't hang.  It is recommended to use
	 * executeProcessAndGatherOutput() if the command is untrusted.
	 *
	 * @param command
	 *  command[0] is the binary to be executed.
	 *  command[1] .. command[n] are the command line parameters to the command.
	 *
	 * @return 127 if the execve() call for command[0]
     * fails,  -1 if there was another error and the return code
     * of the command otherwise.
	 */
	int safeSystem(const Array<String>& command);
	/**
	 * Execute a command.
	 * The command's stdin, stdout, and stderr will be connected via pipes to
	 * the parent process that can be accessed from the return value.
	 * This function will not search the path for command[0], so
	 * the absolute path to the binary should be specified.  If the path needs
	 * to be searched, you can set command[0] = "/bin/sh"; command[1] = "-c";
	 * and then fill in the rest of the array with the command you wish to 
	 * execute.
	 * This function does *not* block until the child process exits.
	 *
	 * @param command
	 *  command[0] is the binary to be executed.
	 *  command[1] .. command[n] are the command line parameters to the command.
	 *
	 * @param initialInput
	 *  The string is sent to stdin of the child process.
	 *
	 * @return A PopenStreams object which can be used to access the child
	 *  process and/or get it's return value.
	 */
	PopenStreams safePopen(const Array<String>& command,
			const String& initialInput = String());
	/**
	 * Wait for output from a child process.  The function returns when the
	 * process exits. In the case that the child process doesn't exit, if a 
	 * timout is specified, then an ExecTimeoutException is thrown.
	 * If the process outputs more bytes than outputlimit, an 
	 * ExecBufferFullException is thrown.
	 *
	 * @param output An out parameter, the process output will be appended to 
	 *  this string.
	 * @param streams The connection to the child process.
	 * @param processstatus An out parameter, which will contain the process
	 *  status.  It is only valid if the funtion returns. In the case an 
	 *  exception is thrown, it's undefined. It should be evaluated using the 
	 *  family of macros (WIFEXITED(), WEXITSTATUS(), etc.) from "sys/wait.h"
	 * @param timeoutsecs Specifies the number of seconds to wait for the 
	 *  process to exit. If the process hasn't exited after timeoutsecs seconds,
	 *  an ExecTimeoutException will be thrown. If timeoutsecs < 0, the 
	 *  timeout will be infinite, and no exception will ever be thrown.
	 * @param outputlimit Specifies the maximum size of the parameter output,
	 *  in order to constrain possible memory usage.  If the process outputs
	 *  more data than will fit into output, then an ExecBufferFullException
	 *  is thrown. If outputlimit < 0, the limit will be infinite, and an
	 *  ExecBufferFullException will never be thrown.
	 *
	 * @throws ProcessError on error. 
	 * @throws ProcessTimeout if the process hasn't finished within timeoutsecs. 
	 * @throws ProcessBufferFull if the process output exceeds outputlimit bytes.
	 */
	void gatherOutput(String& output, PopenStreams& streams, int& processstatus, int timeoutsecs = -1, int outputlimit = -1);
	
	/**
	 * Run a process, collect the output, and wait for it to exit.  The 
	 * function returns when the
	 * process exits. In the case that the child process doesn't exit, if a 
	 * timout is specified, then an ExecTimeoutException is thrown.
	 * If the process outputs more bytes than outputlimit, an 
	 * ExecBufferFullException is thrown.
	 * This function will not search the path for command[0], so
	 * the absolute path to the binary should be specified.  If the path needs
	 * to be searched, you can set command[0] = "/bin/sh"; command[1] = "-c";
	 * and then fill in the rest of the array with the command you wish to 
	 * execute. Exercise caution when doing this, as you may be creating a
	 * security hole.
	 * If the process does not terminate by itself, or if an exception is 
	 * thrown because a limit has been reached (time or output), then the
	 * the following steps will be taken to attempt to terminate the child
	 * process.
	 * 1. The input and output pipes will be closed.  This may cause the
	 *    child to get a SIGPIPE which may terminate it.
	 * 2. If the child still hasn't terminated after 10 seconds, a SIGTERM 
	 *    is sent.
	 * 3. If the child still hasn't terminated after 10 seconds, a SIGKILL
	 *    is sent.
	 *
	 * @param command
	 *  command[0] is the binary to be executed.
	 *  command[1] .. command[n] are the command line parameters to the command.
	 * @param output An out parameter, the process output will be appended to 
	 *  this string.
	 * @param streams The connection to the child process.
	 * @param processstatus An out parameter, which will contain the process
	 *  status.  It is only valid if the funtion returns. In the case an 
	 *  exception is thrown, it's undefined. It should be evaluated using the 
	 *  family of macros (WIFEXITED(), WEXITSTATUS(), etc.) from "sys/wait.h"
	 * @param timeoutsecs Specifies the number of seconds to wait for the 
	 *  process to exit. If the process hasn't exited after timeoutsecs seconds,
	 *  an ExecTimeoutException will be thrown, and the process will be 
	 *  killed. 
	 *  If timeoutsecs < 0, the timeout will be infinite, and a 
	 *  ExecTimeoutException will not be thrown.
	 * @param outputlimit Specifies the maximum size of the parameter output,
	 *  in order to constrain possible memory usage.  If the process outputs
	 *  more data than will fit into output, then an ExecBufferFullException
	 *  is thrown, and the process will be killed. 
	 *  If outputlimit < 0, the limit will be infinite, and an
	 *  ExecBufferFullException will not be thrown.
	 *
	 * @throws ProcessError on error. 
	 * @throws ProcessTimeout if the process hasn't finished within timeoutsecs. 
	 * @throws ProcessBufferFull if the process output exceeds outputlimit bytes.
	 */
	void executeProcessAndGatherOutput(const Array<String>& command,
		String& output, int& processstatus,
		int timeoutsecs = -1, int outputlimit = -1);
	
	
} // end namespace Exec

} // end namespace OpenWBEM

typedef OpenWBEM::ExecTimeoutException OW_ExecTimeoutException;
typedef OpenWBEM::ExecBufferFullException OW_ExecBufferFullException;
typedef OpenWBEM::ExecErrorException OW_ExecErrorException;
typedef OpenWBEM::PopenStreams OW_PopenStreams;

#endif
