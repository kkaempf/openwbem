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
#include "OW_InetServerSocket.hpp"
#include "OW_IOException.hpp"
#include "OW_InetSocket.hpp"
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
#include "OW_InetSocketBaseImpl.hpp" // for setDumpFiles()

#ifdef OW_HAVE_SLP_H
#ifdef OW_GNU_LINUX
#define OW_STRPLATFORM "Linux"
#endif

#ifdef OW_OPENUNIX
#define OW_STRPLATFORM "OpenUnix"
#endif

#ifdef OW_SOLARIS
#define OW_STRPLATFORM "Solaris"
#endif

#ifdef OW_OPENSERVER
#define OW_STRPLATFORM "OpenServer"
#endif

#ifndef OW_STRPLATFORM
#error "OW_STRPLATFORM is undefined"
#endif

static const long POLLING_INTERVAL = 60 * 5;
static const long INITIAL_POLLING_INTERVAL = 5;

#endif	// OW_HAVE_SLP_H


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
	, m_slpRegistrator(this)
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
	
	if (info.length() < 1)
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
	OW_String item = env->getConfigItem(OW_ConfigOpts::HTTP_PORT_opt);
	if (item.length() == 0)
	{
		item = DEFAULT_HTTP_PORT;
	}
	m_options.httpPort = item.toInt32();

	item = env->getConfigItem(OW_ConfigOpts::HTTPS_PORT_opt);
	if (item.length() == 0)
	{
		item = DEFAULT_HTTPS_PORT;
	}
	m_options.httpsPort = item.toInt32();

	item = env->getConfigItem(OW_ConfigOpts::MAX_CONNECTIONS_opt);
	if (item.length() == 0)
	{
		item = DEFAULT_MAX_CONNECTIONS;
	}
	m_options.maxConnections = item.toInt32() + 1;

	item = env->getConfigItem(OW_ConfigOpts::SINGLE_THREAD_opt);
	if (item.length() == 0)
	{
		item = "false";
	}
	m_options.isSepThread = !item.equalsIgnoreCase("true");

	item = env->getConfigItem(OW_ConfigOpts::ENABLE_DEFLATE_opt);
	if (item.length() == 0)
	{
		item = "true";
	}
	m_options.enableDeflate = !item.equalsIgnoreCase("false");

	item = env->getConfigItem(OW_ConfigOpts::HTTP_USE_DIGEST_opt);
	if (item.length() == 0)
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

	OW_InetSocketBaseImpl::setDumpFiles(
		env->getConfigItem(OW_ConfigOpts::DUMP_SOCKET_IO_opt) + "/owHTTPSockDumpIn",
		env->getConfigItem(OW_ConfigOpts::DUMP_SOCKET_IO_opt) + "/owHTTPSockDumpOut");
}

//////////////////////////////////////////////////////////////////////////////
class OW_HTTPServerSelectableCallback : public OW_SelectableCallbackIFC
{
public:
	OW_HTTPServerSelectableCallback(bool isHTTPS,
		OW_HTTPServer* httpServer)
		: OW_SelectableCallbackIFC()
		, m_isHTTPS(isHTTPS)
        , m_HTTPServer(httpServer)
	{
	}

	virtual ~OW_HTTPServerSelectableCallback() {}

	virtual void doSelected()
	{
		try
		{

			OW_Reference<OW_InetServerSocket> pServerSocket;
			if(m_isHTTPS)
			{
				pServerSocket = m_HTTPServer->m_pHttpsServerSocket;
			}
			else
			{
				pServerSocket = m_HTTPServer->m_pHttpServerSocket;
			}

			OW_InetSocket socket = pServerSocket->accept(2);

			m_HTTPServer->m_options.env->getLogger()->logCustInfo(
				 format("Received connection on port %1 from %2",
				 socket.getLocalAddress().toString(),
				 socket.getPeerAddress().toString()));

			m_HTTPServer->incThreadCount();

			OW_HTTPServer::Options newOpts = m_HTTPServer->m_options;
			OW_RunnableRef rref(new OW_HTTPSvrConnection(socket,
				 m_HTTPServer, m_HTTPServer->m_upipe, newOpts));

			OW_Thread::run(rref, m_HTTPServer->m_options.isSepThread);
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
};

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::startService()
{
	OW_ServiceEnvironmentIFCRef env = m_options.env;
	OW_LoggerRef lgr = env->getLogger();
	lgr->logDebug("HTTP Service is starting...");

	if (m_options.httpPort < 0 && m_options.httpsPort < 0)
	{
		OW_THROW(OW_SocketException, "No ports to listen on");
	}

	if (m_options.httpPort >= 0)
	{
		OW_UInt16 lport = static_cast<OW_UInt16>(m_options.httpPort);
		m_pHttpServerSocket = new OW_InetServerSocket;
		m_pHttpServerSocket->doListen(lport, false, 1000, true);
		m_options.httpPort = m_pHttpServerSocket->getLocalAddress().getPort();

		lgr->logCustInfo(format("HTTP server listening on port: %1",
		   m_options.httpPort));

		OW_String URL = "http://" + OW_InetAddress::getAnyLocalHost().getName()
			+ ":" + OW_String(m_options.httpPort) + "/cimom";
		addURL(OW_URL(URL));
		
		OW_SelectableCallbackIFCRef cb(new OW_HTTPServerSelectableCallback(
			false, this));

		env->addSelectable(m_pHttpServerSocket, cb);
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
			m_pHttpsServerSocket = new OW_InetServerSocket;
			m_pHttpsServerSocket->doListen(lport, true, 1000, true);

			m_options.httpsPort =
			   m_pHttpsServerSocket->getLocalAddress().getPort();

			lgr->logCustInfo(format("HTTPS server listening on port: %1",
			   m_options.httpsPort));

			OW_String URL = "https://" +
				OW_InetAddress::getAnyLocalHost().getName() + ":" +
				OW_String(m_options.httpsPort) + "/cimom";

			addURL(OW_URL(URL));

			OW_SelectableCallbackIFCRef cb(new OW_HTTPServerSelectableCallback(
				true, this));

			env->addSelectable(m_pHttpsServerSocket, cb);
		}
		else
#endif // #ifndef OW_NO_SSL
		{
			if (m_options.httpPort < 0)
			{
				OW_THROW(OW_SocketException, "No ports to listen on.  "
					"SSL unavailable (SSL not initialized in server mode) "
					"and no http port defined.");
			}

			lgr->logError(format("Unable to listen on port %1.  "
				"SSL not initialized in server mode.", m_options.httpsPort));
		}
	} // if (m_httpsPort > 0)

	OW_InetSocket::createShutDownMechanism();


	if(env->getConfigItem(
		OW_ConfigOpts::HTTP_SLP_DISABLED_opt).equalsIgnoreCase("true"))
	{
		lgr->logCustInfo("CIMOM SLP Registration has been disabled in config"
			" file");
	}
	else
	{
		// Start SLP Registration thread
		m_slpRegistrator.start();
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
OW_InetAddress
OW_HTTPServer::getLocalHTTPAddress()
{
	if (m_pHttpServerSocket)
	{
		return m_pHttpServerSocket->getLocalAddress();
	}
	else
	{
		return OW_InetAddress::allocEmptyAddress();
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_InetAddress
OW_HTTPServer::getLocalHTTPSAddress()
{
	if (m_pHttpsServerSocket)
	{
		return m_pHttpsServerSocket->getLocalAddress();
	}
	else
	{
		return OW_InetAddress::allocEmptyAddress();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::shutdown()
{
	m_options.env->getLogger()->logDebug("HTTP Service is shutting down...");

	m_slpRegistrator.shutdown();

	OW_InetSocket::shutdownAllSockets();
	m_upipe->write("shutdown");
	while (m_threadCountSemaphore->getCount() < m_options.maxConnections)
	{
		OW_Thread::yield();
	}
	OW_Thread::yield();
	OW_InetSocket::deleteShutDownMechanism();
	m_pHttpServerSocket = 0;
	m_pHttpsServerSocket = 0;

	m_options.env->getLogger()->logDebug("HTTP Service has shut down");
}

#ifdef OW_HAVE_SLP_H

//////////////////////////////////////////////////////////////////////////////
void
OW_HTTPServer::doSlpRegister()
{
	OW_LoggerRef logger = m_options.env->getLogger();
	SLPError err;
	SLPHandle slpHandle;

	OW_String attributes(
		"(namespace=root),(implementation=OpenWbem),(version="OW_VERSION"),"
		"(query-language=WBEMSQL2),(host-os="OW_STRPLATFORM")");

	if((err = SLPOpen("en", SLP_FALSE, &slpHandle)) != SLP_OK)
	{

		logger->logError(format("SLP open operation failed: %1", err));
		return;
	}

	OW_String urlString;
	OW_String addrString;

	for(size_t i = 0; i < m_urls.size(); i++)
	{
		addrString = m_urls[i].toString();
		urlString = OW_CIMOM_SLP_URL_PREFIX;
		urlString += addrString;

		// Register URL with SLP
		err = SLPReg(slpHandle,		// SLP Handle from open
			urlString.c_str(),		// Service URL
			POLLING_INTERVAL+60,	// Length of time registration last
			0,						// Service type (not used)
			attributes.c_str(),		// Attributes string
			SLP_TRUE,				// Fresh registration (Always true for OpenSLP)
			slpRegReport,			// Call back for registration error reporting
			(void*)&logger);		// Give logger to callback

		if(err != SLP_OK)
		{
			logger->logError(format("CIMOM failed to registered url with SLP:"
				" %1", addrString).c_str());
		}
		else
		{
			logger->logDebug(format("CIMOM registered service url with SLP: %1",
				urlString).c_str());
		}
	}

	SLPClose(slpHandle);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_HTTPServer::slpRegReport(SLPHandle /*hdl*/, SLPError errArg, void* cookie)
{
	if(errArg < SLP_OK)
	{
		const OW_LoggerRef* logger = (const OW_LoggerRef*)cookie;
		(*logger)->logError(format("CIMOM received error during SLP"
			" registration: %1", (int)errArg));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPSlpRegistrator::start()
{
	if(!m_isRunning)
	{
		OW_Thread::run(OW_RunnableRef(this, true));
		OW_Thread::yield();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPSlpRegistrator::shutdown()
{
	if(m_isRunning)
	{
		OW_LoggerRef logger = m_pServer->getEnvironment()->getLogger();
		logger->logDebug("CIMOM SLP registration process shutting down...");

		m_shuttingDown = true;
		m_threadEvent.signal();
		while(m_isRunning)
		{
			OW_Thread::yield();
		}

		logger->logDebug("CIMOM SLP registration process has shutdown");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
HTTPSlpRegistrator::run()
{
	OW_ServiceEnvironmentIFCRef env = m_pServer->getEnvironment();
	OW_LoggerRef logger = env->getLogger();
	m_shuttingDown = false;
	m_isRunning = true;

	logger->logCustInfo("CIMOM SLP registration process running...");
	m_pServer->doSlpRegister();

	m_threadEvent.reset();
	while(!m_shuttingDown)
	{
		m_threadEvent.waitForSignal(POLLING_INTERVAL * 1000);
		m_threadEvent.reset();
		if(m_shuttingDown)
		{
			break;
		}

		m_pServer->doSlpRegister();
	}

	m_isRunning = false;

	logger->logCustInfo("CIMOM SLP registration process exiting");
}

#endif	// OW_HAVE_SLP_H

//////////////////////////////////////////////////////////////////////////////
// This allows the http server to be dynamically loaded
OW_SERVICE_FACTORY(OW_HTTPServer)
