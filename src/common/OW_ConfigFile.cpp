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
#include "OW_config.h"
#include "OW_ConfigFile.hpp"
#include "OW_ConfigException.hpp"
#include "OW_Format.hpp"

#include <fstream>

namespace OpenWBEM
{
namespace ConfigFile
{

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
	while(file)
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
				if(idx + 1 < line.length())
				{
					String itemValue = line.substring(idx + 1).trim();
					if(!itemValue.empty())
					{
						String item = line.substring(0, idx).trim();
						ConfigMap::iterator it = rval.find(item);
						if(it == rval.end())
						{
							rval.insert(std::make_pair(item, itemValue));
						}
					}
				}
			}
			else
			{
				OW_THROW(ConfigException, format("Error in config file:"
					" %1 at line %2.\n  Line is %3", filename, lineNum,
					line).c_str());
			}
		}
	}
}

} // end namespace ConfigFile
} // end namespace OpenWBEM

