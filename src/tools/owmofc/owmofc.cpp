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
#include "Grammar.h"
#include "OW_HTTPClient.hpp"
#include "OW_HTTPUtils.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "CIMOMVisitor.hpp"
#include "OW_Assertion.hpp"
#include "OW_MofParserErrorHandlerIFC.hpp"
#include "MofCompiler.hpp"
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

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

class GetLoginInfo : public OW_ClientAuthCBIFC
{
	public:
		OW_Bool getCredentials(const OW_String& realm, OW_String& name,
				OW_String& passwd, const OW_String& details)
		{
			(void)details;
			cout << "Authentication required for " << realm << endl;
			cout << "Enter the user name: ";
			name = OW_String::getLine(cin);
			passwd = OW_GetPass::getPass("Enter the password for " +
				name + ": ");
			return OW_Bool(true);
		}
};

//////////////////////////////////////////////////////////////////////////////
class TheErrorHandler: public OW_MofParserErrorHandlerIFC
{
public:
	TheErrorHandler() : OW_MofParserErrorHandlerIFC() {}
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
static OW_String repository_dir;
static OW_String url_arg = "http://localhost";
static OW_String namespace_arg = "root/cimv2";
static OW_String encoding_arg = "cimxml";
OW_StringArray filelist;

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
	{ "suppress-warnings", 0, NULL, 'w' },
	{ "quiet", 0, NULL, 'q' },
	{ "include", required_argument, NULL, 'I' },
	{ "ignore-double-includes", 0, NULL, 'i' },
	{ "use-openwbem-binary", 0, NULL, 'b' },
    { "help", 0, NULL, 'h' },
    { 0, 0, 0, 0 }
};
#endif

static const char* const short_options = "d:u:n:ce:sx:rpwqI:ih";

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

class coutLogger : public OW_Logger
{
	virtual void doLogMessage(const OW_String &message, const OW_LogLevel) const 
	{
		cout << message << endl;
	}
};

class OW_MOFCompEnvironment : public OW_ServiceEnvironmentIFC
{
public:
	virtual OW_LoggerRef getLogger() const 
	{
		return OW_LoggerRef(new coutLogger);
	}
	virtual void setConfigItem(const OW_String &, const OW_String &, OW_Bool) 
	{
	}
	virtual OW_Bool authenticate(OW_String &, const OW_String &, OW_String &) 
	{
		return true;
	}
	virtual void addSelectable(OW_SelectableIFCRef, OW_SelectableCallbackIFCRef) 
	{
		OW_THROW(OW_Exception, "Unsupported");
	}
	virtual void removeSelectable(OW_SelectableIFCRef, OW_SelectableCallbackIFCRef) 
	{
		OW_THROW(OW_Exception, "Unsupported");
	}
	virtual OW_RequestHandlerIFCRef getRequestHandler(const OW_String &) 
	{
		OW_THROW(OW_Exception, "Unsupported");
	}
	virtual OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_String &, const OW_Bool, const OW_Bool) 
	{
		OW_THROW(OW_Exception, "Unsupported");
	}
	virtual OW_String getConfigItem(const OW_String &, const OW_String &defRetVal) const 
	{
		return defRetVal;
	}
};

int main(int argc, char** argv)
{
	long errors = 0;
	try
	{
		if (processCommandLineOptions(argc, argv) == -1)
		{
			cout << "Usage: owmofc [OPTION] <FILE>..." << endl;
			return 1;
		}

		OW_Reference<OW_MofParserErrorHandlerIFC> theErrorHandler(new TheErrorHandler);
		OW_Reference<OW_CIMOMHandleIFC> handle;
		if (use_cim_repository)
		{
			OW_ServiceEnvironmentIFCRef mofCompEnvironment(new OW_MOFCompEnvironment());
			OW_RepositoryIFCRef cimRepository = OW_RepositoryIFCRef(new OW_CIMRepository(mofCompEnvironment));
			cimRepository->open(repository_dir);
			try
			{
				cimRepository->createNameSpace(OW_CIMNameSpaceUtils::prepareNamespace(namespace_arg), OW_UserInfo(""));
			}
			catch (const OW_CIMException& e)
			{
				// ignore the already exists error.
				if (e.getErrNo() != OW_CIMException::ALREADY_EXISTS)
				{
					throw e;
				}
			}
			handle = OW_CIMOMHandleIFCRef(new OW_MOFCompCIMOMHandle(cimRepository, OW_UserInfo("")));
		}
		else
		{
			OW_URL url(url_arg);
			OW_CIMProtocolIFCRef client;
			client = new OW_HTTPClient(url_arg);

			// TODO: The /owbinary path part is deprecated, remove it post 3.0
			if(encoding_arg == "owbinary" || url.path.equalsIgnoreCase("/owbinary"))
			{
				handle = OW_CIMOMHandleIFCRef(new OW_BinaryCIMOMHandle(client));
			}
			else if (encoding_arg == "cimxml")
			{
				handle = OW_CIMOMHandleIFCRef(new OW_CIMXMLCIMOMHandle(client));
			}
			else
			{
				cerr << "Invalid encoding.  Valid encodings: cimxml, owbinary" << endl;
				return 1;
			}
			client->setLoginCallBack(OW_ClientAuthCBIFCRef(new GetLoginInfo));
		}


		MofCompiler theCompiler(handle, namespace_arg, theErrorHandler);
		if (filelist.empty())
		{
			filelist.push_back("-"); // if they didn't specify a file, read from stdin.
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
	catch(OW_Exception& e)
	{
		cerr << "Caught OW_Exception: " << e.type() << endl;
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

