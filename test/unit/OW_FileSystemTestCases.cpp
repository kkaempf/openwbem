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

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_FileSystemTestCases.hpp"
#include "OW_FileSystem.hpp"
#include "OW_File.hpp"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void OW_FileSystemTestCases::setUp()
{
}

void OW_FileSystemTestCases::tearDown()
{
}

void OW_FileSystemTestCases::testgetLock()
{
    OW_File f = OW_FileSystem::openFile("Makefile");
    unitAssert(f);
	unitAssert( f.getLock() == 0 );
}

void OW_FileSystemTestCases::testtryLock()
{
    OW_File f = OW_FileSystem::openFile("Makefile");
    unitAssert(f);
	unitAssert( f.getLock() == 0 );
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
            if( f.tryLock() == -1 && errno == EAGAIN )
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
}

void OW_FileSystemTestCases::testunlock()
{
    OW_File f = OW_FileSystem::openFile("Makefile");
    unitAssert(f);
	unitAssert( f.getLock() == 0 );
	unitAssert( f.unlock() == 0 );
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
            if( f.tryLock() == 0 )
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
}

Test* OW_FileSystemTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_FileSystem");

	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testgetLock);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testtryLock);
	ADD_TEST_TO_SUITE(OW_FileSystemTestCases, testunlock);

	return testSuite;
}

