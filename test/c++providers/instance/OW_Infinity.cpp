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


#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"

namespace{

class InfiniteIP : public OW_CppInstanceProviderIFC
{
public:

	virtual ~InfiniteIP()
	{
	}

	InfiniteIP()
	{
	}

	// Here are the virtual functions we'll implement.  Check the base
	// classes for the documentation.


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_Bool& deep,
		const OW_CIMClass& cimClass )
	{
		(void)deep;
		(void)cimClass;
		(void)env;

		OW_UInt64 count = 0;
		for (;;++count)
		{
			OW_CIMObjectPath newCop(className, ns);
			newCop.addKey(OW_String("InstanceNumber"), OW_CIMValue(count));
			result.handle(newCop);
		}
	}


//////////////////////////////////////////////////////////////////////////////
	void
		enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_Bool& deep,
		const OW_CIMClass& cimClass,
		const OW_Bool& localOnly )
	{
		(void)env;
		(void)ns;
		(void)className;
		(void)deep;
		(void)localOnly;

		OW_UInt64 count = 0;
		OW_CIMInstance newInst = cimClass.newInstance();
		for (;;++count)
		{
			newInst.setProperty(OW_String("InstanceNumber"), OW_CIMValue(count));
			result.handle(newInst);
		}

	}

//////////////////////////////////////////////////////////////////////////////
	OW_CIMInstance
		getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		const OW_CIMClass& cimClass,
		const OW_Bool& localOnly )
	{
		(void)env;
		(void)ns;
		(void)instanceName;
		(void)cimClass;
		(void)localOnly;

		OW_THROWCIM(OW_CIMException::FAILED);

	}






//////////////////////////////////////////////////////////////////////////////
	OW_CIMObjectPath
		createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMInstance& cimInstance )
	{
		(void)env;
		(void)cop;
		(void)cimInstance;
		OW_THROWCIM(OW_CIMException::FAILED);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMInstance& cimInstance)
	{
		(void)env;
		(void)cop;
		(void)cimInstance;
		OW_THROWCIM(OW_CIMException::FAILED);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		(void)cop;
		OW_THROWCIM(OW_CIMException::FAILED);
	}

};

//////////////////////////////////////////////////////////////////////////////

}


OW_PROVIDERFACTORY(InfiniteIP,infiniteip)


	
