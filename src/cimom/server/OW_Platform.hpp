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
 * @author Jon Carey
 * @author Dan Nuffer
 * @author Bart Whiteley
 */

#ifndef OW_PLATFORM_HPP_INCLUDE_GUARD_
#define OW_PLATFORM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Exception.hpp"
#include "OW_IOException.hpp"
#include "OW_UserUtils.hpp"

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(Daemon);
namespace Platform
{
	enum
	{
		DAEMONIZE_SUCCESS, 
		DAEMONIZE_FAIL 
	}; 
	enum
	{
		SHUTDOWN,
		REINIT
	};
	struct Options
	{
		Options()
		: debug(false)
		, configFile(false)
		, configFilePath()
		, help(false)
		, error(false)
		{}
		bool debug;
		bool configFile;
		String configFilePath;
		bool help;
		bool error;
	};
	Options daemonInit( int argc, char* argv[] );
	/**
	 * @throws DaemonException on error
	 */
	void daemonize(bool dbgFlg, const String& daemonName);
	int daemonShutdown(const String& daemonName);
	void initDaemonizePipe();
	void sendDaemonizeStatus(int status);
	void initSig();
	void pushSig(int sig);
	int popSig();
	void shutdownSig();
	SelectableIFCRef getSigSelectable();
	using UserUtils::getCurrentUserName;

	/** 
	 * Re-run the daemon.  This closes all file handles and then calls
	 * execv to replace the current process with a new copy of the daemon.
	 * precondition: daemonInit() must have been called previously, because
	 * the same set of arguments will be passed to execv().
	 * 
	 * This function does not return.
	 * 
	 * @throws DaemonException in the case execv() fails.
	 */
	void rerunDaemon();

	/**
	 * Restart the daemon. This initiates the restart process. On POSIX
	 * platforms, it just sends a SIGHUP to the main process.
	 */
	void restartDaemon();

	void installFatalSignalHandlers();
	void removeFatalSignalHandlers();
}; // end namespace Platform


} // end namespace OpenWBEM

#endif
