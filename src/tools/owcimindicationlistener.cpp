/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_HTTPXMLCIMListener.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Semaphore.hpp"

#include <iostream> // for cout and cerr

using std::cout;
using std::endl;
using std::cerr;

OW_Mutex coutMutex;

class myCallBack : public OW_CIMListenerCallback
{
protected:
	virtual void doIndicationOccurred(OW_CIMInstance &ci,
		const OW_String &listenerPath)
	{
		(void)listenerPath;
		OW_MutexLock lock(coutMutex);
		cout << ci.toString() << "\n";
	}
};


class ListenerLogger : public OW_Logger
{
protected:
	virtual void doLogMessage(const OW_String &message, const OW_LogLevel) const
	{
		cerr << message << endl;
	}
};

OW_Semaphore shutdownSem;

extern "C" 
void sig_handler(int)
{
	shutdownSem.signal();
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 4)
		{
			cerr << "Usage: " << argv[0] << " <URL> <namespace> <filter query>" << endl;
			exit(1);
		}

		signal(SIGINT, sig_handler);

		OW_String url(argv[1]);
		OW_String ns(argv[2]);
		OW_String query(argv[3]);

		OW_CIMListenerCallbackRef mcb(new myCallBack);

		OW_LoggerRef logger(new ListenerLogger);

		OW_HTTPXMLCIMListener hxcl(logger);

		OW_CIMProtocolIFCRef httpClient(new OW_HTTPClient(url));
		OW_CIMXMLCIMOMHandle rch(httpClient);

		OW_String handle = hxcl.registerForIndication(url, ns, query, "wql1", ns, mcb);

		// wait until we get a SIGINT
		shutdownSem.wait();

		hxcl.shutdownHttpServer();
		hxcl.deregisterForIndication(handle);

		return 0;
	}
	catch(OW_Exception& e)
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

