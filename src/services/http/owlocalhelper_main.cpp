/*******************************************************************************
* Copyright (C) 2001-2005 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_LocalAuthenticationCommon.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#if defined(OW_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#if defined(OW_HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif

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

using std::cerr;
using std::endl;
using std::cin;
using std::cout;
using namespace OpenWBEM;
using namespace OpenWBEM::LocalAuthenticationCommon;

namespace
{

const int SUCCESS = 0;

// list of possible error codes.
const int NOT_SETUID_ROOT = 3;
const int INVALID_INPUT = 4;
const int REMOVE_FAILED = 5;
const int UNEXPECTED_EXCEPTION = 6;
const int SETRLIMIT_FAILED = 7;
const int MISSING_LOCAL_AUTH_DIR_ARG = 8;

String local_auth_dir;

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

	String fullPath = local_auth_dir + OW_FILENAME_SEPARATOR + filename;
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
	if (realPath != fullPath || realPathDirname != local_auth_dir)
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

int processCreate()
{
	// Read the uid from stdin
	String uid;
	if (!getLineFromStdin(uid))
	{
		cerr << "owlocalhelper::processCreate(): expected to get the uid" << endl;
		return INVALID_INPUT;
	}
	
	// Read a random number from stdin to put in file for client to read
	String cookie;
	if (!getLineFromStdin(cookie))
	{
		cerr << "owlocalhelper::processCreate(): expected to get the cookie" << endl;
		return INVALID_INPUT;
	}

    cout << createFile(local_auth_dir, uid, cookie) << endl;
	
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
		initializeDir(local_auth_dir);
		return SUCCESS;
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
	if (argc < 2)
	{
		return MISSING_LOCAL_AUTH_DIR_ARG;
	}
	local_auth_dir = argv[1];
	try
	{

		// I want full control over file permissions
		::umask(0);

#ifdef OW_HAVE_SETRLIMIT
		// Be careful to not drop core.
		struct rlimit rlim;
		rlim.rlim_cur = rlim.rlim_max = 0;
		if (setrlimit(RLIMIT_CORE, &rlim) < 0)
		{
			perror("owlocalhelper::setrlimit failed");
			return SETRLIMIT_FAILED;
		}
#endif
	
		// I can't work without euid 0, because only root can change file ownership.
		if (::geteuid() != 0)
		{
			cerr << "owlocalhelper must be setuid root to work" << endl;
			return NOT_SETUID_ROOT;
		}
	
		return processStdin();
	}
	catch (LocalAuthenticationException& e)
	{
		cerr << e.getMessage() << endl;
		return e.getErrorCode();
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


