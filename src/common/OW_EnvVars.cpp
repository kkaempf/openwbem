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

#include "OW_config.h"
#include "OW_EnvVars.hpp"

#include <algorithm>
#include <cstring>

// according to man environ:
// This variable must be declared in the user program, but is
// declared in the header file unistd.h in case the header files came from
// libc4 or libc5, and in case they came from glibc and _GNU_SOURCE was
// defined.
extern char** environ;

namespace OW_NAMESPACE
{

namespace
{

void getKeyValue(
	const char *const strArg, 
	String& key, 
	String& value)
{
	key.erase();
	value.erase();

	const char* p = ::strchr(strArg, '=');
	if(p && *(p+1))
	{
		key = String(strArg, size_t(p-strArg));
		value = p+1;
	}
}

}	// End of anonymous namespace

//////////////////////////////////////////////////////////////////////////////
EnvVars::EnvVars(EEnvVarFlag flag)
	: m_envMap()
	, m_envp(0)
{
	if(flag == E_CURRENT_ENVIRONMENT)
	{
		fillEnvMap(environ, m_envMap);
	}
}

//////////////////////////////////////////////////////////////////////////////
EnvVars::EnvVars(const char* const envp[])
	: m_envMap()
	, m_envp(0)
{
	fillEnvMap(envp, m_envMap);
}

//////////////////////////////////////////////////////////////////////////////
EnvVars::~EnvVars()
{
	deleteEnvp();
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void 
EnvVars::fillEnvMap(const char* const envp[], EnvMap& envMap)
{
	envMap.clear();
	String key, value;
	for(size_t i = 0; envp[i]; i++)
	{
		getKeyValue(envp[i], key, value);
		if(key.length())
		{
			envMap[key] = value;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void 
EnvVars::deleteEnvp() const
{
	if(m_envp)
	{
		int i;
	
		// Delete all char pointers env var array
		for(i = 0; m_envp[i]; i++)
		{
			// m_envp[i] may be null if deleteEnvp was called because an
			// exception was caught while trying to allocate the array
			// in getenvp()
			delete [] m_envp[i];
		}

		delete [] m_envp;		// Delete pointer array
		m_envp = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
String 
EnvVars::getValue(const String& key, 
	const String& notFoundRetVal) const
{
	EnvMap::const_iterator it = m_envMap.find(key);
	return (it != m_envMap.end()) ? it->second : notFoundRetVal;
}

//////////////////////////////////////////////////////////////////////////////
const char* const* 
EnvVars::getenvp() const
{
	if(!m_envp && m_envMap.size())
	{
		int i;
		m_envp = new char* [m_envMap.size()+1];
		std::fill(m_envp, m_envp+m_envMap.size()+1, (char*)0);
		try
		{
			EnvMap::const_iterator it = m_envMap.begin();
			for(i = 0, it = m_envMap.begin(); it != m_envMap.end(); i++, it++)
			{
				m_envp[i] = new char[it->first.length() + 
					it->second.length() + 1];
				::strcpy(m_envp[i], it->first.c_str());
				m_envp[i][it->first.length()] = '=';
				::strcpy(m_envp[i]+it->first.length()+1, it->second.c_str());
			}
		}
		catch(...)
		{
			deleteEnvp();	// Delete what has been allocated thus far.
			throw;			// Re-throw this exception
		}
	}

	return m_envp;
}

//////////////////////////////////////////////////////////////////////////////
bool 
EnvVars::removeVar(const String& varName)
{
	bool cc = false;
	EnvMap::iterator it = m_envMap.find(varName);
	if (it != m_envMap.end())
	{
		cc = true;
		deleteEnvp();
		m_envMap.erase(it);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
bool 
EnvVars::addVar(const String& name, const String& value)
{
	bool cc = false;
	if(m_envMap.find(name) == m_envMap.end())
	{
		cc = true;
		deleteEnvp();
		m_envMap[name] = value;
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
void 
EnvVars::setVar(const String& key, const String& value)
{
	deleteEnvp();
	m_envMap[key] = value;
}

//////////////////////////////////////////////////////////////////////////////
void 
EnvVars::setVar(const String& keyValue)
{
	String key, value;
	getKeyValue(keyValue.c_str(), key, value);
	if(key.length())
	{
		setVar(key, value);
	}
}

//////////////////////////////////////////////////////////////////////////////
bool 
EnvVars::updateVar(const String& name, const String& value)
{
	bool cc = false;
	EnvMap::iterator it = m_envMap.find(name);
	if (it != m_envMap.end())
	{
		cc = true;
		deleteEnvp();
		it->second = value;
	}

	return cc;
}

}	// End of OW_NAMESPACE
