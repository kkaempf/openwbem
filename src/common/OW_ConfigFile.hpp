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

#ifndef OW_CONFIG_FILE_HPP_INCLUDE_GUARD_
#define OW_CONFIG_FILE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_SortedVectorMap.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{
									
namespace ConfigFile
{

	typedef SortedVectorMap<String, String> ConfigMap;

	/**
	 * Loads a config file and stores the options in rval.
	 * Items which already exist in rval will not be overwritten.
	 * @throws ConfigException if the file doesn't exist or is malformed
	 */
	void loadConfigFile(const String& filename, ConfigMap& rval);

	/**
	 * Retrieve itemName item from configItems. If it's not present, defRetVal will be returned.
	 */
	String getConfigItem(const ConfigMap& configItems, const String &itemName, const String& defRetVal = String());

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
	void setConfigItem(ConfigMap& configItems, const String& itemName,
		const String& value, EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS);



} // end namespace ConfigFile

} // end namespace OpenWBEM

#endif

