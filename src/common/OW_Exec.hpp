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

#ifndef OW_EXEC_HPP_
#define OW_EXEC_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_String.hpp"
#include "OW_ArrayFwd.hpp"
#include "OW_CommonFwd.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(ExecTimeout, OW_COMMON_API);
OW_DECLARE_APIEXCEPTION(ExecBufferFull, OW_COMMON_API);
OW_DECLARE_APIEXCEPTION(ExecError, OW_COMMON_API);

class PopenStreamsImpl;
/**
 * This class represents a connection to a process.
 */
class OW_COMMON_API PopenStreams
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
	void in(const UnnamedPipeRef& pipe);
	/**
	 * Get a read-only pipe to the process's stdout
	 */
	UnnamedPipeRef out() const;
	/**
	 * Set a pipe to the process's stdout
	 */
	void out(const UnnamedPipeRef& pipe);
	/**
	 * Get a read-only pipe to the process's stderr
	 */
	UnnamedPipeRef err() const;
	/**
	 * Set a pipe to the process's stderr
	 */
	void err(const UnnamedPipeRef& pipe);
	/**
	 * Get the process's pid.  If the process's exit status has already been
	 * read by calling getExitStatus(), then this will return -1
	 */
	ProcId pid() const;
	/**
	 * Set the process's pid.
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
	IntrusiveReference<PopenStreamsImpl> m_impl;

	friend bool operator==(const PopenStreams& x, const PopenStreams& y);
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
	OW_COMMON_API int safeSystem(const Array<String>& command);
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
	 * If the binary specified in command[0] does not exist, execv() will fail,
	 * and the return code of the sub-process will be 127. However, this is not
	 * distinguishable from the command process returning 127.
	 *
	 * @param command
	 *  command[0] is the binary to be executed.
	 *  command[1] .. command[n] are the command line parameters to the command.
	 *
	 * @return A PopenStreams object which can be used to access the child
	 *  process and/or get it's return value.
	 *
	 * @throws IOException If writing initialInput to the child process input fails.
	 *         ExecErrorException If command.size() == 0 or if fork() fails.
	 */
	OW_COMMON_API PopenStreams safePopen(const Array<String>& command);

	/**
	 * The use of initialInput is deprecated, because it's not safe to use it in a
	 * portable manner. Either use the input parameter to gatherOutput or do it
	 * manually (not that you must be careful not to cause a deadlock).
	 */
	OW_COMMON_API PopenStreams safePopen(const Array<String>& command,
			const String& initialInput) OW_DEPRECATED;

	const int INFINITE_TIMEOUT = -1;

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
	 *  an ExecTimeoutException will be thrown. If timeoutsecs == INFINITE_TIMEOUT, the
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
	OW_COMMON_API void gatherOutput(String& output, PopenStreams& streams, int& processstatus, int timeoutsecs = INFINITE_TIMEOUT, int outputlimit = -1);
	
	enum EOutputSource
	{
		E_STDOUT,
		E_STDERR
	};

	class OutputCallback
	{
	public:
		virtual ~OutputCallback();
		void handleData(const char* data, size_t dataLen, EOutputSource outputSource, PopenStreams& theStream, size_t streamIndex, Array<char>& inputBuffer);
	private:
		/**
		 * @param data The data output from the process identified by theStream. Will be NULL terminated.  However, if the process
		 * output 0 bytes, those will be contained in data.
		 */
		virtual void doHandleData(const char* data, size_t dataLen, EOutputSource outputSource, PopenStreams& theStream, size_t streamIndex, Array<char>& inputBuffer) = 0;
	};

	class InputCallback
	{
	public:
		virtual ~InputCallback();
		void getData(Array<char>& inputBuffer, PopenStreams& theStream, size_t streamIndex);
	private:
		virtual void doGetData(Array<char>& inputBuffer, PopenStreams& theStream, size_t streamIndex) = 0;
	};

	enum EProcessRunning
	{
		E_PROCESS_RUNNING,
		E_PROCESS_EXITED
	};

	// class invariant: if m_running == E_PROCESS_RUNNING, then m_status == 0.
	class ProcessStatus
	{
	public:
		ProcessStatus()
		: m_running(E_PROCESS_RUNNING)
		, m_status(0)
		{
		}

		explicit ProcessStatus(int status)
		: m_running(E_PROCESS_EXITED)
		, m_status(status)
		{
		}

		bool hasExited() const
		{
			return m_running == E_PROCESS_EXITED;
		}

		const int& getStatus() const
		{
			return m_status;
		}
	private:
		EProcessRunning m_running;
		int m_status;
	};

	/**
	 * Wait for output from child processes.  The function returns when the
	 * processes have exited. In the case that a child process doesn't exit, if a
	 * timout is specified, then an ExecTimeoutException is thrown.
	 *
	 * @param output A callback, whenever data is received from a process, it will
	 *  be passed to output.handleData().
	 * 
	 * @param streams The connections to the child processes.
	 * 
	 * @param processstatus An out parameter, which will contain a enum flag
	 *  indicating if the process has exited, and if it has, the processes'
	 *  status. The ProcessStatus::status value, if ProcessStatus::running == E_PROCESS_RUNNING,
	 *  should be evaluated using the family of macros (WIFEXITED(), WEXITSTATUS(), etc.)
	 *  from "sys/wait.h"
	 *  Each status corresponds to the element at the same index in streams.
	 *  If processStatuses.size() != streams.size(), it will be resized.
	 *  Each element will be set to (E_PROCESS_RUNNING, 0) or else
	 *  (E_PROCESS_EXITED, the status of the exited process).
	 * 
	 * @param timeoutSecs Specifies the number of seconds to wait for all the
	 *  processes to exit. If no output has been received and all the processes
	 *  haven't exited after timeoutSecs seconds, an ExecTimeoutException will
	 *  be thrown. If timeoutSecs == INFINITE_TIMEOUT, the timeout will be infinite, and no
	 *  exception will ever be thrown.
	 * 
	 * @param input Callback to provide data to be written to the process(es) standard input.
	 *  input.getData() will be called once for each stream, and subsequently once every time
	 *  data has been written to a process. output.handleData() may also provide input data
	 *  via the inputBuffer parameter, it is called every time data is read from a process.
	 *
	 * @throws ExecErrorException on error.
	 * @throws ExecTimeoutException if the process hasn't finished within timeoutSecs.
	 */
	OW_COMMON_API void gatherOutput(OutputCallback& output, Array<PopenStreams>& streams, Array<ProcessStatus>& processStatuses,
		InputCallback& input, int timeoutSecs = INFINITE_TIMEOUT);
	
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
	 * 
	 * @param output An out parameter, the process output will be appended to
	 *  this string.
	 * 
	 * @param streams The connection to the child process.
	 * 
	 * @param processstatus An out parameter, which will contain the process
	 *  status.  It is only valid if the funtion returns. In the case an
	 *  exception is thrown, it's undefined. It should be evaluated using the
	 *  family of macros (WIFEXITED(), WEXITSTATUS(), etc.) from "sys/wait.h"
	 * 
	 * @param timeoutsecs Specifies the number of seconds to wait for the
	 *  process to exit. If the process hasn't exited after timeoutsecs seconds,
	 *  an ExecTimeoutException will be thrown, and the process will be
	 *  killed.
	 *  If timeoutsecs == INFINITE_TIMEOUT, the timeout will be infinite, and a
	 *  ExecTimeoutException will not be thrown.
	 * 
	 * @param outputlimit Specifies the maximum size of the parameter output,
	 *  in order to constrain possible memory usage.  If the process outputs
	 *  more data than will fit into output, then an ExecBufferFullException
	 *  is thrown, and the process will be killed.
	 *  If outputlimit < 0, the limit will be infinite, and an
	 *  ExecBufferFullException will not be thrown.
	 * 
	 * @param input Data to be written to the child's stdin. After the data has been
	 *  written, stdin will be closed.
	 *
	 * @throws ExecErrorException on error.
	 * @throws ExecTimeoutException if the process hasn't finished within timeoutsecs.
	 * @throws ExecBufferFullException if the process output exceeds outputlimit bytes.
	 */
	OW_COMMON_API void executeProcessAndGatherOutput(
		const Array<String>& command,
		String& output, int& processstatus,
		int timeoutsecs = INFINITE_TIMEOUT, int outputlimit = -1, const String& input = String());
	
	
} // end namespace Exec

} // end namespace OW_NAMESPACE

#endif
