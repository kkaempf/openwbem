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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_INSTANCEREPOSITORY_HPP_INCLUDE_GUARD_
#define OW_INSTANCEREPOSITORY_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_GenericHDBRepository.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_WBEMFlags.hpp"

namespace OpenWBEM
{

class InstanceRepository : public GenericHDBRepository
{
public:
	/**
	 * Create a new InstanceRepository object.
	 */
	InstanceRepository(ServiceEnvironmentIFCRef env)
		: GenericHDBRepository(env) {}
	void getCIMInstances(
		const String& ns,
		const String& className, 
		const CIMClass& requestedClass,
		const CIMClass& theClass,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList=NULL);
	void getInstanceNames(const String& ns, const CIMClass& theClass,
		CIMObjectPathResultHandlerIFC& result);
	/**
	 * Retrieve a specific instance
	 *
	 * @param ns The namespace
	 * @param instanceName The CIMObectPath that specifies the instance
	 * @param theClass The CIM class of the instance to retrieve
	 * @return An CIMInstance object
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	CIMInstance getCIMInstance(const String& ns,
		const CIMObjectPath& instanceName,
		const CIMClass& theClass, WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Delete an existing instance from the store
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param theClass The CIM class of the instance to delete
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	void deleteInstance(const String& ns, const CIMObjectPath& cop,
		const CIMClass& theClass);
	/**
	 * Creates a instance in the store
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param ci The instance that is to be stored with that object path
	 * @return The object path of the newly added CIMInstance.  This will be
	 *			cop + the keys from ci.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	void createInstance(const String& ns, const CIMClass& theClass,
		const CIMInstance& ci);
	/**
	 * Update an instance
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param theClass The class the cim instance belongs to.
	 * @param ci	The instance with the new values
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	void modifyInstance(const String& ns, const CIMObjectPath& cop,
		const CIMClass& theClass, const CIMInstance& ci,
		const CIMInstance& oldInst,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList);
	/**
	 * Determin if a given class has instances.
	 * @param classPath	The object path for the class to check.
	 * @return true if the class specified has instances.
	 */
	bool classHasInstances(const CIMObjectPath& classPath);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	/**
	 * Delete the given namespace and all object contained within it.
	 * @param nsName	The name of the namespace
	 */
	void deleteNameSpace(const String& nsName);
	/**
	 * Create the necessary containers to make a valid path. Fail if the
	 * last container already exists.
	 * @param ns	The name of the namespace
	 * @return 0 on success. Otherwise -1 if the bottom most container already
	 * exists.
	 */
	virtual int createNameSpace(String ns);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Ensure there is a container for a given class in the instance
	 * repository.
	 * @param ns	The namespace for the class
	 * @param cimClass	The class to create a container for
	 */
	void createClass(const String& ns, const CIMClass& cimClass);
	/**
	 * Remove the class name container for a given class.
	 * @param ns 			The namespace that contains the class
	 * @param className	The name of the class container to remove.
	 */
	void deleteClass(const String& ns, const String& className);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
private:
	String makeClassKey(const String& ns, const String& className);
	String makeInstanceKey(const String& ns, const CIMObjectPath& cop,
		const CIMClass& theClass);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	void _removeDuplicatedQualifiers(CIMInstance& inst, const CIMClass& theClass);
#endif
};

} // end namespace OpenWBEM

#endif
