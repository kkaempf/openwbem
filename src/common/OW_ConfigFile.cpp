/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*	this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*	this list of conditions and the following disclaimer in the documentation
*	and/or other materials provided with the distribution.
*
*  - Neither the name of Vintela, Inc. nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ConfigFile.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Format.hpp"

#include <fstream>

namespace OW_NAMESPACE
{
namespace ConfigFile
{

/////////////////////////////////////////////////////////////////////////////
void loadConfigFile(const String& filename, ConfigMap& rval)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
		OW_THROW(ConfigException, Format("Unable to read config"
					" file: %1", filename).c_str());
	}
	
	String line;
	int lineNum = 0;
	while (file)
	{
		lineNum++;
		line = String::getLine(file);
		if (!line.empty())
		{
			// If comment line, ignore
			if (line[0] == '#' || line[0] == ';')
			{
				continue;
			}
			size_t idx = line.indexOf('=');
			if (idx != String::npos)
			{
				if (idx + 1 < line.length())
				{
					String itemValue = line.substring(idx + 1).trim();
					if (!itemValue.empty())
					{
						String item = line.substring(0, idx).trim();
						rval[item].push_back(ItemData(filename, itemValue));
					}
				}
			}
			else
			{
				OW_THROW(ConfigException, Format("Error in config file:"
					" %1 at line %2.\n  Line is %3", filename, lineNum,
					line).c_str());
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
String
getConfigItem(const ConfigMap& configItems, const String &itemName, const String& defRetVal)
{
	ConfigMap::const_iterator i = configItems.find(itemName);
	if (i != configItems.end() && i->second.size() > 0)
	{
		return i->second.back().value;
	}
	else
	{
		return defRetVal;
	}
}

/////////////////////////////////////////////////////////////////////////////
StringArray
getMultiConfigItem(const ConfigMap& configItems, const String &itemName, const StringArray& defRetVal, const char* tokenizeSeparator)
{
	ConfigMap::const_iterator item = configItems.find(itemName);
	if (item != configItems.end())
	{
		StringArray rv;
		rv.reserve(item->second.size());
		for (size_t i = 0; i < item->second.size(); ++i)
		{
			if (tokenizeSeparator)
			{
				StringArray tokenizedValue(item->second[i].value.tokenize(tokenizeSeparator));
				rv.insert(rv.end(), tokenizedValue.begin(), tokenizedValue.end());
			}
			else
			{
				rv.push_back(item->second[i].value);
			}
		}
		return rv;
	}
	else
	{
		return defRetVal;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
setConfigItem(ConfigMap& configItems, const String& itemName,
	const String& value, EOverwritePreviousFlag overwritePrevious)
{
	ConfigMap::iterator it = configItems.find(itemName);
	if (it == configItems.end())
	{
		configItems[itemName].push_back(ItemData("", value));
	}
	else if (overwritePrevious == E_OVERWRITE_PREVIOUS)
	{
		ItemDataArray& values = configItems[itemName];
		values.clear();
		values.push_back(ItemData("", value));
	}
	// else overwritePrevious == E_PRESERVE_PREVIOUS, and do nothing
}


} // end namespace ConfigFile
} // end namespace OW_NAMESPACE

