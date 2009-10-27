/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_MOFGrammar.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_MOFCIMOMVisitor.hpp"
#include "OW_MOFParserErrorHandlerIFC.hpp"
#include "OW_MOFCompiler.hpp"
#include "blocxx/GetPass.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_MOFCompCIMOMHandle.hpp"
#include "blocxx/Format.hpp"

#ifdef OW_ENABLE_DB4_REPOSITORY
	#include "OW_CIMRepository2.hpp"
#else
	#include "OW_CIMRepository.hpp"
#endif

#include "OW_RequestHandlerIFC.hpp"
#include "OW_LocalOperationContext.hpp"
#include "OW_URL.hpp"
#include "blocxx/Logger.hpp"
#include "blocxx/Reference.hpp"
#include "blocxx/CmdLineParser.hpp"
#include "blocxx/CerrLogger.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_CIMProtocolIFC.hpp"

#include <iostream>

using namespace OpenWBEM;
using namespace OpenWBEM::MOF;
using namespace blocxx;

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

Compiler::Options g_opts;
bool g_enableDebug;
bool g_useCimRepository;
String g_repositoryDir;
String g_url;
StringArray g_filelist;

#define LOG_DEBUG(message) \
do \
{ \
	if (g_enableDebug) \
	{ \
		cerr << message << endl; \
	} \
} while (0)

namespace
{

//////////////////////////////////////////////////////////////////////////////
class GetLoginInfo : public ClientAuthCBIFC
{
public:
	bool getCredentials(const String& realm, String& name,
						String& passwd, const String& details)
	{
		cout << "Authentication required for " << realm << endl;
		cout << "Enter the user name: ";
		name = String::getLine(cin);
		passwd = GetPass::getPass("Enter the password for " +
								  name + ": ");
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////////
class TheErrorHandler: public ParserErrorHandlerIFC
{
public:
	TheErrorHandler() : ParserErrorHandlerIFC()
	{
	}
	virtual ~TheErrorHandler()
	{
	}
protected:
	virtual void doFatalError(const char *error, const LineInfo& li )
	{
		cerr << "Fatal error in file: " << li.filename << " on line: " << li.lineNum << ':' << li.columnNum << ": " << error << endl;
	}
	virtual EParserAction doRecoverableError(const char *error, const LineInfo& li )
	{
		cerr << "Recoverable error in file: " << li.filename << " on line: " << li.lineNum << ':' << li.columnNum << ": " << error << endl;
		return E_IGNORE_ACTION;
	}
	virtual void doProgressMessage( const char* message, const LineInfo& li )
	{
		cout << "File: " << li.filename << " Line: " << li.lineNum << ':' << li.columnNum << ": " << message << endl;
	}
};

// If these are changed, don't forget to change the corresponding command usage description.
const char* const def_url_arg = "http://localhost/root/cimv2";
const char* const def_namespace_arg = "root/cimv2";

enum
{
	E_OPThelp,
	E_OPTversion,
	E_OPTdebug,
	E_OPTdirect,
	E_OPTurl,
	E_OPTnamespace,
	E_OPTcreate_namespaces,
	E_OPTcheck_syntax,
	E_OPTdump_xml,
	E_OPTremove,
	E_OPTpreserve,
	E_OPTupgrade,
	E_OPTsuppress_warnings,
	E_OPTquiet,
	E_OPTinclude,
	E_OPTignore_double_includes,
	E_OPTremove_descriptions,
	E_OPTinvalid,
	E_OPTcreate_deps
};

CmdLineParser::Option options[] =
{
	{ E_OPThelp, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{ E_OPTversion, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{ E_OPTdebug, '\0', "debug", CmdLineParser::E_NO_ARG, "0", "Enable debug output"},
	{ E_OPTdirect, 'd', "direct", CmdLineParser::E_REQUIRED_ARG, 0,
		"Create a repository in the specified directory without connecting to a cimom. "
		"Bypassing the cimom may incorrectly bypass providers and create instances which should not exist in the repository."},
	{ E_OPTurl, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0,
		"The url identifying the cimom and namespace. Default is http://localhost/root/cimv2 if not specified."},
	{ E_OPTremove_descriptions, 'm', "remove-descriptions", CmdLineParser::E_NO_ARG, 0,
		"Remove all the Description qualifiers to save space."},
	{ E_OPTcreate_namespaces, 'c', "create-namespaces", CmdLineParser::E_NO_ARG, 0,
		"If the namespace doesn't exist, create it."},
	{ E_OPTcreate_deps, 's', "create-deps", CmdLineParser::E_REQUIRED_ARG, 0,
		"If a superclass or reference end point does not exist, look for a file called <classname>.mof "
		"under the specified directory, compile this MOF, and try again to create the class"},
	{ E_OPTremove, 'r', "remove", CmdLineParser::E_NO_ARG, 0,
		"Instead of creating objects, remove them."},
	{ E_OPTnamespace, 'n', "namespace", CmdLineParser::E_REQUIRED_ARG, 0,
		"This option is overridden by the URL namespace. The initial namespace "
		"to use. Default is root/cimv2 if not specified via this option or in the URL."},
#if 0
	{ E_OPTcheck_syntax, 's', "check-syntax", CmdLineParser::E_NO_ARG, 0,
		"Only parse the mof, don't actually do anything. <UNIMPLEMENTED>"},
	{ E_OPTdump_xml, 'x', "dump-xml", CmdLineParser::E_REQUIRED_ARG, 0,
		"Write the xml to the specified file. <UNIMPLEMENTED>"},
	{ E_OPTpreserve, 'p', "preserve", CmdLineParser::E_NO_ARG, 0,
		"If a class or instance already exists, don't overwrite it with the one in the mof. <UNIMPLEMENTED>"},
	{ E_OPTupgrade, 'g', "upgrade", CmdLineParser::E_NO_ARG, 0,
		"Overwrite a class only if it has a larger Version qualifier. <UNIMPLEMENTED>"},
	{ E_OPTsuppress_warnings, 'w', "suppress-warnings", CmdLineParser::E_NO_ARG, 0,
		"Only print errors. <UNIMPLEMENTED>"},
	{ E_OPTquiet, 'q', "quiet", CmdLineParser::E_NO_ARG, 0,
		"Don't print anything. <UNIMPLEMENTED>"},
	{ E_OPTinclude, 'I', "include", CmdLineParser::E_REQUIRED_ARG, 0,
		"Add the specifed directory to the include search path. <UNIMPLEMENTED>"},
	{ E_OPTignore_double_includes, 'i', "ignore-double-includes", CmdLineParser::E_NO_ARG, 0,
		"If a mof file has already been included, don't parse it again. <UNIMPLEMENTED>"},
	{ E_OPTignore_includes, '\0', "ignore-includes", CmdLineParser::E_NO_ARG, 0,
		"Only process files specified on the command line. <UNIMPLEMENTED>"},
#endif
	{ 0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void usage()
{
	cout << "Usage: owmofc [OPTION] <FILE>...\n";
	cout << CmdLineParser::getUsage(options) << endl;
}

/// returns true on success, false on error
bool
processCommandLineOptions(int argc, char** argv)
{
	// Set defaults
	g_url = def_url_arg;

	try
	{
		CmdLineParser parser(argc, argv, options, CmdLineParser::E_NON_OPTION_ARGS_ALLOWED);
		if (parser.isSet(E_OPThelp))
		{
			usage();
			return false;
		}
		if (parser.isSet(E_OPTversion))
		{
			cout << "owmofc (OpenWBEM) " << OW_VERSION << '\n';
			cout << "Written by Dan Nuffer.\n";
			return 0;
		}
		if (parser.isSet(E_OPTdebug))
		{
			g_enableDebug = true;
		}
		if (parser.isSet(E_OPTdirect))
		{
			g_useCimRepository = true;
			g_repositoryDir = parser.getOptionValue(E_OPTdirect);
		}
		if (parser.isSet(E_OPTurl))
		{
			g_url = parser.getOptionValue(E_OPTurl, def_url_arg);
		}
		if (parser.isSet(E_OPTnamespace))
		{
			g_opts.m_namespace = parser.getOptionValue(E_OPTnamespace);
		}
		if (parser.isSet(E_OPTcreate_namespaces))
		{
			g_opts.m_createNamespaces = true;
		}
		if (parser.isSet(E_OPTcreate_deps))
		{
			g_opts.m_depSearchDir = parser.getOptionValue(E_OPTcreate_deps);
		}
		if (parser.isSet(E_OPTremove_descriptions))
		{
			g_opts.m_removeDescriptions = true;
		}
		if (parser.isSet(E_OPTremove))
		{
			g_opts.m_removeObjects = true;
		}

		g_filelist.appendArray(parser.getNonOptionArgs());
		if (g_filelist.empty())
		{
			usage();
			return false;
		}
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
		usage();
		return false;
	}

	return true;
}

class MOFCompEnvironment : public ServiceEnvironmentIFC
{
public:
	virtual String getConfigItem(const String& name, const String& defRetVal) const
	{
		// CIMRepository may query for this.
		if (name == ConfigOpts::DATADIR_opt)
		{
			return g_repositoryDir;
		}
		return defRetVal;
	}

};

} // end unnamed namespace

int main(int argc, char** argv)
{
	long errors = 0;
	try
	{
		if (!processCommandLineOptions(argc, argv))
		{
			return 1;
		}

		if (g_enableDebug)
		{
			if (g_useCimRepository)
			{
				LOG_DEBUG(Format("Using direct repository access to repository at %1", g_repositoryDir));
			}
			else
			{
				LOG_DEBUG(Format("Connecting to CIMOM with url: %1", g_url));
			}

			LOG_DEBUG("Supplied Filenames:");
			for (size_t i = 0; i < g_filelist.size(); ++i)
			{
				LOG_DEBUG(Format("\t%1", g_filelist[i] ));
			}

			LOG_DEBUG("Compiler Options:");
			LOG_DEBUG(Format("\tNamespace: %1", g_opts.m_namespace ));
			LOG_DEBUG(Format("\tCreate Namespaces: %1", g_opts.m_createNamespaces ));
			LOG_DEBUG(Format("\tCheck Syntax Only: %1", g_opts.m_checkSyntaxOnly ));
			LOG_DEBUG(Format("\tDump XML File Name: %1", g_opts.m_dumpXmlFile ));
			LOG_DEBUG(Format("\tRemove: %1", g_opts.m_remove ));
			LOG_DEBUG(Format("\tPreserve: %1", g_opts.m_preserve ));
			LOG_DEBUG(Format("\tUpgrade: %1", g_opts.m_upgrade ));
			LOG_DEBUG(Format("\tRemove Descriptions: %1", g_opts.m_removeDescriptions ));
			LOG_DEBUG(Format("\tRemove Objects: %1", g_opts.m_removeObjects ));
			LOG_DEBUG(Format("\tDependency Search Dir: %1", g_opts.m_depSearchDir ));
			LOG_DEBUG(Format("\tIgnore Double Includes: %1", g_opts.m_ignoreDoubleIncludes ));
			LOG_DEBUG("\tInclude Directories:");
			for (size_t i = 0; i < g_opts.m_includeDirs.size(); ++i)
			{
				LOG_DEBUG(Format("\t%1", g_opts.m_includeDirs[i] ));
			}
		}

		ParserErrorHandlerIFCRef theErrorHandler(new TheErrorHandler);
		Reference<OperationContext> context;
		CIMOMHandleIFCRef handle;
		if (g_useCimRepository)
		{
			LOG_DEBUG("Creating MOF Comp Environment");
			ServiceEnvironmentIFCRef mofCompEnvironment(new MOFCompEnvironment());

			LOG_DEBUG("Creating CIM Repository Handle");
#ifdef OW_ENABLE_DB4_REPOSITORY
			RepositoryIFCRef cimRepository = RepositoryIFCRef(new CIMRepository2);
#else
			RepositoryIFCRef cimRepository = RepositoryIFCRef(new CIMRepository);
#endif
			LOG_DEBUG("Initializing Repository Handle");
			cimRepository->init(mofCompEnvironment);
			LOG_DEBUG("Creating Operation Context");
			context = Reference<OperationContext>(new LocalOperationContext);
			LOG_DEBUG("Creating CIMOM Handle");
			handle = CIMOMHandleIFCRef(new MOFCompCIMOMHandle(cimRepository, *context));
		}
		else
		{
			URL url(g_url);
			LOG_DEBUG("Getting Login Info");
			ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
			LOG_DEBUG("Creating CIMOM Handle");
			ClientCIMOMHandleRef clientHandle = ClientCIMOMHandle::createFromURL(url.toString(), getLoginInfo);
			LOG_DEBUG("Setting WBEM Protocol Timeout to Infinity");
			clientHandle->getWBEMProtocolHandler()->setTimeouts(Timeout::infinite);
			handle = clientHandle;
		}
		LOG_DEBUG("Creating MOF Compiler");
		Compiler theCompiler(handle, g_opts, theErrorHandler);
		for (size_t i = 0; i < g_filelist.size(); ++i)
		{
			LOG_DEBUG("Compiling file: " << g_filelist[i]);
			errors += theCompiler.compile(g_filelist[i]);
		}
		cout
		<< "Compilation finished.  "
		<< errors
		<< " errors occurred." << endl;
	}
	catch (Exception& e)
	{
		cerr << "Caught Exception: " << e.type() << endl;
		cerr << "File: " << e.getFile() << endl;
		cerr << "Line: " << e.getLine() << endl;
		cerr << "Msg: " << e.getMessage() << endl;
		++errors;
	}
	catch (...)
	{
		cerr << "Caught unknown exception" << endl;
		++errors;
	}
	return errors;
}


