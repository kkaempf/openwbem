/*******************************************************************************
* Copyright (C) 2005 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc., nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc., OR THE 
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Jon Carey (jcarey@novell.com)
 */

#ifndef OW_ENVVARS_HPP__
#define OW_ENVVARS_HPP__

#include "OW_config.h"
#include "OW_Map.hpp"
#include "OW_String.hpp"

namespace OW_NAMESPACE
{

/**
 * The EnvVars class is an abstraction for a set of environment variables.
 */
class OW_COMMON_API EnvVars
{
public:
	enum EEnvVarFlag
	{
		E_CURRENT_ENVIRONMENT,
		E_EMPTY_ENVIRONMENT
	};

	/**
	 * Create an EnvVars object that is either empty or contains
	 * all of the environment variable from the current environment.
	 */
	EnvVars(EEnvVarFlag flag=E_EMPTY_ENVIRONMENT);
	/**
	 * Create an EnvVars object that contains the evironment variables 
	 * specifed in a string array.
	 * @param envp A NULL terminated char* array that contains the environment
	 *		variables for the EnvVars object. Each element of the array must
	 *		be in the form of "ENV_VAR_NAME=VALUE".
	 */
	EnvVars(const char* const envp[]);
	/**
	 * Copy constructor.
	 * @param arg The EnvVars object this object will be a copy of.
	 */
	EnvVars(const EnvVars& arg);
	/**
	 * Destructor
	 */
	~EnvVars();
	/**
	 * Assignment operator
	 * @param arg The EnvVars object to assign to this one.
	 * @return A reference to this EnvVars object after the assignment
	 *		is done.
	 */
	EnvVars& operator= (const EnvVars& arg);
	/**
	 * @return A null terminated char* array that contains all the environment
	 * variables contained by this EnvVars object. Each element of the
	 * returned array will be of the form "ENV_VAR_NAME=VALUE".
	 */
	const char* const* getenvp() const;
	/**
	 * @return The number of environment variables contained by this object as
	 *		a size_t.
	 */
	size_t size() const
	{
		return m_envMap.size();
	}
	/**
	 * Get the value associated with a given key.
	 * @param key The name for the value to retrieve.
	 * @param notFoundRetVal If the environment variable by the name of 'key'
	 *	is not found, then return the value specified by this parameter.
	 * @return The value associated with a given key. If the key does not
	 *		exist in this EnvVars object, the return value will be an
	 *		empty string.
	 */
	String getValue(const String& key, const String& notFoundRetVal=String()) const;

	/**
	 * Remove an environment variable from this EnvVars object.
	 * @param key The name of the environment variable to be removed.
	 * @return true if key was found and removed. Otherwise return false.
	 */
	bool removeVar(const String& key);

	/**
	 * Set the value for a given environemt variable. If the environment
	 * variable does not exist in the EnvVars object, it will be added.
	 * @param keyValue A key=value string that represent the key/value of
	 *		the environemt variable to be set/added.
	 */
	void setVar(const String& keyValue);

	/**
	 * Set the value for a given environemt variable. If the environment
	 * variable does not exist in the EnvVars object, it will be added.
	 * @param key The name of the environment variable
	 * @param value The value to set the environment variable to.
	 */
	void setVar(const String& key, const String& value);

	/**
	 * Adds the variable name if its not already part of the set.
	 * @return true if it was added, false if it already exists.
	 */
	bool addVar(const String& name, const String& value);

	/**
	 * Updates an existing variable
	 * @return true if variable name was updated, false it it doesn't exist.
	 */
	bool updateVar(const String& name, const String& value);

private:
	typedef Map<String, String> EnvMap;

	void deleteEnvp() const;
	static void fillEnvMap(const char* const envp[], EnvMap& envMap);

	EnvMap m_envMap;
	mutable char** m_envp;
};

}	// End of OW_NAMESPACE

#endif	// OW_ENVVARS_HPP__
