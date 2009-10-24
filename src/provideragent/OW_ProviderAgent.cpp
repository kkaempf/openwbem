/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ProviderAgent.hpp"
#include "OW_HTTPServer.hpp"
#include "OW_CIMClass.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "blocxx/SelectEngine.hpp"
#include "blocxx/IOException.hpp"
#include "blocxx/Thread.hpp"
#include "OW_ProviderAgentEnvironment.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "blocxx/Format.hpp"

#include <cerrno>

namespace OW_NAMESPACE
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
	SelectEngineThread(const Reference<Array<SelectablePair_t> >& selectables,
		const ProviderAgentLifecycleCallbackIFCRef& lifecycleCB)
	: Thread()
	, m_selectables(selectables)
	, m_stopObject(UnnamedPipe::createUnnamedPipe())
	, m_lifecycleCB(lifecycleCB)
	{
		m_stopObject->setBlocking(UnnamedPipe::E_NONBLOCKING);
	}
	/**
	 * The method that will be run when the start method is called on this
	 * Thread object.
	 */
	virtual Int32 run()
	{
		try
		{
			SelectEngine engine;
			for (size_t i = 0; i < m_selectables->size(); ++i)
			{
				engine.addSelectableObject((*m_selectables)[i].first->getSelectObj(),
					(*m_selectables)[i].second, SelectableCallbackIFC::E_ACCEPT_EVENT);
			}

			SelectableCallbackIFCRef cb(new SelectEngineStopper(engine));
			engine.addSelectableObject(m_stopObject->getReadSelectObj(),
				cb, SelectableCallbackIFC::E_READ_EVENT);

			if (m_lifecycleCB)
			{
				m_lifecycleCB->started();
			}
			engine.go(Timeout::infinite);
		}
		catch (Exception& e)
		{
			if (m_lifecycleCB)
			{
				m_lifecycleCB->fatalError(Format("%1", e));
			}
			throw;
		}
		catch (...)
		{
			if (m_lifecycleCB)
			{
				m_lifecycleCB->fatalError("unknown");
			}
			throw;
		}
		return 0;
	}
	virtual void doShutdown()
	{
		// write something into the stop pipe to stop the select engine so the
		// thread will exit
		if (m_stopObject->writeInt(0) == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, "Writing to the termination pipe failed");
		}
	}
private:
	Reference<Array<SelectablePair_t> > m_selectables;
	UnnamedPipeRef m_stopObject;
	ProviderAgentLifecycleCallbackIFCRef m_lifecycleCB;
};


} // end anonymous namespace


const char* const ProviderAgent::LockingType_opt = "provider_agent.locking_type";
const char* const ProviderAgent::LockingTypeNone = "none";
const char* const ProviderAgent::LockingTypeSWMR = "swmr";
const char* const ProviderAgent::LockingTypeSingleThreaded = "single_threaded";

const char* const ProviderAgent::LockingTimeout_opt = "provider_agent.locking_timeout";
const char* const ProviderAgent::DynamicClassRetrieval_opt = "provider_agent.dynamic_class_retrieval";
const char* const ProviderAgent::UseConnectionCredentials_opt = "provider_agent.use_connection_credentials";

//////////////////////////////////////////////////////////////////////////////
ProviderAgent::ProviderAgent(
	const ConfigFile::ConfigMap& configMap,
	const Array<CppProviderBaseIFCRef>& providers,
	const Array<CIMClass>& cimClasses,
	const Array<RequestHandlerIFCRef>& requestHandlers,
	const AuthenticatorIFCRef& authenticator,
	const String& callbackURL,
	const ProviderAgentLockerIFCRef& locker,
	const ProviderAgentLifecycleCallbackIFCRef& lifecycleCB)
	: m_httpServer(new HTTPServer)
	, m_lifecycleCB(lifecycleCB)
{
	Reference<Array<SelectablePair_t> >
			selectables(new Array<SelectablePair_t>);
	ServiceEnvironmentIFCRef env(new ProviderAgentEnvironment(configMap,
			providers, cimClasses, authenticator, requestHandlers,
			callbackURL, selectables, locker));
	m_httpServer->init(env);
	m_httpServer->start();  // The http server will add it's server
	// sockets to the environment's selectables, which is really
	// the selectables defined above.  We'll give these to the select engine
	// thread below which will use them to run the select engine.

	// start a thread to run the http server
	m_httpThread = new SelectEngineThread(selectables, m_lifecycleCB);
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
	if (m_lifecycleCB)
	{
		m_lifecycleCB->shuttingDown();
	}
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

} // end namespace OW_NAMESPACE

