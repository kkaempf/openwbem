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
#include "OW_CIMClient.hpp"
#include "OW_GetPass.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_URL.hpp"
#include "OW_String.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_ClientCIMOMHandle.hpp"

#include <iterator>
#include <algorithm>
#include <iostream>

using namespace OpenWBEM;
using namespace WBEMFlags;

using std::cout;
using std::cin;
using std::endl;
using std::cerr;

namespace
{

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

class NamespacePrinter : public StringResultHandlerIFC
{
	void doHandle(const String& s)
	{
		cout << s << endl;
	}
};

enum
{
	HELP_OPT,
	VERSION_OPT,
	URL_OPT
};

CmdLineParser::Option g_options[] =
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{URL_OPT, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0,
		"The url identifying the cimom. Default is http://localhost/ if not specified."},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void Usage()
{
	cerr << "Usage: owenumnamespace [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}

}

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	try
	{
		String url;
		String ns;
		bool use__Namespace(false);

		// handle backwards compatible options, which was <URL> <namespace>
		// TODO: This is deprecated in 3.1.0, remove it post 3.1
		if (argc == 3 && argv[1][0] != '-' && argv[2][0] != '-')
		{
			url = argv[1];
			ns = argv[2];
			use__Namespace = true;
			cerr << "This cmd line usage is deprecated!\n";
		}
		else
		{
			CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);
	
			if (parser.isSet(HELP_OPT))
			{
				Usage();
				return 0;
			}
			else if (parser.isSet(VERSION_OPT))
			{
				cout << "owenumnamespace (OpenWBEM) " << OW_VERSION << '\n';
				cout << "Written by Dan Nuffer.\n";
				return 0;
			}
	
			url = parser.getOptionValue(URL_OPT);
			if (url.empty())
			{
				url = "http://localhost/";
			}
	
		}
		
		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
		if (use__Namespace)
		{
			ClientCIMOMHandleRef rch = ClientCIMOMHandle::createFromURL(url, getLoginInfo);
			EDeepFlag deep = E_DEEP; // TODO: get this from a command line argument

			StringArray rval = CIMNameSpaceUtils::enum__Namespace(*rch, ns, deep);
			copy(rval.begin(), rval.end(), std::ostream_iterator<String>(cout, "\n"));
		}
		else
		{
			CIMClient client(url, "root", getLoginInfo);
			NamespacePrinter namespacePrinter;
			client.enumCIM_Namespace(namespacePrinter);
		}
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
			case CmdLineParser::E_INVALID_NON_OPTION_ARG:
				cerr << "invalid non-option argument: " << e.getMessage() << '\n';
			break;
			default:
				cerr << "failed parsing command line options: " << e << "\n";
			break;
		}
		Usage();
	}
	catch(const Exception& e)
	{
		cerr << e << endl;
	}
	catch(const std::exception& e)
	{
		cerr << e.what() << endl;
	}
	catch(...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}


