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


#include "OW_config.h"
#include "OW_CIMClient.hpp"
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"

OW_CIMClient::OW_CIMClient(const OW_String& url, const OW_String& ns,
	const OW_ClientAuthCBIFCRef& authCB)
{

	OW_URL owurl(url);
	OW_CIMProtocolIFCRef client(new OW_HTTPClient(url));


	/**********************************************************************
	 * Assign our callback to the HTTP Client.
	 **********************************************************************/

	client->setLoginCallBack(authCB);

	/**********************************************************************
	 * Here we create a OW_CIMXMLCIMOMHandle and have it use the
	 * OW_HTTPClient we've created.  OW_CIMXMLCIMOMHandle takes
	 * a OW_Reference<OW_CIMProtocol> it it's constructor, so
	 * we have to make a OW_Reference out of our HTTP Client first.
	 * By doing this, we don't have to worry about deleting our
	 * OW_HTTPClient.  OW_Reference will delete it for us when the
	 * last copy goes out of scope (reference count goes to zero).
	 **********************************************************************/

	if (owurl.path.equalsIgnoreCase("/owbinary"))
	{
		m_ch = new OW_BinaryCIMOMHandle(client);
	}
	else
	{
		m_ch = new OW_CIMXMLCIMOMHandle(client);
	}

	m_namespace = ns;

}

/**
 * Create a cim namespace.
 * @param ns 	The namespace name to be created.
 * @exception OW_CIMException If the namespace already exists.
 */
void OW_CIMClient::createNameSpace(const OW_String& ns)
{
	m_ch->createNameSpace(ns);
}

/**
 * Delete a specified namespace.
 * @param ns	The namespace to delete.
 * @exception OW_CIMException If the namespace does not exist.
 */
void OW_CIMClient::deleteNameSpace(const OW_String& ns)
{
	m_ch->deleteNameSpace(ns);
}

/**
 * Gets a list of the namespaces within the namespace specified by the CIM
 * object path.
 * @param deep If set to DEEP, the enumeration returned will contain the
 *		entire hierarchy of namespaces present under the enumerated
 *		namespace. If set to SHALLOW  the enuermation will return only the
 *		first level children of the enumerated namespace.
 * @return An Array of namespace names as strings.
 * @exception OW_CIMException If the namespace does not exist or the object
 *		cannot be found in the specified namespace.
 */
OW_StringArray 
	OW_CIMClient::enumNameSpaceE(OW_Bool deep)
{
	return m_ch->enumNameSpaceE(m_namespace, deep);
}

void 
	OW_CIMClient::enumNameSpace(OW_StringResultHandlerIFC& result, 
	OW_Bool deep)
{
	m_ch->enumNameSpace(m_namespace, result, deep);
}

/**
 * Deletes the CIM class specified by className in namespace ns.
 * @param className The class to delete
 * @exception OW_CIMException If the object does not exist
 */
void 
	OW_CIMClient::deleteClass(const OW_String& className)
{
	m_ch->deleteClass(m_namespace, className);
}

/**
 * Deletes the CIM instance specified by the CIM object path.
 * A CIM object path consists of two  parts: namespace + model path. The
 * model path is created by concatenating the properties of a class that are
 * qualified with the KEY qualifier.
 * @param path	The OW_CIMObjectPath identifying the instance to delete.
 * @exception OW_CIMException If the instance does not exist.
 */
void 
	OW_CIMClient::deleteInstance(const OW_CIMObjectPath& path)
{
	m_ch->deleteInstance(m_namespace, path);
}

/**
 * Deletes a CIM qualfier type.
 * @param qualName The qualifier type to delete.
 * @exception OW_CIMException If the qualifier type does not exist.
 */
void 
OW_CIMClient::deleteQualifierType(const OW_String& qualName)
{
	m_ch->deleteQualifierType(m_namespace, qualName);
}

/**
 * Enumerates the class specified by the OW_CIMObjectPath.
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
 * @exception OW_CIMException If the specified CIMObjectPath object cannot
 *		be found
 */
void 
OW_CIMClient::enumClass(const OW_String& className,
	OW_CIMClassResultHandlerIFC& result,
	OW_Bool deep, OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin)
{
	m_ch->enumClass(m_namespace, className, result, deep, localOnly, 
		includeQualifiers, includeClassOrigin);
}

OW_CIMClassEnumeration 
OW_CIMClient::enumClassE(const OW_String& className,
	OW_Bool deep,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin)
{
	return m_ch->enumClassE(m_namespace, className, deep, localOnly, includeQualifiers, 
		includeClassOrigin);
}

/**
 * Enumerates the class specified by the OW_CIMObjectPath.
 * @param className The class to be enumerated.
 * @param deep If set to DEEP, the enumeration returned will
 *		contain the names of all classes derived from the enumerated class.
 *		If set to SHALLOW the enumermation will return only
 *		the names of the first level children of the enumerated class.
 * @exception OW_CIMException  	If the specified CIMObjectPath object
 *											cannot be found
 */
void 
	OW_CIMClient::enumClassNames(
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep)
{
	m_ch->enumClassNames(m_namespace, className, result, deep);
}

OW_CIMObjectPathEnumeration 
	OW_CIMClient::enumClassNamesE(
	const OW_String& className,
	OW_Bool deep)
{
	return m_ch->enumClassNamesE(m_namespace, className, deep);
}

/**
 * Returns all instances (the whole instance and not just the names)
 * belonging to the class specified in the path. This could include
 * instances of all the classes in the specified class' hierarchy.
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
 * @exception OW_CIMException 	If the object cannot be found
 */
void 
OW_CIMClient::enumInstances(
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result,
	OW_Bool deep,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_ch->enumInstances(m_namespace, className, result, deep, localOnly,
		includeQualifiers, includeClassOrigin, propertyList);
}

OW_CIMInstanceEnumeration 
OW_CIMClient::enumInstancesE(
	const OW_String& className,
	OW_Bool deep,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->enumInstancesE(m_namespace, className, deep, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}

/**
 * Returns all instance names belonging to the class specified in the path.
 * This could include instances of all the classes in the specified class'
 * hierarchy.
 *
 * @param className The class whose instances are to be enumerated.
 * @exception OW_CIMException 	If the object cannot be found
 */
void 
OW_CIMClient::enumInstanceNames(
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result)
{
	m_ch->enumInstanceNames(m_namespace, className, result);
}

OW_CIMObjectPathEnumeration 
OW_CIMClient::enumInstanceNamesE(
	const OW_String& className)
{
	return m_ch->enumInstanceNamesE(m_namespace, className);
}

/**
 * Enumerates the qualifiers defined in a namespace.
 * @exception OW_CIMException	If the specified OW_CIMObjectPath cannot be
 *										found
 */
void 
OW_CIMClient::enumQualifierTypes(
	OW_CIMQualifierTypeResultHandlerIFC& result)
{
	m_ch->enumQualifierTypes(m_namespace, result);
}

OW_CIMQualifierTypeEnumeration 
OW_CIMClient::enumQualifierTypesE()
{
	return m_ch->enumQualifierTypesE(m_namespace);
}

/**
 * Gets the CIM class for the specified CIM object path.
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
OW_CIMClass 
	OW_CIMClient::getClass(
	const OW_String& className,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->getClass(m_namespace, className, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}

/**
 * Gets the CIM instance for the specified CIM object path.
 *
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
OW_CIMInstance 
	OW_CIMClient::getInstance(
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList) 
{
	return m_ch->getInstance(m_namespace, instanceName, localOnly, 
		includeQualifiers, includeClassOrigin, propertyList);
}

/**
 * Executes the specified method on the specified object. A method is a
 * declaration containing the method name, return type, and parameters in
 * the method.
 * @param path An OW_CIMObjectPath that identifies the class or instance
 *  on which to invoke the method.
 * @param methodName The string name of the method to be invoked
 * @param inParams The input parameters specified as an OW_CIMValueArray.
 * @param outParams	The output parameters.
 * @return A non-null OW_CIMValue on success. Otherwise a null OW_CIMValue.
 * @exception OW_CIMException	If the specified method cannot be found
 */
OW_CIMValue 
	OW_CIMClient::invokeMethod(
	const OW_CIMObjectPath& path,
	const OW_String& methodName,
	const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams)
{
	return m_ch->invokeMethod(m_namespace, path, methodName, inParams, outParams);
}

/**
 * Gets the CIM qualifier type specified in the CIM object path.
 * @param qualifierName The name of the CIM qualifier type.
 * @return An OW_CIMQualifierType identified by name.
 * @exception OW_CIMException If the CIM qualifier type cannot be found
 */
OW_CIMQualifierType 
	OW_CIMClient::getQualifierType(const OW_String& qualifierName)
{
	return m_ch->getQualifierType(m_namespace, qualifierName);
}

/**
 * Updates the specified CIM qualifier type in the specified namespace if
 * it exist. If it doesn't exist, it will be added.
 * @param qualifierType The CIM qualifier type to be updated or added.
 */
void 
	OW_CIMClient::setQualifierType(const OW_CIMQualifierType& qualifierType)
{
	m_ch->setQualifierType(m_namespace, qualifierType);
}

/**
 * Updates the CIM class associated with the specified namespace.
 * @param cimClass The OW_CIMClass to be updated
 * @exception OW_CIMException If the class does not exists
 */
void 
	OW_CIMClient::modifyClass(const OW_CIMClass& cimClass) 
{
	m_ch->modifyClass(m_namespace, cimClass);
}

/**
 * Adds the CIM class to the specified namespace.
 * @param cimClass The OW_CIMClass to be added
 * @exception OW_CIMException If the CIM class already exists in the
 *		namespace.
 */
void 
	OW_CIMClient::createClass(const OW_CIMClass& cimClass) 
{
	m_ch->createClass(m_namespace, cimClass);
}

/**
 * Update the specified CIM instance associated with the specified
 * namespace.
 * @param modifiedInstance The OW_CIMInstance to be updated.
 * @param includeQualifiers Modify the qualifiers
 * @param propertyList The properties to modify.  0 means all properties.
 * @exception OW_CIMException
 */
void 
	OW_CIMClient::modifyInstance(const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	OW_StringArray* propertyList)
{
	m_ch->modifyInstance(m_namespace, modifiedInstance, includeQualifiers,
		propertyList);
}

/**
 * Add the specified CIM instance to the specified namespace.
 * @param instance the OW_CIMInstance to be added
 * @return An OW_CIMObjectPath of the created instance.
 * @exception OW_CIMException If the CIM instance already exists in the
 *		namespace
 */
OW_CIMObjectPath 
	OW_CIMClient::createInstance(const OW_CIMInstance& instance) 
{
	return m_ch->createInstance(m_namespace, instance);
}

/**
 * Get the specified CIM instance property.
 * @param instanceName An OW_CIMObjectPath that identifies the CIM instance to be
 *		accessed
 * @param propertyName	The name of the property to retrieve.
 * @return The OW_CIMvalue for property identified by propertyName.
 * @exception OW_CIMException
 */
OW_CIMValue 
	OW_CIMClient::getProperty(
	const OW_CIMObjectPath& instanceName,
	const OW_String& propertyName)
{
	return m_ch->getProperty(m_namespace, instanceName, propertyName);
}

/**
 * Set the specified CIM instance property.
 * @param instanceName An OW_CIMObjectPath that identifies the CIM instance
 *		to be accessed
 * @param propertyName The name of the property to set the value on.
 * @param newValue The new value for property propertyName.
 * @exception OW_CIMException
 */
void 
	OW_CIMClient::setProperty(
	const OW_CIMObjectPath& instanceName,
	const OW_String& propertyName,
	const OW_CIMValue& newValue) 
{
	m_ch->setProperty(m_namespace, instanceName, propertyName, newValue);
}

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
void 
	OW_CIMClient::associatorNames(
	const OW_CIMObjectPath& objectName,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole)
{
	m_ch->associatorNames(m_namespace, objectName, result,
		assocClass, resultClass, role, resultRole);
}

OW_CIMObjectPathEnumeration 
	OW_CIMClient::associatorNamesE(
	const OW_CIMObjectPath& objectName,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole)
{
	return m_ch->associatorNamesE(m_namespace, objectName, 
		assocClass, resultClass, role, resultRole);
}

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
void 
OW_CIMClient::associators(
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_ch->associators(m_namespace, path, result,
		assocClass, resultClass,
		role, resultRole, includeQualifiers, includeClassOrigin, propertyList);
}

OW_CIMInstanceEnumeration 
OW_CIMClient::associatorsE(
	const OW_CIMObjectPath& path,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->associatorsE(m_namespace, path, assocClass, 
		resultClass, role, resultRole, includeQualifiers, 
		includeClassOrigin, propertyList);
}

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
void 
OW_CIMClient::associatorsClasses(
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	m_ch->associatorsClasses(m_namespace, path, result, assocClass, 
		resultClass, role, resultRole, includeQualifiers, includeClassOrigin, 
		propertyList);
}

OW_CIMClassEnumeration 
OW_CIMClient::associatorsClassesE(
	const OW_CIMObjectPath& path,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->associatorsClassesE(m_namespace, path, assocClass, 
		resultClass, role, resultRole, includeQualifiers, includeClassOrigin, 
		propertyList);
}

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
void 
	OW_CIMClient::referenceNames(
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role)
{
	m_ch->referenceNames(m_namespace, path, result, resultClass, role);
}

OW_CIMObjectPathEnumeration 
	OW_CIMClient::referenceNamesE(
	const OW_CIMObjectPath& path,
	const OW_String& resultClass,
	const OW_String& role)
{
	return m_ch->referenceNamesE(m_namespace, path, resultClass, role);
}

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
void 
OW_CIMClient::references(
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList) 
{
	m_ch->references(m_namespace, path, result, resultClass, 
		role, includeQualifiers, includeClassOrigin, propertyList);
}

OW_CIMInstanceEnumeration 
OW_CIMClient::referencesE(
	const OW_CIMObjectPath& path,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->referencesE(m_namespace, path, resultClass, role, 
		includeQualifiers, includeClassOrigin, propertyList);
}

void 
OW_CIMClient::referencesClasses(
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList) 
{
	m_ch->referencesClasses(m_namespace, path, result, resultClass, 
		role, includeQualifiers, includeClassOrigin, propertyList);
}

OW_CIMClassEnumeration 
OW_CIMClient::referencesClassesE(
	const OW_CIMObjectPath& path,
	const OW_String& resultClass,
	const OW_String& role,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList)
{
	return m_ch->referencesClassesE(m_namespace, path, resultClass, role, 
		includeQualifiers, includeClassOrigin, propertyList);
}

/**
 * Executes a query to retrieve or modify objects.
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
void 
OW_CIMClient::execQuery(
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& query,
	const OW_String& queryLanguage) 
{
	m_ch->execQuery(m_namespace, result, query, queryLanguage);
}

OW_CIMInstanceEnumeration 
OW_CIMClient::execQueryE(
	const OW_String& query,
	const OW_String& queryLanguage)
{
	return m_ch->execQueryE(m_namespace, query, queryLanguage);
}




