/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */



#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"

using namespace OpenWBEM;
using namespace WBEMFlags;

namespace{

class InfiniteIP : public CppInstanceProviderIFC
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
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		(void)cimClass;
		(void)env;

		UInt64 count = 0;
		for (;;++count)
		{
			CIMObjectPath newCop(className, ns);
			newCop.setKeyValue(String("InstanceNumber"), CIMValue(count));
			result.handle(newCop);
		}
	}


//////////////////////////////////////////////////////////////////////////////
	void
		enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass )
	{
		(void)env;
		(void)ns;
		(void)className;

		UInt64 count = 0;
		CIMInstance newInst = cimClass.newInstance();
		for (;;++count)
		{
			newInst.setProperty(String("InstanceNumber"), CIMValue(count));
			result.handle(newInst.clone(localOnly,deep,includeQualifiers,includeClassOrigin,propertyList,requestedClass,cimClass));
		}

	}

//////////////////////////////////////////////////////////////////////////////
	CIMInstance
		getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, 
		const CIMClass& cimClass )
	{
		(void)env; (void)ns; (void)instanceName; (void)localOnly; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)cimClass;
		OW_THROWCIM(CIMException::FAILED);

	}






//////////////////////////////////////////////////////////////////////////////
	CIMObjectPath
		createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		(void)env;
		(void)ns;
		(void)cimInstance;
		OW_THROWCIM(CIMException::FAILED);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		(void)env;
		(void)ns;
		(void)modifiedInstance;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
		OW_THROWCIM(CIMException::FAILED);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		(void)cop;
		OW_THROWCIM(CIMException::FAILED);
	}

};

//////////////////////////////////////////////////////////////////////////////

}


OW_PROVIDERFACTORY(InfiniteIP,infiniteip)


	
