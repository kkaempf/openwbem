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

#include "OW_config.h"
#include "OW_IndicationRepLayerImpl.hpp"
#include "OW_Format.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_IndicationRepLayerMediator.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_UserInfo.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_IndicationRepLayer::~OW_IndicationRepLayer() 
{
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationRepLayerImpl::~OW_IndicationRepLayerImpl()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	OW_CIMInstance theInst = m_pServer->getInstance(ns, instanceName, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	
	if (m_pEnv->getIndicationRepLayerMediator()->getInstReadSubscriptionCount() > 0)
	{
		try
		{
			OW_CIMInstance expInst("CIM_InstRead");
			expInst.setProperty("SourceInstance", OW_CIMValue(theInst));
			exportIndication(expInst, ns);
		}
		catch (OW_CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for getInstance"
				" because CIM_InstRead does not exist");
		}
	}

	return theInst;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_IndicationRepLayerImpl::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String& methodName, const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams, const OW_UserInfo& aclInfo)
{
	OW_CIMValue rval = m_pServer->invokeMethod(ns, path, methodName, inParams,
		outParams, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getInstMethodCallSubscriptionCount() > 0)
	{
		if (path.isInstancePath()) // process the indication only if instance.
		{
			try
			{
				OW_CIMInstance expInst("CIM_InstMethodCall");

				OW_UserInfo intAclInfo;
				OW_CIMInstance theInst = m_pServer->getInstance(ns, path, false,
					true, true, NULL, intAclInfo);
	
				if (!theInst)
				{
					// can't export indication
					return rval;
				}
	
				OW_CIMInstance ParamsEmbed;
				ParamsEmbed.setClassName("__MethodParameters");
	
				for(size_t i = 0; i < inParams.size(); i++)
				{
					OW_CIMProperty prop(inParams[i].getName(), inParams[i].getValue());
					ParamsEmbed.setProperty(prop);
				}
	
				for(size_t i = 0; i < outParams.size(); i++)
				{
					OW_CIMProperty prop(outParams[i].getName(), outParams[i].getValue());
					ParamsEmbed.setProperty(prop);
				}
	
	
				expInst.setProperty("SourceInstance", OW_CIMValue(theInst)); // from CIM_InstIndication
				expInst.setProperty("MethodName", OW_CIMValue(methodName));
				expInst.setProperty("MethodParameters", OW_CIMValue(ParamsEmbed));
				expInst.setProperty("PreCall", OW_CIMValue(OW_Bool(false)));
				expInst.setProperty("ReturnValue", OW_CIMValue(rval.toString()));
				exportIndication(expInst, ns);
			}
			catch (OW_CIMException&)
			{
				m_pEnv->logDebug("Unable to export indication for"
					" invokeMethod because CIM_InstMethodCall does not exist");
			}
		}
	}

	return rval;
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::modifyClass(const OW_String &ns,
	const OW_CIMClass& cc, const OW_UserInfo& aclInfo)
{
	OW_CIMClass CCOrig = m_pServer->modifyClass(ns, cc, aclInfo);
	
	if (m_pEnv->getIndicationRepLayerMediator()->getClassModificationSubscriptionCount() > 0)
	{
	
		try
		{
			OW_CIMInstance expInst("CIM_ClassModification");
			expInst.setProperty("PreviousClassDefinition", OW_CIMValue(CCOrig));
			expInst.setProperty("ClassDefinition", OW_CIMValue(cc));
			exportIndication(expInst, ns);
		}
		catch (OW_CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for modifyClass"
				" because CIM_ClassModification does not exist");
		}
	}
	return CCOrig;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::createClass(const OW_String& ns,
	const OW_CIMClass& cc, const OW_UserInfo& aclInfo)
{
	m_pServer->createClass(ns, cc, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getClassCreationSubscriptionCount() > 0)
	{
	
		try
		{
			OW_CIMInstance expInst("CIM_ClassCreation");
			expInst.setProperty("ClassDefinition", OW_CIMValue(cc));
			exportIndication(expInst, ns);
		}
		catch(OW_CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for createClass"
				" because CIM_ClassCreation does not exist");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_UserInfo& aclInfo)
{
	OW_CIMClass cc = m_pServer->deleteClass(ns, className, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getClassDeletionSubscriptionCount() > 0)
	{
		try
		{
			OW_CIMInstance expInst("CIM_ClassDeletion");
			expInst.setProperty("ClassDefinition", OW_CIMValue(cc));
			exportIndication(expInst, ns);
		}
		catch (OW_CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for"
				" deleteClass because CIM_ClassDeletion does not exist");
		}
	}
	
	return cc;
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_UserInfo& aclInfo)
{
	OW_CIMInstance ciOrig = m_pServer->modifyInstance(ns, modifiedInstance,
		includeQualifiers, propertyList, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getInstModificationSubscriptionCount() > 0)
	{
		try
		{
			OW_CIMInstance expInst("CIM_InstModification");
			expInst.setProperty("PreviousInstance", OW_CIMValue(ciOrig));
			// Filtering the properties in ss is done per the CIM_Interop 
			// schema MOF by the indication server, we don't need to do it here.
			expInst.setProperty("SourceInstance", OW_CIMValue(modifiedInstance));
			exportIndication(expInst, ns);
		}
		catch (OW_CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for modifyInstance"
				" because CIM_InstModification does not exist");
		}
	}
	return ciOrig;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_IndicationRepLayerImpl::createInstance(const OW_String& ns,
	const OW_CIMInstance& ci, const OW_UserInfo& aclInfo)
{
	OW_CIMObjectPath rval = m_pServer->createInstance(ns, ci, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getInstCreationSubscriptionCount() > 0)
	{
		try
		{
			OW_CIMInstance expInst("CIM_InstCreation");
			expInst.setProperty("SourceInstance", OW_CIMValue(ci));
			exportIndication(expInst, ns);
		}
		catch(OW_CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for createInstance"
				" because CIM_InstCreation does not exist");
		}
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& path,
	const OW_UserInfo& aclInfo)
{
	OW_CIMInstance instOrig = m_pServer->deleteInstance(ns, path, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getInstDeletionSubscriptionCount() > 0)
	{
		try
		{
			OW_CIMInstance expInst("CIM_InstDeletion");
			expInst.setProperty("SourceInstance", OW_CIMValue(instOrig));
			exportIndication(expInst, ns);
		}
		catch (OW_CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for deleteInstance"
				" because CIM_InstDeletion does not exist");
		}
	}
	return instOrig;
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


OW_IndicationRepLayerImpl::OW_IndicationRepLayerImpl() : OW_IndicationRepLayer(), m_pServer(0) {}

void OW_IndicationRepLayerImpl::open(const OW_String&) {}

void OW_IndicationRepLayerImpl::close() {}

OW_ServiceEnvironmentIFCRef OW_IndicationRepLayerImpl::getEnvironment() const
{
	return m_pEnv;
}


void OW_IndicationRepLayerImpl::enumClasses(const OW_String& ns,
	const OW_String& className,
	OW_CIMClassResultHandlerIFC& result,
	OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_UserInfo& aclInfo)
{
	m_pServer->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, aclInfo);
}


void OW_IndicationRepLayerImpl::enumClassNames(
	const OW_String& ns,
	const OW_String& className,
	OW_StringResultHandlerIFC& result,
	OW_Bool deep, const OW_UserInfo& aclInfo)
{
	m_pServer->enumClassNames(ns, className, result, deep, aclInfo);
}

void OW_IndicationRepLayerImpl::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result,
	OW_Bool deep, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_Bool enumSubClasses, 
	const OW_UserInfo& aclInfo)
{
	m_pServer->enumInstances(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, enumSubClasses, aclInfo);
}


void OW_IndicationRepLayerImpl::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep, const OW_UserInfo& aclInfo)
{
	return m_pServer->enumInstanceNames(ns, className, result, deep, aclInfo);
}


OW_CIMQualifierType OW_IndicationRepLayerImpl::getQualifierType(
	const OW_String& ns,
	const OW_String& qualifierName, const OW_UserInfo& aclInfo)
{
	return m_pServer->getQualifierType(ns, qualifierName, aclInfo);
}


OW_CIMClass OW_IndicationRepLayerImpl::getClass(
	const OW_String& ns,
	const OW_String& className,
	OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
	const OW_UserInfo& aclInfo)
{
	return m_pServer->getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}


#ifndef OW_DISABLE_QUALIFIER_DECLARATION
void OW_IndicationRepLayerImpl::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result, const OW_UserInfo& aclInfo)
{
	m_pServer->enumQualifierTypes(ns, result, aclInfo);
}

void OW_IndicationRepLayerImpl::deleteQualifierType(const OW_String& ns, const OW_String& qualName,
	const OW_UserInfo& aclInfo)
{
	m_pServer->deleteQualifierType(ns, qualName, aclInfo);
}

void OW_IndicationRepLayerImpl::setQualifierType(const OW_String& ns,
	const OW_CIMQualifierType& qt, const OW_UserInfo& aclInfo)
{
	m_pServer->setQualifierType(ns, qt, aclInfo);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION



#ifndef OW_DISABLE_INSTANCE_MANIPULATION
void OW_IndicationRepLayerImpl::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath &name,
	const OW_String &propertyName, const OW_CIMValue &cv,
	const OW_UserInfo& aclInfo)
{
	m_pServer->setProperty(ns, name, propertyName, cv, aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

OW_CIMValue OW_IndicationRepLayerImpl::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath &name,
	const OW_String &propertyName, const OW_UserInfo& aclInfo)
{
	return m_pServer->getProperty(ns, name, propertyName, aclInfo);
}


#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void OW_IndicationRepLayerImpl::associators(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &assocClass, const OW_String &resultClass,
	const OW_String &role, const OW_String &resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	m_pServer->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}


void OW_IndicationRepLayerImpl::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String &assocClass, const OW_String &resultClass,
	const OW_String &role, const OW_String &resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	m_pServer->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}


void OW_IndicationRepLayerImpl::references(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &resultClass, const OW_String &role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	m_pServer->references(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}


void OW_IndicationRepLayerImpl::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String &resultClass, const OW_String &role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	m_pServer->referencesClasses(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}


void OW_IndicationRepLayerImpl::associatorNames(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String &assocClass,
	const OW_String &resultClass, const OW_String &role,
	const OW_String &resultRole, const OW_UserInfo& aclInfo)
{
	m_pServer->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, aclInfo);
}


void OW_IndicationRepLayerImpl::referenceNames(
	const OW_String& ns,
	const OW_CIMObjectPath &path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String &resultClass,
	const OW_String &role, const OW_UserInfo& aclInfo)
{
	m_pServer->referenceNames(ns, path, result, resultClass, role, aclInfo);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL


void OW_IndicationRepLayerImpl::execQuery(
	const OW_String& ns,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &query, const OW_String& queryLanguage,
	const OW_UserInfo& aclInfo)
{
	m_pServer->execQuery(ns, result, query, queryLanguage, aclInfo);
}


#ifndef OW_DISABLE_INSTANCE_MANIPULATION
void OW_IndicationRepLayerImpl::deleteNameSpace(const OW_String &ns, const OW_UserInfo& aclInfo)
{
	m_pServer->deleteNameSpace(ns, aclInfo);
}


void OW_IndicationRepLayerImpl::createNameSpace(const OW_String& ns, const OW_UserInfo& aclInfo)
{
	m_pServer->createNameSpace(ns, aclInfo);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


void OW_IndicationRepLayerImpl::enumNameSpace(OW_StringResultHandlerIFC& result,
	const OW_UserInfo& aclInfo)
{
	m_pServer->enumNameSpace(result, aclInfo);
}


void OW_IndicationRepLayerImpl::getSchemaReadLock()
{
	m_pServer->getSchemaReadLock();
}

void OW_IndicationRepLayerImpl::getSchemaWriteLock()
{
	m_pServer->getSchemaWriteLock();
}

void OW_IndicationRepLayerImpl::releaseSchemaReadLock()
{
	m_pServer->releaseSchemaReadLock();
}

void OW_IndicationRepLayerImpl::releaseSchemaWriteLock()
{
	m_pServer->releaseSchemaWriteLock();
}

void OW_IndicationRepLayerImpl::getInstanceReadLock()
{
	m_pServer->getInstanceReadLock();
}

void OW_IndicationRepLayerImpl::getInstanceWriteLock()
{
	m_pServer->getInstanceWriteLock();
}

void OW_IndicationRepLayerImpl::releaseInstanceReadLock()
{
	m_pServer->releaseInstanceReadLock();
}

void OW_IndicationRepLayerImpl::releaseInstanceWriteLock()
{
	m_pServer->releaseInstanceWriteLock();
}

void OW_IndicationRepLayerImpl::setCIMServer(const OW_RepositoryIFCRef& src)
{
	m_pServer = src;
	m_pEnv = m_pServer->getEnvironment().cast_to<OW_CIMOMEnvironment>();
}


void OW_IndicationRepLayerImpl::exportIndication(const OW_CIMInstance& instance,
	const OW_String& instNS)
{
	m_pEnv->exportIndication(instance, instNS);
}





//////////////////////////////////////////////////////////////////////////////
extern "C" OW_IndicationRepLayer*
createIndicationRepLayer()
{
	return new OW_IndicationRepLayerImpl;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" const char*
getOWVersion()
{
	return OW_VERSION;
}

