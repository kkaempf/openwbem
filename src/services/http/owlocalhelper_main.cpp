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
#include "OW_Types.hpp"
#include "OW_LocalAuthenticationCommon.hpp"
#include "OW_Exception.hpp"
#include "blocxx/GlobalString.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#if defined(OW_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#include <cstring>
#include <cstdio>
#include <cerrno>
#include <ctime>
#include <iostream>

// commands (first line read from stdin):
//   remove - delete one file
//    input: filename w/out the path, cookie
//    output: S\n<empty>
//   initialize - create dir if necessary & remove all old files
//    input: none
//    output: S\n<empty>
//   create - creates a file
//    input: uid, cookie
//    output: S\nabsolute pathname
//
// If the operation was successful, S is printed to stdout, followed by a newline and then the return value.
// If the operation failed, F followed by a newline a unsigned integer error number a space and an error message is printed to stdout.

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
const int ERR_NOT_ROOT = 3;
const int ERR_INVALID_INPUT = 4;
const int ERR_REMOVE_FAILED = 5;
const int ERR_UNEXPECTED_EXCEPTION = 6;
const int ERR_MISSING_LOCAL_AUTH_DIR_ARG = 7;

OW_DECLARE_EXCEPTION(LocalHelper);
OW_DEFINE_EXCEPTION(LocalHelper);

GlobalString local_auth_dir = BLOCXX_GLOBAL_STRING_INIT(""); // assigned in main()

// returns true if a line was successfully read
void getLineFromStdin(String& line)
{
	// Use basic_istream::getline() instead of String::getline() so that I can limit the amount of data that is read.

	const int lineBufferSize = 1024;
	char buffer[lineBufferSize];
	cin.getline(buffer, lineBufferSize, '\n');
	int count = cin.gcount();

	// if hit eof
	if (!cin && count == 0)
	{
		OW_THROW_ERR(LocalHelperException, "owlocalhelper:getLineFromStdin(): expected a line, but hit eof.", ERR_INVALID_INPUT);
	}

	// read a partial line, then hit eof
	if (cin.eof())
	{
		line = buffer;
		return;
	}

	// line is longer than the buffer.  This shouldn't happen, and I'll treat it as a fatal error
	else if (cin.fail())
	{
		OW_THROW_ERR(LocalHelperException, "owlocalhelper:getLineFromStdin(): line too long.", ERR_INVALID_INPUT);
	}

	// successfully read a line
	else
	{
		line = buffer;
		return;
	}
}

void processRemove()
{
	String filename;
	getLineFromStdin(filename);

	String cookie;
	getLineFromStdin(cookie);

	if (filename.indexOf(OW_FILENAME_SEPARATOR) != String::npos)
	{
		OW_THROW_ERR(LocalHelperException, Format("owlocalhelper::processRemove(): filename cannot contain %1", OW_FILENAME_SEPARATOR).c_str(), ERR_INVALID_INPUT);
	}

	if (filename.empty())
	{
		OW_THROW_ERR(LocalHelperException, "owlocalhelper::processRemove(): filename cannot be empty.", ERR_INVALID_INPUT);
	}

	String fullPath = Format("%1%2%3", local_auth_dir, OW_FILENAME_SEPARATOR, filename);
	// translate to the real path and recheck the directory
	String realPath;
	try
	{
		 realPath = FileSystem::Path::realPath(fullPath);
	}
	catch (FileSystemException& e)
	{
		OW_THROW_ERR(LocalHelperException, Format("owlocalhelper::processRemove(): realPath failed: %1", e).c_str(), ERR_INVALID_INPUT);
	}

	String realPathDirname = FileSystem::Path::dirname(realPath);
	if (realPath != fullPath || realPathDirname != local_auth_dir)
	{
		OW_THROW_ERR(LocalHelperException, Format("owlocalhelper::processRemove(): real path is not equal. no symlinks allowed in \"%1\"", fullPath).c_str(), ERR_INVALID_INPUT);
	}

	String realCookie = FileSystem::getFileContents(realPath);

	if (realCookie != cookie)
	{
		OW_THROW_ERR(LocalHelperException, "owlocalhelper::processRemove(): real cookie is not equal, not removing.", ERR_INVALID_INPUT);
	}

	if (!FileSystem::removeFile(realPath))
	{
		OW_THROW_ERR(LocalHelperException, Format("owlocalhelper::processRemove(): failed to remove \"%1\":%2", realPath, ::strerror(errno)).c_str(), ERR_REMOVE_FAILED);
	}
}

void processCreate(String& filepath)
{
	// Read the uid from stdin
	String uid;
	getLineFromStdin(uid);
	
	// Read a random number from stdin to put in file for client to read
	String cookie;
	getLineFromStdin(cookie);

    filepath = createFile(local_auth_dir, uid, cookie);
}

void processStdin(String& output)
{
	// Be extremely paranoid about any input we get, since as a root binary, we could do a lot of damage

	String curLine;
    getLineFromStdin(curLine);

	if (curLine == REMOVE_CMD)
	{
		processRemove();
		output = "S\n";
	}
	else if (curLine == INITIALIZE_CMD)
	{
		local_auth_dir = initializeDir(local_auth_dir);
		output = "S\n";
	}
	else if (curLine == CREATE_CMD)
	{
		String filepath;
		processCreate(filepath);
		output = Format("S\n%1", filepath);
	}
	else
	{
		OW_THROW_ERR(LocalHelperException, Format("owlocalhelper::processStdin(): invalid command: \"%1\"", curLine).c_str(), ERR_INVALID_INPUT);
	}
}

void removeNewlines(String& s)
{
	for (size_t i = 0; i < s.length(); ++i)
	{
		if (s[i] == '\n')
		{
			s[i] = ' ';
		}
	}
}

} // end unnamed namespace


//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		return ERR_MISSING_LOCAL_AUTH_DIR_ARG;
	}
	local_auth_dir = argv[1];

	// I want full control over file permissions
	::umask(0);

	// I can't work without euid 0, because only root can change file ownership.
	if (::geteuid() != 0)
	{
		cerr << "owlocalhelper must run as root to work" << endl;
		return ERR_NOT_ROOT;
	}

	while (true)
	{
		try
		{
			String output;
			processStdin(output);
			cout << output << endl;
		}
		catch (LocalHelperException& e)
		{
			cout << "F\n";
			if (e.getErrorCode() != Exception::UNKNOWN_ERROR_CODE)
			{
				cout << e.getErrorCode();
			}
			else
			{
				cout << ERR_UNEXPECTED_EXCEPTION;
			}
			cout << ' ';

			// have to make sure no newlines mess up the protocol
			String msg = e.getMessage();
			removeNewlines(msg);

			cout << msg << endl;
		}
		catch (LocalAuthenticationException& e)
		{
			cout << "F\n";
			if (e.getErrorCode() != Exception::UNKNOWN_ERROR_CODE)
			{
				cout << e.getErrorCode();
			}
			else
			{
				cout << ERR_UNEXPECTED_EXCEPTION;
			}
			cout << ' ';

			// have to make sure no newlines mess up the protocol
			String msg = e.getMessage();
			removeNewlines(msg);

			cout << msg << endl;
		}
		catch (std::exception& e)
		{
			cout << "F\n";
			cout << ERR_UNEXPECTED_EXCEPTION;
			cout << ' ';

			// have to make sure no newlines mess up the protocol
			String msg = e.what();
			removeNewlines(msg);

			cout << msg << endl;
		}
		catch (...)
		{
			cout << "F\n";
			cout << ERR_UNEXPECTED_EXCEPTION;
			cout << ' ';
			cout << "Caught unexpected exception" << endl;
		}
	}
}


