/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_CPP_INSTANCEPROVIDERIFC_HPP_
#define OW_CPP_INSTANCEPROVIDERIFC_HPP_

#include "OW_config.h"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_InstanceProviderInfo.hpp"

/**
 * Classes wishing to implement an instance provider must derive from this
 * class.
 * All calls to the derived provider will be serialized so that providers need
 * not worry about multi-threading issues.
 */
class OW_CppInstanceProviderIFC: public virtual OW_CppProviderBaseIFC
{
public:
	virtual ~OW_CppInstanceProviderIFC();

	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 * If the method does nothing, then the provider's class must have a
	 * provider qualifier that identifies the provider.  This old behavior is
	 * deprecated and will be removed sometime in the future.  This method
	 * has a default implementation that does nothing, to allow old providers
	 * to be migrated forward with little or no change, but once the old
	 * provider location method is removed, this member function will be pure
	 * virtual.
	 */
	virtual void getProviderInfo(OW_InstanceProviderInfo& info)
	{
		(void)info;
	}

	/**
	 * This method enumerates all names of instances of the class which is
	 * specified in cop.
	 *
	 * @param cop The object path specifies the class that must be enumerated.
	 * @param cimClass The class reference
	 *
	 * @returns An array of OW_CIMObjectPath containing names of the
	 * 	enumerated instances.
	 * @throws OW_CIMException - throws if the CIMObjectPath is incorrect
	 * 	or does not exist.
	 */
	virtual void enumInstanceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_String& className,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_CIMClass& cimClass ) = 0;

	/**
	 * This method enumerates
	 * all instances of the class which is specified in className.  The entire
	 * instances and not just the names are returned.  
	 *
	 * @param cop The object path specifies the class that must be
	 * 	enumerated.
	 *
	 * @param cimClass The class reference.
	 *
	 * @param localOnly If true, only the non-inherited properties are to be
	 * 	returned, otherwise all properties are required.
	 *
	 * @returns An array of OW_CIMInstance containing names of the enumerated
	 * 	instances.
	 *
	 * @throws OW_CIMException - thrown if cop is incorrect or does not exist.
	 */
	virtual void enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_String& className,
			OW_CIMInstanceResultHandlerIFC& result,
			OW_Bool localOnly, 
			OW_Bool deep, 
			OW_Bool includeQualifiers, 
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList,
			const OW_CIMClass& requestedClass,
			const OW_CIMClass& cimClass ) = 0;

	/**
	 * This method retrieves the instance specified in the object path.
	 *
	 * @param cop The name of the instance to be retrieved.
	 *
	 * @param cimClass The class to which the instance belongs.  This is
	 * 	useful for providers which do not want to create instances from
	 * 	scratch.  They can call the class newInstance() routine to create
	 * 	a template for the new instance.
	 *
	 * @param localOnly If true, only the non-inherited properties are to be
	 * 	returned, otherwise all properties are required.
	 *
	 * @returns The retrieved instance
	 *
	 * @throws OW_CIMException - thrown if cop is incorrect or does not exist
	 */
	virtual OW_CIMInstance getInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& instanceName,
			OW_Bool localOnly,
			OW_Bool includeQualifiers, 
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList, 
			const OW_CIMClass& cimClass ) = 0;

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * This method creates the instance specified in the object path.  If the
	 * instance does exist an OW_CIMException with ID CIM_ERR_ALREADY_EXISTS
	 * must be thrown.  The parameter should be the instance name.
	 *
	 * @param cop The path to the instance to be set.  The import part in
	 * 	this parameter is the namespace component.
	 *
	 * @param cimInstance The instance to be set
	 *
	 * @returns A CIM ObjectPath of the instance that was created.
	 *
	 * @throws OW_CIMException
	 */
	virtual OW_CIMObjectPath createInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMInstance& cimInstance ) = 0;

	/**
	 * This method sets the instance specified in the object path.  If the
	 * instance does not exist an OW_CIMException with ID CIM_ERR_NOT_FOUND
	 * must be thrown.  The parameter should be the instance name.
	 *
	 * @param cop The path of the instance to be set.  The important part in
	 * 	this parameter is the namespace component.
	 *
	 * @param cimInstance The instance to be set.
	 *
	 * @throws OW_CIMException
	 */
	virtual void modifyInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMInstance& modifiedInstance,
			const OW_CIMInstance& previousInstance,
			OW_Bool includeQualifiers,
			const OW_StringArray* propertyList,
			const OW_CIMClass& theClass) = 0;

	/**
	 * This method deletes the instance specified in the object path
	 *
	 * @param cop The instance to be deleted
	 *
	 * @throws OW_CIMException
	 */
	virtual void deleteInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& cop) = 0;
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

	virtual OW_CppInstanceProviderIFC* getInstanceProvider() { return this; }
};

typedef OW_SharedLibraryReference<OW_CppInstanceProviderIFC> OW_CppInstanceProviderIFCRef;

#endif

