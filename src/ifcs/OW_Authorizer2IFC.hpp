/*******************************************************************************
* Copyright (C) 2003-2004 Novell, Inc. All rights reserved.
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
 * @author Bart Whiteley
 */

#ifndef __OW_AUTHORIZER2IFC_HPP__
#define __OW_AUTHORIZER2IFC_HPP__

#include "OW_config.h"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_SharedLibraryReference.hpp"

namespace OpenWBEM
{

class Authorizer2IFC
{
public:

	enum EWriteFlag { E_CREATE, E_MODIFY, E_DELETE };

	enum EAccessType { E_READ, E_WRITE, E_READWRITE };

	enum EDynamicFlag { E_NOT_DYNAMIC, E_DYNAMIC }; 

	virtual ~Authorizer2IFC();

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
		const ProviderEnvironmentIFCRef& env,
        const String& ns,
		const String& className,
		const StringArray* clientPropertyList,
		StringArray& authorizedPropertyList) = 0;


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
		const ProviderEnvironmentIFCRef& env,
		const String& ns, 
		const CIMObjectPath& instanceName, 
		EDynamicFlag dynamic,
		EWriteFlag flag) = 0;

#endif

	/**
	 * Determine if a read of the schema is allowed in the given namespace.
	 * @param env A reference to a provider environment
	 * @param ns The namespace the schema will be read from.
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowReadSchema(
		const ProviderEnvironmentIFCRef& env,
		const String& ns) = 0;

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Determine if a write of the schema in the given namespace is allowed.
	 * @param env A reference to a provider environment
	 * @param ns The namespace the schema write will take place in.
	 * @param flag Indicates create/modify/delete operation
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowWriteSchema(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		EWriteFlag flag) = 0;
#endif

	/**
	 * Determine if the user is allowed access to a namespace.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace that will be accessed.
	 * @param accessType EREAD, WRITE, EREADWRITE
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowAccessToNameSpace(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		EAccessType accessType) = 0;

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Determine if user is allowed to create the given namespace.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace that will be created.
	 * @return true if the creation is authorized. Otherwise false.
	 */
	virtual bool doAllowCreateNameSpace(
		const ProviderEnvironmentIFCRef& env,
		const String& ns) = 0;

	/**
	 * Determine if the user is allow to delete the given namespace.
	 * @param env A reference to a provider environment.
	 * @param ns The namespace that will be deleted.
	 * @return true if the deletion is authorized. Otherwise false.
	 */
	virtual bool doAllowDeleteNameSpace(
		const ProviderEnvironmentIFCRef& env,
		const String& ns) = 0;
#endif

	/**
	 * Determine if the user is allowed to enumerate namespaces.
	 * @param env A reference to a provider environment
	 * @return true if the enumerate is allowed. Otherwise false.
	 */
	virtual bool doAllowEnumNameSpace(
		const ProviderEnvironmentIFCRef& env) = 0;

	/**
	 * Determine if a method may be invoked. 
	 * @param env A reference to a provider environment.
	 * @param ns The namespace containing the instance or class. 
	 * @param path The name of the instance or class containing
	 * 		the method. 
	 * @param methodName The name of the method. 
	 * @return true if access is allowed. Otherwise false.
	 */
	virtual bool doAllowMethodInvocation(
		const ProviderEnvironmentIFCRef& env, 
		const String& ns, 
		const CIMObjectPath path, 
		const String& methodName) = 0;

	virtual void init(ProviderEnvironmentIFCRef&) {}
};

typedef SharedLibraryReference<Reference<Authorizer2IFC> > Authorizer2IFCRef;

}

#if !defined(OW_STATIC_SERVICES)
	#define OW_AUTHORIZER2_FACTORY(derived, authorizerName) \
	extern "C" OpenWBEM::Authorizer2IFC* \
	createAuthorizer2() \
	{ \
		return new derived; \
	} \
	extern "C" const char* \
	getOWVersion() \
	{ \
		return OW_VERSION; \
	}
#else
	#define OW_AUTHORIZER2_FACTORY(derived, authorizerName) \
	extern "C" OpenWBEM::Authorizer2IFC* \
	createAuthorizer2_##authorizerName() \
	{ \
		return new derived; \
	}
#endif /* !defined(OW_STATIC_SERVICES) */


#endif	// __OW_AUTHORIZER2IFC_HPP__

