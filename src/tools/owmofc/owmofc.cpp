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
#include "OW_MOFGrammar.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_MOFCIMOMVisitor.hpp"
#include "OW_Assertion.hpp"
#include "OW_MOFParserErrorHandlerIFC.hpp"
#include "OW_MOFCompiler.hpp"
#include "OW_GetPass.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_MOFCompCIMOMHandle.hpp"

#ifdef OW_ENABLE_DB4_REPOSITORY
	#include "OW_CIMRepository2.hpp"
#else
	#include "OW_CIMRepository.hpp"
#endif

#include "OW_RequestHandlerIFC.hpp"
#include "OW_OperationContext.hpp"
#include "OW_URL.hpp"
#include "OW_Logger.hpp"
#include "OW_Reference.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_CerrLogger.hpp"

#include <iostream>

using namespace OpenWBEM;
using namespace OpenWBEM::MOF;

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

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
		cerr << "Fatal error in file: " << li.filename << " on line: " << li.lineNum << ": " << error << endl;
	}
	virtual EParserAction doRecoverableError(const char *error, const LineInfo& li )
	{
		cerr << "Recoverable error in file: " << li.filename << " on line: " << li.lineNum << ": " << error << endl;
		return E_IGNORE_ACTION;
	}
	virtual void doProgressMessage( const char* message, const LineInfo& li )
	{
		cout << "File: " << li.filename << " Line: " << li.lineNum << ": " << message << endl;
	}
};

// If these are changed, don't forget to change the corresponding command usage description.
const char* const def_url_arg = "http://localhost/root/cimv2";
const char* const def_namespace_arg = "root/cimv2";
const char* const def_encoding_arg = "cimxml";


Compiler::Options g_opts;
bool g_useCimRepository;
String g_repositoryDir;
String g_url;
String g_encoding;
StringArray g_filelist;

enum
{
	E_OPThelp,
	E_OPTversion,
	E_OPTdirect,
	E_OPTurl,
	E_OPTnamespace,
	E_OPTcreate_namespaces,
	E_OPTencoding,
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
	E_OPTinvalid
};

CmdLineParser::Option options[] =
{
	{ E_OPThelp, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{ E_OPTversion, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{ E_OPTdirect, 'd', "direct", CmdLineParser::E_REQUIRED_ARG, 0,
		"Create a repository in the specified directory without connecting to a cimom. "
		"Be extremely cautious using this option. Bypassing "
		"the cimom can have the following negative consequences:\n"
		"- Instances may incorrectly bypass providers.\n"
		"- The repository will be corrupted if the cimom is running simultaneously."},
	{ E_OPTurl, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0,
		"The url of the cimom. Default is http://localhost/root/cimv2 if not specified."},
	{ E_OPTremove_descriptions, 'm', "remove-descriptions", CmdLineParser::E_NO_ARG, 0,
		"Remove all the Description qualifiers to save space."},
	{ E_OPTcreate_namespaces, 'c', "create-namespaces", CmdLineParser::E_NO_ARG, 0,
		"If the namespace doesn't exist, create it."},
	{ E_OPTremove, 'r', "remove", CmdLineParser::E_NO_ARG, 0,
		"Instead of creating objects, remove them."},
	{ E_OPTnamespace, 'n', "namespace", CmdLineParser::E_REQUIRED_ARG, 0,
		"This option is deprecated (in 3.1.0) in favor of the URL namespace. The initial namespace "
		"to use. Default is root/cimv2 if not specified via this option or in the URL."},
	{ E_OPTencoding, 'e', "encoding", CmdLineParser::E_REQUIRED_ARG, 0,
		"This option is deprecated (in 3.1.0) in favor of the URL scheme. "
		"Specify the encoding, valid values are cimxml and owbinary. "
		"This can also be specified by using the owbinary.wbem URI scheme."},
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
	g_encoding = def_encoding_arg;

	// handle backwards compatible options, which was <URL> <namespace> <file>
	// This has to be done after setting the defaults.
	// TODO: This is deprecated in 3.0.0, remove it post 3.1
	if (argc == 4 && argv[1][0] != '-' && argv[2][0] != '-' && argv[3][0] != '-')
	{
		g_url = argv[1];
		g_opts.m_namespace = argv[2];
		g_filelist.push_back(argv[3]);
		cerr << "This cmd line usage is deprecated!\n";
		return true;
	}

	try
	{
		CmdLineParser parser(argc, argv, options);
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
		if (parser.isSet(E_OPTdirect))
		{
			g_useCimRepository = true;
			g_repositoryDir = parser.getOptionValue(E_OPTdirect);
		}
		if (parser.isSet(E_OPTurl))
		{
			g_url = parser.getOptionValue(E_OPTurl);
		}
		if (parser.isSet(E_OPTnamespace))
		{
			g_opts.m_namespace = parser.getOptionValue(E_OPTnamespace);
		}
		if (parser.isSet(E_OPTencoding))
		{
			g_encoding = parser.getOptionValue(E_OPTencoding);
		}
		if (parser.isSet(E_OPTcreate_namespaces))
		{
			g_opts.m_createNamespaces = true;
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
			default:
				cerr << "failed parsing command line options\n";
				break;
		}
		usage();
		return false;
	}

	// Set default namespace, this is a bit strange to maintain backward compat.
	// This was deprecated in 3.1.0, remove it later.
	if (g_opts.m_namespace.empty())
	{
		URL url(g_url);
		if (url.namespaceName.empty())
		{
			g_opts.m_namespace = def_namespace_arg;
		}
		else
		{
			g_opts.m_namespace = url.namespaceName;
		}
	}
	return true;
}

class MOFCompEnvironment : public ServiceEnvironmentIFC
{
public:
	virtual LoggerRef getLogger() const
	{
		return LoggerRef(new CerrLogger);
	}
	virtual LoggerRef getLogger(const String& componentName) const
	{
		return getLogger();
	}
	virtual void setConfigItem(const String &, const String &, EOverwritePreviousFlag)
	{
	}
	virtual bool authenticate(String &, const String &, String &, OperationContext& context)
	{
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

		ParserErrorHandlerIFCRef theErrorHandler(new TheErrorHandler);
		Reference<OperationContext> context;
		CIMOMHandleIFCRef handle;
		if (g_useCimRepository)
		{
			ServiceEnvironmentIFCRef mofCompEnvironment(new MOFCompEnvironment());
#ifdef OW_ENABLE_DB4_REPOSITORY
			RepositoryIFCRef cimRepository = RepositoryIFCRef(new CIMRepository2(mofCompEnvironment));
#else
			RepositoryIFCRef cimRepository = RepositoryIFCRef(new CIMRepository(mofCompEnvironment));
#endif
			cimRepository->open(g_repositoryDir);
			context = Reference<OperationContext>(new OperationContext);
			handle = CIMOMHandleIFCRef(new MOFCompCIMOMHandle(cimRepository, *context));
		}
		else
		{
			URL url(g_url);
			// override what was in the url - TODO: deprecated in 3.0.0. Remove this post 3.1.x
			if (g_encoding == URL::OWBINARY)
			{
				if (url.scheme.endsWith('s'))
				{
					url.scheme == URL::OWBINARY_WBEMS;
				}
				else
				{
					url.scheme == URL::OWBINARY_WBEM;
				}
			}
			else if (g_encoding == "cimxml")
			{
				; // don't do anything
			}
			else
			{
				cerr << "Invalid encoding.  Valid encodings: cimxml, " << URL::OWBINARY << endl;
				return 1;
			}
			ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
			handle = ClientCIMOMHandle::createFromURL(url.toString(), getLoginInfo);
		}
		Compiler theCompiler(handle, g_opts, theErrorHandler);
		for (size_t i = 0; i < g_filelist.size(); ++i)
		{
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


