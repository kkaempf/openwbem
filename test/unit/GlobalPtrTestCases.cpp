/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "GlobalPtrTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(GlobalPtrTestCases,"GlobalPtr");
#include "OW_GlobalPtr.hpp"

using namespace OpenWBEM;

class GlobalPtrTestClass
{
};

void GlobalPtrTestCases::setUp()
{
}

void GlobalPtrTestCases::tearDown()
{
}

void GlobalPtrTestCases::testGet()
{
	typedef GlobalPtr<GlobalPtrTestClass> G;
	G p = OW_GLOBAL_PTR_INIT;
	GlobalPtrTestClass* p1 = p.get();
	GlobalPtrTestClass* p2 = p.get();
	unitAssertNotEquals( static_cast<GlobalPtrTestClass*>(0), p1 );
	unitAssertNotEquals( static_cast<GlobalPtrTestClass*>(0), p2 );
	unitAssertEquals( p1, p2 );
}

void GlobalPtrTestCases::testSet()
{
	typedef GlobalPtr<GlobalPtrTestClass> G;
	G p = OW_GLOBAL_PTR_INIT;
	GlobalPtrTestClass i;
	GlobalPtrTestClass* p2 = p.set(&i);
	unitAssertNotEquals( static_cast<GlobalPtrTestClass*>(0), p2 );
	GlobalPtrTestClass* p3 = p.set(p2);
	unitAssertEquals( &i, p3 );
	GlobalPtrTestClass* p4 = p.get();
	unitAssertEquals( p2, p4 );
}

struct GlobalPtrTestClass2
{
	GlobalPtrTestClass2(int i)
	: m_i(i)
	{}
	int m_i;
};

struct GlobalPtrTestClass2Factory
{
	static GlobalPtrTestClass2* create()
	{
		return new GlobalPtrTestClass2(1);
	}
};

void GlobalPtrTestCases::testFactory()
{
	typedef GlobalPtr<GlobalPtrTestClass2, GlobalPtrTestClass2Factory> G;
	G p = OW_GLOBAL_PTR_INIT;
	unitAssertNotEquals( static_cast<GlobalPtrTestClass2*>(0), p.get() );
	unitAssertEquals( 1, p.get()->m_i );
}

Test* GlobalPtrTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("GlobalPtr");

	ADD_TEST_TO_SUITE(GlobalPtrTestCases, testGet);
	ADD_TEST_TO_SUITE(GlobalPtrTestCases, testSet);
	ADD_TEST_TO_SUITE(GlobalPtrTestCases, testFactory);

	return testSuite;
}

