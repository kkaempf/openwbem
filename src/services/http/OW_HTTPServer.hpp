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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_HTTPSERVER_HPP_INCLUDE_GUARD_
#define OW_HTTPSERVER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Mutex.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_URL.hpp"
#include "OW_ThreadPool.hpp"
#include "OW_Exception.hpp"
#include <ctime>

namespace OpenWBEM
{

class ServerSocket;
class IPCHandler;
class HTTPSvrConnection;
#ifndef OW_DISABLE_DIGEST
class DigestAuthentication;
#endif
class HTTPServer;
class UnnamedPipe;
class LocalAuthentication;

OW_DECLARE_EXCEPTION(HTTPServer)

class HTTPServer : public ServiceIFC
{
public:
	HTTPServer();
	virtual ~HTTPServer();
	virtual void setServiceEnvironment(ServiceEnvironmentIFCRef env);
	virtual void startService();
	/**
	 * Shutdown the http server.  This function does not return
	 * untill all connections have been terminated, and cleaned up.
	 */
	virtual void shutdown();
	/**
	 * Get the URLs associated with this http server.  This is used
	 * by slp discovery.
	 * @return an array of URLs representing all urls that can be
	 * 	used to access the HTTP server.
	 */
	Array<URL> getURLs() const;
	ServiceEnvironmentIFCRef getEnvironment() const { return m_options.env; }
	/**
	 * Add a new url (to be returned by getURLs())
	 * @param url the URL to be added
	 */
	void addURL(const URL& url);
	SocketAddress getLocalHTTPAddress();
	SocketAddress getLocalHTTPSAddress();
	
	
	struct Options
	{
		Int32 httpPort;
		Int32 httpsPort;
		String UDSFilename;
		Int32 maxConnections;
		bool isSepThread;
		bool enableDeflate;
		bool useDigest;
		bool allowAnonymous;
		bool useUDS;
		bool reuseAddr;
		ServiceEnvironmentIFCRef env;
		Int32 timeout;
		bool useLocalAuthentication;
	};
private:
	bool authenticate(HTTPSvrConnection* pconn,
		String& userName, const String& info, OperationContext& context);
	Mutex m_guard;
	Options m_options;
	Reference<UnnamedPipe> m_upipe;
	Array<URL> m_urls;
	Reference<ServerSocket> m_pHttpServerSocket;
	Reference<ServerSocket> m_pHttpsServerSocket;
	Reference<ServerSocket> m_pUDSServerSocket;
#ifndef OW_DISABLE_DIGEST
	Reference<DigestAuthentication> m_digestAuthentication;
#endif
	Reference<LocalAuthentication> m_localAuthentication;
	Mutex m_authGuard;
	ThreadPoolRef m_threadPool;
	friend class HTTPSvrConnection;
	friend class HTTPListener;
	friend class IPCConnectionHandler;
	friend class HTTPServerSelectableCallback;
};

} // end namespace OpenWBEM

#endif
