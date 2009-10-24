/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_HTTPXMLCIMListener.hpp"
#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "blocxx/Semaphore.hpp"
#include "blocxx/Mutex.hpp"
#include "blocxx/MutexLock.hpp"
#include "blocxx/UnnamedPipe.hpp"
#include "blocxx/CerrLogger.hpp"
#include "blocxx/CmdLineParser.hpp"
#include "OW_URL.hpp"
#include "OW_ToolsCommon.hpp"

#include <iostream> // for cout and cerr
#include <csignal>

using namespace OpenWBEM;
using namespace OpenWBEM::Tools;

using std::cout;
using std::cin;
using std::endl;
using std::cerr;

Mutex coutMutex;

class myCallBack : public CIMListenerCallback
{
protected:
	virtual void doIndicationOccurred(CIMInstance &ci,
		const String &listenerPath)
	{
		MutexLock lock(coutMutex);
		cout << ci.toString() << endl;
	}
};

#ifdef OW_WIN32
HANDLE exitEvent = NULL;
BOOL WINAPI exitHandler(DWORD ctrlEvent)
{
	if(exitEvent)
	{
		switch(ctrlEvent)
		{
			case CTRL_C_EVENT:
			case CTRL_CLOSE_EVENT:
			case CTRL_BREAK_EVENT:
				::SetEvent(exitEvent);
				return TRUE;
		}
	}
	return FALSE;
}
#else
UnnamedPipeRef sigPipe;
extern "C"
void sig_handler(int)
{
	sigPipe->writeInt(0);
}
#endif

enum
{
	HELP_OPT,
	VERSION_OPT,
	URL_OPT,
	FILTER_QUERY_OPT,
	CERT_FILE_OPT,
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
		"The fully qualified path to a certificate file in PEM format. If specified the listener will support HTTPS only"},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void Usage()
{
	cerr << "Usage: owcimindicationlistener [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}


int main(int argc, char* argv[])
{
	try
	{
		bool useHttps;
		String url;
		String ns;
		String query;
		String certfile;
		if (argc == 4 && argv[1][0] != '-' && argv[2][0] != '-' && argv[3][0] != '-')
		{
			url = argv[1];
			ns = argv[2];
			query = argv[3];
			cerr << "This command line usage is deprecated." << endl; // TODO: Remove this, it was deprecated in 3.1.0
		}
		else
		{
			CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);

			if (parser.isSet(HELP_OPT))
			{
				Usage();
				return 0;
			}
			else if (parser.isSet(VERSION_OPT))
			{
				cout << "owcimindicationlistener (OpenWBEM) " << OW_VERSION << '\n';
				cout << "Written by Dan Nuffer.\n";
				return 0;
			}
			url = parser.getOptionValue(URL_OPT, "http://localhost/root/cimv2");
			query = parser.mustGetOptionValue(FILTER_QUERY_OPT, "-f, --filter_query");
			URL urlObj(url);
			bool useHttps = urlObj.scheme.endsWith('s') || urlObj.scheme.endsWith('S');
			ns = urlObj.namespaceName;
			if (ns.empty())
			{
				cerr << "No namespace given as part of the url." << endl;
				Usage();
				return 1;
			}
			certfile = parser.getOptionValue(CERT_FILE_OPT);
		}

#ifdef OW_WIN32
		exitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if(!exitEvent)
		{
			cerr << "Failed to create exit event. Aborting..." << endl;
			return 1;
		}
		if(!SetConsoleCtrlHandler(exitHandler, TRUE))
		{
			cerr << "Failed to set the console event handler. Aborting..." << endl;
			return 1;
		}
#else
		sigPipe = UnnamedPipe::createUnnamedPipe();
		::signal(SIGINT, sig_handler);
		::signal(SIGTERM, sig_handler);
#endif


		CIMListenerCallbackRef mcb(new myCallBack);
		CerrLogger logger;
		logger.setLogLevel(E_FATAL_ERROR_LEVEL);
		HTTPXMLCIMListener hxcl(certfile);
		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
		String handle = hxcl.registerForIndication(url, ns, query, "wql1", ns, mcb, getLoginInfo);
#ifdef OW_WIN32
		::WaitForSingleObject(exitEvent, INFINITE);
#else
		// wait until we get a SIGINT as a shutdown signal
		int dummy;
		sigPipe->readInt(&dummy);
#endif

		cout << "De-registering and shutting down." << endl;
		hxcl.shutdownHttpServer();
		hxcl.deregisterForIndication(handle);
		return 0;
	}
	catch (CmdLineParserException& e)
	{
		printCmdLineParserExceptionMessage(e);
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


