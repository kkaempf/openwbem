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

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Format.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <ctime>
#include <iostream>

// commands (first line read from stdin):
//   remove - delete one file
//    input: filename w/out the path, cookie
//    output: none
//   initialize - create dir if necessary & remove all old files
//    input: none
//    output: none
//   create - creates a file
//    input: uid, cookie
//    output: absolute pathname
//
// If the operation was successful, 0 (SUCCESS) is the return value.
// If the operation failed, an error message is printed to stderr and the return code is != 0.

namespace
{

using namespace std;
using namespace OpenWBEM;

const char* OWCIMOMD_USER_STR = "owcimomd";
const char* REMOVE_CMD = "remove";
const char* INITIALIZE_CMD = "initialize";
const char* CREATE_CMD = "create";

#define LOCAL_AUTH_DIR OW_DEFAULT_STATE_DIR "/openwbem/OWLocal"

const int SUCCESS = 0;

// list of possible error codes.
const int INVALID_USER = 2;
const int NOT_SETUID_ROOT = 3;
const int INVALID_INPUT = 4;
const int REMOVE_FAILED = 5;
const int INITIALIZE_FAILED = 6;
const int UNEXPECTED_EXCEPTION = 7;
const int CREATE_FAILED = 8;

bool checkRealUser()
{
	struct passwd* pw = ::getpwuid(::getuid());
	if (pw == 0)
	{
		perror("owlocalhelper:checkRealUser():getpwuid");
		return false;
	}
	if (strcmp(OWCIMOMD_USER_STR, pw->pw_name) == 0)
	{
		return true;
	}
	cerr << "owlocalhelper:checkRealUser(): username is not " << OWCIMOMD_USER_STR << endl;
	return false;
}

// returns true if a line was successfully read
bool getLineFromStdin(String& line)
{
	// Use basic_istream::getline() instead of String::getline() so that I can limit the amount of data that is read.

	const int lineBufferSize = 1024;
	char buffer[lineBufferSize];
	cin.getline(buffer, lineBufferSize, '\n');
	int count = cin.gcount();

	// if hit eof
	if (!cin && count == 0)
	{
		cerr << "owlocalhelper:getLineFromStdin(): expected a line, but hit eof." << endl;
		return false;
	}

	// read a partial line, then hit eof
	if (cin.eof())
	{
		line = buffer;
		return true;
	}

	// line is longer than the buffer.  This shouldn't happen, and I'll treat it as a fatal error
	else if (cin.fail())
	{
		cerr << "owlocalhelper:getLineFromStdin(): line too long." << endl;
		return false;
	}

	// successfully read a line
	else
	{
		line = buffer;
		return true;
	}
}

int processRemove()
{
	String filename;
	if (!getLineFromStdin(filename))
	{
		return INVALID_INPUT;
	}

	if (filename.indexOf(OW_FILENAME_SEPARATOR) != String::npos)
	{
		cerr << "owlocalhelper::processRemove(): filename cannot contain " OW_FILENAME_SEPARATOR << endl;
		return INVALID_INPUT;
	}

	if (filename.empty())
	{
		cerr << "owlocalhelper::processRemove(): filename cannot be empty." << endl;
		return INVALID_INPUT;
	}

	String fullPath = String(LOCAL_AUTH_DIR) + OW_FILENAME_SEPARATOR + filename;
	// translate to the real path and recheck the directory
	String realPath;
	try
	{
		 realPath = FileSystem::Path::realPath(fullPath);
	}
	catch (FileSystemException& e)
	{
		cerr << "owlocalhelper::processRemove(): realPath failed: " << e << endl;
		return INVALID_INPUT;
	}

	String realPathDirname = FileSystem::Path::dirname(realPath);
	if (realPath != fullPath || realPathDirname != LOCAL_AUTH_DIR)
	{
		cerr << "owlocalhelper::processRemove(): real path is not equal. no symlinks allowed in " << fullPath << endl;
		return INVALID_INPUT;
	}

	String cookie;
	if (!getLineFromStdin(cookie))
	{
		return INVALID_INPUT;
	}

	String realCookie = FileSystem::getFileContents(realPath);

	if (realCookie != cookie)
	{
		cerr << "owlocalhelper::processRemove(): real cookie is not equal, not removing." << endl;
		return INVALID_INPUT;
	}

	if (!FileSystem::removeFile(realPath))
	{
		perror(Format("owlocalhelper::processRemove(): failed to remove %1", realPath).c_str());
		return REMOVE_FAILED;
	}

	return SUCCESS;
}

int processInitialize()
{
	StringArray dirParts = String(LOCAL_AUTH_DIR).tokenize(OW_FILENAME_SEPARATOR);
	String curDir;
	for (size_t i = 0; i < dirParts.size(); ++i)
	{
		// for each intermediary dir, try to create it.
		curDir += OW_FILENAME_SEPARATOR;
		curDir += dirParts[i];
		int rv = ::mkdir(curDir.c_str(), 0755);
		if (rv == -1 && errno != EEXIST)
		{
			perror(Format("owlocalhelper::processInitialize(): mkdir(%1, 0755)", curDir).c_str());
			return INITIALIZE_FAILED;
		}
		if (rv == 0)
		{
			// if we actually created the directory, remove the set-group-id bit 
			// and change the file & group ownership to the real user/group id
			struct stat statbuf;
			if (lstat(curDir.c_str(), &statbuf) == -1)
			{
				perror(Format("owlocalhelper::processInitialize(): lstat(%1, ...)", curDir).c_str());
				return INITIALIZE_FAILED;
			}
			
			if (chmod(curDir.c_str(), statbuf.st_mode & ~S_ISGID) == -1)
			{
				perror(Format("owlocalhelper::processInitialize(): chmod(%1, ...)", curDir).c_str());
				return INITIALIZE_FAILED;
			}

			if (chown(curDir.c_str(), 0, 0) == -1)
			{
				perror(Format("owlocalhelper::processInitialize(): chown(%1, 0, 0)", curDir).c_str());
				return INITIALIZE_FAILED;
			}
		}

		// check that all intermediary dirs have at least a+rx perms, otherwise fail.
		struct stat statbuf;
		if (lstat(curDir.c_str(), &statbuf) == -1)
		{
			perror(Format("owlocalhelper::processInitialize(): lstat(%1, ...)", curDir).c_str());
			return INITIALIZE_FAILED;
		}

		int necessaryMask = S_IROTH | S_IXOTH;
		if (statbuf.st_mode & necessaryMask != necessaryMask)
		{
			cerr << "owlocalhelper::processInitialize(): directory permissions on " << curDir 
				<< " are " << statbuf.st_mode << ". That is insufficient." << endl;
			return INITIALIZE_FAILED;
		}
	}

	// for each file in the dir, check it's creation time and delete it if its more than a day old or newer than the current time.
	StringArray files;
	if (!FileSystem::getDirectoryContents(LOCAL_AUTH_DIR, files))
	{
		perror(Format("owlocalhelper::processInitialize(): getDirectoryContents(%1, ...) failed", curDir).c_str());
		return INITIALIZE_FAILED;
	}

	for (size_t i = 0; i < files.size(); ++i)
	{
		struct stat statbuf;
		String curFilePath = String(LOCAL_AUTH_DIR) + OW_FILENAME_SEPARATOR + files[i];
		if (lstat(curFilePath.c_str(), &statbuf) == -1)
		{
			perror(Format("owlocalhelper::processInitialize(): lstat(%1, ...)", curFilePath).c_str());
			return INITIALIZE_FAILED;
		}

		if (S_ISREG(statbuf.st_mode))
		{
			time_t curTime = ::time(NULL);
			const time_t ONE_DAY = 24 * 60 * 60;
			if ((statbuf.st_ctime < curTime - ONE_DAY) || statbuf.st_ctime > curTime)
			{
				if (::unlink(curFilePath.c_str()) == -1)
				{
					perror(Format("owlocalhelper::processInitialize(): unlink(%1)", curFilePath).c_str());
					return INITIALIZE_FAILED;
				}
			}
		}
	}
	return SUCCESS;
}

int processCreate()
{
	// Read the uid from stdin
	String uid;
	if (!getLineFromStdin(uid))
	{
		cerr << "owlocalhelper::processCreate(): expected to get the uid" << endl;
		return INVALID_INPUT;
	}

	uid_t userid = ~0;
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
		cerr << "owlocalhelper::processCreate(): uid \"" << uid << "\" is not a valid uid_t" << endl;
		return INVALID_INPUT;
	}

	//-- Create temporary file for auth process
	// Some old implementations of mkstemp() create a file with mode 0666.
	// The fact that the umask is set to 0077 makes this safe from prying eyes.
	::umask(0077);

	char tfname[1024];
	int authfd;
	::snprintf(tfname, sizeof(tfname), "%s/%dXXXXXX", LOCAL_AUTH_DIR, ::getpid());
	authfd = ::mkstemp(tfname);
	if (authfd == -1)
	{
		perror(Format("owlocalhelper::processCreate(): mkstemp(%1)", tfname).c_str());
		return CREATE_FAILED;
	}

	//-- Change file permission on temp file to read/write for user only
	if (::fchmod(authfd, 0400) == -1)
	{
		perror(Format("owlocalhelper::processCreate(): fchmod on %1", tfname).c_str());
		::close(authfd);
		::unlink(tfname);
		return CREATE_FAILED;
	}
	
	//-- Change file so the user connecting is the owner
	if (::fchown(authfd, userid, static_cast<gid_t>(-1)) == -1)
	{
		perror(Format("owlocalhelper::processCreate(): fchown on %1 to %2", tfname, userid).c_str());
		::close(authfd);
		::unlink(tfname);
		return CREATE_FAILED;
	}
	
	// Read a random number from stdin to put in file for client to read
	String cookie;
	if (!getLineFromStdin(cookie))
	{
		cerr << "owlocalhelper::processCreate(): expected to get the cookie" << endl;
		::close(authfd);
		::unlink(tfname);
		return INVALID_INPUT;
	}

	// Write the servers random number to the temp file
	if (::write(authfd, cookie.c_str(), cookie.length()) != static_cast<ssize_t>(cookie.length()))
	{
		perror(Format("owlocalhelper::processCreate(): failed to write() the cookie to %1", tfname).c_str());
		::close(authfd);
		::unlink(tfname);
		return CREATE_FAILED;
	}
	
	::close(authfd);

	// caller is looking for this.
	cout << tfname << endl;
	
	return SUCCESS;
}

int processStdin()
{
	// even though we'll only run if getuid() == owcimomd, we'll still be extremely 
	// paranoid about any input we get, since as a setuid binary, we could do a lot
	// of damage

	String curLine;
    if (!getLineFromStdin(curLine))
	{
		return INVALID_INPUT;
	}

	if (curLine == REMOVE_CMD)
	{
		return processRemove();
	}
	else if (curLine == INITIALIZE_CMD)
	{
		return processInitialize();
	}
	else if (curLine == CREATE_CMD)
	{
		return processCreate();
	}

	return INVALID_INPUT;
}

} // end unnamed namespace


//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	try
	{

		// I want full control over file permissions
		::umask(0);
	
		// only owcimomd user can run me
		if (!checkRealUser())
		{
			return INVALID_USER;
		}
	
		// I can't work without euid 0, because only root can change file ownership.
		if (::geteuid() != 0)
		{
			cerr << "owlocalhelper must be setuid root to work" << endl;
			return NOT_SETUID_ROOT;
		}
	
		return processStdin();
	}
	catch (std::exception& e)
	{
		cerr << "Caught unexpected exception: " << e.what() << endl;
		return UNEXPECTED_EXCEPTION;
	}
	catch (...)
	{
		cerr << "Caught unexpected exception" << endl;
		return UNEXPECTED_EXCEPTION;
	}
}


