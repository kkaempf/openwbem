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
#include "OW_CIMRepository.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_OperationContext.hpp"
#include "OW_URL.hpp"
#include "OW_Reference.hpp"

#include <iostream>
#if defined(OW_HAVE_GETOPT_H) && !defined(OW_GETOPT_AND_UNISTD_CONFLICT)
#include <getopt.h>
#else
#include <stdlib.h> // for getopt on Solaris
#include <unistd.h> // for getopt on Linux
#endif

using namespace OpenWBEM;
using namespace OpenWBEM::MOF;

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
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
	TheErrorHandler() : ParserErrorHandlerIFC() {}
	virtual ~TheErrorHandler(){}
protected:
	virtual void doFatalError(const char *error, const lineInfo& li )
	{
		cerr << "Fatal error in file: " << li.filename << " on line: " << li.lineNum << ": " << error << endl;
	}
	virtual EParserAction doRecoverableError(const char *error, const lineInfo& li )
	{
		cerr << "Recoverable error in file: " << li.filename << " on line: " << li.lineNum << ": " << error << endl;
		return E_IGNORE_ACTION;
	}
	virtual void doProgressMessage( const char* message, const lineInfo& li )
	{
		cout << "File: " << li.filename << " Line: " << li.lineNum << ": " << message << endl;
	}
};

static const char* const def_url_arg = "http://localhost/root/cimv2";
static const char* const def_namespace_arg = "root/cimv2";
static const char* const def_encoding_arg = "cimxml";


static Compiler::Options g_opts;
static bool g_useCimRepository;
static String g_repositoryDir;
static String g_url;
static String g_encoding;
static StringArray g_filelist;


// TODO: switch all this junk to use the new CmdLineParser class.
#ifdef OW_HAVE_GETOPT_LONG
//////////////////////////////////////////////////////////////////////////////
static struct option   long_options[] =
{
	{ "direct", required_argument, NULL, 'd' },
	{ "url", required_argument, NULL, 'u' },
	{ "namespace", required_argument, NULL, 'n' },
	{ "create-namespaces", 0, NULL, 'c' },
	{ "encoding", required_argument, NULL, 'e' },
	{ "check-syntax", 0, NULL, 's' },
	{ "dump-xml", required_argument, NULL, 'x' },
	{ "remove", 0, NULL, 'r' },
	{ "preserve", 0, NULL, 'p' },
	{ "upgrade", 0, NULL, 'g' },
	{ "suppress-warnings", 0, NULL, 'w' },
	{ "quiet", 0, NULL, 'q' },
	{ "include", required_argument, NULL, 'I' },
	{ "ignore-double-includes", 0, NULL, 'i' },
	{ "help", 0, NULL, 'h' },
	{ "remove-descriptions", 0, NULL, 'm' },
	{ 0, 0, 0, 0 }
};
#endif
static const char* const short_options = "d:u:n:ce:sx:rpgwqI:ihm";
//////////////////////////////////////////////////////////////////////////////
static int
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
		return 0;
	}

#ifdef OW_HAVE_GETOPT_LONG
	int optndx = 0;
	optind = 1;
	int c = getopt_long(argc, argv, short_options, long_options, &optndx);
#else
	optind = 1;
	int c = getopt(argc, argv, short_options);
#endif
	while (c != -1)
	{
		switch (c)
		{
			case 'd':
				g_useCimRepository = true;
				g_repositoryDir = optarg;
				break;
			case 'u':
				g_url = optarg;
				break;
			case 'n':
				g_opts.m_namespace = optarg;
				break;
			case 'e':
				g_encoding = optarg;
				break;
			case 'c':
				g_opts.m_createNamespaces = true;
				break;
			case 'm':
				g_opts.m_removeDescriptions = true;
				break;
			default:
				return -1;
		}
#ifdef OW_HAVE_GETOPT_LONG
		c = getopt_long(argc, argv, short_options, long_options, &optndx);
#else
		c = getopt(argc, argv, short_options);
#endif
	}
	if (optind < argc)
	{
		while (optind < argc)
			g_filelist.push_back(argv[optind++]);
	}

	// Set default namespace, this is a bit strange to maintain backward compat.
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

	return 0;
}
class coutLogger : public Logger
{
	virtual void doLogMessage(const String &message, const ELogLevel) const
	{
		cout << message << endl;
	}
};
class MOFCompEnvironment : public ServiceEnvironmentIFC
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

void usage()
{
	cout << "Usage: owmofc [OPTION] <FILE>...\n";
	cout << "  -d,--direct <DIR>: create a repository in the specified directory without connecting to a cimom\n";
	cout << "                     Be extremely cautious using this option. Bypassing the cimom can have the following negative consequences:\n";
	cout << "                     - Instances may incorrectly bypass providers.\n";
	cout << "                     - The repository will be corrupted if the cimom is running simultaneously.\n";
	cout << "  -u,--url <URL>: the url of the cimom. Default is " << def_url_arg << " if not specified\n";
	cout << "  -n,--namespace <NAMESPACE>: This option is deprecated (in 3.1.0) in favor of the URL namespace. The initial namespace to use. Default is " << def_namespace_arg << " if not specified via this option or in the URL\n";
	cout << "  -c,--create-namespaces: If the namespace doesn't exist, create it\n";
	cout << "  -e,--encoding <ENCODING>: This option is deprecated (in 3.1.0) in favor of the URL scheme. Specify the encoding, valid values are cimxml and owbinary. "
		"This can also be specified by using the owbinary.wbem URI scheme.\n";
	cout << "  -s,--check-syntax: Only parse the mof, don't actually do anything <UNIMPLEMENTED>\n";
	cout << "  -x,--dump-xml <FILE>: Write the xml to FILE <UNIMPLEMENTED>\n";
	cout << "  -r,--remove: Instead of creating classes and instances, remove them <UNIMPLEMENTED>\n";
	cout << "  -p,--preserve: If a class or instance already exists, don't overwrite it with the one in the mof <UNIMPLEMENTED>\n";
	cout << "  -g,--upgrade: Overwrite a class only if it has a larger Version qualifier <UNIMPLEMENTED>\n";
	cout << "  -w,--suppress-warnings: Only print errors <UNIMPLEMENTED>\n";
	cout << "  -q,--quiet: Don't print anything <UNIMPLEMENTED>\n";
	cout << "  -I,--include <DIR>: Add a directory to the include search path <UNIMPLEMENTED>\n";
	cout << "  -i,--ignore-double-includes: If a mof file has already been included, don't parse it again <UNIMPLEMENTED>\n";
	cout << "  -m,--remove-descriptions: Remove all the Description qualifiers to save space.\n";
	cout << "  -h,--help: Print this help message\n";
}

int main(int argc, char** argv)
{
	long errors = 0;
	try
	{
		if (processCommandLineOptions(argc, argv) == -1)
		{
			usage();
			return 1;
		}
		ParserErrorHandlerIFCRef theErrorHandler(new TheErrorHandler);
		Reference<OperationContext> context;
		CIMOMHandleIFCRef handle;
		if (g_useCimRepository)
		{
			ServiceEnvironmentIFCRef mofCompEnvironment(new MOFCompEnvironment());
			RepositoryIFCRef cimRepository = RepositoryIFCRef(new CIMRepository(mofCompEnvironment));
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
		if (g_filelist.empty())
		{
			usage();
			return 1;
		}
		for (size_t i = 0; i < g_filelist.size(); ++i)
		{
			errors += theCompiler.compile(g_filelist[i]);
		}
		cout
			<< "Compilation finished.  "
			<< errors
			<< " errors occurred." << endl;
	}
	catch(Exception& e)
	{
		cerr << "Caught Exception: " << e.type() << endl;
		cerr << "File: " << e.getFile() << endl;
		cerr << "Line: " << e.getLine() << endl;
		cerr << "Msg: " << e.getMessage() << endl;
		++errors;
	}
	catch(...)
	{
		cerr << "Caught unknown exception" << endl;
		++errors;
	}
	return errors;
}


