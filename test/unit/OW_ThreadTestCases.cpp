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
#include "OW_ThreadTestCases.hpp"
#include "OW_Thread.hpp"

#include <sys/types.h>
#include <unistd.h>


#include <iostream> 

using namespace std; 
using namespace OpenWBEM;

void OW_ThreadTestCases::setUp()
{
}

void OW_ThreadTestCases::tearDown()
{
}

namespace {

class testReturnThread : public Thread
{
public:
	testReturnThread()
		: Thread()
	{}

	Int32 run()
	{
		return 4321;
	}
};

} // end anonymous namespace

void OW_ThreadTestCases::testReturn()
{
	testReturnThread theThread;
	theThread.start();
	unitAssert(theThread.join() == 4321);
}

namespace {

bool cancelCleanedUp = false;
struct shouldRunOnException
{
	~shouldRunOnException() { cancelCleanedUp = true; }
	Int32 getRV() { return 0; } // this is just so we can use the variable down below and avoid an unused variable warning.
};

class testCancellationThread1 : public Thread
{
public:
	testCancellationThread1()
		: Thread()
		, m_cooperativeCancelCalled(false)
		, m_definitiveCancelCalled(false)
	{
		cancelCleanedUp = false;
	}

	Int32 run()
	{
		shouldRunOnException x;
		// just wait a long time for us to get cancelled.
		while (true)
		{
			testCancel();
			Thread::yield();
		}
		return x.getRV();
	}

	virtual void doCooperativeCancel()
	{
		m_cooperativeCancelCalled = true;
	}

	bool m_cooperativeCancelCalled;

	virtual void doDefinitiveCancel()
	{
		m_definitiveCancelCalled = true;
	}

	bool m_definitiveCancelCalled;
};

class testCancellationThread2 : public Thread
{
public:
	testCancellationThread2()
		: Thread()
		, m_cooperativeCancelCalled(false)
		, m_definitiveCancelCalled(false)
	{
		cancelCleanedUp = false;
	}

	Int32 run()
	{
		shouldRunOnException x;
		// just wait a long time for us to get cancelled, and make sure
		// we'll get actually cancelled by pthreads by catching all
		// exceptions.
		while (true)
		{
			try
			{
				Thread::yield();
			}
			catch (...) {}
		}
		return x.getRV();
	}

	virtual void doCooperativeCancel()
	{
		m_cooperativeCancelCalled = true;
	}

	bool m_cooperativeCancelCalled;

	virtual void doDefinitiveCancel()
	{
		m_definitiveCancelCalled = true;
	}

	bool m_definitiveCancelCalled;
};

// here's a thread that won't allow definitive cancellation.
class testCancellationThread3 : public Thread
{
public:
	testCancellationThread3()
		: Thread()
		, m_cooperativeCancelCalled(false)
		, m_firstTime(true)
	{
		cancelCleanedUp = false;
	}

	Int32 run()
	{
		shouldRunOnException x;
		// just wait a long time for us to get cancelled.
		while (true)
		{
			Thread::yield();
		}
		return x.getRV();
	}

	virtual void doCooperativeCancel()
	{
		m_cooperativeCancelCalled = true;
	}

	bool m_cooperativeCancelCalled;

	virtual void doDefinitiveCancel()
	{
		if (m_firstTime)
		{
			m_firstTime = false;
			OW_THROW(CancellationDeniedException, "test");
		}
		else
			return;
	}

	bool m_firstTime;
};

} // end anonymous namespace

void OW_ThreadTestCases::testCooperativeCancellation()
{
	testCancellationThread1 theThread;
	unitAssert(!cancelCleanedUp);
	theThread.start();
	theThread.cooperativeCancel();
	unitAssertNoThrow(theThread.cooperativeCancel());
	theThread.join();
	unitAssert(!theThread.isRunning());
	unitAssert(cancelCleanedUp);
	unitAssert(theThread.m_cooperativeCancelCalled == true);
	unitAssert(theThread.m_definitiveCancelCalled == false);
}

void OW_ThreadTestCases::testDefinitiveCancellation()
{
	// first a thread that calls testCancel()
	testCancellationThread1 theThread1;
	unitAssert(!cancelCleanedUp);
	theThread1.start();
	unitAssert(theThread1.definitiveCancel() == true);
	unitAssertNoThrow(theThread1.cooperativeCancel());
	unitAssertNoThrow(theThread1.definitiveCancel());
	theThread1.join();
	unitAssert(!theThread1.isRunning());
	unitAssert(cancelCleanedUp);
	unitAssert(theThread1.m_cooperativeCancelCalled == true);
	unitAssert(theThread1.m_definitiveCancelCalled == false);

	// this breaks on RH, doing cancellation causes a segfault :(
#if 0
	// now a thread that is bad and doesn't call testCancel().
	testCancellationThread2 theThread2;
	unitAssert(!cancelCleanedUp);
	theThread2.start();
	// wait 0 seconds for cooperative cancellation to finish,
	// since we know it won't ever finish.
	unitAssert(theThread2.definitiveCancel(0) == false);
	unitAssertNoThrow(theThread2.cooperativeCancel());
	unitAssertNoThrow(theThread2.definitiveCancel());
	theThread2.join();
	unitAssert(!theThread2.isRunning());
	unitAssert(!cancelCleanedUp);
	unitAssert(theThread2.m_cooperativeCancelCalled == true);
	unitAssert(theThread2.m_definitiveCancelCalled == true);
#endif

	// this breaks on RH, doing cancellation causes a segfault :(
#if 0
	// now a thread that denies the first definitive cancellation request.
	testCancellationThread3 theThread3;
	unitAssert(!cancelCleanedUp);
	theThread3.start();
	// first time, we'll get an CancellationDeniedException
	unitAssertThrows(theThread3.definitiveCancel(0));

	// second time it'll allow itself to be cancelled.
	// wait 0 seconds for cooperative cancellation to finish,
	// since we know it won't ever finish.
	unitAssert(theThread3.definitiveCancel(0) == false);
	unitAssertNoThrow(theThread3.cooperativeCancel());
	unitAssertNoThrow(theThread3.definitiveCancel());
	theThread3.join();
	unitAssert(!theThread3.isRunning());
	unitAssert(!cancelCleanedUp);
	unitAssert(theThread3.m_cooperativeCancelCalled == true);
#endif

}

namespace {


class SetUIDThread : public Thread
{
private: 
	int _threadNum; 
	uid_t _uids[2]; 
public:
	SetUIDThread(int arg)
		: _threadNum(arg)
	{
		_uids[0] = 500;  // change to a real UID 
		_uids[1] = 501;  // change to a real UID
	}
	Int32 run()
	{
		setuid(_uids[_threadNum]); 
		if (_threadNum == 0)
		{
			Thread::sleep(1000); 
		}
		else
		{
			Thread::sleep(2000); 
		}
		for (int i = 0; i < 3; ++i)
		{
			cout << "Thread" << _threadNum << ": " << getuid() << endl;
			Thread::sleep(3000); 
		}
		return 0; 
	}

};
} // end anonymous namespace

void OW_ThreadTestCases::testSetUID()
{
	SetUIDThread t0(0); 
	SetUIDThread t1(1); 
	t0.start(); 
	t1.start(); 
	for (int i = 0; i < 4; ++i)
	{
		Thread::sleep(3000); 
		cout << "Parent: " << getuid() << endl;
	}
}

Test* OW_ThreadTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Thread");

	// Leave this one out by default, since the class 
	// has to be configured above with real UIDs on the
	// system, and run as root
	//ADD_TEST_TO_SUITE(OW_ThreadTestCases, testSetUID);
	ADD_TEST_TO_SUITE(OW_ThreadTestCases, testReturn);
	ADD_TEST_TO_SUITE(OW_ThreadTestCases, testCooperativeCancellation);
	ADD_TEST_TO_SUITE(OW_ThreadTestCases, testDefinitiveCancellation);

	return testSuite;
}

