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
#include "OW_ExceptionTestCases.hpp"
#include "OW_Semaphore.hpp"
#include "OW_Exception.hpp"
#include "OW_Thread.hpp"

using namespace OpenWBEM;

OW_DECLARE_EXCEPTION(Test);
OW_DEFINE_EXCEPTION(Test);

void OW_ExceptionTestCases::setUp()
{
}

void OW_ExceptionTestCases::tearDown()
{
}

static Semaphore g_sem;
static bool g_caught = false;

namespace {
class ExTestRunnable: public Thread
{
protected:
	Int32 run()
	{
		try
		{
			OW_THROW(TestException, "test");
		}
		catch(Exception& e)
		{
			g_caught = true;
			g_sem.signal();
		}
		return 0;
	}
};
} // end anonymous namespace

void OW_ExceptionTestCases::testThreadThrow()
{
	g_caught = false;
	try
	{
		OW_THROW(TestException, "test");
	}
	catch(TestException& e)
	{
		g_caught = true;
	}
	unitAssert(g_caught);

	g_caught = false;
	ExTestRunnable t1;
	t1.start();
	unitAssert(g_sem.timedWait(30));

	unitAssert(g_caught);
}

namespace 
{
OW_DECLARE_EXCEPTION(test1);
OW_DEFINE_EXCEPTION(test1);
OW_DECLARE_EXCEPTION(test2);
OW_DEFINE_EXCEPTION(test2);
}

void OW_ExceptionTestCases::testSubException()
{
	try
	{
		try
		{
			OW_THROW_ERR(test1Exception, "message 1", 1);
		}
		catch (const test1Exception& e)
		{
			OW_THROW_ERR_SUBEX(test2Exception, "message 2", 2, e);
		}
		unitAssert(false);
	}
	catch (const test2Exception& e2)
	{
		unitAssert(e2.getFile() != 0);
		unitAssert(e2.getLine() != 0);
		unitAssert(e2.getMessage() == String("message 2"));
		unitAssert(e2.getErrorCode() == 2);
		unitAssert(e2.type() == String("test2Exception"));
		unitAssert(e2.getSubClassId() == Exception::UNKNOWN_SUBCLASS_ID);
		unitAssert(e2.what() == String("message 2"));
		unitAssert(e2.getSubException() != 0);

		unitAssert(e2.getSubException()->getFile() != 0);
		unitAssert(e2.getSubException()->getLine() != 0);
		unitAssert(e2.getSubException()->getMessage() == String("message 1"));
		unitAssert(e2.getSubException()->getErrorCode() == 1);
		unitAssert(e2.getSubException()->type() == String("test1Exception"));
		unitAssert(e2.getSubException()->getSubClassId() == Exception::UNKNOWN_SUBCLASS_ID);
		unitAssert(e2.getSubException()->what() == String("message 1"));
		unitAssert(e2.getSubException()->getSubException() == 0);
	}

}

Test* OW_ExceptionTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Exception");

	ADD_TEST_TO_SUITE(OW_ExceptionTestCases, testThreadThrow);
	ADD_TEST_TO_SUITE(OW_ExceptionTestCases, testSubException);
	

	return testSuite;
}

