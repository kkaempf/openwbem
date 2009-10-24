/*******************************************************************************
* Copyright (C) 2001-2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Jon Carey
 */

#ifndef OW_SIMPLE_AUTHORIZER2_HPP_INCLUDE_GUARD_
#define OW_SIMPLE_AUTHORIZER2_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Authorizer2IFC.hpp"

namespace OW_NAMESPACE
{

// This class is responsible for Access control.
class SimpleAuthorizer2 : public Authorizer2IFC
{
public:
	/**
	 * Create a new SimpleAuthorization object.
	 */
	SimpleAuthorizer2();
	/**
	 * Destroy this SimpleAuthorization object.
	 */
	virtual ~SimpleAuthorizer2();

	/**
	 * Determine if a read of the given instance is allowed. The given
	 * objectPath could be a class path or an instance path.
	 * @param env A reference to a provider environment
	 * @param ns The namespace the instance will be read from
	 * @param className The class name of the instances that will be read.
	 * @param clientPropertyList This is the property list given by the client
	 * 		when requesting the instance(s). The client expects it to have the
	 * 		following meaning:
	 * 			If not NULL then it specifies the only properties that can be
	 *			returned in the instance. If not NULL but the array is empty,
	 * 			then no properties should be returned. If NULL then all
	 * 			properties will be returned.
	 * @param authorizedPropertyList This is the property list the authorizer
	 * 		will placed the authorized property names in. The return value of
	 * 		this method determines how this property list is interpreted. If
	 * 		this method returns NULL, then this propertyList will be iignored
	 * 		on returned. If a pointer to this string array is returned, then
	 * 		authorizedPropertyList contains the property names the client is
	 * 		allowed to retrieve. In this case, if the property list is empty,
	 * 		then the client will not get any properties.
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowReadInstance(
		const ServiceEnvironmentIFCRef& env,
        const String& ns,
		const String& className,
		const StringArray* clientPropertyList,
		StringArray& authorizedPropertyList,
		OperationContext& context);

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Determine if a write of the given instance is allowed.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace the instance will be written to.
	 * @param instanceName The name of the instance that will be
	 * 		created/modified/deleted.
	 * @param dynamic If E_DYNAMIC, then this instance is being written.
	 * 		through a provider. Otherwise it is being written to the
	 * 		static repository.
	 * @param flag Indicates create/modify/delete operation.
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowWriteInstance(
		const ServiceEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		EDynamicFlag dynamic,
		EWriteFlag flag,
		OperationContext& context);
#endif

	/**
	 * Determine if a read of the schema is allowed in the given namespace.
	 * @param env A reference to a provider environment
	 * @param ns The namespace the schema will be read from.
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowReadSchema(
		const ServiceEnvironmentIFCRef& env,
		const String& ns,
		OperationContext& context);

#if !defined(OW_DISABLE_SCHEMA_MANIPULATION) || !defined(OW_DISABLE_QUALIFIER_DECLARATION)

	/**
	 * Determine if a write of the schema in the given namespace is allowed.
	 * @param env A reference to a provider environment
	 * @param ns The namespace the schema write will take place in.
	 * @param flag Indicates create/modify/delete operation
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowWriteSchema(
		const ServiceEnvironmentIFCRef& env,
		const String& ns,
		EWriteFlag flag,
		OperationContext& context);

#endif

	/**
	 * Determine if the user is allowed access to a namespace.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace that will be accessed.
	 * @param actype - See Authorizer2IFC
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowAccessToNameSpace(
		const ServiceEnvironmentIFCRef& env,
		const String& ns,
		Authorizer2IFC::EAccessType actype,
		OperationContext& context);

#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	/**
	 * Determine if user is allowed to create the given namespace.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace that will be created.
	 * @return true if the creation is authorized. Otherwise false.
	 */
	virtual bool doAllowCreateNameSpace(
		const ServiceEnvironmentIFCRef& env,
		const String& ns,
		OperationContext& context);

	/**
	 * Determine if the user is allow to delete the given namespace.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace that will be deleted.
	 * @return true if the deletion is authorized. Otherwise false.
	 */
	virtual bool doAllowDeleteNameSpace(
		const ServiceEnvironmentIFCRef& env,
		const String& ns,
		OperationContext& context);
#endif

	/**
	 * Determine if the user is allowed to enumerate namespaces.
	 * @param env A reference to a provider environment
	 * @return true if the enumerate is allowed. Otherwise false.
	 */
	virtual bool doAllowEnumNameSpace(
		const ServiceEnvironmentIFCRef& env,
		OperationContext& context);

	/**
	 * Determine if a method may be invoked.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace containing the instance or class.
	 * @param path The name of the instance or class containing
	 * 		the method.
	 * @param MethodName The name of the method.
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowMethodInvocation(
		const ServiceEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& MethodName,
		OperationContext& context);

private:

	bool checkAccess(const String& opType, const String& ns,
		const ServiceEnvironmentIFCRef& env,
		OperationContext& context);
};

} // end namespace OW_NAMESPACE

#endif
