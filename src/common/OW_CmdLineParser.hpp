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

#ifndef OW_CMD_LINE_PARSER_HPP_INCLUDE_GUARD_
#define OW_CMD_LINE_PARSER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Array.hpp"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

OW_DECLARE_APIEXCEPTION(CmdLineParser, OW_COMMON_API)

/**
 * Do command line parsing.
 *
 * Thread safety: read/write
 * Copy semantics: Value
 * Exception safety: Strong
 */
class OW_COMMON_API CmdLineParser
{
public:
	enum EArgumentTypeFlag
	{
		E_NO_ARG,       // the option does not take an argument
		E_REQUIRED_ARG, // the option requires an argument
		E_OPTIONAL_ARG  // the option might have an argument
	};

	// errors codes that may be specified when a CmdLineParserException is thrown
	enum EErrorCodes
	{
		E_INVALID_OPTION,  // an unknown option was specified
		E_MISSING_ARGUMENT // an option for which argtype == E_REQUIRED_ARG did not have an argument
	};

	struct Option
	{
		int id;                    // unique option id, used to retrieve option values
		char shortopt;             // short option char.  Set to '\0' for none.
		const char* longopt;       // long option string.  Set to 0 for none.
		EArgumentTypeFlag argtype; // specifies constraints for the option's argument
		const char* defaultValue;  // if argtype == E_OPTIONAL_ARG and no argument is specified, this value will be returned.  Set to 0 for none.
		const char* description;   // description used by getUsage().  May be 0.
	};

	/**
	 * @param argc Count of pointers in argv.  Pass value from main().
	 * @param argv Arguments.  Pass value from main(). Value is not saved.
	 * @param options An array of Option terminated by a final entry that has a '\0' shortopt && 0 longopt.
	 *   Value is not saved.
	 * @throws CmdLineParserException if the given command line is invalid.
	 */
	CmdLineParser(int argc, char const* const* const argv, const Option* options);

	/**
	 * Read out a string option.
	 * @param id The id of the option.
	 * @return The value of the option, if given, otherwise an empty String.
	 */
	String getOptionValue(int id) const;
 	
	/**
	 * Read out all occurences of a string option.
	 * @param id The id of the option.
	 * @return The value of the option, if given, otherwise an empty StringArray.
	 */
 	StringArray getOptionValueList(int id) const;
 	
	/**
	 * Read out a boolean option or check for the presence of string option.
	 */
	bool isSet(int id) const;
 	
	/**
	 * Read the number of arguments that aren't options (but, for example, filenames).
	 */
	size_t getNonOptionCount () const;
 	
	/**
	 * Read out an non-option argument.
	 * @param n The 0-based index of the argument.  Valid values are 0 to count()-1.
	 */
	String getNonOptionArg(size_t n) const;

	/**
	 * Read out the non-option args
	 */
	StringArray getNonOptionArgs() const;
	
	/**
	 * Generate a usage string for the options.  e.g.:
	 * "Options:\n"
	 * "  -1, --one                 first description\n"
	 * "  -2, --two [arg]           second description (default is optional)\n"
	 * "  -3, --three <arg>         third description\n"
	 *
	 * [arg] is used for E_OPTIONAL_ARG options, and <arg> for E_REQUIRED_ARG options.
	 *
	 * @param options An array of Option terminated by a final entry that has a '\0' shortopt && 0 longopt.
	 * @param maxColumns Wrap the descriptions so no line of the usage string exceeds the specified
	 *   number of columns.
	 */
	static String getUsage(const Option* options, unsigned int maxColumns = 80);


private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	// key is Option::id, value is the value(s) specified by the user
	typedef SortedVectorMap<int, StringArray> optionsMap_t;
	optionsMap_t m_parsedOptions;
	StringArray m_nonOptionArgs;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};

} // end namespace OpenWBEM

#endif


