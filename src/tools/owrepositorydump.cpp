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
 * @author Dan Nuffer
 */
#include "OW_config.h"
#include "OW_CIMRepository.hpp"
#include "OW_OperationContext.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_Logger.hpp"
#include "OW_String.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RequestHandlerIFC.hpp"

#include <iostream>
#include <fstream>
#include <cerrno>

using std::cout;
using std::cerr;
using std::endl;

using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{

class coutLogger : public Logger
{
	virtual void doLogMessage(const String &message, const ELogLevel) const
	{
		cout << message << endl;
	}
};

class TheServiceEnvironment : public ServiceEnvironmentIFC
{
public:
	virtual LoggerRef getLogger() const
	{
		return LoggerRef(new coutLogger);
	}
	virtual void setConfigItem(const String &, const String &, EOverwritePreviousFlag)
	{
	}
	virtual bool authenticate(String &, const String &, String &, OperationContext& context)
	{
		(void) context;
		return true;
	}
	virtual void addSelectable(const SelectableIFCRef& , const SelectableCallbackIFCRef&)
	{
		OW_ASSERT("Unsupported" == 0);
	}
	virtual void removeSelectable(const SelectableIFCRef&)
	{
		OW_ASSERT("Unsupported" == 0);
	}
	virtual RequestHandlerIFCRef getRequestHandler(const String &)
	{
		OW_ASSERT("Unsupported" == 0);
		return RequestHandlerIFCRef();
	}
	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext&, ESendIndicationsFlag, EBypassProvidersFlag)
	{
		OW_ASSERT("Unsupported" == 0);
		return CIMOMHandleIFCRef();
	}
	virtual String getConfigItem(const String &, const String &defRetVal) const
	{
		return defRetVal;
	}
	virtual CIMInstanceArray getInteropInstances(const String& className) const
	{
		return CIMInstanceArray();
	}
	virtual void setInteropInstance(const CIMInstance& inst)
	{
	}
};

enum
{
	HELP_OPT,
	VERSION_OPT,
	REPOSITORY_DIR_OPT,
	OUTPUT_OPT
};

CmdLineParser::Option g_options[] = 
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{REPOSITORY_DIR_OPT, 'd', "repository-dir", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the repository dir. Default is "OW_DEFAULT_DATA_DIR },
	{OUTPUT_OPT, 'o', "output", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the output file. Default is stdout" },
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void usage()
{
	cerr << "Usage: owrepositorydump [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << '\n';
	cerr << "\nowrepositorydump is used to convert the contents of an owcimomd repository into MOF.\n";
	cerr << "The output is suitable for compiling with owmofc.\n";
	cerr << "It can be used to:\n";
	cerr << " - View the contents of a repository.\n";
	cerr << " - Transfer a repository.\n";
	cerr << " - Upgrade a repository when the format is changed.\n";
	cerr << endl;
}

std::ostream g_output(cout.rdbuf());

struct FbCleanuper
{
	FbCleanuper(std::filebuf*& fb) : m_fb(fb) {}
	~FbCleanuper()
	{
		if (m_fb)
		{
			m_fb->close();
			delete m_fb;
			m_fb = 0;
		}
	}
	std::filebuf*& m_fb;
};

} // end unnamed namespace

int main(int argc, char** argv)
{
	try
	{
		
		CmdLineParser parser(argc, argv, g_options);
	
		if (parser.isSet(HELP_OPT))
		{
			usage();
			return 0;
		}
		else if (parser.isSet(VERSION_OPT))
		{
			cout << "owrepositorydump (OpenWBEM) " << OW_VERSION << '\n';
			cout << "Written by Dan Nuffer.\n";
			return 0;
		}
	
		String repositoryDir;
		if (parser.isSet(REPOSITORY_DIR_OPT))
		{
			repositoryDir = parser.getOptionValue(REPOSITORY_DIR_OPT);
		}
		else
		{
			repositoryDir = OW_DEFAULT_DATA_DIR;
		}
	
		std::filebuf* fb = 0;
		FbCleanuper fbCleanuper(fb);
		String output = parser.getOptionValue(OUTPUT_OPT);
		if (output.empty())
		{
			// g_output is default initialized to use cout
		}
		else
		{
			fb = new std::filebuf;
			if (!fb->open(output.c_str(), std::ios_base::out|std::ios_base::trunc))
			{
				cerr << "Unable to open: " << output << ": " << strerror(errno) << endl;
				return 1;
			}
			g_output.rdbuf(fb);
		}
		g_output << "Hello\n";
	
		OperationContext context;
		RepositoryIFCRef cimRepository = RepositoryIFCRef(new CIMRepository(ServiceEnvironmentIFCRef(new TheServiceEnvironment)));
		cimRepository->open(repositoryDir);
	
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
		usage();
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

