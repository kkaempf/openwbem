/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_UserUtils.hpp"

#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef OW_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef OW_HAVE_PWD_H
#include <pwd.h>
#endif

namespace OW_NAMESPACE
{

namespace UserUtils
{

/////////////////////////////////////////////////////////////////////////////
String getEffectiveUserId()
{
#ifdef OW_WIN32
#pragma message(Reminder "TODO: Implement getEffectiveUserID using SID method!")
	// TODO
	// The user ID is represented by a SID on Win32. Going to return 0 for
	// root user until I get through the Win32 CIMOM. Eventually OW will
	// deal with userid on Win32 the proper way.
	return String("0");
#else
	return String(Int64(::geteuid()));
#endif
}

//////////////////////////////////////////////////////////////////////////////
String getCurrentUserName()
{
	bool ok;
#ifdef OW_WIN32
	return getUserName(0, ok);
#else
	return getUserName(getuid(),ok);
#endif
}

namespace
{
Mutex g_getpwMutex;
}

//////////////////////////////////////////////////////////////////////////////
String getUserName(uid_t uid,bool& ok)
{
#ifdef OW_WIN32
#pragma message(Reminder "TODO: HONOR uid parm in getUserName!")
	// TODO
	// Ignore uid for right now. Just return the current User (WRONG!)
	// Need to come back to this later when the uid_t stuff is worked out.
	char name[256];
	unsigned long size = sizeof(name);
	size = sizeof(name);
	if (!::GetUserName(name, &size))
	{
		return String();
	}

	return String(name);
#else

#ifdef HAVE_GETPWUID_R
	passwd pw;
	//I can't imagine an easy way to calculate the size of the additional buffer
	//  ahead of time. 
	size_t const additionalSize =
#ifdef _SC_GETPW_R_SIZE_MAX
		sysconf (_SC_GETPW_R_SIZE_MAX);
#else
		10240;
#endif
	std::vector<char> additional(additionalSize);
	passwd* result;
	::getpwuid_r(uid, &pw, &additional[0], additional.size(), &result);
#else
	MutexLock lock(g_getpwMutex);
	passwd* result = ::getpwuid(uid);
#endif  
	if (result)
	{
		ok = true;
		return result->pw_name;
	}
	ok = false;
	return "";
#endif
}

//////////////////////////////////////////////////////////////////////////////
UserID
getUserId(const String& userName, bool& validUserName)
{
	validUserName = false;

#ifdef OW_WIN32
#pragma message(Reminder "TODO: Write getUserId!")
	return 0;
#else


#ifdef HAVE_GETPWNAM_R
	size_t bufsize =
#ifdef _SC_GETPW_R_SIZE_MAX
		sysconf (_SC_GETPW_R_SIZE_MAX);
#else
		1024;
#endif
	std::vector<char> buf(bufsize);
	struct passwd pwd;
	passwd* result = 0;
	int rv = getpwnam_r(userName.c_str(), &pwd, &buf[0], bufsize, &result);
	if ((rv != 0) || (p != &pwd))
	{
		return INVALID_USERID;
	}

#else
	MutexLock ml(g_getpwMutex);
	struct passwd* result;
	result = ::getpwnam(userName.c_str());
#endif
	if (result)
	{
		validUserName = true;
		return result->pw_uid;
	}
	return INVALID_USERID;
#endif
}
} // end namespace UserUtils
} // end namespace OW_NAMESPACE


