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


#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_FileSystemTestCases.hpp"
#include "OW_FileSystem.hpp"
#include "OW_File.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include <errno.h>
#ifndef OW_WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif
#include <sys/types.h>

using namespace OpenWBEM;
using namespace std;

void OW_FileSystemTestCases::setUp()
{
}

void OW_FileSystemTestCases::tearDown()
{
}

void OW_FileSystemTestCases::testgetLock()
{
	File f = FileSystem::openFile("Makefile");
	unitAssert(f);
	unitAssert( f.getLock() == 0 );
}

void OW_FileSystemTestCases::testtryLock()
{
	File f = FileSystem::openFile("Makefile");
	unitAssert(f);
	unitAssert( f.getLock() == 0 );
#ifndef OW_WIN32	// no fork :(
	// the lock is recursive, meaning to get a block, we've got to try to
	// acquire it from another process.  So fork()
	int rval = 0;
	pid_t p = fork();
	switch (p)
	{
		case -1:
			unitAssert(0);
			break;
		case 0: // child
			if ( f.tryLock() == -1 && (errno == EAGAIN || errno == EACCES) )
				_exit(0);
			else
				_exit(1);
			break;
		default: // parent
			{
				int status;
				pid_t p2 = waitpid(p, &status, 0);
				unitAssert(p2 == p);
				unitAssert(WIFEXITED(status));
				rval = WEXITSTATUS(status);
			}
				
			break;
	}
	// child should have returned 0 if the test worked.
	unitAssert( rval == 0 );
#endif
}

void OW_FileSystemTestCases::testunlock()
{
	File f = FileSystem::openFile("Makefile");
	unitAssert(f);
	unitAssert( f.getLock() == 0 );
	unitAssert( f.unlock() == 0 );
#ifndef OW_WIN32	// no fork :(
	// the lock is recursive, meaning to get a block, we've got to try to
	// acquire it from another process.  So fork()
	int rval = 0;
	pid_t p = fork();
	switch (p)
	{
		case -1:
			unitAssert(0);
			break;
		case 0: // child
			if ( f.tryLock() == 0 )
				_exit(0);
			else
				_exit(1);
			break;
		default: // parent
			{
				int status;
				pid_t p2 = waitpid(p, &status, 0);
				unitAssert(p2 == p);
				unitAssert(WIFEXITED(status));
				rval = WEXITSTATUS(status);
			}
				
			break;
	}
	// child should have returned 0 if the test worked.
	unitAssert( rval == 0 );
#endif
}

void OW_FileSystemTestCases::testopenOrCreateFile()
{
	// make sure it's gone.
	FileSystem::removeFile("testfile");
	// first it should be created
	File f = FileSystem::openOrCreateFile("testfile");
	unitAssert(f);
	unitAssert(f.close() == 0);
	// now it will be opened
	f = FileSystem::openOrCreateFile("testfile");
	unitAssert(f);
	unitAssert(f.close() == 0);
	unitAssert(FileSystem::removeFile("testfile"));
}

void OW_FileSystemTestCases::testgetFileContents()
{
	FileSystem::removeFile("testfile");
	File f = FileSystem::openOrCreateFile("testfile");
	const char* contents = "line1\nline2";
	f.write(contents, ::strlen(contents));
	f.close();
	unitAssert(FileSystem::getFileContents("testfile") == contents);
	unitAssert(FileSystem::removeFile("testfile"));
}

void OW_FileSystemTestCases::testgetFileLines()
{
	FileSystem::removeFile("testfile");
	File f = FileSystem::openOrCreateFile("testfile");
	const char* contents = "line1\nline2";
	f.write(contents, ::strlen(contents));
	f.close();
	unitAssert(FileSystem::getFileLines("testfile").size() == 2);
	unitAssert(FileSystem::getFileLines("testfile")[0] == "line1");
	unitAssert(FileSystem::getFileLines("testfile")[1] == "line2");
	unitAssert(FileSystem::removeFile("testfile"));
}

void OW_FileSystemTestCases::testdirname()
{
	unitAssert(FileSystem::Path::dirname("/x/y") == "/x");
	unitAssert(FileSystem::Path::dirname("/x/y/") == "/x");
	unitAssert(FileSystem::Path::dirname("/x/") == "/");
	unitAssert(FileSystem::Path::dirname("/x") == "/");
	unitAssert(FileSystem::Path::dirname("x") == ".");
	unitAssert(FileSystem::Path::dirname("//x") == "/");
	unitAssert(FileSystem::Path::dirname("///x////") == "/");
	unitAssert(FileSystem::Path::dirname("/") == "/");
	unitAssert(FileSystem::Path::dirname("////") == "/");
	unitAssert(FileSystem::Path::dirname(".") == ".");
	unitAssert(FileSystem::Path::dirname("..") == ".");
	unitAssert(FileSystem::Path::dirname("x/") == ".");
	unitAssert(FileSystem::Path::dirname("x//") == ".");
	unitAssert(FileSystem::Path::dirname("x///") == ".");
	unitAssert(FileSystem::Path::dirname("x/y") == "x");
	unitAssert(FileSystem::Path::dirname("x///y") == "x");
}



void OW_FileSystemTestCases::testrealPath()
{
	using namespace FileSystem::Path;

	symlink("SYMLINK_LOOP", "SYMLINK_LOOP");
	symlink(".", "SYMLINK_1");
	symlink("//////./../../etc", "SYMLINK_2");
	symlink("SYMLINK_1", "SYMLINK_3");
	symlink("SYMLINK_2", "SYMLINK_4");
	symlink("doesNotExist", "SYMLINK_5");
	File f(FileSystem::createFile("doesExist"));

	unitAssert(realPath("/") == "/");
	unitAssert(realPath("/////////////////////////////////") == "/");
	unitAssert(realPath("/.././.././.././..///") ==  "/");
	unitAssert(realPath("/etc") == "/etc");
	unitAssert(realPath("/etc/../etc") == "/etc");
	try
	{
		realPath("/doesNotExist/../etc");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
	unitAssert(realPath("././././.") == getCurrentWorkingDirectory());
	unitAssert(realPath("/././././.") == "/");
	unitAssert(realPath("/etc/./././.") == "/etc");
	try
	{
        realPath("/etc/.//doesNotExist");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
    unitAssert(realPath("./doesExist") == getCurrentWorkingDirectory() + "/doesExist");
    unitAssert(realPath("./doesExist/") == getCurrentWorkingDirectory() + "/doesExist");
    unitAssert(realPath("./doesExist/../doesExist") == getCurrentWorkingDirectory() + "/doesExist");
	try
	{
//		cout << '"' << realPath("doesNotExist") << '"' << endl;
        realPath("doesNotExist");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
    unitAssert(realPath(".") == getCurrentWorkingDirectory());
	try
	{
//		cout << '"' << realPath("doesNotExist") << '"' << endl;
        realPath("./doesNotExist");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
	try
	{
        realPath("SYMLINK_LOOP");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ELOOP);
	}
	try
	{
        realPath("./SYMLINK_LOOP");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ELOOP);
	}
	unitAssert(realPath("SYMLINK_1") == getCurrentWorkingDirectory());
	try
	{
        realPath("SYMLINK_1/doesNotExist");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
	unitAssert(realPath("SYMLINK_2") == "/etc");
	unitAssert(realPath("SYMLINK_3") == getCurrentWorkingDirectory());
	unitAssert(realPath("SYMLINK_4") == "/etc");
	unitAssert(realPath("../unit/SYMLINK_1") == getCurrentWorkingDirectory());
	unitAssert(realPath("../unit/SYMLINK_2") == "/etc");
	unitAssert(realPath("../unit/SYMLINK_3") == getCurrentWorkingDirectory());
	unitAssert(realPath("../unit/SYMLINK_4") == "/etc");
	try
	{
        realPath("./SYMLINK_5");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
	try
	{
        realPath("SYMLINK_5");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
	try
	{
        realPath("SYMLINK_5/doesNotExist");
		unitAssert(0);
	}
	catch (FileSystemException& e)
	{
		unitAssert(e.getErrorCode() == ENOENT);
	}
	unitAssert(realPath("doesExist/../../unit/doesExist") == getCurrentWorkingDirectory() + "/doesExist");
	unitAssert(realPath("doesExist/../../unit/.") == getCurrentWorkingDirectory());

	FileSystem::removeFile("doesExist");
	FileSystem::removeFile("SYMLINK_LOOP");
	FileSystem::removeFile("SYMLINK_1");
	FileSystem::removeFile("SYMLINK_2");
	FileSystem::removeFile("SYMLINK_3");
	FileSystem::removeFile("SYMLINK_4");
	FileSystem::removeFile("SYMLINK_5");
}

Test* OW_FileSystemTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_FileSystem");

	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testgetLock);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testtryLock);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testunlock);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testopenOrCreateFile);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testgetFileContents);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testgetFileLines);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testdirname);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testrealPath);

	return testSuite;
}

