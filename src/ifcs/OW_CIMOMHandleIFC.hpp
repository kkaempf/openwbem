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

#ifndef OW_CIMOMHANDLE_HPP_
#define OW_CIMOMHANDLE_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_Bool.hpp"
#include "OW_Reference.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_String.hpp"

/**
 * The OW_CIMOMHandleIFC class is an abstract class used as an interface
 * definition for classes that provide access to a CIMOM.
 */
class OW_CIMOMHandleIFC
{
public:

	/**
	 * Constant for deep specification for CIMOM handle calls.
	 * Set to true.
	 */
	static const OW_Bool DEEP;

	/**
	 * Constant for non-deep specification for CIMOM handle calls.
	 * Set to false
	 */
	static const OW_Bool SHALLOW;

	/**
	 * Constant for include qualifier specification on CIMOM handle calls.
	 * Set to true.
	 */
	static const OW_Bool INCLUDE_QUALIFIERS;

	/**
	 * Constant for exclude qualifier specification on CIMOM handle calls.
	 * Set to false.
	 */
	static const OW_Bool EXCLUDE_QUALIFIERS;

	/**
	 * Constant for include class origin specification on CIMOM handle calls.
	 * Set to true.
	 */
	static const OW_Bool INCLUDE_CLASS_ORIGIN;

	/**
	 * Constant for exclude class origin specification on CIMOM handle calls.
	 * Set to false.
	 */
	static const OW_Bool EXCLUDE_CLASS_ORIGIN;

	/**
	 * Constant for local only specification on CIMOM handle calls.
	 * Set to true.
	 */
	static const OW_Bool LOCAL_ONLY;

	/**
	 * Constant for non-local only specification on CIMOM handle calls.
	 * Set to false.
	 */
	static const OW_Bool NOT_LOCAL_ONLY;

	/**
	 * Create a new OW_CIMOMHandleIFC object.
	 */
	OW_CIMOMHandleIFC() {}

	/**
	 * Destroy this OW_CIMOMHandleIFC object.
	 */
	virtual ~OW_CIMOMHandleIFC() {}

	/**
	 * Close the connetion to the CIMOM. This will free resources used for the
	 * client session.
	 */
	virtual void close() = 0;

	/**
	 * Create a cim namespace.
	 * @param ns 	The namespace name to be created.
	 * @exception OW_CIMException If the namespace already exists.
	 */
	virtual void createNameSpace(const OW_String& ns) = 0;

	/**
	 * Delete a specified namespace.
	 * @param ns	The namespace to delete.
	 * @exception OW_CIMException If the namespace does not exist.
	 */
	virtual void deleteNameSpace(const OW_String& ns) = 0;

	/**
	 * Gets a list of the namespaces within the namespace specified by the CIM
	 * object path.
	 * @param ns The parent namespace to enumerate
	 * @param deep If set to DEEP, the enumeration returned will contain the
	 *		entire hierarchy of namespaces present under the enumerated
	 *		namespace. If set to SHALLOW  the enuermation will return only the
	 *		first level children of the enumerated namespace.
	 * @return An Array of namespace names as strings.
	 * @exception OW_CIMException If the namespace does not exist or the object
	 *		cannot be found in the specified namespace.
	 */
	virtual OW_StringArray enumNameSpaceE(const OW_String& ns,
		OW_Bool deep=DEEP);

	virtual void enumNameSpace(const OW_String& ns,
		OW_StringResultHandlerIFC& result,
		OW_Bool deep=DEEP) = 0;

	/**
	 * Deletes the CIM class specified by className in namespace ns.
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete
	 * @exception OW_CIMException If the object does not exist
	 */
	virtual void deleteClass(const OW_String& ns, const OW_String& className) = 0;

	/**
	 * Deletes the CIM instance specified by the CIM object path.
	 * A CIM object path consists of two  parts: namespace + model path. The
	 * model path is created by concatenating the properties of a class that are
	 * qualified with the KEY qualifier.
	 * @param ns The namespace containing the instance
	 * @param path	The OW_CIMObjectPath identifying the instance to delete.
	 * @exception OW_CIMException If the instance does not exist.
	 */
	virtual void deleteInstance(const OW_String& ns, const OW_CIMObjectPath& path) = 0;

	/**
	 * Deletes a CIM qualfier type.
	 * @param ns The namespace containing the qualifier type
	 * @param qualName The qualifier type to delete.
	 * @exception OW_CIMException If the qualifier type does not exist.
	 */
	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName) = 0;

	/**
	 * Enumerates the class specified by the OW_CIMObjectPath.
	 * @param ns The namespace.
	 * @param className The class to be enumerated.
	 * @param deep If set to DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If set to LOCAL_ONLY, only the non-inherited properties
	 *		are returned on each instance, otherwise all properties are returned.
	 * @param includeQualifiers If set to INCLUDE_QUALIFIERS, then all class,
	 *		property and method qualifiers will be returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be included with all appropriate elements of each class.
	 * @return An enumeration of OW_CIMClass objects (OW_CIMClassEnumeration)
	 * @exception OW_CIMException If the specified CIMObjectPath object cannot
	 *		be found
	 */
	virtual void enumClass(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep=SHALLOW, OW_Bool localOnly=NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers=INCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=INCLUDE_CLASS_ORIGIN) = 0;

	virtual OW_CIMClassEnumeration enumClassE(const OW_String& ns,
		const OW_String& className,
		OW_Bool deep=SHALLOW,
		OW_Bool localOnly = OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers = OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin = OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN);

	/**
	 * Enumerates the class specified by className.
	 * @param ns The namespace.
	 * @param className The class to be enumerated.
	 * @param deep If set to DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @return An enumeration of OW_CIMObjectPath objects
	 *		(OW_CIMObjectPathEnumeration)
	 * @exception OW_CIMException  	If the specified CIMObjectPath object
	 *											cannot be found
	 */
	virtual void enumClassNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep=DEEP) = 0;

	virtual OW_CIMObjectPathEnumeration enumClassNamesE(
		const OW_String& ns,
		const OW_String& className,
		OW_Bool deep=DEEP);

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
	 * @param localOnly If set to LOCAL_ONLY, only the non-inherited properties
	 *	are returned on each instance, otherwise all inherited properties are
	 *	returned.
	 *
	 * @param includeQualifiers If set to INCLUDE_QUALIFIERS, then all of the
	 *	qualifiers from the class will be returned with the each instance.
	 *
	 * @param includeClassOrigin If true, then the class origin attribute will
	 * be returned on all appropriate elements.
	 *
	 * @param propertyList If not NULL and has 0 or more elements, the returned
	 * instances will not contain properties missing from this list. If not NULL
	 * and it contains NO elements, then no properties will be included in the
	 * instances. If propertyList IS NULL, then it is ignored and all properties
	 * are returned with the instances subject to constraints specified in the
	 * other parameters.
	 *
	 * @return An Enumeration of OW_CIMInstance (OW_CIMInstanceEnumeration)
	 * @exception OW_CIMException 	If the object cannot be found
	 */
	virtual void enumInstances(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool deep = DEEP,
		OW_Bool localOnly = NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers = EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin = EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0) = 0;

	virtual OW_CIMInstanceEnumeration enumInstancesE(
		const OW_String& ns,
		const OW_String& className,
		OW_Bool deep = DEEP,
		OW_Bool localOnly = NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers = EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin = EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0);

	/**
	 * Returns all instance names belonging to the class specified in the path.
	 * This could include instances of all the classes in the specified class'
	 * hierarchy.
	 *
	 * @param ns The namespace.
	 * @param className The class whose instances are to be enumerated.
	 * @return An Enumeration of OW_CIMObjectPaths (OW_CIMObjectPathEnumeration)
	 * @exception OW_CIMException 	If the object cannot be found
	 */
	virtual void enumInstanceNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result) = 0;

	virtual OW_CIMObjectPathEnumeration enumInstanceNamesE(
		const OW_String& ns,
		const OW_String& className);

	/**
	 * Enumerates the qualifiers defined in a namespace.
    * @param ns	The namespace whose qualifier definitions are to be enumerated.
	 * @return 	An Enumeration of OW_CIMQualifierTypes
	 *				(OW_CIMQualifierEnumeration)
	 * @exception OW_CIMException	If the specified OW_CIMObjectPath cannot be
	 *										found
	 */
	virtual void enumQualifierTypes(
		const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result) = 0;

	virtual OW_CIMQualifierTypeEnumeration enumQualifierTypesE(
		const OW_String& ns);

	/**
	 * Gets the CIM class for the specified CIM object path.
	 * @param ns The namespace
	 * @param classNname The CIM class
	 * @param localOnly	If set to LOCAL_ONLY, only the non-inherited properties
	 *		and methods	are returned, otherwise all properties and methods are
	 *		returned.
	 * @param includeQualifiers If set to INCLUDE_QUALIFIERS, then all class,
	 *		property and method qualifiers will be included in the OW_CIMClass.
	 *		Otherwise qualifiers will be excluded.
	 * @param includeClassOrigin If true, then the classOrigin attribute will
	 *		be present on all appropriate elements of the OW_CIMClass
	 * @param propertyList If not NULL, only properties contained in this array
	 *		will be included in the OW_CIMClass. If NULL, all properties will
	 *		be returned.
	 * @return OW_CIMClass the CIM class indentified by the CIMObjectPath
	 * @exception OW_CIMException If the namespace or the model path identifying
	 *										the object cannot be found
	 */
	virtual OW_CIMClass getClass(
		const OW_String& ns,
		const OW_String& className,
		OW_Bool localOnly = NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers = INCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin = INCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList = 0) = 0;

	/**
	 * Gets the CIM instance for the specified CIM object path.
	 *
	 * @param ns The namespace.
	 * @param instanceName The OW_CIMObjectPath that identifies this CIM instance
	 * @param localOnly	If set to LOCAL_ONLY, only the non-inherited properties
	 *		are returned, otherwise all properties are returned.
	 *
	 * @param includeQualifiers If set to INCLUDE_QUALIFIERS, then all of the
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
	 * @return The OW_CIMInstance identified by the OW_CIMObjectPath
	 * @exception OW_CIMException If the specified OW_CIMObjectPath cannot be
	 *										found
	 */
	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly = NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers = EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin = EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0) = 0;

	/**
	 * Executes the specified method on the specified object. A method is a
	 * declaration containing the method name, return type, and parameters in
	 * the method.
	 * @param ns The namespace
	 * @param path An OW_CIMObjectPath that identifies the class or instance
	 *  on which to invoke the method.
	 * @param methodName The string name of the method to be invoked
	 * @param inParams The input parameters specified as an OW_CIMValueArray.
	 * @param outParams	The output parameters.
	 * @return A non-null OW_CIMValue on success. Otherwise a null OW_CIMValue.
	 * @exception OW_CIMException	If the specified method cannot be found
	 */
	virtual OW_CIMValue invokeMethod(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& methodName,
		const OW_CIMParamValueArray& inParams,
		OW_CIMParamValueArray& outParams) = 0;

	/**
	 * Gets the CIM qualifier type specified in the CIM object path.
	 * @param ns The namespace
	 * @param qualifierName The name of the CIM qualifier type.
	 * @return An OW_CIMQualifierType identified by name.
	 * @exception OW_CIMException If the CIM qualifier type cannot be found
	 */
	virtual OW_CIMQualifierType getQualifierType(
		const OW_String& ns,
		const OW_String& qualifierName) = 0;

	/**
	 * Updates the specified CIM qualifier type in the specified namespace if
	 * it exist. If it doesn't exist, it will be added.
	 * @param ns The namespace
	 * @param qualifierType The CIM qualifier type to be updated or added.
	 */
	virtual void setQualifierType(
		const OW_String& ns,
		const OW_CIMQualifierType& qualifierType) = 0;

	/**
	 * Updates the CIM class associated with the specified namespace.
	 * @param ns The namespace
	 * @param cimClass The OW_CIMClass to be updated
	 * @exception OW_CIMException If the class does not exists
	 */
	virtual void modifyClass(
		const OW_String& ns,
		const OW_CIMClass& cimClass) = 0;

	/**
	 * Adds the CIM class to the specified namespace.
	 * @param ns The namespace
	 * @param cimClass The OW_CIMClass to be added
	 * @exception OW_CIMException If the CIM class already exists in the
	 *		namespace.
	 */
	virtual void createClass(
		const OW_String& ns,
		const OW_CIMClass& cimClass) = 0;

	/**
	 * Update the specified CIM instance associated with the specified
	 * namespace.
	 * @param ns The namespace
	 * @param modifiedInstance The OW_CIMInstance to be updated.
	 * @param includeQualifiers Modify the qualifiers
	 * @param propertyList The properties to modify.  0 means all properties.
	 * @exception OW_CIMException
	 */
	virtual void modifyInstance(
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		OW_Bool includeQualifiers = OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
		OW_StringArray* propertyList = 0) = 0;

	/**
	 * Add the specified CIM instance to the specified namespace.
	 * @param ns The namespace
	 * @param instance the OW_CIMInstance to be added
	 * @return An OW_CIMObjectPath of the created instance.
	 * @exception OW_CIMException If the CIM instance already exists in the
	 *		namespace
	 */
	virtual OW_CIMObjectPath createInstance(
		const OW_String& ns,
		const OW_CIMInstance& instance) = 0;

	/**
	 * Get the specified CIM instance property.
	 * @param ns The namespace
	 * @param instanceName An OW_CIMObjectPath that identifies the CIM instance to be
	 *		accessed
	 * @param propertyName	The name of the property to retrieve.
	 * @return The OW_CIMvalue for property identified by propertyName.
	 * @exception OW_CIMException
	 */
	virtual OW_CIMValue getProperty(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		const OW_String& propertyName) = 0;

	/**
	 * Set the specified CIM instance property.
	 * @param ns The namespace
	 * @param instanceName An OW_CIMObjectPath that identifies the CIM instance
	 *		to be accessed
	 * @param propertyName The name of the property to set the value on.
	 * @param newValue The new value for property propertyName.
	 * @exception OW_CIMException
	 */
	virtual void setProperty(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		const OW_String& propertyName,
		const OW_CIMValue& newValue) = 0;

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
	 * @exception OW_CIMException - as defined in the associator method
	 */
	virtual void associatorNames(
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& assocClass=OW_String(),
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		const OW_String& resultRole=OW_String()) = 0;

	virtual OW_CIMObjectPathEnumeration associatorNamesE(
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& assocClass=OW_String(),
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		const OW_String& resultRole=OW_String());

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
	 * @param includeQualifiers If set to INCLUDE_QUALIFIERS then all
	 *		Qualifiers for each Object (including Qualifiers on the Object and
	 *		on any returned Properties) MUST be included as elements in the
	 *		response.If set to EXCLUDE_QUALIFIERS, then no qualifiers will be
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
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& assocClass=OW_String(),
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		const OW_String& resultRole=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0) = 0;

	virtual OW_CIMInstanceEnumeration associatorsE(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& assocClass=OW_String(),
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		const OW_String& resultRole=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0);

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
	 * @param includeQualifiers If set to INCLUDE_QUALIFIERS then all
	 *		Qualifiers for each Object (including Qualifiers on the Object and
	 *		on any returned Properties) MUST be included as elements in the
	 *		response.If set to EXCLUDE_QUALIFIERS, then no qualifiers will be
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
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String& assocClass=OW_String(),
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		const OW_String& resultRole=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0) = 0;

	virtual OW_CIMClassEnumeration associatorsClassesE(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& assocClass=OW_String(),
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		const OW_String& resultRole=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0);

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
	 * @exception OW_CIMException As defined for associators method.
	 */
	virtual void referenceNames(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String()) = 0;

	virtual OW_CIMObjectPathEnumeration referenceNamesE(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String());

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
	 * @exception OW_CIMException - as defined for associators method.
	 */
	virtual void references(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0) = 0;

	virtual OW_CIMInstanceEnumeration referencesE(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0);

	virtual void referencesClasses(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0) = 0;

	virtual OW_CIMClassEnumeration referencesClassesE(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0);

	/**
	 * Executes a query to retrieve or modify objects.
	 *
	 * @param ns OW_CIMNameSpace that identifies the namespace in which to query.
	 *
	 * @param query A string containing the text of the query. This parameter
	 * 		cannot be null.
	 *
	 * @param queryLanguage A string containing the query language the query is
	 * 		written in.
	 *
	 * @return OW_CIMInstanceArray A deep enumeration of all instances of the
	 *		specified class and all classes derived from the specified class,
	 *		that match the query string.
	 *
	 * @exception OW_CIMException If any of the following errors occur:
	 *		The user does not have permission to view the result.
	 *		The requested query language is not supported.
	 *		The query specifies a class that does not exist.
	 */
	virtual void execQuery(
		const OW_String& ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& query,
		const OW_String& queryLanguage) = 0;

	virtual OW_CIMInstanceEnumeration execQueryE(
		const OW_String& ns,
		const OW_String& query,
		const OW_String& queryLanguage);
	
	/**
	 * @return The features of the CIMOM this OW_CIMOMHandleIFC is connected to as
	 * an OW_CIMFeatures object.
	 */
	virtual OW_CIMFeatures getServerFeatures();

	/**
	 * Export a given instance of an indication.
	 * This will cause all CIMListerners that are interested in this type
	 * of indication to be notified.
	 * @param instance	The indication instance to use in the notification.
	 */
	virtual void exportIndication(const OW_CIMInstance& instance,
		const OW_String& instNS);

};

typedef OW_Reference<OW_CIMOMHandleIFC> OW_CIMOMHandleIFCRef;

#endif


