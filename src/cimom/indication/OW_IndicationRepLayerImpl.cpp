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


// TODO: Should we really do a getClass for all the intrinsic indication classes?
// or should we just create an instance and set the class name?
// - by using getClass we will only export the indication if the schema is loaded
//   in the operation's namespace.  Is this desireable?
											
//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_UserInfo& aclInfo)
{
	OW_CIMClass cc = m_pServer->deleteClass(ns, className, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getClassDeletionSubscriptionCount() > 0)
	{
		OW_UserInfo intAclInfo;

		try
		{
			OW_CIMClass expCC = m_pServer->getClass(ns, "CIM_ClassDeletion",
				false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
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

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& path,
	const OW_UserInfo& aclInfo)
{
	OW_CIMInstance instOrig = m_pServer->deleteInstance(ns, path, aclInfo);

	if (m_pEnv->getIndicationRepLayerMediator()->getInstDeletionSubscriptionCount() > 0)
	{
		OW_UserInfo intAclInfo;
	
		try
		{
			OW_CIMClass expCC = m_pServer->getClass(ns, "CIM_InstDeletion",
				false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
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
		OW_UserInfo intAclInfo;
	
		try
		{
			OW_CIMClass expCC = m_pServer->getClass(ns,
				"CIM_InstRead", false, true, true, NULL, intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
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
		if (path.getKeys().size() > 0) // process the indication only if instance.
		{
			OW_UserInfo intAclInfo;
			try
			{
				OW_CIMClass expCC = m_pServer->getClass(ns,
						"CIM_InstMethodCall", false, true, true, NULL,
						intAclInfo);
				OW_CIMInstance expInst = expCC.newInstance();
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

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::modifyClass(const OW_String &ns,
	const OW_CIMClass& cc, const OW_UserInfo& aclInfo)
{
	OW_CIMClass CCOrig = m_pServer->modifyClass(ns, cc, aclInfo);
	
	if (m_pEnv->getIndicationRepLayerMediator()->getClassModificationSubscriptionCount() > 0)
	{
		OW_UserInfo intAclInfo;
	
		try
		{
			OW_CIMClass expCC = m_pServer->getClass(ns,
				"CIM_ClassModification", false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
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
		OW_UserInfo intAclInfo;
	
		try
		{
			OW_CIMClass expCC = m_pServer->getClass(ns,
				"CIM_ClassCreation", false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
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
		OW_UserInfo intAclInfo;
	
		try
		{
			OW_CIMClass expCC = m_pServer->getClass(ns,
				"CIM_InstModification", false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
			expInst.setProperty("PreviousInstance", OW_CIMValue(ciOrig));
			// TODO refer to MOF.  What about filtering the properties in ss?
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
			OW_UserInfo intAclInfo;
			OW_CIMClass expCC = m_pServer->getClass(ns,
				"CIM_InstCreation", false, true, true, NULL,
				intAclInfo);
			OW_CIMInstance expInst = expCC.newInstance();
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

