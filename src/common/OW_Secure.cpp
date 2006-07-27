/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Quest Software, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_Secure.hpp"
#include "OW_FileSystem.hpp"
#include "OW_String.hpp"
#include "OW_Paths.hpp"
#include "OW_Format.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <algorithm>

#ifdef AIX
#include "OW_StringBuffer.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include <fstream>
#include <cctype>
#endif

#if defined(OW_NO_SETRESGID_PROTO) && defined(OW_HAVE_SETRESGID)
extern "C" { int setresgid(gid_t rgid, gid_t egid, gid_t sgid); }
#endif

#if defined(OW_NO_SETRESUID_PROTO) && defined(OW_HAVE_SETRESUID)
extern "C" { int setresuid(uid_t ruid, uid_t euid, uid_t suid); }
#endif

using namespace OpenWBEM;

#define THROW_IF(tst, ExceptionClass, msg) \
	do \
	{ \
		if (tst) \
		{ \
			OW_THROW(ExceptionClass, (msg)); \
		} \
	} while (false)

#define THROW_ERRNO_IF(tst, ExceptionClass, msg) \
	do \
	{ \
		if (tst) \
		{ \
			OW_THROW_ERRNO_MSG(ExceptionClass, (msg)); \
		} \
	} while (false)

#define ABORT_IF(tst, msg) THROW_IF((tst), Secure::ProcessAbortException, (msg))

#define ABORT_ERRNO_IF(tst, msg) \
  THROW_ERRNO_IF((tst), Secure::ProcessAbortException, (msg))

namespace
{
#if !defined(OW_HAVE_SETEUID) && defined(OW_HAVE_SETREUID)
int seteuid(uid_t euid)
{
	return (setreuid(-1, euid));
}

#endif

#if !defined(OW_HAVE_SETEGID) && defined(OW_HAVE_SETRESGID)
int setegid(uid_t egid)
{
	return(setresgid(-1, egid, -1));
}
#endif

} // end anonymous namespace

namespace OW_NAMESPACE
{
namespace Secure
{
	OW_DEFINE_EXCEPTION(ProcessAbort);

	// Original source: Item 1.3, _Secure Programming Cookbook for C and C++_, by
	// John Viega and Matt Messier. 
	// Original C code reformatted and modified for C++.
	// Some inspiration provided by uidswap.c from openssh-portable
	void dropPrivilegesPermanently(::uid_t newuid, ::gid_t newgid)
	{
		// Note: If any manipulation of privileges cannot be completed
		// successfully, it is safest to assume that the process is in an
		// unknown state and not allow it to continue (abort).

		if (newgid == ::gid_t(-1))
		{
			newgid = ::getgid();
		}
		::gid_t oldegid = ::getegid();
		::gid_t oldgid = ::getgid();
		if (newuid == ::uid_t(-1))
		{
			newuid = ::getuid();
		}
		::uid_t oldeuid = ::geteuid();
		::uid_t olduid = ::getuid();

		// If root privileges are to be dropped, be sure to pare down the
		// ancillary groups for the process before doing anything else because
		// the setgroups() system call requires root privileges.  Drop ancillary
		// groups regardless of whether privileges are being dropped temporarily
		// or permanently.
		if (oldeuid == 0)
		{
			::setgroups(1, &newgid);
		}

		if (newgid != oldegid)
		{
#if defined(OW_HAVE_SETRESGID) && !defined(OW_BROKEN_SETRESGID)
			ABORT_ERRNO_IF(::setresgid(newgid, newgid, newgid) == -1, "drop_privileges [1]");
#elif defined(OW_HAVE_SETREGID) && !defined(OW_BROKEN_SETREGID)
			ABORT_ERRNO_IF(::setregid(newgid, newgid) == -1, "drop_privileges [1]");
#else
			ABORT_ERRNO_IF(::setegid(newgid) == -1, "drop_privileges [1]");
			ABORT_ERRNO_IF(::setgid(newgid) == -1, "drop_privileges [1.1]");
#endif
		}

		if (newuid != oldeuid)
		{
#if defined(OW_HAVE_SETRESUID) && !defined(OW_BROKEN_SETRESUID)
			ABORT_ERRNO_IF(::setresuid(newuid, newuid, newuid) == -1, "drop_privileges [2]");
#elif defined(OW_HAVE_SETREUID) && !defined(OW_BROKEN_SETREUID)
			ABORT_ERRNO_IF(::setreuid(newuid, newuid) == -1, "drop_privileges [2]");
#else
#if !defined(OW_SETEUID_BREAKS_SETUID)
			ABORT_ERRNO_IF(::seteuid(newuid) == -1, "drop_privileges [2]");
#endif
			ABORT_ERRNO_IF(::setuid(newuid) == -1, "drop_privileges [2.1]");
#endif
		}

		// verify that the changes were successful
		// make sure gid drop was successful
		ABORT_IF(::getgid() != newgid || ::getegid() != newgid, "drop_privileges [3]");

		// make sure gid restoration fails
		ABORT_IF(
			newuid != 0 && newgid != oldegid &&
			(::setegid(oldegid) != -1 || ::setgid(oldgid) != -1),
			"drop_privileges [4]"
		);

		// make sure uid drop was successful
		ABORT_IF(::getuid() != newuid || ::geteuid() != newuid, "drop_privileges [5]");

		// make sure uid restoration fails
		ABORT_IF(
			newuid != 0 && newuid != oldeuid &&
			(::seteuid(oldeuid) != -1 || ::setuid(olduid) != -1),
			"drop_privileges [6]"
		);
	}

namespace
{
#ifdef AIX
	NonRecursiveMutex envMutex;
	String odmdir;

	char const default_odmdir[] = "ODMDIR=/etc/objrepos";

	String check_line(String const & line)
	{
		StringBuffer sb;
		char const * s;
		char c;
		for (s = line.c_str(); (c = *s) && !std::isspace(c); ++s)
		{
			switch (c)
			{
				case '\\':
					if (s[1] == '\0')
					{
						// Unexpected format
						return default_odmdir;
					}
					c = *++s;
					break;
				case '$':
				case '`':
				case '"':
				case '\'':
					// Unexpected format
					return default_odmdir;
				default:
					;
			}
			sb += c;
		}
		if (c == '\0')
		{
			return sb.releaseString();
		}
		while (std::isspace(*s))
		{
			++s;
		}
		if (*s == '#')
		{
			return sb.releaseString();
		}
		// Unexpected format
		return default_odmdir;
	}

	String setODMDIR()
	{
		String retval(default_odmdir);
		std::ifstream is("/etc/environment");
		while (is)
		{
			String s = String::getLine(is).trim();
			if (s.startsWith("ODMDIR="))
			{
				retval = check_line(s);
			}
		}
		return retval;
	}

	void addPlatformSpecificEnvVars(StringArray & environ)
	{
		NonRecursiveMutexLock lock(envMutex);
		if (odmdir.empty())
		{
			odmdir = setODMDIR();
		}
		environ.push_back(odmdir);
	}

#else

	inline void addPlatformSpecificEnvVars(StringArray &)
	{
	}

#endif
}

	StringArray minimalEnvironment()
	{
		StringArray retval;
		retval.push_back("IFS= \t\n");
		retval.push_back("PATH=" _PATH_STDPATH);
		char * tzstr = ::getenv("TZ");
		if (tzstr)
		{
			retval.push_back(String("TZ=") + tzstr);
		}
		addPlatformSpecificEnvVars(retval);
		return retval;
	}

	void runAs(char const * username)
	{
		ABORT_IF(!username, "null user name");
		ABORT_IF(*username == '\0', "empty user name");
		ABORT_IF(::getuid() != 0 || ::geteuid() != 0, "non-root user calling runAs");
		errno = 0;
		struct passwd * pwent = ::getpwnam(username);
		// return value from getpwnam is a static, so don't free it.
		ABORT_ERRNO_IF(!pwent && errno != 0, Format("getpwnam(\"%1\") failed", username).c_str());
		ABORT_IF(!pwent, Format("user name (%1) not found", username).c_str());
		int rc = ::chdir("/");
		ABORT_ERRNO_IF(rc != 0, "chdir failed");
		Secure::dropPrivilegesPermanently(pwent->pw_uid, pwent->pw_gid);
	}

} // namespace Secure
} // namespace OW_NAMESPACE
