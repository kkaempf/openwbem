/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_Format.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_CIMOMLocatorSLP.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"


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
		// TODO: Fix this up to use provider instances instead of just config file options, which may not reflect the real state of the cimom.
		if (env->getConfigItem(ConfigOpts::SLP_ENABLE_ADVERTISEMENT_opt, OW_DEFAULT_SLP_ENABLE_ADVERTISEMENT).equalsIgnoreCase("false"))
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
	
		
		m_interopSchemaNamespace = env->getConfigItem(ConfigOpts::INTEROP_SCHEMA_NAMESPACE_opt, OW_DEFAULT_INTEROP_SCHEMA_NAMESPACE);

		m_serviceId = "unknown";
		try
		{
			CIMObjectPathArray a = env->getCIMOMHandle()->enumInstanceNamesA(m_interopSchemaNamespace, "CIM_ObjectManager");
			if (a.size() == 1)
			{
				const CIMObjectPath& om = a[0];
				m_serviceId = om.getKeyT("Name").getValueT().toString();
			}
		}
		catch (CIMException& e)
		{
			env->getLogger()->logError(Format("SLP provider caught (%1) when executing enumInstanceNames(%2, \"CIM_ObjectManager\")", e, m_interopSchemaNamespace));
			env->getLogger()->logError("SLP provider unable to determine service-id");
		}

		m_queryEnabled = !env->getConfigItem(ConfigOpts::WQL_LIB_opt, OW_DEFAULT_WQL_LIB).empty();
		m_indicationEnabled = !env->getConfigItem(ConfigOpts::DISABLE_INDICATIONS_opt, OW_DEFAULT_DISABLE_INDICATIONS).equalsIgnoreCase("true");

		

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
	String m_serviceId;
	String m_interopSchemaNamespace;
	bool m_queryEnabled;
	bool m_indicationEnabled;

	void doSlpRegister(const ProviderEnvironmentIFCRef& env)
	{
		SLPError err;
		SLPHandle slpHandle;
		if((err = SLPOpen("en", SLP_FALSE, &slpHandle)) != SLP_OK)
		{
			env->getLogger()->logError(format("SLPProvider::doSlpRegister - SLPOpenFailed: %1",
				err).c_str());
			return;
		}

		/*
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
		*/
		StringBuffer attributes;

		// service-hi-name - Optional

		// service-hi-description - Optional

		// service-id
		attributes += "(service-id=";
		attributes += m_serviceId;
		attributes += ')';

		// CommunicationMechanism
		attributes += ",(CommunicationMechanism=cim-xml)";

		// OtherCommunicationMechanismDescription
        
		// InteropSchemaNamespace
		attributes += ",(InteropSchemaNamespace=";
		attributes += m_interopSchemaNamespace;
		attributes += ')';

		// ProtocolVersion
		attributes += ",(ProtocolVersion=1.1)";

		// FunctionalProfilesSupported
		attributes += ",(FunctionalProfilesSupported=";
		attributes += 
			"Basic Read"
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
			",Schema Manipulation"
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
			",Basic Write"
			",Instance Manipulation"
#endif
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			",Association Traversal"
#endif
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
			",Qualifier Declaration"
#endif
			;
		if (m_queryEnabled)
		{
			attributes += ",Query Execution";
		}
		if (m_indicationEnabled)
		{
			attributes += ",Indications";
		}
		attributes += ")";

		// FunctionalProfileDescriptions - Only use if we put "Other" in FunctionalProfilesSupported

		// MultipleOperationsSupported
		attributes += ",(MultipleOperationsSupported=true)";

		// AuthenticationMechanismsSupported
		attributes += ",(AuthenticationMechanismsSupported=";
		if (m_allowAnonymous) // this takes precedence over the other schemes
		{
			attributes += "None)";
		}
		else if (m_useDigest)
		{
			attributes += "Digest)";
		}
		else
		{
			attributes += "Basic)";
		}

		// AuthenticationMechansimDescriptions - Only use if we put "Other" in AuthenticationMechanismsSupported

		// Namespace - Optional, and it may be more information that we want to advertise.  Evaluate whether we want this when we've got more experience with it.
		// Classinfo - Options, won't do it for the same reason as Namespace

		// RegisteredProfilesSupported - TODO
		



		String hostname = SocketAddress::getAnyLocalHost().getName();
		StringArray urls;
		try
		{
			if (m_httpPort.toInt32() > 0)
			{
				String newUrl = "http://";
				newUrl += hostname + ":" + m_httpPort;
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
				newUrl += hostname + ":" + m_httpsPort;
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

