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
#include "OW_config.h"
#include "OW_Types.hpp"
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

namespace OpenWBEM
{

static const Int32 POLLING_INTERVAL = 60 * 5;
static const Int32 INITIAL_POLLING_INTERVAL = 5;
extern "C"
{
static
void
slpRegReport(SLPHandle hdl, SLPError errArg, void* cookie)
{
	(void)hdl;
	if(errArg < SLP_OK)
	{
		LoggerRef* pLogger = (LoggerRef*)cookie;
		(*pLogger)->logError(format("cimom received error durring SLP registration: %1",
			(int)errArg));
	}
}
}
class SLPProvider : public CppPolledProviderIFC
{
public:
	/**
	 * @return The amount of seconds before the first call to the poll method.
	 * If this method returns zero, then the poll method is never called.
	 */
	virtual Int32 getInitialPollingInterval(const ProviderEnvironmentIFCRef &env)
	{
		if (env->getConfigItem(ConfigOpts::HTTP_SLP_DISABLED_opt).equalsIgnoreCase("true"))
		{
			return 0;
		}
		Int32 rval = INITIAL_POLLING_INTERVAL;
		env->getLogger()->logDebug(format(
			"SLPProvider::getInitialPollingInterval returning %1",
			INITIAL_POLLING_INTERVAL).c_str());
		m_httpsPort = env->getConfigItem(ConfigOpts::HTTPS_PORT_opt, OW_DEFAULT_HTTPS_PORT);
		m_httpPort = env->getConfigItem(ConfigOpts::HTTP_PORT_opt, OW_DEFAULT_HTTP_PORT);
		Int32 httpsPort = 0, httpPort = 0;
		try
		{
			httpsPort = m_httpsPort.toInt32();
		}
		catch (const StringConversionException&)
		{
		}
		try
		{
			httpPort = m_httpPort.toInt32();
		}
		catch (const StringConversionException&)
		{
		}
		if (httpsPort < 1 && httpPort < 1)
		{
			return 0;
		}
		m_useDigest = env->getConfigItem(ConfigOpts::HTTP_USE_DIGEST_opt)
				.equalsIgnoreCase("true");
		m_allowAnonymous = env->getConfigItem(ConfigOpts::ALLOW_ANONYMOUS_opt, OW_DEFAULT_ALLOW_ANONYMOUS)
				.equalsIgnoreCase("true");
	
		return rval;
	}
	/**
	 * Called by the CIMOM to give this CppPolledProviderIFC to
	 * opportunity to export indications if needed.
	 * @param lch	A local CIMOM handle the provider can use to export
	 *					indications if needed.
	 * @return How many seconds before the next call to the poll method. If this
	 * method returns -1 then the last polling interval will be used. If it
	 * returns 0 then the poll method will never be called again.
	 */
	virtual Int32 poll(const ProviderEnvironmentIFCRef &env)
	{
		doSlpRegister(env);
		return POLLING_INTERVAL;
	}
private:
	String m_httpsPort;
	String m_httpPort;
	bool m_useDigest;
	bool m_allowAnonymous;
	void doSlpRegister(const ProviderEnvironmentIFCRef& env)
	{
		SLPError err;
		SLPHandle slpHandle;
		String attributes(
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
			env->getLogger()->logError(format("SLPProvider::doSlpRegister - SLPOpenFailed: %1",
				err).c_str());
			return;
		}
		String hostname = SocketAddress::getAnyLocalHost().getName();
		StringArray urls;
		try
		{
			if (m_httpPort.toInt32() > 0)
			{
				String newUrl = "http://";
				newUrl += hostname + ":" + m_httpPort + "/cimom";
				urls.push_back(newUrl);
			}
		}
		catch (const StringConversionException&)
		{
		}
		try
		{
			if (m_httpsPort.toInt32() > 0)
			{
				String newUrl = "https://";
				newUrl += hostname + ":" + m_httpsPort + "/cimom";
				urls.push_back(newUrl);
			}
		}
		catch (const StringConversionException&)
		{
		}
		for(size_t i = 0; i < urls.size(); i++)
		{
			String urlString;
			urlString = OW_CIMOM_SLP_URL_PREFIX;
			urlString += urls[i];
			// Register URL with SLP
			LoggerRef lgr = env->getLogger();
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
				env->getLogger()->logError(format("cimom failed to register url with SLP: %1",
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

} // end namespace OpenWBEM

OW_NOIDPROVIDERFACTORY(OpenWBEM::SLPProvider)

