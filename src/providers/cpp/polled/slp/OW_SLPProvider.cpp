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
#include "OW_Types.h"
#include "OW_Format.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_CIMOMLocatorSLP.hpp"
#include "OW_SocketAddress.hpp"


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


extern "C"
{
#include <slp.h>
}

static const long POLLING_INTERVAL = 60 * 5;
static const long INITIAL_POLLING_INTERVAL = 5;

extern "C"
{
static 
void
slpRegReport(SLPHandle hdl, SLPError errArg, void* cookie)
{
	(void)hdl;
	if(errArg < SLP_OK)
	{
		OW_LoggerRef* pLogger = (OW_LoggerRef*)cookie;
		(*pLogger)->logError(format("cimom received error durring SLP registration: %1",
			(int)errArg));
	}
}
}

class OW_SLPProvider : public OW_CppPolledProviderIFC
{
public:

	/**
	 * @return The amount of seconds before the first call to the poll method.
	 * If this method returns zero, then the poll method is never called.
	 */
	virtual OW_Int32 getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef &env) 
	{
		if (env->getConfigItem(OW_ConfigOpts::HTTP_SLP_DISABLED_opt).equalsIgnoreCase("true"))
		{
			return 0;
		}
		OW_Int32 rval = INITIAL_POLLING_INTERVAL;
		env->getLogger()->logDebug(format(
			"OW_SLPProvider::getInitialPollingInterval returning %1", 
			INITIAL_POLLING_INTERVAL).c_str());

		m_httpsPort = env->getConfigItem(OW_ConfigOpts::HTTPS_PORT_opt);
		m_httpPort = env->getConfigItem(OW_ConfigOpts::HTTP_PORT_opt);

		if (m_httpsPort.toInt32() < 1 && m_httpPort.toInt32() < 1)
		{
			return 0;
		}

		m_useDigest = env->getConfigItem(OW_ConfigOpts::HTTP_USE_DIGEST_opt)
				.equalsIgnoreCase("true");
		m_allowAnonymous = env->getConfigItem(OW_ConfigOpts::ALLOW_ANONYMOUS_opt)
				.equalsIgnoreCase("true");
	
		return rval;
	}
	/**
	 * Called by the CIMOM to give this OW_CppPolledProviderIFC to
	 * opportunity to export indications if needed.
	 * @param lch	A local CIMOM handle the provider can use to export
	 *					indications if needed.
	 * @return How many seconds before the next call to the poll method. If this
	 * method returns -1 then the last polling interval will be used. If it
	 * returns 0 then the poll method will never be called again.
	 */
	virtual OW_Int32 poll(const OW_ProviderEnvironmentIFCRef &env) 
	{
		doSlpRegister(env);
		return POLLING_INTERVAL;
	}

	/**
	 * Called by the CIMOM when the provider is removed.
	 * This method will be called when the CIMOM is exiting.  The CIM server
	 * will have already been destroyed, so providers should not try to
	 * perform any CIM operations.
	 * @throws OW_CIMException
	 */
	virtual void cleanup() 
	{
	}

private:

	OW_String m_httpsPort;
	OW_String m_httpPort;
	bool m_useDigest;
	bool m_allowAnonymous;

	void doSlpRegister(const OW_ProviderEnvironmentIFCRef& env)
	{

		SLPError err;
		SLPHandle slpHandle;

		OW_String attributes(
			"(namespace=root),(implementation=OpenWbem),(version="OW_VERSION"),"
			"(query-language=WBEMSQL2),(host-os="OW_STRPLATFORM")");

		if (!m_allowAnonymous)
		{
			attributes += ",(authentication=";
			if (m_useDigest)
			{
				attributes += "Digest)";
			}
			else
			{
				attributes += "Basic)";
			}
		}

		if((err = SLPOpen("en", SLP_FALSE, &slpHandle)) != SLP_OK)
		{

			env->getLogger()->logError(format("OW_SLPProvider::doSlpRegister - SLPOpenFailed: %1",
				err).c_str());
			return;
		}

		OW_String hostname = OW_SocketAddress::getAnyLocalHost().getName();

		OW_StringArray urls;
		if (m_httpPort.toInt32() > 0)
		{
			OW_String newUrl = "http://";
			newUrl += hostname + ":" + m_httpPort + "/cimom";
			urls.push_back(newUrl);
		}
		if (m_httpsPort.toInt32() > 0)
		{
			OW_String newUrl = "https://";
			newUrl += hostname + ":" + m_httpsPort + "/cimom";
			urls.push_back(newUrl);
		}
		for(size_t i = 0; i < urls.size(); i++)
		{
			OW_String urlString;
			urlString = OW_CIMOM_SLP_URL_PREFIX;
			urlString += urls[i];

			// Register URL with SLP

			OW_LoggerRef lgr = env->getLogger();
			err = SLPReg(slpHandle,		// SLP Handle from open
				urlString.c_str(),		// Service URL
				POLLING_INTERVAL+60,		// Length of time registration last
				0,								// Service type (not used)
				attributes.c_str(),		// Attributes string
				SLP_TRUE,					// Fresh registration (Always true for OpenSLP)
				slpRegReport,				// Call back for registration error reporting
				&lgr);						// Give cimom handle to callback
			if(err != SLP_OK)
			{
				env->getLogger()->logError(format("cimom failed to registered url with SLP: %1",
					urlString).c_str());
			}
			else
			{
				env->getLogger()->logDebug(format("cimom registered service url with SLP: %1",
					urlString).c_str());
			}
		}

		SLPClose(slpHandle);
	}

};



OW_NOIDPROVIDERFACTORY(OW_SLPProvider)

