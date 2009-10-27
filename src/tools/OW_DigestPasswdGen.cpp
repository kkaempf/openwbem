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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "blocxx/MD5.hpp"
#include "blocxx/String.hpp"
#include "blocxx/GetPass.hpp"
#include "blocxx/SocketAddress.hpp"
#include "blocxx/CmdLineParser.hpp"
#include "OW_ToolsCommon.hpp"

#include <iostream>
#include <fstream>

using namespace OpenWBEM;
using namespace OpenWBEM::Tools;
using namespace std;
using namespace blocxx;

namespace
{

enum
{
	HELP_OPT,
	VERSION_OPT,
	LOGIN_NAME_OPT,
	HOSTNAME_OPT,
	PASSWORD_FILE_OPT,
	PASSWORD_OPT
};

CmdLineParser::Option g_options[] =
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{LOGIN_NAME_OPT, 'l', "login_name", CmdLineParser::E_REQUIRED_ARG, 0, "Set the user name. Required."},
	{HOSTNAME_OPT, 'n', "hostname", CmdLineParser::E_REQUIRED_ARG, 0, "Set the hostname of the computer on which the password file will be used. "
		"If omitted, the current system's hostname will be used."},
	{PASSWORD_FILE_OPT, 'f', "password_file", CmdLineParser::E_REQUIRED_ARG, 0, "Set the path identifying the password file. Required."},
	{PASSWORD_OPT, 'p', "password", CmdLineParser::E_REQUIRED_ARG, 0, "Set the password for the new user. If this option is omitted, you will be "
		"prompted for the password."},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void Usage()
{
	cerr << "Usage: owdigestgenpass [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}

} // end unnamed namespace

int main(int argc, char* argv[])
{
	try
	{
		CmdLineParser parser(argc, argv, g_options, CmdLineParser::E_NON_OPTION_ARGS_INVALID);

		if (parser.isSet(HELP_OPT))
		{
			Usage();
			return 0;
		}
		else if (parser.isSet(VERSION_OPT))
		{
			cout << "owdigestgenpass (OpenWBEM) " << OW_VERSION << '\n';
			cout << "Written by Bart Whiteley and Dan Nuffer.\n";
			return 0;
		}

		String name = parser.mustGetOptionValue(LOGIN_NAME_OPT, "-l, --login_name");
		String hostname = parser.getOptionValue(HOSTNAME_OPT);
		String filename = parser.mustGetOptionValue(PASSWORD_FILE_OPT, "-f, --password_file");
		String passwd = parser.getOptionValue(PASSWORD_OPT);

		if (hostname.empty())
		{
			SocketAddress iaddr = SocketAddress::getAnyLocalHost();
			hostname = iaddr.getName();
		}

		ofstream outfile(filename.c_str(), std::ios::app);

		if (!outfile)
		{
			cerr << "Unable to open password file " << filename << endl;
			return 1;
		}

		if (passwd.empty())
		{
			for (;;)
			{
				passwd = GetPass::getPass("Please enter the password for " +
					name + ": ");
				String rePasswd = GetPass::getPass("Please retype the password for " +
					name + ": ");
				if (passwd.equals(rePasswd))
				{
					break;
				}
				else
				{
					cout << "Passwords do not match.  Please try again" << endl;
					continue;
				}
			}
		}

		MD5 md5;
		md5.update(name);
		md5.update(":");
		md5.update(hostname);
		md5.update(":");
		md5.update(passwd);
		outfile << name << ":" << hostname << ":" << md5.toString() << endl;

		return 0;

	}
	catch (CmdLineParserException& e)
	{
		printCmdLineParserExceptionMessage(e);
		Usage();
	}
	return 1;
}


