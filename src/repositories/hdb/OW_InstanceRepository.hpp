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

#ifndef OW_INSTANCEREPOSITORY_HPP_INCLUDE_GUARD_
#define OW_INSTANCEREPOSITORY_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_GenericHDBRepository.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_WBEMFlags.hpp"

class OW_InstanceRepository : public OW_GenericHDBRepository
{
public:

	/**
	 * Create a new OW_InstanceRepository object.
	 */
	OW_InstanceRepository(OW_ServiceEnvironmentIFCRef env)
		: OW_GenericHDBRepository(env) {}

	void getCIMInstances(
		const OW_String& ns,
		const OW_String& className, 
		const OW_CIMClass& requestedClass,
		const OW_CIMClass& theClass,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_WBEMFlags::EDeepFlag deep, OW_WBEMFlags::ELocalOnlyFlag localOnly, OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList=NULL);

	void getInstanceNames(const OW_String& ns, const OW_CIMClass& theClass,
		OW_CIMObjectPathResultHandlerIFC& result);

	/**
	 * Retrieve a specific instance
	 *
	 * @param ns The namespace
	 * @param instanceName The OW_CIMObectPath that specifies the instance
	 * @param theClass The CIM class of the instance to retrieve
	 * @return An OW_CIMInstance object
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	OW_CIMInstance getCIMInstance(const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		const OW_CIMClass& theClass, OW_WBEMFlags::ELocalOnlyFlag localOnly,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList);

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Delete an existing instance from the store
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param theClass The CIM class of the instance to delete
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	void deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop,
		const OW_CIMClass& theClass);

	/**
	 * Creates a instance in the store
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param ci The instance that is to be stored with that object path
	 * @return The object path of the newly added CIMInstance.  This will be
	 *			cop + the keys from ci.
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	void createInstance(const OW_String& ns, const OW_CIMClass& theClass,
		const OW_CIMInstance& ci);

	/**
	 * Update an instance
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param theClass The class the cim instance belongs to.
	 * @param ci	The instance with the new values
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	void modifyInstance(const OW_String& ns, const OW_CIMObjectPath& cop,
		const OW_CIMClass& theClass, const OW_CIMInstance& ci,
		const OW_CIMInstance& oldInst,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const OW_StringArray* propertyList);

	/**
	 * Determin if a given class has instances.
	 * @param classPath	The object path for the class to check.
	 * @return true if the class specified has instances.
	 */
	OW_Bool classHasInstances(const OW_CIMObjectPath& classPath);

	/**
	 * Delete the given namespace and all object contained within it.
	 * @param nsName	The name of the namespace
	 */
	void deleteNameSpace(const OW_String& nsName);

	/**
	 * Create the necessary containers to make a valid path. Fail if the
	 * last container already exists.
	 * @param ns	The name of the namespace
	 * @return 0 on success. Otherwise -1 if the bottom most container already
	 * exists.
	 */
	virtual int createNameSpace(OW_String ns);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Ensure there is a container for a given class in the instance
	 * repository.
	 * @param ns	The namespace for the class
	 * @param cimClass	The class to create a container for
	 */
	void createClass(const OW_String& ns, const OW_CIMClass& cimClass);

	/**
	 * Remove the class name container for a given class.
	 * @param ns 			The namespace that contains the class
	 * @param className	The name of the class container to remove.
	 */
	void deleteClass(const OW_String& ns, const OW_String& className);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION


private:

	OW_String makeClassKey(const OW_String& ns, const OW_String& className);

	OW_String makeInstanceKey(const OW_String& ns, const OW_CIMObjectPath& cop,
		const OW_CIMClass& theClass);

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	void _removeDuplicatedQualifiers(OW_CIMInstance& inst, const OW_CIMClass& theClass);
#endif

};

#endif


