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

#ifndef __OW_INSTANCEREPOSITORY_HPP__
#define __OW_INSTANCEREPOSITORY_HPP__

#include "OW_config.h"
#include "OW_GenericHDBRepository.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"

class OW_CIMServer;

class OW_InstanceRepository : public OW_GenericHDBRepository
{
public:

	/**
	 * Create a new OW_InstanceRepository object.
	 */
	OW_InstanceRepository(OW_CIMOMEnvironmentRef env)
		: OW_GenericHDBRepository(env) {}

	void getCIMInstances(
		const OW_String& ns,
		const OW_String& className, const OW_CIMClass& theClass,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList=NULL, OW_CIMServer* pServer = NULL,
		const OW_ACLInfo* pACLInfo=NULL);

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
		const OW_CIMClass& theClass);

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
	void createInstance(const OW_CIMObjectPath& cop, const OW_CIMClass& theClass,
		OW_CIMInstance& ci);

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
		const OW_CIMClass& theClass, const OW_CIMInstance& ci);

	/**
	 * Determines if an instance already exists
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param theClass The CIM class the instance belongs to
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	OW_Bool instanceExists(const OW_String& ns, const OW_CIMObjectPath& cop,
		const OW_CIMClass& theClass);

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
	 * @param nameComps	The names of the idividual containers where the 1st
	 * element in the array is the topmost container.
	 * @return 0 on success. Otherwise -1 if the bottom most container already
	 * exists.
	 */
	virtual int createNameSpace(const OW_StringArray& nameComps,
		OW_Bool rootCheck);

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

private:

	OW_String makeClassKey(const OW_String& ns, const OW_String& className);

	OW_String makeInstanceKey(const OW_String& ns, const OW_CIMObjectPath& cop,
		const OW_CIMClass& theClass);

	friend class OW_CIMServer;
};

#endif	// __OW_INSTANCEREPOSITORY_HPP__

