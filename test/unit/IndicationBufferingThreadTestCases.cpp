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
#undef NDEBUG
#include <cassert>

#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "IndicationBufferingThreadTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(IndicationBufferingThreadTestCases,"IndicationBufferingThread");
#include "MockObject.hpp"
#include "MockClock.hpp"
#include "CIMInstanceUtils.hpp"

#include "OW_IndicationBufferingThread.hpp"
#include "blocxx/Reference.hpp"
#include "blocxx/Format.hpp"

#include <ctime>

#define DEBUGIT 1
#if DEBUGIT
#include <iostream>
#endif

using namespace std;
using namespace OpenWBEM;
using namespace blocxx;
using namespace blocxx::MTQueueEnum;
using namespace CIMInstanceUtils;
typedef IndicationBufferingThread::ExportIndicationArgs ExportIndicationArgs;
typedef IndicationBufferingThread::Config Config;

namespace
{

CIMProperty handler1Props[] = {	cimProp("Destination", "owipc://foo") };
CIMInstance const handler1 =
	cimInst("CIM_IndicationHandlerXMLHTTP", handler1Props);

CIMProperty handler2Props[] = {	cimProp("Destination", "https://bar/baz") };
CIMInstance const handler2 =
	cimInst("CIM_IndicationHandlerXMLHTTPS", handler2Props);

CIMProperty handler3Props[] = { cimProp("Destination", "http://hoo:37") };
CIMInstance const handler3 =
	cimInst("CIM_IndicationHandlerCIMXML", handler3Props);

CIMProperty handler4Props[] = { cimProp("Destination", "http://arg:2884/a") };
CIMInstance const handler4 =
	cimInst("CIM_ListenerDestinationCIMXML", handler4Props);

CIMProperty handler4aProps[] = {
	cimProp("Destination", "http://arg:2884/a"), cimProp("Foo", 27) };
CIMInstance const handler4a =
	cimInst("cim_listenerdestinationcimxml", handler4aProps);

CIMProperty badHandlerProps[] = { cimProp("Foo", "bar") };
CIMInstance const badHandler =
	cimInst("CIM_ListenerDestinationCIMXML", badHandlerProps);


CIMProperty ind1Props[] = {	cimProp("ooga", "booga"), cimProp("fimbl", 2783) };
CIMInstance const ind1 = cimInst("My_Foo", ind1Props);

CIMProperty ind2Props[] = { cimProp("eeny", "meeny") };
CIMInstance const ind2 = cimInst("Your_Bar", ind2Props);

CIMProperty ind3Props[] = {
	cimProp("a", "bee"), cimProp("See", 0xD), cimProp("ee", "eff")
};
CIMInstance const ind3 = cimInst("Alpha_Bet", ind3Props);

CIMProperty ind4Props[] = { cimProp("heeby", "jeebies") };
CIMInstance const ind4 = cimInst("Hrunkle", ind4Props);


std::time_t const T0 = 1234567890; // arbitrary


template <typename T> Array<T> arr(T const & x1)
{
	Array<T> a;
	a.push_back(x1);
	return a;
}

template <typename T> Array<T> arr(T x1, T x2)
{
	Array<T> a;
	a.push_back(x1);
	a.push_back(x2);
	return a;
}

template <typename T> Array<T> arr(T x1, T x2, T x3)
{
	Array<T> a;
	a.push_back(x1);
	a.push_back(x2);
	a.push_back(x3);
	return a;
}

struct Action : public IntrusiveCountableBase
{
	virtual ~Action()
	{
	}

	virtual void doAction() = 0;
};

typedef IntrusiveReference<Action> ActionRef;

struct DoNothing : public Action
{
	virtual void doAction()
	{
	}
};

template <typename T>
struct ThrowException : public Action
{
	ThrowException(T const & e)
	: m_exception(e)
	{
	}

	virtual void doAction()
	{
		throw m_exception;
	}

private:
	T m_exception;
};

DateTime dt(time_t sec, UInt32 microsec)
{
	return DateTime(sec, microsec);
}

struct MockIndicationBurstExporter :
	public IndicationBurstExporter, public MockObject
{
	MockIndicationBurstExporter()
	: m_initializeCalled(false),
	  m_shutdownCalled(false),
	  m_callIndex(0),
	  m_stepIndex(0)
	{
		// 0 calls to sendBurst expected before first step starts
		m_totalCallsExpectedByStepEnd.push_back(0);
	}

	virtual ~MockIndicationBurstExporter()
	{
	}

	virtual void initialize(UInt32 maxNumIoThreads)
	{
		mockAssert(!m_initializeCalled,
			"MockIndicationBurstExporter::initialize: called twice");
		mockAssert(maxNumIoThreads == m_expectedMaxNumIoThreads,
			"MockIndicationBurstExporter::initialize: "
			"wrong maxNumIoThreads value");
		m_initializeCalled = true;
	}

	virtual void shutdown()
	{
		mockAssert(m_initializeCalled,
			"MockIndicationBurstExporter::shutdown: called before initialize");
		mockAssert(!m_shutdownCalled,
			"MockIndicationBurstExporter::shutdown: called twice");
		mockAssert(m_stepIndex + 1 == m_totalCallsExpectedByStepEnd.size(),
			"MockIndicationBurstExporter::shutdown: called too soon; "
			"more sendBurst steps expected first");
		mockAssert(m_callIndex == m_totalCallsExpectedByStepEnd[m_stepIndex],
			"MockIndicationBurstExporter::shutdown: "
			"called too soon; more sendBurst calls expected "
			"in the last step");
		m_shutdownCalled = true;
	}

	virtual void sendBurst(
		CIMInstance const & handler, CIMInstanceArray const & indications)
	{
		size_t callIndex = m_callIndex++;
		checkInvariants();
		mockAssert(m_initializeCalled,
			"MockIndicationBurstExporter::sendBurst called before initialize");
		mockAssert(!m_shutdownCalled,
			"MockIndicationBurstExporter::sendBurst called after shutdown");
		mockAssert(callIndex < m_totalCallsExpectedByStepEnd[m_stepIndex],
			"MockIndicationBurstExporter::sendBurst: "
			"too many calls in the current step");
		mockAssert(basicEqual(handler, m_handlerArgs[callIndex]),
			"MockIndicationBurstExporter::sendBurst: wrong handler arg value");
		mockAssert(basicEqual(indications, m_indicationsArgs[callIndex]),
			"MockIndicationBurstExporter::sendBurst: "
			"wrong indications arg value");
		m_actions[callIndex]->doAction();
	}

	void expectMaxNumIoThreadsInitializedTo(UInt32 maxNumIoThreads)
	{
		m_expectedMaxNumIoThreads = maxNumIoThreads;
	}

	void startStep()
	{
		checkInvariants();
		mockAssert(m_stepIndex + 1 < m_totalCallsExpectedByStepEnd.size(),
			"MockIndicationBurstExporter::startStep: "
			"too many sendBurst steps");
		mockAssert(m_callIndex == m_totalCallsExpectedByStepEnd[m_stepIndex],
			"MockIndicationBurstExporter::startStep: "
			"wrong # of sendBurst calls in previous step");
		++m_stepIndex;
	}

	// REQUIRE: If you call this method, call it BEFORE the corresponding
	// expectSendBurst call.
	//
	template <typename T>
	void givenThrowsException(T const & ex)
	{
		m_actions.push_back(new ThrowException<T>(ex));
	}

	void expectSendBurst(
		CIMInstance const & handler, CIMInstanceArray const & indications)
	{
		m_handlerArgs.push_back(handler);
		m_indicationsArgs.push_back(indications);

		// If not specified to throw an exception, this implicitly
		// specifies to NOT throw an exception
		std::size_t nh = m_handlerArgs.size();
		std::size_t na = m_actions.size();
		assert(na <= nh && nh <= na + 1);
		if (na < nh)
		{
			m_actions.push_back(new DoNothing());
		}
	}

	void endStepSpecification()
	{
		std::size_t nh = m_handlerArgs.size();
		assert(nh == m_indicationsArgs.size());
		m_totalCallsExpectedByStepEnd.push_back(nh);
	}

private:
	virtual void doVerify()
	{
		mockAssert(m_shutdownCalled,
			"MockIndicationBurstExporter::shutdown not called");
	}

	void checkInvariants()
	{
		// Just checking for errors in implementing the mock object itself.
		assert(m_handlerArgs.size() == m_indicationsArgs.size());
		assert(m_handlerArgs.size() == m_actions.size());
		size_t numCallsExpected = m_handlerArgs.size();
		std::size_t numStepsExpectedPlus1 =
			m_totalCallsExpectedByStepEnd.size();
		assert(numStepsExpectedPlus1 > 0);
		std::size_t numStepsExpected = numStepsExpectedPlus1 - 1;
		assert(m_totalCallsExpectedByStepEnd[numStepsExpected]
			== numCallsExpected);
	}

	bool m_initializeCalled;
	bool m_shutdownCalled;
	UInt32 m_expectedMaxNumIoThreads;
	std::vector<CIMInstance> m_handlerArgs;
	std::vector<CIMInstanceArray> m_indicationsArgs;
	std::vector<ActionRef> m_actions;
	std::vector<std::size_t> m_totalCallsExpectedByStepEnd;
	std::size_t m_callIndex;
	std::size_t m_stepIndex; // 1 is first step
};

typedef IntrusiveReference<MockIndicationBurstExporter>
	MockIndicationBurstExporterRef;

struct MockMTSource :
	public MTSourceIfc<ExportIndicationArgs>, public MockObject
{
	MockMTSource(
		MockClockRef clock, MockIndicationBurstExporterRef burstExporter)
	: m_call_index(0),
	  m_value_index(0),
	  m_clock(clock),
	  m_burstExporter(burstExporter)
	{
	}

	void givenPopFrontReturnsTimedOutAt(DateTime when)
	{
		m_results.push_back(E_TIMED_OUT);
		m_returnTimes.push_back(when);
	}

	void givenPopFrontReturnsShutDownAt(DateTime when)
	{
		m_results.push_back(E_SHUT_DOWN);
		m_returnTimes.push_back(when);
	}

	void givenPopFrontReturnsValueAt(
		DateTime when, ExportIndicationArgs const & value)
	{
		m_results.push_back(E_VALUE);
		m_returnTimes.push_back(when);
		m_values.push_back(value);
	}

	void expectPopFrontCalledWith(Timeout const & timeout)
	{
		m_timeouts.push_back(timeout);
	}

	virtual ~MockMTSource()
	{
	}

	static String toString(Timeout const & timeout)
	{
		switch (timeout.getType())
		{
		case Timeout::E_RELATIVE:
			return "<relative>";
		case Timeout::E_RELATIVE_WITH_RESET:
			return "<relative with reset>";
		case Timeout::E_ABSOLUTE:
			{
				DateTime dt(timeout.getAbsolute());
				Format fmt("absolute(%1, %2)", dt.get(), dt.getMicrosecond());
				return fmt.toString();
			}
		default:
			return "<unknown>";
		}
	}

	// REQUIRE: Total # of calls to given*() methods must equal number of
	// calls to expectPopFrontCalledWith().
	//
	virtual EPopResult popFront(
		Timeout const & timeout, ExportIndicationArgs & value)
	{
		assert(m_timeouts.size() == m_results.size());
		mockAssert(m_call_index < m_timeouts.size(),
			"Unexpected call to MockMTSource::popFront()");
		Format timeout_msg(
			"Unexpected timeout arg in MockMTSource::popFront() call: "
			"expected %1, got %2",
			toString(m_timeouts[m_call_index]),
			toString(timeout));
		mockAssert(timeout == m_timeouts[m_call_index],
			timeout_msg.c_str());

		m_clock->setTime(m_returnTimes[m_call_index]);
		m_burstExporter->startStep();

		EPopResult result = m_results[m_call_index];
		if (result == E_VALUE)
		{
			value = m_values[m_value_index];
			++m_value_index;
		}

		++m_call_index;
		return result;
	}

private:
	virtual void doVerify()
	{
		mockAssert(m_call_index == m_timeouts.size(),
			"Missing calls to MockMTSource::popFront()");
		assert(m_value_index == m_values.size());
	}

	std::vector<Timeout> m_timeouts;
	std::vector<EPopResult> m_results;
	std::vector<DateTime> m_returnTimes;
	std::vector<ExportIndicationArgs> m_values;
	std::size_t m_call_index;
	std::size_t m_value_index;
	MockClockRef m_clock;
	MockIndicationBurstExporterRef m_burstExporter;
};

struct TC : public TestCase
{
	TC( const char* name )
	: TestCase( name )
	{
	}

	void setUp()
	{
		m_clock = new MockClock();
		m_burstExporter = new MockIndicationBurstExporter();
		m_requestSource = new MockMTSource(m_clock, m_burstExporter);
		m_bufferingThread =
			new IndicationBufferingThread(m_burstExporter, m_clock);
	}

	void tearDown()
	{
		m_bufferingThread = 0;
		m_requestSource = 0;
		m_burstExporter = 0;
		m_clock = 0;
	}

	TC & wait(Real32 w)
	{
		m_bufferingWaitSeconds = w;
		return *this;
	}

	TC & delay(Real32 d)
	{
		m_maxBufferingDelaySeconds = d;
		return *this;
	}

	TC & bufsz(UInt32 n)
	{
		m_maxBufferSize = n;
		return *this;
	}

	TC & dests(UInt32 n)
	{
		m_maxBufferedDestinations = n;
		return *this;
	}

	TC & thrds(UInt32 n)
	{
		m_maxNumIoThreads = n;
		return *this;
	}

	void init()
	{
		Config cfg;
		cfg.bufferingWaitSeconds = m_bufferingWaitSeconds;
		cfg.maxBufferingDelaySeconds = m_maxBufferingDelaySeconds;
		cfg.maxBufferSize = m_maxBufferSize;
		cfg.maxBufferedDestinations = m_maxBufferedDestinations;
		cfg.maxNumIoThreads = m_maxNumIoThreads;

		m_burstExporter
			->expectMaxNumIoThreadsInitializedTo(cfg.maxNumIoThreads);

		m_bufferingThread->initialize(cfg, *m_requestSource);
	}

	void givenTimeIs(DateTime t)
	{
		m_clock->setTime(t);

	}

	void givenPopFrontReturnsShutDownAt(DateTime t)
	{
		m_requestSource->givenPopFrontReturnsShutDownAt(t);
	}

	void givenPopFrontReturnsTimedOutAt(DateTime t)
	{
		m_requestSource->givenPopFrontReturnsTimedOutAt(t);
	}

	void givenPopFrontReturnsValueAt(
		DateTime when,
		CIMInstance const & handler, CIMInstance const & indication)
	{
		ExportIndicationArgs value(handler, indication);
		m_requestSource->givenPopFrontReturnsValueAt(when, value);
	}

	void expectPopFrontCalledWith(Timeout const & timeout)
	{
		m_requestSource->expectPopFrontCalledWith(timeout);
	}

	void expectPopFrontCalledWithTimeoutOf(DateTime when)
	{
		expectPopFrontCalledWith(Timeout::absolute(when));
	}

	void expectSendBurst(
		CIMInstance const & handler, CIMInstanceArray const & indications)
	{
		m_burstExporter->expectSendBurst(handler, indications);
	}

	template <typename T>
	void givenSendBurstThrows(T const & ex)
	{
		m_burstExporter->givenThrowsException(ex);
	}

	void endStepSpecification()
	{
		m_burstExporter->endStepSpecification();
	}

	void runThread(Int32 expectedReturnValue = 0)
	{
		Int32 rv = m_bufferingThread->run();
		unitAssertEquals(expectedReturnValue, rv);
	}

	void verify()
	{
		m_burstExporter->verify();
		m_requestSource->verify();
	}

	void processShutdown(DateTime when, DateTime timeout)
	{
		expectPopFrontCalledWith(Timeout::absolute(timeout));
		givenPopFrontReturnsShutDownAt(when);
		endStepSpecification();
	}

	void processesIsolatedShutdown(DateTime when)
	{
		expectPopFrontCalledWith(Timeout::infinite);
		givenPopFrontReturnsShutDownAt(when);
		endStepSpecification();
	}

	void testNoIndicationsReceived()
	{
		givenTimeIs(
			dt(T0, 0));
		wait(0.1).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		processesIsolatedShutdown(
			dt(T0+1000, 0));

		runThread();
		verify();
	}

	void firstIndicationArrives(
		DateTime when, CIMInstance handler, CIMInstance indication)
	{
		expectPopFrontCalledWith(Timeout::infinite);
		givenPopFrontReturnsValueAt(when, handler, indication);
		endStepSpecification();
	}

	void indicationArrives(
		DateTime whenArrives, DateTime timeoutArg,
		CIMInstance handler, CIMInstance indication)
	{
		expectPopFrontCalledWithTimeoutOf(timeoutArg);
		givenPopFrontReturnsValueAt(whenArrives, handler, indication);
		endStepSpecification();
	}

	void timesOutAndSendsBurst(
		DateTime whenTimedOut, DateTime timeoutArg,
		CIMInstance handler, CIMInstanceArray indications)
	{
		expectPopFrontCalledWithTimeoutOf(timeoutArg);
		givenPopFrontReturnsTimedOutAt(whenTimedOut);
		expectSendBurst(handler, indications);
		endStepSpecification();
	}

	void timesOutAndSendsBursts(
		DateTime whenTimedOut, DateTime timeoutArg,
		CIMInstance handler1, CIMInstanceArray indications1,
		CIMInstance handler2, CIMInstanceArray indications2)
	{
		expectPopFrontCalledWithTimeoutOf(timeoutArg);
		givenPopFrontReturnsTimedOutAt(whenTimedOut);
		expectSendBurst(handler1, indications1);
		expectSendBurst(handler2, indications2);
		endStepSpecification();
	}

	void processesIsolatedIndication(
		DateTime whenReceived,
		CIMInstance const & handler, CIMInstance const & indication,
		DateTime timeoutArg, DateTime whenTimedOut)
	{
		firstIndicationArrives(whenReceived, handler, indication);
		timesOutAndSendsBurst(
			whenTimedOut, timeoutArg, handler, arr(indication));
	}

	void testIsolatedIndicationsReceived()
	{
		givenTimeIs(
			dt(T0, 0));
		wait(0.1).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		processesIsolatedIndication(
			dt(T0+7, 500000) /*arrived*/, handler1, ind1,
			dt(T0+7, 600000) /*delayed*/,
			dt(T0+7, 601037) /*processed*/);

		processesIsolatedIndication(
			dt(T0+8, 300000) /*arrived*/, handler2, ind2,
			dt(T0+8, 400000) /*delayed*/,
			dt(T0+8, 400736) /*processed*/);

		processesIsolatedShutdown(
			dt(T0+1000, 0));

		runThread();
		verify();
	}

	void receivesIndicationAndSendsBurst(
		DateTime whenArrives, DateTime timeoutArg,
		CIMInstance handlerNew, CIMInstance indicationNew,
		CIMInstance handler, CIMInstanceArray indications)
	{
		expectPopFrontCalledWithTimeoutOf(timeoutArg);
		givenPopFrontReturnsValueAt(whenArrives, handlerNew, indicationNew);
		expectSendBurst(handler, indications);
		endStepSpecification();
	}

	void testOneBurstEndedByBufferingWait()
	{
		givenTimeIs(
			dt(T0, 175000));
		wait(0.125).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 200000), handler1, ind1);
		indicationArrives(
			dt(T0, 260000), dt(T0, 325000), handler1, ind2);
		timesOutAndSendsBurst(
			dt(T0, 386023), dt(T0, 385000), handler1, arr(ind1, ind2));
		processesIsolatedShutdown(
			dt(T0+3, 240000));

		runThread();
		verify();
	}

	void testBufferingWaitTimeoutDelayedAndNewIndicationArrives()
	{
		givenTimeIs(
			dt(T0, 175000));
		wait(0.125).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 200000), handler1, ind1);
		indicationArrives(
			dt(T0, 326000), dt(T0, 325000), handler1, ind2);
		timesOutAndSendsBurst(
			dt(T0, 451023), dt(T0, 451000), handler1, arr(ind1, ind2));
		processesIsolatedShutdown(
			dt(T0+3, 240000));

		runThread();
		verify();
	}

	void testOneBurstEndedByMaxBufferingDelay()
	{
		givenTimeIs(
			dt(T0, 95000));
		wait(0.25).delay(0.35).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 100000), handler1, ind1);
		indicationArrives(
			dt(T0, 300000), dt(T0, 350000), handler1, ind2);
		timesOutAndSendsBurst(
			dt(T0, 452000), dt(T0, 450000), handler1, arr(ind1, ind2));
		processesIsolatedShutdown(
			dt(T0+5, 370000));

		runThread();
		verify();
	}

	void testOneBurstEndedByMaxBufferingDelayAndNewValueAtSameTime()
	{
		givenTimeIs(
			dt(T0, 95000));
		wait(0.25).delay(0.35).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 100000), handler1, ind1);
		indicationArrives(
			dt(T0, 300000), dt(T0, 350000), handler1, ind2);
		receivesIndicationAndSendsBurst(
			dt(T0, 452000), dt(T0, 450000), handler1, ind3,
			handler1, arr(ind1, ind2, ind3));
		processesIsolatedShutdown(
			dt(T0+5, 370000));

		runThread();
		verify();
	}

	void testOneBurstEndedByMaxBufferSize()
	{
		givenTimeIs(
			dt(T0, 87000));
		wait(0.10).delay(0.45).bufsz(2).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 100000), handler1, ind1);
		receivesIndicationAndSendsBurst(
			dt(T0, 150000), dt(T0, 200000), handler1, ind2,
			handler1, arr(ind1, ind2));
		processesIsolatedShutdown(
			dt(T0+100, 700000));

		runThread();
		verify();
	}

	void testTwoOverlappingBurstsToDifferentDestinations()
	{
		givenTimeIs(
			dt(T0, 400000));
		wait(0.1).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 500000), handler1, ind1);
		indicationArrives(
			dt(T0, 560000), dt(T0, 600000), handler2, ind3);
		indicationArrives(
			dt(T0, 590000), dt(T0, 600000), handler1, ind2);
		indicationArrives(
			dt(T0, 630000), dt(T0, 660000), handler2, ind4);
		timesOutAndSendsBurst(
			dt(T0, 690000), dt(T0, 690000), handler1, arr(ind1, ind2));
		timesOutAndSendsBurst(
			dt(T0, 730005), dt(T0, 730000), handler2, arr(ind3, ind4));
		processesIsolatedShutdown(
			dt(T0+200, 300000));

		runThread();
		verify();
	}

	void testTwoBurstsReadyToSendAtSameTime()
	{
		givenTimeIs(
			dt(T0, 400000));
		wait(0.1).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 500000), handler1, ind1);
		indicationArrives(
			dt(T0, 560000), dt(T0, 600000), handler2, ind3);
		indicationArrives(
			dt(T0, 590000), dt(T0, 600000), handler1, ind2);
		indicationArrives(
			dt(T0, 630000), dt(T0, 660000), handler2, ind4);
		timesOutAndSendsBursts(
			dt(T0, 730005), dt(T0, 690000),
			handler1, arr(ind1, ind2), handler2, arr(ind3, ind4));
		processesIsolatedShutdown(
			dt(T0+200, 300000));

		runThread();
		verify();
	}

	void testHitMaxBufferedDestinations()
	{
		givenTimeIs(
			dt(T0, 300000));
		wait(0.1).delay(0.5).bufsz(1000).dests(2).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 400000), handler1, ind1);
		indicationArrives(
			dt(T0, 450000), dt(T0, 500000), handler2, ind2);
		// At this point we cannot buffer any more destinations
		receivesIndicationAndSendsBurst(
			dt(T0, 480000), dt(T0, 500000), handler3, ind3,
			handler3, arr(ind3));
		timesOutAndSendsBurst(
			dt(T0, 500700), dt(T0, 500000), handler1, arr(ind1));
		// The burst for handler1 has been sent, freeing up a buffer
		indicationArrives(
			dt(T0, 520000), dt(T0, 550000), handler4, ind4);
		timesOutAndSendsBurst(
			dt(T0, 551300), dt(T0, 550000), handler2, arr(ind2));
		timesOutAndSendsBurst(
			dt(T0, 620000), dt(T0, 620000), handler4, arr(ind4));
		processesIsolatedShutdown(
			dt(T0+500, 123456));

		runThread();
		verify();
	}

	void testEquivalentHandlers()
	{
		givenTimeIs(
			dt(T0, 100000));
		wait(0.1).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 200000), handler4, ind3);
		// handler4a is unequal but equivalent to handler4.
		indicationArrives(
			dt(T0, 240000), dt(T0, 300000), handler4a, ind4);
		timesOutAndSendsBurst(
			dt(T0, 342537), dt(T0, 340000), handler4, arr(ind3, ind4));
		processesIsolatedShutdown(
			dt(T0+1200, 500000));

		runThread();
		verify();
	}

	void testShutdownWhileActiveBuffers()
	{
		givenTimeIs(
			dt(T0, 0));
		wait(0.1).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 100000), handler1, ind1);
		processShutdown(
			dt(T0, 150000), dt(T0, 200000));

		runThread();
		verify();
	}

	// PURPOSE: A more complex test case to flush out possible problems
	// with managing multiple buffered destinations at once.
	void testVariousDestinations()
	{
		givenTimeIs(
			dt(T0, 0));
		wait(0.1).delay(1.0).bufsz(3).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 100000), handler1, ind1);
		indicationArrives(
			dt(T0, 110000), dt(T0, 200000), handler2, ind1);
		indicationArrives(
			dt(T0, 120000), dt(T0, 200000), handler3, ind1);
		indicationArrives(
			dt(T0, 130000), dt(T0, 200000), handler4, ind1);


		indicationArrives(
			dt(T0, 140000), dt(T0, 200000), handler1, ind2);
		indicationArrives(
			dt(T0, 150000), dt(T0, 210000), handler2, ind2);
		indicationArrives(
			dt(T0, 160000), dt(T0, 220000), handler3, ind2);
		indicationArrives(
			dt(T0, 170000), dt(T0, 230000), handler4, ind2);

		receivesIndicationAndSendsBurst(
			dt(T0, 180000), dt(T0, 240000), handler1, ind3,
			handler1, arr(ind1, ind2, ind3));
		receivesIndicationAndSendsBurst(
			dt(T0, 190000), dt(T0, 250000), handler2, ind3,
			handler2, arr(ind1, ind2, ind3));
		receivesIndicationAndSendsBurst(
			dt(T0, 200000), dt(T0, 260000), handler3, ind3,
			handler3, arr(ind1, ind2, ind3));
		receivesIndicationAndSendsBurst(
			dt(T0, 210000), dt(T0, 270000), handler4, ind3,
			handler4, arr(ind1, ind2, ind3));

		processesIsolatedShutdown(
			dt(T0+1200, 0));

		runThread();
		verify();

	}

	struct SomeException : public std::exception
	{
	};

	void testSendBurstThrowsException()
	{
		givenTimeIs(
			dt(T0, 0));
		wait(0.1).delay(1.0).bufsz(3).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 100000), handler1, ind1);

		SomeException ex;
		givenSendBurstThrows(ex);
		timesOutAndSendsBurst(
			dt(T0, 200000), dt(T0, 200000), handler1, arr(ind1));

		firstIndicationArrives(
			dt(T0, 300000), handler1, ind2);
		timesOutAndSendsBurst(
			dt(T0, 400000), dt(T0, 400000), handler1, arr(ind2));

		processesIsolatedShutdown(
			dt(T0+10000, 0));
		runThread();
		verify();
	}

	void testShutdownThreadPool()
	{
		givenTimeIs(
			dt(T0, 0));
		wait(0.1).delay(1.0).bufsz(3).dests(10).thrds(10).init();

		// shutdownThreadPool is called from the indication server thread,
		// not from the IndicationBufferingThread thread itself, hence
		// no call to runThread() here.
		m_bufferingThread->shutdownThreadPool();

		// MockIndicationBurstExporter::verify checks that shutdown was called.
		verify();
	}

	void testHandlerLacksDestinationProperty()
	{
		givenTimeIs(
			dt(T0, 0));
		wait(0.1).delay(0.5).bufsz(1000).dests(10).thrds(10).init();

		firstIndicationArrives(
			dt(T0, 100000), badHandler, ind1);

		// Discards indication and does nothing, other than possibly
		// logging an error message.
		processesIsolatedShutdown(
			dt(T0+10000, 0));
		runThread();
		verify();
	}

private:
	Reference<IndicationBufferingThread> m_bufferingThread;
	MockIndicationBurstExporterRef m_burstExporter;
	Reference<MockMTSource> m_requestSource;
	MockClockRef m_clock;
	Real32 m_bufferingWaitSeconds;
	Real32 m_maxBufferingDelaySeconds;
	UInt32 m_maxBufferSize;
	UInt32 m_maxBufferedDestinations;
	UInt32 m_maxNumIoThreads;
};

} // anon ns

Test* IndicationBufferingThreadTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("IndicationBufferingThread");

	ADD_TEST_TO_SUITE(TC, testNoIndicationsReceived);
	ADD_TEST_TO_SUITE(TC, testIsolatedIndicationsReceived);
	ADD_TEST_TO_SUITE(TC, testOneBurstEndedByBufferingWait);
	ADD_TEST_TO_SUITE(TC,
		testBufferingWaitTimeoutDelayedAndNewIndicationArrives);
	ADD_TEST_TO_SUITE(TC, testOneBurstEndedByMaxBufferingDelay);
	ADD_TEST_TO_SUITE(TC,
		testOneBurstEndedByMaxBufferingDelayAndNewValueAtSameTime);
	ADD_TEST_TO_SUITE(TC, testOneBurstEndedByMaxBufferSize);
	ADD_TEST_TO_SUITE(TC, testTwoOverlappingBurstsToDifferentDestinations);
	ADD_TEST_TO_SUITE(TC, testTwoBurstsReadyToSendAtSameTime);
	ADD_TEST_TO_SUITE(TC, testHitMaxBufferedDestinations);
	ADD_TEST_TO_SUITE(TC, testEquivalentHandlers);
	ADD_TEST_TO_SUITE(TC, testVariousDestinations);
	ADD_TEST_TO_SUITE(TC, testShutdownWhileActiveBuffers);
	ADD_TEST_TO_SUITE(TC, testSendBurstThrowsException);
	ADD_TEST_TO_SUITE(TC, testShutdownThreadPool);
	ADD_TEST_TO_SUITE(TC, testHandlerLacksDestinationProperty);

	return testSuite;
}

