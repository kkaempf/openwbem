/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_CppPropertyProviderIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"

#include <fstream>

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

using std::ifstream;
using std::ofstream;

class OW_OpenLinuxVersion : public OW_CppPropertyProviderIFC
{
public:
	~OW_OpenLinuxVersion();


	virtual OW_CIMValue getPropertyValue(
			const OW_ProviderEnvironmentRef& env,
			OW_CIMObjectPath cop,
			OW_String originClass,
			OW_String propertyName);

	virtual void setPropertyValue(
			const OW_ProviderEnvironmentRef& env,
			OW_CIMObjectPath cop,
			OW_String originClass,
			OW_String propertyName,
			OW_CIMValue val );

	virtual void initialize(const OW_ProviderEnvironmentRef& env);
	virtual void cleanup();

private:
};

//////////////////////////////////////////////////////////////////////////////
OW_OpenLinuxVersion::~OW_OpenLinuxVersion()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_OpenLinuxVersion::getPropertyValue(
		const OW_ProviderEnvironmentRef& env,
		OW_CIMObjectPath cop,
		OW_String originClass,
		OW_String propertyName)
{
	env->getLogger()->logDebug(format("OW_OpenLinuxVersion::getPropertyValue: "
				"cop = %1, originClass = %2, propertyName = %3",
				cop.toString(), originClass, propertyName));
	ifstream in("/etc/.installdata");
	OW_String data;
	while(in)
	{
		data += OW_String(char(in.get()));
	}
	return OW_CIMValue(data);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_OpenLinuxVersion::setPropertyValue(
		const OW_ProviderEnvironmentRef& env,
		OW_CIMObjectPath cop,
		OW_String originClass,
		OW_String propertyName,
		OW_CIMValue val )
{
	env->getLogger()->logDebug(format("OW_OpenLinuxVersion::setPropertyValue: "
				"cop = %1, originClass = %2, propertyName = %3, "
				"val = %4",
				cop.toString(), originClass, propertyName, val.toString()));
	ofstream out("/etc/.installdata", std::ios::trunc);
	out << val.toString();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_OpenLinuxVersion::initialize(const OW_ProviderEnvironmentRef& env)
{
	env->getLogger()->logDebug("OW_OpenLinuxVersion initialize called");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_OpenLinuxVersion::cleanup()
{
}

} // end anonymous namespace

//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY(OW_OpenLinuxVersion);


