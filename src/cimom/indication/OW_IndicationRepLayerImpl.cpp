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

#include "OW_config.h"
#include "OW_CIMServer.hpp"
#include "OW_Format.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_IndicationRepLayerImpl.hpp"

#include <iostream>

//////////////////////////////////////////////////////////////////////////////
OW_IndicationRepLayerImpl::OW_IndicationRepLayerImpl(OW_CIMServer* pRepos) :
	OW_IndicationRepLayer(), m_pServer(pRepos)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationRepLayerImpl::OW_IndicationRepLayerImpl(
	const OW_IndicationRepLayerImpl& arg)
	: OW_IndicationRepLayer()
	, m_pServer(arg.m_pServer)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_IndicationRepLayerImpl&
OW_IndicationRepLayerImpl::operator= (const OW_IndicationRepLayerImpl& arg)
{
	m_pServer = arg.m_pServer;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::createNameSpace(const OW_CIMNameSpace& ns,
	const OW_ACLInfo& aclInfo)
{
	m_pServer->createNameSpace(ns, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::close()
{
	// TODO: Unregister with repository?
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::deleteNameSpace(const OW_CIMNameSpace& ns,
	const OW_ACLInfo& aclInfo)
{
	m_pServer->deleteNameSpace(ns, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::deleteClass(const OW_CIMObjectPath& path,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMClass cc = m_pServer->deleteClass(path, aclInfo);

	if (cc)
	{
		OW_ACLInfo intAclInfo;

		try
		{
			OW_CIMClass expCC = m_pServer->getClass(
				OW_CIMObjectPath("CIM_ClassDeletion"), false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			expInst.setProperty("ClassDefinition", OW_CIMValue(cc));
			exportIndication(expInst, path.getFullNameSpace());
		}
		catch (OW_CIMException&)
		{
			getEnvironment()->logError("Unable to export indication for"
				" createClass because CIM_ClassDeletion does not exist");
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::deleteInstance(const OW_CIMObjectPath& path,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMInstance instOrig = m_pServer->deleteInstance(path, aclInfo);
	if (instOrig)
	{
		OW_ACLInfo intAclInfo;

		try
		{
			OW_CIMClass expCC = m_pServer->getClass(
				OW_CIMObjectPath("CIM_InstDeletion"), false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			expInst.setProperty("SourceInstance", OW_CIMValue(instOrig));
			exportIndication(expInst, path.getFullNameSpace());
		}
		catch (OW_CIMException&)
		{
			getEnvironment()->logError("Unable to export indication for createClass"
				" because CIM_InstDeletion does not exist");
		}
	}
	return instOrig;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::deleteQualifierType(const OW_CIMObjectPath& path,
	const OW_ACLInfo& aclInfo)
{
	m_pServer->deleteQualifierType(path, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_IndicationRepLayerImpl::enumNameSpace(const OW_CIMNameSpace& ns, OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->enumNameSpace(ns, deep, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_IndicationRepLayerImpl::enumClasses(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->enumClasses(path, deep, localOnly, includeQualifiers,
		includeClassOrigin, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IndicationRepLayerImpl::enumClassNames(const OW_CIMObjectPath& path,
	OW_Bool deep, const OW_ACLInfo& aclInfo)
{
	return m_pServer->enumClassNames(path, deep, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_IndicationRepLayerImpl::enumInstances(const OW_CIMObjectPath& path,
	OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->enumInstances(path, deep, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IndicationRepLayerImpl::enumInstanceNames(const OW_CIMObjectPath& path,
	OW_Bool deep, const OW_ACLInfo& aclInfo)
{
	return m_pServer->enumInstanceNames(path, deep, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_IndicationRepLayerImpl::enumQualifierTypes(const OW_CIMObjectPath& path,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->enumQualifierTypes(path, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::getClass(const OW_CIMObjectPath& path,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	return m_pServer->getClass(path, localOnly, includeQualifiers,
		includeClassOrigin, propertyList, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::getInstance(const OW_CIMObjectPath& name,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	OW_CIMInstance theInst = m_pServer->getInstance(name, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	if (theInst)
	{
		OW_ACLInfo intAclInfo;

		try
		{
			OW_CIMClass expCC = m_pServer->getClass(OW_CIMObjectPath("CIM_InstRead"),
				false, true, true, NULL, intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			expInst.setProperty("SourceInstance", OW_CIMValue(theInst));
			exportIndication(expInst, name.getFullNameSpace());
		}
		catch (OW_CIMException&)
		{
			getEnvironment()->logError("Unable to export indication for createClass"
				" because CIM_InstRead does not exist");
		}

	}
	return theInst;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_IndicationRepLayerImpl::invokeMethod(const OW_CIMObjectPath& name,
	const OW_String& methodName, const OW_CIMValueArray& inParams,
	OW_CIMValueArray& outParams, const OW_ACLInfo& aclInfo)
{
	OW_CIMValue rval = m_pServer->invokeMethod(name, methodName, inParams,
		outParams, aclInfo);

	if (name.getKeys().size() > 0) // process the indication only if instance.
	{
		OW_ACLInfo intAclInfo;
		try
		{
			OW_CIMClass expCC = m_pServer->getClass(
					OW_CIMObjectPath("CIM_InstMethodCall"), false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			OW_CIMInstance theInst = m_pServer->getInstance(name, false,
				true, true, NULL, intAclInfo);

			if (!theInst)
			{
				// can't export indication
				return rval;
			}

			OW_CIMClass inParamsEmbed("__MethodParameters");

			OW_CIMClass cc = m_pServer->getClass(name, false,
				true, true, NULL, intAclInfo);
			

			OW_CIMMethod cm = cc.getMethod(methodName);
			if (!cm)
			{
				// can't export indication
				return rval;
			}

			OW_CIMParameterArray paramList = cm.getParameters();

			size_t paramIdx = 0;
			for(size_t i = 0; i < inParams.size(); i++)
			{
				for(; paramIdx < paramList.size(); ++paramIdx)
				{
					if (paramList[paramIdx].getQualifier("IN"))
					{
						break;
					}
				}
				if(paramIdx >= paramList.size() ||
					 paramList[paramIdx].getQualifier("OUT"))
				{
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
							 "Too many parameters (%1) for method %2", inParams.size(),
							  methodName).c_str());
				}
				if(inParams[i].getType() != paramList[paramIdx].getType().getType())
				{
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
								"Parameter type mismatch for parameter number %1 of "
								"method %2", i + 1, methodName).c_str());
				}

				OW_CIMProperty prop(paramList[paramIdx].getName(), inParams[i]);
				inParamsEmbed.addProperty(prop);
			}


			expInst.setProperty("MethodName", OW_CIMValue(methodName));
			expInst.setProperty("MethodParameters", OW_CIMValue(inParamsEmbed));
			expInst.setProperty("PreCall", OW_CIMValue(OW_Bool(false)));
			expInst.setProperty("ReturnValue", OW_CIMValue(rval.toString()));
			exportIndication(expInst, name.getFullNameSpace());
		}
		catch (OW_CIMException&)
		{
			getEnvironment()->logError("Unable to export indication for"
				" createClass because CIM_InstMethodCall does not exist");
		}
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_IndicationRepLayerImpl::getQualifierType(const OW_CIMObjectPath& name,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->getQualifierType(name, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::setQualifierType(const OW_CIMObjectPath& name,
		const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo)
{
	m_pServer->setQualifierType(name, qt, aclInfo);
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::modifyClass(const OW_CIMObjectPath& name,
	OW_CIMClass& cc, const OW_ACLInfo& aclInfo)
{
	OW_CIMClass lcc(cc);
	OW_CIMClass CCOrig = m_pServer->modifyClass(name, lcc, aclInfo);
	if (CCOrig)
	{
		OW_ACLInfo intAclInfo;

		try
		{
			OW_CIMClass expCC = m_pServer->getClass(
				OW_CIMObjectPath("CIM_ClassModification"), false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			expInst.setProperty("PreviousClassDefinition", OW_CIMValue(CCOrig));
			expInst.setProperty("ClassDefinition", OW_CIMValue(lcc));
			exportIndication(expInst, name.getFullNameSpace());
		}
		catch (OW_CIMException&)
		{
			getEnvironment()->logError("Unable to export indication for setClass"
				" because CIM_ClassModification does not exist");
		}
	}
	return CCOrig;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::createClass(const OW_CIMObjectPath& name,
	OW_CIMClass& cc, const OW_ACLInfo& aclInfo)
{
	OW_CIMClass lcc(cc);
	m_pServer->createClass(name, lcc, aclInfo);
	OW_ACLInfo intAclInfo;

	try
	{
		OW_CIMClass expCC = m_pServer->getClass(
			  OW_CIMObjectPath("CIM_ClassCreation"), false, true, true, NULL,
			intAclInfo);
		OW_CIMInstance expInst = expCC.newInstance();
		expInst.setProperty("ClassDefinition", OW_CIMValue(cc));
		exportIndication(expInst, name.getFullNameSpace());
	}
	catch(OW_CIMException&)
	{
		getEnvironment()->logError("Unable to export indication for createClass"
			" because CIM_ClassCreation does not exist");
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::modifyInstance(const OW_CIMObjectPath& name,
		OW_CIMInstance& ci, const OW_ACLInfo& aclInfo)
{
	OW_CIMInstance lci(ci);
	OW_CIMInstance ciOrig = m_pServer->modifyInstance(name, lci, aclInfo);
	if (ciOrig)
	{
		OW_ACLInfo intAclInfo;

		try
		{
			OW_CIMClass expCC = m_pServer->getClass(
				OW_CIMObjectPath("CIM_InstModification"), false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			expInst.setProperty("PreviousInstance", OW_CIMValue(ciOrig));
			// TODO refer to MOF.  What about filtering the properties in ss?
			expInst.setProperty("SourceInstance", OW_CIMValue(lci));
			exportIndication(expInst, name.getFullNameSpace());
		}
		catch (OW_CIMException&)
		{
			getEnvironment()->logError("Unable to export indication for createClass"
				" because CIM_InstModification does not exist");
		}
	}
	return ciOrig;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_IndicationRepLayerImpl::createInstance(const OW_CIMObjectPath& name,
	OW_CIMInstance& ci, const OW_ACLInfo& aclInfo)
{
	OW_CIMInstance lci(ci);
	OW_CIMObjectPath rval = m_pServer->createInstance(name, lci, aclInfo);

	if (rval)
	{
		OW_CIMObjectPath op(name);
		op.setKeys(ci.getKeyValuePairs());

		OW_ACLInfo intAclInfo;

		try
		{
			OW_CIMClass expCC = m_pServer->getClass(
				OW_CIMObjectPath("CIM_InstCreation"), false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			// TODO refer to MOF.  What about filtering the properties in ss?
			expInst.setProperty("SourceInstance", OW_CIMValue(lci));
			exportIndication(expInst, name.getFullNameSpace());
		}
		catch(OW_CIMException&)
		{
			getEnvironment()->logError("Unable to export indication for createClass"
				" because CIM_InstCreation does not exist");
		}
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_IndicationRepLayerImpl::getProperty(const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_ACLInfo& aclInfo)
{
	return m_pServer->getProperty(name, propertyName, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::setProperty(const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& cv,
	const OW_ACLInfo& aclInfo)
{
	m_pServer->setProperty(name, propertyName, cv, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IndicationRepLayerImpl::associatorNames(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->associatorNames(path, assocClass, resultClass, role,
		resultRole, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_IndicationRepLayerImpl::associators(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	return m_pServer->associators(path, assocClass, resultClass, role,
		resultRole, includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_IndicationRepLayerImpl::referenceNames(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->referenceNames(path, resultClass, role, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_IndicationRepLayerImpl::references(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	return m_pServer->references(path, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_IndicationRepLayerImpl::execQuery(const OW_CIMNameSpace& ns,
	const OW_String& query, const OW_String& queryLanguage,
	const OW_ACLInfo& aclInfo)
{
	return m_pServer->execQuery(ns, query, queryLanguage, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::exportIndication(const OW_CIMInstance& instance,
	const OW_CIMNameSpace& instNS)
{
	getEnvironment()->exportIndication(instance, instNS);
}

//////////////////////////////////////////////////////////////////////////////
OW_ReadLock
OW_IndicationRepLayerImpl::getReadLock()
{
	return m_pServer->getReadLock();
}

//////////////////////////////////////////////////////////////////////////////
OW_WriteLock
OW_IndicationRepLayerImpl::getWriteLock()
{
	return m_pServer->getWriteLock();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_IndicationRepLayerImpl::setCIMServer(OW_RepositoryIFC *src)
{
	m_pServer = src;
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

