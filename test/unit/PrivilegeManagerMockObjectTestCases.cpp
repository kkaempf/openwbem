/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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
 * @author Kevin Harris
 */

#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "PrivilegeManagerMockObjectTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(PrivilegeManagerMockObjectTestCases,"PrivilegeManagerMockObject");
#include "OW_PrivilegeManagerMockObject.hpp"
#include "OW_PrivilegeManagerMockObjectScope.hpp"
#include "OW_PrivilegeManager.hpp"
#include "blocxx/FileSystem.hpp"

using namespace OpenWBEM;
using namespace blocxx;

void PrivilegeManagerMockObjectTestCases::setUp()
{
}

void PrivilegeManagerMockObjectTestCases::tearDown()
{
}

namespace // anonymous
{
	class MockNonNull : public PrivilegeManagerMockObject
	{
	public:
		MockNonNull() { }
		~MockNonNull() { }

		virtual bool isNull() { return false; }
	};
}

void PrivilegeManagerMockObjectTestCases::testManagerCreation()
{
	// No privilege manager
	{
		PrivilegeManager pm(PrivilegeManager::getPrivilegeManager());
		unitAssert(pm.isNull());
	}

	// With a mock privilege manager.
	{
		PrivilegeManagerMockObjectScope mos(Reference<PrivilegeManagerMockObject>(new MockNonNull));

		PrivilegeManager pm(PrivilegeManager::getPrivilegeManager());
		unitAssert(!pm.isNull());
	}
}


namespace // anonymous
{
	class MockStatter : public PrivilegeManagerMockObject
	{
	public:
		MockStatter() { }
		~MockStatter() { }

		virtual FileSystem::FileInformation stat(const char* pathname);
	};

	FileSystem::FileInformation MockStatter::stat(const char* pathname)
	{
		FileSystem::FileInformation info;

		info.type = FileSystem::FileInformation::E_FILE_REGULAR;
		info.size = 123456789u;

		return info;
	}
} // end anonymous namespace

void PrivilegeManagerMockObjectTestCases::testFakeStat()
{
	{
		PrivilegeManager pm(PrivilegeManager::getPrivilegeManager());
		unitAssertThrowsEx(pm.stat("/foo/bar"), PrivilegeManagerException);
	}

	{
		PrivilegeManagerMockObjectScope mos(Reference<PrivilegeManagerMockObject>(new MockStatter));

		PrivilegeManager pm(PrivilegeManager::getPrivilegeManager());

		FileSystem::FileInformation info;
		unitAssertNoThrow(info = pm.stat("/foo/bar"));

		unitAssertEquals(FileSystem::FileInformation::E_FILE_REGULAR, info.type);
		unitAssertEquals(123456789u, info.size);

		// Not implemented in this mock object.  Should throw.
		unitAssertThrowsEx( pm.readDirectory("/any/random/dir", PrivilegeManager::E_KEEP_SPECIAL),
			PrivilegeManagerException );
	}
}

Test* PrivilegeManagerMockObjectTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("PrivilegeManagerMockObject");

	ADD_TEST_TO_SUITE(PrivilegeManagerMockObjectTestCases, testManagerCreation);
	ADD_TEST_TO_SUITE(PrivilegeManagerMockObjectTestCases, testFakeStat);

	return testSuite;
}

