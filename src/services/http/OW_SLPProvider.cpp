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
#include "OW_CIMOMLocatorSLP.hpp"
#include "OW_URL.hpp"

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

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

static const long POLLING_INTERVAL = 60 * 5;
static const long INITIAL_POLLING_INTERVAL = 5;

class OW_SLPProvider : public OW_CppPolledProviderIFC
{
public:

	OW_SLPProvider();
	~OW_SLPProvider();

	virtual long getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env);
	virtual long poll(const OW_ProviderEnvironmentIFCRef& env);
	virtual void initialize(const OW_ProviderEnvironmentIFCRef& env);
	virtual void cleanup();

	void doSlpRegister(const OW_LoggerRef& logger);

	static void slpRegReport(SLPHandle hdl, SLPError errArg, void* cookie);

};

//////////////////////////////////////////////////////////////////////////////
OW_SLPProvider::OW_SLPProvider() :
	OW_CppPolledProviderIFC()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_SLPProvider::~OW_SLPProvider()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SLPProvider::initialize(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug("OW_SLPProvider::initialize called");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SLPProvider::cleanup()
{
}

//////////////////////////////////////////////////////////////////////////////
long
OW_SLPProvider::getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug(format("OW_SLPProvider::getInitialPollingInterval returning "
		"%1", INITIAL_POLLING_INTERVAL).c_str());

	return INITIAL_POLLING_INTERVAL;
}

//////////////////////////////////////////////////////////////////////////////
long
OW_SLPProvider::poll(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug("OW_SLPProvider::poll called. Registering CIMOM with SLP");

	doSlpRegister(env->getLogger());
	return POLLING_INTERVAL;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SLPProvider::doSlpRegister(const OW_LoggerRef& logger)
{
	/* TODO: rework this somehow
	OW_HTTPServer* phttpServer = OW_Environment::getHTTPServer();
	if(!phttpServer)
	{
		return;
	}
	*/

	SLPError err;
	SLPHandle slpHandle;

	OW_String attributes(
		"(namespace=root),(implementation=OpenWbem),(version="OW_VERSION"),"
		"(query-language=WBEMSQL2),(host-os="OW_STRPLATFORM")");

	if((err = SLPOpen("en", SLP_FALSE, &slpHandle)) != SLP_OK)
	{

		logger->logError(format("OW_SLPProvider::doSlpRegister - SLPOpenFailed: %1",
			err).c_str());
		return;
	}

	OW_String urlString;
	OW_String addrString;
	//TODO: Fixme! OW_Array<OW_URL> urls = phttpServer->getURLs();
	OW_Array<OW_URL> urls;
	for(size_t i = 0; i < urls.size(); i++)
	{
		addrString = urls[i].toString();
		urlString = OW_CIMOM_SLP_URL_PREFIX;
		urlString += addrString;

		// Register URL with SLP

		err = SLPReg(slpHandle,		// SLP Handle from open
			urlString.c_str(),		// Service URL
			POLLING_INTERVAL+60,		// Length of time registration last
			0,								// Service type (not used)
			attributes.c_str(),		// Attributes string
			SLP_TRUE,					// Fresh registration (Always true for OpenSLP)
			slpRegReport,				// Call back for registration error reporting
			(void*)&logger);			// Give logger to callback
		if(err != SLP_OK)
		{
			logger->logError(format("cimom failed to registered url with SLP: %1",
				addrString).c_str());
		}
		else
		{
			logger->logDebug(format("cimom registered service url with SLP: %1",
				urlString).c_str());
		}
	}

	SLPClose(slpHandle);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_SLPProvider::slpRegReport(SLPHandle /*hdl*/, SLPError errArg, void* cookie)
{
	if(errArg < SLP_OK)
	{
		const OW_LoggerRef* logger = (const OW_LoggerRef*)cookie;
		(*logger)->logError(format("cimom received error durring SLP registration: %1",
			(int)errArg).c_str());
	}
}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY(OW_SLPProvider);



