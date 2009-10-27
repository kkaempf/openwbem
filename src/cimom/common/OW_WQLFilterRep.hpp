/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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

#ifndef OW_WQLFILTERREP_HPP_INCLUDE_GUARD_
#define OW_WQLFILTERREP_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_RepositoryIFC.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CimomCommonFwd.hpp"

namespace OW_NAMESPACE
{

class OW_CIMOMCOMMON_API WQLFilterRep : public RepositoryIFC
{
public:
	WQLFilterRep(const CIMInstance& inst, const RepositoryIFCRef& cimServer);
	/**
	 * Open this RepositoryIFC.
	 * @exception IOException
	 */
	virtual void open(const blocxx::String &/*path*/) {}
	/**
	 * Close this GenericHDBRepository.
	 */
	virtual void close() {}
	virtual blocxx::String getName() const;
	virtual void init(const ServiceEnvironmentIFCRef& env) {}
	virtual void shutdown() {}
	/**
	 * @return A reference to the ServiceEnvironmentIFC this object is using.
	 */
	virtual ServiceEnvironmentIFCRef getEnvironment() const;
	/**
	 * Gets a list of the namespaces.
	 * @param aclInfo ACL object describing user making request.
	 * @return An Array of namespace names as strings.
	 * @exception 	CIMException If the namespace does not exist or the object
	 *					cannot be found in the specified namespace.
	 */
	virtual void enumNameSpace(StringResultHandlerIFC& result,
		OperationContext& context);
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	/**
	 * Create a cim namespace.
	 * @param ns 	The namespace to be created.
	 * @exception CIMException If the namespace already exists.
	 */
	virtual void createNameSpace(const blocxx::String& ns,
		OperationContext& context);
	/**
	 * Delete a specified namespace.
	 * @param ns	The namespace to delete.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException If the namespace does not exist.
	 */
	virtual void deleteNameSpace(const blocxx::String& ns,
		OperationContext& context);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Get an existing qualifier type from the repository.
	 * @param objPath	The object path for the qaulifer type.
	 * @param aclInfo ACL object describing user making request.
	 * @return A valid CIMQaulifer type on success. Otherwise a NULL
	 * CIMQualifier type.
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMQualifierType getQualifierType(
		const blocxx::String& ns,
		const blocxx::String& qualifierName, OperationContext& context);
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Updates the specified CIM qualifier type in the specified namespace.
	 * @param name	Am CIMObjectPath that identifies the CIM qualifier type.
	 * @param qt	The CIM qualifier type to be updated.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException If the qualifier type cannot be found or the
	 *										user does not have write permission to the
	 *										namespace.
	 */
	virtual void setQualifierType(const blocxx::String& ns,
		const CIMQualifierType &qt, OperationContext& context);
	/**
	 * Enumerate the qualifier types in a name space.
	 * @param path	The object path to enumeration the qualifiers in.
	 * @param aclInfo ACL object describing user making request.
	 * @return An enumeration of CIMQualifierTypes
	 * @exception	CIMException
	 */
	virtual void enumQualifierTypes(
		const blocxx::String& ns,
		CIMQualifierTypeResultHandlerIFC& result, OperationContext& context);
	/**
	 * Delete an existing qualifier type from the repository
	 * @param objPath	The object path fro the qualifer type.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual void deleteQualifierType(const blocxx::String& ns, const blocxx::String& qualName,
		OperationContext& context);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * creates a class in the store
	 *
	 * @param path The path for the class to create
	 * @param cimClass The class to create
	 * @param aclInfo ACL object describing user making request.
	 * @exception 	CIMException if the class already exists, or parent class
	 *					is not yet on file.
	 * @exception HDBException An error occurred in the database.
	 * @exception IOException Couldn't write class object to file.
	 */
	virtual void createClass(const blocxx::String& ns, const CIMClass &cimClass,
		OperationContext& context);
	/**
	 * set a class in the store - note children are not affected
	 *
	 * @param path The path for the class to create
	 * @param cimClass The class to create
	 * @param aclInfo ACL object describing user making request.
	 * @return an CIMClass representing the state of the class prior to
	 * 	the update.  This is likely usefull only for creating
	 *		CIM_ClassModification indications.
	 * @exception CIMException if the class already exists
	 */
	virtual CIMClass modifyClass(const blocxx::String &ns,
		const CIMClass &cc, OperationContext& context);
	/**
	 * Delete an existing class from the store
	 *
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete
	 * @param aclInfo ACL object describing user making request.
	 * @return A CIMClass representing the class which was deleted.
	 *		This is likely usefull only for creating CIM_ClassCreation incidations.
	 * @exception CIMException if class does not exist
	 */
	virtual CIMClass deleteClass(const blocxx::String& ns, const blocxx::String& className,
		OperationContext& context);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
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
	 * @return A valid CIMClass object on success. Otherwise a NULL
	 * CIMClass object.
	 * @exception CIMException
	 * @exception HDBException An error occurred in the database.
	 * @exception IOException Couldn't read class object from file.
	 */
	virtual CIMClass getClass(
		const blocxx::String& ns,
		const blocxx::String& className,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const blocxx::StringArray* propertyList,
		OperationContext& context);
	/**
	 * Retrieve a specific instance
	 *
	 * @param cop	The CIMObectPath that specifies the instance
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
	 * @return An CIMInstance object
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMInstance getInstance(
		const blocxx::String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const blocxx::StringArray* propertyList,
		OperationContext& context);
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param path The CIMObjectPath identifying the class to be enumerated.
	 * @param deep If set to CIMClient::DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to CIMClient::SHALLOW the	enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If true, then only CIM elements (properties, methods,
	 *		qualifiers) overriden within the definition are returned.
	 * @param includeQualifiers If true, then all qualifiers for the class
	 *		(including properties, methods, and method parameters) are returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be returned on all appropriate components.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException  	If the specified CIMObjectPath object
	 *											cannot be foundl
	 */
	virtual void enumClasses(const blocxx::String& ns,
		const blocxx::String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, OperationContext& context);
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param path		The CIMObjectPath identifying the class to be
	 *						enumerated.
	 * @param deep		If set to CIMClient::DEEP, the enumeration returned will
	 *						contain the names of all classes derived from the
	 *						enumerated class. If set to CIMClient::SHALLOW the
	 *						enumermation will return only the names of the first level
	 *						children of the enumerated class.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException  	If the specified CIMObjectPath object
	 *											cannot be foundl
	 */
	virtual void enumClassNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, OperationContext& context);
	/**
	 * Retrieve an enumeration of instances (CIMInstance) for a particular
	 * class
	 *
	 * @param path The CIMObjectPath identifying the class whose	instances
	 *		are to be enumerated.
	 * @param deep If set to CIMClient::DEEP, the enumeration returned will
	 *		contain the names of all instances of the specified class and all
	 *		classes derived from it. If set to CIMClient::SHALLOW only names
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
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual void enumInstances(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList,
		WBEMFlags::EEnumSubclassesFlag enumSubclasses,
		OperationContext& context);
	/**
	 * Retrieve an enumeration of instances (CIMInstance) for a particular
	 * class
	 *
	 * @param path			The CIMObjectPath identifying the class whose
	 *							instances are to be enumerated.
	 * @param deep			If set to CIMClient::DEEP, the enumeration returned
	 *							will contain the names of all instances of the specified
	 *                	class and all classes derived from it. If set to
	 *							CIMClient::SHALLOW only names of instances belonging
	 *							to the specified class are returned.
	 * @param aclInfo ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual void enumInstanceNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMObjectPathResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, OperationContext& context);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Creates a instance in the store
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param ci The instance that is to be stored with that object path
	 * @param aclInfo ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMObjectPath createInstance(const blocxx::String& ns,
		const CIMInstance &ci,
		OperationContext& context);
	/**
	 * Update an instance
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param ci	The instance with the new values
	 * @param aclInfo ACL object describing user making request.
	 * @return a CIMInstance representing the state of the instance prior
	 * 	to the update.  This is likely usefull only for creating
	 *		CIM_InstModification indications.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMInstance modifyInstance(
		const blocxx::String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const blocxx::StringArray* propertyList,
		OperationContext& context);
	/**
	 * Delete an existing instance from the store
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param aclInfo ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 * @return an CIMInstance representing the Instance just deleted.
	 *		This is likely usefull only for creating CIM_InstDeletion indications;
	 */
	virtual CIMInstance deleteInstance(const blocxx::String& ns, const CIMObjectPath &cop,
		OperationContext& context);
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	/**
	 * Set a property value on an CIMInstance.
	 * @param name				The object path of the instance
	 * @param propertyName	The name of the property to update
	 * @param cv				The value to set the property to.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual void setProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName, const CIMValue &cv,
		OperationContext& context);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	/**
	 * Get the specified CIM instance property.
	 * @param name				An CIMObjectPath that identifies the CIM instance
	 *								to be accessed
	 * @param propertyName	The name of the property to retrieve.
	 * @param aclInfo ACL object describing user making request.
	 * @return The CIMvalue for property identified by propertyName.
	 * @exception CIMException
	 */
	virtual CIMValue getProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName, OperationContext& context);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	virtual RepositoryIFC::ELockType getLockTypeForMethod(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		const blocxx::String& methodName,
		const CIMParamValueArray& in,
		OperationContext& context);
	/**
	 * Invokes a method
	 *
	 * @param name	The object path of the object on which the method is being
	 *					invoked this can be a class or instance
	 * @param methodName	The name of the method to invoke.
	 * @param inParams An array of CIMProperty objects for inbound values
	 * @param outParams An array of CIMProperty objects for inbound values
	 * @param outParams A vector of CIMProperty for outbound values
	 * @param aclInfo ACL object describing user making request.
	 * @return The return from the extrinsic method as a CIMValue.
	 * @exception CIMException
	 */
	virtual CIMValue invokeMethod(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		const blocxx::String &methodName, const CIMParamValueArray &inParams,
		CIMParamValueArray &outParams, OperationContext& context);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void associators(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &assocClass, const blocxx::String &resultClass,
		const blocxx::String &role, const blocxx::String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray *propertyList, OperationContext& context);
	virtual void associatorsClasses(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String &assocClass, const blocxx::String &resultClass,
		const blocxx::String &role, const blocxx::String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray *propertyList, OperationContext& context);
	virtual void referenceNames(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &resultClass,
		const blocxx::String &role, OperationContext& context);
	virtual void associatorNames(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &assocClass,
		const blocxx::String &resultClass, const blocxx::String &role,
		const blocxx::String &resultRole, OperationContext& context);
	virtual void references(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray *propertyList, OperationContext& context);
	virtual void referencesClasses(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray *propertyList, OperationContext& context);
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void execQuery(
		const blocxx::String& ns,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &query, const blocxx::String &queryLanguage,
		OperationContext& context);
	virtual void enumInstancesWQL(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMInstanceResultHandlerIFC& result,
		const WQLSelectStatement& wss,
		const WQLCompile& wc,
		OperationContext& context);

	virtual void beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context);
	virtual void endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result);
private:
	CIMInstance m_inst;
	RepositoryIFCRef m_pCIMServer;
};

} // end namespace OW_NAMESPACE

#endif
