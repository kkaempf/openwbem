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
#include "OW_CmdLineParser.hpp"
#include "OW_Array.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_Assertion.hpp"

#include <algorithm>


namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(CmdLineParser)

namespace
{
/////////////////////////////////////////////////////////////////////////////
	struct longOptIs
	{
		longOptIs(const String& longOpt) : m_longOpt(longOpt) {}

		bool operator()(const CmdLineParser::Option& x) const
		{
			if (x.longopt != 0)
			{
				return m_longOpt.startsWith(x.longopt);
			}
			return false;
		}

		String m_longOpt;
	};

/////////////////////////////////////////////////////////////////////////////
	struct shortOptIs
	{
		shortOptIs(char shortOpt) : m_shortOpt(shortOpt) {}

		bool operator()(const CmdLineParser::Option& x) const
		{
			return m_shortOpt == x.shortopt;
		}

		char m_shortOpt;
	};

}

/////////////////////////////////////////////////////////////////////////////
CmdLineParser::CmdLineParser(int argc, char const* const* const argv_, const Option* options)
{
	OW_ASSERT(argc > 0); // have to get at least the name
	OW_ASSERT(argv_ != 0);
	OW_ASSERT(options != 0);
	char const* const* argv(argv_);
	char const* const* argvEnd(argv + argc);
	const Option* optionsEnd;

	// m_options is an array terminated by a final entry that has a '\0' shortopt && 0 longopt.
	optionsEnd = options;
	while (optionsEnd->shortopt != '\0' || optionsEnd->longopt != 0)
	{
		++optionsEnd;
	}

	// skip the first argv, which is the program name and loop through the rest
	for (++argv ;argv != argvEnd; ++argv)
	{
		OW_ASSERT(*argv != 0);
		String arg(*argv);

		// look for an option
		if ((arg.length() >= 2) && (arg[0] == '-'))
		{
			const Option* theOpt;
			bool longOpt = false;
			if (arg[1] == '-')
			{
				// long option
				longOpt = true;
				arg = arg.substring(2); // erase the --
				theOpt = std::find_if(options,  optionsEnd, longOptIs(arg));
			}
			else // short option
			{
				longOpt = false;
				arg = arg.substring(1); // erase the -
				theOpt = std::find_if(options,  optionsEnd, shortOptIs(arg[0]));
			}

			if (theOpt == optionsEnd)
			{
				OW_THROW_ERR(CmdLineParserException, arg.c_str(), E_INVALID_OPTION);
			}

			if (theOpt->argtype == E_NO_ARG)
			{
				m_parsedOptions[theOpt->id]; // if one is already there, don't modify it, else insert a new one
				continue;
			}
			// now see if we can get the value
			String val;
			if ((theOpt->argtype == E_OPTIONAL_ARG) && (theOpt->defaultValue != 0))
			{
				val = theOpt->defaultValue;
			}
			
			const char* p = ::strchr(arg.c_str(), '=');
			if (p)
			{
				// get everything after the =
				val = String(p+1);
			}
			else
			{
				// a short option can have the value together with it (i.e. -I/opt/vintela/include)
				if (longOpt == false && arg.length() > 1)
				{
					val = arg.substring(1);
				}
				// look at the next arg
				else if (argv+1 != argvEnd)
				{
					if (**(argv+1) != '-')
					{
						val = *(argv+1);
						++argv;
					}
				}
			}

			// make sure we got an arg if one is required
			if (theOpt->argtype == E_REQUIRED_ARG && val.empty())
			{
				OW_THROW_ERR(CmdLineParserException, arg.c_str(), E_MISSING_ARGUMENT);
			}

			m_parsedOptions[theOpt->id].push_back(val);
		}
		else
		{
			m_nonOptionArgs.push_back(arg);
		}
		
	}
}

/////////////////////////////////////////////////////////////////////////////
// static
String
CmdLineParser::getUsage(const Option* options)
{
// looks like this:
//     "Options:\n"
//     "  -1, --one                 first description\n"
//     "  -2, --two [arg]           second description (default is optional)\n"
//     "  -3, --three <arg>         third description\n"
	StringBuffer usage("Options:\n");

	// m_options is an array terminated by a final entry that has a '\0' shortopt && 0 longOpt.
	for (const Option* curOption = options; curOption->shortopt != '\0' || curOption->longopt != 0; ++curOption)
	{
		StringBuffer curLine;
		curLine += "  ";
		if (curOption->shortopt != '\0')
		{
			curLine += '-';
			curLine += curOption->shortopt;
			if (curOption->longopt != 0)
			{
				curLine += ", ";
			}
		}
		if (curOption->longopt != 0)
		{
			curLine += "--";
			curLine += curOption->longopt;
		}

		if (curOption->argtype == E_REQUIRED_ARG)
		{
			curLine += " <arg>";
		}
		else if (curOption->argtype == E_OPTIONAL_ARG)
		{
			curLine += " [arg]";
		}

		size_t bufferlen = (curLine.length() >= 27) ? 1 : (28 - curLine.length());
		for (size_t i = 0; i < bufferlen; ++i)
		{
			curLine += ' ';
		}

		if (curOption->description != 0)
		{
			curLine += curOption->description;
		}
		
		if (curOption->defaultValue != 0)
		{
			curLine += " (default is ";
			curLine += curOption->defaultValue;
			curLine += ')';
		}
		
		curLine += '\n';
		usage += curLine;
	}
	return usage.releaseString();
}

/////////////////////////////////////////////////////////////////////////////
String
CmdLineParser::getOptionValue(int id) const
{
	String rval;
	optionsMap_t::const_iterator ci = m_parsedOptions.find(id);
	if (ci != m_parsedOptions.end() && ci->second.size() > 0)
	{
		// grab the last one
		rval = ci->second[ci->second.size()-1];
	}
	return rval;
}

/////////////////////////////////////////////////////////////////////////////
StringArray
CmdLineParser::getOptionValueList(int id) const
{
	StringArray rval;
	optionsMap_t::const_iterator ci = m_parsedOptions.find(id);
	if (ci != m_parsedOptions.end() && ci->second.size() > 0)
	{
		rval = ci->second;
	}
	return rval;
}

/////////////////////////////////////////////////////////////////////////////
bool
CmdLineParser::isSet(int id) const
{
	return m_parsedOptions.count(id) > 0;
}

/////////////////////////////////////////////////////////////////////////////
size_t
CmdLineParser::getNonOptionCount () const
{
	return m_nonOptionArgs.size();
}

/////////////////////////////////////////////////////////////////////////////
String
CmdLineParser::getNonOptionArg(size_t n) const
{
	return m_nonOptionArgs[n];
}



} // end namespace OpenWBEM



