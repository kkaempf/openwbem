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
#include <string>
#include "OW_LoggerTestCases.hpp"
#include "OW_Logger.hpp"
#include <stdio.h> // for remove
#include <fstream>
/*
#include <iostream>

using std::cout;
using std::endl;
*/
using std::ifstream;
using std::string;

using namespace OpenWBEM;

void OW_LoggerTestCases::setUp()
{
}

void OW_LoggerTestCases::tearDown()
{
}

void OW_LoggerTestCases::testCreateFileLogger()
{
	String logname = "/tmp/testlog";
	LoggerRef pLogger = Logger::createLogger(logname, false);

	unitAssert( pLogger );
	unitAssert( remove(logname.c_str()) != -1 );

	String badfilename = "some/dir/that/doesn't/exist";
	unitAssertThrows(Logger::createLogger(badfilename, false));
}

void OW_LoggerTestCases::testCreateSyslogLogger()
{
	LoggerRef pLogger = Logger::createLogger(String("syslog"), false);
	unitAssert( pLogger );
}

void OW_LoggerTestCases::verifyFileLog( const char* file, int line, const char* filename, const char* test )
{
	ifstream in(filename);
	std::string contents, temp;
	while (getline(in, temp))
	{
		getline(in, temp);
		contents += temp;
		contents += '\n';
		temp.erase();
	}
	this->assertImplementation( contents == test, "verifyFileLog", line, file );
}

void OW_LoggerTestCases::testFileLogging()
{
	String filename = "/tmp/test";
	LoggerRef pLogger = Logger::createLogger(filename, false);
	pLogger->logFatalError("fatalerror1");
	pLogger->logError( "error1" );
	pLogger->logInfo( "custinfo1" );
	pLogger->logDebug( "debug1" );
	verifyFileLog( __FILE__, __LINE__, filename.c_str(),
				   "fatalerror1\n"
				   "error1\n" );

	pLogger->setLogLevel( E_INFO_LEVEL );
	pLogger->logFatalError("fatalerror2");
	pLogger->logError( "error2" );
	pLogger->logInfo( "custinfo2" );
	pLogger->logDebug( "debug2" );
	verifyFileLog( __FILE__, __LINE__, filename.c_str(),
				    "fatalerror1\n"
				    "error1\n"
					"fatalerror2\n"
					"error2\n"
					"custinfo2\n"
					 );

	pLogger->setLogLevel( E_DEBUG_LEVEL );
	pLogger->logFatalError("fatalerror3");
	pLogger->logError( "error3" );
	pLogger->logInfo( "custinfo3" );
	pLogger->logDebug( "debug3" );
	verifyFileLog( __FILE__, __LINE__, filename.c_str(),
					"fatalerror1\n"
					"error1\n"
					"fatalerror2\n"
					"error2\n"
					"custinfo2\n"
					"fatalerror3\n"
					"error3\n"
					"custinfo3\n"
					"debug3\n"
					 );
	
	pLogger->setLogLevel( E_FATAL_ERROR_LEVEL );
	pLogger->logFatalError("fatalerror4");
	pLogger->logError( "error4" );
	pLogger->logInfo( "custinfo4" );
	pLogger->logDebug( "debug4" );
	verifyFileLog( __FILE__, __LINE__, filename.c_str(),
					"fatalerror1\n"
					"error1\n"
					"fatalerror2\n"
					"error2\n"
					"custinfo2\n"
					"fatalerror3\n"
					"error3\n"
					"custinfo3\n"
					"debug3\n"
				    "fatalerror4\n"
					 );
	remove( filename.c_str() );
}

void OW_LoggerTestCases::testSyslogLogging()
{
	String filename = "syslog";
	LoggerRef pLogger = Logger::createLogger(filename, false);
	pLogger->logFatalError("fatalerror1");
	pLogger->logError( "error1" );
	pLogger->logInfo( "custinfo1" );
	pLogger->logDebug( "debug1" );
}

Test* OW_LoggerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Logger");

	testSuite->addTest (new TestCaller <OW_LoggerTestCases>
			("testCreateFileLogger",
			&OW_LoggerTestCases::testCreateFileLogger));
	testSuite->addTest (new TestCaller <OW_LoggerTestCases>
			("testCreateSyslogLogger",
			&OW_LoggerTestCases::testCreateSyslogLogger));
	testSuite->addTest (new TestCaller <OW_LoggerTestCases>
			("testFileLogging",
			&OW_LoggerTestCases::testFileLogging));
	testSuite->addTest (new TestCaller <OW_LoggerTestCases>
			("testSyslogLogging",
			&OW_LoggerTestCases::testSyslogLogging));

	return testSuite;
}

