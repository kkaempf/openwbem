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
#include "OW_IPCService.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Select.hpp"
#include "OW_MutexLock.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_ConfigOpts.hpp"

#include <iostream>
#include <cstring>

/*
class OW_IPCServiceEnvironment : public OW_ServiceEnvironmentIFC
{
public:
	OW_IPCServiceEnvironment(OW_ServiceEnvironmentIFCRef env,
		const OW_String& userName)
		: OW_ServiceEnvironmentIFC()
		, m_env(env)
		, m_userName(userName) {}

	virtual OW_LoggerRef getLogger() const
	{
		return m_env->getLogger();
	}

	virtual OW_String getConfigItem(const OW_String &name) const
	{
		return (name.equals(OW_ConfigOpts::)) ? m_userName
			: m_env->getConfigItem(name);
	}

	virtual void setConfigItem(const OW_String &item, const OW_String &value,
		OW_Bool overwritePrevious)
	{
		m_env->setConfigItem(item, value, overwritePrevious);
	}

	virtual OW_Bool authenticate(OW_String &userName, const OW_String &info,
		OW_String &details)
	{
		return m_env->authenticate(userName, info, details);
	}

	virtual void addSelectable(OW_SelectableIFCRef obj,
		OW_SelectableCallbackIFCRef cb)
	{
		m_env->addSelectable(obj, cb);
	}

	virtual void removeSelectable(OW_SelectableIFCRef obj,
		OW_SelectableCallbackIFCRef cb)
	{
		m_env->removeSelectable(obj, cb);
	}

	virtual OW_RequestHandlerIFCRef getRequestHandler(const OW_String &id) const
	{
		return m_env->getRequestHandler(id);
	}

	virtual OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_String &username,
		const OW_Bool doIndications)
	{
		return m_env->getCIMOMHandle(username, doIndications);
	}

private:
	OW_ServiceEnvironmentIFCRef m_env;
	OW_String m_userName;
};
*/


class IPCSelectableCallback : public OW_SelectableCallbackIFC
{
public:
	IPCSelectableCallback(OW_IPCService* pservice)
		: OW_SelectableCallbackIFC(), m_pservice(pservice) {}

protected:
	virtual void doSelected(OW_SelectableIFCRef& selectedObject)
			{ (void)selectedObject; m_pservice->doSelected(); }
	OW_IPCService* m_pservice;
};

//////////////////////////////////////////////////////////////////////////////
OW_IPCService::OW_IPCService()
	: OW_ServiceIFC()
	, m_server(true)
	, m_env()
	, m_upipe(0)
	, m_connCount(0)
	, m_countGuard()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_IPCService::~OW_IPCService()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCService::shutdown()
{
	m_env->getLogger()->logDebug("IPC Service is shutting down...");

	// Send shutdown notification to connection handlers
	m_upipe->write(int(-1));
	while(m_connCount > 0)
	{
		OW_Thread::yield();
	}

	m_server.cleanup();
	m_env->getLogger()->logDebug("IPC Service has shut down");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCService::startService()
{
	m_env->getLogger()->logDebug("IPC Service starting...");

	m_upipe = OW_UnnamedPipe::createUnnamedPipe(false);
	m_upipe->open();
	m_server.initialize();
	if(m_env.getPtr())
	{
		OW_SelectableIFCRef sref(&m_server, true);
		m_env->addSelectable(sref, OW_SelectableCallbackIFCRef(
			new IPCSelectableCallback(this)));
	}
	m_env->getLogger()->logDebug("IPC Service is now running");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCService::setServiceEnvironment(OW_ServiceEnvironmentIFCRef env)
{
	m_env = env;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCService::doSelected()
{
	OW_LoggerRef lgr = m_env->getLogger();
	OW_IPCConnection conn = m_server.getClientConnection();
	if(!conn)
	{
		lgr->logError("Got NULL local API connnection - Ignoring");
		return;
	}

	OW_Bool isSepThread = !(m_env->getConfigItem(
		OW_ConfigOpts::SINGLE_THREAD_opt).equalsIgnoreCase("true"));

	lgr->logDebug(format("Received API connection. User: %1",
		conn.getUserName()));

	OW_RunnableRef rref(new OW_IPCConnectionHandler(this, conn));
	OW_Thread::run(rref, isSepThread);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCService::incConnCount()
{
	OW_MutexLock ml(m_countGuard);
	m_connCount++;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCService::decConnCount()
{
	OW_MutexLock ml(m_countGuard);
	if((--m_connCount) < 0)
	{
		m_connCount = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_IPCConnectionHandler::OW_IPCConnectionHandler(OW_IPCService* pservice,
	OW_IPCConnection conn)
	: OW_Runnable()
	, m_pservice(pservice)
	, m_conn(conn)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_IPCConnectionHandler::~OW_IPCConnectionHandler()
{
	m_conn.disconnect();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IPCConnectionHandler::run()
{
	class RunMonitor
	{
	public:
		RunMonitor(OW_IPCService* pservice) : m_pservice(pservice)
		{
			m_pservice->incConnCount();
		}

		~RunMonitor()
		{
			m_pservice->decConnCount();
			
		}

		OW_IPCService* m_pservice;
	};
	RunMonitor rm(m_pservice);
	(void)rm;

	OW_ServiceEnvironmentIFCRef env = m_pservice->getEnvironment();
	OW_LoggerRef lgr = env->getLogger();
	OW_UnnamedPipeRef upipe = m_pservice->getUPipe();

	try
	{

		std::istream& istrm = m_conn.getInputStream();
		std::ostream& ostrm = m_conn.getOutputStream();
		OW_String userName = m_conn.getUserName();
		

		OW_SelectTypeArray selra;
		selra.append(upipe->getSelectObj());
		selra.append(m_conn.getSelectObj());
		int selndx;
		OW_Int32 actionValue = 0;
		while(actionValue != OW_IPC_CLOSECONN)
		{
			if((selndx = OW_Select::select(selra)) < 0)
			{
				lgr->logError(format("IPC Service: Encountered error waiting"
					" for client to respond. Closing conection to %1",
					m_conn.getUserName()));
				break;
			}

			if(selndx == 0)
			{
				lgr->logError(format("IPC Service: Received shutdown"
					" notification. Closing connection to %1",
					m_conn.getUserName()));
				break;
			}

			OW_BinIfcIO::read(istrm, actionValue, OW_Bool(true));
			switch(actionValue)
			{
				case OW_IPC_AUTHENTICATE:
					{
						OW_String uname = authenticate(ostrm, istrm, userName);
						if(uname.length())
						{
							userName = uname;
						}
					}
					break;

				case OW_IPC_FUNCTIONCALL:
					{

						OW_String contentType;
						OW_BinIfcIO::read(istrm, contentType, OW_Bool(true));
						if (contentType.length() < 1)
						{
							lgr->logError("IPC Service is unable to process connection: "
								"Client failed to provide content type");
							return;
						}
						OW_RequestHandlerIFCRef handler =
							env->getRequestHandler(contentType);

						if(!handler)
						{
							lgr->logError(format("IPC Service is unable to process connection"
								" for %1: No Request Handler Available", m_conn.getUserName()));
							return;
						}
						handler->setEnvironment(env);

						OW_TempFileStream tfs;
						OW_SortedVector<OW_String, OW_String> handlerVars;
						handlerVars[OW_ConfigOpts::USER_NAME_opt] = userName;
						handler->process(&istrm, &ostrm, &tfs, handlerVars);
						if(handler->hasError())
						{
							ostrm << tfs.rdbuf() << std::flush;
						}
					}
					break;

				case OW_IPC_CLOSECONN:
					lgr->logDebug(format("IPC Service: %1 closed connection",
						m_conn.getUserName()));
					break;
			}
		}
	}
	catch(OW_Exception& e)
	{
		lgr->logError("OW_Exception caught in OW_IPCConnectionHandler::run");
		lgr->logError(format("Type: %1", e.type()));
		lgr->logError(format("File: %1", e.getFile()));
		lgr->logError(format("Line: %1", e.getLine()));
		lgr->logError(format("Msg: %1", e.getMessage()));
	}
	catch(...)
	{
		lgr->logError("Unknown exception caught in "
			"OW_IPCConnectionHandler::run");
	}

	m_conn.disconnect();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_IPCConnectionHandler::authenticate(std::ostream& ostrm, std::istream& istrm,
	const OW_String& oldUser)
{
	OW_ServiceEnvironmentIFCRef env = m_pservice->getEnvironment();
	OW_LoggerRef lgr = env->getLogger();
	OW_Bool authenticated = false;
	OW_String details;
	OW_String userName(OW_BinIfcIO::readString(istrm));
	OW_String info(OW_BinIfcIO::readString(istrm));
	if(userName.length())
	{
		if(env->getConfigItem(OW_ConfigOpts::ACL_SUPERUSER_opt).
		   equalsIgnoreCase(oldUser) || oldUser == userName)
		{
			authenticated = true;
		}
		else
		{
			try
			{
				authenticated = env->authenticate(userName, info, details);
			}
			catch(OW_Exception& e)
			{
				authenticated = false;
				details = "Exception in authenticate: ";
				details += e.type();
				details += " - ";
				details += e.getMessage();
			}
		}
	}

	if(!authenticated)
	{
		lgr->logCustInfo(format("USER %1 FAILED TO AUTHENTICATED AS %2 - %3",
			oldUser, userName, details));
		userName.erase();
	}
	else
	{
		if(oldUser != userName)
		{
			lgr->logCustInfo(format("USER %1 AUTHENTICATED AS %2",
				m_conn.getUserName(), userName));
		}
	}

	OW_BinIfcIO::write(ostrm, (OW_Int32)OW_BIN_OK, OW_Bool(true));
	OW_BinIfcIO::writeBool(ostrm, authenticated);
	OW_BinIfcIO::writeString(ostrm, details);
	return userName;
}


//////////////////////////////////////////////////////////////////////////////
// This allows the ipc service to be dynamically loaded
OW_SERVICE_FACTORY(OW_IPCService)
