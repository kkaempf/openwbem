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
#include "OW_ProviderAgent.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_XMLExecute.hpp"
#include "OW_CIMClass.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_Format.hpp"
#include "OW_CmdLineParser.hpp"

#include <csignal>
#include <iostream> // for cout and cerr




using namespace OpenWBEM;
using namespace WBEMFlags;
using namespace std;

class RPALogger : public Logger
{
protected:
	virtual void doLogMessage(const String &message, const ELogLevel) const
	{
		cerr << message << endl;
	}
};

UnnamedPipeRef sigPipe;
extern "C"
void sig_handler(int)
{
	sigPipe->writeInt(0);
}

enum
{
	HELP_OPT,
	VERSION_OPT,
	URL_OPT,
	CONFIG_OPT,
	HTTP_PORT_OPT,
	HTTPS_PORT_OPT,
	UDS_FILENAME_OPT,
	PROVIDER_OPT
};

CmdLineParser::Option g_options[] = 
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{URL_OPT, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the cimom callback url."},
	{CONFIG_OPT, 'c', "config", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the config file.  Command line options take precedence over config file options."},
	{HTTP_PORT_OPT, '\0', "http-port", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the http port."},
	{HTTPS_PORT_OPT, '\0', "https-port", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the https port."},
	{UDS_FILENAME_OPT, '\0', "uds-filename", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the filename of the unix domain socket."},
	{PROVIDER_OPT, 'p', "provider", CmdLineParser::E_REQUIRED_ARG, 0, "Specify a filename of a provider library to use. May be used multiple times."},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};


//////////////////////////////////////////////////////////////////////////////
// TODO stole this stuff from OW_CppProviderIFC.cpp. 
// need to stick them in a common header. 
typedef CppProviderBaseIFC* (*ProviderCreationFunc)();
typedef const char* (*versionFunc_t)();

CppProviderBaseIFCRef
//getProvider(const ProviderEnvironmentIFCRef& env, const char* provIdString,
getProvider(const String& libName, LoggerRef logger)
{
	String provId = libName.substring(libName.lastIndexOf(OW_FILENAME_SEPARATOR)+1); 
	// chop of lib and .so
	provId = provId.substring(3,
			  provId.length() - (strlen(OW_SHAREDLIB_EXTENSION) + 3));

	SharedLibraryLoaderRef ldr =
		SharedLibraryLoader::createSharedLibraryLoader();
	if(ldr.isNull())
	{
		logger->logError("C++ provider ifc failed to get shared lib loader");
		return CppProviderBaseIFCRef();
	}
	logger->logDebug(Format("getProvider loading library: %1",
		libName));
	SharedLibraryRef theLib = ldr->loadSharedLibrary(libName, logger); 
	if(theLib.isNull())
	{
		logger->logError(Format("C++ provider ifc failed to load library: %1 "
			"for provider id %2", libName, provId));
		return CppProviderBaseIFCRef();
	}
	versionFunc_t versFunc;
	if (!theLib->getFunctionPointer("getOWVersion", versFunc))
	{
		logger->logError("C++ provider ifc failed getting"
			" function pointer to \"getOWVersion\" from library");
		return CppProviderBaseIFCRef();
	}
	const char* strVer = (*versFunc)();
	if(strcmp(strVer, OW_VERSION))
	{
		logger->logError(Format("C++ provider ifc got invalid version from provider:"
			" %1", libName));
		return CppProviderBaseIFCRef();
	}
	ProviderCreationFunc createProvider;
	String creationFuncName = String("createProvider") + provId;
	if(!theLib->getFunctionPointer(creationFuncName, createProvider))
	{
		logger->logError(Format("C++ provider ifc: Libary %1 does not contain"
			" %2 function", libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}
	CppProviderBaseIFC* pProv = (*createProvider)();
	if(!pProv)
	{
		logger->logError(Format("C++ provider ifc: Libary %1 - %2 returned null"
			" provider", libName, creationFuncName));
		return CppProviderBaseIFCRef();
	}
	/*
	logger->logDebug(Format("C++ provider ifc loaded library %1. Calling initialize"
		" for provider %2", libName, provId));
	pProv->initialize(env);	// Let provider initialize itself
	*/
	logger->logDebug(Format("C++ provider ifc: provider %1 loaded and initialized",
		provId));
	CppProviderBaseIFCRef rval(theLib, pProv);
	return rval;
}

void Usage()
{
	cerr << "Usage: owprovideragent [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}

int main(int argc, char* argv[])
{
	try
	{
		CmdLineParser parser(argc, argv, g_options);

		sigPipe = UnnamedPipe::createUnnamedPipe();
		sigPipe->setOutputBlocking(UnnamedPipe::E_NONBLOCKING);
		sigPipe->setWriteTimeout(0);
		signal(SIGINT, sig_handler);

		ConfigFile::ConfigMap cmap; 

		// set up some defaults
		cmap[ConfigOpts::HTTP_PORT_opt] = String(-1);
		cmap[ConfigOpts::HTTPS_PORT_opt] = String(-1);
		cmap[ConfigOpts::MAX_CONNECTIONS_opt] = String(10);
		cmap[ConfigOpts::ENABLE_DEFLATE_opt] = "true";
		cmap[ConfigOpts::HTTP_USE_DIGEST_opt] = "false";
		cmap[ConfigOpts::USE_UDS_opt] = "true";
		cmap[ProviderAgent::DynamicClassRetieval_opt] = "true";

		String url;

		if (parser.isSet(HELP_OPT))
		{
			Usage();
			return 0;
		}
		else if (parser.isSet(VERSION_OPT))
		{
			cout << "owprovideragent (OpenWBEM) " << OW_VERSION << '\n';
			cout << "Written by Bart Whiteley and Dan Nuffer.\n";
			return 0;
		}

		LoggerRef logger(new RPALogger);
		logger->setLogLevel(E_DEBUG_LEVEL);



		Reference<AuthenticatorIFC> authenticator; 
		RequestHandlerIFCRef rh(SharedLibraryRef(0), new XMLExecute); 
		Array<RequestHandlerIFCRef> rha; 
		rha.push_back(rh); 

		Array<CppProviderBaseIFCRef> pra;
		StringArray providers = parser.getOptionValueList(PROVIDER_OPT);
		if (providers.empty())
		{
			cerr << "Error: no providers specified\n";
			Usage();
			return 1;
		}
		for (size_t i = 0; i < providers.size(); ++i)
		{
			String libName(providers[i]); 
			CppProviderBaseIFCRef provider = getProvider(libName, logger); 
			if (!provider->getInstanceProvider() 
				&& !provider->getSecondaryInstanceProvider()
	#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
				&& !provider->getAssociatorProvider()
	#endif
				&& !provider->getMethodProvider())
			{
				cerr << "Error: Provider " << libName << " is not a supported type" << endl;
				return 1; 
			}
			pra.push_back(provider); 
		}

		CIMClassArray cra; 

		ProviderAgent pa(cmap, pra, cra, rha, authenticator, logger, url);
		
		// wait until we get a SIGINT as a shutdown signal
		int dummy;
		sigPipe->readInt(&dummy);

		cout << "Shutting down." << endl;
		pa.shutdownHttpServer();
		return 0;
	}
	catch (CmdLineParserException& e)
	{
		switch (e.getErrorCode())
		{
			case CmdLineParser::E_INVALID_OPTION:
				cerr << "unknown option: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_MISSING_ARGUMENT:
				cerr << "missing argument for option: " << e.getMessage() << '\n';
			break;
			default:
				cerr << "failed parsing command line options\n";
			break;
		}
		Usage();
	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}

