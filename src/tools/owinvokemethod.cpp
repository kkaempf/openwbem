/*******************************************************************************
 * Copyright (C) 2001-2005 Vintela, Inc. All rights reserved.
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
#include "OW_Assertion.hpp"
#include "OW_GetPass.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_CmdLineParser.hpp"
#include "OW_URL.hpp"
#include "OW_ToolsCommon.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_Array.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>

using namespace OpenWBEM;
using namespace OpenWBEM::Tools;

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using namespace WBEMFlags;

namespace
{

enum
{
	HELP_OPT,
	VERSION_OPT,
	URL_OPT,
	TARGET_OPT,
	METHODNAME_OPT,
	INPARAMS_OPT,
	ACCEPT_LANGUAGE_OPT
};

CmdLineParser::Option g_options[] =
{
	{HELP_OPT, 'h', "help", CmdLineParser::E_NO_ARG, 0, "Show help about options."},
	{VERSION_OPT, 'v', "version", CmdLineParser::E_NO_ARG, 0, "Show version information."},
	{URL_OPT, 'u', "url", CmdLineParser::E_REQUIRED_ARG, 0,
		"Url identifying the cimom and namespace."},
	{TARGET_OPT, 't', "target", CmdLineParser::E_REQUIRED_ARG, 0,
		"Object path identifying the target class or instance."},
	{METHODNAME_OPT, 'm', "methodname", CmdLineParser::E_REQUIRED_ARG, 0, "The method to call"},
	{INPARAMS_OPT, 'i', "inparams", CmdLineParser::E_REQUIRED_ARG, 0, "In parameters. A semi-colon separated list of name=value pairs. "
		"In values, the ; \\ , characters must be escaped using \\. 2-digit hex chars can be specified e.g. \\x0F. "
		"Use , to separate array element values. This parameter is optional."},
	{ACCEPT_LANGUAGE_OPT, 'a', "accept-language", CmdLineParser::E_OPTIONAL_ARG, "en-US", "Specify an language tag."},
	{0, 0, 0, CmdLineParser::E_NO_ARG, 0, 0}
};

void Usage()
{
	cerr << "Usage: owinvokemethod [options]\n\n";
	cerr << CmdLineParser::getUsage(g_options) << endl;
}

// grammar for the parameters
// <params> := [ <name value pair> ( ";" <name value pair> )* ]
// <name value pair> := <name> "=" <value>
// <name> := [A-Za-z][A-Za-z0-9_]*
// <value> := <single value> | <array value>
// <single value> := <escaped string>
// <escaped string> := ( [^;\,] | "\" [;\,ntr] | "\x" <hex> <hex> )*
// <array value> := <escaped string> ( "," <escaped string> )*

OW_DECLARE_EXCEPTION(ParameterParse);
OW_DEFINE_EXCEPTION(ParameterParse);

char getNext(size_t& idx, const String& paramStr)
{
	if (idx < paramStr.length())
	{
		return paramStr[idx];
	}
	else
	{
		OW_THROW(ParameterParseException, "End of string");
	}
}

void next(size_t& idx, const String& paramStr, char expected)
{
	if (getNext(idx, paramStr) == expected)
	{
		++idx;
	}
	else
	{
		OW_THROW(ParameterParseException, Format("Expected %1", expected).c_str());
	}
}

CIMName parseName(size_t& idx, const String& paramStr)
{
	String name;
	char first = getNext(idx, paramStr);
	if (isalpha(first))
	{
		name += first;
		++idx;
	}
	else
	{
		OW_THROW(ParameterParseException, "Expected alpha char");
	}

	while (idx < paramStr.length() && (isalnum(paramStr[idx]) || paramStr[idx] == '_'))
	{
		name += paramStr[idx];
		++idx;
	}
	return name;
}

unsigned char decodeHex(char c)
{
	if (isdigit(c))
	{
		return c - '0';
	}
	else
	{
		c = toupper(c);
		return c - 'A' + 0xA;
	}
}

String parseEscapedString(size_t& idx, const String& paramStr)
{
	// <escaped string> := ( [^;\,] | "\" [;\,ntr] | "\x" <hex> <hex> )*
	// parse it and un-escape it
	StringBuffer unescaped;
	while (idx < paramStr.length() && !(paramStr[idx] == ';' || paramStr[idx] == ','))
	{
		if (paramStr[idx] == '\\')
		{
			++idx;
			char c = getNext(idx, paramStr);
			switch (c)
			{
				case ';':
					unescaped += ';';
					break;
				case '\\':
					unescaped += '\\';
					break;
				case ',':
					unescaped += ',';
					break;
				case 'n':
					unescaped += '\n';
					break;
				case 't':
					unescaped += '\t';
					break;
				case 'r':
					unescaped += '\r';
					break;
				case 'x':
				{
					++idx;
					char first = getNext(idx, paramStr);
					++idx;
					char second = getNext(idx, paramStr);
					if (!isxdigit(first) || !isxdigit(second))
					{
						OW_THROW(ParameterParseException, "Expected 2 hex chars after \\x");
					}
					unescaped += decodeHex(first) << 4 + decodeHex(second);
				}
			}
			++idx;
		}
		else
		{
			unescaped += paramStr[idx];
			++idx;
		}
	}
	return unescaped.releaseString();
}

CIMValue parseValue(size_t& idx, const String& paramStr)
{
	StringArray a;
	String firstVal = parseEscapedString(idx, paramStr);
	a.push_back(firstVal);
	while (idx < paramStr.length() && paramStr[idx] == ',')
	{
		++idx;
		String nextVal = parseEscapedString(idx, paramStr);
		a.push_back(nextVal);
	}
	// the return is either a String or a StringArray, since we don't know what the actual type should be.
	if (a.size() == 1)
	{
		return CIMValue(a[0]);
	}
	else
	{
		return CIMValue(a);
	}
}

CIMParamValue parseNameValuePair(size_t& idx, const String& paramStr)
{
	CIMName name = parseName(idx, paramStr);
	next(idx, paramStr, '=');
	CIMValue val = parseValue(idx, paramStr);
	return CIMParamValue(name, val);
}

CIMParamValueArray parseParams(const String& paramStr)
{
	CIMParamValueArray parsedParams;
	if (!paramStr.empty())
	{
		size_t idx = 0;
		CIMParamValue curpv = parseNameValuePair(idx, paramStr);
		parsedParams.push_back(curpv);
		while (idx < paramStr.length())
		{
			next(idx, paramStr, ';');
			curpv = parseNameValuePair(idx, paramStr);
			parsedParams.push_back(curpv);
		}		
	}
	return parsedParams;
}

}

//////////////////////////////////////////////////////////////////////////////
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
			cout << "owinvokemethod (OpenWBEM) " << OW_VERSION << '\n';
			cout << "Written by Dan Nuffer.\n";
			return 0;
		}

		String url = parser.getOptionValue(URL_OPT, "http://localhost/root/cimv2");
		String ns = URL(url).namespaceName;
		if (ns.empty())
		{
			ns = "root/cimv2";
		}
		CIMObjectPath target(CIMNULL);
		String targetOpt = parser.mustGetOptionValue(TARGET_OPT, "-t, --target");
		try
		{
			target = CIMObjectPath::parse(targetOpt);
		}
		catch (CIMException& e)
		{
			// CIMObjectPath::parse() only handles instance paths. If it throws, treat it as a class name
			target = CIMObjectPath(targetOpt);
		}

		String methodname = parser.mustGetOptionValue(METHODNAME_OPT, "-m, --methodname");
		String inParamsStr = parser.getOptionValue(INPARAMS_OPT);
		String acceptLanguage = parser.getOptionValue(ACCEPT_LANGUAGE_OPT);

		CIMParamValueArray inParams = parseParams(inParamsStr);
		CIMParamValueArray outParams;

		ClientAuthCBIFCRef getLoginInfo(new GetLoginInfo);

		CIMClient rch(url, ns, getLoginInfo);
		rch.setHTTPRequestHeader("Accept-Language", acceptLanguage);
		//ClientCIMOMHandleRef rch = ClientCIMOMHandle::createFromURL(url, getLoginInfo);
		//rch->getWBEMProtocolHandler()->

		CIMValue rv = rch.invokeMethod(target, methodname, inParams, outParams);
		cout << rv << '\n';
		for (size_t i = 0; i < outParams.size(); ++i)
		{
			cout << outParams[i].getName() << '=' << outParams[i].getValue() << '\n';
		}
		return 0;
	}
	catch (CmdLineParserException& e)
	{
		printCmdLineParserExceptionMessage(e);
		Usage();
	}
	catch(const CIMException& e)
	{
		cerr << CIMException::getCodeName(e.getErrNo()) << ':' << e.getMessage() << endl;
	}
	catch(const Exception& e)
	{
		cerr << e.getMessage() << endl;
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



