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

#include "OW_config.h"
#include "OW_HTTPXMLCIMListener.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Semaphore.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_GetPass.hpp"
#include "OW_CerrLogger.hpp"

#include <iostream> // for cout and cerr
#include <csignal>

using namespace OpenWBEM;

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

class GetLoginInfo : public ClientAuthCBIFC
{
public:
	bool getCredentials(const String& realm, String& name,
			String& passwd, const String& details)
	{
		cout << "Authentication required for " << realm << endl;
		cout << "Enter the user name: ";
		name = String::getLine(cin);
		passwd = GetPass::getPass("Enter the password for " +
			name + ": ");
		return true;
	}
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 4)
		{
			cerr << "Usage: " << argv[0] << " <URL> <namespace> <filter query>" << endl;
			exit(1);
		}

#ifdef OW_WIN32
		exitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if(!exitEvent)
		{
			cerr << "Failed to create exit event. Aborting..." << endl;
			exit(1);
		}
		if(!SetConsoleCtrlHandler(exitHandler, TRUE))
		{
			cerr << "Failed to set the console event handler. Aborting..." << endl;
			exit(1);
		}
#else
		sigPipe = UnnamedPipe::createUnnamedPipe();
		::signal(SIGINT, sig_handler);
		::signal(SIGTERM, sig_handler);
#endif

		String url(argv[1]);
		String ns(argv[2]);
		String query(argv[3]);
		CIMListenerCallbackRef mcb(new myCallBack);
		LoggerRef logger(new CerrLogger);
		logger->setLogLevel(E_FATAL_ERROR_LEVEL);
		HTTPXMLCIMListener hxcl(logger);
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


