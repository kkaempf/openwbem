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

#ifndef OW_INDICATION_REP_LAYER_IMPL_HPP_
#define OW_INDICATION_REP_LAYER_IMPL_HPP_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_RepositoryIFC.hpp"
#include "OW_IndicationRepLayer.hpp"

class OW_CIMServer;

class OW_IndicationRepLayerImpl : public OW_IndicationRepLayer
{
public:
	OW_IndicationRepLayerImpl() : OW_IndicationRepLayer(), m_pServer(0) {}
	OW_IndicationRepLayerImpl(OW_CIMServer* pRepos);
	OW_IndicationRepLayerImpl(const OW_IndicationRepLayerImpl& arg);

	OW_IndicationRepLayerImpl& operator= (const OW_IndicationRepLayerImpl& arg);

	virtual OW_CIMClass deleteClass(const OW_CIMObjectPath &path,
		const OW_ACLInfo& aclInfo);

	virtual void open(const OW_String&) {}

	virtual void close();

	virtual OW_CIMOMEnvironmentRef getEnvironment() const
	{
		return m_pServer->getEnvironment();
	}

	virtual void deleteQualifierType(const OW_CIMObjectPath &path,
		const OW_ACLInfo& aclInfo);

	virtual OW_CIMClassEnumeration enumClass(const OW_CIMObjectPath& path,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo& aclInfo);

	virtual OW_CIMObjectPathEnumeration enumClassNames(
		const OW_CIMObjectPath &path, OW_Bool deep, const OW_ACLInfo& aclInfo);

	virtual OW_CIMInstance deleteInstance(const OW_CIMObjectPath &path,
		const OW_ACLInfo& aclInfo);

	virtual OW_CIMQualifierTypeEnumeration enumQualifierTypes(const
		OW_CIMObjectPath &path, const OW_ACLInfo& aclInfo);

	virtual OW_CIMInstance getCIMInstance(const OW_CIMObjectPath& cop,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	virtual OW_CIMInstanceEnumeration getCIMInstances(
		const OW_CIMObjectPath& path, OW_Bool deep, OW_Bool localOnly,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo);

	virtual OW_CIMObjectPathEnumeration getCIMInstanceNames(
		const OW_CIMObjectPath& path, OW_Bool deep, const OW_ACLInfo& aclInfo);

	virtual OW_CIMQualifierType getQualifierType(
		const OW_CIMObjectPath &name, const OW_ACLInfo& aclInfo);

	virtual OW_CIMClass getClass(const OW_CIMObjectPath& path,
		OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
		const OW_ACLInfo& aclInfo);

	virtual void addQualifierType(const OW_CIMObjectPath& name,
		const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo);

	virtual OW_CIMValue invokeMethod(const OW_CIMObjectPath &name,
		const OW_String &methodName, const OW_CIMValueArray &inParams,
		OW_CIMValueArray &outParams, const OW_ACLInfo& aclInfo);

	virtual void createClass(const OW_CIMObjectPath &name,
		OW_CIMClass &cc, const OW_ACLInfo& aclInfo);

	virtual void updateQualifierType(const OW_CIMObjectPath& name,
		const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo);

	virtual OW_CIMObjectPath createInstance(const OW_CIMObjectPath &name,
		OW_CIMInstance &ci, const OW_ACLInfo& aclInfo);

	virtual OW_CIMClass updateClass(const OW_CIMObjectPath& name,
		OW_CIMClass& cc, const OW_ACLInfo& aclInfo);

	virtual void setProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_CIMValue &cv,
		const OW_ACLInfo& aclInfo);

	virtual OW_CIMInstance updateInstance(const OW_CIMObjectPath& cop,
		OW_CIMInstance& ci, const OW_ACLInfo& aclInfo);

	virtual OW_CIMInstanceEnumeration associators(const OW_CIMObjectPath &path,
		const OW_String &assocClass, const OW_String &resultClass,
		const OW_String &role, const OW_String &resultRole,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo);

	virtual OW_CIMValue getProperty(const OW_CIMObjectPath &name,
		const OW_String &propertyName, const OW_ACLInfo& aclInfo);

	virtual OW_CIMInstanceEnumeration references(const OW_CIMObjectPath &path,
		const OW_String &resultClass, const OW_String &role,
		OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo);

	virtual OW_CIMObjectPathEnumeration associatorNames(
		const OW_CIMObjectPath &path, const OW_String &assocClass,
		const OW_String &resultClass, const OW_String &role,
		const OW_String &resultRole, const OW_ACLInfo& aclInfo);

	virtual OW_CIMObjectPathEnumeration referenceNames(
		const OW_CIMObjectPath &path, const OW_String &resultClass,
		const OW_String &role, const OW_ACLInfo& aclInfo);
	
	virtual OW_CIMInstanceArray execQuery(const OW_CIMNameSpace &cop,
		const OW_String &query, const OW_String& queryLanguage,
		const OW_ACLInfo& aclInfo);

	/**
	 * Delete a specified namespace.
	 * @param ns	The OW_CIMNameSpace object that identifies the namespace
	 *					to delete.
	 * @exception OW_CIMException If the namespace does not exist.
	 */
	void deleteNameSpace(const OW_CIMNameSpace &ns, const OW_ACLInfo& aclInfo);

	/**
	 * Create a cim namespace.
	 * @param ns 	The OW_CIMNameSpace object that specified a string for the
	 *					host and a string for the namespace.
	 * @exception OW_CIMException If the namespace already exists.
	 */
	void createNameSpace(const OW_CIMNameSpace &ns, const OW_ACLInfo& aclInfo);

	/**
	 * Gets a list of the namespaces within the namespace specified by the CIM
	 * object path.
	 * @param path	The OW_CIMObjectPath identifying the namespace to be
	 *					enumerated.
	 * @param deep	If set to OW_CIMClient::DEEP, the enumeration returned will
	 *					contain the entire hierarchy of namespaces present
	 *             under the enumerated namespace. If set to
	 *					OW_CIMClient::SHALLOW  the enuermation will return only the
	 *					first level children of the enumerated namespace.
	 * @return An Array of namespace names as strings.
	 * @exception 	OW_CIMException If the namespace does not exist or the object
	 *					cannot be found in the specified namespace.
	 */
	OW_StringArray enumNameSpace(const OW_CIMNameSpace& path, OW_Bool deep,
		const OW_ACLInfo& aclInfo);

	virtual OW_ReadLock getReadLock();
	virtual OW_WriteLock getWriteLock();

	operator void*() { return (void*)m_pServer; }

	virtual void setCIMServer(OW_RepositoryIFC *src);


private:

	void exportIndication(const OW_CIMInstance& instance,
		const OW_CIMNameSpace& instNS);

	OW_RepositoryIFC* m_pServer;
};

#endif


