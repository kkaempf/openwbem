/*******************************************************************************
* Copyright (C) 2002 Center 7, Inc All rights reserved.
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

#ifndef OW_CIM_REPOSITORY_HPP_INCLUDE_GUARD_
#define OW_CIM_REPOSITORY_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_RepositoryIFC.hpp"
#include "OW_CIMOMEnvironment.hpp"

#include "OW_MetaRepository.hpp"
#include "OW_InstanceRepository.hpp"
#include "OW_AssocDb.hpp"

// This class is responsible for storing/retrieving the static CIM data.
class OW_CIMRepository : public OW_RepositoryIFC
{
public:

	static const char* const INST_REPOS_NAME;
	static const char* const META_REPOS_NAME;
	static const char* const NS_REPOS_NAME;
	static const char* const CLASS_ASSOC_REPOS_NAME;
	static const char* const INST_ASSOC_REPOS_NAME;

	/**
	 * Create a new OW_CIMRepository object.
	 */
	OW_CIMRepository(OW_CIMOMEnvironmentRef env);

	/**
	 * Destroy this OW_CIMRepository object.
	 */
	virtual ~OW_CIMRepository();

	/**
	 * Open
	 * @exception OW_IOException
	 */
	virtual void open(const OW_String& path);

	virtual void close();

	/**
	 * Create a cim namespace.
	 * @param ns 	The namespace to be created.
	 * @param aclInfo ignored
	 * @exception OW_CIMException If the namespace already exists.
	 */
	virtual void createNameSpace(const OW_String& ns, const OW_ACLInfo& aclInfo);

	/**
	 * Delete a specified namespace.
	 * @param ns	The namespace to delete.
	 * @param aclInfo ignored
	 * @exception OW_CIMException If the namespace does not exist.
	 */
	virtual void deleteNameSpace(const OW_String& ns, const OW_ACLInfo& aclInfo);

	/**
	 * Get an existing qualifier type from the repository.
	 * @param ns The namespace of the qualifier type
	 * @param qualifierName	The name of the qualifier type.
	 * @param aclInfo ignored
	 * @return A valid OW_CIMQaulifer type.
	 * @exception OW_CIMException
	 * @exception OW_IOException
	 */
	virtual OW_CIMQualifierType getQualifierType(const OW_String& ns,
		const OW_String& qualifierName,
		const OW_ACLInfo& aclInfo);

	/**
	 * Gets a list of the namespaces
	 * @param aclInfo ignored
	 * @param result result.handle() will be called for each namespace.
	 * @exception 	OW_CIMException If the namespace does not exist or the object
	 *					cannot be found in the specified namespace.
	 */
	virtual void enumNameSpace(OW_StringResultHandlerIFC& result,
		const OW_ACLInfo& aclInfo);

	/**
	 * Enumerate the qualifier types in a name space.
	 * @param ns	The namespace to enumerate the qualifiers in.
	 * @param aclInfo ignored
	 * @param result result.handle() will be called for each qualifier type.
	 * @exception	OW_CIMException
	 */
	virtual void enumQualifierTypes(
		const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result,
		const OW_ACLInfo& aclInfo);

	/**
	 * Delete an existing qualifier type from the repository
	 * @param ns	The name space.
	 * @param qualName The qualifier type name
	 * @param aclInfo ignored
	 * @exception OW_CIMException
	 */
	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName,
		const OW_ACLInfo& aclInfo);

	/**
	 * Updates the specified CIM qualifier type in the specified namespace.
	 * @param name	Am OW_CIMObjectPath that identifies the CIM qualifier type.
	 * @param qt	The CIM qualifier type to be updated.
	 * @param aclInfo ignored
	 * @exception OW_CIMException If the qualifier type cannot be found or the
	 *										user does not have write permission to the
	 *										namespace.
	 */
	virtual void setQualifierType(const OW_String& ns,
		const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo);

	/**
	 * Gets an existing class from a store
	 * @param path The path for the class to retrieve
	 * @param localOnly ignored
	 * @param includeQualifiers ignored
	 * @param includeClassOrigin ignored
	 * @param propertyList ignored
	 * @param aclInfo ignored
	 * @return A valid OW_CIMClass object on success.  A NULL OW_CIMClass
	 *    will never be returned.  An exception will be thrown instead.
	 * OW_CIMClass object.
	 * @exception OW_CIMException
	 *		CIM_ERR_FAILED
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
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
	 * @return an OW_CIMClass representing the class which was deleted.
	 * @exception CIMException
	 *		CIM_ERR_CLASS_HAS_CHILDREN
	 *		CIM_ERR_CLASS_HAS_INSTANCES
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_FOUND
  	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_FAILED
	 */
	virtual OW_CIMClass deleteClass(const OW_String& ns, const OW_String& className,
		const OW_ACLInfo& aclInfo);

	/**
	 * creates a class in the store
	 *
	 * @param path The path for the class to create
	 * @param cimClass The class to create
	 * @param aclInfo ACL object describing user making request.
	 * @exception 	CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_ALREADY_EXISTS
	 *		CIM_ERR_INVALID_SUPERCLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void createClass(const OW_String& ns, const OW_CIMClass& cimClass,
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
	 * @exception CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_INVALID_SUPERCLASS
	 *		CIM_ERR_CLASS_HAS_CHILDREN
	 *		CIM_ERR_CLASS_HAS_INSTANCES
	 *		CIM_ERR_FAILED
	 */
	virtual OW_CIMClass modifyClass(
		const OW_String& ns,
		const OW_CIMClass& cc,
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
	 * @exception OW_CIMException  	
 	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
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
	 * @param localOnly Only include properties
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException  	
 	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void enumClassNames(const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo& aclInfo);

	/**
	 * Retrieve an enumeration of instances (OW_CIMInstance) for a particular
	 * class
	 *
	 * @param path The OW_CIMObjectPath identifying the class whose	instances
	 *		are to be enumerated.
	 * @param deep Return properties defined on subclasses of the class in path
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
	 * @exception OW_CIMException
 	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void enumInstances(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, OW_Bool enumSubClasses, 
		const OW_ACLInfo& aclInfo);

	/**
	 * Retrieve an enumeration of instances object paths (OW_CIMObjectPath)
	 * for a particular class
	 *
	 * @param path			The OW_CIMObjectPath identifying the class whose
	 *							instances are to be enumerated.
	 * @param deep			If set to OW_CIMClient::DEEP, the enumeration returned
	 *							will contain the names of all instances of the specified
	 *                	class and all classes derived from it. If set to
	 *							OW_CIMClient::SHALLOW only names of instances belonging
	 *							to the specified class are returned.
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException
 	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void enumInstanceNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep, const OW_ACLInfo& aclInfo);

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
	 * @exception OW_CIMException
	 *		CIM_ERR_FAILED
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_INVALID_PARAMETER
	 */
	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		OW_CIMClass* pOutClass, const OW_ACLInfo& aclInfo);

	/**
	 * Delete an existing instance from the store
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual OW_CIMInstance deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop,
		const OW_ACLInfo& aclInfo);

	/**
	 * Creates a instance in the store
	 *
	 * @param cop	The OW_CIMObectPath that specifies the instance
	 * @param ci The instance that is to be stored with that object path
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_ALREADY_EXISTS
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual OW_CIMObjectPath createInstance(const OW_String& ns,
		const OW_CIMInstance& ci,
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
	 * @exception OW_CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_FAILED
	 */
	virtual OW_CIMInstance modifyInstance(
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		OW_Bool includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	/**
	 * Set a property value on an OW_CIMInstance.
	 * @param name				The object path of the instance
	 * @param propertyName	The name of the property to update
	 * @param cv				The value to set the property to.
	 * @param aclInfo ACL object describing user making request.
	 * @exception OW_CIMException
	 */
	virtual void setProperty(
		const OW_String& ns,
		const OW_CIMObjectPath& name, const OW_String& propertyName,
		const OW_CIMValue& cv, const OW_ACLInfo& aclInfo);

	/**
	 * Get the specified CIM instance property.
	 * @param name				An OW_CIMObjectPath that identifies the CIM instance
	 *								to be accessed
	 * @param propertyName	The name of the property to retrieve.
	 * @param aclInfo ACL object describing user making request.
	 * @return The OW_CIMvalue for property identified by propertyName.
	 * @exception OW_CIMException
	 */
	virtual OW_CIMValue getProperty(
		const OW_String& ns,
		const OW_CIMObjectPath& name,
		const OW_String& propertyName, const OW_ACLInfo& aclInfo);

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
	 *
	 * @exception OW_CIMException
	 */
	virtual OW_CIMValue invokeMethod(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& methodName, const OW_CIMParamValueArray& inParams,
		OW_CIMParamValueArray& outParams, const OW_ACLInfo& aclInfo);

	virtual void associatorNames(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole, const OW_ACLInfo& aclInfo);

	virtual void associators(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole,  OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	virtual void associatorsClasses(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String& assocClass,
		const OW_String& resultClass, const OW_String& role,
		const OW_String& resultRole,  OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	virtual void referenceNames(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role, const OW_ACLInfo& aclInfo);

	virtual void references(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);
	
	virtual void referencesClasses(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);
	
	/**
	 *
	 * @exception OW_CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_QUERY
	 *		CIM_ERR_FAILED
	 *
	 */
	virtual void execQuery(
		const OW_String& ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &query, const OW_String& queryLanguage,
		const OW_ACLInfo& aclInfo);

	virtual void getSchemaReadLock() {}
	virtual void getSchemaWriteLock() {}
	virtual void releaseSchemaReadLock() {}
	virtual void releaseSchemaWriteLock() {}
	virtual void getInstanceReadLock() {}
	virtual void getInstanceWriteLock() {}
	virtual void releaseInstanceReadLock() {}
	virtual void releaseInstanceWriteLock() {}

	OW_CIMOMEnvironmentRef getEnvironment() const { return m_env; }

private:

	OW_CIMClass _getClass(const OW_String& ns, const OW_String& className);

	OW_CIMClass _instGetClass(const OW_String& ns, const OW_String& className);

	void checkGetClassRvalAndThrow(OW_CIMException::ErrNoType rval,
		const OW_String& ns, const OW_String& className);

	void checkGetClassRvalAndThrowInst(OW_CIMException::ErrNoType rval,
		const OW_String& ns, const OW_String& className);

	void _commonAssociators(
		const OW_String& ns,
		const OW_CIMObjectPath& path_,
		const OW_String& assocClassName, const OW_String& resultClass,
		const OW_String& role, const OW_String& resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList,
		OW_CIMInstanceResultHandlerIFC* piresult,
		OW_CIMObjectPathResultHandlerIFC* popresult,
		OW_CIMClassResultHandlerIFC* pcresult,
		const OW_ACLInfo& aclInfo);

	void _commonReferences(
		const OW_String& ns,
		const OW_CIMObjectPath& path_,
		const OW_String& resultClass, const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
		OW_CIMObjectPathResultHandlerIFC* popresult,
		OW_CIMClassResultHandlerIFC* pcresult, const OW_ACLInfo& aclInfo);
	
	void _getAssociationClasses(const OW_String& ns,
		const OW_String& assocClassName, const OW_String& className,
		OW_CIMClassResultHandlerIFC& result, const OW_String& role,
		const OW_ACLInfo& aclInfo);

	void _staticReferences(const OW_CIMObjectPath& path,
		const OW_SortedVectorSet<OW_String>* refClasses, const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC& result,
		const OW_ACLInfo& aclInfo);
	
	void _staticReferenceNames(const OW_CIMObjectPath& path,
		const OW_SortedVectorSet<OW_String>* refClasses, const OW_String& role,
		OW_CIMObjectPathResultHandlerIFC& result);

	void _staticReferencesClass(const OW_CIMObjectPath& path,
		const OW_SortedVectorSet<OW_String>* resultClasses,
		const OW_String& role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList,
		OW_CIMObjectPathResultHandlerIFC* popresult,
		OW_CIMClassResultHandlerIFC* pcresult,
		const OW_ACLInfo& aclInfo);

	void _staticAssociators(const OW_CIMObjectPath& path,
		const OW_SortedVectorSet<OW_String>* passocClasses,
		const OW_SortedVectorSet<OW_String>* presultClasses,
		const OW_String& role, const OW_String& resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC& result,
		const OW_ACLInfo& aclInfo);

	void _staticAssociatorNames(const OW_CIMObjectPath& path,
		const OW_SortedVectorSet<OW_String>* passocClasses,
		const OW_SortedVectorSet<OW_String>* presultClasses,
		const OW_String& role, const OW_String& resultRole,
		OW_CIMObjectPathResultHandlerIFC& result);

	void _staticAssociatorsClass(
		const OW_CIMObjectPath& path,
		const OW_SortedVectorSet<OW_String>* assocClassNames,
		const OW_SortedVectorSet<OW_String>* resultClasses,
		const OW_String& role, const OW_String& resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, OW_CIMObjectPathResultHandlerIFC* popresult,
		OW_CIMClassResultHandlerIFC* pcresult,
		const OW_ACLInfo& aclInfo);

	void _validatePropagatedKeys(const OW_String& ns,
		const OW_CIMInstance& ci, const OW_CIMClass& theClass);

	OW_GenericHDBRepository m_nStore;
	OW_InstanceRepository m_iStore;
	OW_MetaRepository m_mStore;
	OW_AssocDb m_classAssocDb;
	OW_AssocDb m_instAssocDb;
	OW_CIMOMEnvironmentRef m_env;

    bool m_checkReferentialIntegrity;

	friend class OW_CIMServer;
};

#endif



