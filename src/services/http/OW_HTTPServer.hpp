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

#ifndef OW_HTTPSERVER_HPP_INCLUDE_GUARD_
#define OW_HTTPSERVER_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Condition.hpp"
#include "OW_Mutex.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_String.hpp"
#include "OW_Thread.hpp"
#include "OW_Map.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_URL.hpp"
#include "OW_ThreadCounter.hpp"
#include <ctime>

class OW_ServerSocket;
class OW_IPCHandler;
class OW_HTTPSvrConnection;
class OW_DigestAuthentication;
class OW_HTTPServer;



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
		bool useUDS;
		bool reuseAddr;
		OW_ServiceEnvironmentIFCRef env;
	};


private:

	OW_Bool authenticate(OW_HTTPSvrConnection* pconn,
		OW_String& userName, const OW_String& info);
	OW_Mutex m_guard;

	Options m_options;

	OW_ThreadCounterRef m_threadCount;

	OW_Reference<OW_UnnamedPipe> m_upipe;
	OW_Array<OW_URL> m_urls;

	OW_Reference<OW_ServerSocket> m_pHttpServerSocket;
	OW_Reference<OW_ServerSocket> m_pHttpsServerSocket;
	OW_Reference<OW_ServerSocket> m_pUDSServerSocket;
	OW_Reference<OW_DigestAuthentication> m_digestAuth;
	OW_Mutex m_authGuard;

	friend class OW_HTTPSvrConnection;
	friend class OW_HTTPListener;
	friend class OW_IPCConnectionHandler;
	friend class OW_HTTPServerSelectableCallback;
};

#endif
