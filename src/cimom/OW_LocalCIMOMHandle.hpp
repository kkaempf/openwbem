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

#ifndef __OW_LOCALCIMOMHANDLE_HPP__
#define __OW_LOCALCIMOMHANDLE_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_RWLocker.hpp"
#include "OW_CIMOMEnvironment.hpp"

/**
 * The OW_LocalCIMOMHandle class is a derivitive of the OW_CIMOMHandleIFC that
 * is used by all components that need access to CIM Services.
 */
class OW_LocalCIMOMHandle : public OW_CIMOMHandleIFC
{
public:

	/**
	 * Default constructor. This will create an invalid OW_LocalCIMOMHandle that
	 * is not suitable for any operations.
	 */
	OW_LocalCIMOMHandle() : OW_CIMOMHandleIFC(), m_pServer(0), m_aclInfo(OW_String()) {}

	/**
	 * Create a new OW_LocalCIMOMHandle with a given repository interface
	 * and user access contol information.
	 * @param env A reference to an OW_CIMOMEnvironment object.
	 * @param pRepos A reference to an OW_Repository that will be used by this
	 *		OW_LocalCIMOMHandle.
	 * @param aclInfo	The access control information that will be associated with
	 *		this OW_LocalCIMOMHandle.
	 */
	OW_LocalCIMOMHandle(OW_CIMOMEnvironmentRef env, OW_RepositoryIFCRef pRepos,
		const OW_ACLInfo& aclInfo);

	/**
	 * Copy constructor
	 * @param arg	The OW_LocalCIMOMHandle object to make this one a copy of.
	 */
	OW_LocalCIMOMHandle(const OW_LocalCIMOMHandle& arg);

	/**
	 * Assignment operator
	 * @param arg	The OW_LocalCIMOMHandle that will be assigned to this one.
	 * @return A reference to this OW_LocalCIMOMHandle.
	 */
	OW_LocalCIMOMHandle& operator= (const OW_LocalCIMOMHandle& arg);

	/**
	 * @return A reference to the OW_CIMOMEnvironment used by this object.
	 */
	OW_CIMOMEnvironmentRef getEnvironment() const { return m_env; }

	/**
	* @param ns The namespace containing the class to delete.
	* @param className The class to delete
	 */
	virtual void deleteClass(const OW_String& ns, const OW_String& className);

	/**
	 * Close the connetion to the CIMOM. Currently this does nothing on the
	 * local cimom handle.
	 */
	virtual void close();

	/**
	 * Deletes the CIM qualfier for the object specified by the CIM object path.
	 * @param path	The OW_CIMObjectPath identifying the qualifier type to delete.
	 */
	virtual void deleteQualifierType(const OW_String& ns, const OW_String& qualName);

	/**
	 * Enumerates the class specified by the OW_CIMObjectPath.
	 * @param path The OW_CIMObjectPath identifying the class to be enumerated.
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
	virtual void enumClass(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep,
		OW_Bool localOnly = OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers = OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin = OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN);


	/**
	 * Enumerates the class specified by the OW_CIMObjectPath.
	 * @param path		The OW_CIMObjectPath identifying the class to be
	 *						enumerated.
	 * @param deep		If set to OW_CIMClient::DEEP, the enumeration returned will
	 *						contain the names of all classes derived from the
	 *						enumerated class. If set to OW_CIMClient::SHALLOW the
	 *						enumermation will return only the names of the first level
	 *						children of the enumerated class.
	 * @return An enumeration of OW_CIMObjectPath objects
	 * 			(OW_CIMObjectPathEnumeration)
	 */
	virtual void enumClassNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		OW_Bool deep=true);

	/**
	 * Deletes the CIM instance specified by the CIM object path.
	 * A CIM object path consists of two  parts: namespace + model path. The
	 * model path is created by concatenating the properties of a class that are
	 * qualified with the KEY qualifier.
	 * @param path	The OW_CIMObjectPath identifying the instance to delete.
	 */
	virtual void deleteInstance(const OW_String& ns, const OW_CIMObjectPath& path);

	/**
	 * Enumerates the qualifiers defined in a namespace.
    * @param path	The OW_CIMObjectPath identifying the namespace whose qualifier
	 *					definitions are to be enumerated.
	 * @return 	An Enumeration of OW_CIMQualifierTypes
	 *				(OW_CIMQualifierEnumeration)
	 */
	virtual void enumQualifierTypes(
		const OW_String& ns,
		OW_CIMQualifierTypeResultHandlerIFC& result);

	/**
	 * Gets the CIM instance for the specified CIM object path.
	 *
	 * @param name			the OW_CIMObjectPath that identifies this CIM instance
	 *
	 * @param localOnly	If true, only the non-inherited properties are returned,
	 *	otherwise all properties are returned.
	 *
	 * @param includeQualifiers If true, then all of the qualifiers from the
	 *	class will be returned with the instance
	 *
	 * @param includeClassOrigin If true, then the class origin attribute will
	 * be returned on all appropriate elements
	 *
	 * @param propertyList If not NULL and has 0 or more elements, the returned
	 * instance will not contain properties missing from this list. If not NULL
	 * and it contains NO elements, then no properties will be included in the
	 * instance. If propertyList IS NULL, then it is ignored and all properties
	 * are returned with the instance subject to constraints specified in the
	 * other parameters.
	 *
	 * @return The OW_CIMInstance identified by the OW_CIMObjectPath
	 */
	virtual OW_CIMInstance getInstance(
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly=false, OW_Bool includeQualifiers=false,
		OW_Bool includeClassOrigin=false,
		const OW_StringArray* propertyList=0);

	/**
	 * Returns all instances (the whole instance and not just the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param path The OW_CIMObjectPath identifying the class whose instances are
	 * to be enumerated.
	 *
	 * @param deep Return properties defined on subclasses of the class in path
	 *
	 * @param localOnly if true, only the non-inherited properties are returned
	 * on each instance, otherwise all properties are returned.
	 *
	 * @param includeQualifiers If true, then all of the qualifiers from the
	 *	class will be returned with the each instance.
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
	 */
	virtual void enumInstances(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool deep=true, OW_Bool localOnly=false,
		OW_Bool includeQualifiers=false, OW_Bool includeClassOrigin=false,
		const OW_StringArray* propertyList=0);

	/**
	 * Returns all instance names belonging to the class specified in the path.
	 * This could include instances of all the classes in the specified class'
	 * hierarchy.
	 *
	 * @param path			The OW_CIMObjectPath identifying the class whose
	 *							instances are to be enumerated.
	 * @param deep			If set to OW_CIMClient::DEEP, the enumeration returned
	 *							will contain instances of the specified class and all
	 *							classes derived from it. If set to
	 *							OW_CIMClient::SHALLOW only instances belonging
	 *							to the specified class are returned.
	 * @return An Enumeration of OW_CIMObjectPaths
	 * 		(OW_CIMObjectPathEnumeration)
	 */
	virtual void enumInstanceNames(
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result);

	/**
	 * Gets the CIM qualifier type specified in the CIM object path.
	 * @param name	The OW_CIMObjectPath that identifies the CIM qualifier type.
	 * @return An OW_CIMQualifierType identified by name.
	 */
	virtual OW_CIMQualifierType getQualifierType(const OW_String& ns,
		const OW_String& qualifierName);

	/**
	 * Gets the CIM class for the specified CIM object path.
	 * @param name The OW_CIMObjectPath that identifies the CIM class
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
		OW_Bool localOnly = OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
		OW_Bool includeQualifiers = OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin = OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=NULL);


	/**
	 * Executes the specified method on the specified object. A method is a
	 * declaration containing the method name, return type, and parameters in
	 * the method.
	 * @param name			An OW_CIMObjectPath that identifies the method
	 * @param methodName	The string name of the method to be invoked
	 * @param inParams	The input parameters specified as an OW_CIMValueArray.
	 * @param outParams	The output parameters.
	 * @return A non-null OW_CIMValue on success. Otherwise a null OW_CIMValue.
	 */
	virtual OW_CIMValue invokeMethod(
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String &methodName, const OW_CIMParamValueArray &inParams,
		OW_CIMParamValueArray &outParams);

	/**
	 * Adds the CIM class to the specified namespace.
	 * @param name	An OW_CIMObjectPath that identifies the CIM class to be added.
	 * @param cc	The OW_CIMClass to be added
	 */
	virtual void createClass(const OW_CIMObjectPath &name,
		const OW_CIMClass &cc);

	/**
	 * Updates the specified CIM qualifier type in the specified namespace if
	 * it exist. If it doesn't exist, it will be added.
	 * @param name	An OW_CIMObjectPath that identifies the CIM qualifier type.
	 * @param qt	The CIM qualifier type to be updated or added.
	 */
	virtual void setQualifierType(const OW_String& ns,
		const OW_CIMQualifierType& qt);

	/**
	 * Add the specified CIM instance to the specified namespace.
	 * @param name	The OW_CIMObjectPath that identifies the CIM instance to be
	 *					added.
	 * @param ci	the OW_CIMInstance to be added
	 * @return An OW_CIMObjectPath of the created instance.
	 */
	virtual OW_CIMObjectPath createInstance(const OW_CIMObjectPath &name,
		const OW_CIMInstance &ci);

	/**
	 * Updates the CIM class associated with the specified namespace.
	 * @param name	An OW_CIMObjectPath that identifies the CIM class to be
	 *					updated.
	 * @param cc	The OW_CIMClass to be updated
	 */
	virtual void modifyClass(const OW_String &ns, const OW_CIMClass &cc);

	/**
	 * Set the specified CIM instance property.
	 * @param name An OW_CIMObjectPath that identifies the CIM instance to be
	 *		accessed
	 * @param propertyName The name of the property to set the value on.
	 * @param newValue The new value for property propertyName.
	 */
	virtual void setProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_CIMValue &cv);

	/**
	 * Update or add the specified CIM instance associated with the specified
	 * namespace.
	 * @param name An OW_CIMObjectPath that identifies the CIM instance to be
	 *		updated.
	 * @param ci The OW_CIMInstance to be updated.
	 */
	virtual void modifyInstance(const OW_CIMObjectPath &name,
		const OW_CIMInstance &ci);

	/**
	 * Get the specified CIM instance property.
	 * @param name An OW_CIMObjectPath that identifies the CIM instance to be
	 *		accessed
	 * @param propertyName The name of the property to retrieve.
	 * @return The OW_CIMvalue for property identified by propertyName.
	 */
	virtual OW_CIMValue getProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName);

	/**
	 * This operation is used to enumerate CIMInstances
	 * that are associated to a particular source CIM Object.
	 *
	 * @param objectName Defines the source CIM Object whose associated Instances
	 * are to be returned. This may be either a Class name or Instance name
	 * (modelpath).
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
	 * @param role The Role input parameter, if not "", MUST be a valid
	 * Property name. It acts as a filter on the returned set of Objects by
	 * mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the source Object plays the specified
	 * role (i.e. the name of the Property in the Association Class that refers
	 * to the source Object MUST match the value of this parameter).
	 *
	 * @param resultRole The ResultRole input parameter, if not NULL, "" be a
	 * valid Property name. It acts as a filter on the returned set of Objects
	 * by mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the returned Object plays the
	 * specified role (i.e. the name of the Property in the Association Class
	 * that refers to the returned Object MUST match the value of this
	 * parameter).
	 *
	 * @param includeQualifiers If the IncludeQualifiers input parameter is true
	 * this specifies that all Qualifiers for each Object (including Qualifiers
	 * on the Object and on any returned Properties) MUST be included as
	 * elements in the response.If false no elements are present in each
	 * returned Object.
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
	virtual void associators(const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList);

	virtual void associatorsClasses(
		const OW_CIMObjectPath& path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String& assocClass=OW_String(),
		const OW_String& resultClass=OW_String(),
		const OW_String& role=OW_String(),
		const OW_String& resultRole=OW_String(),
		OW_Bool includeQualifiers=EXCLUDE_QUALIFIERS,
		OW_Bool includeClassOrigin=EXCLUDE_CLASS_ORIGIN,
		const OW_StringArray* propertyList=0);
	
	/**
	 * This operation is used to enumerate the association objects that refer to
	 * a particular target CIM Object
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
	 */
	virtual void references(const OW_CIMObjectPath &path,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList);

	virtual void referencesClasses(const OW_CIMObjectPath &path,
		OW_CIMClassResultHandlerIFC& result,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList);

	/**
	 * This method is used to enumerate the names of CIM Objects that are
	 * associated to a particular CIM Object.
	 *
	 * @param path Defines the source CIM Object whose associated Objects are to
	 * be returned. This may be either a Class name or Instance name (modelpath).
	 *
	 * @param assocClass The AssocClass input parameter, if not NULL, MUST be a
	 * valid CIM Association Class name. It acts as a filter on the returned set
	 * of Objects by mandating that each returned Object MUST be associated to
	 * the source Object via an Instance of this Class or one of its subclasses.
	 *
	 * @param resultClass The ResultClass input parameter, if not NULL, MUST be
	 * a valid CIM Class name. It acts as a filter on the returned set of
	 * Objects by mandating that each returned Object MUST be either an Instance
	 * of this Class (or one of its subclasses) or be this Class (or one of its
	 * subclasses).
	 *
	 * @param role	The Role input parameter, if not NULL, MUST be a valid
	 * Property name. It acts as a filter on the returned set of Objects by
	 * mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the source Object plays the specified
	 * role (i.e. the name of the Property in the Association Class that refers
	 * to the source Object MUST match the value of this parameter).
	 *
	 * @param resultRole The ResultRole input parameter, if not NULL, MUST be a
	 * valid Property name. It acts as a filter on the returned set of Objects
	 * by mandating that each returned Object MUST be associated to the source
	 * Object via an Association in which the returned Object plays the
	 * specified role (i.e. the name of the Property in the Association Class
	 * that refers to the returned Object MUST match the value of this
	 * parameter).
	 *
	 * @return	If successful, the method returns zero or more full CIM Class
	 *				paths or Instance paths of Objects meeting the requested
	 *				criteria.
	 */
	virtual void associatorNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &assocClass,
		const OW_String &resultClass, const OW_String &role,
		const OW_String &resultRole);

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
	 */
	virtual void referenceNames(
		const OW_CIMObjectPath &path,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String &resultClass,
		const OW_String &role);
	
	/**
	 * Executes a query to retrieve CIM objects.
	 *
	 * @param ns The namespace to perform the query within
	 *
	 * @param query A string containing the text of the query.
	 *
	 * @param queryLanguage A string containing the query language the query is
	 * 	written in.
	 *
	 * @return OW_CIMInstanceArray A deep enumeration of all instances of the
	 * specified class and all classes derived from the specified class, that
	 * match the query string.
	 */
	virtual void execQuery(const OW_CIMNameSpace &ns,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String &query, const OW_String& queryLanguage);

	/**
	 * Delete a specified namespace.
	 * @param ns	The OW_CIMNameSpace object that identifies the namespace
	 *					to delete.
	 */
	virtual void deleteNameSpace(const OW_String &ns);

	/**
	 * Create a cim namespace.
	 * @param ns 	The namespace to be created.
	 */
	virtual void createNameSpace(const OW_String& ns);

	/**
	 * Gets a list of the namespaces within a specified namespace
	 * @param ns The parent namespace to enumerate
	 * @param deep If set to OW_CIMClient::DEEP, the enumeration returned will
	 *		contain the entire hierarchy of namespaces present under the
	 *		enumerated namespace. If set to	OW_CIMClient::SHALLOW  the
	 *		enuermation will return only the first level children of the
	 *		enumerated namespace.
	 * @return An Array of namespace names as strings.
	 */
	virtual void enumNameSpace(const OW_String& ns,
		OW_StringResultHandlerIFC& result,
		OW_Bool deep);
	
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
	void exportIndication(const OW_CIMInstance& instance,
		const OW_String& instNS);

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (m_pServer) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (m_pServer) ? 0: &dummy::nonnull; }

private:

	/**
	 * Create a new OW_LocalCIMOMHandle with a given repository interface
	 * and user access contol information.
	 * @param env A reference to an OW_CIMOMEnvironment object.
	 * @param pRepos A reference to a OW_Repository that will be used by this
	 *		OW_LocalCIMOMHandle.
	 * @param aclInfo	The access control information that will be associated with
	 *		this OW_LocalCIMOMHandle.
	 * @param noLock If true, the this object will never attempt to acquire a
	 *		read/write lock on the OW_CIMServer.
	 */
	OW_LocalCIMOMHandle(OW_CIMOMEnvironmentRef env, OW_RepositoryIFCRef pRepos,
		const OW_ACLInfo& aclInfo, OW_Bool noLock);

	/**
	 * Acquire a read lock on the OW_CIMServer if m_noLock is false.
	 */
	OW_ReadLock getReadLock();

	/**
	 * Acquire a write lock on the OW_CIMServer if m_noLock is false.
	 */
	OW_WriteLock getWriteLock();

	/**
	 * A Reference to the OW_Repository interface that this OW_LocalCIMOMHandle
	 * will use.
	 */
	OW_RepositoryIFCRef m_pServer;

	/**
	 * The user access control information that is associated with this
	 * OW_LocalCIMOMHandle
	 */
	OW_ACLInfo m_aclInfo;

	/**
	 * If m_noLock is true, then this OW_LocalCIMOMHandle will never attempt
	 * to acquire a read/write lock on the cim server.
	 */
	OW_Bool m_noLock;

	OW_CIMOMEnvironmentRef m_env;

	friend class OW_CIMServer;
};

typedef OW_Reference<OW_LocalCIMOMHandle> OW_LocalCIMOMHandleRef;

#endif	// __OW_LOCALCIMOMHANDLE_HPP__

