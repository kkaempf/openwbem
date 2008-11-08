/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
#include "OW_LocalAuthenticationCommon.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_FileSystem.hpp"
#include "OW_File.hpp"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_Types.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <ctime>
#include <iostream>

namespace OW_NAMESPACE
{

namespace LocalAuthenticationCommon
{

OW_DEFINE_EXCEPTION(LocalAuthentication);

/**
 * @throws LocalAuthenticationException if something failed
 */
String initializeDir(String const & localAuthDir)
{
	StringArray dirParts = localAuthDir.tokenize(OW_FILENAME_SEPARATOR);
	String curDir;
	for (size_t i = 0; i < dirParts.size(); ++i)
	{
		// for each intermediary dir, try to create it.
		curDir += OW_FILENAME_SEPARATOR;
		curDir += dirParts[i];
		int rv = ::mkdir(curDir.c_str(), 0755);
		if (rv == -1 && errno != EEXIST)
		{
			OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): mkdir(%1, 0755)", curDir).c_str());
		}
		if (rv == 0)
		{
			// if we actually created the directory, remove the set-group-id bit & fix any permissions removed by the current umask 
			// and change the file & group ownership to the real user/group id
			struct stat statbuf;
			if (lstat(curDir.c_str(), &statbuf) == -1)
			{
				OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): lstat(%1, ...)", curDir).c_str());
			}
			
			::mode_t newmode(statbuf.st_mode);
			newmode &= ~S_ISGID;
			newmode |= S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
			if (chmod(curDir.c_str(), newmode) == -1)
			{
				OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): chmod(%1, ...)", curDir).c_str());
			}

			if (chown(curDir.c_str(), 0, 0) == -1)
			{
				OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): chown(%1, 0, 0)", curDir).c_str());
			}
		}

		// check that all intermediary dirs have at least a+rx perms, otherwise fail.
		struct stat statbuf;
		if (lstat(curDir.c_str(), &statbuf) == -1)
		{
			OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): lstat(%1, ...)", curDir).c_str());
		}

		mode_t necessaryMask = S_IROTH | S_IXOTH;
		if ((statbuf.st_mode & necessaryMask) != necessaryMask)
		{
			OW_THROW(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): directory permissions on %1"
				" are %2. That is insufficient", curDir, statbuf.st_mode).c_str());
		}
	}

	String realLocalAuthDir = FileSystem::Path::realPath(localAuthDir);
	// for each file in the dir, check it's creation time and delete it if its more than a day old or newer than the current time.
	StringArray files;
	if (!FileSystem::getDirectoryContents(realLocalAuthDir, files))
	{
		OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): getDirectoryContents(%1, ...) failed", realLocalAuthDir).c_str());
	}

	for (size_t i = 0; i < files.size(); ++i)
	{
		struct stat statbuf;
		String curFilePath = localAuthDir + OW_FILENAME_SEPARATOR + files[i];
		if (lstat(curFilePath.c_str(), &statbuf) == -1)
		{
			OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): lstat(%1, ...)", curFilePath).c_str());
		}

		if (S_ISREG(statbuf.st_mode))
		{
			time_t curTime = ::time(NULL);
			const time_t ONE_DAY = 24 * 60 * 60;
			if ((statbuf.st_ctime < curTime - ONE_DAY) || statbuf.st_ctime > curTime)
			{
				if (::unlink(curFilePath.c_str()) == -1)
				{
					OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthentication::initializeDir(): unlink(%1)", curFilePath).c_str());
				}
			}
		}
	}

	return realLocalAuthDir;
}

namespace
{
	class UmaskRestorer
	{
	public:
		UmaskRestorer(::mode_t oldumask) 
			: m_oldumask(oldumask) 
		{
		}
		~UmaskRestorer()
		{
			::umask(m_oldumask);
		}
	private:
		::mode_t m_oldumask;
	};

	class FileDeleter
	{
	public:
		FileDeleter(const String& filename)
			: m_filename(filename)
			, m_delete(true)
		{
		}
		~FileDeleter()
		{
			if (m_delete)
			{
				FileSystem::removeFile(m_filename);
			}
		}
		void dontDelete()
		{
			m_delete = false;
		}
	private:
		String m_filename;
		bool m_delete;
	};
}

String createFile(String const & localAuthDir, const String& uid, const String& cookie)
{
	uid_t userid = uid_t(~0);
	try
	{
		if (sizeof(userid) == sizeof(UInt16))
		{
			userid = uid.toUInt16();
		}
		else if (sizeof(userid) == sizeof(UInt32))
		{
			userid = uid.toUInt32();
		}
		else if (sizeof(userid) == sizeof(UInt64))
		{
			userid = uid.toUInt64();
		}
	}
	catch (StringConversionException& e)
	{
		OW_THROW(LocalAuthenticationException, Format("LocalAuthenticationCommon::createFile(): uid \"%1\" is not a valid uid_t", uid).c_str());
	}

	//-- Create temporary file for auth process

	// Some old implementations of mkstemp() create a file with mode 0666.

	String tfname = Format("%1/%2XXXXXX", localAuthDir, ::getpid());
	int authfd = -1;

	{
		// The fact that the umask is set to 0077 makes this safe from prying eyes.
		// Note that this could cause a problem for the calling code, especially if it's threaded.
		// however, owcimomd has a 0077 mask anyway, and owlocalhelper is single threaded.
		::mode_t oldumask = ::umask(0077);
		UmaskRestorer umaskRestorer(oldumask);
		authfd = ::mkstemp(&tfname[0]);
	}

	if (authfd == -1)
	{
		OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthenticationCommon::createFile(): mkstemp(\"%1\")", tfname).c_str());
	}

	FileDeleter fileDeleter(tfname);
	File file(authfd);

	//-- Change file permission on temp file to read/write for user only
	if (::fchmod(authfd, 0400) == -1)
	{
		OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthenticationCommon::createFile(): fchmod on \"%1\"", tfname).c_str());
	}
	
	//-- Change file so the user connecting is the owner
	if (::fchown(authfd, userid, static_cast<gid_t>(-1)) == -1)
	{
		OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthenticationCommon::createFile(): fchown on \"%1\" to %2", tfname, userid).c_str());
	}
	
	// Write the servers random number to the temp file
	if (file.write(cookie.c_str(), cookie.length()) != cookie.length())
	{
		OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthenticationCommon::createFile(): failed to write() the cookie to \"%1\"", tfname).c_str());
	}
	if (file.flush() == -1)
	{
		OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthenticationCommon::createFile(): failed to flush() \"%1\"", tfname).c_str());
	}
	if (file.close() == -1)
	{
		OW_THROW_ERRNO_MSG(LocalAuthenticationException, Format("LocalAuthenticationCommon::createFile(): failed to close() \"%1\"", tfname).c_str());
	}
	
	fileDeleter.dontDelete();

	return tfname;
}

} // end namespace LocalAuthenticationCommon
} // end namespace OW_NAMESPACE


