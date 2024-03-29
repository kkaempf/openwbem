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
 */

#ifndef OW_SECONDARY_INSTANCEPROVIDERIFC_HPP_INCLUDE_GUARD_
#define OW_SECONDARY_INSTANCEPROVIDERIFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_ProviderBaseIFC.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_WBEMFlags.hpp"

namespace OW_NAMESPACE
{

class OW_PROVIDER_API SecondaryInstanceProviderIFC: public ProviderBaseIFC
{
public:
	virtual ~SecondaryInstanceProviderIFC();
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
			const ProviderEnvironmentIFCRef& env,
			const blocxx::String& ns,
			const blocxx::String& className,
			CIMInstanceArray& instances,
			WBEMFlags::ELocalOnlyFlag localOnly,
			WBEMFlags::EDeepFlag deep,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const blocxx::StringArray* propertyList,
			const CIMClass& requestedClass,
			const CIMClass& cimClass ) = 0;
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
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
	virtual void createInstance(
			const ProviderEnvironmentIFCRef& env,
			const blocxx::String& ns,
			const CIMInstance& cimInstance ) = 0;
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
	virtual void modifyInstance(
			const ProviderEnvironmentIFCRef& env,
			const blocxx::String& ns,
			const CIMInstance& modifiedInstance,
			const CIMInstance& previousInstance,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			const blocxx::StringArray* propertyList,
			const CIMClass& theClass) = 0;
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
	virtual void deleteInstance(
			const ProviderEnvironmentIFCRef& env,
			const blocxx::String& ns,
			const CIMObjectPath& cop) = 0;
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

	virtual SecondaryInstanceProviderIFC* getSecondaryInstanceProvider();
};

} // end namespace OW_NAMESPACE

#endif
