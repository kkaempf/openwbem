/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ProviderAgent.hpp"
#include "OW_HTTPServer.hpp"
#include "OW_XMLExecute.hpp"
#include "OW_HTTPException.hpp"
#include "OW_Format.hpp"
#include "OW_NonAuthenticatingAuthenticator.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ServerSocket.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_SelectableIFC.hpp"
#include "OW_IOException.hpp"
#include "OW_Thread.hpp"
#include "OW_Assertion.hpp"
#include "OW_ProviderAgentCIMOMHandle.hpp"
#include "OW_ProviderAgentEnvironment.hpp"

#include "OW_CppProviderBaseIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_CppSecondaryInstanceProviderIFC.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_CIMQualifierType.hpp"
#include <algorithm> // for std::remove
#include <iostream>

using namespace std; 

namespace OpenWBEM
{

using namespace WBEMFlags;
namespace
{
// This anonymous namespace has the effect of giving this class internal
// linkage.  That means it won't cause a link error if another translation
// unit has a class with the same name.




class SelectEngineThread : public Thread
{
public:
	SelectEngineThread(Reference<Array<SelectablePair_t> > selectables)
	: Thread()
	, m_selectables(selectables)
	, m_stopObject(UnnamedPipe::createUnnamedPipe())
	{
		m_stopObject->setBlocking(UnnamedPipe::E_NONBLOCKING);
	}
	/**
	 * The method that will be run when the start method is called on this
	 * Thread object.
	 */
	virtual Int32 run()
	{
		SelectEngine engine;
		SelectableCallbackIFCRef cb(new SelectEngineStopper(engine));
		m_selectables->push_back(std::make_pair(m_stopObject, cb));
		for (size_t i = 0; i < m_selectables->size(); ++i)
		{
			engine.addSelectableObject((*m_selectables)[i].first,
				(*m_selectables)[i].second);
		}
		engine.go();
		return 0;
	}
	virtual void doCooperativeCancel()
	{
		// write something into the stop pipe to stop the select engine so the
		// thread will exit
		if (m_stopObject->writeInt(0) == -1)
		{
			OW_THROW(IOException, "Writing to the termination pipe failed");
		}
	}
private:
	Reference<Array<SelectablePair_t> > m_selectables;
	UnnamedPipeRef m_stopObject;
};


} // end anonymous namespace


const char* const ProviderAgent::LockingType_opt = "provider_agent.locking_type"; 
const char* const ProviderAgent::LockingTimeout_opt = "provider_agent.locking_timeout";
//////////////////////////////////////////////////////////////////////////////
ProviderAgent::ProviderAgent(ConfigFile::ConfigMap configMap, 
							 CppProviderBaseIFCRef provider, 
							 Array<RequestHandlerIFCRef> requestHandlers, 
							 Reference<AuthenticatorIFC> authenticator,
							 LoggerRef logger)
	: m_httpServer(new HTTPServer)
	, m_httpListenPort(0)
	, m_httpsListenPort(0)
{
	Reference<Array<SelectablePair_t> >
		selectables(new Array<SelectablePair_t>);
	ServiceEnvironmentIFCRef env(new ProviderAgentEnvironment(configMap,
		provider, authenticator, requestHandlers, logger, selectables));
	m_httpServer->setServiceEnvironment(env);
	m_httpServer->startService();  // The http server will add it's server
	// sockets to the environment's selectables, which is really
	// the selectabls defined above.  We'll give these to the select engine thread
	// below which will use them to run the select engine.
	m_httpListenPort = m_httpServer->getLocalHTTPAddress().getPort();
	cerr << "listening on: " << m_httpListenPort << endl;
	m_httpsListenPort = m_httpServer->getLocalHTTPSAddress().getPort();
	// start a thread to run the http server
	m_httpThread = new SelectEngineThread(selectables);
	m_httpThread->start();
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgent::~ProviderAgent()
{
	try
	{
		shutdownHttpServer();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}

//////////////////////////////////////////////////////////////////////////////
void
ProviderAgent::shutdownHttpServer()
{
	if (m_httpThread)
	{
		m_httpThread->definitiveCancel();
		// wait for the thread to quit
		m_httpThread->join();
		m_httpThread = 0;
	}
	if (m_httpServer)
	{
		// stop the http server
		m_httpServer->shutdown();
		m_httpServer = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////

} // end namespace OpenWBEM

