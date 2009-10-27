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
#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "ConfigureCppIndicationExportXMLHTTPProviderTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(ConfigureCppIndicationExportXMLHTTPProviderTestCases,"ConfigureCppIndicationExportXMLHTTPProvider");
#include "OW_CppIndicationExportXMLHTTPProvider.hpp"
#include "DummyProvEnv.hpp"
#include "OW_ConfigException.hpp"

using namespace OpenWBEM;
using namespace blocxx;

namespace
{
	struct MockProvEnv : public DummyProvEnv
	{
		MockProvEnv(
			String const & bufferingWaitSeconds,
			String const & maxBufferingDelaySeconds,
			String const & maxBufferSize,
			String const & maxBufferedDestinations,
			String const & maxNumIoThreads)
		: m_bufferingWaitSeconds(bufferingWaitSeconds),
		  m_maxBufferingDelaySeconds(maxBufferingDelaySeconds),
		  m_maxBufferSize(maxBufferSize),
		  m_maxBufferedDestinations(maxBufferedDestinations),
		  m_maxNumIoThreads(maxNumIoThreads)
		{
		}

		virtual ~MockProvEnv()
		{
		}

		virtual String getConfigItem(
			const String &name, const String& defRetVal="") const
		{
			String retVal = defRetVal;
			if (name == "indication_export_xml_http.buffering_wait_seconds")
			{
				retVal = m_bufferingWaitSeconds;
			}
			else if (name ==
				"indication_export_xml_http.max_buffering_delay_seconds")
			{
				retVal = m_maxBufferingDelaySeconds;
			}
			else if (name == "indication_export_xml_http.max_buffer_size")
			{
				retVal = m_maxBufferSize;
			}
			else if (name ==
				"indication_export_xml_http.max_buffered_destinations")
			{
				retVal = m_maxBufferedDestinations;
			}
			else if (name ==
				"indication_export_xml_http.max_num_io_threads")
			{
				retVal = m_maxNumIoThreads;
			}
			return retVal.empty() ? defRetVal : retVal;
		}

	private:
		String m_bufferingWaitSeconds;
		String m_maxBufferingDelaySeconds;
		String m_maxBufferSize;
		String m_maxBufferedDestinations;
		String m_maxNumIoThreads;
	}; // class ProvEnv

}

void ConfigureCppIndicationExportXMLHTTPProviderTestCases::setUp()
{
	m_provider = new CppIndicationExportXMLHTTPProvider();
}

void ConfigureCppIndicationExportXMLHTTPProviderTestCases::tearDown()
{
	m_provider = 0;
}

#define TestCases ConfigureCppIndicationExportXMLHTTPProviderTestCases
#define Prov CppIndicationExportXMLHTTPProvider

void TestCases::configureReturns(
	char const * bufferingWaitSecondsStr, Real32 bufferingWaitSeconds,
	char const * maxBufferingDelaySecondsStr, Real32 maxBufferingDelaySeconds,
	char const * maxBufferSizeStr, UInt32 maxBufferSize,
	char const * maxBufferedDestinationsStr, UInt32 maxBufferedDestinations,
	char const * maxNumIoThreadsStr, UInt32 maxNumIoThreads
	)
{
	ProviderEnvironmentIFCRef env(
		new MockProvEnv(
			bufferingWaitSecondsStr, maxBufferingDelaySecondsStr,
			maxBufferSizeStr, maxBufferedDestinationsStr, maxNumIoThreadsStr));
	Prov::Config config = m_provider->configValues(env);
	unitAssertEquals(bufferingWaitSeconds, config.bufferingWaitSeconds);
	unitAssertEquals(maxBufferingDelaySeconds, config.maxBufferingDelaySeconds);
	unitAssertEquals(maxBufferSize, config.maxBufferSize);
	unitAssertEquals(maxBufferedDestinations, config.maxBufferedDestinations);

	unitAssertEquals(maxNumIoThreads, config.maxNumIoThreads);
	unitAssertEquals(config.maxNumIoThreads, config.maxQueueSize);
}

void TestCases::configureThrows(
	char const * bufferingWaitSecondsStr,
	char const * maxBufferingDelaySecondsStr,
	char const * maxBufferSizeStr,
	char const * maxBufferedDestinationsStr,
	char const * maxNumIoThreadsStr
)
{
	ProviderEnvironmentIFCRef env(
		new MockProvEnv(
			bufferingWaitSecondsStr, maxBufferingDelaySecondsStr,
			maxBufferSizeStr, maxBufferedDestinationsStr, maxNumIoThreadsStr));
	try
	{
		m_provider->configValues(env);
		unitAssert(false);
	}
	catch (ConfigException &)
	{
	}
}

void TestCases::testAllOK()
{
	configureReturns(
		"0.15625", 0.15625, "0.515625", 0.515625, "1000", 1000, "10", 10,
		"20", 20);
}

void TestCases::testBufferingWaitSecondsMissing()
{
	configureReturns(
		"", 0.125, "1.5", 1.5, "1500", 1500, "12", 12, "7", 7);
}

void TestCases::testBufferingWaitSecondsBad()
{
	configureThrows("not a number", "1.2", "200", "8", "12");
}

void TestCases::testMaxBufferingDelaySecondsMissing()
{
	configureReturns(
		"0.375", 0.375, "", 0.5, "1222", 1222, "20", 20, "12", 12);
}

void TestCases::testMaxBufferingDelaySecondsBad()
{
	configureThrows("0.375", "not an int", "1237", "15", "13");
}

void TestCases::testMaxBufferSizeMissing()
{
	configureReturns(
		"0.125", 0.125, "0.75", 0.75, "", 1000, "13", 13, "9", 9);
}

void TestCases::testMaxBufferSizeBad()
{
	configureThrows("0.125", "1.500", "not an int", "18", "9");
}

void TestCases::testMaxBufferedDestinationsMissing()
{
	configureReturns(
		"0.1", 0.1, "0.6", 0.6, "2000", 2000, "", 10, "19", 19);
}

void TestCases::testMaxBufferedDestinationsBad()
{
	configureThrows("0.1", "0.6", "2000", "not an int", "22");
}

void TestCases::testMaxNumIoThreadsMissing()
{
	configureReturns(
		"0.08", 0.08, "0.45", 0.45, "1237", 1237, "22", 22, "", 10);
}

void TestCases::testMaxNumIoThreadsBad()
{
	configureThrows("0.09", "0.62", "777", "26", "not an int");
}

void TestCases::testBufferingWaitIsGreaterThanMaxBufferingDelay()
{
	configureThrows("0.5", "0.125", "1000", "10", "10");
}

void TestCases::testMaxBufferSizeIsZero()
{
	configureThrows("0.1", "0.5", "0", "10", "10");
}

void TestCases::testMaxNumIoThreadsIsZero()
{
	configureThrows("0.1", "0.5", "1000", "10", "0");
}



Test* ConfigureCppIndicationExportXMLHTTPProviderTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("ConfigureCppIndicationExportXMLHTTPProvider");

	ADD_TEST_TO_SUITE(TestCases, testAllOK);

	ADD_TEST_TO_SUITE(TestCases, testBufferingWaitSecondsMissing);
	ADD_TEST_TO_SUITE(TestCases, testBufferingWaitSecondsBad);

	ADD_TEST_TO_SUITE(TestCases, testMaxBufferingDelaySecondsMissing);
	ADD_TEST_TO_SUITE(TestCases, testMaxBufferingDelaySecondsBad);

	ADD_TEST_TO_SUITE(TestCases, testMaxBufferSizeMissing);
	ADD_TEST_TO_SUITE(TestCases, testMaxBufferSizeBad);

	ADD_TEST_TO_SUITE(TestCases, testMaxBufferedDestinationsMissing);
	ADD_TEST_TO_SUITE(TestCases, testMaxBufferedDestinationsBad);

	ADD_TEST_TO_SUITE(TestCases, testMaxNumIoThreadsMissing);
	ADD_TEST_TO_SUITE(TestCases, testMaxNumIoThreadsBad);

	ADD_TEST_TO_SUITE(TestCases, testBufferingWaitIsGreaterThanMaxBufferingDelay);
	ADD_TEST_TO_SUITE(TestCases, testMaxBufferSizeIsZero);
	ADD_TEST_TO_SUITE(TestCases, testMaxNumIoThreadsIsZero);

	return testSuite;
}
