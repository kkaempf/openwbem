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

#include "OW_config.h"
#include "OW_HTTPServer.hpp"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_ServerSocket.hpp"
#include "OW_IOException.hpp"
#include "OW_Socket.hpp"
#include "OW_Format.hpp"
#include "OW_UnnamedPipe.hpp"
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
#include "OW_LocalAuthentication.hpp"
#include "OW_SSLException.hpp"
#include "OW_SSLCtxMgr.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(HTTPServer)

//////////////////////////////////////////////////////////////////////////////
HTTPServer::HTTPServer()
	: m_upipe(UnnamedPipe::createUnnamedPipe())
	, m_allowAllUsers(false)
{
	m_upipe->setBlocking(UnnamedPipe::E_NONBLOCKING);
}
//////////////////////////////////////////////////////////////////////////////
HTTPServer::~HTTPServer()
{
}


//////////////////////////////////////////////////////////////////////////////
bool HTTPServer::isAllowedUser(const String& user) const
{
	return m_allowedUsers.count(user) != 0 || m_allowAllUsers;
}

//////////////////////////////////////////////////////////////////////////////
bool
HTTPServer::authenticate(HTTPSvrConnection* pconn,
	String& userName, const String& info, OperationContext& context)
{
	MutexLock lock(m_authGuard);
	
	// user supplied creds.  Find out what type of auth they're using.  We currently support Basic, Digest & OWLocal
	if (m_options.allowLocalAuthentication && info.startsWith("OWLocal"))
	{
		getEnvironment()->getLogger()->logDebug("HTTPServer::authenticate: processing OWLocal");
		bool rv = m_localAuthentication->authenticate(userName, info, pconn) && isAllowedUser(userName);
		if (rv)
		{
			getEnvironment()->getLogger()->logInfo(Format("HTTPServer::authenticate: authenticated %1", userName));
		}
		else
		{
			getEnvironment()->getLogger()->logInfo(Format("HTTPServer::authenticate: authentication failed for: %1", userName));
		}
		return rv;
	}
	else if (m_options.allowDigestAuthentication && info.startsWith("Digest"))
	{
#ifndef OW_DISABLE_DIGEST
		getEnvironment()->getLogger()->logDebug("HTTPServer::authenticate: processing Digest");
		bool rv = m_digestAuthentication->authenticate(userName, info, pconn) && isAllowedUser(userName);
		if (rv)
		{
			getEnvironment()->getLogger()->logInfo(Format("HTTPServer::authenticate: authenticated %1", userName));
		}
		else
		{
			getEnvironment()->getLogger()->logInfo(Format("HTTPServer::authenticate: authentication failed for: %1", userName));
		}
		return rv;
#endif
	}
	else if (m_options.allowBasicAuthentication && info.startsWith("Basic"))
	{
		getEnvironment()->getLogger()->logDebug("HTTPServer::authenticate: processing Basic");
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
			getEnvironment()->getLogger()->logDebug("HTTPServer::authenticate: Problem decoding credentials");
			return false;
		}
		String details;
		bool rv = m_options.env->authenticate(userName, password, details, context) && isAllowedUser(userName);
		if (!rv)
		{
			pconn->setErrorDetails(details);
			pconn->addHeader("WWW-Authenticate", authChallenge); 
			getEnvironment()->getLogger()->logInfo(Format("HTTPServer::authenticate: failed: %1", details));
		}
		else
		{
			getEnvironment()->getLogger()->logInfo(Format("HTTPServer::authenticate: authenticated %1", userName));
		}
		return rv;
	}

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
	getEnvironment()->getLogger()->logDebug(Format("HTTPServer::authenticate: Returning WWW-Authenticate: %1", authChallenge));
	pconn->addHeader("WWW-Authenticate", authChallenge); 
	return false;
	
}
//////////////////////////////////////////////////////////////////////////////
void
HTTPServer::setServiceEnvironment(ServiceEnvironmentIFCRef env)
{
	try
	{
		String item = env->getConfigItem(ConfigOpts::HTTP_PORT_opt, OW_DEFAULT_HTTP_PORT);
		m_options.httpPort = item.toInt32();
		
		item = env->getConfigItem(ConfigOpts::HTTPS_PORT_opt, OW_DEFAULT_HTTPS_PORT);
		m_options.httpsPort = item.toInt32();

		m_options.defaultContentLanguage = env->getConfigItem(
			ConfigOpts::HTTP_SERVER_DEFAULT_CONTENT_LANGUAGE_opt, OW_DEFAULT_HTTP_SERVER_CONTENT_LANGUAGE);
		
		m_options.UDSFilename = env->getConfigItem(ConfigOpts::UDS_FILENAME_opt, OW_DEFAULT_UDS_FILENAME);
		
		item = env->getConfigItem(ConfigOpts::USE_UDS_opt, OW_DEFAULT_USE_UDS);
		m_options.useUDS = item.equalsIgnoreCase("true");
		
		item = env->getConfigItem(ConfigOpts::MAX_CONNECTIONS_opt, OW_DEFAULT_MAX_CONNECTIONS);
		m_options.maxConnections = item.toInt32() + 1;
		// TODO: Make the type of pool and the size of the queue be separate config options.
		m_threadPool = ThreadPoolRef(new ThreadPool(ThreadPool::DYNAMIC_SIZE, m_options.maxConnections, m_options.maxConnections * 100, env->getLogger(), "HTTPServer"));
		
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
			m_options.defaultAuthChallenge = E_BASIC;
		}
		// TODO: right now basic and digest are mutually exclusive because of the config file setup.  
		// When possible deprecate the existing config items and make them independent.
		m_options.allowBasicAuthentication = !m_options.allowDigestAuthentication;
		
		item = env->getConfigItem(ConfigOpts::HTTP_ALLOW_LOCAL_AUTHENTICATION_opt, OW_DEFAULT_ALLOW_LOCAL_AUTHENTICATION);
		m_options.allowLocalAuthentication = !item.equalsIgnoreCase("false");
		if (m_options.allowLocalAuthentication)
		{
			m_localAuthentication = IntrusiveReference<LocalAuthentication>(
				new LocalAuthentication());
		}

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
			LoggerRef logger = m_HTTPServer->m_options.env->getLogger();
			logger->logInfo(
				 Format("Received connection on %1 from %2",
				 socket.getLocalAddress().toString(),
				 socket.getPeerAddress().toString()));
			HTTPServer::Options newOpts = m_HTTPServer->m_options;
			if (m_isIPC)
			{
				newOpts.enableDeflate = false;
			}
			RunnableRef rref(new HTTPSvrConnection(socket,
				 m_HTTPServer, m_HTTPServer->m_upipe, newOpts));
			if (!m_HTTPServer->m_threadPool->tryAddWork(rref))
			{
				// TODO: Send back a server too busy error.  We'll need a different thread pool for that, since our
				// main thread can't block.
				logger->logInfo("Server too busy, closing connection");
				socket.disconnect();
			}
		}
		catch (SSLException& se)
		{
			m_HTTPServer->m_options.env->getLogger()->logInfo(
				"SSL Handshake failed");
		}
		catch (SocketTimeoutException &e)
		{
			m_HTTPServer->m_options.env->getLogger()->logInfo(Format(
				"Socket TimeOut in HTTPServer: %1", e));
		}
		catch (SocketException &e)
		{
			m_HTTPServer->m_options.env->getLogger()->logInfo(Format(
				"Socket Exception in HTTPServer: %1", e));
		}
		catch (IOException &e)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(Format(
				"IO Exception in HTTPServer: %1", e));
		}
		catch (Exception& e)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(Format(
				"Exception in HTTPServer: %1", e));
			throw;
		}
		catch (ThreadCancelledException&)
		{
			throw;
		}
		catch (...)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(
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
HTTPServer::startService()
{
	Socket::createShutDownMechanism();
	ServiceEnvironmentIFCRef env = m_options.env;
	LoggerRef lgr = env->getLogger();
	lgr->logDebug("HTTP Service is starting...");
	if (m_options.httpPort < 0 && m_options.httpsPort < 0 && !m_options.useUDS)
	{
		OW_THROW(SocketException, "No ports to listen on and use_UDS set to false");
	}
	if (m_options.useUDS)
	{
		try
		{
			m_pUDSServerSocket = new ServerSocket;
			m_pUDSServerSocket->doListen(m_options.UDSFilename, 1000, m_options.reuseAddr);
			lgr->logInfo("HTTP server listening on Unix Domain Socket");
			String theURL = "ipc://localhost/cimom";
			addURL(URL(theURL));
			
			SelectableCallbackIFCRef cb(new HTTPServerSelectableCallback(
				false, this, true));
			env->addSelectable(m_pUDSServerSocket, cb);
		}
		catch (SocketException& e)
		{
			lgr->logError(Format("HTTP Server failed to listen on UDS: %1", e));
			throw;
		}
	}
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
					SocketFlags::E_NOT_SSL, 1000,
					curAddress,
					m_options.reuseAddr ? SocketFlags::E_REUSE_ADDR : SocketFlags::E_DONT_REUSE_ADDR);
				m_options.httpPort = m_pHttpServerSocket->getLocalAddress().getPort();
				lgr->logInfo(Format("HTTP server listening on: %1:%2",
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
				lgr->logError(Format("HTTP Server failed to listen on: %1:%2.  Msg: %3", curAddress, m_options.httpPort, e));
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
				lgr->logError(msg);
				OW_THROW(HTTPServerException, msg.c_str());
			}
#else
			try
			{
				String keyfile = env->getConfigItem(ConfigOpts::SSL_CERT_opt);
				SSLCtxMgr::initServer(keyfile);
			}
			catch (SSLException& e)
			{
				lgr->logError(Format("HTTP Service: Error initializing SSL: %1",
					e.getMessage()));
				throw;
			}

			UInt16 lport = static_cast<UInt16>(m_options.httpsPort);
			if (SSLCtxMgr::isServer())
			{
				try
				{
					m_pHttpsServerSocket = new ServerSocket;
					m_pHttpsServerSocket->doListen(lport,
						SocketFlags::E_SSL, 1000,
						curAddress,
						m_options.reuseAddr ? SocketFlags::E_REUSE_ADDR : SocketFlags::E_DONT_REUSE_ADDR);
					m_options.httpsPort =
					   m_pHttpsServerSocket->getLocalAddress().getPort();
					lgr->logInfo(Format("HTTPS server listening on: %1:%2",
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
					lgr->logError(Format("HTTP Server failed to listen on: %1:%2.  Msg: %3", curAddress, m_options.httpPort, e));
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
				lgr->logError(msg);
				OW_THROW(HTTPServerException, msg.c_str());

			}
		} // if (m_httpsPort > 0)
	}
	lgr->logDebug("HTTP Service has started");
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
void
HTTPServer::shutdown()
{
	m_options.env->getLogger()->logDebug("HTTP Service is shutting down...");
	// first stop all new connections
	m_options.env->removeSelectable(m_pHttpServerSocket);
	m_options.env->removeSelectable(m_pHttpsServerSocket);
	m_options.env->removeSelectable(m_pUDSServerSocket);

	// now stop all current connections
	Socket::shutdownAllSockets();
	if (m_upipe->writeString("shutdown") == -1)
	{
		OW_THROW(IOException, "Failed writing to HTTPServer shutdown pipe");
	}
	// not going to finish off what's in the queue, and we'll give the threads 60 seconds to exit before they're clobbered.
	m_threadPool->shutdown(ThreadPool::E_DISCARD_WORK_IN_QUEUE, 60);
	Socket::deleteShutDownMechanism();
	m_pHttpServerSocket = 0;
	m_pHttpsServerSocket = 0;
	m_pUDSServerSocket = 0;
	m_options.env->getLogger()->logDebug("HTTP Service has shut down");

	// clear out variables to avoid circular reference counts.
	m_options.env = 0;
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
// This allows the http server to be dynamically loaded
OW_SERVICE_FACTORY(OpenWBEM::HTTPServer,HTTPServer)

