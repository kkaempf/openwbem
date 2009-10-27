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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_BINARYCIMOMHANDLE_HPP_
#define OW_BINARYCIMOMHANDLE_HPP_
#include "OW_config.h"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_CIMProtocolIFC.hpp"

namespace OW_NAMESPACE
{

class OW_CLIENT_API BinaryCIMOMHandle : public ClientCIMOMHandle
{
public:
	/**
	 * Create a remote CIMOM handle.  This represents a single
	 * connection to a CIMOM.
	 * @param prot An Reference to a CIMProtocol to be used
	 * 	with this remote handle.  This will typically be an IPCClient.
	 * @example
	 * 	String url("ipc://user:pass@localhost");
	 * 	Reference<HTTPClient> clientRef(new IPCClient(url));
	 * 	BinaryCIMOMHandle rch(clientRef);
	 * 	// now the binary over ipc connection is established, and
	 * 	// the remote CIMOM handle can be used.
	 */
	BinaryCIMOMHandle( const CIMProtocolIFCRef& prot );

	virtual ~BinaryCIMOMHandle();

	/**
	 * Get a Reference to the WBEM protocol handler (HTTPClient)
	 */
	virtual CIMProtocolIFCRef getWBEMProtocolHandler() const;

	/**
	 * Close the connetion to the CIMOM. This will free resources used for the
	 * client session.
	 */
	virtual void close();
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Deletes the CIM qualfier for the object specified by the CIM object path.
	 * @param path	The CIMObjectPath identifying the qualifier type to delete.
	 * @exception CIMException If the qualifier type does not exist.
	 */
	virtual void deleteQualifierType(const blocxx::String& ns, const blocxx::String& qualName);
	/**
	 * Updates the specified CIM qualifier type to the specified
	 * namespace.
	 * @param name	Am CIMObjectPath that identifies the CIM qualifier type.
	 * @param qt	The CIM qualifier type to be updated.
	 * @exception CIMException If the qualifier type cannot be found or the
	 *										user does not have write permission to the
	 *										namespace.
	 */
	virtual void setQualifierType(const blocxx::String& ns,
		const CIMQualifierType &qt);
	/**
	 * Enumerates the qualifiers defined in a namespace.
	 * @param path	The CIMObjectPath identifying the namespace whose qualifier
	 *					definitions are to be enumerated.
	 * @exception CIMException	If the specified CIMObjectPath cannot be
	 *										found
	 */
	virtual void enumQualifierTypes(
		const blocxx::String& ns,
		CIMQualifierTypeResultHandlerIFC& result);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param path The CIMObjectPath identifying the class to be enumerated.
	 * @param deep If set to DEEP, the enumeration returned will
	 *		contain the names of all classes derived from the enumerated class.
	 *		If set to SHALLOW the enumermation will return only
	 *		the names of the first level children of the enumerated class.
	 * @param localOnly If set to LOCAL_ONLY, only the non-inherited properties
	 *		are returned on each instance, otherwise all properties are returned.
	 * @param includeQualifiers If set toWBEMFlags::E_INCLUDE_QUALIFIERS, then all class,
	 *		property and method qualifiers will be returned.
	 * @param includeClassOrigin If true, then the class origin attribute will
	 *		be included with all appropriate elements of each class.
	 * @exception CIMException If the specified CIMObjectPath object cannot
	 *		be found
	 */
	virtual void enumClass(const blocxx::String& ns,
		const blocxx::String& className,
		CIMClassResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN);
	/**
	 * Enumerates the class specified by the CIMObjectPath.
	 * @param path		The CIMObjectPath identifying the class to be
	 *						enumerated.
	 * @param deep		If set to CIMClient::DEEP, the enumeration returned will
	 *						contain the names of all classes derived from the
	 *						enumerated class. If set to CIMClient::SHALLOW the
	 *						enumermation will return only the names of the first level
	 *						children of the enumerated class.
	 * @exception CIMException  	If the specified CIMObjectPath object cannot be found.
	 */
	virtual void enumClassNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep=WBEMFlags::E_DEEP);
	/**
	 * Gets the CIM instance for the specified CIM object path.
	 *
	 * @param name			the CIMObjectPath that identifies this CIM instance
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
	 * @return The CIMInstance identified by the CIMObjectPath
	 * @exception CIMException If the specified CIMObjectPath cannot be
	 *										found
	 */
	virtual CIMInstance getInstance(
		const blocxx::String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=0);
	/**
	 * Returns all instances (the whole instance and not just the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param path The CIMObjectPath identifying the class whose instances are
	 * to be enumerated.
	 *
	 * @param deep If set to CIMClient::DEEP, the enumeration returned will
	 * contain instances of the specified class and all classes derived from it.
	 * If set to CIMClient::SHALLOW only instances belonging to the specified
	 * class are returned.
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
	 * @exception CIMException 	If the object cannot be found
	 */
	virtual void enumInstances(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=0);
	/**
	 * Gets the CIM qualifier type specified in the CIM object path.
	 * @param name	The CIMObjectPath that identifies the CIM qualifier type.
	 * @return An CIMQualifierType identified by name.
	 * @exception CIMException If the CIM qualifier type cannot be found
	 */
	virtual CIMQualifierType getQualifierType(const blocxx::String& ns,
		const blocxx::String& qualifierName);
	/**
	 * Returns all instances (the the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param path			The CIMObjectPath identifying the class whose
	 *							instances are to be enumerated.
	 * @exception CIMException 	If the object cannot be found
	 */
	virtual void enumInstanceNames(
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMObjectPathResultHandlerIFC& result);
	/**
	 * Gets the CIM class for the specified CIM object path.
	 * @param name The CIMObjectPath that identifies the CIM class
	 * @param localOnly	If set to LOCAL_ONLY, only the non-inherited properties
	 *		and methods	are returned, otherwise all properties and methods are
	 *		returned.
	 * @param includeQualifiers If set toWBEMFlags::E_INCLUDE_QUALIFIERS, then all class,
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
		const blocxx::String& ns,
		const blocxx::String& className,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=NULL);
	/**
	 * Executes the specified method on the specified object. A method is a
	 * declaration containing the method name, return type, and parameters in
	 * the method.
	 * @param name			An CIMObjectPath that identifies the method
	 * @param methodName	The string name of the method to be invoked
	 * @param inParams	The input parameters specified as an CIMValueArray.
	 * @param outParams	The output parameters.
	 * @return A non-null CIMValue on success. Otherwise a null CIMValue.
	 * @exception CIMException	If the specified method cannot be found
	 */
	virtual CIMValue invokeMethod(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		const blocxx::String &methodName, const CIMParamValueArray &inParams,
		CIMParamValueArray &outParams);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Adds the CIM class to the specified namespace.
	 * @param name	An CIMObjectPath that identifies the CIM class to be added.
	 * @param cc	The CIMClass to be added
	 * @exception CIMException If the CIM class already exists in the
	 *										namespace.
	 */
	virtual void createClass(const blocxx::String& ns,
		const CIMClass &cc);
	/**
	 * Updates the CIM class associated with the specified namespace.
	 * @param name	An CIMObjectPath that identifies the CIM class to be
	 *					updated.
	 * @param cc	The CIMClass to be updated
	 * @exception CIMException If the class does not exists
	 */
	virtual void modifyClass(const blocxx::String &ns, const CIMClass &cc);
	/**
	 * @param ns The namespace containing the class to delete.
	 * @param className The class to delete
	 * @exception CIMException If the object does not exist
	 */
	virtual void deleteClass(const blocxx::String& ns, const blocxx::String& className);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Add the specified CIM instance to the specified namespace.
	 * @param name	The CIMObjectPath that identifies the CIM instance to be
	 *					added.
	 * @param ci	the CIMInstance to be added
	 * @return An CIMObjectPath of the created instance.
	 * @exception CIMException If the CIM instance already exists in the
	 *										namespace
	 */
	virtual CIMObjectPath createInstance(const blocxx::String& ns,
		const CIMInstance &ci);
	/**
	 * Update the specified CIM instance associated with the specified
	 * namespace.
	 * @param name	An CIMObjectPath that identifies the CIM instance to be
	 *					updated.
	 * @param ci	The CIMInstance to be updated.
	 * @exception CIMException
	 */
	virtual void modifyInstance(
		const blocxx::String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const blocxx::StringArray* propertyList);
	/**
	 * Deletes the CIM instance specified by the CIM object path.
	 * A CIM object path consists of two  parts: namespace + model path. The
	 * model path is created by concatenating the properties of a class that are
	 * qualified with the KEY qualifier.
	 * @param path	The CIMObjectPath identifying the instance to delete.
	 * @exception CIMException If the instance does not exist.
	 */
	virtual void deleteInstance(const blocxx::String& ns, const CIMObjectPath &path);
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	/**
	 * Set the specified CIM instance property.
	 * @param name				An CIMObjectPath that identifies the CIM instance
	 *								to be accessed
	 * @param propertyName	The name of the property to set the value on.
	 * @param newValue		The new value for property propertyName.
	 * @exception CIMException
	 */
	virtual void setProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName, const CIMValue &cv);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
	/**
	 * Get the specified CIM instance property.
	 * @param name				An CIMObjectPath that identifies the CIM instance
	 *								to be accessed
	 * @param propertyName	The name of the property to retrieve.
	 * @return The CIMvalue for property identified by propertyName.
	 * @exception CIMException
	 */
	virtual CIMValue getProperty(
		const blocxx::String& ns,
		const CIMObjectPath &name,
		const blocxx::String &propertyName);
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
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
	virtual void associators(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &assocClass, const blocxx::String &resultClass,
		const blocxx::String &role, const blocxx::String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);
	virtual void associatorsClasses(
		const blocxx::String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String& assocClass=blocxx::String(),
		const blocxx::String& resultClass=blocxx::String(),
		const blocxx::String& role=blocxx::String(),
		const blocxx::String& resultRole=blocxx::String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers=WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin=WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const blocxx::StringArray* propertyList=0);
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
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);
	/**
	 * This operation is used to enumerate the association objects that refer to
	 * a particular target CIM Object (Class or Instance).
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
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &resultClass,
		const blocxx::String &role);
	virtual void referencesClasses(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const blocxx::String &resultClass, const blocxx::String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);
	/**
	 * This method is used to enumerate the names of CIM Objects (Classes or
	 * Instances) that are associated to a particular CIM Object.
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
	 *
	 * @exception CIMException - as defined in the associator method
	 */
	virtual void associatorNames(
		const blocxx::String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String &assocClass,
		const blocxx::String &resultClass, const blocxx::String &role,
		const blocxx::String &resultRole);
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	/**
	 * Executes a query to retrieve objects. The WBEM Query Language (WQL) is a
	 * subset of standard American National Standards Institute Structured Query
	 * Language (ANSI SQL) with semantic changes to support WBEM on Solaris.
	 * Only queries on class instances are supported.
	 *
	 * @param path CIMNameSpace that identifies the namespace in which to query.
	 *
	 * @param query A string containing the text of the query. This parameter
	 * cannot be null.
	 *
	 * @param wqlLevel - Static integer that identifies the query language to use for
	 * parsing the query string. WQL levels 1 and 2 are the only currently supported
	 * query language.
	 *
	 * @return CIMInstanceArray A deep enumeration of all instances of the specified
	 * class and all classes derived from the specified class, that match the
	 * query string.
	 *
	 * @exception CIMException If any of the following errors occur:
	 *		The user does not have permission to view the result.
	 *		The requested query language is not supported.
	 *		The query specifies a class that does not exist.
	 */
	CIMInstanceEnumeration execQuery(
		const blocxx::String& ns,
		const blocxx::String &query, int wqlLevel);
	/**
	 * Executes a query to retrieve or modify objects.
	 *
	 * @param ns CIMNameSpace that identifies the namespace in which to query.
	 *
	 * @param query A string containing the text of the query. This parameter
	 * cannot be null.
	 *
	 * @param queryLanguage A string containing the query language the query is
	 * written in.
	 *
	 * @return CIMInstanceArray A deep enumeration of all instances of the specified
	 * class and all classes derived from the specified class, that match the
	 * query string.
	 *
	 * @exception CIMException If any of the following errors occur:
	 *		The user does not have permission to view the result.
	 *		The requested query language is not supported.
	 *		The query specifies a class that does not exist.
	 */
	virtual void execQuery(
		const blocxx::String& ns,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String& query, const blocxx::String& queryLanguage);
	virtual CIMFeatures getServerFeatures();

	void exportIndication(const CIMInstance& instance, const blocxx::String& instNS);
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	CIMProtocolIFCRef m_protocol;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

};

} // end namespace OW_NAMESPACE

#endif
