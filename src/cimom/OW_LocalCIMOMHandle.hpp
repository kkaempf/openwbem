/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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
#ifndef OW_LOCALCIMOMHANDLE_HPP_INCLUDE_GUARD_
#define OW_LOCALCIMOMHANDLE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_RWLocker.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_UserInfo.hpp"

namespace OpenWBEM
{

/**
 * The LocalCIMOMHandle class is a derivitive of the CIMOMHandleIFC that
 * is used by all components that need access to CIM Services.
 */
class LocalCIMOMHandle : public CIMOMHandleIFC
{
public:
	/**
	 * Default constructor. This will create an invalid LocalCIMOMHandle that
	 * is not suitable for any operations.
	 */
	LocalCIMOMHandle() : CIMOMHandleIFC(), m_pServer(0), m_aclInfo(String()) {}
	/**
	 * Create a new LocalCIMOMHandle with a given repository interface
	 * and user access contol information.
	 * @param env A reference to an CIMOMEnvironment object.
	 * @param pRepos A reference to an Repository that will be used by this
	 *		LocalCIMOMHandle.
	 * @param aclInfo	The access control information that will be associated with
	 *		this LocalCIMOMHandle.
	 */
	LocalCIMOMHandle(CIMOMEnvironmentRef env, RepositoryIFCRef pRepos,
		const UserInfo& aclInfo);
	/**
	 * Copy constructor
	 * @param arg	The LocalCIMOMHandle object to make this one a copy of.
	 */
	LocalCIMOMHandle(const LocalCIMOMHandle& arg);
	/**
	 * Assignment operator
	 * @param arg	The LocalCIMOMHandle that will be assigned to this one.
	 * @return A reference to this LocalCIMOMHandle.
	 */
	LocalCIMOMHandle& operator= (const LocalCIMOMHandle& arg);
	/**
	 * @return A reference to the CIMOMEnvironment used by this object.
	 */
	CIMOMEnvironmentRef getEnvironment() const { return m_env; }
	/**
	 * Close the connetion to the CIMOM. Currently this does nothing on the
	 * local cimom handle.
	 */
	virtual void close();
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
	virtual void enumClass(
		const String& ns,
		const String& className,
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
	 */
	virtual void enumClassNames(
		const String& ns,
		const String& className,
		StringResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep=WBEMFlags::E_DEEP);
	/**
	 * Gets the CIM qualifier type specified in the CIM object path.
	 * @param name	The CIMObjectPath that identifies the CIM qualifier type.
	 * @return An CIMQualifierType identified by name.
	 */
	virtual CIMQualifierType getQualifierType(const String& ns,
		const String& qualifierName);
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	/**
	 * Enumerates the qualifiers defined in a namespace.
	* @param path	The CIMObjectPath identifying the namespace whose qualifier
	 *					definitions are to be enumerated.
	 */
	virtual void enumQualifierTypes(
		const String& ns,
		CIMQualifierTypeResultHandlerIFC& result);
	/**
	 * Deletes the CIM qualfier for the object specified by the CIM object path.
	 * @param path	The CIMObjectPath identifying the qualifier type to delete.
	 */
	virtual void deleteQualifierType(const String& ns, const String& qualName);
	/**
	 * Updates the specified CIM qualifier type in the specified namespace if
	 * it exist. If it doesn't exist, it will be added.
	 * @param name	An CIMObjectPath that identifies the CIM qualifier type.
	 * @param qt	The CIM qualifier type to be updated or added.
	 */
	virtual void setQualifierType(const String& ns,
		const CIMQualifierType& qt);
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
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
	 */
	virtual CIMInstance getInstance(
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList=0);
	/**
	 * Returns all instances (the whole instance and not just the names)
	 * belonging to the class specified in the path. This could include
	 * instances of all the classes in the specified class' hierarchy.
	 *
	 * @param path The CIMObjectPath identifying the class whose instances are
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
	 */
	virtual void enumInstances(
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::EDeepFlag deep = WBEMFlags::E_DEEP, 
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_EXCLUDE_QUALIFIERS, 
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList=0);
	/**
	 * Returns all instance names belonging to the class specified in the path.
	 * This could include instances of all the classes in the specified class'
	 * hierarchy.
	 *
	 * @param path			The CIMObjectPath identifying the class whose
	 *							instances are to be enumerated.
	 * @param deep			If set to CIMClient::DEEP, the enumeration returned
	 *							will contain instances of the specified class and all
	 *							classes derived from it. If set to
	 *							CIMClient::SHALLOW only instances belonging
	 *							to the specified class are returned.
	 */
	virtual void enumInstanceNames(
		const String& ns,
		const String& className,
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
		const String& ns,
		const String& className,
		WBEMFlags::ELocalOnlyFlag localOnly = WBEMFlags::E_NOT_LOCAL_ONLY,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers = WBEMFlags::E_INCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin = WBEMFlags::E_INCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList=NULL);
	/**
	 * Executes the specified method on the specified object. A method is a
	 * declaration containing the method name, return type, and parameters in
	 * the method.
	 * @param name			An CIMObjectPath that identifies the method
	 * @param methodName	The string name of the method to be invoked
	 * @param inParams	The input parameters specified as an CIMValueArray.
	 * @param outParams	The output parameters.
	 * @return A non-null CIMValue on success. Otherwise a null CIMValue.
	 */
	virtual CIMValue invokeMethod(
		const String& ns,
		const CIMObjectPath& path,
		const String &methodName, const CIMParamValueArray &inParams,
		CIMParamValueArray &outParams);
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	/**
	 * Adds the CIM class to the specified namespace.
	 * @param name	An CIMObjectPath that identifies the CIM class to be added.
	 * @param cc	The CIMClass to be added
	 */
	virtual void createClass(const String& ns,
		const CIMClass &cc);
	/**
	 * Updates the CIM class associated with the specified namespace.
	 * @param name	An CIMObjectPath that identifies the CIM class to be
	 *					updated.
	 * @param cc	The CIMClass to be updated
	 */
	virtual void modifyClass(const String &ns, const CIMClass &cc);
	/**
	* @param ns The namespace containing the class to delete.
	* @param className The class to delete
	 */
	virtual void deleteClass(const String& ns, const String& className);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Add the specified CIM instance to the specified namespace.
	 * @param name	The CIMObjectPath that identifies the CIM instance to be
	 *					added.
	 * @param ci	the CIMInstance to be added
	 * @return An CIMObjectPath of the created instance.
	 */
	virtual CIMObjectPath createInstance(const String& ns,
		const CIMInstance &ci);
	/**
	 * Set the specified CIM instance property.
	 * @param name An CIMObjectPath that identifies the CIM instance to be
	 *		accessed
	 * @param propertyName The name of the property to set the value on.
	 * @param newValue The new value for property propertyName.
	 */
	virtual void setProperty(
		const String& ns,
		const CIMObjectPath &name,
		const String &propertyName, const CIMValue &cv);
	/**
	 * Update or add the specified CIM instance associated with the specified
	 * namespace.
	 * @param name An CIMObjectPath that identifies the CIM instance to be
	 *		updated.
	 * @param ci The CIMInstance to be updated.
	 */
	virtual void modifyInstance(
		const String& ns,
		const CIMInstance& modifiedInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList);
	
	/**
	 * Deletes the CIM instance specified by the CIM object path.
	 * A CIM object path consists of two  parts: namespace + model path. The
	 * model path is created by concatenating the properties of a class that are
	 * qualified with the KEY qualifier.
	 * @param path	The CIMObjectPath identifying the instance to delete.
	 */
	virtual void deleteInstance(const String& ns, const CIMObjectPath& path);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	/**
	 * Get the specified CIM instance property.
	 * @param name An CIMObjectPath that identifies the CIM instance to be
	 *		accessed
	 * @param propertyName The name of the property to retrieve.
	 * @return The CIMvalue for property identified by propertyName.
	 */
	virtual CIMValue getProperty(
		const String& ns,
		const CIMObjectPath &name,
		const String &propertyName);
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
		const String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const String &assocClass, const String &resultClass,
		const String &role, const String &resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList);
	virtual void associatorsClasses(
		const String& ns,
		const CIMObjectPath& path,
		CIMClassResultHandlerIFC& result,
		const String& assocClass=String(),
		const String& resultClass=String(),
		const String& role=String(),
		const String& resultRole=String(),
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers=WBEMFlags::E_EXCLUDE_QUALIFIERS,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin=WBEMFlags::E_EXCLUDE_CLASS_ORIGIN,
		const StringArray* propertyList=0);
	
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
	virtual void references(
		const String& ns,
		const CIMObjectPath &path,
		CIMInstanceResultHandlerIFC& result,
		const String &resultClass, const String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList);
	virtual void referencesClasses(
		const String& ns,
		const CIMObjectPath &path,
		CIMClassResultHandlerIFC& result,
		const String &resultClass, const String &role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList);
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
		const String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const String &assocClass,
		const String &resultClass, const String &role,
		const String &resultRole);
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
		const String& ns,
		const CIMObjectPath &path,
		CIMObjectPathResultHandlerIFC& result,
		const String &resultClass,
		const String &role);
#endif
	
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
	 * @return CIMInstanceArray A deep enumeration of all instances of the
	 * specified class and all classes derived from the specified class, that
	 * match the query string.
	 */
	virtual void execQuery(
		const String& ns,
		CIMInstanceResultHandlerIFC& result,
		const String &query, const String& queryLanguage);
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
	void exportIndication(const CIMInstance& instance,
		const String& instNS);
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
	enum ELockingFlag
	{
		E_NO_LOCKING,
		E_LOCKING
	};
	/**
	 * Create a new LocalCIMOMHandle with a given repository interface
	 * and user access contol information.
	 * @param env A reference to an CIMOMEnvironment object.
	 * @param pRepos A reference to a Repository that will be used by this
	 *		LocalCIMOMHandle.
	 * @param aclInfo	The access control information that will be associated with
	 *		this LocalCIMOMHandle.
	 * @param noLock If true, the this object will never attempt to acquire a
	 *		read/write lock on the CIMServer.
	 */
	LocalCIMOMHandle(CIMOMEnvironmentRef env, RepositoryIFCRef pRepos,
		const UserInfo& aclInfo, ELockingFlag lock);
	void getSchemaReadLock();
	void getSchemaWriteLock();
	void releaseSchemaReadLock();
	void releaseSchemaWriteLock();
	void getInstanceReadLock();
	void getInstanceWriteLock();
	void releaseInstanceReadLock();
	void releaseInstanceWriteLock();
private:
	/**
	 * A Reference to the Repository interface that this LocalCIMOMHandle
	 * will use.
	 */
	RepositoryIFCRef m_pServer;
	/**
	 * The user access control information that is associated with this
	 * LocalCIMOMHandle
	 */
	UserInfo m_aclInfo;
	/**
	 * If m_lock is E_NO_LOCKING, then this LocalCIMOMHandle will never attempt
	 * to acquire a read/write lock on the cim server.
	 */
	ELockingFlag m_lock;
	CIMOMEnvironmentRef m_env;
};
typedef Reference<LocalCIMOMHandle> LocalCIMOMHandleRef;

} // end namespace OpenWBEM

#endif
