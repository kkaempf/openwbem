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

#ifndef OW_HTTPXMLCIMLISTENER_HPP_
#define OW_HTTPXMLCIMLISTENER_HPP_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Map.hpp"
#include "OW_Reference.hpp"
#include "OW_CIMListenerCallback.hpp"
#include "OW_URL.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_Thread.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Mutex.hpp"

class OW_HTTPServer;
class OW_ListenerAuthenticator;

class OW_HTTPXMLCIMListener : public OW_CIMListenerCallback
{
public:
	// If a logger specified then it will receive log messages, otherwise
	// all log messages will be discarded.
	OW_HTTPXMLCIMListener(OW_LoggerRef logger = OW_LoggerRef(0));

	~OW_HTTPXMLCIMListener();


	/**
	 * Register for an indication.
	 * @param op The namespace where the
	 *            indication subscription and related objects will be created.
	 * @param filter The filter for the indication subscription
	 * @param querylanguage The language of the filter (typically wql1)
	 * @param cb An object derived from OW_CIMListenerCallback.  When an
	 *		indication is received, the doIndicationOccured member function will be called
	 *
	 * @return A unique handle identifying the indication subscription and callback.
	 *		Use this handle to de-register the listener.
	 */
	OW_String registerForIndication(const OW_String& url,
		const OW_String& ns, const OW_String& filter,
		const OW_String& querylanguage, 
        const OW_String& sourceNamespace,
        OW_CIMListenerCallbackRef cb);

	/**
	 * De-register for an indication
	 * @param handle The string returned from registerForIndication
	 */
	void deregisterForIndication( const OW_String& handle );

	/**
	 * Shut down the http server that is listening for indications.
	 *	This function blocks until all threads that are running callbacks
	 *	have terminated.
	 */
	void shutdownHttpServer();

protected:

	virtual void doIndicationOccurred( OW_CIMInstance& ci,
		const OW_String& listenerPath );

private:

	struct registrationInfo
	{
		registrationInfo()
			: handler(OW_CIMNULL)
			, filter(OW_CIMNULL)
			, subscription(OW_CIMNULL)
		{}

		OW_URL cimomUrl;
		OW_String ns;
		OW_CIMObjectPath handler;
		OW_CIMObjectPath filter;
		OW_CIMObjectPath subscription;
		OW_CIMListenerCallbackRef callback;
		OW_String httpCredentials;
	};

	typedef OW_Map< OW_String, registrationInfo > callbackMap_t;

	callbackMap_t m_callbacks;
	OW_RequestHandlerIFCRef m_XMLListener;
	OW_Reference<OW_ListenerAuthenticator> m_pLAuthenticator;
	OW_Reference<OW_HTTPServer> m_httpServer;
	OW_UInt16 m_httpListenPort;
	OW_UInt16 m_httpsListenPort;

	void deleteRegistrationObjects( const registrationInfo& reg );

	OW_Mutex m_mutex;
	OW_Reference<OW_Thread> m_httpThread;
	OW_UnnamedPipeRef m_stopHttpPipe;
};


#endif
