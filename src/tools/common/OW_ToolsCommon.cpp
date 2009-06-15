/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
#include "OW_ToolsCommon.hpp"
#include "blocxx/CmdLineParser.hpp"
#include "blocxx/GetPass.hpp"

#include <iostream>

using namespace std;

namespace OW_NAMESPACE
{
namespace Tools
{

void printCmdLineParserExceptionMessage(const CmdLineParserException& e)
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
}

/////////////////////////////////////////////////////////////////////////////
bool GetLoginInfo::getCredentials(const String& realm, String& name, String& passwd, const String& details)
{
	cout << "Authentication required for " << realm << endl;
	cout << "Enter the user name: ";
	name = String::getLine(cin);
	passwd = GetPass::getPass("Enter the password for " +
		name + ": ");
	return true;
}

} // end namespace Tools
} // end namespace OpenWBEM




