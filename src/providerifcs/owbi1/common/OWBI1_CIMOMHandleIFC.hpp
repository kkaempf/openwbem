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

#ifndef OWBI1_CIMOMHANDLE_IFC_HPP_INCLUDE_GUARD_
#define OWBI1_CIMOMHANDLE_IFC_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_CIMFwd.hpp"
#include "OWBI1_IntrusiveCountableBase.hpp"
#include "OWBI1_WBEMFlags.hpp"
#include "OWBI1_CommonFwd.hpp"
#include "OWBI1_CIMPropertyList.hpp"
#include "OWBI1_CIMName.hpp"

namespace OWBI1
{

/**
 * The CIMOMHandleIFC class is an abstract class used as an interface
 * definition for classes that provide access to a CIMOM.
 */
class OWBI1_OWBI1PROVIFC_API CIMOMHandleIFC : public IntrusiveCountableBase
{
public:
	/**
	 * Destroy this CIMOMHandleIFC object.
	 */
	virtual ~CIMOMHandleIFC();
	/**
	 * Close the connetion to the CIMOM. This will free resources used for the
	 * client session. The destructor will call close().
	 */
	virtual void close() = 0;
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param ns The namespace.
	 * @param className The class to be enumerated.
	 * @param result A callback object that will handle the classes as they are
	 *	received.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 * 	the names of all classes derived from the enumerated class. If
	 * 	set to E_SHALLOW the enumermation will return onlythe names of
	 * 	the first level children of the enumerated class.
	 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited
	 * 	properties are returned on each instance, otherwise all
	 * 	properties are returned.
	 * @exception CIMException If the specified class cannot be found
	 */
	virtual void enumClass(const String& ns,
		const CIMName& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY) = 0;

	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param ns The namespace.
	 * @param className The class to be enumerated.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 * 	the names of all classes derived from the enumerated class.
	 * 	If set to E_SHALLOW the enumermation will return only the names
	 * 	of the first level children of the enumerated class.
	 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited
	 * 	properties are returned on each instance, otherwise all
	 * 	properties are returned.
	 * @return An enumeration of CIMClass objects (CIMClassEnumeration)
	 * @exception CIMException If the specified class cannot be found
	 */
	virtual CIMClassEnumeration enumClassE(const String& ns,
		const CIMName& className,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY);
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param ns The namespace.
	 * @param className The class to be enumerated.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 * 	the names of all classes derived from the enumerated class.
	 *	If set to E_SHALLOW the enumermation will return only the names
	 *	of the first level children of the enumerated class.
	 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited
	 * 	properties are returned on each instance, otherwise all
	 * 	properties are returned.
	 * @return An array of CIMClass objects (CIMClassArray)
	 * @exception CIMException If the specified class cannot be found
	 */
	virtual CIMClassArray enumClassA(const String& ns,
		const CIMName& className,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY);

	/**
	 * Enumerates the child classes of className.
	 * @param ns The namespace.
	 * @param className The class to be enumerated. Pass an empty string if
	 *	you wish to enumerate all classes.
	 * @param result A callback object that will handle the classe names as
	 *	they are received.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 *	the names of all classes derived from the enumerated class.
 	 *	If set to E_SHALLOW the enumermation will return only the names
	 *	of the first level children of the enumerated class.
	 * @exception CIMException If the specified CIMObjectPath object cannot
	 *	be found.
	 */
	virtual void enumClassNames(
		const String& ns,
		const CIMName& className,
		CIMNameResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP) = 0;
	/**
	 * Enumerates the child classes of className.
	 * @param ns The namespace.
	 * @param className The class to be enumerated. Pass an empty string if
	 *	you wish to enumerate all classes.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 *	the names of all classes derived from the enumerated class.
	 *	If set to E_SHALLOW the enumermation will return only the names
	 *	of the first level children of the enumerated class.
	 * @return A CIMNameEnumeration of classnames.
	 * @exception CIMException If the specified CIMObjectPath object cannot
	 *	be found.
	 */
	virtual CIMNameEnumeration enumClassNamesE(
		const String& ns,
		const CIMName& className,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP);
	/**
	 * Enumerates the child classes of className.
	 * @param ns The namespace.
	 * @param className The class to be enumerated. Pass an empty string if
	 *	you wish to enumerate all classes.
	 * @param deep If set to E_DEEP, the enumeration returned will contain
	 *	the names of all classes derived from the enumerated class.
	 *	If set to E_SHALLOW the enumermation will return only the names
	 *	of the first level children of the enumerated class.
	 * @return A CIMNameArray of classnames.
	 * @exception CIMException If the specified CIMObjectPath object cannot
	 *	be found.
	 */
	virtual CIMNameArray enumClassNamesA(
		const String& ns,
		const CIMName& className,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP);
	/**
	 * Gathers all instances (the whole instance and not just the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param ns The namespace.
	 *
	 * @param className The class whose instances are to be enumerated.
	 *
	 * @param result A callback object that will handle the instances as
	 *	they are received.
	 *
	 * @param propertyList If not NULL and has 0 or more elements, the
	 * 	returned instances will not contain properties missing from
	 *	this list. If not NULL and it contains NO elements, then no
	 * 	properties will be included in the instances. If propertyList
	 * 	IS NULL, then it is ignored and all properties are returned
	 * 	with the instances subject to constraints specified in the
	 * 	other parameters.
	 *
	 * @exception CIMException If the object cannot be found.
	 */
	virtual void enumInstances(
		const String& ns,
		const CIMName& className,
		CIMInstanceResultHandlerIFC& result,
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * Returns all instances (the whole instance and not just the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param ns The namespace.
	 *
	 * @param className The class whose instances are to be enumerated.
	 *
	 * @param propertyList If not NULL and has 0 or more elements, the
	 *	returned instances will not contain properties missing from
	 *	this list. If not NULL and it contains NO elements, then no
	 *	properties will be included in the instances. If propertyList
	 *	IS NULL, then it is ignored and all properties are returned
	 *	with the instances subject to constraints specified in the
	 *	other parameters.
	 *
	 * @return An Enumeration of CIMInstance (CIMInstanceEnumeration)
	 * @exception CIMException If the object cannot be found
	 */
	virtual CIMInstanceEnumeration enumInstancesE(
		const String& ns,
		const CIMName& className,
		const CIMPropertyList& propertyList = CIMPropertyList());
	/**
	 * Returns all instances (the whole instance and not just the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param ns The namespace.
	 *
	 * @param className The class whose instances are to be enumerated.
	 *
	 * @param propertyList If not NULL and has 0 or more elements, the
	 * 	returned instances will not contain properties missing from
	 *	this list. If not NULL and it contains NO elements, then no
	 * 	properties will be included in the instances. If propertyList
	 *	IS NULL, then it is ignored and all properties are returned
	 *	with the instances subject to constraints specified in the
	 *	other parameters.
	 *
	 * @return An array of CIMInstances (CIMInstanceArray)
	 * @exception CIMException If the object cannot be found
	 */
	virtual CIMInstanceArray enumInstancesA(
		const String& ns,
		const CIMName& className,
		const CIMPropertyList& propertyList = CIMPropertyList());
	/**
	 * Gathers all instance names belonging to the class specified in the
	 * path. This could include instances of all the classes in the
	 * specified class' hierarchy.
	 *
	 * @param ns The namespace.
	 * @param className The class whose instances are to be enumerated.
	 * @param result A callback object that will handle the instances names
	 * 	as they are received.
	 *
	 * @exception CIMException If the object cannot be found.
	 */
	virtual void enumInstanceNames(
		const String& ns,
		const CIMName& className,
		CIMObjectPathResultHandlerIFC& result) = 0;
	/**
	 * Returns all instance names belonging to the class specified in the
	 * path. This could include instances of all the classes in the
	 * specified class' hierarchy.
	 *
	 * @param ns The namespace.
	 * @param className The class whose instances are to be enumerated.
	 * @return An Enumeration of CIMObjectPaths (CIMObjectPathEnumeration).
	 * @exception CIMException If the object cannot be found.
	 */
	virtual CIMObjectPathEnumeration enumInstanceNamesE(
		const String& ns,
		const CIMName& className);
	/**
	 * Returns all instance names belonging to the class specified in the path.
	 * This could include instances of all the classes in the specified class'
	 * hierarchy.
	 *
	 * @param ns The namespace.
	 * @param className The class whose instances are to be enumerated.
	 * @return An array of CIMObjectPaths (CIMObjectPathEnumeration)
	 * @exception CIMException If the object cannot be found
	 */
	virtual CIMObjectPathArray enumInstanceNamesA(
		const String& ns,
		const CIMName& className);
	/**
	 * Gets the CIM class for the specified CIM object path.
	 * @param ns The namespace
	 * @param classNname The CIM class
	 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited
	 * 	properties and methods are returned, otherwise all properties
	 * 	and methods are returned.
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all
	 *	class, property and method qualifiers will be included in the
	 * 	CIMClass.  Otherwise qualifiers will be excluded.
	 * @param includeClassOrigin If true, then the classOrigin attribute
	 * 	will be present on all appropriate elements of the CIMClass.
	 * @param propertyList If not NULL, only properties contained in this
	 * 	array will be included in the CIMClass. If NULL, all properties
	 * 	will be returned.
	 * @return CIMClass the CIM class indentified by the CIMObjectPath
	 * @exception CIMException If the namespace or the model path
	 * 	identifying the object cannot be found.
	 */
	virtual CIMClass getClass(
		const String& ns,
		const CIMName& className,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * Gets the CIM instance for the specified CIM object path.
	 *
	 * @param ns The namespace.
	 * 
	 * @param instanceName The CIMObjectPath that identifies this CIM instance
	 * 
	 * @param propertyList If not NULL and has 0 or more elements, the
	 *	returned instance will not contain properties missing from this
	 *	list. If not NULL and it contains NO elements, then no
	 *	properties will be included in the instance. If propertyList IS
	 *	NULL, then it is ignored and all properties are returned with
	 *	the instance subject to constraints specified in the other
	 *	parameters.
	 *
	 * @return The CIMInstance identified by the CIMObjectPath.
	 * 
	 * @exception CIMException If the specified CIMObjectPath cannot be found.
	 */
	virtual CIMInstance getInstance(
		const String& ns,
		const CIMObjectPath& instanceName,
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * Executes the specified method on the specified object. A method is a
	 * declaration containing the method name, return type, and parameters
	 * in the method.
	 * @param ns The namespace.
	 * @param path An CIMObjectPath that identifies the class or instance
	 * 	on which to invoke the method.
	 * @param methodName The string name of the method to be invoked
	 * @param inParams The input parameters specified as an CIMValueArray.
	 * @param outParams The output parameters.
	 * @return A non-null CIMValue on success. Otherwise a null CIMValue.
	 * @exception CIMException If the specified method cannot be found.
	 */
	virtual CIMValue invokeMethod(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& methodName,
		const CIMParamValueArray& inParams,
		CIMParamValueArray& outParams) = 0;
	/**
	 * Gets the CIM qualifier type specified in the CIM object path.
	 * @param ns The namespace
	 * @param qualifierName The name of the CIM qualifier type.
	 * @return An CIMQualifierType identified by name.
	 * @exception CIMException If the CIM qualifier type cannot be found.
	 */
	virtual CIMQualifierType getQualifierType(
		const String& ns,
		const CIMName& qualifierName) = 0;
	/**
	 * Updates the specified CIM qualifier type in the specified namespace
	 * if it exists. If it doesn't exist, it will be added.
	 * @param ns The namespace
	 * @param qualifierType The CIM qualifier type to be updated or added.
	 */
	virtual void setQualifierType(
		const String& ns,
		const CIMQualifierType& qualifierType) = 0;
	/**
	 * Deletes a CIM qualfier type.
	 * @param ns The namespace containing the qualifier type
	 * @param qualName The qualifier type to delete.
	 * @exception CIMException If the qualifier type does not exist.
	 */
	virtual void deleteQualifierType(const String& ns, const CIMName& qualName) = 0;
	/**
	 * Enumerates the qualifiers defined in a namespace.
	 * @param ns The namespace whose qualifier definitions are to be
	 * 	enumerated.
	 * @param result A callback object that will handle the qualifier types
	 * 	as they are received.
	 */
	virtual void enumQualifierTypes(
		const String& ns,
		CIMQualifierTypeResultHandlerIFC& result) = 0;
	/**
	 * Enumerates the qualifiers defined in a namespace.
	 * @param ns The namespace whose qualifier definitions are to be
	 *	enumerated.
	 * @return An Enumeration of CIMQualifierTypes
	 *	(CIMQualifierEnumeration).
	 */
	virtual CIMQualifierTypeEnumeration enumQualifierTypesE(
		const String& ns);
	/**
	 * Enumerates the qualifiers defined in a namespace.
	 * @param ns The namespace whose qualifier definitions are to be
	 * 	enumerated.
	 * @return An array of CIMQualifierTypes (CIMQualifierTypeArray).
	 */
	virtual CIMQualifierTypeArray enumQualifierTypesA(
		const String& ns);
	/**
	 * Updates the CIM class within the specified namespace.
	 * @param ns The namespace
	 * @param cimClass The CIMClass to be updated
	 * @exception CIMException If the class does not exists
	 */
	virtual void modifyClass(
		const String& ns,
		const CIMClass& cimClass) = 0;
	/**
	 * Adds the CIM class to the specified namespace.
	 * @param ns The namespace
	 * @param cimClass The CIMClass to be added
	 * @exception CIMException If the CIM class already exists in the
	 *	namespace.
	 */
	virtual void createClass(
		const String& ns,
		const CIMClass& cimClass) = 0;
	/**
	 * Deletes the CIM class specified by className from namespace ns.
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete.
	 * @exception CIMException If the object does not exist
	 */
	virtual void deleteClass(const String& ns, const CIMName& className) = 0;
	/**
	 * Update the specified CIM instance within the specified namespace.
	 * @param ns The namespace
	 * @param modifiedInstance The CIMInstance to be updated.
	 * @param propertyList The properties to modify.  0 means all properties.
	 * @exception CIMException
	 */
	virtual void modifyInstance(
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * Add the specified CIM instance to the specified namespace.
	 * @param ns The namespace
	 * @param instance the CIMInstance to be added
	 * @return An CIMObjectPath of the created instance.
	 * @exception CIMException If the CIM instance already exists in the
	 *	namespace
	 */
	virtual CIMObjectPath createInstance(
		const String& ns,
		const CIMInstance& instance) = 0;
	/**
	 * Deletes the CIM instance specified by path from namespace ns.
	 * @param ns The namespace containing the instance.
	 * @param path The CIMObjectPath identifying the instance to delete.
	 * @exception CIMException If the instance does not exist.
	 */
	virtual void deleteInstance(const String& ns, const CIMObjectPath& path) = 0;
	/**
	 * Set the specified CIM instance property.
	 * @param ns The namespace containing the instance to modify.
	 * @param instanceName An CIMObjectPath that identifies the CIM
	 * 	instance to be accessed.
	 * @param propertyName The name of the property to set the value on.
	 * @param newValue The new value for property propertyName.
	 * @exception CIMException
	 */
	virtual void setProperty(
		const String& ns,
		const CIMObjectPath& instanceName,
		const CIMName& propertyName,
		const CIMValue& newValue) = 0;
	/**
	 * Get the specified CIM instance property.
	 * @param ns The namespace
	 * @param instanceName A CIMObjectPath that identifies the CIM
	 *	instance to be accessed.
	 * @param propertyName	The name of the property to retrieve.
	 * @return The CIMvalue for property identified by propertyName.
	 * @exception CIMException
	 */
	virtual CIMValue getProperty(
		const String& ns,
		const CIMObjectPath& instanceName,
		const CIMName& propertyName) = 0;
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
	 * @exception CIMException - as defined in the associator method
	 */
	virtual void associatorNames(
		const String& ns,
		const CIMObjectPath& objectName,
		CIMObjectPathResultHandlerIFC& result,
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName()) = 0;
	/**
	 * This method is used to enumerate the names of CIM Objects (Classes
	 * or Instances that are associated to a particular CIM Object.
	 *
	 * @see associatorNames
	 *
	 * @return If successful, this method returns an enumeration of zero or
	 * 	more full CIM Class paths or Instance paths of Objects meeting
	 * 	the requested criteria.
	 */
	virtual CIMObjectPathEnumeration associatorNamesE(
		const String& ns,
		const CIMObjectPath& objectName,
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName());
	/**
	 * This method is used to enumerate the names of CIM Objects (Classes
	 * or Instances that are associated to a particular CIM Object.
	 *
	 * @see associatorNames
	 *
	 * @return If successful, this method returns an array of zero or more
	 * 	full CIM Class paths or Instance paths of Objects meeting the
	 * 	requested criteria.
	 */
  	virtual CIMObjectPathArray associatorNamesA(
		const String& ns,
		const CIMObjectPath& objectName,
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName());
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
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * This operation is used to enumerate CIM Objects (Classes or
	 * Instances) that are associated to a particular source CIM Object.
	 *
	 * @see associators
	 *
	 * @return If successful, the method returns an enumeration of zero or
	 * 	more CIM Instances meeting the requested criteria.  Since it is
	 * 	possible for CIM Objects from different hosts or namespaces to
	 * 	be associated, each returned Object includes location
	 * 	information.
	 */
  	virtual CIMInstanceEnumeration associatorsE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
	/**
	 * This operation is used to enumerate CIM Objects (Classes or
	 * Instances) that are associated to a particular source CIM Object.
	 *
	 * @see associators
	 *
	 * @return If successful, the method returns an array of zero or more
	 *	CIM Instances meeting the requested criteria.  Since it is
	 *	possible for CIM Objects from different hosts or namespaces to
	 *	be associated, each returned Object includes location
	 *	information.
	 */
	virtual CIMInstanceArray associatorsA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
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
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * This operation is used to enumerate CIM Classes that are associated
	 * to a particular source CIM Object.
	 *
	 * @see associatorsClasses
	 *
	 * @return If successful, the method returns an enumeration of zero or
	 * 	more CIM Classes meeting the requested criteria.  Since it is
	 * 	possible for CIM Objects from different hosts or namespaces to
	 * 	be associated, each returned Object includes location
	 * 	information.  If the ObjectName refers to a class, then classes
	 * 	are returned. Otherwise, the ObjectName refers to an instance,
	 * 	and instances are returned.
	 */
	virtual CIMClassEnumeration associatorsClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
	/**
	 * This operation is used to enumerate CIM Classes that are associated
	 * to a particular source CIM Object.
	 *
	 * @see associatorsClasses
	 *
	 * @return If successful, the method returns an array of zero or
	 * 	more CIM Classes meeting the requested criteria.  Since it is
	 * 	possible for CIM Objects from different hosts or namespaces to
	 * 	be associated, each returned Object includes location
	 * 	information.
	 */
	virtual CIMClassArray associatorsClassesA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass = CIMName(),
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMName& resultRole = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
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
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName()) = 0;
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Object (Class or Instance).
	 *
	 * @see referenceNames
	 *
	 * @return If successful, the method returns an enumeration of zero or
	 * 	more CIM Class paths or Instance paths meeting the requested
	 * 	criteria.  Since it is possible for CIM Objects from different
	 * 	hosts or namespaces to be associated, each returned Object
	 * 	includes location information.
	 *
	 * @exception CIMException As defined for associators method.
	 */
	virtual CIMObjectPathEnumeration referenceNamesE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName());
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Object (Class or Instance).
	 *
	 * @see referenceNames
	 *
	 * @return If successful, the method returns an enumeration of zero or
	 * 	more CIM Class paths or Instance paths meeting the requested
	 * 	criteria.  Since it is possible for CIM Objects from different
	 * 	hosts or namespaces to be associated, each returned Object
	 * 	includes location information.
	 *
	 * @exception CIMException As defined for associators method.
	 */
	virtual CIMObjectPathArray referenceNamesA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName());
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
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Object (Class or Instance).
	 *
	 * @see references
	 *
	 * @return If successful, the method returns an enumeration of zero or
	 *	more CIM Instances meeting the requested criteria.  Since it is
	 *	possible for CIM Objects from different hosts or namespaces to
	 *	be associated, each returned Object includes location
	 *	information.
	 *
	 * @exception CIMException - as defined for associators method.
	 */
	virtual CIMInstanceEnumeration referencesE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Object (Class or Instance).
	 *
	 * @see references
	 *
	 * @return If successful, the method returns an array of zero or
	 *	more CIM Classes or Instances meeting the requested criteria.
	 *	Since it is possible for CIM Objects from different hosts or
	 *	namespaces to be associated, each returned Object includes
	 *	location information.
	 *
	 * @exception CIMException - as defined for associators method.
	 */
	virtual CIMInstanceArray referencesA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
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
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList()) = 0;
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Class.
	 *
	 * @see references
	 *
	 * @return If successful, the method returns an enumeration of zero or
	 *	more CIM Classes meeting the requested criteria.  Since it is
	 *	possible for CIM Objects from different hosts or namespaces to
	 *	be associated, each returned Object includes location
	 *	information.
	 *
	 * @exception CIMException - as defined for associators method.
	 */
	virtual CIMClassEnumeration referencesClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
	/**
	 * This operation is used to enumerate the association objects that
	 * refer to a particular target CIM Class.
	 *
	 * @see references
	 *
	 * @return If successful, the method returns an array of zero or
	 *	more CIM Classes meeting the requested criteria.  Since it is
	 *	possible for CIM Objects from different hosts or namespaces to
	 *	be associated, each returned Object includes location
	 *	information.
	 *
	 * @exception CIMException - as defined for associators method.
	 */
	virtual CIMClassArray referencesClassesA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass = CIMName(),
		const CIMName& role = CIMName(),
		const CIMPropertyList& propertyList = CIMPropertyList());
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
		const String& query,
		const String& queryLanguage) = 0;
	/**
	 * Executes a query to retrieve or modify objects.
	 *
	 * @see execQuery
	 *
	 * @return CIMInstanceEnumeration A deep enumeration of all instances
	 *	of the specified class and all classes derived from the
	 *	specified class, that match the query string.
	 *
	 * @exception CIMException If any of the following errors occur:
	 *	The user does not have permission to view the result.
	 *	The requested query language is not supported.
	 *	The query specifies a class that does not exist.
	 */
	virtual CIMInstanceEnumeration execQueryE(
		const String& ns,
		const String& query,
		const String& queryLanguage);
	/**
	 * Executes a query to retrieve or modify objects.
	 *
	 * @see execQuery
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
	virtual CIMInstanceArray execQueryA(
		const String& ns,
		const String& query,
		const String& queryLanguage);
	
	/**
	 * Export a given instance of an indication.
	 * This will cause all CIMListerners that are interested in this type
	 * of indication to be notified.
	 * @param instance The indication instance to use in the notification.
	 */
	virtual void exportIndication(const CIMInstance& instance,
		const String& instNS);
};

} // end namespace OWBI1

#endif
