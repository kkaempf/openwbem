/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"

#include "OW_ProviderAgentCIMOMHandle.hpp"

#include "OW_CppProviderBaseIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppSecondaryInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_Mutex.hpp"
#include "OW_RWLocker.hpp"
#include "OW_ProviderAgent.hpp"
#include "OW_Assertion.hpp"
#include "OW_ConfigException.hpp"

namespace OpenWBEM
{

//using namespace WBEMFlags;

ProviderAgentCIMOMHandle::ProviderAgentCIMOMHandle(CppProviderBaseIFCRef provider, 
												   ProviderEnvironmentIFCRef env)
	: m_prov(provider)
	, m_PAEnv(env)
	, m_locker(0)
{
	String confItem = m_PAEnv->getConfigItem(ProviderAgent::LockingType_opt, "none"); 
	confItem.toLowerCase(); 
	LockingType lt = NONE; 
	if (confItem == "none")
	{
		lt = NONE; 
	}
	else if (confItem == "swmr")
	{
		lt = SWMR; 
	}
	else if (confItem == "single_threaded")
	{
		lt = SINGLE_THREADED;  
	}
	else
	{
		OW_THROW(ConfigException, "unknown locking type"); 
	}
	confItem = m_PAEnv->getConfigItem(ProviderAgent::LockingTimeout_opt, "300"); 
	UInt32 timeout = 300; 
	try
	{
		timeout = confItem.toUInt32(); 
	}
	catch (StringConversionException&)
	{
		OW_THROW(ConfigException, "invalid locking timeout"); 
	}

	m_locker = PALockerRef(new PALocker(lt, timeout)); 
}
/**
 * Gets the CIM instance for the specified CIM object path.
 *
 * @param ns The namespace.
 * @param instanceName The CIMObjectPath that identifies this CIM instance
 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited
 * 	properties are returned, otherwise all properties are returned.
 *
 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all of
 *	the qualifiers from the class will be returned with the
 *	instance. Otherwise no qualifiers will be included with the
 *	instance.
 *
 * @param includeClassOrigin If set to E_INCLUDE_CLASS_ORIGIN, then the
 * 	class origin attribute will be returned on all appropriate
 * 	elements.
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
 * @exception CIMException If the specified CIMObjectPath cannot be found.
 */
CIMInstance 
ProviderAgentCIMOMHandle::getInstance(const String &ns, 
			const CIMObjectPath &instanceName, 
			WBEMFlags:: ELocalOnlyFlag localOnly,
			WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList)
{
	CppInstanceProviderIFC* pInstProv = m_prov->getInstanceProvider(); 
	CppSecondaryInstanceProviderIFC* pSInstProv = m_prov->getSecondaryInstanceProvider(); 
	if (!pInstProv && !pSInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	CIMInstance rval(CIMNULL); 
	{
		PAReadLock rl(m_locker); 
		if (pInstProv)
		{
			rval = pInstProv->getInstance(m_PAEnv,ns ,instanceName ,localOnly ,
					includeQualifiers ,includeClassOrigin ,
					propertyList , m_cimclass); 
		}
		if (pSInstProv)
		{
			CIMInstanceArray ia; 
			if (pInstProv)
			{
				ia.push_back(rval); 
			}
			else
			{
				CIMInstance newInst(instanceName.getClassName()); 
				newInst.setProperties(instanceName.getKeys()); 
				newInst.setKeys(instanceName.getKeys()); 
				ia.push_back(newInst); 
			}
			pSInstProv->filterInstances(m_PAEnv,ns , instanceName.getClassName(), 
                                        ia,localOnly , OpenWBEM::WBEMFlags::E_SHALLOW, 
										includeQualifiers, includeClassOrigin, 
										propertyList, m_cimclass, m_cimclass); 
			OW_ASSERT(ia.size() == 1); // did the secondary instance provider do something horribly wrong? 
			rval = ia[0]; 
		}
	}
	return rval; 
}
/**
 * Enumerates the qualifiers defined in a namespace.
 * @param ns The namespace whose qualifier definitions are to be
 * 	enumerated.
 * @param result A callback object that will handle the qualifier types
 * 	as they are received.
 */
void 
ProviderAgentCIMOMHandle::enumQualifierTypes(const String &ns, 
			CIMQualifierTypeResultHandlerIFC &result)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
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
CIMValue 
ProviderAgentCIMOMHandle::invokeMethod(const String &ns, 
			const CIMObjectPath &path, 
			const String &methodName, 
			const CIMParamValueArray &inParams, 
			CIMParamValueArray &outParams)
{
	CppMethodProviderIFC* pMethodProv = m_prov->getMethodProvider(); 
	if (!pMethodProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAWriteLock wl(m_locker); 
		return pMethodProv->invokeMethod(m_PAEnv,ns ,path ,methodName , 
				inParams, outParams); 
	}
}
/**
 * Gets the CIM qualifier type specified in the CIM object path.
 * @param ns The namespace
 * @param qualifierName The name of the CIM qualifier type.
 * @return An CIMQualifierType identified by name.
 * @exception CIMException If the CIM qualifier type cannot be found.
 */

CIMQualifierType 
ProviderAgentCIMOMHandle::getQualifierType(const String &ns, const String &qualifierName)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
/**
 * Updates the specified CIM qualifier type in the specified namespace
 * if it exists. If it doesn't exist, it will be added.
 * @param ns The namespace
 * @param qualifierType The CIM qualifier type to be updated or added.
 */
void 
ProviderAgentCIMOMHandle::setQualifierType(const String &ns, const CIMQualifierType &qualifierType)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
/**
 * Deletes a CIM qualfier type.
 * @param ns The namespace containing the qualifier type
 * @param qualName The qualifier type to delete.
 * @exception CIMException If the qualifier type does not exist.
 */
void 
ProviderAgentCIMOMHandle::deleteQualifierType(const String &ns, const String &qualName)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
/**
 * Adds the CIM class to the specified namespace.
 * @param ns The namespace
 * @param cimClass The CIMClass to be added
 * @exception CIMException If the CIM class already exists in the
 *	namespace.
 */
void 
ProviderAgentCIMOMHandle::createClass(const String &ns, const CIMClass &cimClass)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
/**
 * Deletes the CIM class specified by className from namespace ns.
 * @param ns The namespace containing the class to delete.
 * @param className The class to delete.
 * @exception CIMException If the object does not exist
 */
void 
ProviderAgentCIMOMHandle::deleteClass(const String &ns, const String &className)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
/**
 * Updates the CIM class within the specified namespace.
 * @param ns The namespace
 * @param cimClass The CIMClass to be updated
 * @exception CIMException If the class does not exists
 */
void 
ProviderAgentCIMOMHandle::modifyClass(const String &ns, const CIMClass &cimClass)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
/**
 * Deletes the CIM instance specified by path from namespace ns.
 * @param ns The namespace containing the instance.
 * @param path The CIMObjectPath identifying the instance to delete.
 * @exception CIMException If the instance does not exist.
 */
void 
ProviderAgentCIMOMHandle::deleteInstance(const String &ns, const CIMObjectPath &path)
{
	CppInstanceProviderIFC* pInstProv = m_prov->getInstanceProvider(); 
	CppSecondaryInstanceProviderIFC* pSInstProv = m_prov->getSecondaryInstanceProvider(); 
	if (!pInstProv && !pSInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAWriteLock wl(m_locker); 
		if (pInstProv)
		{
			pInstProv->deleteInstance(m_PAEnv,ns , path); 
		}
		if (pSInstProv)
		{
			pSInstProv->deleteInstance(m_PAEnv,ns , path); 
		}
	}
}
/**
 * Set the specified CIM instance property.
 * @param ns The namespace containing the instance to modify.
 * @param instanceName An CIMObjectPath that identifies the CIM
 * 	instance to be accessed.
 * @param propertyName The name of the property to set the value on.
 * @param newValue The new value for property propertyName.
 * @exception CIMException
 */
void 
ProviderAgentCIMOMHandle::setProperty(const String &ns, const CIMObjectPath &instanceName, const String &propertyName, const CIMValue &newValue)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
		// CIMOM will never demand this of us.  It always translates 
		// to modifyInstance. 
	}
/**
 * Update the specified CIM instance within the specified namespace.
 * @param ns The namespace
 * @param modifiedInstance The CIMInstance to be updated.
 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, the
 * 	qualifiers will also be modified.
 * @param propertyList The properties to modify.  0 means all properties.
 * @exception CIMException
 */
void 
ProviderAgentCIMOMHandle::modifyInstance(const String &ns, 
			const CIMInstance &modifiedInstance, 
			WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
			const StringArray *propertyList)
	{
		CppInstanceProviderIFC* pInstProv = m_prov->getInstanceProvider(); 
		CppSecondaryInstanceProviderIFC* pSInstProv = m_prov->getSecondaryInstanceProvider(); 
		if (!pInstProv && !pSInstProv)
		{
			OW_THROWCIM(CIMException::NOT_SUPPORTED); 
		}
		{
			PAWriteLock wl(m_locker); 
			if (pInstProv)
			{
				pInstProv->modifyInstance(m_PAEnv,ns ,modifiedInstance , 
						CIMInstance(CIMNULL),	// previousInstance unavailable
						includeQualifiers ,
						propertyList , m_cimclass); 
			}
			if (pSInstProv)
			{
				pSInstProv->modifyInstance(m_PAEnv,ns ,modifiedInstance , 
										   CIMInstance(CIMNULL),
										   includeQualifiers ,propertyList , 
										   m_cimclass); 
			}
		}
	}
/**
 * Add the specified CIM instance to the specified namespace.
 * @param ns The namespace
 * @param instance the CIMInstance to be added
 * @return An CIMObjectPath of the created instance.
 * @exception CIMException If the CIM instance already exists in the
 *	namespace
 */
CIMObjectPath 
ProviderAgentCIMOMHandle::createInstance(const String &ns, const CIMInstance &instance)
{
	CppInstanceProviderIFC* pInstProv = m_prov->getInstanceProvider(); 
	CppSecondaryInstanceProviderIFC* pSInstProv = m_prov->getSecondaryInstanceProvider(); 
	if (!pInstProv && !pSInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	CIMObjectPath rval(CIMNULL); 
	{
		PAWriteLock wl(m_locker); 
		if (pInstProv)
		{
			rval = pInstProv->createInstance(m_PAEnv,ns , instance); 
		}
		if (pSInstProv)
		{
			pSInstProv->createInstance(m_PAEnv,ns , instance); 
		}
	}
	return rval; 
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
/**
 * Get the specified CIM instance property.
 * @param ns The namespace
 * @param instanceName A CIMObjectPath that identifies the CIM
 *	instance to be accessed.
 * @param propertyName	The name of the property to retrieve.
 * @return The CIMvalue for property identified by propertyName.
 * @exception CIMException
 */
CIMValue 
ProviderAgentCIMOMHandle::getProperty(const String &ns, 
			const CIMObjectPath &instanceName, 
			const String &propertyName)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	// CIMOM will never demand this of us.  
}

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
 * @exception CIMException - as defined in the associator method
 */
void 
ProviderAgentCIMOMHandle::associatorNames(const String &ns, 
			const CIMObjectPath &objectName, 
			CIMObjectPathResultHandlerIFC &result, 
			const String &assocClass,
			const String &resultClass,
			const String &role,
			const String &resultRole)
{
	CppAssociatorProviderIFC* pAssocProv = m_prov->getAssociatorProvider(); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->associatorNames(m_PAEnv,result ,ns ,objectName ,
				assocClass ,resultClass ,role ,resultRole); 
	}
}
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
void 
ProviderAgentCIMOMHandle::associatorsClasses(const String &ns, 
				const CIMObjectPath &path, 
				CIMClassResultHandlerIFC &result, 
				const String &assocClass, 
				const String &resultClass, 
				const String &role, 
				const String &resultRole, 
				WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, 
				WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, 
				const StringArray *propertyList)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
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
void 
ProviderAgentCIMOMHandle::associators(const String &ns, const CIMObjectPath &path, 
		CIMInstanceResultHandlerIFC &result, 
		const String &assocClass, 
		const String &resultClass, 
		const String &role, 
		const String &resultRole, 
		WBEMFlags:: EIncludeQualifiersFlag 
		includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray *propertyList)
{
	CppAssociatorProviderIFC* pAssocProv = m_prov->getAssociatorProvider(); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->associators(m_PAEnv,result ,ns , path,assocClass ,
				resultClass ,role ,resultRole ,
				includeQualifiers ,includeClassOrigin ,
				propertyList); 
	}
}
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
void 
ProviderAgentCIMOMHandle::referenceNames(const String &ns, 
		const CIMObjectPath &path, 
		CIMObjectPathResultHandlerIFC &result, 
		const String &resultClass,
		const String &role)
{
	CppAssociatorProviderIFC* pAssocProv = m_prov->getAssociatorProvider(); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->referenceNames(m_PAEnv,result ,ns , path,resultClass ,role); 
	}
}
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
void 
ProviderAgentCIMOMHandle::references(const String &ns, 
		const CIMObjectPath &path, 
		CIMInstanceResultHandlerIFC &result, 
		const String &resultClass,
		const String &role,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	CppAssociatorProviderIFC* pAssocProv = m_prov->getAssociatorProvider(); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->references(m_PAEnv,result ,ns , path,resultClass ,role ,
				includeQualifiers ,
				includeClassOrigin ,
				propertyList); 
	}
}
/**
 * This operation is used to enumerate the association objects that
 * refer to a particular target CIM Class.
 *
 * @see references
 *
 * @exception CIMException - as defined for associators method.
 */
void 
ProviderAgentCIMOMHandle::referencesClasses(const String &ns, 
		const CIMObjectPath &path, 
		CIMClassResultHandlerIFC &result, 
		const String &resultClass,
		const String &role,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
/**
 * Close the connetion to the CIMOM. This will free resources used for the
 * client session. The destructor will call close().
 */
void 
ProviderAgentCIMOMHandle::close()
{
}
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
void 
ProviderAgentCIMOMHandle::enumClassNames(const String &ns, 
		const String &className, 
		StringResultHandlerIFC &result, 
		WBEMFlags:: EDeepFlag deep)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
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
 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all
 * 	class, property and method qualifiers will be returned.
 * @param includeClassOrigin If E_INCLUDE_CLASS_ORIGIN, then the class
 * 	origin attribute will be included with all appropriate elements
 * 	of each class.
 * @exception CIMException If the specified class cannot be found
 */
void 
ProviderAgentCIMOMHandle::enumClass(const String &ns, 
		const String &className, 
		CIMClassResultHandlerIFC &result, 
		WBEMFlags:: EDeepFlag deep,
		WBEMFlags:: ELocalOnlyFlag localOnly,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
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
 * @param deep Return properties defined on subclasses of the class in
 *	path
 *
 * @param localOnly If set to E_LOCAL_ONLY, only the non-inherited
 *	properties are returned on each instance, otherwise all
 *	inherited properties are returned.
 *
 * @param includeQualifiers If set to E_INCLUDE_QUALIFIERS, then all of
 *	the qualifiers from the class will be returned with the each
 *	instance.
 *
 * @param includeClassOrigin If E_INCLUDE_CLASS_ORIGIN, then the class
 *	origin attribute will be returned on all appropriate elements.
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
void 
ProviderAgentCIMOMHandle::enumInstances(const String &ns, 
		const String &className, 
		CIMInstanceResultHandlerIFC &result, 
		WBEMFlags:: EDeepFlag deep,
		WBEMFlags:: ELocalOnlyFlag localOnly,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	CppInstanceProviderIFC* pInstProv = m_prov->getInstanceProvider(); 
	if (!pInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pInstProv->enumInstances(m_PAEnv,ns ,className ,result ,localOnly ,
				deep ,includeQualifiers ,includeClassOrigin,
				propertyList, 
				CIMClass(CIMNULL),	// requestedClass is unavailable
				m_cimclass); 
	}
}
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
void 
ProviderAgentCIMOMHandle::enumInstanceNames(const String &ns, 
		const String &className, 
		CIMObjectPathResultHandlerIFC &result)
{
	CppInstanceProviderIFC* pInstProv = m_prov->getInstanceProvider(); 
	if (!pInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pInstProv->enumInstanceNames(m_PAEnv,ns ,className ,result , m_cimclass); 
	}
}
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
CIMClass 
ProviderAgentCIMOMHandle::getClass(const String &ns, 
		const String &className, 
		WBEMFlags:: ELocalOnlyFlag localOnly,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
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
void 
ProviderAgentCIMOMHandle::execQuery(const String &ns, 
		CIMInstanceResultHandlerIFC &result, 
		const String &query, 
		const String &queryLanguage)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PALocker::PALocker(ProviderAgentCIMOMHandle::LockingType lt, 
											 UInt32 timeout)
	: m_lt(lt)
	, m_mutex(0)
	, m_rwlocker(0)
	, m_timeout(timeout)
{
	switch (m_lt)
	{
	case NONE:
		break; 
	case SWMR:
		m_rwlocker = new RWLocker; 
		break; 
	case SINGLE_THREADED:
		m_mutex = new Mutex; 
		break; 
	default: 
		OW_ASSERT("shouldn't happen" == 0); 
	}
}

//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PALocker::~PALocker()
{
	if (m_mutex)
	{
		delete m_mutex; 
	}
	if (m_rwlocker)
	{
		delete m_rwlocker; 
	}
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAReadLock::PAReadLock(PALockerRef locker)
	: m_locker(locker.get())
{
	m_locker->getReadLock(); 
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAReadLock::~PAReadLock()
{
	m_locker->releaseReadLock(); 
}
//////////////////////////////////////////////////////////////////////////////
void
ProviderAgentCIMOMHandle::PALocker::releaseReadLock()
{
	switch (m_lt)
	{
	case NONE:
		break; 
	case SWMR:
		m_rwlocker->releaseReadLock(); 
		break; 
	case SINGLE_THREADED:
		m_mutex->release(); 
		break; 
	default: 
		OW_ASSERT("shouldn't happen" == 0); 
	}
}
//////////////////////////////////////////////////////////////////////////////
void
ProviderAgentCIMOMHandle::PALocker::getReadLock()
{
	switch (m_lt)
	{
	case NONE:
		break; 
	case SWMR:
		m_rwlocker->getReadLock(m_timeout); 
		break; 
	case SINGLE_THREADED:
		m_mutex->acquire(); 
		break; 
	default: 
		OW_ASSERT("shouldn't happen" == 0); 
	}
}
//////////////////////////////////////////////////////////////////////////////
void
ProviderAgentCIMOMHandle::PALocker::releaseWriteLock()
{
	switch (m_lt)
	{
	case NONE:
		break; 
	case SWMR:
		m_rwlocker->releaseWriteLock(); 
		break; 
	case SINGLE_THREADED:
		m_mutex->release(); 
		break; 
	default: 
		OW_ASSERT("shouldn't happen" == 0); 
	}
}
//////////////////////////////////////////////////////////////////////////////
void
ProviderAgentCIMOMHandle::PALocker::getWriteLock()
{
	switch (m_lt)
	{
	case NONE:
		break; 
	case SWMR:
		m_rwlocker->getWriteLock(m_timeout); 
		break; 
	case SINGLE_THREADED:
		m_mutex->acquire(); 
		break; 
	default: 
		OW_ASSERT("shouldn't happen" == 0); 
	}
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAWriteLock::PAWriteLock(PALockerRef locker)
	: m_locker(locker.get())
{
	m_locker->getWriteLock(); 
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAWriteLock::~PAWriteLock()
{
	m_locker->releaseWriteLock(); 
}
//////////////////////////////////////////////////////////////////////////////


} // end namespace OpenWBEM

