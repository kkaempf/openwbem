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

#ifndef _OW_WQLFILTERREP_HPP__
#define _OW_WQLFILTERREP_HPP__

#include "OW_config.h"
#include "OW_RepositoryIFC.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMInstance.hpp"

class OW_CIMServer;

class OW_WQLFilterRep : public OW_RepositoryIFC
{
public:

	OW_WQLFilterRep(const OW_CIMInstance& inst, OW_CIMServer* cimServer);

	/**
	 * Open this OW_RepositoryIFC.
	 * @exception OW_IOException
	 */
	virtual void open(const OW_String &/*path*/) {}

	/**
	 * Close this OW_GenericHDBRepository.
	 */
	virtual void close() {}

	/**
	 * @return A reference to the OW_CIMOMEvironment this object is using.
	 */
	virtual OW_CIMOMEnvironmentRef getEnvironment() const;

	/**
	 * Get an existing qualifier type from the repository.
	 * @param objPath	The object path for the qaulifer type.
	 * @param aclInfo ACL object describing user making request.
	 * @return A valid OW_CIMQaulifer type on success. Otherwise a NULL
	 * OW_CIMQualifier type.
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	virtual OW_CIMQualifierType getQualifierType(
		const OW_CIMObjectPath &objPath, const OW_ACLInfo& aclInfo);

	/**
	 * Gets a list of the namespaces within the namespace specified by the CIM
	 * object path.
	 * @param ns The parent namespace to enumerate.
	 * @param deep	If set to OW_CIMClient::DEEP, the enumeration returned will
	 *					contain the entire hierarchy of namespaces present
	 *             under the enumerated namespace. If set to
	 *					OW_CIMClient::SHALLOW  the enuermation will return only the
	 *					first level children of the enumerated namespace.
	 * @param aclInfo ACL object describing user making request.
	 * @return An Array of namespace names as strings.
	 * @exception 	OW_CIMException If the namespace does not exist or the object
	 *					cannot be found in the specified namespace.
	 */
	virtual void enumNameSpace(const OW_String& ns,
		OW_StringResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo& aclInfo);

	/**
	 * Create a cim namespace.
	 * @param ns 	The namespace to be created.
	 * @exception OW_CIMException If the namespace already exists.
	 */
	virtual void createNameSpace(const OW_String& ns,
		const OW_ACLInfo& aclInfo);

	/**
	 * Delete a specified namespace.
	 * @param ns	The namespace to delete.
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException If the namespace does not exist.
	 */
	virtual void deleteNameSpace(const OW_String& ns,
		const OW_ACLInfo& aclInfo);

	/**
	 * Updates the specified CIM qualifier type in the specified namespace.
	 * @param name	Am OW_CIMObjectPath that identifies the CIM qualifier type.
	 * @param qt	The CIM qualifier type to be updated.
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException If the qualifier type cannot be found or the
	 *										user does not have write permission to the
	 *										namespace.
	 */
	virtual void setQualifierType(const OW_CIMObjectPath &name,
		const OW_CIMQualifierType &qt, const OW_ACLInfo& aclInfo);

	/**
	 * Enumerate the qualifier types in a name space.
	 * @param path	The object path to enumeration the qualifiers in.
	 * @param aclInfo ACL object describing user making request.
	 * @return An enumeration of OW_CIMQualifierTypes
	 * @exception	OW_CIMException
	 */
	virtual void enumQualifierTypes(
		const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result, const OW_ACLInfo& aclInfo);

	/**
	 * Delete an existing qualifier type from the repository
	 * @param objPath	The object path fro the qualifer type.
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException
	 */
	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName,
		const OW_ACLInfo& aclInfo);

	/**
	 * creates a class in the store
	 *
	 * @param path The path for the class to create
	 * @param cimClass The class to create
	 * @param aclInfo ACL object describing user making request.
	 * @exception 	CIMException if the class already exists, or parent class
	 *					is not yet on file.
	 * @exception OW_HDBException An error occurred in the database.
	 * @exception OW_IOException Couldn't write class object to file.
	 */
	virtual void createClass(const OW_CIMObjectPath &path, OW_CIMClass &cimClass,
		const OW_ACLInfo& aclInfo);

	/**
	 * set a class in the store - note children are not affected
	 *
	 * @param path The path for the class to create
	 * @param cimClass The class to create
	 * @param aclInfo ACL object describing user making request.
	 * @return an OW_CIMClass representing the state of the class prior to
	 * 	the update.  This is likely usefull only for creating
	 *		CIM_ClassModification indications.
	 * @exception CIMException if the class already exists
	 */
	virtual OW_CIMClass modifyClass(const OW_CIMObjectPath &name,
		OW_CIMClass &cc, const OW_ACLInfo& aclInfo);

	/**
	 * Gets an existing class from a store
	 * @param path The path for the class to retrieve
	 * @param localOnly If true, then only CIM elements (properties, methods,
	 *		qualifiers) overriden within the definition are returned.
	 * @param includeQualifiers If true, then all qualifiers for the class
	 *		(including properties, methods, and method parameters) are returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be returned on all appropriate components.
	 * @param propertyList If not NULL then is specifies the only properties
	 *		that can be returned with the class. If not NULL but the array is
	 *		empty, then no properties should be returned. If NULL then all
	 *		properties will be returned.
	 * @param aclInfo ACL object describing user making request.
	 * @return A valid OW_CIMClass object on success. Otherwise a NULL
	 * OW_CIMClass object.
	 * @exception OW_CIMException
	 * @exception OW_HDBException An error occurred in the database.
	 * @exception OW_IOException Couldn't read class object from file.
	 */
	virtual OW_CIMClass getClass(
		const OW_String& ns,
		const OW_String& className,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	/**
	 * Delete an existing class from the store
	 *
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete
	 * @param aclInfo ACL object describing user making request.
	 * @return A OW_CIMClass representing the class which was deleted.
	 *		This is likely usefull only for creating CIM_ClassCreation incidations.
	 * @exception CIMException if class does not exist
	 */
	virtual OW_CIMClass deleteClass(const OW_String& ns, const OW_String& className,
		const OW_ACLInfo& aclInfo);

	/**
	 * Retrieve a specific instance
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param localOnly If true, only return non-derived properties.
	 * @param includeQualifiers If true, then all qualifiers for the instance
	 *		(including properties, methods, and method parameters) are returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be returned on all appropriate components.
	 * @param propertyList If not NULL then is specifies the only properties
	 *		that can be returned with the class. If not NULL but the array is
	 *		empty, then no properties should be returned. If NULL then all
	 *		properties will be returned.
	 * @param aclInfo ACL object describing user making request.
	 * @return An OW_CIMInstance object
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	/**
	 * Delete an existing instance from the store
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 * @return an OW_CIMInstance representing the Instance just deleted.
	 *		This is likely usefull only for creating CIM_InstDeletion indications;
	 */
	virtual OW_CIMInstance deleteInstance(const OW_String& ns, const OW_CIMObjectPath &cop,
		const OW_ACLInfo& aclInfo);

	/**
	 * Enumerates the class specified by the OW_CIMObjectPath.
	 * @param path The OW_CIMObjectPath identifying the class to be enumerated.
	 * @param deep If set to OW_CIMClient::DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to OW_CIMClient::SHALLOW the	enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If true, then only CIM elements (properties, methods,
	 *		qualifiers) overriden within the definition are returned.
	 * @param includeQualifiers If true, then all qualifiers for the class
	 *		(including properties, methods, and method parameters) are returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be returned on all appropriate components.
	 * @param aclInfo ACL object describing user making request.
	 * @return An enumeration of OW_CIMClass objects (OW_CIMClassEnumeration)
	 * @exception OW_CIMException  	If the specified CIMObjectPath object
	 *											cannot be foundl
	 */
	virtual void enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo& aclInfo);

	/**
	 * Enumerates the class specified by the OW_CIMObjectPath.
	 * @param path		The OW_CIMObjectPath identifying the class to be
	 *						enumerated.
	 * @param deep		If set to OW_CIMClient::DEEP, the enumeration returned will
	 *						contain the names of all classes derived from the
	 *						enumerated class. If set to OW_CIMClient::SHALLOW the
	 *						enumermation will return only the names of the first level
	 *						children of the enumerated class.
	 * @param aclInfo ACL object describing user making request.
	 * @return An enumeration of OW_CIMClass objects (OW_CIMClassEnumeration)
	 * @exception OW_CIMException  	If the specified CIMObjectPath object
	 *											cannot be foundl
	 */
	virtual void enumClassNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo& aclInfo);

	/**
	 * Retrieve an enumeration of instances (OW_CIMInstance) for a particular
	 * class
	 *
	 * @param path The OW_CIMObjectPath identifying the class whose	instances
	 *		are to be enumerated.
	 * @param deep If set to OW_CIMClient::DEEP, the enumeration returned will
	 *		contain the names of all instances of the specified class and all
	 *		classes derived from it. If set to OW_CIMClient::SHALLOW only names
	 *		of instances belonging to the specified class are returned.
	 * @param localOnly	If true only non-inherited properties/qualifiers are
	 *							included in the instances.
	 * @param includeQualifiers If true, then all qualifiers for the instance
	 *		(including properties, methods, and method parameters) are returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be returned on all appropriate components.
	 * @param propertyList If not NULL then is specifies the only properties
	 *		that can be returned with the class. If not NULL but the array is
	 *		empty, then no properties should be returned. If NULL then all
	 *		properties will be returned.
	 * @param aclInfo ACL object describing user making request.
	 * @return An OW_CIMInstanceEnumeration object.
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	virtual void enumInstances(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo);

	/**
	 * Retrieve an enumeration of instances (OW_CIMInstance) for a particular
	 * class
	 *
	 * @param path			The OW_CIMObjectPath identifying the class whose
	 *							instances are to be enumerated.
	 * @param deep			If set to OW_CIMClient::DEEP, the enumeration returned
	 *							will contain the names of all instances of the specified
	 *                	class and all classes derived from it. If set to
	 *							OW_CIMClient::SHALLOW only names of instances belonging
	 *							to the specified class are returned.
	 * @param aclInfo ACL object describing user making request.
	 * @return An OW_CIMInstanceEnumeration object.
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	virtual void enumInstanceNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo& aclInfo);

	/**
	 * Creates a instance in the store
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param ci The instance that is to be stored with that object path
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	virtual OW_CIMObjectPath createInstance(const OW_CIMObjectPath &cop, OW_CIMInstance &ci,
		const OW_ACLInfo& aclInfo);

	/**
	 * Update an instance
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param ci	The instance with the new values
	 * @param aclInfo ACL object describing user making request.
	 * @return a OW_CIMInstance representing the state of the instance prior
	 * 	to the update.  This is likely usefull only for creating
	 *		CIM_InstModification indications.
	 * @exception OW_HDBException
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	virtual OW_CIMInstance modifyInstance(const OW_CIMObjectPath &cop,
		OW_CIMInstance &ci, const OW_ACLInfo& aclInfo);

	virtual void associators(const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo& aclInfo);

	virtual void associatorsClasses(const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo& aclInfo);

	/**
	 * Set a property value on an OW_CIMInstance.
	 * @param name				The object path of the instance
	 * @param propertyName	The name of the property to update
	 * @param cv				The value to set the property to.
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException
	 */
	virtual void setProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_CIMValue &cv,
		const OW_ACLInfo& aclInfo);

	virtual void referenceNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &resultClass,
		const OW_String &role, const OW_ACLInfo& aclInfo);

	/**
	 * Get the specified CIM instance property.
	 * @param name				An OW_CIMObjectPath that identifies the CIM instance
	 *								to be accessed
	 * @param propertyName	The name of the property to retrieve.
	 * @param aclInfo ACL object describing user making request.
	 * @return The OW_CIMvalue for property identified by propertyName.
	 * @exception OW_CIMException
	 */
	virtual OW_CIMValue getProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_ACLInfo& aclInfo);

	/**
	 * Invokes a method
	 *
	 * @param name	The object path of the object on which the method is being
	 *					invoked this can be a class or instance
	 * @param methodName	The name of the method to invoke.
	 * @param inParams An array of OW_CIMProperty objects for inbound values
	 * @param outParams An array of OW_CIMProperty objects for inbound values
	 * @param outParams A vector of CIMProperty for outbound values
	 * @param aclInfo ACL object describing user making request.
	 * @return The return from the extrinsic method as a OW_CIMValue.
	 * @exception OW_CIMException
	 */
	virtual OW_CIMValue invokeMethod(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String &methodName, const OW_CIMParamValueArray &inParams,
		OW_CIMParamValueArray &outParams, const OW_ACLInfo& aclInfo);

	virtual void associatorNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &assocClass,
		const OW_String &resultClass, const OW_String &role,
		const OW_String &resultRole, const OW_ACLInfo& aclInfo);

	virtual void references(const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo& aclInfo);

	virtual void referencesClasses(const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray *propertyList, const OW_ACLInfo& aclInfo);

	virtual void execQuery(const OW_CIMNameSpace& ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &query, const OW_String &queryLanguage,
		const OW_ACLInfo& aclInfo);

	virtual OW_ReadLock getReadLock();
	virtual OW_WriteLock getWriteLock();

private:
	OW_CIMInstance m_inst;
	OW_CIMServer* m_pCIMServer;
};


#endif //_OW_WQLFILTERREP_HPP__
