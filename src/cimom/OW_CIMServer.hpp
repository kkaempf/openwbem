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
#ifndef OW_CIMSERVER_HPP_INCLUDE_GUARD_
#define OW_CIMSERVER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_RepositoryIFC.hpp"
#include "OW_ProviderManager.hpp"
#include "OW_Map.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_CIMClass.hpp"
#include "OW_SortedVectorSet.hpp" // fwd?
#include "OW_CIMException.hpp"
#include "OW_RWLocker.hpp"

namespace OpenWBEM
{

// TODO: Make this configurable?  Maybe even a parameter that can be specifed by the client on each request?
const UInt32 LockTimeout = 100; // seconds
#if !defined(OW_DISABLE_ACLS)
class AccessMgr;
#endif
class CIMRepository;
// This class is responsible for:
// 1. calling either providers/or the CIM Repository
// 2. Access control
class CIMServer : public RepositoryIFC
{
public:
	static const char* const INST_REPOS_NAME;
	static const char* const META_REPOS_NAME;
	static const char* const NS_REPOS_NAME;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	static const char* const CLASS_ASSOC_REPOS_NAME;
	static const char* const INST_ASSOC_REPOS_NAME;
#endif
	/**
	 * Create a new CIMServer object.
	 */
	CIMServer(CIMOMEnvironmentRef env,
		const ProviderManagerRef& providerManager,
		const RepositoryIFCRef& repository);
	/**
	 * Destroy this CIMServer object.
	 */
	virtual ~CIMServer();
	/**
	 * Open this CIMServer.
	 * @exception IOException
	 */
	virtual void open(const String& path);
	/**
	 * Close this GenericHDBRepository.
	 */
	virtual void close();
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Create a cim namespace.
	 * @param ns 	The namespace to be created.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException If the namespace already exists.
	 */
	virtual void createNameSpace(const String& ns, const UserInfo& aclInfo);
	/**
	 * Delete a specified namespace.
	 * @param ns	The namespace to delete.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException If the namespace does not exist.
	 */
	virtual void deleteNameSpace(const String& ns, const UserInfo& aclInfo);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Gets a list of the namespaces.
	 * @param aclInfo ACL object describing user making request.
	 * @return An Array of namespace names as strings.
	 * @exception 	CIMException If the namespace does not exist or the object
	 *					cannot be found in the specified namespace.
	 */
	virtual void enumNameSpace(StringResultHandlerIFC& result,
		const UserInfo& aclInfo);
	/**
	 * Get an existing qualifier type from the repository.
	 * @param objPath	The object path for the qaulifer type.
	 * @param aclInfo ACL object describing user making request.
	 * @return A valid CIMQaulifer type on success. Otherwise a NULL
	 * CIMQualifier type.
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMQualifierType getQualifierType(const String& ns,
		const String& qualifierName,
		const UserInfo& aclInfo);
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Enumerate the qualifier types in a name space.
	 * @param path	The object path to enumeration the qualifiers in.
	 * @param aclInfo ACL object describing user making request.
	 * @return An enumeration of CIMQualifierTypes
	 * @exception	CIMException
	 */
	virtual void enumQualifierTypes(
		const String& ns,
		CIMQualifierTypeResultHandlerIFC& result,
		const UserInfo& aclInfo);
	/**
	 * Delete an existing qualifier type from the repository
	 * @param objPath	The object path fro the qualifer type.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual void deleteQualifierType(const String& ns, const String& qualName,
		const UserInfo& aclInfo);
	/**
	 * Updates the specified CIM qualifier type in the specified namespace.
	 * @param name	Am CIMObjectPath that identifies the CIM qualifier type.
	 * @param qt	The CIM qualifier type to be updated.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException If the qualifier type cannot be found or the
	 *										user does not have write permission to the
	 *										namespace.
	 */
	virtual void setQualifierType(const String& ns,
		const CIMQualifierType& qt, const UserInfo& aclInfo);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
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
	 * @return A valid CIMClass object on success.  A NULL CIMClass
	 *    will never be returned.  An exception will be thrown instead.
	 * CIMClass object.
	 * @exception CIMException
	 *		CIM_ERR_FAILED
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 */
	virtual CIMClass getClass(
		const String& ns,
		const String& className,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const UserInfo& aclInfo);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Delete an existing class from the store
	 *
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete
	 * @param aclInfo ACL object describing user making request.
	 * @return an CIMClass representing the class which was deleted.
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
	virtual CIMClass deleteClass(const String& ns, const String& className,
		const UserInfo& aclInfo);
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
	virtual void createClass(const String& ns, const CIMClass& cimClass,
		const UserInfo& aclInfo);
	/**
	 * set a class in the store - note children are not affected
	 *
	 * @param path The path for the class to create
	 * @param cimClass The class to create
	 * @param aclInfo ACL object describing user making request.
	 * @return an CIMClass representing the state of the class prior to
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
	virtual CIMClass modifyClass(
		const String& ns,
		const CIMClass& cc,
		const UserInfo& aclInfo);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
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
	 * @exception CIMException  	
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const UserInfo& aclInfo);
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param path		The CIMObjectPath identifying the class to be
	 *						enumerated.
	 * @param deep		If set to CIMClient::DEEP, the enumeration returned will
	 *						contain the names of all classes derived from the
	 *						enumerated class. If set to CIMClient::SHALLOW the
	 *						enumermation will return only the names of the first level
	 *						children of the enumerated class.
	 * @param localOnly Only include properties
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException  	
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void enumClassNames(const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, const UserInfo& aclInfo);
	/**
	 * Retrieve an enumeration of instances (CIMInstance) for a particular
	 * class
	 *
	 * @param path The CIMObjectPath identifying the class whose	instances
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
	 * @exception CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void enumInstances(
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, 
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, 
		WBEMFlags::EEnumSubclassesFlag enumSubclasses, 
		const UserInfo& aclInfo);
	/**
	 * Retrieve an enumeration of instances object paths (CIMObjectPath)
	 * for a particular class
	 *
	 * @param path			The CIMObjectPath identifying the class whose
	 *							instances are to be enumerated.
	 * @param deep			If set to CIMClient::DEEP, the enumeration returned
	 *							will contain the names of all instances of the specified
	 *                	class and all classes derived from it. If set to
	 *							CIMClient::SHALLOW only names of instances belonging
	 *							to the specified class are returned.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual void enumInstanceNames(
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, const UserInfo& aclInfo);
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
	 * @exception CIMException
	 *		CIM_ERR_FAILED
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_INVALID_PARAMETER
	 */
	virtual CIMInstance getInstance(
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const UserInfo& aclInfo);
	virtual CIMInstance getInstance(
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		CIMClass* pOutClass, const UserInfo& aclInfo);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Delete an existing instance from the store
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual CIMInstance deleteInstance(const String& ns, const CIMObjectPath& cop,
		const UserInfo& aclInfo);
	/**
	 * Creates a instance in the store
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param ci The instance that is to be stored with that object path
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_ALREADY_EXISTS
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_FAILED
	 */
	virtual CIMObjectPath createInstance(const String& ns,
		const CIMInstance& ci,
		const UserInfo& aclInfo);
	/**
	 * Update an instance
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @param ci	The instance with the new values
	 * @param aclInfo ACL object describing user making request.
	 * @return a CIMInstance representing the state of the instance prior
	 * 	to the update.  This is likely usefull only for creating
	 *		CIM_InstModification indications.
	 * @exception CIMException
	 *		CIM_ERR_ACCESS_DENIED
	 *		CIM_ERR_NOT_SUPPORTED
	 *		CIM_ERR_INVALID_NAMESPACE
	 *		CIM_ERR_INVALID_PARAMETER
	 *		CIM_ERR_INVALID_CLASS
	 *		CIM_ERR_NOT_FOUND
	 *		CIM_ERR_FAILED
	 */
	virtual CIMInstance modifyInstance(
		const String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const UserInfo& aclInfo);
	/**
	 * Set a property value on an CIMInstance.
	 * @param name				The object path of the instance
	 * @param propertyName	The name of the property to update
	 * @param cv				The value to set the property to.
	 * @param aclInfo ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual void setProperty(
		const String& ns,
		const CIMObjectPath& name, const String& propertyName,
		const CIMValue& cv, const UserInfo& aclInfo);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
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
		const String& ns,
		const CIMObjectPath& name,
		const String& propertyName, const UserInfo& aclInfo);
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
	 *
	 * @exception CIMException
	 */
	virtual CIMValue invokeMethod(
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName, const CIMParamValueArray& inParams,
		CIMParamValueArray& outParams, const UserInfo& aclInfo);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	virtual void associatorNames(
		const String& ns,
		const CIMObjectPath& path,
		CIMObjectPathResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole, const UserInfo& aclInfo);
	virtual void associators(
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole,  WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const UserInfo& aclInfo);
	virtual void associatorsClasses(
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole,  WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const UserInfo& aclInfo);
	virtual void referenceNames(
		const String& ns,
		const CIMObjectPath& path,
		CIMObjectPathResultHandlerIFC& result,
		const String& resultClass,
		const String& role, const UserInfo& aclInfo);
	virtual void references(
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& resultClass,
		const String& role, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const UserInfo& aclInfo);
	
	virtual void referencesClasses(
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& resultClass,
		const String& role, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const UserInfo& aclInfo);
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	
	/**
	 *
	 * @exception CIMException
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
		const String& ns,
		CIMInstanceResultHandlerIFC& result,
		const String &query, const String& queryLanguage,
		const UserInfo& aclInfo);
	virtual void getSchemaReadLock() { m_rwSchemaLocker.getReadLock(LockTimeout); }
	virtual void getSchemaWriteLock() { m_rwSchemaLocker.getWriteLock(LockTimeout); }
	virtual void releaseSchemaReadLock() { m_rwSchemaLocker.releaseReadLock(); }
	virtual void releaseSchemaWriteLock() { m_rwSchemaLocker.releaseWriteLock(); }
	virtual void getInstanceReadLock() { m_rwInstanceLocker.getReadLock(LockTimeout); }
	virtual void getInstanceWriteLock() { m_rwInstanceLocker.getWriteLock(LockTimeout); }
	virtual void releaseInstanceReadLock() { m_rwInstanceLocker.releaseReadLock(); }
	virtual void releaseInstanceWriteLock() { m_rwInstanceLocker.releaseWriteLock(); }
	ServiceEnvironmentIFCRef getEnvironment() const { return m_env; }
public:
	void _getCIMInstanceNames(const String& ns, const String& className,
		const CIMClass& theClass, CIMObjectPathResultHandlerIFC& result,
		const UserInfo& aclInfo);
	void _getCIMInstances(
		const String& ns,
		const String& className,
		const CIMClass& theTopClass,
		const CIMClass& theClass, CIMInstanceResultHandlerIFC& result,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EDeepFlag deep, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		const UserInfo& aclInfo);
private:
	/**
	 * Determines if an instance already exists
	 *
	 * @param cop	The CIMObectPath that specifies the instance
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	bool _instanceExists(const String& ns, const CIMObjectPath& cop,
		const UserInfo& aclInfo);
public:
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	bool _isDynamicAssoc(const String& ns, const CIMClass& cc);
#endif
private:
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	void _commonAssociators(
		const String& ns,
		const CIMObjectPath& path,
		const String& assocClassName, const String& resultClass,
		const String& role, const String& resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
		CIMObjectPathResultHandlerIFC* popresult,
		CIMClassResultHandlerIFC* pcresult,
		const UserInfo& aclInfo);
	void _dynamicAssociators(const CIMObjectPath& path,
		const CIMClassArray& assocClasses, const String& resultClass,
		const String& role, const String& resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
		CIMObjectPathResultHandlerIFC* popresult, const UserInfo& aclInfo);
	void _commonReferences(
		const String& ns,
		const CIMObjectPath& path,
		const String& resultClass, const String& role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
		CIMObjectPathResultHandlerIFC* popresult,
		CIMClassResultHandlerIFC* pcresult,
		const UserInfo& aclInfo);
	void _dynamicReferences(const CIMObjectPath& path,
		const CIMClassArray& dynamicAssocs, const String& role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
		CIMObjectPathResultHandlerIFC* popresult, const UserInfo& aclInfo);
	void _getAssociationClasses(const String& ns,
		const String& assocClassName, const String& className,
		CIMClassResultHandlerIFC& result, const String& role);
#endif
	/**
	 * Get the special __Namespace class
	 * @param className	The name of the class to check __Namespace for.
	 * @return If className is __Namespace a valid CIMClass. Otherwise a
	 * null CIMClass.
	 */
	CIMClass _getNameSpaceClass(const String& className);
	InstanceProviderIFCRef _getInstanceProvider(const String& ns,
		const CIMClass& cls);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	AssociatorProviderIFCRef _getAssociatorProvider(const String& ns, const CIMClass& cls);
#endif
private:
	CIMClass _getClass(const String& ns, const String& className, 
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, const UserInfo& aclInfo);
	CIMClass _instGetClass(const String& ns, const String& className,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, const UserInfo& aclInfo);
	ProviderManagerRef m_provManager;
	mutable RWLocker m_rwSchemaLocker;
	mutable RWLocker m_rwInstanceLocker;
#if !defined(OW_DISABLE_ACLS)
	Reference<AccessMgr> m_accessMgr;
#endif
	CIMClass m_nsClass_Namespace;
	//CIMClass m_nsClassCIM_Namespace;
	CIMOMEnvironmentRef m_env;
	RepositoryIFCRef m_cimRepository;
	IntrusiveReference<CIMRepository> m_realRepository;
};

} // end namespace OpenWBEM

#endif
