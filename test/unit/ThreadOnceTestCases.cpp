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
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "ThreadOnceTestCases.hpp"
#include "OW_ThreadOnce.hpp"
#include "OW_ThreadPool.hpp"
#include "OW_Runnable.hpp"

using namespace OpenWBEM;

void ThreadOnceTestCases::setUp()
{
}

void ThreadOnceTestCases::tearDown()
{
}

int g_value = 0;
OnceFlag once = OW_ONCE_INIT;


void initOnceValue()
{
    g_value++;
}

class OnceThread : public Runnable
{
	void run()
	{
		callOnce(once, initOnceValue);
	}
};

void ThreadOnceTestCases::testOnce()
{
    const int NUM_THREADS = 10;
    ThreadPool pool(ThreadPool::FIXED_SIZE, NUM_THREADS, ThreadPool::UNLIMITED_QUEUE_SIZE);
    for (int i = 0; i < NUM_THREADS; ++i)
	{
        pool.addWork(new OnceThread());
	}

    pool.shutdown();
	unitAssert(g_value == 1);

}

Test* ThreadOnceTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("ThreadOnce");

	ADD_TEST_TO_SUITE(ThreadOnceTestCases, testOnce);

	return testSuite;
}

