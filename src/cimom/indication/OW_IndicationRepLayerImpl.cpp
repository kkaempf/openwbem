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
#include "OW_IndicationRepLayerImpl.hpp"
#include "OW_Format.hpp"
#include "OW_DateTime.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParamValue.hpp"

											
//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMClass cc = m_pServer->deleteClass(ns, className, aclInfo);

	OW_ACLInfo intAclInfo;

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
		getEnvironment()->logDebug("Unable to export indication for"
			" deleteClass because CIM_ClassDeletion does not exist");
	}
	
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& path,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMInstance instOrig = m_pServer->deleteInstance(ns, path, aclInfo);
	OW_ACLInfo intAclInfo;

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
		getEnvironment()->logDebug("Unable to export indication for deleteInstance"
			" because CIM_InstDeletion does not exist");
	}
	return instOrig;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_IndicationRepLayerImpl::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	OW_CIMInstance theInst = m_pServer->getInstance(ns, instanceName, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	
	OW_ACLInfo intAclInfo;

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
		getEnvironment()->logDebug("Unable to export indication for getInstance"
			" because CIM_InstRead does not exist");
	}

	return theInst;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_IndicationRepLayerImpl::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String& methodName, const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams, const OW_ACLInfo& aclInfo)
{
	OW_CIMValue rval = m_pServer->invokeMethod(ns, path, methodName, inParams,
		outParams, aclInfo);

	if (path.getKeys().size() > 0) // process the indication only if instance.
	{
		OW_ACLInfo intAclInfo;
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

			OW_CIMClass ParamsEmbed("__MethodParameters");

			for(size_t i = 0; i < inParams.size(); i++)
			{
				OW_CIMProperty prop(inParams[i].getName(), inParams[i].getValue());
				ParamsEmbed.addProperty(prop);
			}

			for(size_t i = 0; i < outParams.size(); i++)
			{
				OW_CIMProperty prop(outParams[i].getName(), outParams[i].getValue());
				ParamsEmbed.addProperty(prop);
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
			getEnvironment()->logDebug("Unable to export indication for"
				" invokeMethod because CIM_InstMethodCall does not exist");
		}
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_IndicationRepLayerImpl::modifyClass(const OW_String &ns,
	const OW_CIMClass& cc, const OW_ACLInfo& aclInfo)
{
	OW_CIMClass CCOrig = m_pServer->modifyClass(ns, cc, aclInfo);
	OW_ACLInfo intAclInfo;

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
		getEnvironment()->logDebug("Unable to export indication for modifyClass"
			" because CIM_ClassModification does not exist");
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
		OW_CIMClass expCC = m_pServer->getClass(name.getNameSpace(),
			"CIM_ClassCreation", false, true, true, NULL,
			intAclInfo);
		OW_CIMInstance expInst = expCC.newInstance();
		expInst.setProperty("ClassDefinition", OW_CIMValue(cc));
		exportIndication(expInst, name.getNameSpace());
	}
	catch(OW_CIMException&)
	{
		getEnvironment()->logDebug("Unable to export indication for createClass"
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
	OW_ACLInfo intAclInfo;

	try
	{
		OW_CIMClass expCC = m_pServer->getClass(name.getNameSpace(),
			"CIM_InstModification", false, true, true, NULL,
			intAclInfo);
		OW_CIMInstance expInst = expCC.newInstance();
		expInst.setProperty("PreviousInstance", OW_CIMValue(ciOrig));
		// TODO refer to MOF.  What about filtering the properties in ss?
		expInst.setProperty("SourceInstance", OW_CIMValue(lci));
		exportIndication(expInst, name.getNameSpace());
	}
	catch (OW_CIMException&)
	{
		getEnvironment()->logDebug("Unable to export indication for modifyInstance"
			" because CIM_InstModification does not exist");
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

	OW_CIMObjectPath op(name);
	op.setKeys(ci.getKeyValuePairs());

	OW_ACLInfo intAclInfo;

	try
	{
		OW_CIMClass expCC = m_pServer->getClass(name.getNameSpace(),
			"CIM_InstCreation", false, true, true, NULL,
			intAclInfo);
		OW_CIMInstance expInst = expCC.newInstance();
		// TODO refer to MOF.  What about filtering the properties in ss?
		expInst.setProperty("SourceInstance", OW_CIMValue(lci));
		exportIndication(expInst, name.getNameSpace());
	}
	catch(OW_CIMException&)
	{
		getEnvironment()->logDebug("Unable to export indication for createInstance"
			" because CIM_InstCreation does not exist");
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

