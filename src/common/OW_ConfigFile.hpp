/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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
 * @author Dan Nuffer
 * @author Kevin Harris
 */

#ifndef OW_CONFIG_FILE_HPP_INCLUDE_GUARD_
#define OW_CONFIG_FILE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/SortedVectorMap.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

namespace ConfigFile
{

	struct ItemData
	{
		ItemData(const blocxx::String& source_, const blocxx::String& value_)
			: source(source_)
			, value(value_)
		{}
		blocxx::String source; // e.g. the filename or empty if from an internal set.
		blocxx::String value;
	};
	typedef blocxx::Array<ItemData> ItemDataArray;

	typedef blocxx::SortedVectorMap<blocxx::String, ItemDataArray> ConfigMap;

	/**
	 * Loads a config file and stores the options in rval.
	 *
	 * Config file lines are formatted as (mixed bnf/regex):
	 *
	 *   ConfigFile:
	 *     ConfigLine
	 *     | ConfigFile ConfigLine
	 *
	 *   ConfigLine:
	 *     CommentLine
	 *     | ConfigValue
	 *
	 *   CommentLine:
	 *     Whitespace ';' Anything '\n'
	 *     | Whitespace '#' Anything '\n'
	 *     | Whitespace '\n'
	 *
	 *   ConfigValue:
	 *     Whitespace <ItemName> Whitespace '=' Whitespace [ItemValue] Whitespace '\n'
	 *
	 *   <ItemName>:
	 *     [^=]+
	 *
	 *   [ItemValue]:
	 *     Anything
	 *
	 *   Blank:
	 *     (any character isspace() says is a space.  POSIX: [ \f\n\r\t\v])
	 *
	 *   Whitespace:
	 *     ^$
	 *     | Blank
	 *     | Whitespace Blank
	 *
	 *   Anything:
	 *     [^\n]*
	 *
	 * All leading and trailing whitespace is removed from <ItemName> and
	 * [ItemValue].
	 *
	 * Each <ItemName> can appear in the config file more than once.  Each
	 * occurance in the config file will cause a new value to be appended.
	 *
	 * <ItemName> must be non-empty.
	 *
	 * [ItemValue] may be empty, which will cause an empty String to be appended
	 * to the map for that particular <ItemName>.
	 *
	 * Comment lines can begin with '#' or ';' and will be ignored and empty
	 * lines are also ignored.
	 *
	 * @throws ConfigException if the file doesn't exist or is malformed
	 */
	OW_COMMON_API void loadConfigFile(const blocxx::String& filename, ConfigMap& rval);

	/**
	 * Retrieve itemName item from configItems. If it's not present, defRetVal will be returned.
	 * If the item has multiple values, only the last value will be returned.
	 */
	OW_COMMON_API blocxx::String getConfigItem(const ConfigMap& configItems, const blocxx::String &itemName, const blocxx::String& defRetVal = blocxx::String());

	/**
	 * Retrieve itemName values from configItems. If it's not present, defRetVal will be returned.
	 * @param tokenizeSeparator If non-null, then each item will be tokenized using the specified separator chars and returned as separate items.
	 */
	OW_COMMON_API blocxx::StringArray getMultiConfigItem(const ConfigMap& configItems, const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal = blocxx::StringArray(), const char* tokenizeSeparator = 0);

	enum EOverwritePreviousFlag
	{
		E_PRESERVE_PREVIOUS,
		E_OVERWRITE_PREVIOUS
	};

	/**
	 * Sets itemName to value in configItems.
	 * @param overwritePrevious E_PRESERVE_PREVIOUS - If itemName already exists in configItems, will not be changed.
	 *   E_OVERWRITE_PREVIOUS - If itemName already exists, it's value will be set to value.
	 */
	OW_COMMON_API void setConfigItem(ConfigMap& configItems, const blocxx::String& itemName,
		const blocxx::String& value, EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS);



} // end namespace ConfigFile

} // end namespace OW_NAMESPACE

#endif

