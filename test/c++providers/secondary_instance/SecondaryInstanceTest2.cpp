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


#include "OW_config.h"

#include "OW_CppSecondaryInstanceProviderIFC.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Format.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"

using namespace OpenWBEM;
using namespace WBEMFlags;

// anonymous namespace to prevent symbol conflicts
namespace
{

class SecondaryInstanceTest2 : public CppSecondaryInstanceProviderIFC
{
public:
	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 */
	virtual void getSecondaryInstanceProviderInfo(SecondaryInstanceProviderInfo &info)
	{
		info.addInstrumentedClass("SecondaryInstanceTest");
	}

	/**
	 * This method is called when the instance specified by cimInstance is
	 * created.  The provider cannot override this operation, since it will
	 * have been already processed by the primary instance provider.
	 * Throwing an exception will cause the error to be returned to the
	 * CIM Client, but the work that other providers may have done will not be
	 * undone.
	 *
	 * @param env The provider's interface to it's environment.
	 * @param ns The namespace
	 * @param cimInstance The instance to be set.
	 */
	virtual void createInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &cimInstance)
	{
		env->getLogger()->logDebug(Format("SecondaryInstanceTest2::createInstance ns = %1, cimInstance = %2", ns, cimInstance));
	}
	
	/**
	 * This method is called when the instance specified by cop is
	 * deleted.  The provider cannot override this operation, since it will
	 * have been already processed by the primary instance provider.
	 * Throwing an exception will cause the error to be returned to the
	 * CIM Client, but the work that other providers may have done will not be
	 * undone.
	 *
	 * @param env The provider's interface to it's environment.
	 * @param ns The namespace
	 * @param cop The instance to be deleted
	 */
	virtual void deleteInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &cop)
	{
		env->getLogger()->logDebug(Format("SecondaryInstanceTest2::deleteInstance ns = %1, cop = %2", ns, cop));
	}
	
	/**
	 * This method allows the provider to modify instances generated by
	 * the enumInstances() or getInstance() member functions of an instance
	 * provider
	 *
	 * @param env The provider's interface to it's environment.
	 * @param ns The namespace
	 * @param className The name of the requested class.
	 * @param instances	The instances the primary instance provider produced.
	 *  This is an in/out parameter.  The provider can modify instances to change
	 *  what is returned to the CIM Client.
	 * @param localOnly The value the CIM Client specified in the request.
	 *  See CIM Operations over HTTP for a description of this parameter.
	 * @param deep The value the CIM Client specified in the request.
	 *  See CIM Operations over HTTP for a description of this parameter.
	 * @param includeQualifiers The value the CIM Client specified in the request.
	 *  See CIM Operations over HTTP for a description of this parameter.
	 * @param includeClassOrigin The value the CIM Client specified in the request.
	 *  See CIM Operations over HTTP for a description of this parameter.
	 * @param propertyList The value the CIM Client specified in the request.
	 *  See CIM Operations over HTTP for a description of this parameter.
	 * @param requestedClass The class the client specified when calling
	 *  EnumerateInstances.  For GetInstance(), requestedClass == cimClass.
	 * @param cimClass The class the provider is responsible for.
	 *
	 * @throws CIMException to indicate error.  The error will be returned to
	 *  the CIM Client.
	 */
	virtual void filterInstances(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const String &className, 
		CIMInstanceArray &instances, 
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray *propertyList, 
		const CIMClass &requestedClass, 
		const CIMClass &cimClass)
	{
		env->getLogger()->logDebug(Format("SecondaryInstanceTest2::filterInstances"
			" ns = %1, className = %2, localOnly = %3, deep = %4,"
			" includeQualifiers = %5, includeClassOrigin = %6,"
			" propertyList = %7, requestedClass = %8, cimClass = %9", 
			ns, className, localOnly, deep, includeQualifiers, 
			includeClassOrigin, propertyList, requestedClass, cimClass));
		env->getLogger()->logDebug(Format("SecondaryInstanceTest2::filterInstances"
			" instances.size() = %1", instances.size()));
		for (size_t i = 0; i < instances.size(); ++i)
		{
			env->getLogger()->logDebug(Format("SecondaryInstanceTest2::filterInstances"
				" instances[%1] = %2", i, instances[i]));

			instances[i].setProperty("prop4", CIMValue("from secondary prov 2"));
		}
	}
	/**
	 * This method is called when the instance specified by previousInstance is
	 * modified.  The provider cannot override this operation, since it will
	 * have been already processed by the primary instance provider.
	 * Throwing an exception will cause the error to be returned to the
	 * CIM Client, but the work that other providers may have done will not be
	 * undone.
	 *
	 * @param env The provider's interface to it's environment.
	 * @param ns The namespace
	 */
	virtual void modifyInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &modifiedInstance, const CIMInstance &previousInstance, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, const StringArray *propertyList, const CIMClass &theClass)
	{
		env->getLogger()->logDebug(Format("SecondaryInstanceTest2::modifyInstance ns = %1, modifiedInstance = %2, previousInstance = %3", ns, modifiedInstance, previousInstance));
	}
};

} // end anonymous namespace


OW_PROVIDERFACTORY(SecondaryInstanceTest2, secondaryinstancetest2)



