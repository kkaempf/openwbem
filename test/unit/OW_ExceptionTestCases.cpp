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
#include "OW_ExceptionTestCases.hpp"
#include "OW_Semaphore.hpp"
#include "OW_Exception.hpp"
#include "OW_Runnable.hpp"

DECLARE_EXCEPTION(Test);
DEFINE_EXCEPTION(Test);

void OW_ExceptionTestCases::setUp()
{
}

void OW_ExceptionTestCases::tearDown()
{
}

static OW_Semaphore g_sem;
static bool g_caught = false;

class ExTestRunnable: public OW_Runnable
{
protected:
	void run()
	{
		try
		{
			OW_THROW(OW_TestException, "test");
		}
		catch(OW_Exception& e)
		{
			g_caught = true;
			g_sem.signal();
		}
	}
};

void OW_ExceptionTestCases::testSomething()
{
	g_caught = false;
	try
	{
		OW_THROW(OW_TestException, "test");
	}
	catch(OW_TestException& e)
	{
		g_caught = true;
	}
	unitAssert(g_caught);

	g_caught = false;
	OW_RunnableRef rref(new ExTestRunnable);
	OW_Runnable::run(rref);
	unitAssert(g_sem.timedWait(30));

	unitAssert(g_caught);
}

Test* OW_ExceptionTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Exception");

	testSuite->addTest (new TestCaller <OW_ExceptionTestCases>
			("testSomething",
			&OW_ExceptionTestCases::testSomething));

	return testSuite;
}

