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

void OW_LoggerTestCases::setUp()
{
}

void OW_LoggerTestCases::tearDown()
{
}

void OW_LoggerTestCases::testCreateFileLogger()
{
	OW_String logname = "/tmp/testlog";
	OW_LoggerRef pLogger = OW_Logger::createLogger(logname, false);

	unitAssert( pLogger );
	unitAssert( remove(logname.c_str()) != -1 );

	OW_String badfilename = "some/dir/that/doesn't/exist";
	unitAssertThrows(OW_Logger::createLogger(badfilename, false));
}

void OW_LoggerTestCases::testCreateSyslogLogger()
{
	OW_LoggerRef pLogger = OW_Logger::createLogger(OW_String("syslog"), false);
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
	OW_String filename = "/tmp/test";
	OW_LoggerRef pLogger = OW_Logger::createLogger(filename, false);
	pLogger->logError( "error1" );
	pLogger->logCustInfo( "custinfo1" );
	pLogger->logDebug( "debug1" );
	verifyFileLog( __FILE__, __LINE__, filename.c_str(), "error1\n" );

	pLogger->setLogLevel( CustInfoLevel );
	pLogger->logError( "error2" );
	pLogger->logCustInfo( "custinfo2" );
	pLogger->logDebug( "debug2" );
	verifyFileLog( __FILE__, __LINE__, filename.c_str(), "error1\n"
					"error2\n"
					"custinfo2\n"
					 );

	pLogger->setLogLevel( DebugLevel );
	pLogger->logError( "error3" );
	pLogger->logCustInfo( "custinfo3" );
	pLogger->logDebug( "debug3" );
	verifyFileLog( __FILE__, __LINE__, filename.c_str(), "error1\n"
					"error2\n"
					"custinfo2\n"
					"error3\n"
					"custinfo3\n"
					"debug3\n"
					 );
	remove( filename.c_str() );
}

void OW_LoggerTestCases::testSyslogLogging()
{
	OW_String filename = "syslog";
	OW_LoggerRef pLogger = OW_Logger::createLogger(filename, false);
	pLogger->logError( "error1" );
	pLogger->logCustInfo( "custinfo1" );
	pLogger->logDebug( "debug1" );

	pLogger->setLogLevel( CustInfoLevel );
	pLogger->logError( "error2" );
	pLogger->logCustInfo( "custinfo2" );
	pLogger->logDebug( "debug2" );

	pLogger->setLogLevel( DebugLevel );
	pLogger->logError( "error3" );
	pLogger->logCustInfo( "custinfo3" );
	pLogger->logDebug( "debug3" );
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

