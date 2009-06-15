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
#include "OW_CIMListenerCallback.hpp"
#include "OW_HTTPServer.hpp"
#include "OW_XMLListener.hpp"
#include "OW_HTTPException.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/MutexLock.hpp"
#include "OW_ListenerAuthenticator.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "blocxx/RandomNumber.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/ServerSocket.hpp"
#include "blocxx/SelectEngine.hpp"
#include "blocxx/SelectableIFC.hpp"
#include "blocxx/SelectableCallbackIFC.hpp"
#include "OW_IOException.hpp"
#include "blocxx/Thread.hpp"
#include "OW_Assertion.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_CIMProtocolIFC.hpp"
#include "blocxx/NullLogger.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_ConfigFile.hpp"

#include <algorithm> // for std::remove

namespace OW_NAMESPACE
{

using namespace WBEMFlags;

// This anonymous namespace has the effect of giving this class internal
// linkage.  That means it won't cause a link error if another translation
// unit has a class with the same name.
namespace
{

const String COMPONENT_NAME("ow.listener.cimxml");

#ifdef OW_WIN32
class EventSelectable : public SelectableIFC
{
public:
	EventSelectable()
		: SelectableIFC()
		, m_event(NULL)
	{
		if(!(m_event = ::CreateEvent(NULL, TRUE, FALSE, NULL)))
		{
			OW_THROW(IOException, "Unable to create WIN32 event handle");
		}
	}
	~EventSelectable()
	{
		if(m_event)
		{
			::CloseHandle(m_event);
		}
	}
	Select_t getSelectObj() const
	{
		Select_t st;
		st.event = m_event;
		return st;
	}

	Select_t getReadSelectObj() const
	{
		return getSelectObj();
	}

	void setEvent()
	{
		if(m_event)
		{
			if(!::SetEvent(m_event))
			{
				OW_THROW(IOException, "Signaling termination event failed");
			}
		}
	}

	void resetEvent()
	{
		if(m_event)
		{
			::ResetEvent(m_event);
		}
	}

private:
	HANDLE m_event;
};
typedef IntrusiveReference<EventSelectable> EventSelectableRef;

#endif

typedef std::pair<SelectableIFCRef, SelectableCallbackIFCRef> SelectablePair_t;
class HTTPXMLCIMListenerServiceEnvironment : public ServiceEnvironmentIFC
{
public:
	HTTPXMLCIMListenerServiceEnvironment(
		IntrusiveReference<ListenerAuthenticator> authenticator,
		RequestHandlerIFCRef listener,
		Reference<Array<SelectablePair_t> > selectables,
		const String& certFileName,
		const String& keyFileName = String())
	: m_pLAuthenticator(authenticator)
	, m_XMLListener(listener)
	, m_selectables(selectables)
	{
		if(certFileName.empty())
		{
			setConfigItem(ConfigOpts::HTTP_SERVER_HTTP_PORT_opt, String(0), E_OVERWRITE_PREVIOUS);
			setConfigItem(ConfigOpts::HTTP_SERVER_HTTPS_PORT_opt, String(-1), E_OVERWRITE_PREVIOUS);
		}
		else
		{
			setConfigItem(ConfigOpts::HTTP_SERVER_HTTP_PORT_opt, String(-1), E_OVERWRITE_PREVIOUS);
			setConfigItem(ConfigOpts::HTTP_SERVER_HTTPS_PORT_opt, String(0), E_OVERWRITE_PREVIOUS);
			setConfigItem(ConfigOpts::HTTP_SERVER_SSL_CERT_opt, certFileName, E_OVERWRITE_PREVIOUS);
			setConfigItem(ConfigOpts::HTTP_SERVER_SSL_KEY_opt,
					keyFileName.empty()?certFileName:keyFileName, E_OVERWRITE_PREVIOUS);
		}

		setConfigItem(ConfigOpts::HTTP_SERVER_MAX_CONNECTIONS_opt, String(10), E_OVERWRITE_PREVIOUS);
		setConfigItem(ConfigOpts::HTTP_SERVER_SINGLE_THREAD_opt, "false", E_OVERWRITE_PREVIOUS);
		setConfigItem(ConfigOpts::HTTP_SERVER_ENABLE_DEFLATE_opt, "true", E_OVERWRITE_PREVIOUS);
		setConfigItem(ConfigOpts::HTTP_SERVER_USE_DIGEST_opt, "false", E_OVERWRITE_PREVIOUS);
		setConfigItem(ConfigOpts::HTTP_SERVER_USE_UDS_opt, "false", E_OVERWRITE_PREVIOUS);
	}
	virtual ~HTTPXMLCIMListenerServiceEnvironment() {}
	virtual bool authenticate(String &userName,
		const String &info, String &details, OperationContext& context) const
	{
		return m_pLAuthenticator->authenticate(userName, info, details, context);
	}
	virtual void addSelectable(const SelectableIFCRef& obj,
		const SelectableCallbackIFCRef& cb)
	{
		m_selectables->push_back(std::make_pair(obj, cb));
	}

	struct selectableFinder
	{
		selectableFinder(const SelectableIFCRef& obj) : m_obj(obj) {}
		template <typename T>
		bool operator()(const T& x)
		{
			return x.first == m_obj;
		}
		const SelectableIFCRef& m_obj;
	};

	virtual void removeSelectable(const SelectableIFCRef& obj)
	{
		m_selectables->erase(std::remove_if (m_selectables->begin(), m_selectables->end(),
			selectableFinder(obj)), m_selectables->end());
	}
	virtual String getConfigItem(const String &name, const String& defRetVal) const
	{
		return ConfigFile::getConfigItem(m_configItems, name, defRetVal);
	}
	virtual StringArray getMultiConfigItem(const String &itemName, 
		const StringArray& defRetVal, const char* tokenizeSeparator) const
	{
		return ConfigFile::getMultiConfigItem(m_configItems, itemName, defRetVal, tokenizeSeparator);
	}
	virtual void setConfigItem(const String& item, const String& value, EOverwritePreviousFlag overwritePrevious)
	{
		ConfigFile::setConfigItem(m_configItems, item, value, 
			overwritePrevious == E_OVERWRITE_PREVIOUS ? ConfigFile::E_OVERWRITE_PREVIOUS : ConfigFile::E_PRESERVE_PREVIOUS);
	}
	
	virtual RequestHandlerIFCRef getRequestHandler(const String&) const
	{
		RequestHandlerIFCRef ref(m_XMLListener.getLibRef(),
				m_XMLListener->clone());
		ref->init(ServiceEnvironmentIFCRef(const_cast<HTTPXMLCIMListenerServiceEnvironment*>(this)));
		return ref;
	}
private:
	ConfigFile::ConfigMap m_configItems;
	IntrusiveReference<ListenerAuthenticator> m_pLAuthenticator;
	RequestHandlerIFCRef m_XMLListener;
	Reference<Array<SelectablePair_t> > m_selectables;
};
class SelectEngineThread : public Thread
{
public:
	SelectEngineThread(const Reference<Array<SelectablePair_t> >& selectables)
	: Thread()
	, m_selectables(selectables)
#ifdef OW_WIN32
	, m_stopObject(new EventSelectable)
	{
	}
#else
	, m_stopObject(UnnamedPipe::createUnnamedPipe())
	{
		m_stopObject->setBlocking(UnnamedPipe::E_NONBLOCKING);
	}
#endif
	/**
	 * The method that will be run when the start method is called on this
	 * Thread object.
	 */
	virtual Int32 run()
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

		engine.go(Timeout::infinite);
		return 0;
	}
	virtual void doShutdown()
	{
#ifdef OW_WIN32
		// signal the event to stop the select engine so the
		// thread will exit
		m_stopObject->setEvent();
#else
		// write something into the stop pipe to stop the select engine so the
		// thread will exit
		if (m_stopObject->writeInt(0) == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, "Writing to the termination pipe failed");
		}
#endif
	}

private:
	Reference<Array<SelectablePair_t> > m_selectables;
#ifdef OW_WIN32
	EventSelectableRef m_stopObject;
#else
	UnnamedPipeRef m_stopObject;
#endif
};



} // end anonymous namespace

class HTTPXMLCIMListenerCallback : public CIMListenerCallback
{
private: 
	IntrusiveReference<ListenerAuthenticator> m_pLAuthenticator;
	Bool m_useHTTPS; 
public:
	/**
	 * @param logger If a logger specified then it will receive log messages, otherwise
	 *  all log messages will be discarded.
	 */
	HTTPXMLCIMListenerCallback(IntrusiveReference<ListenerAuthenticator> authenticator,
		Bool useHTTPS = false)
		: m_pLAuthenticator(authenticator)
		, m_useHTTPS(useHTTPS) 
	{
	}
	~HTTPXMLCIMListenerCallback()
	{
		try
		{
			MutexLock lock(m_mutex);
			for (callbackMap_t::iterator i = m_callbacks.begin();
				i != m_callbacks.end(); ++i)
			{
				registrationInfo reg = i->second;
		
				try
				{
					deleteRegistrationObjects(reg);
				}
				catch (Exception&)
				{
					// if an error occured, then just ignore it.  We don't have any way
					// of logging it!
				}
				catch (...)
				{
					// who knows what happened, but we need to continue deregistering...
				}
			}
			m_pLAuthenticator = 0;
		}
		catch (...)
		{
			// don't let exceptions escape
		}
	}
	/**
	 * Register for an indication.  The destructor will attempt to deregister
	 * any subscriptions which are still outstanding at the time.
	 * @param url The url identifying the cimom
	 * @param ns The namespace where the
	 *  indication subscription and related objects will be created.
	 * @param filter The filter for the indication subscription
	 * @param querylanguage The language of the filter (typically wql1)
	 * @param sourceNamespace The path to a local namespace where the Indications
	 *  originate. If empty, the namespace of the Filter registration
	 *  is assumed.
	 * @param cb An object derived from CIMListenerCallback.  When an
	 *	indication is received, the doIndicationOccured member function will be called
	 * @param authCb If authentication is necessary, and authCb != NULL, then
	 *  authCb->getCredentials() will be called to obtain credentials.
	 *
	 * @return A unique handle identifying the indication subscription and callback.
	 *		Use this handle to de-register the listener.
	 */
	String registerForIndication(const String& url,
			const String& ns, const String& filter,
			const String& querylanguage,
			const String& sourceNamespace,
			const CIMListenerCallbackRef& cb,
			const ClientAuthCBIFCRef& authCb,
			UInt16 httpPort, 
			UInt16 httpsPort)
	{
		registrationInfo reg;
		// create an http client with the url from the object path
		URL curl(url);
		reg.cimomUrl = curl;
		ClientCIMOMHandleRef hdl = ClientCIMOMHandle::createFromURL(url, authCb);
		String ipAddress = hdl->getWBEMProtocolHandler()->getLocalAddress().getAddress();
		CIMClass delivery(CIMNULL);
		String urlPrefix;

		UInt16 listenerPort = httpsPort;
		if(m_useHTTPS)	// Listener will be recieving over https
		{
			urlPrefix = "https://";
			try
			{
				delivery = hdl->getClass(ns, "CIM_IndicationHandlerCIMXML");
			}
			catch (CIMException& e)
			{
				if (e.getErrNo() == CIMException::NOT_FOUND)
				{
					// the > 2.6 doesn't exist, try to get the 2.5 class
					delivery = hdl->getClass(ns, "CIM_IndicationHandlerXMLHTTPS");
				}
				else
					throw;
			}
		}
		else
		{
			try
			{
				delivery = hdl->getClass(ns, "CIM_IndicationHandlerCIMXML");
			}
			catch (CIMException& e)
			{
				if (e.getErrNo() == CIMException::NOT_FOUND)
				{
					// the > 2.6 doesn't exist, try to get the 2.5 class
					delivery = hdl->getClass(ns, "CIM_IndicationHandlerXMLHTTP");
				}
				else
					throw;
			}
			urlPrefix = "http://";
			listenerPort = httpPort;
		}
		CIMInstance ci = delivery.newInstance();
		MutexLock lock(m_mutex);
		String httpPath;
		RandomNumber rn(0, 0x7FFFFFFF);
		do
		{
			String randomHashValue(rn.getNextNumber());
			httpPath = "/cimListener" + randomHashValue;
		} while (m_callbacks.find(httpPath) != m_callbacks.end());
		reg.httpCredentials = m_pLAuthenticator->getNewCredentials();
		ci.setProperty("Destination", CIMValue(urlPrefix + reg.httpCredentials + "@" +
					ipAddress + ":" + String(UInt32(listenerPort)) + httpPath));
		ci.setProperty("SystemCreationClassName", CIMValue("CIM_System"));
		ci.setProperty("SystemName", CIMValue(ipAddress));
		ci.setProperty("CreationClassName", CIMValue(delivery.getName()));
		ci.setProperty("Name", CIMValue(httpPath));
		ci.setProperty("Owner", CIMValue("HTTPXMLCIMListener on " + ipAddress));
		try
		{
			reg.handler = hdl->createInstance(ns, ci);
		}
		catch (CIMException& e)
		{
			// We don't care if it already exists, but err out on anything else
			if (e.getErrNo() != CIMException::ALREADY_EXISTS)
			{
				throw;
			}
			else
			{
				reg.handler = CIMObjectPath(ns, ci);
			}
		}
		// get class of CIM_IndicationFilter and new instance of it
		CIMClass cimFilter = hdl->getClass(ns, "CIM_IndicationFilter", E_LOCAL_ONLY);
		ci = cimFilter.newInstance();
		// set Query property to query that was passed into function
		ci.setProperty("Query", CIMValue(filter));
		// set QueryLanguage property
		ci.setProperty("QueryLanguage", CIMValue(querylanguage));
		ci.setProperty("SystemCreationClassName", CIMValue("CIM_System"));
		ci.setProperty("SystemName", CIMValue(ipAddress));
		ci.setProperty("CreationClassName", CIMValue(cimFilter.getName()));
		ci.setProperty("Name", CIMValue(httpPath));
		if (!sourceNamespace.empty())
		{
			ci.setProperty("SourceNamespace", CIMValue(sourceNamespace));
		}
		// create instance of filter
		reg.filter = hdl->createInstance(ns, ci);
		// get class of CIM_IndicationSubscription and new instance of it.
		// CIM_IndicationSubscription is an association class that connects
		// the IndicationFilter to the IndicationHandler.
		CIMClass cimClientFilterDelivery = hdl->getClass(ns,
				"CIM_IndicationSubscription", E_LOCAL_ONLY);
		ci = cimClientFilterDelivery.newInstance();
		// set the properties for the filter and the handler
		ci.setProperty("filter", CIMValue(reg.filter));
		ci.setProperty("handler", CIMValue(reg.handler));
		// creating the instance the CIM_IndicationSubscription creates
		// the event subscription
		reg.subscription = hdl->createInstance(ns, ci);
		//save info for deletion later and callback delivery
		reg.callback = cb;
		reg.ns = ns;
		reg.authCb = authCb;
		m_callbacks[httpPath] = reg;
		return httpPath;
	}

	/**
	 * De-register for an indication
	 * @param handle The string returned from registerForIndication
	 * @param authCb If authentication is necessary, and authCb != NULL, then
	 *  authCb->getCredentials() will be called to obtain credentials.
	 */
	void deregisterForIndication( const String& handle )
	{
		MutexLock lock(m_mutex);
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

protected:
	virtual void doIndicationOccurred( CIMInstance& ci,
			const String& listenerPath )
	{
		CIMListenerCallbackRef cb;
		{ // scope for the MutexLock
			MutexLock lock(m_mutex);
			callbackMap_t::iterator i = m_callbacks.find(listenerPath);
			if (i == m_callbacks.end())
			{
				OW_THROWCIMMSG(CIMException::ACCESS_DENIED,
						Format("No listener for path: %1", listenerPath).c_str());
			}
			cb = i->second.callback;
		}
		cb->indicationOccurred( ci, listenerPath );
	}
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	struct registrationInfo
	{
		registrationInfo()
			: handler(CIMNULL)
			, filter(CIMNULL)
			, subscription(CIMNULL)
		{}
		URL cimomUrl;
		String ns;
		CIMObjectPath handler;
		CIMObjectPath filter;
		CIMObjectPath subscription;
		CIMListenerCallbackRef callback;
		String httpCredentials;
		ClientAuthCBIFCRef authCb;
	};
	typedef Map< String, registrationInfo > callbackMap_t;
	callbackMap_t m_callbacks;
	void deleteRegistrationObjects( const registrationInfo& reg )
	{
		ClientCIMOMHandleRef hdl = ClientCIMOMHandle::createFromURL(reg.cimomUrl.toString(), reg.authCb);
		hdl->deleteInstance(reg.ns, reg.subscription);
		hdl->deleteInstance(reg.ns, reg.filter);
		hdl->deleteInstance(reg.ns, reg.handler);
	}
	Mutex m_mutex;
};



//////////////////////////////////////////////////////////////////////////////
HTTPXMLCIMListener::HTTPXMLCIMListener(
	const String& certFileName, const String& keyFileName)
	: m_pLAuthenticator(new ListenerAuthenticator)
	, m_httpServer(new HTTPServer)
	, m_httpListenPort(0)
	, m_httpsListenPort(0)
	, m_certFileName(certFileName)
	, m_keyFileName(keyFileName)
	, m_callback(new HTTPXMLCIMListenerCallback(m_pLAuthenticator, !certFileName.empty()))
	, m_XMLListener(SharedLibraryRef(0), new XMLListener(m_callback))
{
	if(!certFileName.empty())
	{
		if(!FileSystem::canRead(certFileName))
		{
			OW_THROW_ERRNO_MSG(IOException, 
				Format("Unable to open certificate file %1",
					certFileName).c_str());
		}
	}
	if(!keyFileName.empty())
	{
		if(!FileSystem::canRead(keyFileName))
		{
			OW_THROW(IOException, 
				Format("Unable to open key file %1",
					keyFileName).c_str());
		}
	}

	Reference<Array<SelectablePair_t> >
		selectables(new Array<SelectablePair_t>);
	ServiceEnvironmentIFCRef env(new HTTPXMLCIMListenerServiceEnvironment(
		m_pLAuthenticator, m_XMLListener, selectables, certFileName, keyFileName));
	m_httpServer->init(env);
	m_httpServer->start();  // The http server will add it's server
	// sockets to the environment's selectables, which is really
	// the selectabls defined above.  We'll give these to the select engine thread
	// below which will use them to run the select engine.
	m_httpListenPort = m_httpServer->getLocalHTTPAddress().getPort();
	m_httpsListenPort = m_httpServer->getLocalHTTPSAddress().getPort();
	// start a thread to run the http server
	m_httpThread = new SelectEngineThread(selectables);
	m_httpThread->start();
}
//////////////////////////////////////////////////////////////////////////////
HTTPXMLCIMListener::~HTTPXMLCIMListener()
{
	try
	{
		shutdownHttpServer();
		m_pLAuthenticator = 0;
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
void
HTTPXMLCIMListener::shutdownHttpServer()
{
	if (m_httpThread)
	{
		m_httpThread->shutdown(Timeout::relative(1));
		m_httpThread->cooperativeCancel();
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
String
HTTPXMLCIMListener::registerForIndication(
	const String& url,
	const String& ns,
	const String& filter,
	const String& querylanguage,
	const String& sourceNamespace,
	const CIMListenerCallbackRef& cb,
	const ClientAuthCBIFCRef& authCb)
{
	return m_callback->registerForIndication(url,ns,filter,querylanguage,
									  sourceNamespace,cb,authCb,
									  m_httpListenPort,m_httpsListenPort); 
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPXMLCIMListener::deregisterForIndication( const String& handle )
{
	m_callback->deregisterForIndication(handle); 
}

} // end namespace OW_NAMESPACE

