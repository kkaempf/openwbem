/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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
#include "OW_EventManagerTestCases.hpp"
#include "OW_EventManager.hpp"

static OW_EventManager g_eventMgr;

static OW_String g_data1 = "unsignalled";
static OW_String g_data2 = "unsignalled";

//////////////////////////////////////////////////////////////////////////////
class Listener1 : public OW_EventListener
{
public:
	virtual void handleEvent(const OW_String &event, size_t dataLength, 
		void* data)
	{
		(void)event;
		(void)dataLength;
		(void)data;
		g_data1 = "signalled";
	}
};

//////////////////////////////////////////////////////////////////////////////
class Listener2 : public OW_EventListener
{
public:
	virtual void handleEvent(const OW_String &event, size_t dataLength,
		void* data)
	{
		(void)event;
		(void)dataLength;
		(void)data;
		g_data2 = "signalled";
	}
};

//////////////////////////////////////////////////////////////////////////////
void OW_EventManagerTestCases::setUp()
{
}

//////////////////////////////////////////////////////////////////////////////
void OW_EventManagerTestCases::tearDown()
{
}

//////////////////////////////////////////////////////////////////////////////
void OW_EventManagerTestCases::testSomething()
{
	Listener1 l1;
	Listener2 l2;
	g_eventMgr.registerListener("event1", &l1);
	g_eventMgr.registerListener("event2", &l2);
	g_eventMgr.sendEvent("event1");
	g_eventMgr.sendEvent("event2");
	unitAssert(g_data1.equals("signalled"));
	unitAssert(g_data2.equals("signalled"));

	g_data1 = g_data2 = "unsignalled";
	g_eventMgr.unregisterListener("event1", &l1);
	g_eventMgr.sendEvent("event1");
	g_eventMgr.sendEvent("event2");
	unitAssert(g_data1.equals("unsignalled"));
	unitAssert(g_data2.equals("signalled"));

}

//////////////////////////////////////////////////////////////////////////////
Test* OW_EventManagerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_EventManager");

	testSuite->addTest (new TestCaller <OW_EventManagerTestCases> 
			("testSomething", 
			&OW_EventManagerTestCases::testSomething));

	return testSuite;
}

//////////////////////////////////////////////////////////////////////////////

