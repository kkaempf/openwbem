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
#include "OW_config.h"
#include "OW_HTTPXMLCIMListener.hpp"
#include "OW_CIMListenerCallback.hpp"
#include "OW_HTTPServer.hpp"
#include "OW_XMLListener.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_HTTPException.hpp"
#include "OW_Format.hpp"
#include "OW_MutexLock.hpp"
#include "OW_ListenerAuthenticator.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_ServerSocket.hpp"
#include "OW_SelectEngine.hpp"
#include "OW_SelectableIFC.hpp"
#include "OW_IOException.hpp"

#include <algorithm> // for std::remove

namespace
{
// This anonymous namespace has the effect of giving this class internal
// linkage.  That means it won't cause a link error if another translation
// unit has a class with the same name.

typedef std::pair<OW_SelectableIFCRef, OW_SelectableCallbackIFCRef> SelectablePair_t;

class OW_HTTPXMLCIMListenerServiceEnvironment : public OW_ServiceEnvironmentIFC
{
public:
	OW_HTTPXMLCIMListenerServiceEnvironment(
		OW_Reference<OW_ListenerAuthenticator> authenticator,
		OW_RequestHandlerIFCRef listener,
		OW_LoggerRef logger,
		OW_Reference<OW_Array<SelectablePair_t> > selectables)
	: m_pLAuthenticator(authenticator)
	, m_XMLListener(listener)
	, m_logger(logger ? logger : OW_LoggerRef(new DummyLogger))
	, m_selectables(selectables)
	{
		m_configItems[OW_ConfigOpts::HTTP_PORT_opt] = OW_String(0);
		m_configItems[OW_ConfigOpts::HTTPS_PORT_opt] = OW_String(-1);
		m_configItems[OW_ConfigOpts::MAX_CONNECTIONS_opt] = OW_String(10);
		m_configItems[OW_ConfigOpts::SINGLE_THREAD_opt] = "false";
		m_configItems[OW_ConfigOpts::ENABLE_DEFLATE_opt] = "true";
		m_configItems[OW_ConfigOpts::HTTP_USE_DIGEST_opt] = "false";
	}

	virtual ~OW_HTTPXMLCIMListenerServiceEnvironment() {}

	virtual OW_Bool authenticate(OW_String &userName,
		const OW_String &info, OW_String &details)
	{
		return m_pLAuthenticator->authenticate(userName, info, details);
	}

	virtual void addSelectable(OW_SelectableIFCRef obj,
		OW_SelectableCallbackIFCRef cb)
	{
		m_selectables->push_back(std::make_pair(obj, cb));
	}

	virtual void removeSelectable(OW_SelectableIFCRef obj, OW_SelectableCallbackIFCRef cb)
	{
		m_selectables->erase(std::remove(m_selectables->begin(), m_selectables->end(),
			std::make_pair(obj, cb)), m_selectables->end());
	}

	virtual OW_String getConfigItem(const OW_String &name) const
	{
		OW_Map<OW_String, OW_String>::const_iterator i =
			m_configItems.find(name);
		if (i != m_configItems.end())
		{
			return (*i).second;
		}
		else
		{
			return OW_String();
		}
	}

	virtual void setConfigItem(const OW_String& item, const OW_String& value, OW_Bool overwritePrevious)
	{
		if (overwritePrevious || getConfigItem(item) == "")
			m_configItems[item] = value;

	}
	
	virtual OW_RequestHandlerIFCRef getRequestHandler(const OW_String&)
	{
		return m_XMLListener;
	}

	virtual OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_String& /*username*/,
		const OW_Bool /*doIndications*/)
	{
		OW_THROW(OW_Exception, "Not implemented");
	}

	virtual OW_LoggerRef getLogger() const
	{
		return m_logger;
	}

private:
	OW_Map<OW_String, OW_String> m_configItems;
	OW_Reference<OW_ListenerAuthenticator> m_pLAuthenticator;
	OW_RequestHandlerIFCRef m_XMLListener;
	OW_LoggerRef m_logger;
	OW_Reference<OW_Array<SelectablePair_t> > m_selectables;

	class DummyLogger : public OW_Logger
	{
	protected:
		virtual void doLogMessage(const OW_String &, const OW_LogLevel) const
		{
			return;
		}
	};
};

class SelectEngineThread : public OW_Thread
{
public:
	SelectEngineThread(OW_Reference<OW_Array<SelectablePair_t> > selectables,
		OW_SelectableIFCRef stopObject)
	: OW_Thread(true) // joinable
	, m_selectables(selectables)
	, m_stopObject(stopObject)
	{}

	/**
	 * The method that will be run when the start method is called on this
	 * OW_Thread object.
	 */
	virtual void run()
	{
		OW_SelectEngine engine;
		OW_SelectableCallbackIFCRef cb(new OW_SelectEngineStopper(engine));
		m_selectables->push_back(std::make_pair(m_stopObject, cb));
		for (size_t i = 0; i < m_selectables->size(); ++i)
		{
			engine.addSelectableObject((*m_selectables)[i].first,
				(*m_selectables)[i].second);
		}
		engine.go();
	}

private:
	OW_Reference<OW_Array<SelectablePair_t> > m_selectables;
	OW_SelectableIFCRef m_stopObject;
};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_HTTPXMLCIMListener::OW_HTTPXMLCIMListener(OW_LoggerRef logger)
	: m_callbacks()
	, m_XMLListener(OW_SharedLibraryRef(0), new OW_XMLListener(this))
	, m_pLAuthenticator(new OW_ListenerAuthenticator)
	, m_httpServer(new OW_HTTPServer)
	, m_httpListenPort(0)
	, m_httpsListenPort(0)
	, m_mutex()
{
	OW_Reference<OW_Array<SelectablePair_t> >
		selectables(new OW_Array<SelectablePair_t>);

	OW_ServiceEnvironmentIFCRef env(new OW_HTTPXMLCIMListenerServiceEnvironment(
		m_pLAuthenticator, m_XMLListener, logger, selectables));

	m_httpServer->setServiceEnvironment(env);
	m_httpServer->startService();  // The http server will add it's server
	// sockets to the environment's selectables, which is really
	// the selectabls defined above.  We'll give these to the select engine thread
	// below which will use them to run the select engine.
	m_httpListenPort = m_httpServer->getLocalHTTPAddress().getPort();
	m_httpsListenPort = m_httpServer->getLocalHTTPSAddress().getPort();

	// start a thread to run the http server
	m_stopHttpPipe = OW_UnnamedPipe::createUnnamedPipe();
    m_httpThread = new SelectEngineThread(selectables, m_stopHttpPipe);
	m_httpThread->start();
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPXMLCIMListener::~OW_HTTPXMLCIMListener()
{
	shutdownHttpServer();
	// unregister all the callbacks from the CIMOMs
	OW_MutexLock lock(m_mutex);
	for (callbackMap_t::iterator i = m_callbacks.begin();
		i != m_callbacks.end(); ++i)
	{
		registrationInfo reg = i->second;

		try
		{
			deleteRegistrationObjects(reg);
		}
		catch (OW_CIMException& ce)
		{
			// if an error occured, then just ignore it.  We don't have any way
			// of logging it!
		}
		catch (OW_HTTPException& e)
		{
			// a network error occured, we can't do anything about it.
		}
		catch (...)
		{
			// who knows what happened, but we need to continue deregistering...
		}
	}
	m_pLAuthenticator = 0;
}

void
OW_HTTPXMLCIMListener::shutdownHttpServer()
{
	if (m_stopHttpPipe)
	{
		// write something into the stop pipe to stop the select engine so the
		// thread will exit
		if (m_stopHttpPipe->write(0) == -1)
		{
			OW_THROW(OW_IOException, "Writing to the termination pipe failed");
		}
		m_stopHttpPipe = 0;
	}

	if (m_httpThread)
	{
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
OW_String
OW_HTTPXMLCIMListener::registerForIndication(
	const OW_String& url,
	const OW_String& ns,
	const OW_String& filter,
	const OW_String& querylanguage,
	OW_CIMListenerCallback& cb)
{
	registrationInfo reg;

	// create an http client with the url from the object path
	OW_URL curl(url);
	reg.cimomUrl = curl;

	OW_CIMProtocolIFCRef client(new OW_HTTPClient(curl.toString()));
	OW_String ipAddress = client->getLocalAddress().getAddress();

	OW_CIMXMLCIMOMHandle hdl(client);

	// If we are connecting to the CIMOM via HTTPS, then assume it will
	// support HTTPS. We'll
	// first try to get a class of CIM_IndicationHandlerXMLHTTPS
	// If we can't get HTTPS then try for HTTP
	// This will be used as the indication handler for all
	// events subscribed to.
	OW_CIMClass delivery;
	OW_String urlPrefix = "https://";
	OW_UInt16 listenerPort = m_httpsListenPort;
	bool useHttps = reg.cimomUrl.protocol.equalsIgnoreCase("https");
	if (useHttps)
	{
		try
		{
			OW_CIMObjectPath cop("CIM_IndicationHandlerXMLHTTPS", ns);
			delivery = hdl.getClass(cop);
		}
		catch (OW_CIMException& e)
		{
			if (e.getErrNo() == OW_CIMException::INVALID_CLASS)
			{
				useHttps = false;
			}
			else
				throw;
		}
	}

	if (!useHttps)
	{
		OW_CIMObjectPath cop("CIM_IndicationHandlerXMLHTTP", ns);
		delivery = hdl.getClass(cop);
		urlPrefix = "http://";
		listenerPort = m_httpListenPort;
	}

	OW_CIMInstance ci = delivery.newInstance();

	OW_MutexLock lock(m_mutex);
	OW_String httpPath;
	OW_RandomNumber rn(0, 0x7FFFFFFF);
	do
	{
		OW_String randomHashValue(rn.getNextNumber());
		httpPath = "/cimListener" + randomHashValue;
	} while (m_callbacks.find(httpPath) != m_callbacks.end());


	reg.httpCredentials = m_pLAuthenticator->getNewCredentials();
	ci.setProperty("Destination", OW_CIMValue(urlPrefix + reg.httpCredentials + "@" +
		ipAddress + ":" + OW_String(OW_UInt32(listenerPort)) + httpPath));

	ci.setProperty("SystemCreationClassName", OW_CIMValue(ipAddress));
	ci.setProperty("SystemName", OW_CIMValue(ipAddress));
	ci.setProperty("CreationClassName", OW_CIMValue(ipAddress));
	ci.setProperty("Name", OW_CIMValue(httpPath));

	OW_CIMObjectPath cop;
	try
	{
		cop = OW_CIMObjectPath(ci.getClassName(), ci.getKeyValuePairs());
		cop.setNameSpace(ns);
		reg.handler = hdl.createInstance(cop, ci);
	}
	catch (OW_CIMException& e)
	{
		// We don't care if it already exists, but err out on anything else
		if (e.getErrNo() != OW_CIMException::ALREADY_EXISTS)
		{
			throw;
		}
		else
		{
			reg.handler = cop;
		}
	}
	
	// get class of CIM_IndicationFilter and new instance of it
	cop = OW_CIMObjectPath("CIM_IndicationFilter", ns);
	OW_CIMClass cimFilter = hdl.getClass(cop, true);
	ci = cimFilter.newInstance();

	// set Query property to query that was passed into function
	ci.setProperty("Query", OW_CIMValue(filter));

	// set QueryLanguage property
	ci.setProperty("QueryLanguage", OW_CIMValue(querylanguage));

	ci.setProperty("SystemCreationClassName", OW_CIMValue(ipAddress));
	ci.setProperty("SystemName", OW_CIMValue(ipAddress));
	ci.setProperty("CreationClassName", OW_CIMValue(ipAddress));
	ci.setProperty("Name", OW_CIMValue(httpPath));
	// create instance of filter
	cop = OW_CIMObjectPath(ci.getClassName(), ci.getKeyValuePairs());
	cop.setNameSpace(ns);
	reg.filter = hdl.createInstance(cop, ci);

	// get class of CIM_IndicationSubscription and new instance of it.
	// CIM_IndicationSubscription is an association class that connects
	// the IndicationFilter to the IndicationHandler.
	cop.setObjectName("CIM_IndicationSubscription");
	OW_CIMClass cimClientFilterDelivery = hdl.getClass(cop, true);
	ci = cimClientFilterDelivery.newInstance();

	// set the properties for the filter and the handler
	ci.setProperty("filter", OW_CIMValue(reg.filter));
	ci.setProperty("handler", OW_CIMValue(reg.handler));

	// creating the instance the CIM_IndicationSubscription creates
	// the event subscription
	cop = OW_CIMObjectPath(ci.getClassName(), ci.getKeyValuePairs());
	cop.setNameSpace(ns);
	reg.subscription = hdl.createInstance(cop, ci);

	//save info for deletion later and callback delivery
	reg.callback = &cb;
	reg.ns = ns;

	m_callbacks[httpPath] = reg;

	return httpPath;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPXMLCIMListener::deregisterForIndication( const OW_String& handle )
{
	OW_MutexLock lock(m_mutex);
	callbackMap_t::iterator i = m_callbacks.find(handle);
	if (i != m_callbacks.end())
	{
		registrationInfo reg = i->second;
		m_callbacks.erase(i);
		lock.release();

		m_pLAuthenticator->removeCredentials(reg.httpCredentials);

		deleteRegistrationObjects(reg);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPXMLCIMListener::doIndicationOccurred( OW_CIMInstance& ci,
		const OW_String& listenerPath )
{
	OW_CIMListenerCallback* cb;
	{ // scope for the OW_MutexLock
		OW_MutexLock lock(m_mutex);
		callbackMap_t::iterator i = m_callbacks.find(listenerPath);
		if (i == m_callbacks.end())
		{
			OW_THROWCIMMSG(OW_CIMException::ACCESS_DENIED,
				format("No listener for path: %1", listenerPath).c_str());
		}

		cb = i->second.callback;
	}

	cb->indicationOccurred( ci, listenerPath );
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPXMLCIMListener::deleteRegistrationObjects( const registrationInfo& reg )
{
	OW_CIMProtocolIFCRef client(new OW_HTTPClient(reg.cimomUrl.toString()));
	OW_CIMXMLCIMOMHandle hdl(client);

	hdl.deleteInstance(reg.ns, reg.subscription);
	hdl.deleteInstance(reg.ns, reg.filter);
	hdl.deleteInstance(reg.ns, reg.handler);
}

