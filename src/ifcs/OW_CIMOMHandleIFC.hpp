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
#ifndef OW_CIMOMHANDLE_HPP_
#define OW_CIMOMHANDLE_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_Reference.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_String.hpp"
#include "OW_StringEnumeration.hpp"
#include "OW_WBEMFlags.hpp"

namespace OpenWBEM
{

/**
 * The CIMOMHandleIFC class is an abstract class used as an interface
 * definition for classes that provide access to a CIMOM.
 */
class CIMOMHandleIFC
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
	 *		received.
	 * @param deep If set to E_DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to E_SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited properties
	 *		are returned on each instance, otherwise all properties are returned.
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all class,
	 *		property and method qualifiers will be returned.
	 * @param includeClassOrigin If E_INCLUDE_CLASS_ORIGIN, then the class origin attribute will
	 *		be included with all appropriate elements of each class.
	 * @exception CIMException If the specified class cannot be found
	 */
	virtual void enumClass(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW, 
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN) = 0;
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param ns The namespace.
	 * @param className The class to be enumerated.
	 * @param deep If set to E_DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to E_SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited properties
	 *		are returned on each instance, otherwise all properties are returned.
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all class,
	 *		property and method qualifiers will be returned.
	 * @param includeClassOrigin If E_INCLUDE_CLASS_ORIGIN, then the class origin attribute will
	 *		be included with all appropriate elements of each class.
	 * @return An enumeration of CIMClass objects (CIMClassEnumeration)
	 * @exception CIMException If the specified class cannot be found
	 */
	virtual CIMClassEnumeration enumClassE(const String& ns,
		const String& className,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW, 
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN);
	/**
	 * Enumerates the child classes of className.
	 * @param ns The namespace.
	 * @param className The class to be enumerated. Pass an empty string if
	 *		you wish to enumerate all classes.
	 * @param deep If set to E_DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to E_SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @return An enumeration of CIMObjectPath objects
	 *		(CIMObjectPathEnumeration)
	 * @exception CIMException If the specified CIMObjectPath object
	 *		cannot be found
	 */
	virtual void enumClassNames(
		const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP) = 0;
	virtual StringEnumeration enumClassNamesE(
		const String& ns,
		const String& className,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP);
	/**
	 * Returns all instances (the whole instance and not just the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param ns The namespace.
	 *
	 * @param className The class whose instances are to be enumerated.
	 *
	 * @param deep Return properties defined on subclasses of the class in path
	 *
	 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited properties
	 *	are returned on each instance, otherwise all inherited properties are
	 *	returned.
	 *
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all of the
	 *	qualifiers from the class will be returned with the each instance.
	 *
	 * @param includeClassOrigin If E_INCLUDE_CLASS_ORIGIN, then the class origin attribute will
	 * be returned on all appropriate elements.
	 *
	 * @param propertyList If not NULL and has 0 or more elements, the returned
	 * instances will not contain properties missing from this list. If not NULL
	 * and it contains NO elements, then no properties will be included in the
	 * instances. If propertyList IS NULL, then it is ignored and all properties
	 * are returned with the instances subject to constraints specified in the
	 * other parameters.
	 *
	 * @return An Enumeration of CIMInstance (CIMInstanceEnumeration)
	 * @exception CIMException 	If the object cannot be found
	 */
	virtual void enumInstances(
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0) = 0;
	virtual CIMInstanceEnumeration enumInstancesE(
		const String& ns,
		const String& className,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0);
	/**
	 * Returns all instance names belonging to the class specified in the path.
	 * This could include instances of all the classes in the specified class'
	 * hierarchy.
	 *
	 * @param ns The namespace.
	 * @param className The class whose instances are to be enumerated.
	 * @return An Enumeration of CIMObjectPaths (CIMObjectPathEnumeration)
	 * @exception CIMException 	If the object cannot be found
	 */
	virtual void enumInstanceNames(
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result) = 0;
	virtual CIMObjectPathEnumeration enumInstanceNamesE(
		const String& ns,
		const String& className);
	/**
	 * Gets the CIM class for the specified CIM object path.
	 * @param ns The namespace
	 * @param classNname The CIM class
	 * @param localOnly	If set to E_LOCAL_ONLY, only the non-inherited properties
	 *		and methods are returned, otherwise all properties and methods are
	 *		returned.
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all class,
	 *		property and method qualifiers will be included in the CIMClass.
	 *		Otherwise qualifiers will be excluded.
	 * @param includeClassOrigin If true, then the classOrigin attribute will
	 *		be present on all appropriate elements of the CIMClass
	 * @param propertyList If not NULL, only properties contained in this array
	 *		will be included in the CIMClass. If NULL, all properties will
	 *		be returned.
	 * @return CIMClass the CIM class indentified by the CIMObjectPath
	 * @exception CIMException If the namespace or the model path identifying
	 *										the object cannot be found
	 */
	virtual CIMClass getClass(
		const String& ns,
		const String& className,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0) = 0;
	/**
	 * Gets the CIM instance for the specified CIM object path.
	 *
	 * @param ns The namespace.
	 * @param instanceName The CIMObjectPath that identifies this CIM instance
	 * @param localOnly	If set to E_LOCAL_ONLY, only the non-inherited properties
	 *		are returned, otherwise all properties are returned.
	 *
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all of the
	 *		qualifiers from the class will be returned with the instance.
	 *		Otherwise no qualifiers will be included with the instance.
	 *
	 * @param includeClassOrigin If true, then the class origin attribute will
	 * 		be returned on all appropriate elements
	 *
	 * @param propertyList If not NULL and has 0 or more elements, the returned
	 * instance will not contain properties missing from this list. If not NULL
	 * and it contains NO elements, then no properties will be included in the
	 * instance. If propertyList IS NULL, then it is ignored and all properties
	 * are returned with the instance subject to constraints specified in the
	 * other parameters.
	 *
	 * @return The CIMInstance identified by the CIMObjectPath
	 * @exception CIMException If the specified CIMObjectPath cannot be
	 *										found
	 */
	virtual CIMInstance getInstance(
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0) = 0;
	/**
	 * Executes the specified method on the specified object. A method is a
	 * declaration containing the method name, return type, and parameters in
	 * the method.
	 * @param ns The namespace
	 * @param path An CIMObjectPath that identifies the class or instance
	 *  on which to invoke the method.
	 * @param methodName The string name of the method to be invoked
	 * @param inParams The input parameters specified as an CIMValueArray.
	 * @param outParams	The output parameters.
	 * @return A non-null CIMValue on success. Otherwise a null CIMValue.
	 * @exception CIMException	If the specified method cannot be found
	 */
	virtual CIMValue invokeMethod(
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& inParams,
		CIMParamValueArray& outParams) = 0;
	/**
	 * Gets the CIM qualifier type specified in the CIM object path.
	 * @param ns The namespace
	 * @param qualifierName The name of the CIM qualifier type.
	 * @return An CIMQualifierType identified by name.
	 * @exception CIMException If the CIM qualifier type cannot be found
	 */
	virtual CIMQualifierType getQualifierType(
		const String& ns,
		const String& qualifierName) = 0;
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Updates the specified CIM qualifier type in the specified namespace if
	 * it exist. If it doesn't exist, it will be added.
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
	virtual void deleteQualifierType(const String& ns, const String& qualName) = 0;
	/**
	 * Enumerates the qualifiers defined in a namespace.
     * @param ns	The namespace whose qualifier definitions are to be enumerated.
	 * @return 	An Enumeration of CIMQualifierTypes
	 *				(CIMQualifierEnumeration)
	 * @exception CIMException	If the specified CIMObjectPath cannot be
	 *										found
	 */
	virtual void enumQualifierTypes(
		const String& ns,
		CIMQualifierTypeResultHandlerIFC& result) = 0;
	virtual CIMQualifierTypeEnumeration enumQualifierTypesE(
		const String& ns);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Updates the CIM class associated with the specified namespace.
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
	 *		namespace.
	 */
	virtual void createClass(
		const String& ns,
		const CIMClass& cimClass) = 0;
	/**
	 * Deletes the CIM class specified by className in namespace ns.
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete
	 * @exception CIMException If the object does not exist
	 */
	virtual void deleteClass(const String& ns, const String& className) = 0;
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Update the specified CIM instance associated with the specified
	 * namespace.
	 * @param ns The namespace
	 * @param modifiedInstance The CIMInstance to be updated.
	 * @param includeQualifiers Modify the qualifiers
	 * @param propertyList The properties to modify.  0 means all properties.
	 * @exception CIMException
	 */
	virtual void modifyInstance(
		const String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		const StringArray* propertyList = 0) = 0;
	/**
	 * Add the specified CIM instance to the specified namespace.
	 * @param ns The namespace
	 * @param instance the CIMInstance to be added
	 * @return An CIMObjectPath of the created instance.
	 * @exception CIMException If the CIM instance already exists in the
	 *		namespace
	 */
	virtual CIMObjectPath createInstance(
		const String& ns,
		const CIMInstance& instance) = 0;
	/**
	 * Deletes the CIM instance specified by path in namespace ns.
	 * @param ns The namespace containing the instance
	 * @param path	The CIMObjectPath identifying the instance to delete.
	 * @exception CIMException If the instance does not exist.
	 */
	virtual void deleteInstance(const String& ns, const CIMObjectPath& path) = 0;
	/**
	 * Set the specified CIM instance property.
	 * @param ns The namespace
	 * @param instanceName An CIMObjectPath that identifies the CIM instance
	 *		to be accessed
	 * @param propertyName The name of the property to set the value on.
	 * @param newValue The new value for property propertyName.
	 * @exception CIMException
	 */
	virtual void setProperty(
		const String& ns,
		const CIMObjectPath& instanceName,
		const String& propertyName,
		const CIMValue& newValue) = 0;
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Get the specified CIM instance property.
	 * @param ns The namespace
	 * @param instanceName An CIMObjectPath that identifies the CIM instance to be
	 *		accessed
	 * @param propertyName	The name of the property to retrieve.
	 * @return The CIMvalue for property identified by propertyName.
	 * @exception CIMException
	 */
	virtual CIMValue getProperty(
		const String& ns,
		const CIMObjectPath& instanceName,
		const String& propertyName) = 0;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * This method is used to enumerate the names of CIM Objects (Classes or
	 * Instances) that are associated to a particular CIM Object.
	 *
	 * @param objectName Defines the source CIM Object whose associated
	 *		Objects are to be returned. This may be either a Class name or
	 *		Instance name (modelpath).
	 * @param assocClass The AssocClass input parameter, if not NULL, MUST be a
	 * 		valid CIM Association Class name. It acts as a filter on the
	 *		returned set of Objects by mandating that each returned Object MUST
	 *		be associated to the source Object via an Instance of this Class or
	 *		one of its subclasses.
	 * @param resultClass The ResultClass input parameter, if not NULL, MUST be
	 * 		a valid CIM Class name. It acts as a filter on the returned set of
	 * 		Objects by mandating that each returned Object MUST be either an
	 *		Instance of this Class (or one of its subclasses) or be this Class
	 *		(or one of its subclasses).
	 * @param role The Role input parameter, if not NULL, MUST be a valid
	 * 		Property name. It acts as a filter on the returned set of Objects by
	 * 		mandating that each returned Object MUST be associated to the source
	 * 		Object via an Association in which the source Object plays the
	 *		specified role (i.e. the name of the Property in the Association
	 *		Class that refers to the source Object MUST match the value of this
	 *		parameter).
	 * @param resultRole The ResultRole input parameter, if not NULL, MUST be a
	 * 		valid Property name. It acts as a filter on the returned set of
	 *		Objects by mandating that each returned Object MUST be associated
	 *		to the source Object via an Association in which the returned Object
	 *		plays the specified role (i.e. the name of the Property in the
	 *		Association Class that refers to the returned Object MUST match the
	 *		value of this parameter).
	 *
	 * @return If successful, the method returns zero or more full CIM Class
	 *		paths or Instance paths of Objects meeting the requested criteria.
	 *
	 * @exception CIMException - as defined in the associator method
	 */
	virtual void associatorNames(
		const String& ns,
		const CIMObjectPath& objectName,
		CIMObjectPathResultHandlerIFC& result,
		const String& assocClass = String(),
		const String& resultClass = String(),
		const String& role = String(),
		const String& resultRole = String()) = 0;
	virtual CIMObjectPathEnumeration associatorNamesE(
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass = String(),
		const String& resultClass = String(),
		const String& role = String(),
		const String& resultRole = String());
	/**
	 * This operation is used to enumerate CIMInstances
	 * that are associated to a particular source CIM Instance.
	 *
	 * @param objectName Defines the source CIM Instance whose associated Instances
	 * are to be returned.
	 *
	 * @param assocClass The AssocClass input parameter, if not "", MUST be a
	 * valid CIM Association Class name. It acts as a filter on the returned set
	 * of Objects by mandating that each returned Object MUST be associated to
	 * the source Object via an Instance of this Class or one of its subclasses.
	 *
	 * @param resultClass The ResultClass input parameter, if not "", MUST be
	 * a valid CIM Class name. It acts as a filter on the returned set of
	 * Objects by mandating that each returned Object MUST be either an Instance
	 * of this Class (or one of its subclasses) or be this Class (or one of its
	 * subclasses).
	 *
	 * @param role The Role input parameter, if not NULL, "" be a valid
	 * Property name. It acts as a filter on the returned set of Objects by
	 * mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the source Object plays the specified
	 * role (i.e. the name of the Property in the Association Class that refers
	 * to the source Object MUST match the value of this parameter).
	 *
	 * @param resultRole The ResultRole input parameter, if not "", MUST be a
	 * valid Property name. It acts as a filter on the returned set of Objects
	 * by mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the returned Object plays the
	 * specified role (i.e. the name of the Property in the Association Class
	 * that refers to the returned Object MUST match the value of this
	 * parameter).
	 *
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS then all
	 *		Qualifiers for each Object (including Qualifiers on the Object and
	 *		on any returned Properties) MUST be included as elements in the
	 *		response.If set to E_EXCLUDE_QUALIFIERS, then no qualifiers will be
	 *		present in the returned object(s).
	 *
	 * @param includeClassOrigin If the IncludeClassOrigin input parameter is
	 * true, this specifies that the CLASSORIGIN attribute MUST be present on
	 * all appropriate elements in each returned Object. If false, no
	 * CLASSORIGIN attributes are present in each returned Object.
	 *
	 * @param propertyList If the PropertyList input parameter is not NULL, the
	 * members of the array define one or more Property names. Each returned
	 * Object MUST NOT include elements for any Properties missing from this
	 * list. Note that if LocalOnly is specified as true (or DeepInheritance is
	 * specified as false) this acts as an additional filter on the set of
	 * Properties returned (for example, if Property A is included in the
	 * PropertyList but LocalOnly is set to true and A is not local to a
	 * returned Instance, then it will not be included in that Instance). If the
	 * PropertyList input parameter is an empty array this signifies that no
	 * Properties are included in each returned Object. If the PropertyList input
	 * parameter is NULL this specifies that all Properties (subject to the
	 * conditions expressed by the other parameters) are included in each
	 * returned Object. If the PropertyList contains duplicate elements, the
	 * Server ignores the duplicates but otherwise process the request normally.
	 * If the PropertyList contains elements which are invalid Property names for
	 * any target Object, the Server ignores such entries but otherwise process
	 * the request normally. Clients SHOULD NOT explicitly specify properties in
	 * the PropertyList parameter unless they have specified a non-NULL value for
	 * the ResultClass parameter.
	 *
	 * @return If successful, the method returns zero or more CIM Instances
	 * meeting the requested criteria.
	 *
	 * @exception CIMException. The following IDs can be expected.
	 * CIM_ERR_ACCESS_DENIED CIM_ERR_NOT_SUPPORTED CIM_ERR_INVALID_NAMESPACE
	 * CIM_ERR_INVALID_PARAMETER (including missing, duplicate, unrecognized or
	 * otherwise incorrect parameters) CIM_ERR_FAILED (some other unspecified
	 * error occurred)
	 */
	virtual void associators(
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& assocClass = String(),
		const String& resultClass = String(),
		const String& role = String(),
		const String& resultRole = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0) = 0;
	virtual CIMInstanceEnumeration associatorsE(
		const String& ns,
		const CIMObjectPath& path,
		const String& assocClass = String(),
		const String& resultClass = String(),
		const String& role = String(),
		const String& resultRole = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0);
	/**
	 * This operation is used to enumerate CIMClasses
	 * that are associated to a particular source CIM Object.
	 *
	 * @param objectName Defines the source CIM Class whose associated classes
	 * are to be returned.
	 *
	 * @param assocClass The AssocClass input parameter, if not "", MUST be a
	 * valid CIM Association Class name. It acts as a filter on the returned set
	 * of Objects by mandating that each returned Object MUST be associated to
	 * the source Object via an Instance of this Class or one of its subclasses.
	 *
	 * @param resultClass The ResultClass input parameter, if not "", MUST be
	 * a valid CIM Class name. It acts as a filter on the returned set of
	 * Objects by mandating that each returned Object MUST be either an Instance
	 * of this Class (or one of its subclasses) or be this Class (or one of its
	 * subclasses).
	 *
	 * @param role The Role input parameter, if not NULL, "" be a valid
	 * Property name. It acts as a filter on the returned set of Objects by
	 * mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the source Object plays the specified
	 * role (i.e. the name of the Property in the Association Class that refers
	 * to the source Object MUST match the value of this parameter).
	 *
	 * @param resultRole The ResultRole input parameter, if not "", MUST be a
	 * valid Property name. It acts as a filter on the returned set of Objects
	 * by mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the returned Object plays the
	 * specified role (i.e. the name of the Property in the Association Class
	 * that refers to the returned Object MUST match the value of this
	 * parameter).
	 *
	 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS then all
	 *		Qualifiers for each Object (including Qualifiers on the Object and
	 *		on any returned Properties) MUST be included as elements in the
	 *		response.If set to E_EXCLUDE_QUALIFIERS, then no qualifiers will be
	 *		present in the returned object(s).
	 *
	 * @param includeClassOrigin If the IncludeClassOrigin input parameter is
	 * true, this specifies that the CLASSORIGIN attribute MUST be present on
	 * all appropriate elements in each returned Object. If false, no
	 * CLASSORIGIN attributes are present in each returned Object.
	 *
	 * @param propertyList If the PropertyList input parameter is not NULL, the
	 * members of the array define one or more Property names. Each returned
	 * Object MUST NOT include elements for any Properties missing from this
	 * list. Note that if LocalOnly is specified as true (or DeepInheritance is
	 * specified as false) this acts as an additional filter on the set of
	 * Properties returned (for example, if Property A is included in the
	 * PropertyList but LocalOnly is set to true and A is not local to a
	 * returned Instance, then it will not be included in that Instance). If the
	 * PropertyList input parameter is an empty array this signifies that no
	 * Properties are included in each returned Object. If the PropertyList input
	 * parameter is NULL this specifies that all Properties (subject to the
	 * conditions expressed by the other parameters) are included in each
	 * returned Object. If the PropertyList contains duplicate elements, the
	 * Server ignores the duplicates but otherwise process the request normally.
	 * If the PropertyList contains elements which are invalid Property names for
	 * any target Object, the Server ignores such entries but otherwise process
	 * the request normally. Clients SHOULD NOT explicitly specify properties in
	 * the PropertyList parameter unless they have specified a non-NULL value for
	 * the ResultClass parameter.
	 *
	 * @return If successful, the method returns zero or more CIM Instances
	 * meeting the requested criteria.
	 *
	 * @exception CIMException. The following IDs can be expected.
	 * CIM_ERR_ACCESS_DENIED CIM_ERR_NOT_SUPPORTED CIM_ERR_INVALID_NAMESPACE
	 * CIM_ERR_INVALID_PARAMETER (including missing, duplicate, unrecognized or
	 * otherwise incorrect parameters) CIM_ERR_FAILED (some other unspecified
	 * error occurred)
	 */
	virtual void associatorsClasses(
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& assocClass = String(),
		const String& resultClass = String(),
		const String& role = String(),
		const String& resultRole = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0) = 0;
	virtual CIMClassEnumeration associatorsClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const String& assocClass = String(),
		const String& resultClass = String(),
		const String& role = String(),
		const String& resultRole = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0);
	/**
	 * This operation is used to enumerate the association objects that refer to
	 * a particular target CIM Object.
	 * @param objectName		refer to reference method.
	 * @param resultClass	refer to reference method.
	 * @param role				refer to reference method.
	 *
	 * @return If successful, the method returns the names of zero or more full
	 * CIM Class paths or Instance paths of Objects meeting the requested
	 * criteria.
	 *
	 * @exception CIMException As defined for associators method.
	 */
	virtual void referenceNames(
		const String& ns,
		const CIMObjectPath& path,
		CIMObjectPathResultHandlerIFC& result,
		const String& resultClass = String(),
		const String& role = String()) = 0;
	virtual CIMObjectPathEnumeration referenceNamesE(
		const String& ns,
		const CIMObjectPath& path,
		const String& resultClass = String(),
		const String& role = String());
	/**
	 * This operation is used to enumerate the association objects that refer to
	 * a particular target CIM Object (Class or Instance).
	 *
	 * @param objectName The ObjectName input parameter defines the target CIM
	 * Object whose referring Objects are to be returned. This is either a Class
	 * name or Instance name (model path).
	 *
	 * @param resultClass The ResultClass input parameter, if not NULL, MUST be
	 * a valid CIM Class name. It acts as a filter on the returned set of
	 * Objects by mandating that each returned Object MUST be an Instance of this
	 * Class (or one of its subclasses), or this Class (or one of its
	 * subclasses).
	 *
	 * @param role The Role input parameter, if not NULL, MUST be a valid
	 * Property name. It acts as a filter on the returned set of Objects by
	 * mandating that each returned Objects MUST refer to the target Object via
	 * a Property whose name matches the value of this parameter.
	 *
	 * @param includeQualifiers Refer to includeQualifiers for associators.
	 *
	 * @param includeClassOrigin Refer to includeQualifiers for associators.
	 *
	 * @param propertyList Refer to includeQualifiers for associators.
	 *
	 * @return If successful, the method returns zero or more CIM Classes or
	 * Instances meeting the requested criteria.
	 *
	 * @exception CIMException - as defined for associators method.
	 */
	virtual void references(
		const String& ns,
		const CIMObjectPath& path,
		CIMInstanceResultHandlerIFC& result,
		const String& resultClass = String(),
		const String& role = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0) = 0;
	virtual CIMInstanceEnumeration referencesE(
		const String& ns,
		const CIMObjectPath& path,
		const String& resultClass = String(),
		const String& role = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0);
	virtual void referencesClasses(
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& resultClass = String(),
		const String& role = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0) = 0;
	virtual CIMClassEnumeration referencesClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const String& resultClass = String(),
		const String& role = String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList = 0);
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * Executes a query to retrieve or modify objects.
	 *
	 * @param ns CIMNameSpace that identifies the namespace in which to query.
	 *
	 * @param query A string containing the text of the query. This parameter
	 * 		cannot be null.
	 *
	 * @param queryLanguage A string containing the query language the query is
	 * 		written in.
	 *
	 * @return CIMInstanceArray A deep enumeration of all instances of the
	 *		specified class and all classes derived from the specified class,
	 *		that match the query string.
	 *
	 * @exception CIMException If any of the following errors occur:
	 *		The user does not have permission to view the result.
	 *		The requested query language is not supported.
	 *		The query specifies a class that does not exist.
	 */
	virtual void execQuery(
		const String& ns,
		CIMInstanceResultHandlerIFC& result,
		const String& query,
		const String& queryLanguage) = 0;
	virtual CIMInstanceEnumeration execQueryE(
		const String& ns,
		const String& query,
		const String& queryLanguage);
	
	/**
	 * @return The features of the CIMOM this CIMOMHandleIFC is connected to as
	 * an CIMFeatures object.
	 */
	virtual CIMFeatures getServerFeatures();
	/**
	 * Export a given instance of an indication.
	 * This will cause all CIMListerners that are interested in this type
	 * of indication to be notified.
	 * @param instance	The indication instance to use in the notification.
	 */
	virtual void exportIndication(const CIMInstance& instance,
		const String& instNS);
};
typedef Reference<CIMOMHandleIFC> CIMOMHandleIFCRef;

} // end namespace OpenWBEM

#endif
