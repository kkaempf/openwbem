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
 * @author Dan Nuffer
 */

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

namespace OpenWBEM
{

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
IndicationRepLayer::~IndicationRepLayer()
{
}
//////////////////////////////////////////////////////////////////////////////
IndicationRepLayerImpl::~IndicationRepLayerImpl()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
IndicationRepLayerImpl::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	CIMInstance theInst = m_pServer->getInstance(ns, instanceName, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, context);
	
	if (m_pEnv->getIndicationRepLayerMediator()->getInstReadSubscriptionCount() > 0)
	{
		try
		{
			CIMInstance expInst("CIM_InstRead");
			expInst.setProperty("SourceInstance", CIMValue(theInst));
			exportIndication(expInst, ns);
		}
		catch (CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for getInstance"
				" because CIM_InstRead does not exist");
		}
	}
	return theInst;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
IndicationRepLayerImpl::invokeMethod(
	const String& ns,
	const CIMObjectPath& path,
	const String& methodName, const CIMParamValueArray& inParams,
	CIMParamValueArray& outParams, OperationContext& context)
{
	CIMValue rval = m_pServer->invokeMethod(ns, path, methodName, inParams,
		outParams, context);
	if (m_pEnv->getIndicationRepLayerMediator()->getInstMethodCallSubscriptionCount() > 0)
	{
		if (path.isInstancePath()) // process the indication only if instance.
		{
			try
			{
				CIMInstance expInst("CIM_InstMethodCall");
				CIMInstance theInst = m_pServer->getInstance(ns, path, E_NOT_LOCAL_ONLY,
					E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL, context);
	
				if (!theInst)
				{
					// can't export indication
					return rval;
				}
	
				CIMInstance ParamsEmbed;
				ParamsEmbed.setClassName("__MethodParameters");
	
				for(size_t i = 0; i < inParams.size(); i++)
				{
					CIMProperty prop(inParams[i].getName(), inParams[i].getValue());
					ParamsEmbed.setProperty(prop);
				}
	
				for(size_t i = 0; i < outParams.size(); i++)
				{
					CIMProperty prop(outParams[i].getName(), outParams[i].getValue());
					ParamsEmbed.setProperty(prop);
				}
	
	
				expInst.setProperty("SourceInstance", CIMValue(theInst)); // from CIM_InstIndication
				expInst.setProperty("MethodName", CIMValue(methodName));
				expInst.setProperty("MethodParameters", CIMValue(ParamsEmbed));
				expInst.setProperty("PreCall", CIMValue(false));
				expInst.setProperty("ReturnValue", CIMValue(rval.toString()));
				exportIndication(expInst, ns);
			}
			catch (CIMException&)
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
CIMClass
IndicationRepLayerImpl::modifyClass(const String &ns,
	const CIMClass& cc, OperationContext& context)
{
	CIMClass CCOrig = m_pServer->modifyClass(ns, cc, context);
	
	if (m_pEnv->getIndicationRepLayerMediator()->getClassModificationSubscriptionCount() > 0)
	{
	
		try
		{
			CIMInstance expInst("CIM_ClassModification");
			expInst.setProperty("PreviousClassDefinition", CIMValue(CCOrig));
			expInst.setProperty("ClassDefinition", CIMValue(cc));
			exportIndication(expInst, ns);
		}
		catch (CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for modifyClass"
				" because CIM_ClassModification does not exist");
		}
	}
	return CCOrig;
}
//////////////////////////////////////////////////////////////////////////////
void
IndicationRepLayerImpl::createClass(const String& ns,
	const CIMClass& cc, OperationContext& context)
{
	m_pServer->createClass(ns, cc, context);
	if (m_pEnv->getIndicationRepLayerMediator()->getClassCreationSubscriptionCount() > 0)
	{
	
		try
		{
			CIMInstance expInst("CIM_ClassCreation");
			expInst.setProperty("ClassDefinition", CIMValue(cc));
			exportIndication(expInst, ns);
		}
		catch(CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for createClass"
				" because CIM_ClassCreation does not exist");
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
IndicationRepLayerImpl::deleteClass(const String& ns, const String& className,
	OperationContext& context)
{
	CIMClass cc = m_pServer->deleteClass(ns, className, context);
	if (m_pEnv->getIndicationRepLayerMediator()->getClassDeletionSubscriptionCount() > 0)
	{
		try
		{
			CIMInstance expInst("CIM_ClassDeletion");
			expInst.setProperty("ClassDefinition", CIMValue(cc));
			exportIndication(expInst, ns);
		}
		catch (CIMException&)
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
CIMInstance
IndicationRepLayerImpl::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	OperationContext& context)
{
	CIMInstance ciOrig = m_pServer->modifyInstance(ns, modifiedInstance,
		includeQualifiers, propertyList, context);
	if (m_pEnv->getIndicationRepLayerMediator()->getInstModificationSubscriptionCount() > 0)
	{
		try
		{
			CIMInstance expInst("CIM_InstModification");
			expInst.setProperty("PreviousInstance", CIMValue(ciOrig));
			// Filtering the properties in ss is done per the CIM_Interop
			// schema MOF by the indication server, we don't need to do it here.
			expInst.setProperty("SourceInstance", CIMValue(modifiedInstance));
			exportIndication(expInst, ns);
		}
		catch (CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for modifyInstance"
				" because CIM_InstModification does not exist");
		}
	}
	return ciOrig;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
IndicationRepLayerImpl::createInstance(const String& ns,
	const CIMInstance& ci, OperationContext& context)
{
	CIMObjectPath rval = m_pServer->createInstance(ns, ci, context);
	if (m_pEnv->getIndicationRepLayerMediator()->getInstCreationSubscriptionCount() > 0)
	{
		try
		{
			CIMInstance expInst("CIM_InstCreation");
			expInst.setProperty("SourceInstance", CIMValue(ci));
			exportIndication(expInst, ns);
		}
		catch(CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for createInstance"
				" because CIM_InstCreation does not exist");
		}
	}
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
IndicationRepLayerImpl::deleteInstance(const String& ns, const CIMObjectPath& path,
	OperationContext& context)
{
	CIMInstance instOrig = m_pServer->deleteInstance(ns, path, context);
	if (m_pEnv->getIndicationRepLayerMediator()->getInstDeletionSubscriptionCount() > 0)
	{
		try
		{
			CIMInstance expInst("CIM_InstDeletion");
			expInst.setProperty("SourceInstance", CIMValue(instOrig));
			exportIndication(expInst, ns);
		}
		catch (CIMException&)
		{
			m_pEnv->logDebug("Unable to export indication for deleteInstance"
				" because CIM_InstDeletion does not exist");
		}
	}
	return instOrig;
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
IndicationRepLayerImpl::IndicationRepLayerImpl() : IndicationRepLayer(), m_pServer(0) {}
void IndicationRepLayerImpl::open(const String&) {}
void IndicationRepLayerImpl::close() {}
ServiceEnvironmentIFCRef IndicationRepLayerImpl::getEnvironment() const
{
	return m_pEnv;
}
void IndicationRepLayerImpl::enumClasses(const String& ns,
	const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, OperationContext& context)
{
	m_pServer->enumClasses(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, context);
}
void IndicationRepLayerImpl::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, OperationContext& context)
{
	m_pServer->enumClassNames(ns, className, result, deep, context);
}
void IndicationRepLayerImpl::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	EEnumSubclassesFlag enumSubclasses,
	OperationContext& context)
{
	m_pServer->enumInstances(ns, className, result, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, enumSubclasses, context);
}
void IndicationRepLayerImpl::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep, OperationContext& context)
{
	return m_pServer->enumInstanceNames(ns, className, result, deep, context);
}
CIMQualifierType IndicationRepLayerImpl::getQualifierType(
	const String& ns,
	const String& qualifierName, OperationContext& context)
{
	return m_pServer->getQualifierType(ns, qualifierName, context);
}
CIMClass IndicationRepLayerImpl::getClass(
	const String& ns,
	const String& className,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
	OperationContext& context)
{
	return m_pServer->getClass(ns, className, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, context);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
void IndicationRepLayerImpl::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result, OperationContext& context)
{
	m_pServer->enumQualifierTypes(ns, result, context);
}
void IndicationRepLayerImpl::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext& context)
{
	m_pServer->deleteQualifierType(ns, qualName, context);
}
void IndicationRepLayerImpl::setQualifierType(const String& ns,
	const CIMQualifierType& qt, OperationContext& context)
{
	m_pServer->setQualifierType(ns, qt, context);
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
void IndicationRepLayerImpl::setProperty(
	const String& ns,
	const CIMObjectPath &name,
	const String &propertyName, const CIMValue &cv,
	OperationContext& context)
{
	m_pServer->setProperty(ns, name, propertyName, cv, context);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
CIMValue IndicationRepLayerImpl::getProperty(
	const String& ns,
	const CIMObjectPath &name,
	const String &propertyName, OperationContext& context)
{
	return m_pServer->getProperty(ns, name, propertyName, context);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void IndicationRepLayerImpl::associators(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	m_pServer->associators(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList, context);
}
void IndicationRepLayerImpl::associatorsClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &assocClass, const String &resultClass,
	const String &role, const String &resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	m_pServer->associatorsClasses(ns, path, result, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList, context);
}
void IndicationRepLayerImpl::references(
	const String& ns,
	const CIMObjectPath &path,
	CIMInstanceResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	m_pServer->references(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, context);
}
void IndicationRepLayerImpl::referencesClasses(
	const String& ns,
	const CIMObjectPath &path,
	CIMClassResultHandlerIFC& result,
	const String &resultClass, const String &role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	m_pServer->referencesClasses(ns, path, result, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, context);
}
void IndicationRepLayerImpl::associatorNames(
	const String& ns,
	const CIMObjectPath &path,
	CIMObjectPathResultHandlerIFC& result,
	const String &assocClass,
	const String &resultClass, const String &role,
	const String &resultRole, OperationContext& context)
{
	m_pServer->associatorNames(ns, path, result, assocClass, resultClass, role,
		resultRole, context);
}
void IndicationRepLayerImpl::referenceNames(
	const String& ns,
	const CIMObjectPath &path,
	CIMObjectPathResultHandlerIFC& result,
	const String &resultClass,
	const String &role, OperationContext& context)
{
	m_pServer->referenceNames(ns, path, result, resultClass, role, context);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void IndicationRepLayerImpl::execQuery(
	const String& ns,
	CIMInstanceResultHandlerIFC& result,
	const String &query, const String& queryLanguage,
	OperationContext& context)
{
	m_pServer->execQuery(ns, result, query, queryLanguage, context);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
void IndicationRepLayerImpl::deleteNameSpace(const String &ns, OperationContext& context)
{
	m_pServer->deleteNameSpace(ns, context);
}
void IndicationRepLayerImpl::createNameSpace(const String& ns, OperationContext& context)
{
	m_pServer->createNameSpace(ns, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
void IndicationRepLayerImpl::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext& context)
{
	m_pServer->enumNameSpace(result, context);
}
void IndicationRepLayerImpl::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context)
{
	m_pServer->beginOperation(op, context);
}
void IndicationRepLayerImpl::endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result)
{
	m_pServer->endOperation(op, context, result);
}
void IndicationRepLayerImpl::setCIMServer(const RepositoryIFCRef& src)
{
	m_pServer = src;
	ServiceEnvironmentIFCRef env = m_pServer->getEnvironment();
	m_pEnv = env.cast_to<CIMOMEnvironment>();
}
void IndicationRepLayerImpl::exportIndication(const CIMInstance& instance,
	const String& instNS)
{
	m_pEnv->exportIndication(instance, instNS);
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
extern "C" OpenWBEM::IndicationRepLayer*
createIndicationRepLayer()
{
	return new OpenWBEM::IndicationRepLayerImpl;
}
//////////////////////////////////////////////////////////////////////////////
#if !defined(OW_STATIC_SERVICES)
extern "C" const char*
getOWVersion()
{
	return OW_VERSION;
}
#endif /* !defined(OW_STATIC_SERVICES) */


