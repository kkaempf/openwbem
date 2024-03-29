/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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

#ifndef OW_HTTPSERVER_HPP_INCLUDE_GUARD_
#define OW_HTTPSERVER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/Mutex.hpp"
#include "blocxx/SocketAddress.hpp"
#include "blocxx/Socket.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_URL.hpp"
#include "blocxx/ThreadPool.hpp"
#include "OW_Exception.hpp"
#include "blocxx/SortedVectorSet.hpp"
#include "blocxx/SSLCtxMgr.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_ServicesHttpFwd.hpp"
#include "blocxx/Timeout.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(HTTPServer)

class OW_HTTPSVC_API HTTPServer : public ServiceIFC
{
public:
	HTTPServer();
	virtual ~HTTPServer();
	virtual blocxx::String getName() const;
	virtual blocxx::StringArray getDependencies() const;

	virtual void init(const ServiceEnvironmentIFCRef& env);
	virtual void start();
	/**
	 * Shutdown the http server.  This function does not return
	 * untill all connections have been terminated, and cleaned up.
	 */
	virtual void shutdown();

	/**
	 * Is the server in the process of shutting down?
	 * @return true if shutting down
	 */
	bool isShuttingDown();
	/**
	 * Get the URLs associated with this http server.  This is used
	 * by slp discovery.
	 * @return an array of URLs representing all urls that can be
	 * 	used to access the HTTP server.
	 */
	blocxx::Array<URL> getURLs() const;
	ServiceEnvironmentIFCRef getEnvironment() const { return m_options.env; }
	/**
	 * Add a new url (to be returned by getURLs())
	 * @param url the URL to be added
	 */
	void addURL(const URL& url);
	blocxx::SocketAddress getLocalHTTPAddress();
	blocxx::Array<blocxx::SocketAddress> getLocalHTTPAddresses();
	blocxx::SocketAddress getLocalHTTPSAddress();
	blocxx::Array<blocxx::SocketAddress> getLocalHTTPSAddresses();

	enum EAuthenticationChallengeMethod
	{
		E_DIGEST,
		E_BASIC,
		E_OWLOCAL,
		E_SPNEGO
	};

	struct Options
	{
		Options()
			: httpPorts()
			, httpsPorts()
			, UDSFilename()
			, maxConnections(0)
			, isSepThread(true)
			, enableDeflate(false)
			, defaultAuthChallenge(E_DIGEST)
			, allowDigestAuthentication(false)
			, allowBasicAuthentication(false)
			, allowLocalAuthentication(false)
			, allowSPNEGOAuthentication(false)
			, allowAnonymous(false)
			, useUDS(false)
			, reuseAddr(false)
			, env()
			, timeout(blocxx::Timeout::infinite)
			, defaultContentLanguage()
			, authenticationRealm()
		{
		}

		blocxx::Array<blocxx::UInt16> httpPorts;
		blocxx::Array<blocxx::UInt16> httpsPorts;
		blocxx::String UDSFilename;
		blocxx::Int32 maxConnections;
		bool isSepThread;
		bool enableDeflate;
		EAuthenticationChallengeMethod defaultAuthChallenge;
		bool allowDigestAuthentication;
		bool allowBasicAuthentication;
		bool allowLocalAuthentication;
		bool allowSPNEGOAuthentication;
		bool allowAnonymous;
		bool useUDS;
		bool reuseAddr;
		ServiceEnvironmentIFCRef env;
		blocxx::Timeout timeout;
		blocxx::String defaultContentLanguage;
		blocxx::String authenticationRealm;
	};
private:
	EAuthenticateResult authenticate(HTTPSvrConnection* pconn,
		blocxx::String& userName, const blocxx::String& info, OperationContext& context,
					  const blocxx::Socket& socket);
	bool isAllowedUser(const blocxx::String& user) const;

	blocxx::Mutex m_guard;
	Options m_options;
#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
	HANDLE m_event;
#else
	blocxx::IntrusiveReference<blocxx::UnnamedPipe> m_upipe;
#endif
	blocxx::Array<URL> m_urls;
	typedef blocxx::Array<blocxx::IntrusiveReference<blocxx::ServerSocket> > ServerSockArray_t;
	ServerSockArray_t m_pHttpServerSockets;
	ServerSockArray_t m_pHttpsServerSockets;
	blocxx::IntrusiveReference<blocxx::ServerSocket> m_pUDSServerSocket;
#ifndef OW_DISABLE_DIGEST
	blocxx::IntrusiveReference<DigestAuthentication> m_digestAuthentication;
#endif
#ifndef OW_WIN32
	blocxx::IntrusiveReference<LocalAuthentication> m_localAuthentication;
#endif
	blocxx::IntrusiveReference<SPNEGOAuthentication> m_SPNEGOAuthentication;
	blocxx::Mutex m_authGuard;
	blocxx::IntrusiveReference<blocxx::ThreadPool> m_threadPool;
	blocxx::SortedVectorSet<blocxx::String> m_allowedUsers;
	bool m_allowAllUsers;
	blocxx::SSLServerCtxRef m_sslCtx;
	blocxx::Mutex m_shutdownGuard;
	bool m_shuttingDown;
#ifndef OW_NO_SSL
	blocxx::SSLOpts m_sslopts;
	blocxx::SSLTrustStoreRef m_trustStore;
#endif //  OW_HAVE_SSL

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	friend class HTTPSvrConnection;
	friend class HTTPListener;
	friend class IPCConnectionHandler;
	friend class HTTPServerSelectableCallback;
};
OW_EXPORT_TEMPLATE(OW_HTTPSVC_API, IntrusiveReference, HTTPServer);

} // end namespace OW_NAMESPACE

#endif
