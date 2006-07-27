/*******************************************************************************
* Copyright (C) 2001-2005 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_HTTPXMLCIMListener.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Condition.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_CerrAppender.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_URL.hpp"
#include "OW_Timeout.hpp"
#include "OW_DateTime.hpp"

#include <iostream> // for cout and cerr
#include <csignal>

using namespace OpenWBEM;

using std::cout;
using std::cin;
using std::endl;
using std::cerr;

Condition g_cond;
NonRecursiveMutex g_guard;
unsigned int g_indicationCount = 0;

class myCallBack : public CIMListenerCallback
{
protected:
	virtual void doIndicationOccurred(CIMInstance &ci,
		const String &listenerPath)
	{
		NonRecursiveMutexLock lock(g_guard);
		
		cout << ci.toMOF() << endl;

		++g_indicationCount;
		g_cond.notifyOne();

	}
};

enum
{
	HELP_OPT,
	VERSION_OPT,
	URL_OPT,
	FILTER_QUERY_OPT,
	CERT_FILE_OPT,
	NUM_EXPECTED_OPT,
	WAIT_SECONDS_OPT,
	SOURCE_NAMESPACE_OPT
};

CmdLineParser::Option g_options[] =
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{URL_OPT, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0,
		"The url identifying the cimom and namespace. Default is http://localhost/root/cimv2 if not specified."},
	{FILTER_QUERY_OPT, 'f', "filter_query", CmdLineParser::E_REQUIRED_ARG, 0, "Set the indication filter WQL query."},
	{SOURCE_NAMESPACE_OPT, 's', "source_namespace", CmdLineParser::E_REQUIRED_ARG, 0, "Set the indication source namespace."},
	{CERT_FILE_OPT, 'c', "cert_file", CmdLineParser::E_REQUIRED_ARG, 0,
		"The fully qualified path to a certificate file in PEM format. If specified the listener will support HTTPS only."},
	{NUM_EXPECTED_OPT, 'n', "num_expected", CmdLineParser::E_REQUIRED_ARG, 0, "Set the number of expected indications to wait for. Defaults to 1."},
	{WAIT_SECONDS_OPT, 'w', "wait_seconds", CmdLineParser::E_REQUIRED_ARG, 0, "Set the number of seconds to wait for the indications to arrive. Defaults to 60."},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void Usage()
{
	cerr << "Usage: testListener [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}


int main(int argc, char* argv[])
{
	try
	{
		CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);

		if (parser.isSet(HELP_OPT))
		{
			Usage();
			return 0;
		}
		else if (parser.isSet(VERSION_OPT))
		{
			cout << "testListener (OpenWBEM) " << OW_VERSION << '\n';
			cout << "Written by Dan Nuffer.\n";
			return 0;
		}
		String url = parser.getOptionValue(URL_OPT, "http://localhost/root/cimv2");
		String query = parser.mustGetOptionValue(FILTER_QUERY_OPT, "-f, --filter_query");
		URL urlObj(url);
		String ns = urlObj.namespaceName;
		if (ns.empty())
		{
			cerr << "No namespace given as part of the url." << endl;
			Usage();
			return 1;
		}
		String certfile = parser.getOptionValue(CERT_FILE_OPT);
		unsigned int numExpected = parser.getOptionValue(NUM_EXPECTED_OPT, "1").toUnsignedInt();
		unsigned int waitSeconds = parser.getOptionValue(WAIT_SECONDS_OPT, "60").toUnsignedInt();
		String sourceNamespace = parser.getOptionValue(SOURCE_NAMESPACE_OPT, ns.c_str());
	

		CIMListenerCallbackRef mcb(new myCallBack);
		LogAppender::setDefaultLogAppender(LogAppenderRef(new CerrAppender()));
		HTTPXMLCIMListener hxcl(certfile);
		String handle = hxcl.registerForIndication(url, ns, query, "wql1", sourceNamespace, mcb);

		// now wait for them to show up
		DateTime timeoutTime = DateTime::getCurrent();
		timeoutTime.addSeconds(waitSeconds);
		Timeout timeout = Timeout::absolute(timeoutTime);
		bool timedOut = false;
		{
			NonRecursiveMutexLock lock(g_guard);
			while (g_indicationCount < numExpected)
			{
				if (!g_cond.timedWait(lock, timeout))
				{
					timedOut = true;
					break;
				}
			}
		}
		
		cout << "De-registering and shutting down." << endl;
		hxcl.shutdownHttpServer();
		hxcl.deregisterForIndication(handle);
		if (timedOut)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	catch (CmdLineParserException& e)
	{
		cerr << e << endl;
		Usage();
	}
	catch(const CIMException& e)
	{
		cerr << CIMException::getCodeName(e.getErrNo()) << ':' << e.getMessage() << endl;
	}
	catch(Exception& e)
	{
		cerr << e << endl;
	}
	catch(std::exception& e)
	{
		cerr << e.what() << endl;
	}
	catch(...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}


