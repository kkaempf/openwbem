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
#include "OW_MOFGrammar.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_MOFCIMOMVisitor.hpp"
#include "OW_Assertion.hpp"
#include "OW_MOFParserErrorHandlerIFC.hpp"
#include "OW_MOFCompiler.hpp"
#include "OW_GetPass.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_MOFCompCIMOMHandle.hpp"
#include "OW_CIMRepository.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include <iostream>
#ifdef OW_HAVE_GETOPT_H
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
			(void)details;
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
	virtual ParserAction doRecoverableError(const char *error, const lineInfo& li )
	{
		cerr << "Recoverable error in file: " << li.filename << " on line: " << li.lineNum << ": " << error << endl;
		return Ignore;
	}
	virtual void doProgressMessage( const char* message, const lineInfo& li )
	{
		cout << "File: " << li.filename << " Line: " << li.lineNum << ": " << message << endl;
	}
};
static bool use_cim_repository = false;
static String repository_dir;
static String url_arg = "http://localhost";
static String namespace_arg = "root/cimv2";
static String encoding_arg = "cimxml";
StringArray filelist;
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
	{ 0, 0, 0, 0 }
};
#endif
static const char* const short_options = "d:u:n:ce:sx:rpgwqI:ih";
//////////////////////////////////////////////////////////////////////////////
static int
processCommandLineOptions(int argc, char** argv)
{
	// handle backwards compatible options, which was <URL> <namespace> <file>
	// TODO: This is deprecated, remove it post 3.0
	if (argc == 4 && argv[1][0] != '-' && argv[2][0] != '-' && argv[3][0] != '-')
	{
		url_arg = argv[1];
		namespace_arg = argv[2];
		filelist.push_back(argv[3]);
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
	while(c != -1)
	{
		switch(c)
		{
			case 'd':
				use_cim_repository = true;
				repository_dir = optarg;
				break;
			case 'u':
				url_arg = optarg;
				break;
			case 'n':
				namespace_arg = optarg;
				break;
			case 'e':
				encoding_arg = optarg;
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
			filelist.push_back(argv[optind++]);
	}
	return 0;
}
class coutLogger : public Logger
{
	virtual void doLogMessage(const String &message, const LogLevel) const 
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
	virtual bool authenticate(String &, const String &, String &) 
	{
		return true;
	}
	virtual void addSelectable(SelectableIFCRef, SelectableCallbackIFCRef) 
	{
		OW_THROW(Exception, "Unsupported");
	}
	virtual void removeSelectable(SelectableIFCRef, SelectableCallbackIFCRef) 
	{
		OW_THROW(Exception, "Unsupported");
	}
	virtual RequestHandlerIFCRef getRequestHandler(const String &) 
	{
		OW_THROW(Exception, "Unsupported");
	}
	virtual CIMOMHandleIFCRef getCIMOMHandle(const String &, ESendIndicationsFlag, EBypassProvidersFlag) 
	{
		OW_THROW(Exception, "Unsupported");
	}
	virtual String getConfigItem(const String &, const String &defRetVal) const 
	{
		return defRetVal;
	}
};

void usage()
{
	cout << "Usage: owmofc [OPTION] <FILE>...\n";
	cout << "  -d,--direct <DIR>: create a repository in the specified directory without connecting to a cimom\n";
	cout << "  -u,--url <URL>: the url of the cimom. Default is " << url_arg << " if not specified\n";
	cout << "  -n,--namespace <NAMESPACE>: The initial namespace to use. Default is " << namespace_arg << " if not specified\n";
	cout << "  -c,--create-namespaces: If the namespace doesn't exist, create it <UNIMPLEMENTED>\n";
	cout << "  -e,--encoding <ENCODING>: Specify the encoding, valid values are cimxml and owbinary. Default is " << encoding_arg << " if not specified\n";
	cout << "  -s,--check-syntax: Only parse the mof, don't actually do anything <UNIMPLEMENTED>\n";
	cout << "  -x,--dump-xml <FILE>: Write the xml to FILE <UNIMPLEMENTED>\n";
	cout << "  -r,--remove: Instead of creating classes and instances, remove them <UNIMPLEMENTED>\n";
	cout << "  -p,--preserve: If a class or instance already exists, don't overwrite it with the one in the mof <UNIMPLEMENTED>\n";
	cout << "  -g,--upgrade: Overwrite a class only if it has a larger Version qualifier <UNIMPLEMENTED>\n";
	cout << "  -w,--suppress-warnings: Only print errors <UNIMPLEMENTED>\n";
	cout << "  -q,--quiet: Don't print anything <UNIMPLEMENTED>\n";
	cout << "  -I,--include <DIR>: Add a directory to the include search path <UNIMPLEMENTED>\n";
	cout << "  -i,--ignore-double-includes: If a mof file has already been included, don't parse it again <UNIMPLEMENTED>\n";
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
		Reference<ParserErrorHandlerIFC> theErrorHandler(new TheErrorHandler);
		Reference<CIMOMHandleIFC> handle;
		if (use_cim_repository)
		{
			ServiceEnvironmentIFCRef mofCompEnvironment(new MOFCompEnvironment());
			RepositoryIFCRef cimRepository = RepositoryIFCRef(new CIMRepository(mofCompEnvironment));
			cimRepository->open(repository_dir);
			try
			{
				cimRepository->createNameSpace(CIMNameSpaceUtils::prepareNamespace(namespace_arg), UserInfo(""));
			}
			catch (const CIMException& e)
			{
				// ignore the already exists error.
				if (e.getErrNo() != CIMException::ALREADY_EXISTS)
				{
					throw e;
				}
			}
			handle = CIMOMHandleIFCRef(new MOFCompCIMOMHandle(cimRepository, UserInfo("")));
		}
		else
		{
			URL url(url_arg);
			CIMProtocolIFCRef client;
			client = new HTTPClient(url_arg);
			// TODO: The /owbinary path part is deprecated, remove it post 3.0
			if(encoding_arg == "owbinary" || url.path.equalsIgnoreCase("/owbinary"))
			{
				handle = CIMOMHandleIFCRef(new BinaryCIMOMHandle(client));
			}
			else if (encoding_arg == "cimxml")
			{
				handle = CIMOMHandleIFCRef(new CIMXMLCIMOMHandle(client));
			}
			else
			{
				cerr << "Invalid encoding.  Valid encodings: cimxml, owbinary" << endl;
				return 1;
			}
			client->setLoginCallBack(ClientAuthCBIFCRef(new GetLoginInfo));
		}
		Compiler theCompiler(handle, namespace_arg, theErrorHandler);
		if (filelist.empty())
		{
			// don't do this, it's too confusing
			// filelist.push_back("-"); // if they didn't specify a file, read from stdin.
			usage();
			return 1;
		}
		for (size_t i = 0; i < filelist.size(); ++i)
		{
			errors += theCompiler.compile(filelist[i]);
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


