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

#ifndef OW_REPOSITORYIFC_HPP_INCLUDE_GUARD_
#define OW_REPOSITORYIFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "blocxx/SharedLibraryReference.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_WBEMFlags.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_ServiceIFC.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_COMMON_API RepositoryIFC : virtual public ServiceIFC
{
	typedef ServiceIFC base_t;

public:
	RepositoryIFC();
	RepositoryIFC(const RepositoryIFC& x);
	virtual ~RepositoryIFC();
	/**
	 * Open this RepositoryIFC.
	 * @exception IOException
	 */
	virtual void open(const String& path) = 0;
	/**
	 * Close this RepositoryIFC.
	 */
	virtual void close() = 0;

	virtual ServiceEnvironmentIFCRef getEnvironment() const = 0;
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
	/**
	 * Create a cim namespace.
	 * @param ns The namespace to be created.
	 * @param context ACL object describing user making request.
	 * @exception CIMException If the namespace already exists.
	 */
	virtual void createNameSpace(const String& ns,
		OperationContext& context) = 0;
	/**
	 * Delete a specified namespace.
	 * @param ns The namespace to delete.
	 * @param context ACL object describing user making request.
	 * @exception CIMException If the namespace does not exist.
	 */
	virtual void deleteNameSpace(const String& ns,
		OperationContext& context) = 0;
#endif
	/**
	 * Gets a list of the namespaces.
	 * @param context ACL object describing user making request.
	 * @return An Array of namespace names as strings.
	 * @exception CIMException If the namespace does not exist or the
	 * 	object cannot be found in the specified namespace.
	 */
	virtual void enumNameSpace(
		StringResultHandlerIFC& result,
		OperationContext& context) = 0;
	/**
	 * Get an existing qualifier type from the repository.
	 * @param ns The namespace to search for the qualifier.
	 * @param qualifierName The name of the qualifier to be located.
	 * @param context ACL object describing user making request.
	 * @return A valid CIMQaulifer type on success. Otherwise a NULL
	 *	CIMQualifier type.
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMQualifierType getQualifierType(
		const String& ns,
		const String& qualifierName, OperationContext& context) = 0;
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Enumerate the qualifier types in a name space.
	 * @param ns The namespeace to search for qualifiers.
	 * @param result A callback object to handle the qualifier types as
	 * 	they are received.
	 * @param context ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual void enumQualifierTypes(
		const String& ns,
		CIMQualifierTypeResultHandlerIFC& result,
		OperationContext& context) = 0;
	/**
	 * Delete an existing qualifier type from the repository
	 * @param ns The namespace from which the qualfier should be deleted.
	 * @param qualName The name of the qualifier to be deleted.
	 * @param context ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual void deleteQualifierType(const String& ns, const String& qualName,
		OperationContext& context) = 0;
	/**
	 * Updates the specified CIM qualifier type in the specified namespace.
	 * @param ns The namespace from which the qualifier should be updated.
	 * @param qt The CIM qualifier type to be updated.
	 * @param context ACL object describing user making request.
	 * @exception CIMException If the qualifier type cannot be found or the
	 *	user does not have write permission to the namespace.
	 */
	virtual void setQualifierType(const String& ns,
		const CIMQualifierType& qt, OperationContext& context) = 0;
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Gets an existing class from a store
	 * @param ns The namespace from which the class should be retrieved.
	 * @param className the name of the class to retrieve from the
	 * 	repository.
	 * @param localOnly If E_LOCAL_ONLY, then only CIM elements
	 * 	(properties, methods, qualifiers) overriden within the
	 * 	definition are returned.
	 * @param includeQualifiers If E_INCLUDE_QUALIFIERS, then all
	 * 	qualifiers for the class (including properties, methods, and
	 * 	method parameters) are returned.
	 * @param includeClassOrigin If E_INCLUDE_CLASS_ORIGIN, then the class
	 * 	origin attribute will be returned on all appropriate
	 * 	components.
	 * @param propertyList If not NULL then it specifies the only
	 * 	properties that can be returned with the class. If not NULL but
	 * 	the array is empty, then no properties should be returned. If
	 * 	NULL then all properties will be returned.
	 * @param context ACL object describing user making request.
	 * @return A valid CIMClass object on success. Otherwise a NULL
	 * 	CIMClass object.
	 * @exception CIMException
	 * @exception HDBException An error occurred in the database.
	 * @exception IOException Couldn't read class object from file.
	 */
	virtual CIMClass getClass(
		const String& ns,
		const String& className,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context) = 0;
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Delete an existing class from the store
	 *
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete
	 * @param context ACL object describing user making request.
	 * @return A CIMClass representing the class which was deleted.  This
	 * 	is likely usefull only for creating CIM_ClassCreation
	 * 	incidations.
	 * @exception CIMException if class does not exist
	 */
	virtual CIMClass deleteClass(const String& ns, const String& className,
		OperationContext& context) = 0;
	/**
	 * Creates a class in the store
	 *
	 * @param ns The namespace in which to create a class.
	 * @param cimClass The class to create.
	 * @param context ACL object describing user making request.
	 * @exception CIMException if the class already exists, or parent class
	 *	is not yet on file.
	 * @exception HDBException An error occurred in the database.
	 * @exception IOException Couldn't write class object to file.
	 */
	virtual void createClass(const String& ns,
		const CIMClass& cimClass, OperationContext& context) = 0;
	/**
	 * set a class in the store - note children are not affected
	 *
	 * @param ns The namespace in which to modify the class.
	 * @param cimClass The class to modify
	 * @param context ACL object describing user making request.
	 * @return an CIMClass representing the state of the class prior to the
	 * 	update.  This is likely usefull only for creating
	 * 	CIM_ClassModification indications.
	 * @exception CIMException if the class already exists
	 */
	virtual CIMClass modifyClass(const String &ns,
		const CIMClass& cimClass, OperationContext& context) = 0;
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param ns The namespace to search.
	 * @param className The name of the class to find.
	 * @param path The CIMObjectPath identifying the class to be enumerated.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 * 	the names of all classes derived from the enumerated class.  If
	 * 	set to E_SHALLOW the enumermation will return only the names of
	 * 	the first level children of the enumerated class.
	 * @param localOnly If E_LOCAL_ONLY, then only CIM elements
	 * 	(properties, methods, qualifiers) overriden within the
	 * 	definition are returned.
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all
	 * 	qualifiers for the class (including properties, methods, and
	 * 	method parameters) are returned.
	 * @param includeClassOrigin If set to E_INCLUDE_CLASS_ORIGIN, then the
	 * 	class origin attribute will be returned on all appropriate
	 * 	components.
	 * @param context ACL object describing user making request.
	 * @exception CIMException If the specified CIMObjectPath object cannot
	 * 	be found.
	 */
	virtual void enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, OperationContext& context) = 0;
	/**
	 * Enumerates the class specified by the namespace and classname.
	 * @param ns The namespace to search for classes.
	 * @param className The name of the class(es) to be enumerated.  If
	 *	this is empty, it specifies that all classnames should be
	 * 	returned.
	 * @param result A callback object which will handle the results as
	 * 	they are received.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 *	the names of all classes derived from the enumerated class. If
	 *	set to E_SHALLOW the enumermation will return only the names of
	 * 	the first level children of the enumerated class.
	 * @param context ACL object describing user making request.
	 * @exception CIMException If the specified namespace object cannot be found.
	 */
	virtual void enumClassNames(const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep, OperationContext& context) = 0;
	/**
	 * Retrieve an enumeration of instances (CIMInstance) for a particular
	 * class.
	 *
	 * @param ns The namespace to search for instances.
	 * @param className The name of the class to locate.
	 * @param result A callback object which will handle the instances as
	 * 	they are received.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 * 	the names of all instances of the specified class and all
	 * 	classes derived from it. If set to E_SHALLOW only names of
	 * 	instances belonging to the specified class are returned.
	 * @param localOnly If set to E_LOCAL_ONLY, only non-inherited
	 * 	properties/qualifiers are included in the instances.
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all
	 * 	qualifiers for the instance (including properties, methods, and
	 * 	method parameters) are returned.
	 * @param includeClassOrigin If set to E_INCLUDE_CLASS_ORIGIN, then the
	 * 	class origin attribute will be returned on all appropriate components.
	 * @param propertyList If not NULL then is specifies the only
	 * 	properties that can be returned with the class. If not NULL but
	 * 	the array is empty, then no properties should be returned. If
	 * 	NULL then all properties will be returned.
	 * @param context ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
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
		OperationContext& context) = 0;
	/**
	 * Retrieve an enumeration of instance object paths (CIMInstance)
	 * for a particular class
	 *
	 * @param ns The namespace to search for instance names.
	 * @param className The name of the class to locate.
	 * @param result A callback object to handle the instance path of the
	 * 	names as they are received.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 * 	the names of all instances of the specified class and all
	 *	classes derived from it. If set to E_SHALLOW only names of
	 *	instances belonging to the specified class are returned.
	 * @param context ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual void enumInstanceNames(
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep,
		OperationContext& context) = 0;
	/**
	 * Retrieve a specific instance
	 *
	 * @param ns The namespace from which to retrieve an instance.
	 * @param instanceName The path of the instance to be retrieved.
	 * @param localOnly If E_LOCAL_ONLY, only return non-derived properties.
	 * @param includeQualifiers If E_INCLUDE_QUALIFIERS, then all
	 * 	qualifiers for the instance (including properties, methods, and
	 * 	method parameters) are returned.
	 * @param includeClassOrigin If E_INCLUDE_CLASS_ORIGIN, then the class
	 * 	origin attribute will be returned on all appropriate
	 * 	components.
	 * @param propertyList If not NULL then is specifies the only
	 * 	properties that can be returned with the class. If not NULL but
	 * 	the array is empty, then no properties should be returned. If
	 * 	NULL then all properties will be returned.
	 * @param context ACL object describing user making request.
	 * @return An CIMInstance object
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMInstance getInstance(
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context) = 0;
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Delete an existing instance from the store
	 *
	 * @param ns The namespace from which to delete an instance.
	 * @param cop The CIMObectPath that specifies the instance.
	 * @param context ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 * @return an CIMInstance representing the Instance just deleted.  This
	 * 	is likely usefull only for creating CIM_InstDeletion
	 * 	indications.
	 */
	virtual CIMInstance deleteInstance(const String& ns, const CIMObjectPath& cop,
		OperationContext& context) = 0;
	/**
	 * Creates a instance in the store
	 *
	 * @param ns The namespace to which the an instance should be added.
	 * @param ci The instance that is to be stored with that object path
	 * @param context ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMObjectPath createInstance(const String& ns,
		const CIMInstance& ci, OperationContext& context) = 0;
	/**
	 * Update an instance
	 *
	 * @param ns The namespace in which an instance should be modified.
	 * @param ci The instance with the new values
	 * @param includeQualfiers If set to E_INCLUDE_QUALIFIERS, then the
	 *	qualifiers will be modified as well.
	 * @param context ACL object describing user making request.
	 * @return a CIMInstance representing the state of the instance prior
	 * 	to the update.  This is likely usefull only for creating
	 * 	CIM_InstModification indications.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual CIMInstance modifyInstance(
		const String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		OperationContext& context) = 0;
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	/**
	 * Set a property value on an CIMInstance.
	 * @param ns The namespace in which a property should be modified.
	 * @param name The object path of the instance.
	 * @param propertyName The name of the property to update
	 * @param cv The value to set the property to.
	 * @param context ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual void setProperty(
		const String& ns,
		const CIMObjectPath& name,
		const String& propertyName, const CIMValue& cv,
		OperationContext& context) = 0;
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	/**
	 * Get the specified CIM instance property.
	 * @param ns The namespace containing the desired instance.
	 * @param name An CIMObjectPath that identifies the CIM instance to be
	 * 	accessed
	 * @param propertyName	The name of the property to retrieve.
	 * @param context ACL object describing user making request.
	 * @return The CIMvalue for property identified by propertyName.
	 * @exception CIMException
	 */
	virtual CIMValue getProperty(
		const String& ns,
		const CIMObjectPath& name,
		const String& propertyName, OperationContext& context) = 0;
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)

	enum ELockType
	{
		E_NO_LOCK,
		E_READ_LOCK,
		E_WRITE_LOCK
	};
	/**
	 * Return the type of lock necessary for the method invocation.
	 * 
	 * @throws CIMException on failure or if not implemented.
	 */
	virtual ELockType getLockTypeForMethod(
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& in, 
		OperationContext& context) = 0;

	/**
	 * Invokes a method
	 *
	 * @param ns The namespace containing the method instance.
	 * @param path The object path of the object on which the method is
	 * 	being invoked this can be a class or instance.
	 * @param methodName The name of the method to invoke.
	 * @param inParams An array of CIMParamValue objects for inbound values
	 * @param outParams An array of CIMParamValue objects for outbound values
	 * @param outParams A vector of CIMProperty for outbound values
	 * @param context ACL object describing user making request.
	 * @exception CIMException
	 */
	virtual CIMValue invokeMethod(
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName, const CIMParamValueArray& inParams,
		CIMParamValueArray& outParams, OperationContext& context) = 0;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * This method is used to enumerate the names of CIM Objects (Classes
	 * or Instances) that are associated to a particular CIM Object.
	 *
	 * @param ns The namespace containing the CIM Objects to be enumerated.
	 * @param objectName Defines the source CIM Object whose associated
	 *	Objects are to be returned. This may be either a Class name or
	 *	Instance name (modelpath).
	 * @param result A callback object that will handle the CIMObjectPaths
	 * 	as they are received.
	 * @param assocClass The AssocClass input parameter, if not empty (""),
	 * 	MUST be a valid CIM Association Class name. It acts as a filter
	 * 	on the returned set of Objects by mandating that each returned
	 * 	Object MUST be associated to the source Object via an Instance
	 * 	of this Class or one of its subclasses.
	 * @param resultClass The ResultClass input parameter, if not empty
	 *	(""), MUST be valid CIM Class name. It acts as a filter on the
	 *	returned set of Objects by mandating that each returned Object
	 *	MUST be either an Instance of this Class (or one of its
	 *	subclasses) or be this Class (or one of its subclasses).
	 * @param role The Role input parameter, if not empty (""), MUST be a
	 * 	valid Property name. It acts as a filter on the returned set of
	 *	Objects by mandating that each returned Object MUST be
	 *	associated to the source Object via an Association in which the
	 *	source Object plays the specified role (i.e. the name of the
	 *	Property in the Association Class that refers to the source
	 *	Object MUST match the value of this parameter).
	 * @param resultRole The ResultRole input parameter, if not empty (""),
	 * 	MUST be a valid Property name. It acts as a filter on the
	 * 	returned set of Objects by mandating that each returned Object
	 *	MUST be associated to the source Object via an Association in
	 *	which the returned Object plays the specified role (i.e. the
	 *	name of the Property in the Association Class that refers to
	 * 	the returned Object MUST match the value of this parameter).
	 *
	 * @exception CIMException - as defined in the associators method
	 */
	virtual void associatorNames(
		const String& ns,
		const CIMObjectPath& path,
		CIMObjectPathResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole, OperationContext& context) = 0;
	/**
	 * This operation is used to enumerate CIM Objects (Classes or
	 * Instances) that are associated to a particular source CIM Object.
	 * @param ns The namspace.
 	 * @param path Defines the path of the source CIM Object whose
	 * 	associated Objects are to be returned.  This may be a path to
	 * 	either a Class name or Instance name (model path).
 	 * @param result A callback object that will handle the CIMInstances
	 * 	as they are received.
	 *
	 * @param assocClass The AssocClass input parameter, if not empty (""),
	 * 	MUST be a valid CIM Association Class name. It acts as a filter
	 * 	on the returned set of Objects by mandating that each returned
	 * 	Object MUST be associated to the source Object via an Instance
	 * 	of this Class or one of its subclasses.
	 *
	 * @param resultClass The ResultClass input parameter, if not empty
	 * 	(""), MUST be a valid CIM Class name. It acts as a filter on
	 * 	the returned set of Objects by mandating that each returned
	 * 	Object MUST be either an Instance of this Class (or one of its
	 * 	subclasses) or be this Class (or one of its subclasses).
	 *
	 * @param role The Role input parameter, if not empty (""), MUST be a
	 * 	valid Property name. It acts as a filter on the returned set of
	 * 	Objects by mandating that each returned Object MUST be
	 * 	associated to the source Object via an Association in which the
	 * 	source Object plays the specified role (i.e. the name of the
	 * 	Property in the Association Class that refers to the source
	 * 	Object MUST match the value of this parameter).
	 *
	 * @param resultRole The ResultRole input parameter, if not empty (""),
	 * 	MUST be a valid Property name. It acts as a filter on the
	 * 	returned set of Objects by mandating that each returned Object
	 * 	MUST be associated to the source Object via an Association in
	 * 	which the returned Object plays the specified role (i.e. the
	 * 	name of the Property in the Association Class that refers to
	 * 	the returned Object MUST match the value of this parameter).
	 *
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS then all
	 *	Qualifiers for each Object (including Qualifiers on the Object
	 *	and on any returned Properties) MUST be included as QUALIFIER
	 *	elements in the response.  If set to E_EXCLUDE_QUALIFIERS, then no
	 *	qualifier elements will be present in the returned object(s).
	 *
	 * @param includeClassOrigin If set to E_INCLUDE_CLASS_ORIGIN, then the
	 * 	CLASSORIGIN attribute MUST be present on all appropriate
	 * 	elements in each returned Object. If set to
	 * 	E_EXCLUDE_CLASS_ORIGIN, no CLASSORIGIN attributes are present
	 * 	in each returned Object.
	 *
	 * @param propertyList If the PropertyList input parameter is not NULL,
	 * 	the members of the array define one or more Property names.
	 * 	Each returned Object MUST NOT include elements for any
	 * 	Properties missing from this list.  If the PropertyList input
	 * 	parameter is an empty array this signifies that no Properties
	 * 	are included in each returned Object. If the PropertyList input
	 * 	parameter is NULL this specifies that all Properties (subject
	 * 	to the conditions expressed by the other parameters) are
	 * 	included in each returned Object.
	 *
	 *	If the PropertyList contains duplicate elements, the Server
	 *	MUST ignore the duplicates but otherwise process the request
	 *	normally.  If the PropertyList contains elements which are
	 *	invalid Property names for any target Object, the Server MUST
	 *	ignore such entries but otherwise process the request
	 *	normally.
	 *
	 *	Clients SHOULD NOT explicitly specify properties in the
	 *	PropertyList parameter unless they have specified a non-empty
	 *	("") value for the ResultClass parameter.
	 *
	 * @exception CIMException. The following IDs can be expected:
	 *	CIM_ERR_ACCESS_DENIED, CIM_ERR_NOT_SUPPORTED,
	 *	CIM_ERR_INVALID_NAMESPACE, CIM_ERR_INVALID_PARAMETER (including
	 *	missing, duplicate, unrecognized or otherwise incorrect
	 *	parameters), CIM_ERR_FAILED (some other unspecified error
	 *	occurred).
	 */
	virtual void associators(
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole,  WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context) = 0;
	/**
	 * This operation is used to enumerate CIMClasses that are associated
	 * to a particular source CIM Object.
	 *
	 * @param ns The namspace.
 	 * @param path Defines the path of the source CIM Object whose
	 * 	associated Objects are to be returned.  This may be a path to
	 * 	either a Class name or Instance name (model path).
	 *
  	 * @param result A callback object that will handle the CIMClasses
	 * 	as they are received.
	 *
	 * @param assocClass The AssocClass input parameter, if not empty (""),
	 * 	MUST be a valid CIM Association Class name. It acts as a filter
	 * 	on the returned set of Objects by mandating that each returned
	 * 	Object MUST be associated to the source Object via an Instance
	 * 	of this Class or one of its subclasses.
	 *
	 * @param resultClass The ResultClass input parameter, if not empty
	 * 	(""), MUST be a valid CIM Class name. It acts as a filter on
	 * 	the returned set of Objects by mandating that each returned
	 * 	Object MUST be either an Instance of this Class (or one of its
	 * 	subclasses) or be this Class (or one of its subclasses).
	 *
	 * @param role The Role input parameter, if not empty (""), MUST be a
	 * 	valid Property name. It acts as a filter on the returned set of
	 * 	Objects by mandating that each returned Object MUST be
	 * 	associated to the source Object via an Association in which the
	 * 	source Object plays the specified role (i.e. the name of the
	 * 	Property in the Association Class that refers to the source
	 * 	Object MUST match the value of this parameter).
	 *
	 * @param resultRole The ResultRole input parameter, if not empty (""),
	 * 	MUST be a valid Property name. It acts as a filter on the
	 * 	returned set of Objects by mandating that each returned Object
	 * 	MUST be associated to the source Object via an Association in
	 * 	which the returned Object plays the specified role (i.e. the
	 * 	name of the Property in the Association Class that refers to
	 * 	the returned Object MUST match the value of this parameter).
	 *
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS then all
	 *	Qualifiers for each Object (including Qualifiers on the Object
	 *	and on any returned Properties) MUST be included as elements in
	 *	the response.If set to E_EXCLUDE_QUALIFIERS, then no qualifiers
	 *	will be present in the returned object(s).
	 *
	 * @param includeClassOrigin If the IncludeClassOrigin input parameter
	 * 	is set to E_INCLUDE_ORIGIN, then the CLASSORIGIN attribute MUST
	 * 	be present on all appropriate elements in each returned
	 * 	Object. If set to E_EXCLUDE_ORIGIN, no CLASSORIGIN attributes
	 * 	are present in each returned Object.
	 *
	 * @param propertyList If the PropertyList input parameter is not NULL,
	 * 	the members of the array define one or more Property names.
	 * 	Each returned Object MUST NOT include elements for any
	 * 	Properties missing from this list.  If the PropertyList input
	 * 	parameter is an empty array this signifies that no Properties
	 * 	are included in each returned Object. If the PropertyList input
	 * 	parameter is NULL this specifies that all Properties (subject
	 * 	to the conditions expressed by the other parameters) are
	 * 	included in each returned Object.
	 *
	 *	If the PropertyList contains duplicate elements, the Server
	 *	MUST ignore the duplicates but otherwise process the request
	 *	normally.  If the PropertyList contains elements which are
	 *	invalid Property names for any target Object, the Server MUST
	 *	ignore such entries but otherwise process the request
	 *	normally.
	 *
	 *	Clients SHOULD NOT explicitly specify properties in the
	 *	PropertyList parameter unless they have specified a non-empty
	 *	("") value for the ResultClass parameter. 	
	 *
	 *
	 * @exception CIMException. The following IDs can be expected:
	 *	CIM_ERR_ACCESS_DENIED, CIM_ERR_NOT_SUPPORTED,
	 *	CIM_ERR_INVALID_NAMESPACE, CIM_ERR_INVALID_PARAMETER (including
	 *	missing, duplicate, unrecognized or otherwise incorrect
	 *	parameters), CIM_ERR_FAILED (some other unspecified error
	 *	occurred)
	 */
	virtual void associatorsClasses(
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& assocClass,
		const String& resultClass, const String& role,
		const String& resultRole,  WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context) = 0;
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Object (Class or Instance).
	 *
	 * @param ns The namespace.
	 * @param path The ObjectName input parameter defines the target CIM
	 * 	Object path whose referring object names are to be returned. It
	 * 	may be either a Class name or an Instance name (model path).
	 * @param result A callback object to handle the object paths as they
	 * 	are received.
	 * @param resultClass  The ResultClass input parameter, if not empty
	 *	(""), MUST be a valid CIM Class name. It acts as a filter on
	 *	the returned set of Object Names by mandating that each
	 *	returned Object Name MUST identify an Instance of this Class
	 *	(or one of its subclasses), or this Class (or one of its
	 *	subclasses).
	 *
	 * @param role The Role input parameter, if not empty (""), MUST be a
	 * 	valid Property name. It acts as a filter on the returned set of
	 * 	Object Names by mandating that each returned Object Name MUST
	 * 	identify an Object that refers to the target Instance via a
	 * 	Property whose name matches the value of this parameter.
	 *
	 * @exception CIMException As defined for associators method.
	 */
	virtual void referenceNames(
		const String& ns,
		const CIMObjectPath& path,
		CIMObjectPathResultHandlerIFC& result,
		const String& resultClass,
		const String& role, OperationContext& context) = 0;
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Object (Class or Instance).
	 *
	 * @param ns The namespace.
	 *
	 * @param path The path input parameter defines the target CIM
	 * 	Object whose referring Objects are to be returned. This is
	 * 	either a Class name or Instance name (model path).
	 * @param result A callback object which will handle the instances as
	 * 	they are received.
	 *
	 * @param resultClass The ResultClass input parameter, if not empty
	 * 	(""), MUST be a valid CIM Class name. It acts as a filter on
	 * 	the returned set of Objects by mandating that each returned
	 * 	Object MUST be an Instance of this Class (or one of its
	 * 	subclasses), or this Class (or one of its subclasses).	
	 *
	 * @param role The Role input parameter, if not empty (""), MUST be a
	 * 	valid Property name. It acts as a filter on the returned set of
	 * 	Objects by mandating that each returned Objects MUST refer to
	 * 	the target Object via a Property whose name matches the value
	 * 	of this parameter.
	 *
	 * @param includeQualifiers Refer to includeQualifiers for associators.
	 *
	 * @param includeClassOrigin Refer to includeQualifiers for associators.
	 *
	 * @param propertyList Refer to includeQualifiers for associators.
	 *
	 * @see associators
	 *
	 * @exception CIMException - as defined for associators method.
	 */
	 virtual void references(
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& resultClass,
		const String& role, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context) = 0;
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Class.
	 *
	 * @see references
	 *
	 * @exception CIMException - as defined for associators method.
	 */	
	virtual void referencesClasses(
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& resultClass,
		const String& role, WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
		OperationContext& context) = 0;
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * Executes a query to retrieve or modify objects.
	 *
	 * @param ns CIMNameSpace that identifies the namespace in which to
	 * 	query.
	 *
	 * @param query A string containing the text of the query. This
	 * 	parameter cannot be empty..
	 *
	 * @param queryLanguage A string containing the query language the
	 * 	query is written in.
	 *
	 * @return CIMInstanceArray A deep enumeration of all instances of the
	 *	specified class and all classes derived from the specified
	 *	class, that match the query string.
	 *
	 * @exception CIMException If any of the following errors occur:
	 *	The user does not have permission to view the result.
	 *	The requested query language is not supported.
	 *	The query specifies a class that does not exist.
	 */	
	virtual void execQuery(
		const String& ns,
		CIMInstanceResultHandlerIFC& result,
		const String &query, const String& queryLanguage,
		OperationContext& context) = 0;
	/**
	 * Export a given instance of an indication.
	 * This will cause all CIMListerners that are interested in this type
	 * of indication to be notified.
	 * @param instance The indication instance to use in the notification.
	 */
	virtual void exportIndication(const CIMInstance&,
		const String&);

	/**
	 * Retrieve an enumeration of instances (CIMInstance) for a particular
	 * class.
	 *
	 * @param ns The namespace to search for instances.
	 * @param className The name of the class to locate.
	 * @param result A callback object which will handle the instances as
	 * 	they are received.
	 * @param context ACL object describing user making request.
	 * @exception HDBException
	 * @exception CIMException
	 * @exception IOException
	 */
	virtual void enumInstancesWQL(
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		const WQLSelectStatement& wss,
		const WQLCompile& wc,
		OperationContext& context) = 0;

	virtual void beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context) = 0;
	virtual void endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result) = 0;
};

} // end namespace OW_NAMESPACE

#endif
