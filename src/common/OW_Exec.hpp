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

DECLARE_EXCEPTION(ExecTimeout);
DECLARE_EXCEPTION(ExecBufferFull);
DECLARE_EXCEPTION(ExecError);

class OW_UnnamedPipe;
typedef OW_Reference<OW_UnnamedPipe> OW_UnnamedPipeRef;

class OW_PopenStreamsImpl;
/**
 * This class represents a connection to a process.
 */
class OW_PopenStreams
{
public:
	OW_PopenStreams();
	~OW_PopenStreams();
	OW_PopenStreams(const OW_PopenStreams& src);
	OW_PopenStreams& operator=(const OW_PopenStreams& src);

	/**
	 * Get a write-only pipe to the process's stdin.
	 */
	OW_UnnamedPipeRef in() const;
	
	/**
	 * Set a pipe to the process's stdin
	 */
	void in(OW_UnnamedPipeRef pipe);

	/**
	 * Get a read-only pipe to the process's stdout
	 */
	OW_UnnamedPipeRef out() const;

	/**
	 * Set a pipe to the process's stdout
	 */
	void out(OW_UnnamedPipeRef pipe);

	/**
	 * Get a read-only pipe to the process's stderr
	 */
	OW_UnnamedPipeRef err() const;

	/**
	 * Set a pipe to the process's stderr
	 */
	void err(OW_UnnamedPipeRef pipe);

	/**
	 * Get the process's pid.  If the process's exit status has already been
	 * read by calling getExitStatus(), then this will return -1
	 */
	OW_ProcId pid() const;

	/**
	 * Set the process's pid.  This is only usefule when constructing an 
	 * instance of this class.
	 */
	void pid(OW_ProcId newPid);

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
	 * This function is used by OW_Exec::gatherOutput()
	 */
	void setProcessStatus(int ps);

private:
	OW_Reference<OW_PopenStreamsImpl> m_impl;
};

class OW_Exec
{
public:

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
	static int safeSystem(const OW_Array<OW_String>& command);

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
	 * @return A OW_PopenStreams object which can be used to access the child
	 *  process and/or get it's return value.
	 */
	static OW_PopenStreams safePopen(const OW_Array<OW_String>& command,
			const OW_String& initialInput = OW_String());

	/**
	 * Wait for output from a child process.  The function returns when the
	 * process exits. In the case that the child process doesn't exit, if a 
	 * timout is specified, then an OW_ExecTimeoutException is thrown.
	 * If the process outputs more bytes than outputlimit, an 
	 * OW_ExecBufferFullException is thrown.
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
	 *  an OW_ExecTimeoutException will be thrown. If timeoutsecs < 0, the 
	 *  timeout will be infinite, and no exception will ever be thrown.
	 * @param outputlimit Specifies the maximum size of the parameter output,
	 *  in order to constrain possible memory usage.  If the process outputs
	 *  more data than will fit into output, then an OW_ExecBufferFullException
	 *  is thrown. If outputlimit < 0, the limit will be infinite, and an
	 *  OW_ExecBufferFullException will never be thrown.
	 *
	 * @throws OW_ProcessError on error. 
	 * @throws OW_ProcessTimeout if the process hasn't finished within timeoutsecs. 
	 * @throws OW_ProcessBufferFull if the process output exceeds outputlimit bytes.
	 */
	static void gatherOutput(OW_String& output, OW_PopenStreams& streams, int& processstatus, int timeoutsecs = -1, int outputlimit = -1);
	
	/**
	 * Run a process, collect the output, and wait for it to exit.  The 
	 * function returns when the
	 * process exits. In the case that the child process doesn't exit, if a 
	 * timout is specified, then an OW_ExecTimeoutException is thrown.
	 * If the process outputs more bytes than outputlimit, an 
	 * OW_ExecBufferFullException is thrown.
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
	 *  an OW_ExecTimeoutException will be thrown, and the process will be 
	 *  killed. 
	 *  If timeoutsecs < 0, the timeout will be infinite, and a 
	 *  OW_ExecTimeoutException will not be thrown.
	 * @param outputlimit Specifies the maximum size of the parameter output,
	 *  in order to constrain possible memory usage.  If the process outputs
	 *  more data than will fit into output, then an OW_ExecBufferFullException
	 *  is thrown, and the process will be killed. 
	 *  If outputlimit < 0, the limit will be infinite, and an
	 *  OW_ExecBufferFullException will not be thrown.
	 *
	 * @throws OW_ProcessError on error. 
	 * @throws OW_ProcessTimeout if the process hasn't finished within timeoutsecs. 
	 * @throws OW_ProcessBufferFull if the process output exceeds outputlimit bytes.
	 */
	static void executeProcessAndGatherOutput(const OW_Array<OW_String>& command,
		OW_String& output, int& processstatus,
		int timeoutsecs = -1, int outputlimit = -1);
	
	
};

#endif
