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
#include "OW_RWLockerTestCases.hpp"
#include "OW_RWLocker.hpp"
#include "OW_Thread.hpp"
#include "OW_ThreadBarrier.hpp"
#include "OW_TimeoutException.hpp"

using namespace OpenWBEM;

void OW_RWLockerTestCases::setUp()
{
}

void OW_RWLockerTestCases::tearDown()
{
}

void OW_RWLockerTestCases::testDeadlock()
{
	RWLocker locker;
	locker.getReadLock(100);
	try
	{
		// this should throw
		locker.getWriteLock(100);
		unitAssert(0);
	}
	catch (const DeadlockException& e)
	{
	}
	locker.releaseReadLock();

	locker.getWriteLock(100);
	try
	{
		// this should throw
		locker.getReadLock(100);
		unitAssert(0);
	}
	catch (const DeadlockException& e)
	{
	}

	locker.releaseWriteLock();
}

class testThread : public Thread
{
public:
	testThread(RWLocker* locker, const ThreadBarrier& bar)
		: Thread()
		, m_locker(locker)
		, m_bar(bar)
	{
	}

protected:
	virtual Int32 run() 
	{
		m_locker->getReadLock(0);
		m_bar.wait();
		m_bar.wait();
		m_locker->releaseReadLock();

		m_locker->getWriteLock(0);
		m_bar.wait();
		m_bar.wait();
		m_locker->releaseWriteLock();
		return 0;
	}

	RWLocker* m_locker;
	ThreadBarrier m_bar;
};

void OW_RWLockerTestCases::testTimeout()
{
	RWLocker locker;
	ThreadBarrier bar(2);
	testThread t1(&locker, bar);
	t1.start();
	bar.wait(); // wait for the thread to start. It's already got the read lock, and will keep it until the barrier is hit again.
	
	// now try to get a write lock.  But we'll use a short timeout to make sure an exception is thrown.
	try
	{
		locker.getWriteLock(0,1);
		unitAssert(0);
	}
	catch (const TimeoutException& e)
	{
	}

	bar.wait(); // signal the thread to release the read lock and get a write lock.
	bar.wait(); // wait for the thread to release the read lock and get a write lock.

	// now try to get a read lock.  But we'll use a short timeout to make sure an exception is thrown.
	try
	{
		locker.getReadLock(0,1);
		unitAssert(0);
	}
	catch (const TimeoutException& e)
	{
	}

	bar.wait();

	t1.join();
}

Test* OW_RWLockerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_RWLocker");

	ADD_TEST_TO_SUITE(OW_RWLockerTestCases, testDeadlock);
	ADD_TEST_TO_SUITE(OW_RWLockerTestCases, testTimeout);

	return testSuite;
}

