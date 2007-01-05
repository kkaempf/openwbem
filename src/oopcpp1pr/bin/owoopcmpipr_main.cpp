/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_OOPCpp1PR.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_String.hpp"
#include "OW_Format.hpp"
#include "OW_FileSystem.hpp"
#include "OW_IOException.hpp"
#include "OW_AutoDescriptor.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_CMPIProviderIFC.hpp"
#include "OW_SharedLibraryException.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_CMPIInstanceProviderProxy.hpp"
#include "OW_CMPIMethodProviderProxy.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_CMPIAssociatorProviderProxy.hpp"
#endif
#include "OW_CMPIIndicationProviderProxy.hpp"

#include <iostream>
using std::cerr;
using std::endl;

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef OW_HAVE_SYS_APPARMOR_H
#include <sys/apparmor.h>
#include "OW_SecureRand.hpp"
#endif


using namespace std;
using namespace OpenWBEM;

namespace
{

class LocalCMPIProvider : public ProviderBaseIFC
{
public:
	LocalCMPIProvider(CMPIFTABLERef& pprov, ::CMPI_Broker& broker)
		: ProviderBaseIFC()
		, m_prov(pprov)
		, m_broker(broker)
		, m_instProv(0)
		, m_methProv(0)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		, m_assocProv(0)
#endif
	{
	}

	///////////////////////////////////////////////////////////////////////////
	virtual InstanceProviderIFC* getInstanceProvider()
	{
		if (!m_instProv);
		{
			if (!m_prov->miVector.instMI)
			{
				return 0;
			}

			m_instProv = InstanceProviderIFCRef(new CMPIInstanceProviderProxy(m_prov));
		}
		return m_instProv->getInstanceProvider();
	}

	///////////////////////////////////////////////////////////////////////////
	virtual SecondaryInstanceProviderIFC* getSecondaryInstanceProvider()
	{
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////
	virtual MethodProviderIFC* getMethodProvider()
	{
		if (!m_methProv);
		{
			if (!m_prov->miVector.methMI)
			{
				return 0;
			}

			m_methProv = MethodProviderIFCRef(new CMPIMethodProviderProxy(m_prov));
		}
		return m_methProv->getMethodProvider();
	}

	///////////////////////////////////////////////////////////////////////////
	virtual IndicationExportProviderIFC* getIndicationExportProvider()
	{
		// TODO: Currently not supported with CMPI
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////
	virtual IndicationProviderIFC* getIndicationProvider()
	{
		// TODO: Currently not supported with CMPI
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////
	virtual PolledProviderIFC* getPolledProvider()
	{
		// TODO: Currently not supported with CMPI
		return 0;
	}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual AssociatorProviderIFC* getAssociatorProvider()
	{
		if (!m_assocProv);
		{
			if (!m_prov->miVector.assocMI)
			{
				return 0;
			}

			m_assocProv = AssociatorProviderIFCRef(new CMPIAssociatorProviderProxy(m_prov));
		}
		return m_assocProv->getAssociatorProvider();
	}
#endif

	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env)
	{
		if (m_prov->miVector.instMI)
		{
			::CMPIOperationContext context;
			CMPI_ContextOnStack eCtx(context);
			CMPI_ThreadContext thr(&m_broker, &eCtx);
			m_prov->miVector.instMI->ft->cleanup(m_prov->miVector.instMI,
				&eCtx, true);
		}
		if (m_prov->miVector.methMI)
		{
			::CMPIOperationContext context;
			CMPI_ContextOnStack eCtx(context);
			CMPI_ThreadContext thr(&m_broker, &eCtx);
			m_prov->miVector.methMI->ft->cleanup(m_prov->miVector.methMI,
				&eCtx, true);
		}
		if (m_prov->miVector.assocMI)
		{
			::CMPIOperationContext context;
			CMPI_ContextOnStack eCtx(context);
			CMPI_ThreadContext thr(&m_broker, &eCtx);
			m_prov->miVector.assocMI->ft->cleanup(m_prov->miVector.assocMI,
				&eCtx, true);
		}
	}

private:
	CMPIFTABLERef m_prov;
	::CMPI_Broker& m_broker;
	InstanceProviderIFCRef m_instProv;
	MethodProviderIFCRef m_methProv;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderIFCRef m_assocProv;
#endif
};

enum
{
	E_HELP_OPT,
	E_VERSION_OPT,
	E_LIBRARY_VERSION_OPT,
	E_PROVIDER_OPT,
	E_LOG_FILE_OPT,
	E_MONITOR_OPT,
	E_LOG_LEVEL_OPT
};

const CmdLineParser::Option g_options[] =
{
	{E_HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options and exit"},
	{E_VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information and exit"},
	{E_LIBRARY_VERSION_OPT, 'l', "libversion", CmdLineParser::E_NO_ARG, 0, "Show the required OpenWBEM library version and exit"},
	{E_PROVIDER_OPT, 'p', "provider", CmdLineParser::E_REQUIRED_ARG, 0, "Load and call <arg>"},
	{E_LOG_FILE_OPT, 0, "logfile", CmdLineParser::E_REQUIRED_ARG, 0, "Debug log file"},
	{E_MONITOR_OPT, 'm', "monitor", CmdLineParser::E_NO_ARG, 0, "Connect to monitor before loading provider library"},
	{E_LOG_LEVEL_OPT, 0, "loglevel", CmdLineParser::E_REQUIRED_ARG, 0,
	 "Lowest category to log"},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

int processCommandLine(
	int argc,
	char* argv[],
	String& provider,
	String& logfile,
	String& loglevel);

void printUsage();


// TODO: CMPI initialized the provider right after it runs. However the provider environment won't work at that time, 
// so it should be changed to initialize using this callback.
class CMPIProvInitializer : public OOPCpp1ProviderRunner::InitializeCallback
{
public:
	CMPIProvInitializer()
	{
	}

private:
	void doInit(const ProviderEnvironmentIFCRef& provenv)
	{
		// nothing to do for now.
	}

};

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int rval = 0;

	String providerLib, logfile, loglevel, providerId;
	int pclrv = processCommandLine(argc, argv, providerLib, logfile, loglevel);
	if (pclrv == -1)
	{
		return 0;
	}
	else if (pclrv != 0)
	{
		return pclrv;
	}
	if (loglevel.empty())
	{
		loglevel = "*";
	}

	AutoDescriptor infd(::dup(0));
	if (infd.get() == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to dup stdin");
	}
	AutoDescriptor outfd(::dup(1));
	if (outfd.get() == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to dup stdout");
	}

	if (::close(0) == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to close stdin");
	}
	if (::close(1) == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to close stdout");
	}

	// Open stdin	== /dev/null
	int fd = ::open("/dev/null", O_RDWR);
	if (fd == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to open /dev/null for stdin");
	}

	// Stdout == /dev/null
	if (::dup(fd) == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to dup /dev/null for stdout");
	}

	UnnamedPipeRef iopipe = UnnamedPipe::createUnnamedPipeFromDescriptor(infd, outfd);
	OOPCpp1ProviderRunner provrunner(iopipe, logfile, loglevel);
	Logger logger(OOPCpp1ProviderRunner::COMPONENT_NAME);
	ProviderEnvironmentIFCRef penv = provrunner.getProviderEnvironment();

/*
	std::pair<FileSystem::Path::ESecurity, String> sec = FileSystem::Path::security(providerLib);
	if (sec.first != FileSystem::Path::E_SECURE_FILE)
	{
		OW_LOG_ERROR(logger, Format("ERROR: %1(%2) is not a secure file.", providerLib, sec.second));
		return 1;
	}
	providerLib = sec.second;
*/
	::CMPI_Broker broker;
	CMPIFTABLERef cmpiprov = CMPIProviderIFC::loadProvider(penv, providerLib, broker);

	if (!cmpiprov)
	{
		OW_LOG_ERROR(logger, Format("CMPI provider %1 did not load", providerLib));
		return 1;
	}

	ProviderBaseIFCRef provider = ProviderBaseIFCRef(new LocalCMPIProvider(cmpiprov, broker));

#ifdef OW_HAVE_SYS_APPARMOR_H
	unsigned int magtok = Secure::rand_uint<unsigned int>(); 
   	String subprofile = FileSystem::Path::basename(providerLib);  
	subprofile = subprofile.substring(3, subprofile.indexOf('.')-3); 
	int aarv = change_hat(subprofile.c_str(), magtok); 
	if (aarv != 0 && errno == EACCES)
	{
		OW_LOG_INFO(logger, Format("AppArmor: Subprofile does not exist: %1", subprofile));
		subprofile = "default_provider_hat"; 
		aarv = change_hat(subprofile.c_str(), magtok);
	}
	if (aarv == 0)
	{
		OW_LOG_INFO(logger, Format("AppArmor: Enforcing subprofile: %1", subprofile));
	}
	else 
	{
		switch (errno)
		{
		case EACCES:
			OW_LOG_INFO(logger, Format("AppArmor: Subprofile does not exist: %1", subprofile));
			break; 
		case EFAULT:
			OW_LOG_ERROR(logger, Format("AppArmor: Internal error while attempting to enforce subprofile: %1", subprofile));
			break; 
		case ENOMEM:
			OW_LOG_ERROR(logger, Format("AppArmor: Insufficient kernel memory to enforce subprofile: %1", subprofile));
			break; 
		default:
			OW_LOG_ERROR(logger, Format("AppArmor: Unknown error while attempting to enforce subprofile: %1: %2", subprofile, strerror(errno)));
		}
	}
#endif

	CMPIProvInitializer cmpiProvInitializer;
	return provrunner.runProvider(provider, providerLib, cmpiProvInitializer);
}

namespace
{

//////////////////////////////////////////////////////////////////////////////
int
processCommandLine(
	int argc,
	char* argv[],
	String& providerLib,
	String& logfile,
	String & loglevel)
{
	try
	{
		CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);

		if (parser.isSet(E_HELP_OPT))
		{
			printUsage();
			return -1;
		}
		else if (parser.isSet(E_VERSION_OPT))
		{
			std::cout << "owoopcpp1pr from " OW_PACKAGE_STRING << std::endl;
			return -1;
		}
		else if (parser.isSet(E_LIBRARY_VERSION_OPT))
		{
			std::cout << OW_OPENWBEM_LIBRARY_VERSION << std::endl;
			return -1;
		}

		providerLib = parser.mustGetOptionValue(E_PROVIDER_OPT, "-p, --provider");
		
		if (parser.isSet(E_LOG_FILE_OPT))
		{
			logfile = parser.getOptionValue(E_LOG_FILE_OPT);
		}
		if (parser.isSet(E_MONITOR_OPT))
		{
			PrivilegeManager::connectToMonitor();
		}
		if (parser.isSet(E_LOG_LEVEL_OPT))
		{
			loglevel = parser.getOptionValue(E_LOG_LEVEL_OPT);
		}
	}
	catch (const CmdLineParserException& e)
	{
		std::cerr << "Command line parser error." << std::endl;
		switch (e.getErrorCode())
		{
			case CmdLineParser::E_INVALID_OPTION:
				cerr << "unknown option: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_MISSING_ARGUMENT:
				cerr << "missing argument for option: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_INVALID_NON_OPTION_ARG:
				cerr << "invalid non-option argument: " << e.getMessage() << '\n';
			break;
			case CmdLineParser::E_MISSING_OPTION:
				cerr << "missing required option: " << e.getMessage() << '\n';
			break;
			default:
				cerr << "failed parsing command line options: " << e << "\n";
			break;
		}
		printUsage();
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
printUsage()
{
	cerr << "owoopcpp1pr [OPTIONS]..." << std::endl;
	cerr << CmdLineParser::getUsage(g_options) << std::endl;
}

} // end unnamed namespace

