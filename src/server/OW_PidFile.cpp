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
#include "OW_config.h"
#include "OW_PidFile.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

namespace OpenWBEM
{

/**
 * Read the contents of a pid file and return the results.
 * @param pidfile	The fully qualified path to the pid file.
 * @return	The process id on success. Otherwise -1
 */
int 
PidFile::readPid(const char *pidfile)
{
	FILE *f;
	int pid = -1;
	if(!(f = fopen(pidfile,"r")))
		return -1;
	fscanf(f,"%d", &pid);
	fclose(f);
	return pid;
}
/**
 * Ensure the pid file corresponds to a currently running process.
 *	@param pidfile	The fully qualified path to the pid file.
 * @return	The process id on success. Otherwise -1
 */
int 
PidFile::checkPid(const char *pidfile)
{
	int pid = readPid(pidfile);
	// Amazing ! _I_ am already holding the pid file...
	if((!pid) || (pid == getpid()))
		return -1;
	// Check if the process exists
	if(kill(pid, 0) && errno == ESRCH)
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
PidFile::writePid(const char *pidfile)
{
	FILE *f;
	int fd;
	int pid;
	int lerrno;
	if((fd = open(pidfile, O_RDWR|O_CREAT, 0644)) == -1)
	{
		return -1;
	}
	if((f = fdopen(fd, "r+")) == NULL)
	{
		lerrno = errno;
		close(fd);
		errno = lerrno;
		return -1;
	}
#ifdef OW_GNU_LINUX
    if(flock(fd, LOCK_EX|LOCK_NB) == -1)
#elif defined(OW_OPENSERVER)
    if(lockf(fd, F_TLOCK, 0) == -1)
#else
	if(ftrylockfile(f) != 0)
#endif
	{
		lerrno = errno;
		fscanf(f, "%d", &pid);
		fclose(f);
		errno = lerrno;
		return -1;
	}
	pid = getpid();
	if(!fprintf(f,"%d\n", pid))
	{
		lerrno = errno;
		fclose(f);
		errno = lerrno;
		return -1;
	}
	fflush(f);
#ifdef OW_GNU_LINUX
    if(flock(fd, LOCK_UN) == -1)
    {
        lerrno = errno;
        fclose(f);
        errno = lerrno;
        return -1;
    }
#elif defined (OW_OPENSERVER)
    if(lockf(fd, F_ULOCK, 0) == -1)
    {
        lerrno = errno;
        fclose(f);
        errno = lerrno;
        return -1;
    }
#else
	funlockfile(f);
#endif
	fclose(f);
	return pid;
}
/**
 * Remove a pid file.
 *
 * @return	The results of the unlink call.
 */
int 
PidFile::removePid(const char *pidfile)
{
	return unlink(pidfile);
}

} // end namespace OpenWBEM

