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

#ifndef OW_HTTPSERVER_HPP_
#define OW_HTTPSERVER_HPP_

#include "OW_config.h"
#include "OW_Semaphore.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_String.hpp"
#include "OW_Thread.hpp"
#include "OW_ThreadEvent.hpp"
#include "OW_Map.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_URL.hpp"
#include "OW_CIMOMLocatorSLP.hpp"
#include <ctime>

class OW_InetServerSocket;
class OW_IPCHandler;
class OW_HTTPSvrConnection;
class OW_DigestAuthentication;
class OW_HTTPServer;

#ifdef OW_HAVE_SLP_H
class HTTPSlpRegistrator : public OW_Runnable
{
public:
	HTTPSlpRegistrator(OW_HTTPServer* pServer)
		: OW_Runnable()
		, m_pServer(pServer)
		, m_shuttingDown(false)
		, m_isRunning(false)
		, m_threadEvent()
	{
	}

	~HTTPSlpRegistrator() { shutdown(); }

	void shutdown();
	void start();
	virtual void run();

private:
	OW_HTTPServer* m_pServer;
	OW_Bool m_shuttingDown;
	OW_Bool m_isRunning;
	OW_ThreadEvent m_threadEvent;
};
#else
class HTTPSlpRegistrator
{
public:
	HTTPSlpRegistrator(OW_HTTPServer*) {}
	void shutdown() {}
	void start() {}
};
#endif	// OW_HAVE_SLP_H


class OW_HTTPServer : public OW_ServiceIFC
{
public:
	OW_HTTPServer();
	virtual ~OW_HTTPServer();

	virtual void setServiceEnvironment(OW_ServiceEnvironmentIFCRef env);
	virtual void startService();

	/**
	 * Shutdown the http server.  This function does not return
	 * untill all connections have been terminated, and cleaned up.
	 */
	virtual void shutdown();

	/**
	 * Get the URLs associated with this http server.  This is used
	 * by slp discovery.
	 * @return an array of OW_URLs representing all urls that can be
	 * 	used to access the HTTP server.
	 */
	OW_Array<OW_URL> getURLs() const;

	OW_ServiceEnvironmentIFCRef getEnvironment() const { return m_options.env; }

	/**
	 * Add a new url (to be returned by getURLs())
	 * @param url the URL to be added
	 */
	void addURL(const OW_URL& url);

	OW_SocketAddress getLocalHTTPAddress();
	OW_SocketAddress getLocalHTTPSAddress();
	
	
	struct Options
	{
		OW_Int32 httpPort;
		OW_Int32 httpsPort;
		OW_Int32 maxConnections;
		bool isSepThread;
		bool enableDeflate;
		bool useDigest;
		bool allowAnonymous;
		OW_ServiceEnvironmentIFCRef env;
	};


#ifdef OW_HAVE_SLP_H
	void doSlpRegister();
	static void slpRegReport(SLPHandle, SLPError, void*);
#endif

private:

	OW_Bool authenticate(OW_HTTPSvrConnection* pconn,
		OW_String& userName, const OW_String& info);
	OW_Mutex m_guard;

	void decThreadCount();
	void incThreadCount();

	Options m_options;

	OW_AutoPtrNoVec<OW_Semaphore> m_threadCountSemaphore;
	OW_Reference<OW_UnnamedPipe> m_upipe;
	OW_Array<OW_URL> m_urls;

	OW_Reference<OW_InetServerSocket> m_pHttpServerSocket;
	OW_Reference<OW_InetServerSocket> m_pHttpsServerSocket;
	OW_Reference<OW_DigestAuthentication> m_digestAuth;
	OW_Mutex m_authGuard;
	HTTPSlpRegistrator m_slpRegistrator;

	friend class OW_HTTPSvrConnection;
	friend class OW_HTTPListener;
	friend class OW_IPCConnectionHandler;
	friend class OW_HTTPServerSelectableCallback;
};

#endif // _OW_HTTPSERVER_HPP__
