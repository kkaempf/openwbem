/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "OW_MutexTestCases.hpp"
#include "OW_Mutex.hpp"
#include "OW_Thread.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::cerr;

void OW_MutexTestCases::setUp()
{
}

void OW_MutexTestCases::tearDown()
{
}

class testClass
{
public:
	static OW_Mutex m;
};

static OW_Mutex g_mutex;
static int g_int;

class myRunnable: public OW_Runnable
{
protected:
	void run()
	{
		try
		{
			cout << "\nIn run" << endl;
			g_mutex.acquire();
			g_int = 2;
			g_mutex.release();
		}
		catch(OW_Exception& e)
		{
			cout << e << endl;
		}
		catch(...)
		{
			cout << "Unknown exception in myRunnable::run" << endl;
		}
	}
};

OW_Mutex testClass::m;

void OW_MutexTestCases::testAcquireRelease()
{
	OW_Mutex m;

	m.acquire();
	m.release();

	// defaults to recursive mutex, so test that
	m.acquire();
	m.acquire();
	m.release();
	m.release();

	// try a static class mutex
	testClass::m.acquire();
	testClass::m.release();

	// test with multiple threads
	try
	{
		g_int = 1;
		g_mutex.acquire();
		OW_RunnableRef rref(new myRunnable);
		OW_Thread::run(rref);
		//OW_Thread::sleep(1000 * 60 * 10);
		OW_Thread::sleep(100);
		unitAssert(g_int == 1);
		g_mutex.release();
		OW_Thread::sleep(100);
		unitAssert(g_int == 2);
	}
	catch(OW_Exception& e)
	{
		cout << e << endl;
		unitAssert(0);
	}
	catch(...)
	{
		cout << "Unknown exception in OW_MutexTestCases::testAcquireRelease"
			<< endl;
		unitAssert(0);
	}
}

Test* OW_MutexTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Mutex");

	testSuite->addTest (new TestCaller <OW_MutexTestCases>
			("testAcquireRelease",
			&OW_MutexTestCases::testAcquireRelease));

	return testSuite;
}

