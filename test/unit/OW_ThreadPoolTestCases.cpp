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

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_ThreadPoolTestCases.hpp"
#include "OW_ThreadPool.hpp"
#include "OW_Thread.hpp"

using namespace OpenWBEM;

void OW_ThreadPoolTestCases::setUp()
{
}

void OW_ThreadPoolTestCases::tearDown()
{
}

namespace {

const int RUNNER_COUNT_MAX = 100000;

class testRunner : public Runnable
{
public:
	testRunner(int& i) : m_i(i) {}

	virtual void run()
	{
		for (int x = 0; x <= RUNNER_COUNT_MAX; ++x)
		{
			// yield every now and then to stir up the pot a bit
			if (!(x % 10000))
			{
				Thread::yield();
			}
			m_i = x;
		}
		
	}
	int& m_i;
};

} // end anonymous namespace

void OW_ThreadPoolTestCases::testThreadPool()
{
	// The pool has 10 threads, max queue of 20
	ThreadPool thePool(ThreadPool::FIXED_SIZE, 10, 20);
	const int NUM_RUNNERS = 100;
	int ints[NUM_RUNNERS];
	memset(ints, 0, NUM_RUNNERS * sizeof(int));
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		unitAssert(thePool.addWork(RunnableRef(new testRunner(ints[i]))));
	}

	// adding a null RunnableRef should fail
	unitAssert(!thePool.addWork(RunnableRef()));

	thePool.shutdown(ThreadPool::E_FINISH_WORK_IN_QUEUE);

	// after the pool is shutdown, addWork should fail
	unitAssert(!thePool.addWork(RunnableRef(new testRunner(ints[0]))));

	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		unitAssert(ints[i] == RUNNER_COUNT_MAX);
	}

}

void OW_ThreadPoolTestCases::testThreadPool2()
{
	// The pool has 10 threads, max queue of 20
	ThreadPool thePool(ThreadPool::FIXED_SIZE, 10, 20);
	const int NUM_RUNNERS = 100;
	int ints[NUM_RUNNERS];
	memset(ints, 0, NUM_RUNNERS * sizeof(int));
	// we'll try and stuff it as full as possible, but some shouldn't make it in.
	int ran = 0, didntRun = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		thePool.tryAddWork(RunnableRef(new testRunner(ints[i]))) ? ++ran : ++didntRun;
		if (!(i % 5))
			Thread::yield();
	}

	// adding a null RunnableRef should fail
	unitAssert(!thePool.tryAddWork(RunnableRef()));

	// let something happen...
	Thread::yield();

	thePool.shutdown(ThreadPool::E_FINISH_WORK_IN_QUEUE);

	// after the pool is shutdown, tryAddWork should fail
	unitAssert(!thePool.tryAddWork(RunnableRef(new testRunner(ints[0]))));

	int ran2 = 0, didntRun2 = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		if (ints[i] == RUNNER_COUNT_MAX)
		{
			++ran2;
		}
		else
		{
			++didntRun2;
		}
	}
	unitAssert(ran == ran2);
	unitAssert(didntRun == didntRun2);

}

void OW_ThreadPoolTestCases::testThreadPool3()
{
	// The pool has 10 threads, max queue of 20
	ThreadPool thePool(ThreadPool::FIXED_SIZE, 10, 20);
	const int NUM_RUNNERS = 100;
	int ints[NUM_RUNNERS];
	memset(ints, 0, NUM_RUNNERS * sizeof(int));
	// we'll try and stuff it as full as possible, but some shouldn't make it in.
	int ran = 0, didntRun = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		thePool.tryAddWork(RunnableRef(new testRunner(ints[i]))) ? ++ran : ++didntRun;
		if (!(i % 5))
			Thread::yield();
	}

	// adding a null RunnableRef should fail
	unitAssert(!thePool.tryAddWork(RunnableRef()));

	// let something happen...
	Thread::yield();

	// shutdown asap. But use 1 as the timeout so we don't need to definitively 
	// cancel any threads (which can cause problems)
	thePool.shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 1);

	// after the pool is shutdown, tryAddWork should fail
	unitAssert(!thePool.tryAddWork(RunnableRef(new testRunner(ints[0]))));

	int ran2 = 0, didntRun2 = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		if (ints[i] == RUNNER_COUNT_MAX)
		{
			++ran2;
		}
		else
		{
			++didntRun2;
		}
	}
	unitAssert(ran >= ran2);
	unitAssert(didntRun <= didntRun2);

}

void OW_ThreadPoolTestCases::testThreadPoolDynamic1()
{
	// The pool has 10 threads, max queue of 20
	ThreadPool thePool(ThreadPool::DYNAMIC_SIZE, 10, 20);
	const int NUM_RUNNERS = 100;
	int ints[NUM_RUNNERS];
	memset(ints, 0, NUM_RUNNERS * sizeof(int));
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		unitAssert(thePool.addWork(RunnableRef(new testRunner(ints[i]))));
	}

	// adding a null RunnableRef should fail
	unitAssert(!thePool.addWork(RunnableRef()));

	thePool.shutdown(ThreadPool::E_FINISH_WORK_IN_QUEUE);

	// after the pool is shutdown, addWork should fail
	unitAssert(!thePool.addWork(RunnableRef(new testRunner(ints[0]))));

	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		unitAssert(ints[i] == RUNNER_COUNT_MAX);
	}

}

void OW_ThreadPoolTestCases::testThreadPoolDynamic2()
{
	// The pool has 10 threads, max queue of 20
	ThreadPool thePool(ThreadPool::DYNAMIC_SIZE, 10, 20);
	const int NUM_RUNNERS = 100;
	int ints[NUM_RUNNERS];
	memset(ints, 0, NUM_RUNNERS * sizeof(int));
	// we'll try and stuff it as full as possible, but some shouldn't make it in.
	int ran = 0, didntRun = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		thePool.tryAddWork(RunnableRef(new testRunner(ints[i]))) ? ++ran : ++didntRun;
		if (!(i % 5))
			Thread::yield();
	}

	// adding a null RunnableRef should fail
	unitAssert(!thePool.tryAddWork(RunnableRef()));

	// let something happen...
	Thread::yield();

	thePool.shutdown(ThreadPool::E_FINISH_WORK_IN_QUEUE);

	// after the pool is shutdown, tryAddWork should fail
	unitAssert(!thePool.tryAddWork(RunnableRef(new testRunner(ints[0]))));

	int ran2 = 0, didntRun2 = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		if (ints[i] == RUNNER_COUNT_MAX)
		{
			++ran2;
		}
		else
		{
			++didntRun2;
		}
	}
	unitAssert(ran == ran2);
	unitAssert(didntRun == didntRun2);

}

void OW_ThreadPoolTestCases::testThreadPoolDynamic3()
{
	// The pool has 10 threads, max queue of 20
	ThreadPool thePool(ThreadPool::DYNAMIC_SIZE, 10, 20);
	const int NUM_RUNNERS = 100;
	int ints[NUM_RUNNERS];
	memset(ints, 0, NUM_RUNNERS * sizeof(int));
	// we'll try and stuff it as full as possible, but some shouldn't make it in.
	int ran = 0, didntRun = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		thePool.tryAddWork(RunnableRef(new testRunner(ints[i]))) ? ++ran : ++didntRun;
		if (!(i % 5))
			Thread::yield();
	}

	// adding a null RunnableRef should fail
	unitAssert(!thePool.tryAddWork(RunnableRef()));

	// let something happen...
	Thread::yield();

	// ditch the work still in the queue and shutdown asap.
	thePool.shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE);

	// after the pool is shutdown, tryAddWork should fail
	unitAssert(!thePool.tryAddWork(RunnableRef(new testRunner(ints[0]))));

	int ran2 = 0, didntRun2 = 0;
	for (int i = 0; i < NUM_RUNNERS; ++i)
	{
		if (ints[i] == RUNNER_COUNT_MAX)
		{
			++ran2;
		}
		else
		{
			++didntRun2;
		}
	}
	unitAssert(ran >= ran2);
	unitAssert(didntRun <= didntRun2);

}

Test* OW_ThreadPoolTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_ThreadPool");

	ADD_TEST_TO_SUITE(OW_ThreadPoolTestCases, testThreadPool);
	ADD_TEST_TO_SUITE(OW_ThreadPoolTestCases, testThreadPool2);
	ADD_TEST_TO_SUITE(OW_ThreadPoolTestCases, testThreadPool3);
	ADD_TEST_TO_SUITE(OW_ThreadPoolTestCases, testThreadPoolDynamic1);
	ADD_TEST_TO_SUITE(OW_ThreadPoolTestCases, testThreadPoolDynamic2);
	ADD_TEST_TO_SUITE(OW_ThreadPoolTestCases, testThreadPoolDynamic3);

	return testSuite;
}

