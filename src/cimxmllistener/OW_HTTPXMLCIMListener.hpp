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

#ifndef OW_HTTPXMLCIMLISTENER_HPP_
#define OW_HTTPXMLCIMLISTENER_HPP_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Map.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_CIMListenerCallback.hpp"
#include "OW_URL.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Mutex.hpp"
#include "OW_ClientAuthCBIFC.hpp"

namespace OpenWBEM
{

class HTTPServer;
class ListenerAuthenticator;
class Thread;

class OW_LISTENER_API HTTPXMLCIMListener : public CIMListenerCallback
{
public:
	/**
	 * @param logger If a logger specified then it will receive log messages, otherwise
	 *  all log messages will be discarded.
	 */
	HTTPXMLCIMListener(const LoggerRef& logger = LoggerRef(0));
	~HTTPXMLCIMListener();
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
		const ClientAuthCBIFCRef& authCb = ClientAuthCBIFCRef());

	/**
	 * De-register for an indication
	 * @param handle The string returned from registerForIndication
	 * @param authCb If authentication is necessary, and authCb != NULL, then
	 *  authCb->getCredentials() will be called to obtain credentials.
	 */
	void deregisterForIndication( const String& handle );

	/**
	 * Shut down the http server that is listening for indications.
	 *	This function blocks until all threads that are running callbacks
	 *	have terminated.
	 */
	void shutdownHttpServer();
protected:
	virtual void doIndicationOccurred( CIMInstance& ci,
		const String& listenerPath );
private:
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
	RequestHandlerIFCRef m_XMLListener;
	IntrusiveReference<ListenerAuthenticator> m_pLAuthenticator;
	IntrusiveReference<HTTPServer> m_httpServer;
	UInt16 m_httpListenPort;
	UInt16 m_httpsListenPort;
	void deleteRegistrationObjects( const registrationInfo& reg );
	Mutex m_mutex;
	IntrusiveReference<Thread> m_httpThread;
	UnnamedPipeRef m_stopHttpPipe;
};

} // end namespace OpenWBEM

#endif
