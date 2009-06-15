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
#include "OW_LocalOperationContext.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/CmdLineParser.hpp"
#include "OW_Logger.hpp"
#include "blocxx/String.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "blocxx/CerrAppender.hpp"
#include "OW_ToolsCommon.hpp"
#include "OW_ResultHandlers.hpp"
#include "OW_RepositoryCIMOMHandle.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMInstance.hpp"

#include <iostream>
#include <fstream>
#include <cerrno>

using std::cout;
using std::cerr;
using std::endl;

using namespace OpenWBEM;
using namespace OpenWBEM::Tools;
using namespace WBEMFlags;

namespace
{

class TheServiceEnvironment : public ServiceEnvironmentIFC
{
public:
	TheServiceEnvironment(const String& repositoryDir)
		: m_repositoryDir(repositoryDir)
	{

	}
	virtual String getConfigItem(const String& item, const String& defRetVal) const
	{
		if (item == ConfigOpts::DATADIR_opt)
		{
			return m_repositoryDir;
		}
		else
		{
			return defRetVal;
		}
	}

private:
	String m_repositoryDir;
};

enum
{
	HELP_OPT,
	VERSION_OPT,
	REPOSITORY_DIR_OPT,
	OUTPUT_OPT,
	NAMESPACE_SELECT_OPT
};

CmdLineParser::Option g_options[] =
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{REPOSITORY_DIR_OPT, 'd', "repository-dir", CmdLineParser::E_REQUIRED_ARG, 0, "Sets the repository dir. Default is "OW_DEFAULT_DATADIR },
	{NAMESPACE_SELECT_OPT, 'n', "namespace", CmdLineParser::E_REQUIRED_ARG, 0, "Selects a single namespace.  The default is to use every namespace." },
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

class ClassPrinter : public CIMClassResultHandlerIFC
{
	virtual void doHandle(const CIMClass &c)
	{
		g_output << c.toMOF() << "\n";
	}
};

class QualifierTypePrinter : public CIMQualifierTypeResultHandlerIFC
{
	virtual void doHandle(const CIMQualifierType& qt)
	{
		g_output << qt.toMOF() << "\n";
	}
};


class InstancePrinter : public CIMInstanceResultHandlerIFC
{
	virtual void doHandle(const CIMInstance& i)
	{
		g_output << i.toMOF() << "\n";
	}
};

void dumpNamespace(const String& ns, const RepositoryCIMOMHandleRef& hdl)
{
	g_output << "#pragma namespace(\"" << ns << "\")\n";

	// qualifier types
	QualifierTypePrinter qtPrinter;
	hdl->enumQualifierTypes(ns, qtPrinter);

	// classes
	ClassPrinter classPrinter;
	hdl->enumClass(ns, "", classPrinter,
		E_DEEP,
		E_LOCAL_ONLY,
		E_INCLUDE_QUALIFIERS,
		E_INCLUDE_CLASS_ORIGIN);

	// instances
	StringArray classNames;
	StringArrayBuilder classNamesBuilder(classNames);
	hdl->enumClassNames(ns, "", classNamesBuilder, E_DEEP);

	InstancePrinter instancePrinter;
	for (size_t clsNameIdx = 0; clsNameIdx < classNames.size(); ++clsNameIdx)
	{
		const String& curClsName(classNames[clsNameIdx]);
		hdl->enumInstances(ns, curClsName, instancePrinter,
			E_DEEP,
			E_NOT_LOCAL_ONLY,
			E_EXCLUDE_QUALIFIERS,
			E_EXCLUDE_CLASS_ORIGIN);
	}
}


void dumpRepository(const RepositoryCIMOMHandleRef& hdl)
{
	StringArray namespaces;
	StringArrayBuilder stringArrayBuilder(namespaces);
	hdl->enumNameSpace(stringArrayBuilder);
	for (size_t curNamespace = 0; curNamespace < namespaces.size(); ++curNamespace)
	{
		const String ns(namespaces[curNamespace]);

		dumpNamespace(ns, hdl);
	}
}

} // end unnamed namespace

int main(int argc, char** argv)
{
	try
	{
		
		CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);
	
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
	
		String repositoryDir = parser.getOptionValue(REPOSITORY_DIR_OPT, OW_DEFAULT_DATADIR);
		if (!FileSystem::exists(repositoryDir))
		{
			cerr << "Specified repository dir doesn't exist\n";
			return 1;
		}
		if (!FileSystem::isDirectory(repositoryDir))
		{
			cerr << "Specified repository dir isn't a directory\n";
			return 1;
		}
		if (!FileSystem::exists(repositoryDir + OW_FILENAME_SEPARATOR + "schema.dat") || 
			!FileSystem::exists(repositoryDir + OW_FILENAME_SEPARATOR + "instances.dat"))
		{
			cerr << "Specified repository dir doesn't contain an OpenWBEM CIM repository\n";
			return 1;
		}
	
		std::filebuf* fb = 0;
		FbCleanuper fbCleanuper(fb);
		// g_output is default initialized to use cout
		if (parser.isSet(OUTPUT_OPT))
		{
			String output = parser.getOptionValue(OUTPUT_OPT);
			fb = new std::filebuf;
			if (!fb->open(output.c_str(), std::ios_base::out|std::ios_base::trunc))
			{
				cerr << "Unable to open: " << output << ": " << strerror(errno) << endl;
				return 1;
			}
			g_output.rdbuf(fb);
		}
	
		LogAppender::setDefaultLogAppender(new CerrAppender());

		LocalOperationContext context;
		RepositoryIFCRef cimRepository = new CIMRepository;
		cimRepository->init(new TheServiceEnvironment(repositoryDir));
		RepositoryCIMOMHandleRef repositoryCIMOMHandle(new RepositoryCIMOMHandle(cimRepository, context));

		if( parser.isSet(NAMESPACE_SELECT_OPT) )
		{
			dumpNamespace(parser.getOptionValue(NAMESPACE_SELECT_OPT), repositoryCIMOMHandle);
		}
		else
		{
			dumpRepository(repositoryCIMOMHandle);
		}

		return 0;
	}
	catch (CmdLineParserException& e)
	{
		printCmdLineParserExceptionMessage(e);
		usage();
	}
	catch(const CIMException& e)
	{
		cerr << CIMException::getCodeName(e.getErrNo()) << ':' << e.getMessage() << endl;
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

