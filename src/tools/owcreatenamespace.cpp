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
#include "OW_CmdLineParser.hpp"
#include "OW_URL.hpp"
#include "OW_String.hpp"
#include "OW_ToolsCommon.hpp"

#include <iostream>

using namespace OpenWBEM;
using namespace OpenWBEM::Tools;

using std::cout;
using std::cin;
using std::endl;
using std::cerr;

enum
{
	HELP_OPT,
	VERSION_OPT,
	URL_OPT,
	NAMESPACE_OPT
};

CmdLineParser::Option g_options[] =
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{URL_OPT, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0,
		"The url identifying the cimom. Default is http://localhost/ if not specified."},
	{NAMESPACE_OPT, 'n', "namespace", CmdLineParser::E_REQUIRED_ARG, 0, "Set the namespace to create."},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void Usage()
{
	cerr << "Usage: owcreatenamespace [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	try
	{
		String url;
		String ns;

		// handle backwards compatible options, which was <URL> <namespace>
		// TODO: This is deprecated in 3.1.0, remove it post 3.1
		if (argc == 3 && argv[1][0] != '-' && argv[2][0] != '-')
		{
			url = argv[1];
			ns = argv[2];
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
				cout << "owcreatenamespace (OpenWBEM) " << OW_VERSION << '\n';
				cout << "Written by Dan Nuffer.\n";
				return 0;
			}
	
			url = parser.getOptionValue(URL_OPT, "http://localhost/");
			ns = parser.mustGetOptionValue(NAMESPACE_OPT, "-n, --namespace");
		}
		
		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);
		CIMClient client(url, "root", getLoginInfo);
		cout << "Creating namespace (" << ns << ")" << endl;
		client.createNameSpace(ns);
		return 0;
	}
	catch (CmdLineParserException& e)
	{
		printCmdLineParserExceptionMessage(e);
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


