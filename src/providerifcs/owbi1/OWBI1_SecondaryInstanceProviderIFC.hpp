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

#ifndef OWBI1_SECONDARY_INSTANCEPROVIDERIFC_HPP_INCLUDE_GUARD_
#define OWBI1_SECONDARY_INSTANCEPROVIDERIFC_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_ProviderBaseIFC.hpp"
#include "OWBI1_ProviderFwd.hpp"
#include "OWBI1_WBEMFlags.hpp"
#include "OWBI1_CIMFwd.hpp"

namespace OWBI1
{

class OWBI1_OWBI1PROVIFC_API BI1SecondaryInstanceProviderIFC: public virtual BI1ProviderBaseIFC
{
public:
	virtual ~BI1SecondaryInstanceProviderIFC();
	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 */
	virtual void getSecondaryInstanceProviderInfo(SecondaryInstanceProviderInfo& info) = 0;

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
			const String& ns,
			const CIMName& className,
			CIMInstanceArray& instances,
			const CIMPropertyList& propertyList,
			const CIMClass& requestedClass,
			const CIMClass& cimClass );
	
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
			const String& ns,
			const CIMInstance& cimInstance );
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
			const String& ns,
			const CIMInstance& modifiedInstance,
			const CIMInstance& previousInstance,
			const CIMPropertyList& propertyList,
			const CIMClass& theClass);
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
			const String& ns,
			const CIMObjectPath& cop);
	
	virtual BI1SecondaryInstanceProviderIFC* getSecondaryInstanceProvider();

};

} // end namespace OWBI1

#endif
