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
#include "OW_HTTPServer.hpp"
#include "OW_HTTPSvrConnection.hpp"
#include "OW_ServerSocket.hpp"
#include "OW_IOException.hpp"
#include "OW_Socket.hpp"
#include "OW_Format.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_SelectableIFC.hpp"
#include "OW_DateTime.hpp"
#include "OW_Assertion.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_MD5.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_DigestAuthentication.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_SocketBaseImpl.hpp" // for setDumpFiles()

//////////////////////////////////////////////////////////////////////////////
OW_HTTPServer::OW_HTTPServer()
	: m_options()
	, m_threadCountSemaphore(0)
	, m_upipe(OW_UnnamedPipe::createUnnamedPipe())
	, m_urls()
	, m_pHttpServerSocket(0)
	, m_pHttpsServerSocket(0)
	, m_digestAuth(0)
	, m_authGuard()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_HTTPServer::~OW_HTTPServer()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_HTTPServer::authenticate(OW_HTTPSvrConnection* pconn,
	OW_String& userName, const OW_String& info)
{
	OW_MutexLock lock(m_authGuard);
	
	if (info.empty())
	{
		OW_String hostname = pconn->getHostName();
		pconn->setErrorDetails("You must authenticate to access this"
			" resource");

		pconn->addHeader("WWW-Authenticate",
			m_options.useDigest ? m_digestAuth->getChallenge(hostname)
			: "Basic");

		return false;
	}
	
	if (m_options.useDigest)
	{
		return m_digestAuth->authorize(userName, info, pconn);
	}
	else
	{
		OW_String password;
		// info is a username:password string that is base64 encoded. decode it.
		try
		{
			OW_HTTPUtils::decodeBasicCreds(info, userName, password);
		}
		catch (const OW_AuthenticationException& e)
		{
			// decoding failed
			pconn->setErrorDetails("Problem decoding credentials");
			pconn->addHeader("WWW-Authenticate", "Basic");
			return false;
		}

		OW_String details;
		if (!m_options.env->authenticate(userName, password, details))
		{
			pconn->setErrorDetails(details);
			pconn->addHeader("WWW-Authenticate", "Basic");
			return false;
		}
		else
		{
			return true;
		}
	}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::setServiceEnvironment(OW_ServiceEnvironmentIFCRef env)
{
	try
	{
		OW_String item = env->getConfigItem(OW_ConfigOpts::HTTP_PORT_opt);
		if (item.empty())
		{
			item = DEFAULT_HTTP_PORT;
		}
		m_options.httpPort = item.toInt32();

		item = env->getConfigItem(OW_ConfigOpts::HTTPS_PORT_opt);
		if (item.empty())
		{
			item = DEFAULT_HTTPS_PORT;
		}
		m_options.httpsPort = item.toInt32();

		item = env->getConfigItem(OW_ConfigOpts::USE_UDS_opt);
		if (item.empty())
		{
			item = DEFAULT_USE_UDS;
		}
		m_options.useUDS = item.equalsIgnoreCase("true");

		item = env->getConfigItem(OW_ConfigOpts::MAX_CONNECTIONS_opt);
		if (item.empty())
		{
			item = DEFAULT_MAX_CONNECTIONS;
		}
		m_options.maxConnections = item.toInt32() + 1;

		item = env->getConfigItem(OW_ConfigOpts::SINGLE_THREAD_opt);
		if (item.empty())
		{
			item = "false";
		}
		m_options.isSepThread = !item.equalsIgnoreCase("true");

		item = env->getConfigItem(OW_ConfigOpts::ENABLE_DEFLATE_opt);
		if (item.empty())
		{
			item = "true";
		}
		m_options.enableDeflate = !item.equalsIgnoreCase("false");

		item = env->getConfigItem(OW_ConfigOpts::HTTP_USE_DIGEST_opt);
		if (item.empty())
		{
			item = "true";
		}
		m_options.useDigest = !item.equalsIgnoreCase("false");

		item = env->getConfigItem(OW_ConfigOpts::ALLOW_ANONYMOUS_opt);
		m_options.allowAnonymous = item.equalsIgnoreCase("true");

		m_options.env = env;

		m_threadCountSemaphore = new OW_Semaphore(m_options.maxConnections);
		if (m_options.useDigest)
		{
			OW_String passwdFile = env->getConfigItem(
				OW_ConfigOpts::DIGEST_AUTH_FILE_opt);

			m_digestAuth = OW_Reference<OW_DigestAuthentication>(
				new OW_DigestAuthentication(passwdFile));
		}

		OW_String dumpPrefix = env->getConfigItem(OW_ConfigOpts::DUMP_SOCKET_IO_opt);
		if (!dumpPrefix.empty())
		{
			OW_SocketBaseImpl::setDumpFiles(
				dumpPrefix + "/owHTTPSockDumpIn",
				dumpPrefix + "/owHTTPSockDumpOut");
		}
	}
	catch (const OW_StringConversionException& e)
	{
		OW_THROW(OW_Exception, format("Unable to initialize HTTP Server because"
			" of invalid config item. %1", e.getMessage()).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class threadCountDecrementer : public OW_ThreadDoneCallback
	{
	public:
		threadCountDecrementer(OW_HTTPServer* httpServer)
		: m_HTTPServer(httpServer)
		{}
	protected:
		virtual void doNotifyThreadDone(OW_Thread *)
		{
			m_HTTPServer->decThreadCount();
		}
	private:
		OW_HTTPServer* m_HTTPServer;
	};
}

//////////////////////////////////////////////////////////////////////////////
class OW_HTTPServerSelectableCallback : public OW_SelectableCallbackIFC
{
public:
	OW_HTTPServerSelectableCallback(bool isHTTPS,
		OW_HTTPServer* httpServer, bool isIPC)
		: OW_SelectableCallbackIFC()
		, m_isHTTPS(isHTTPS)
		, m_HTTPServer(httpServer)
		, m_isIPC(isIPC)
	{
	}

	virtual ~OW_HTTPServerSelectableCallback() {}

	virtual void doSelected(OW_SelectableIFCRef& selectedObject)
	{
		try
		{
			(void)selectedObject;

			OW_Reference<OW_ServerSocket> pServerSocket;
			if (m_isIPC)
			{
				pServerSocket = m_HTTPServer->m_pUDSServerSocket;
			}
			else if(m_isHTTPS)
			{
				pServerSocket = m_HTTPServer->m_pHttpsServerSocket;
			}
			else
			{
				pServerSocket = m_HTTPServer->m_pHttpServerSocket;
			}

			OW_Socket socket = pServerSocket->accept(2);

			m_HTTPServer->m_options.env->getLogger()->logCustInfo(
				 format("Received connection on %1 from %2",
				 socket.getLocalAddress().toString(),
				 socket.getPeerAddress().toString()));

			m_HTTPServer->incThreadCount();

			OW_HTTPServer::Options newOpts = m_HTTPServer->m_options;
			if (m_isIPC)
			{
				newOpts.enableDeflate = false;
			}
			OW_RunnableRef rref(new OW_HTTPSvrConnection(socket,
				 m_HTTPServer, m_HTTPServer->m_upipe, newOpts));

			OW_Thread::run(rref, m_HTTPServer->m_options. isSepThread,
				OW_ThreadDoneCallbackRef(new threadCountDecrementer(m_HTTPServer)));
		}
		catch (OW_SSLException& se)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(
				"SSL Handshake failed");
		}
		catch (OW_TimeOutException &e)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(format(
				"Socket TimeOut in HTTPServer: %1", e));
		}
		catch (OW_SocketException &e)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(format(
				"Socket Exception in HTTPServer: %1", e));
		}
		catch (OW_IOException &e)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(format(
				"IO Exception in HTTPServer: %1", e));
		}
		catch (OW_Exception& e)
		{
			m_HTTPServer->m_options.env->getLogger()->logError(format(
				"OW_Exception in HTTPServer: %1", e));
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
	OW_HTTPServer* m_HTTPServer;
	bool m_isIPC;
};

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::startService()
{
	OW_Socket::createShutDownMechanism();
	OW_ServiceEnvironmentIFCRef env = m_options.env;
	OW_LoggerRef lgr = env->getLogger();
	lgr->logDebug("HTTP Service is starting...");

	bool gothttp = false, gothttps = false, gotuds = false;
	if (m_options.httpPort < 0 && m_options.httpsPort < 0 && !m_options.useUDS)
	{
		OW_THROW(OW_SocketException, "No ports to listen on and use_UDS set to false");
	}

	if (m_options.useUDS)
	{
		try
		{
			m_pUDSServerSocket = new OW_ServerSocket;
			m_pUDSServerSocket->doListen(OW_DOMAIN_SOCKET_NAME, 1000);

			lgr->logCustInfo("HTTP server listening on Unix Domain Socket");

			OW_String URL = "ipc://localhost/cimom";
			addURL(OW_URL(URL));
			
			OW_SelectableCallbackIFCRef cb(new OW_HTTPServerSelectableCallback(
				false, this, true));

			env->addSelectable(m_pUDSServerSocket, cb);
			gotuds = true;
		}
		catch (OW_SocketException& e)
		{
			lgr->logError(format("HTTP Server failed to listen on UDS: %1", e));
		}
	}
	if (m_options.httpPort >= 0)
	{
		try
		{
			OW_UInt16 lport = static_cast<OW_UInt16>(m_options.httpPort);
			m_pHttpServerSocket = new OW_ServerSocket;
			m_pHttpServerSocket->doListen(lport, false, 1000, true);
			m_options.httpPort = m_pHttpServerSocket->getLocalAddress().getPort();

			lgr->logCustInfo(format("HTTP server listening on port: %1",
			   m_options.httpPort));

			OW_String URL = "http://" + OW_SocketAddress::getAnyLocalHost().getName()
				+ ":" + OW_String(m_options.httpPort) + "/cimom";
			addURL(OW_URL(URL));
			
			OW_SelectableCallbackIFCRef cb(new OW_HTTPServerSelectableCallback(
				false, this, false));

			env->addSelectable(m_pHttpServerSocket, cb);
			gothttp = true;
		}
		catch (OW_SocketException& e)
		{
			lgr->logError(format("HTTP Server failed to listen on TCP port: %1.  Msg: %2", m_options.httpPort, e));
		}
	}

	if (m_options.httpsPort >= 0)
	{
#ifndef OW_NO_SSL
		try
		{
			OW_String keyfile = env->getConfigItem(OW_ConfigOpts::SSL_CERT_opt);
			OW_SSLCtxMgr::initServer(keyfile);
		}
		catch (OW_SSLException& e)
		{
			lgr->logError(format("HTTP Service: Error initializing SSL: %1",
				e.getMessage()));
		}
		
		OW_UInt16 lport = static_cast<OW_UInt16>(m_options.httpsPort);
		if (OW_SSLCtxMgr::isServer())
		{
			try
			{
				m_pHttpsServerSocket = new OW_ServerSocket;
				m_pHttpsServerSocket->doListen(lport, true, 1000, true);

				m_options.httpsPort =
				   m_pHttpsServerSocket->getLocalAddress().getPort();

				lgr->logCustInfo(format("HTTPS server listening on port: %1",
				   m_options.httpsPort));

				OW_String URL = "https://" +
					OW_SocketAddress::getAnyLocalHost().getName() + ":" +
					OW_String(m_options.httpsPort) + "/cimom";

				addURL(OW_URL(URL));

				OW_SelectableCallbackIFCRef cb(new OW_HTTPServerSelectableCallback(
					true, this, false));

				env->addSelectable(m_pHttpsServerSocket, cb);
				gothttps = true;
			}
			catch (OW_SocketException& e)
			{
				lgr->logError(format("HTTP Server failed to listen on TCP port: %1.  Msg: %2", m_options.httpPort, e));
			}
		}
		else
#endif // #ifndef OW_NO_SSL
		{
			if (m_options.httpPort < 0 && !m_options.useUDS)
			{
				OW_THROW(OW_SocketException, "No ports to listen on.  "
					"SSL unavailable (SSL not initialized in server mode) "
					"and no http port defined.");
			}

			lgr->logError(format("Unable to listen on port %1.  "
				"SSL not initialized in server mode.", m_options.httpsPort));
		}
	} // if (m_httpsPort > 0)

	if (!gotuds && !gothttp && !gothttps)
	{
		lgr->logError("HTTP Server failed to start any services");
		OW_THROW(OW_SocketException, "HTTP Server failed to start any services");
	}


	lgr->logDebug("HTTP Service has started");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::decThreadCount()
{
	m_threadCountSemaphore->signal();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::incThreadCount()
{
	m_threadCountSemaphore->wait();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::addURL(const OW_URL& url)
{
	m_urls.push_back(url);
}

//////////////////////////////////////////////////////////////////////////////
OW_Array<OW_URL>
OW_HTTPServer::getURLs() const
{
	return m_urls;
}


//////////////////////////////////////////////////////////////////////////////
OW_SocketAddress
OW_HTTPServer::getLocalHTTPAddress()
{
	if (m_pHttpServerSocket)
	{
		return m_pHttpServerSocket->getLocalAddress();
	}
	else
	{
		return OW_SocketAddress::allocEmptyAddress(OW_SocketAddress::INET);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_SocketAddress
OW_HTTPServer::getLocalHTTPSAddress()
{
	if (m_pHttpsServerSocket)
	{
		return m_pHttpsServerSocket->getLocalAddress();
	}
	else
	{
		return OW_SocketAddress::allocEmptyAddress(OW_SocketAddress::INET);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::shutdown()
{
	m_options.env->getLogger()->logDebug("HTTP Service is shutting down...");


	OW_Socket::shutdownAllSockets();
	if (m_upipe->writeString("shutdown") == -1)
	{
		OW_THROW(OW_IOException, "Failed writing to OW_HTTPServer shutdown pipe");
	}
	while (m_threadCountSemaphore->getCount() < m_options.maxConnections)
	{
		OW_Thread::yield();
	}
	OW_Thread::yield();
	OW_Socket::deleteShutDownMechanism();
	m_pHttpServerSocket = 0;
	m_pHttpsServerSocket = 0;

	m_options.env->getLogger()->logDebug("HTTP Service has shut down");
}



//////////////////////////////////////////////////////////////////////////////
// This allows the http server to be dynamically loaded
OW_SERVICE_FACTORY(OW_HTTPServer)
