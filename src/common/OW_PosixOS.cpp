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
#include "OW_OS.hpp"
#include "OW_MutexLock.hpp"

extern "C"
{
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
}

static OW_Mutex g_pwdLock;

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_OS::getUserId(const OW_String& userName, OW_UserId& userId)
{
	OW_MutexLock ml(g_pwdLock);
	OW_Bool rv = false;
	struct passwd* pwd;
	pwd = ::getpwnam(userName.c_str());
	if(pwd)
	{
		userId = pwd->pw_uid;
		rv = true;
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_OS::getUserName(const OW_UserId& userId, OW_String& userName)
{
	OW_MutexLock ml(g_pwdLock);
	OW_Bool rv = false;
	struct passwd* pwd;
	pwd = ::getpwuid(userId);
	if(pwd)
	{
		userName = pwd->pw_name;
		rv = true;
	}
	return rv;
}
