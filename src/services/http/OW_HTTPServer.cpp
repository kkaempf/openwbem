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
*  - Neither the name of Vintela, Inc., Novell, Inc. nor the names of its
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
#include "OW_HTTPServer.hpp"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_ServerSocket.hpp"
#include "OW_IOException.hpp"
#include "OW_Socket.hpp"
#include "OW_Format.hpp"
#include "OW_SelectableIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_MD5.hpp"
#include "OW_HTTPUtils.hpp"
#ifndef OW_DISABLE_DIGEST
#include "OW_DigestAuthentication.hpp"
#endif
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_SocketBaseImpl.hpp" // for setDumpFiles()
#include "OW_Runnable.hpp"
#include "OW_ThreadCancelledException.hpp"
#include "OW_ThreadPool.hpp"
#include "OW_ExceptionIds.hpp"
#ifndef OW_WIN32
#include "OW_UnnamedPipe.hpp"
#include "OW_LocalAuthentication.hpp"
#endif
#include "OW_SSLException.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_AuthenticationException.hpp"
#include "OW_OperationContext.hpp"
#include "OW_ServiceIFCNames.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(HTTPServer)

namespace
{
	const String COMPONENT_NAME("ow.httpserver");
}

//////////////////////////////////////////////////////////////////////////////
HTTPServer::HTTPServer()
#ifdef OW_WIN32
	: m_event(NULL)
#else
	: m_upipe(UnnamedPipe::createUnnamedPipe())
#endif
	, m_allowAllUsers(false)
	, m_sslCtx(0)
	, m_shuttingDown(false)
#ifndef OW_NO_SSL
	, m_trustStore(0)
#endif
{
#ifdef OW_WIN32
	m_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
#else
	m_upipe->setBlocking(UnnamedPipe::E_NONBLOCKING);
#endif
}
//////////////////////////////////////////////////////////////////////////////
HTTPServer::~HTTPServer()
{
}


//////////////////////////////////////////////////////////////////////////////
String
HTTPServer::getName() const
{
	return ServiceIFCNames::HTTPServer;
}

//////////////////////////////////////////////////////////////////////////////
bool HTTPServer::isAllowedUser(const String& user) const
{
	return m_allowedUsers.count(user) != 0 || m_allowAllUsers;
}

//////////////////////////////////////////////////////////////////////////////
bool
HTTPServer::authenticate(HTTPSvrConnection* pconn,
	String& userName, const String& info, OperationContext& context,
	const Socket& socket)
{
	MutexLock lock(m_authGuard);

	
	// user supplied creds.  Find out what type of auth they're using.  We currently support Basic, Digest & OWLocal
#ifndef OW_WIN32
	if (m_options.allowLocalAuthentication && info.startsWith("OWLocal"))
	{
		OW_LOG_DEBUG(getEnvironment()->getLogger(COMPONENT_NAME), "HTTPServer::authenticate: processing OWLocal");
		bool rv = m_localAuthentication->authenticate(userName, info, pconn) && isAllowedUser(userName);
		if (rv)
		{
			OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: authenticated %1", userName));
		}
		else
		{
			OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: authentication failed for: %1", userName));
		}
		return rv;
	}
#endif

#ifndef OW_NO_SSL
	if (m_sslopts.verifyMode != SSLOpts::MODE_DISABLED)
	{
            if (socket.peerCertVerified())
            {

				SSL* ssl = socket.getSSL();
				OW_ASSERT(ssl);
				X509* cert = SSL_get_peer_certificate(ssl);
				OW_ASSERT(cert);
				String hash = SSLTrustStore::getCertMD5Fingerprint(cert);
				String uid;
				if (!m_trustStore->getUser(hash, userName, uid))
				{
					OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: authentication failed for: %1.  (Cert verified, but unknown user)", userName));
					return false;
				}
                OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: authenticated %1", userName));
				if (!uid.empty())
				{
					context.setStringData(OperationContext::CURUSER_UIDKEY, uid);
				}
                return true;
            }
	}
#endif
	bool rv = false;
	if (m_options.allowDigestAuthentication && info.startsWith("Digest"))
	{
#ifndef OW_DISABLE_DIGEST
		OW_LOG_DEBUG(getEnvironment()->getLogger(COMPONENT_NAME), "HTTPServer::authenticate: processing Digest");
		rv = m_digestAuthentication->authenticate(userName, info, pconn) && isAllowedUser(userName);
		if (rv)
		{
			OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: authenticated %1", userName));
		}
		else
		{
			OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: authentication failed for: %1", userName));
		}
#endif
	}
	else if (m_options.allowBasicAuthentication && info.startsWith("Basic"))
	{
		OW_LOG_DEBUG(getEnvironment()->getLogger(COMPONENT_NAME), "HTTPServer::authenticate: processing Basic");
		String authChallenge = "Basic realm=\"" + pconn->getHostName() + "\"";
		String password;
		// info is a username:password string that is base64 encoded. decode it.
		try
		{
			HTTPUtils::decodeBasicCreds(info, userName, password);
		}
		catch (const AuthenticationException& e)
		{
			// decoding failed
			pconn->setErrorDetails("Problem decoding credentials");
			pconn->addHeader("WWW-Authenticate", authChallenge);
			OW_LOG_DEBUG(getEnvironment()->getLogger(COMPONENT_NAME), "HTTPServer::authenticate: Problem decoding credentials");
			return false;
		}
		String details;
		rv = m_options.env->authenticate(userName, password, details, context) && isAllowedUser(userName);
		if (!rv)
		{
			pconn->setErrorDetails(details);
			pconn->addHeader("WWW-Authenticate", authChallenge);
			OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: failed: %1", details));
		}
		else
		{
			OW_LOG_INFO(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: authenticated %1", userName));
		}
	}
	else
	{
		// We don't handle whatever they sent, so send the default challenge
		String hostname = pconn->getHostName();
		pconn->setErrorDetails("You must authenticate to access this"
			" resource");
		String authChallenge;
		switch (m_options.defaultAuthChallenge)
		{
#ifndef OW_DISABLE_DIGEST
			case E_DIGEST:
				authChallenge = m_digestAuthentication->getChallenge(hostname);
				break;
#endif
			case E_BASIC:
				authChallenge = "Basic realm=\"" + pconn->getHostName() + "\"";
				break;

			default:
				OW_ASSERT("Internal implementation error! m_options.defaultAuthChallenge is invalid!" == 0);
		}
		OW_LOG_DEBUG(getEnvironment()->getLogger(COMPONENT_NAME), Format("HTTPServer::authenticate: Returning WWW-Authenticate: %1", authChallenge));
		pconn->addHeader("WWW-Authenticate", authChallenge);
		return false;
	}

#ifndef OW_NO_SSL
	if (rv && m_sslopts.verifyMode == SSLOpts::MODE_AUTOUPDATE)
	{
		SSL* ssl = socket.getSSL();
                if (ssl)
                {
                    X509* cert = SSL_get_peer_certificate(ssl);
                    if (cert)
                    {
                            try
                            {
								String uid = context.getStringDataWithDefault(OperationContext::CURUSER_UIDKEY);
                                m_trustStore->addCertificate(cert, userName, uid);
                            }
                            catch (SSLException& e)
                            {
                                OW_LOG_ERROR(getEnvironment()->getLogger(COMPONENT_NAME), e.getMessage());
                            }
                    }
                }
	}
#endif
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPServer::init(const ServiceEnvironmentIFCRef& env)
{
	try
	{
		String item = env->getConfigItem(ConfigOpts::HTTP_PORT_opt, OW_DEFAULT_HTTP_PORT);
		m_options.httpPort = item.toInt32();
		
		item = env->getConfigItem(ConfigOpts::HTTPS_PORT_opt, OW_DEFAULT_HTTPS_PORT);
		m_options.httpsPort = item.toInt32();

		m_options.defaultContentLanguage = env->getConfigItem(
			ConfigOpts::HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE_opt, OW_DEFAULT_HTTP_SERVER_CONTENT_LANGUAGE);
	
#ifndef OW_WIN32
		m_options.UDSFilename = env->getConfigItem(ConfigOpts::UDS_FILENAME_opt, OW_DEFAULT_UDS_FILENAME);
#endif
		
		item = env->getConfigItem(ConfigOpts::USE_UDS_opt, OW_DEFAULT_USE_UDS);
		m_options.useUDS = item.equalsIgnoreCase("true");
		
		item = env->getConfigItem(ConfigOpts::MAX_CONNECTIONS_opt, OW_DEFAULT_MAX_CONNECTIONS);
		m_options.maxConnections = item.toInt32() + 1;
		// TODO: Make the type of pool and the size of the queue be separate config options.
		m_threadPool = ThreadPoolRef(new ThreadPool(ThreadPool::DYNAMIC_SIZE, m_options.maxConnections, m_options.maxConnections * 100, env->getLogger(COMPONENT_NAME), "HTTPServer"));
		
		item = env->getConfigItem(ConfigOpts::SINGLE_THREAD_opt, OW_DEFAULT_SINGLE_THREAD);
		m_options.isSepThread = !item.equalsIgnoreCase("true");
		
		item = env->getConfigItem(ConfigOpts::ENABLE_DEFLATE_opt, OW_DEFAULT_ENABLE_DEFLATE);
		m_options.enableDeflate = !item.equalsIgnoreCase("false");
		
		item = env->getConfigItem(ConfigOpts::ALLOW_ANONYMOUS_opt, OW_DEFAULT_ALLOW_ANONYMOUS);
		m_options.allowAnonymous = item.equalsIgnoreCase("true");
		m_options.env = env;
		
		item = env->getConfigItem(ConfigOpts::HTTP_USE_DIGEST_opt, OW_DEFAULT_USE_DIGEST);
		m_options.allowDigestAuthentication = !item.equalsIgnoreCase("false");
		if (m_options.allowDigestAuthentication)
		{
#ifndef OW_DISABLE_DIGEST
			String passwdFile = env->getConfigItem(
				ConfigOpts::DIGEST_AUTH_FILE_opt, OW_DEFAULT_DIGEST_PASSWD_FILE);
			m_digestAuthentication = IntrusiveReference<DigestAuthentication>(
				new DigestAuthentication(passwdFile));
			m_options.defaultAuthChallenge = E_DIGEST;
#else
			OW_THROW(HTTPServerException, "Unable to initialize HTTP Server because"
				" digest is enabled in the config file, but the digest code has been disabled");
#endif
		}
		else
		{
			// TODO: deprecate ConfigOpts::HTTP_USE_DIGEST_opt and create a new option for the default auth challenge
			// If OWLocal is desired for the default, set defaultAuthChallenge to E_OWLOCAL
			m_options.defaultAuthChallenge = E_BASIC;
		}
		// TODO: right now basic and digest are mutually exclusive because of the config file setup.
		// When possible deprecate the existing config items and make them independent.
		m_options.allowBasicAuthentication = !m_options.allowDigestAuthentication;
	
#ifndef OW_WIN32
		item = env->getConfigItem(ConfigOpts::HTTP_ALLOW_LOCAL_AUTHENTICATION_opt, OW_DEFAULT_ALLOW_LOCAL_AUTHENTICATION);
		m_options.allowLocalAuthentication = !item.equalsIgnoreCase("false");
		if (m_options.allowLocalAuthentication)
		{
			m_localAuthentication = IntrusiveReference<LocalAuthentication>(
				new LocalAuthentication());
		}
#endif

		String dumpPrefix = env->getConfigItem(ConfigOpts::DUMP_SOCKET_IO_opt);
		if (!dumpPrefix.empty())
		{
			SocketBaseImpl::setDumpFiles(
				dumpPrefix + "/owHTTPSockDumpIn",
				dumpPrefix + "/owHTTPSockDumpOut");
		}
		
		item = env->getConfigItem(ConfigOpts::REUSE_ADDR_opt);
		m_options.reuseAddr = !item.equalsIgnoreCase("false");
		
		item = env->getConfigItem(ConfigOpts::HTTP_TIMEOUT_opt, OW_DEFAULT_HTTP_TIMEOUT);
		m_options.timeout = item.toInt32();

		item = env->getConfigItem(ConfigOpts::ALLOWED_USERS_opt, OW_DEFAULT_ALLOWED_USERS);
		if (item == "*")
		{
			m_allowAllUsers = true;
		}
		else
		{
			m_allowAllUsers = false;
			StringArray users = item.tokenize();
			m_allowedUsers.insert(users.begin(), users.end());
		}
	}
	catch (const StringConversionException& e)
	{
		OW_THROW(HTTPServerException, Format("Unable to initialize HTTP Server because"
			" of invalid config item. %1", e.getMessage()).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
class HTTPServerSelectableCallback : public SelectableCallbackIFC
{
public:
	HTTPServerSelectableCallback(bool isHTTPS,
		HTTPServer* httpServer, bool isIPC)
		: SelectableCallbackIFC()
		, m_isHTTPS(isHTTPS)
		, m_HTTPServer(httpServer)
		, m_isIPC(isIPC)
	{
	}
	virtual ~HTTPServerSelectableCallback() {}
	virtual void doSelected(SelectableIFCRef& selectedObject)
	{
		try
		{
			IntrusiveReference<ServerSocket> pServerSocket;
			if (m_isIPC)
			{
				pServerSocket = m_HTTPServer->m_pUDSServerSocket;
			}
			else if (m_isHTTPS)
			{
				pServerSocket = m_HTTPServer->m_pHttpsServerSocket;
			}
			else
			{
				pServerSocket = m_HTTPServer->m_pHttpServerSocket;
			}
			Socket socket = pServerSocket->accept(2);
			LoggerRef logger = m_HTTPServer->m_options.env->getLogger(COMPONENT_NAME);
			OW_LOG_INFO(logger,
				 Format("Received connection on %1 from %2",
				 socket.getLocalAddress().toString(),
				 socket.getPeerAddress().toString()));
			HTTPServer::Options newOpts = m_HTTPServer->m_options;
			if (m_isIPC)
			{
				newOpts.enableDeflate = false;
			}
#ifdef OW_WIN32
			RunnableRef rref(new HTTPSvrConnection(socket,
				 m_HTTPServer, m_HTTPServer->m_event, newOpts));
#else
			RunnableRef rref(new HTTPSvrConnection(socket,
				 m_HTTPServer, m_HTTPServer->m_upipe, newOpts));
#endif
			if (!m_HTTPServer->m_threadPool->tryAddWork(rref))
			{
				// TODO: Send back a server too busy error.  We'll need a different thread pool for that, since our
				// main thread can't block.
				OW_LOG_INFO(logger, "Server too busy, closing connection");
				socket.disconnect();
			}
		}
		catch (SSLException& se)
		{
			OW_LOG_INFO(m_HTTPServer->m_options.env->getLogger(COMPONENT_NAME),
				Format("SSL Handshake failed: %1", se.getMessage()).c_str());
		}
		catch (SocketTimeoutException &e)
		{
			OW_LOG_INFO(m_HTTPServer->m_options.env->getLogger(COMPONENT_NAME), Format(
				"Socket TimeOut in HTTPServer: %1", e));
		}
		catch (SocketException &e)
		{
			OW_LOG_INFO(m_HTTPServer->m_options.env->getLogger(COMPONENT_NAME), Format(
				"Socket Exception in HTTPServer: %1", e));
		}
		catch (IOException &e)
		{
			OW_LOG_ERROR(m_HTTPServer->m_options.env->getLogger(COMPONENT_NAME), Format(
				"IO Exception in HTTPServer: %1", e));
		}
		catch (Exception& e)
		{
			OW_LOG_ERROR(m_HTTPServer->m_options.env->getLogger(COMPONENT_NAME), Format(
				"Exception in HTTPServer: %1", e));
			throw;
		}
		catch (ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			OW_LOG_ERROR(m_HTTPServer->m_options.env->getLogger(COMPONENT_NAME),
				"Unknown exception in HTTPServer.");
			throw;
		}
	}
private:
	bool m_isHTTPS;
	HTTPServer* m_HTTPServer;
	bool m_isIPC;
};
//////////////////////////////////////////////////////////////////////////////
void
HTTPServer::start()
{
	ServiceEnvironmentIFCRef env = m_options.env;
	LoggerRef lgr = env->getLogger(COMPONENT_NAME);
	OW_LOG_DEBUG(lgr, "HTTP Service is starting...");
	if (m_options.httpPort < 0 && m_options.httpsPort < 0 && !m_options.useUDS)
	{
		OW_THROW(SocketException, "No ports to listen on and use_UDS set to false");
	}
#ifndef OW_WIN32
	if (m_options.useUDS)
	{
		try
		{
			m_pUDSServerSocket = new ServerSocket;
			m_pUDSServerSocket->doListen(m_options.UDSFilename, 1000, m_options.reuseAddr);
			OW_LOG_INFO(lgr, "HTTP server listening on Unix Domain Socket");
			String theURL = "ipc://localhost/cimom";
			addURL(URL(theURL));
			
			SelectableCallbackIFCRef cb(new HTTPServerSelectableCallback(
				false, this, true));
			env->addSelectable(m_pUDSServerSocket, cb);
		}
		catch (SocketException& e)
		{
			OW_LOG_ERROR(lgr, Format("HTTP Server failed to listen on UDS: %1", e));
			throw;
		}
	}
#endif
	String listenAddressesOpt = env->getConfigItem(ConfigOpts::LISTEN_ADDRESSES_opt, OW_DEFAULT_LISTEN_ADDRESSES);
	StringArray listenAddresses = listenAddressesOpt.tokenize(" ");
	if (listenAddresses.empty())
	{
		OW_THROW(HTTPServerException, "http_server.listen_addresses config item malformed");
	}
	for (size_t i = 0; i < listenAddresses.size(); ++i)
	{
		const String& curAddress = listenAddresses[i];
		if (m_options.httpPort >= 0)
		{
			try
			{
				UInt16 lport = static_cast<UInt16>(m_options.httpPort);
				m_pHttpServerSocket = new ServerSocket;
				m_pHttpServerSocket->doListen(lport,
					1000, curAddress,
					m_options.reuseAddr ? SocketFlags::E_REUSE_ADDR : SocketFlags::E_DONT_REUSE_ADDR);
				m_options.httpPort = m_pHttpServerSocket->getLocalAddress().getPort();
				OW_LOG_INFO(lgr, Format("HTTP server listening on: %1:%2",
				   curAddress, m_options.httpPort));
				String theURL = "http://" + SocketAddress::getAnyLocalHost().getName()
					+ ":" + String(m_options.httpPort) + "/cimom";
				addURL(URL(theURL));

				SelectableCallbackIFCRef cb(new HTTPServerSelectableCallback(
					false, this, false));
				env->addSelectable(m_pHttpServerSocket, cb);
			}
			catch (SocketException& e)
			{
				OW_LOG_ERROR(lgr, Format("HTTP Server failed to listen on: %1:%2.  Msg: %3", curAddress, m_options.httpPort, e));
				throw;
			}
		}
		if (m_options.httpsPort >= 0)
		{
#ifdef OW_NO_SSL
			if (m_options.httpPort < 0 && !m_options.useUDS)
			{
				OW_THROW(HTTPServerException, "No ports to listen on.  "
						"SSL unavailable (OpenWBEM not built with SSL support) "
						"and no http port defined.");
			}
			else
			{
				String msg = Format("Unable to listen on %1:%2.  "
						"OpenWBEM not built with SSL support.", curAddress, m_options.httpsPort);
				OW_LOG_ERROR(lgr, msg);
				OW_THROW(HTTPServerException, msg.c_str());
			}
#else
			try
			{
				m_sslopts.keyfile = env->getConfigItem(ConfigOpts::SSL_CERT_opt);
				m_sslopts.trustStore = env->getConfigItem(ConfigOpts::HTTP_SERVER_SSL_TRUST_STORE,
													   OW_DEFAULT_HTTP_SERVER_SSL_TRUST_STORE);
				String verifyMode = env->getConfigItem(ConfigOpts::HTTP_SERVER_SSL_CLIENT_VERIFICATION_opt,
													   OW_DEFAULT_HTTP_SERVER_SSL_CLIENT_VERIFICATION);
				verifyMode.toLowerCase();
				if (verifyMode == "disabled")
				{
					m_sslopts.verifyMode = SSLOpts::MODE_DISABLED;
				}
				else if (verifyMode == "optional")
				{
					m_sslopts.verifyMode = SSLOpts::MODE_OPTIONAL;
				}
				else if (verifyMode == "required")
				{
					m_sslopts.verifyMode = SSLOpts::MODE_REQUIRED;
				}
				else if (verifyMode == "autoupdate")
				{
					m_sslopts.verifyMode = SSLOpts::MODE_AUTOUPDATE;
				}
				else
				{
					OW_THROW(HTTPServerException, Format("Bad value (%1) for configuration item %2",
														 verifyMode,
														 ConfigOpts::HTTP_SERVER_SSL_CLIENT_VERIFICATION_opt).c_str());
				}
				//SSLCtxMgr::initServer(keyfile);
				m_sslCtx = SSLServerCtxRef(new SSLServerCtx(m_sslopts));
				if (m_sslopts.verifyMode != SSLOpts::MODE_DISABLED)
				{
					m_trustStore = SSLTrustStoreRef(new SSLTrustStore(m_sslopts.trustStore));
				}
			}
			catch (SSLException& e)
			{
				OW_LOG_ERROR(lgr, Format("HTTP Service: Error initializing SSL: %1",
					e.getMessage()));
				throw;
			}

			UInt16 lport = static_cast<UInt16>(m_options.httpsPort);
			//if (SSLCtxMgr::isServer())
			if (m_sslCtx)
			{
				try
				{
					m_pHttpsServerSocket = new ServerSocket(m_sslCtx.getPtr());
					m_pHttpsServerSocket->doListen(lport,
						1000, curAddress,
						m_options.reuseAddr ? SocketFlags::E_REUSE_ADDR : SocketFlags::E_DONT_REUSE_ADDR);
					m_options.httpsPort =
					   m_pHttpsServerSocket->getLocalAddress().getPort();
					OW_LOG_INFO(lgr, Format("HTTPS server listening on: %1:%2",
					   curAddress, m_options.httpsPort));
					String theURL = "https://" +
						SocketAddress::getAnyLocalHost().getName() + ":" +
						String(m_options.httpsPort) + "/cimom";
					addURL(URL(theURL));
					SelectableCallbackIFCRef cb(new HTTPServerSelectableCallback(
						true, this, false));
					env->addSelectable(m_pHttpsServerSocket, cb);
				}
				catch (SocketException& e)
				{
					OW_LOG_ERROR(lgr, Format("HTTP Server failed to listen on: %1:%2.  Msg: %3", curAddress, m_options.httpPort, e));
					throw;
				}
			}
			else
#endif // #ifndef OW_NO_SSL
			{
				if (m_options.httpPort < 0 && !m_options.useUDS)
				{
					OW_THROW(HTTPServerException, "No ports to listen on.  "
						"SSL unavailable (SSL not initialized in server mode) "
						"and no http port defined.");
				}
				String msg = Format("Unable to listen on: %1:%2.  "
					"SSL not initialized in server mode.", curAddress, m_options.httpsPort);
				OW_LOG_ERROR(lgr, msg);
				OW_THROW(HTTPServerException, msg.c_str());

			}
		} // if (m_httpsPort > 0)
	}
	OW_LOG_DEBUG(lgr, "HTTP Service has started");
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPServer::addURL(const URL& url)
{
	m_urls.push_back(url);
}
//////////////////////////////////////////////////////////////////////////////
Array<URL>
HTTPServer::getURLs() const
{
	return m_urls;
}
//////////////////////////////////////////////////////////////////////////////
SocketAddress
HTTPServer::getLocalHTTPAddress()
{
	if (m_pHttpServerSocket)
	{
		return m_pHttpServerSocket->getLocalAddress();
	}
	else
	{
		return SocketAddress::allocEmptyAddress(SocketAddress::INET);
	}
}
//////////////////////////////////////////////////////////////////////////////
SocketAddress
HTTPServer::getLocalHTTPSAddress()
{
	if (m_pHttpsServerSocket)
	{
		return m_pHttpsServerSocket->getLocalAddress();
	}
	else
	{
		return SocketAddress::allocEmptyAddress(SocketAddress::INET);
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
HTTPServer::isShuttingDown()
{
	MutexLock lock(m_shutdownGuard);
	return m_shuttingDown;
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPServer::shutdown()
{
	{
		MutexLock lock(m_shutdownGuard);
		m_shuttingDown = true;
	}
	OW_LOG_DEBUG(m_options.env->getLogger(COMPONENT_NAME), "HTTP Service is shutting down...");
	// first stop all new connections
	m_options.env->removeSelectable(m_pHttpServerSocket);
	m_options.env->removeSelectable(m_pHttpsServerSocket);
	m_options.env->removeSelectable(m_pUDSServerSocket);

	// now stop all current connections
#ifdef OW_WIN32
	if (!::SetEvent(m_event))
	{
		OW_THROW(IOException,
			"Failed signaling event for HTTP server shutdown");
	}
#else
	if (m_upipe->writeString("shutdown") == -1)
	{
		OW_THROW(IOException, "Failed writing to HTTPServer shutdown pipe");
	}
#endif
	// not going to finish off what's in the queue, and we'll give the threads 60 seconds to exit before they're clobbered.
	m_threadPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 60);
	m_pHttpServerSocket = 0;
	m_pHttpsServerSocket = 0;
	m_pUDSServerSocket = 0;
	OW_LOG_DEBUG(m_options.env->getLogger(COMPONENT_NAME), "HTTP Service has shut down");

	// clear out variables to avoid circular reference counts.
	m_options.env = 0;
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
// This allows the http server to be dynamically loaded
OW_SERVICE_FACTORY(OpenWBEM::HTTPServer,HTTPServer)

