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
 */

#include "OW_config.h"
#include "OW_PidFile.hpp"
#include "blocxx/File.hpp"
#include "blocxx/FileSystem.hpp"
#include "blocxx/String.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace OW_NAMESPACE
{

namespace PidFile
{
/**
 * Read the contents of a pid file and return the results.
 * @param pidfile	The fully qualified path to the pid file.
 * @return	The process id on success. Otherwise -1
 */
int 
readPid(const char *pidfile)
{
	FILE *f;
	int pid = -1;
	if (!(f = fopen(pidfile,"r")))
	{
		return -1;
	}

	if (fscanf(f,"%d", &pid) != 1)
	{
		fclose(f);
		return -1;
	}
	
	if (fclose(f) != 0)
	{
		return -1;
	}

	return pid;
}
/**
 * Ensure the pid file corresponds to a currently running process.
 *	@param pidfile	The fully qualified path to the pid file.
 * @return	The process id on success. Otherwise -1
 */
int 
checkPid(const char *pidfile)
{
	int pid = readPid(pidfile);
	// Check for failure to open the pid file.  Without this test, a kill(-1,0)
	// would be attempted, which will only show that there is at least one
	// process running on the system (any random process).
	if ( pid <= 0 )
	{
		return -1;
	}
	// Amazing ! _I_ am already holding the pid file...
	if (pid == getpid())
		return -1;
	// Check if the process exists
	if (kill(pid, 0) && errno == ESRCH)
		return -1;
	return pid;
}
/**
 * Writes the pid to the specified file.
 * @param pidfile		The fully qualified path to the pid file
 *	@return	The process id on success. Otherwise -1 and errno is set to
 *				the error encountered by this function
 */
int 
writePid(const char *pidfile)
{
	FILE *f;
	int fd;
	int pid;
	int lerrno;
	if ((fd = open(pidfile, O_RDWR|O_CREAT, 0644)) == -1)
	{
		return -1;
	}
	if ((f = fdopen(fd, "r+")) == NULL)
	{
		lerrno = errno;
		close(fd);
		errno = lerrno;
		return -1;
	}
	File OWf(::dup(fd)); // need a dup, since the FILE* will close it.
	if (OWf.tryLock() == -1)
	{
		lerrno = errno;
		fscanf(f, "%d", &pid);
		fclose(f);
		errno = lerrno;
		return -1;
	}
	pid = getpid();
	if (!fprintf(f,"%d\n", pid))
	{
		lerrno = errno;
		fclose(f);
		errno = lerrno;
		return -1;
	}
	fflush(f);
	OWf.unlock();
	if (fclose(f) != 0)
	{
		return -1;
	}
	return pid;
}
/**
 * Remove a pid file.
 *
 * @return	The results of the unlink call.
 */
int 
removePid(const char *pidfile)
{
	return FileSystem::removeFile(pidfile);
}

} // end namespace PidFile
} // end namespace OW_NAMESPACE

