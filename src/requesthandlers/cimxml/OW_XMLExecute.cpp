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
#include "OW_XMLExecute.hpp"
#include "OW_Format.hpp"
#include "OW_XMLClass.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_XMLException.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_CIMtoXML.hpp"

using std::ostream;


#define OW_LOGDEBUG(msg) this->getEnvironment()->getLogger()->logDebug(msg)
#define OW_LOGCUSTINFO(msg) this->getEnvironment()->getLogger()->logCustInfo(msg)
#define OW_LOGERROR(msg) this->getEnvironment()->getLogger()->logError(msg)

typedef void (OW_XMLExecute::*execFuncPtr_t)(ostream& ostr,
	OW_XMLNode& qualNode, OW_CIMObjectPath& path, OW_CIMOMHandle& hdl);

typedef  OW_Map<OW_String, execFuncPtr_t> funcMap_t;

static funcMap_t g_funcMap;

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::init()
{
	g_funcMap.clear();
	g_funcMap["associatornames"] = &OW_XMLExecute::associatorNames;
	g_funcMap["associators"] = &OW_XMLExecute::associators;
	g_funcMap["createclass"] = &OW_XMLExecute::createClass;
	g_funcMap["createinstance"] = &OW_XMLExecute::createInstance;
	g_funcMap["deleteclass"] = &OW_XMLExecute::deleteClass;
	g_funcMap["deleteinstance"] = &OW_XMLExecute::deleteInstance;
	g_funcMap["deletequalifier"] = &OW_XMLExecute::deleteQualifier;
	g_funcMap["enumerateclasses"] = &OW_XMLExecute::enumerateClasses;
	g_funcMap["enumerateinstances"] = &OW_XMLExecute::enumerateInstances;
	g_funcMap["enumerateclassnames"] = &OW_XMLExecute::enumerateClassNames;
	g_funcMap["enumerateinstancenames"] = &OW_XMLExecute::enumerateInstanceNames;
	g_funcMap["enumeratequalifiers"] = &OW_XMLExecute::enumerateQualifiers;
	g_funcMap["getclass"] = &OW_XMLExecute::getClass;
	g_funcMap["getinstance"] = &OW_XMLExecute::getInstance;
	g_funcMap["getproperty"] = &OW_XMLExecute::getProperty;
	g_funcMap["getqualifier"] = &OW_XMLExecute::getQualifier;
	g_funcMap["modifyclass"] = &OW_XMLExecute::modifyClass;
	g_funcMap["modifyinstance"] = &OW_XMLExecute::modifyInstance;
	g_funcMap["referencenames"] = &OW_XMLExecute::referenceNames;
	g_funcMap["references"] = &OW_XMLExecute::references;
	g_funcMap["setqualifier"] = &OW_XMLExecute::setQualifier;
	g_funcMap["setproperty"] = &OW_XMLExecute::setProperty;
	g_funcMap["execquery"] = &OW_XMLExecute::execQuery;
}

class OW_XMLFuncLoader
{
public:
	OW_XMLFuncLoader()
	{
		OW_XMLExecute::init();
	}
};

OW_XMLFuncLoader g_funcLoader;


//////////////////////////////////////////////////////////////////////////////
OW_XMLExecute::OW_XMLExecute()
	: OW_RequestHandlerIFCXML(),
	m_ostrEntity(NULL),
	m_ostrError(NULL),
	m_hasError(false),
	m_isIntrinsic(false),
	m_functionName()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_String OW_XMLExecute::doGetId() const
{
	return OW_CIMXML_ID;
}

//////////////////////////////////////////////////////////////////////////////
// Private
int
OW_XMLExecute::executeXML(OW_XMLNode& node, ostream* ostrEntity,
	ostream* ostrError, const OW_String& userName)
{
	m_hasError = false;
	m_ostrEntity = ostrEntity;
	m_ostrError = ostrError;
	m_isIntrinsic = false;
	try
	{
		OW_String messageId = node.mustGetAttribute(OW_XMLOperationGeneric::MSG_ID);

		node = node.getChild();

		if (!node)
		{
			OW_THROW(OW_CIMErrorException,
				"No <SIMPLEREQ> or <MULTIREQ> tag");
		}

		makeXMLHeader(messageId, *m_ostrEntity);

		if (node.getToken() == OW_XMLNode::XML_ELEMENT_MULTIREQ)
		{
			(*m_ostrEntity) << "<MULTIRSP>\r\n";
			node = node.getChild();

			while (node)
			{
				if (node.getToken() != OW_XMLNode::XML_ELEMENT_SIMPLEREQ)
				{
					OW_THROW(OW_XMLException, format("Expected <SIMPLEREQ>, "
						"got %1", node.getNodeName()).c_str());
				}
				OW_TempFileStream ostrEnt, ostrErr(500);
				processSimpleReq(node, ostrEnt, ostrErr, userName);
				if (m_hasError)
				{
					(*m_ostrEntity) << ostrErr.rdbuf();
					m_hasError = false;
				}
				else
				{
					(*m_ostrEntity) << ostrEnt.rdbuf();
				}
				node = node.getNext();
			} // while

			(*m_ostrEntity) << "</MULTIRSP>\r\n";
		} // if MULTIRSP
		else if (node.getToken() == OW_XMLNode::XML_ELEMENT_SIMPLEREQ)
		{
			makeXMLHeader(messageId, *m_ostrError);
			processSimpleReq(node, *m_ostrEntity, *m_ostrError, userName);
			if (m_hasError)
			{
				(*m_ostrError) << "</MESSAGE></CIM>\r\n";
			}
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No <SIMPLEREQ> or <MULTIREQ> tag");
		}

		(*m_ostrEntity) << "</MESSAGE></CIM>\r\n";

	}
	catch (OW_CIMException& e)
	{
		// TODO
	}

	return 0; // TODO should we keep previous value instead?

}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::executeIntrinsic(ostream& ostr,
	OW_XMLNode node, OW_CIMOMHandle& hdl,
	OW_CIMObjectPath& path)
{

	OW_String functionNameLC = m_functionName;
	functionNameLC.toLowerCase();

	funcMap_t::const_iterator i = g_funcMap.find(functionNameLC);

	OW_LOGDEBUG(format("Got function name. calling function %1",
		functionNameLC));

	if (i == g_funcMap.end())
	{
		OW_THROW (OW_CIMException, "CIMException.CIM_ERR_NOT_FOUND");
	}
	else
	{
		ostr << "<IMETHODRESPONSE NAME=\"" << m_functionName <<
			"\"><IRETURNVALUE>\r\n";

		// call the member function that was found
		(this->*((*i).second))(ostr, node, path, hdl);

		ostr << "</IRETURNVALUE></IMETHODRESPONSE>\r\n";
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::executeExtrinsic(ostream& ostr, OW_XMLNode node,
	OW_CIMOMHandle& lch)
// throws OW_IOException					
{
	ostr << "<METHODRESPONSE NAME=\"" << m_functionName <<
		"\"><RETURNVALUE>";

	doInvokeMethod(ostr, node, m_functionName, lch);

	ostr << "</RETURNVALUE></METHODRESPONSE>\r\n";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doInvokeMethod(ostream& ostr, OW_XMLNode& node,
	const OW_String& methodName, OW_CIMOMHandle& hdl)
{
	OW_CIMValueArray inParams;
	OW_CIMValueArray outParams;

	OW_CIMObjectPath instancePath = OW_XMLCIMFactory::createObjectPath(node);
	OW_CIMClass cc = hdl.getClass(instancePath, false);

	if(!cc)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,"Class was null");
	}

	OW_CIMMethod method = cc.getMethod(methodName);

	if(!method)
	{
		OW_THROWCIMMSG(OW_CIMException::METHOD_NOT_FOUND,
			format("Looking for method %1", methodName).c_str() );
	}

	OW_CIMParameterArray parameters = method.getParameters();

	node = getParameters(node, parameters, inParams);

	OW_CIMValue cv = hdl.invokeMethod(instancePath, methodName, inParams,
		outParams);

	if(cv)
	{
		OW_CIMtoXML(cv, ostr);
		//cv.toXML(ostr);
	}

	if(parameters.size() == 0)
	{
		return;	// Assume no paramters
	}

	size_t outParamsIdx = 0;
	for (size_t i=0; i < parameters.size(); i++)
	{
		OW_CIMParameter cp = parameters[i];
		if (cp.getQualifier("out"))
		{
			ostr << "<PARAMVALUE NAME=\"" << cp.getName() << "\">";
			OW_CIMtoXML(outParams[outParamsIdx++], ostr);
			//outParams[outParamsIdx++].toXML(ostr);
			ostr << "</PARAMVALUE";
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
//
// Reads paramters for extrinsinc method calls
//
// Paramlist is the list of parameters (in and out) for this method, params will
// be returned with input parameters in the same order as IN's were found in the
// paramlist.
//
// paramlist is a vector of CIMParameter
// params is a vector of CIMValues
//
OW_XMLNode
OW_XMLExecute::getParameters(OW_XMLNode& node,
	const OW_Array<OW_CIMParameter>& paramlist, OW_Array<OW_CIMValue>& params)
{
	//
	// Process parameters
	//
	size_t paramIdx = 0;
	for (; node; node = node.getNext())
	{
		//
		// Only process PARAMVALUE tags
		//
		if (node.getToken() != OW_XMLNode::XML_ELEMENT_PARAMVALUE)
			continue;

		OW_String parameterName = node.getAttribute(paramName);
		if (parameterName.length() == 0)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("can't find parameterName(%1) Name", paramName).c_str());
		}

		OW_XMLNode childNode=node.getChild();
		if (!childNode)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
				"Parameter %1, has no value", paramName).c_str());
		}

		for (; paramIdx < paramlist.size(); paramIdx++)
		{
			if (paramlist[paramIdx].getName().equalsIgnoreCase(parameterName))
			{
				break;
			}
			if (paramlist[paramIdx].getQualifier("IN"))
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
					"Parameter %1 is out of order.  Next expected parameter "
					"is %2", parameterName, paramlist[paramIdx].getName()).c_str());
			}
			params.push_back(OW_CIMValue());	// insert an invalid cimValue
			paramIdx++;
		} // for

		if (!paramlist[paramIdx].getQualifier("IN"))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
				"Parameter %1 is not an \"IN\" parameter.", parameterName).c_str());
		}

		params.push_back(OW_XMLCIMFactory::createValue(childNode,
			paramlist[paramIdx].getType().toString()));
		++paramIdx;

	} // for (; node; node = node.getNext()
	for (; paramIdx < paramlist.size(); ++paramIdx)
	{
		if (paramlist[paramIdx].getQualifier("IN"))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
				"No in parameter for %1", paramlist[paramIdx].getName()).c_str());
		}
		else
		{
			params.push_back(OW_CIMValue());	// fill in remainder of inParams.
		}
	}
	return node;
}

//////////////////////////////////////////////////////////////////////////////
// Private
void
OW_XMLExecute::outputError(const OW_CIMException& ce, ostream& ostr)
{
	int errorCode;
	ostr << "<SIMPLERSP>\r\n";
	if (m_isIntrinsic)
		ostr << "<IMETHODRESPONSE NAME=\"" << m_functionName << "\">\r\n";
	else
		ostr << "<METHODRESPONSE NAME=\"" << m_functionName << "\">\r\n";

	errorCode = ce.getErrNo();

	ostr << "<ERROR Code=\"" << errorCode << "\" Description=\"" <<
		OW_XMLEscape(ce.getMessage()) <<
		"\"></ERROR>\r\n";

	if (m_isIntrinsic)
		ostr << "</IMETHODRESPONSE>\r\n";
	else
		ostr << "</METHODRESPONSE>\r\n";

	ostr << "</SIMPLERSP>\r\n";
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::associatorNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String className;
	OW_CIMObjectPathEnumeration assocNames;

	OW_String role = node.extractParameterValue(XMLP_ROLE, OW_String());
	OW_String resultRole = node.extractParameterValue(XMLP_RESULTROLE,
		OW_String());

	OW_String ns = path.getNameSpace();

	// Look for ObjectName
	OW_XMLNode tmpNode = node;
	for ( ;tmpNode; tmpNode = tmpNode.getNext() )
	{
		if ( tmpNode.getToken() != OW_XMLNode::XML_ELEMENT_IPARAMVALUE )
			continue;

		OW_String attr = tmpNode.getAttribute( paramName );
		if ( attr == "" || !attr.equalsIgnoreCase(XMLP_OBJECTNAME) )
			continue;

		tmpNode = tmpNode.getChild();
		if ( !tmpNode )
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Needed a child for <IPARAMVALUE> when handling AssociatorNames");
		}

		path = OW_XMLCIMFactory::createObjectPath(tmpNode);
		path.setNameSpace(ns);
		if (path.getKeys().size() == 0)
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED,
					"Class paths not supported");
		}
		break;
	}

	OW_String resultClass = node.extractParameterValueAttr(
		XMLP_RESULTCLASS, OW_XMLNode::XML_ELEMENT_CLASSNAME,
		paramName);

	OW_String assocClass = node.extractParameterValueAttr(
		XMLP_ASSOCCLASS, OW_XMLNode::XML_ELEMENT_CLASSNAME,
		paramName);

	// If you wish to change the code below, you may wish to make
	// similar changes to XMLReferenceNames
	assocNames = hdl.associatorNames(path, assocClass, resultClass, role,
		resultRole);

	while(assocNames.hasMoreElements())
	{
		OW_CIMObjectPath cop = assocNames.nextElement();

		OW_CIMtoXML(cop, ostr, OW_CIMtoXMLFlags::isNotInstanceName);
		//cop.toXML(ostr);
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void OW_XMLExecute::associators(ostream& ostr,
	OW_XMLNode& node, OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	// Look for ObjectName
	OW_XMLNode tmpNode = node;
	OW_String ns = path.getNameSpace();
	for ( ;tmpNode; tmpNode = tmpNode.getNext() )
	{
		if(tmpNode.getToken() != OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
			continue;

		OW_String attr = tmpNode.getAttribute(paramName);
		if ( attr == "" || !attr.equalsIgnoreCase(XMLP_OBJECTNAME) )
			continue;

		tmpNode = tmpNode.getChild();
		if ( !tmpNode )
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Needed a child for <IPARAMVALUE> when handling AssociatorNames");
		}
		path = OW_XMLCIMFactory::createObjectPath(tmpNode);
		path.setNameSpace(ns);
		break;
	}

	OW_Bool isPropertyList;
	OW_String role = OW_String(node.extractParameterValue(XMLP_ROLE,
		OW_String("")));

	OW_String resultRole = OW_String(node.extractParameterValue(XMLP_RESULTROLE,
		OW_String("")));

	OW_StringArray propertyList = node.extractParameterStringArray(
		XMLP_PROPERTYLIST, isPropertyList);

	OW_Bool includeQualifiers = node.extractParameterValue(XMLP_QUAL,
		OW_Bool(false));

	OW_Bool includeClassOrigin  = node.extractParameterValue(XMLP_ORIGIN,
		OW_Bool(false));

	OW_String resultClass = node.extractParameterValueAttr(
		XMLP_RESULTCLASS, OW_XMLNode::XML_ELEMENT_CLASSNAME,
		paramName );

	OW_String assocClass = node.extractParameterValueAttr(
		XMLP_ASSOCCLASS, OW_XMLNode::XML_ELEMENT_CLASSNAME,
		paramName );

	OW_StringArray* pPropList = (isPropertyList) ? &propertyList : NULL;

	OW_CIMInstanceEnumeration associators = hdl.associators( path, assocClass,
		resultClass, role, resultRole, includeQualifiers, includeClassOrigin,
		pPropList);

	while (associators.hasMoreElements())
	{
		OW_CIMInstance ci = associators.nextElement();
		ostr <<  "<VALUE.OBJECTWITHPATH>\r\n";
		OW_CIMObjectPath cop( ci.getClassName(), ci.getKeyValuePairs() );
		cop.setNameSpace( path.getNameSpace() );

		OW_CIMtoXML(ci, ostr, cop, OW_CIMtoXMLFlags::notLocalOnly,
			includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
			includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
			propertyList, (isPropertyList && propertyList.size() == 0));
		//ci.toXML(ostr, cop, false, includeQualifiers, includeClassOrigin,
		//	propertyList, (isPropertyList && propertyList.size() == 0));

		ostr << "</VALUE.OBJECTWITHPATH>\r\n";
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void OW_XMLExecute::createClass(ostream& /*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	node = node.getChild();
	if (!node)
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Could not find child on <IPARAMVALUE> element");

	hdl.createClass( path, OW_XMLCIMFactory::createClass(node) );
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::createInstance(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	node = node.getChild();		// Point node to <INSTANCE> tag

	if (!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Could not find child on <IPARAMVALUE> element");
	}

	OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(node);
	OW_String className = cimInstance.getClassName();
	OW_CIMObjectPath realPath(className, path.getNameSpace());

	// Special treatment for __Namespace class
	if(className.equals(OW_CIMClass::NAMESPACECLASS))
	{
		OW_CIMProperty prop = cimInstance.getProperty(
			OW_CIMProperty::NAME_PROPERTY);

		// Need the name property since it contains the name of the new
		// name space
		if (!prop)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Name property not specified for new namespace");
		}

		// If the name property didn't come acrossed as a key, then
		// set the name property as the key
		if (!prop.isKey())
		{
			prop.addQualifier(OW_CIMQualifier::createKeyQualifier());
		}

		cimInstance.setProperty(prop);
	}

	OW_CIMPropertyArray keys = cimInstance.getKeyValuePairs();
	if(keys.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,"Instance doesn't have keys");
	}

	for (size_t i = 0; i < keys.size(); ++i)
	{
		OW_CIMProperty key = keys[i];
		if (!key.getValue())
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("Key must be provided!  Property \"%1\" does not have a "
					"valid value.", key.getName()).c_str());
		}
	}

	realPath.setKeys(keys);

	hdl.createInstance(realPath, cimInstance);
	OW_CIMObjectPath newPath(cimInstance.getClassName(),
		cimInstance.getKeyValuePairs());
	newPath.setNameSpace(path.getNameSpace());
	OW_CIMtoXML(newPath, ostr, OW_CIMtoXMLFlags::isInstanceName);
	//newPath.toXML(ostr, true);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void OW_XMLExecute::deleteClass(ostream& /*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String className = node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName );

	if ( className == "" )
		OW_THROWCIM( OW_CIMException::INVALID_CLASS );

	path.setObjectName( className );
	hdl.deleteClass( path );
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::deleteInstance(ostream&	/*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String name = node.getAttribute( paramName );
	if ( !name.equalsIgnoreCase( "InstanceName" ) )
		OW_THROWCIMMSG( OW_CIMException::INVALID_PARAMETER,
			OW_String( "Parameter name was " + name ).c_str() );

	node = node.getChild();
	if ( !node )
		OW_THROWCIMMSG( OW_CIMException::INVALID_PARAMETER,
			"No element within <IPARAMVALUE>" );

	OW_CIMObjectPath completePath = OW_XMLCIMFactory::createObjectPath(node);
	completePath.setNameSpace(path.getNameSpace());
	hdl.deleteInstance( completePath );
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::deleteQualifier(ostream& /*ostr*/, OW_XMLNode& qualNode,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String qname = getQualifierName(qualNode);
	path.setObjectName(qname);
	hdl.deleteQualifierType(path);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::enumerateClassNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String className;
	OW_Bool deepInheritance;

	className=node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);
	deepInheritance=node.extractParameterValue(XMLP_DEEP,OW_Bool(false));

	path.setObjectName(className);

	OW_CIMObjectPathEnumeration enu = hdl.enumClassNames(path, deepInheritance);

	while (enu.hasMoreElements())
	{
		//
		// Note, we assume a class name won't need escaping
		//
		ostr << "<CLASSNAME NAME=\"" << enu.nextElement().getObjectName() <<
			"\"/>\r\n";
	}
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::enumerateClasses( ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String className;
	OW_Bool localOnly;
	OW_Bool deep;
	OW_Bool includeQualifiers;
	OW_Bool includeClassOrigin;

	className = node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME,
		paramName);

	localOnly = node.extractParameterValue(XMLP_LOCAL,OW_Bool(true));
	deep = node.extractParameterValue(XMLP_DEEP, OW_Bool(false));
	includeQualifiers = node.extractParameterValue(XMLP_QUAL,OW_Bool(true));
	includeClassOrigin = node.extractParameterValue(XMLP_ORIGIN,OW_Bool(false));

	path.setObjectName(className);

	//
	// Build result
	//
	OW_CIMClassEnumeration enu = hdl.enumClass(path, deep, false);

	while (enu.hasMoreElements())
	{
		OW_CIMClass cimClass = enu.nextElement();
		OW_CIMtoXML(cimClass, ostr,
			localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
			includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
			includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
			OW_StringArray());
		//cimClass.toXML(ostr, localOnly, includeQualifiers,
		//	includeClassOrigin, OW_StringArray());
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::enumerateInstanceNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String className = node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);

	if (className.length() == 0)
	{
		OW_THROWCIM(OW_CIMException::NOT_FOUND);
	}

	path.setObjectName(className);
	//
	// Note that while the SUN API allows deep the
	// XML encodings don't so we simply assume deep
	//

	OW_CIMObjectPathEnumeration enu = hdl.enumInstanceNames(path, true);
	while (enu.hasMoreElements())
	{
		OW_CIMtoXML(enu.nextElement(), ostr, OW_CIMtoXMLFlags::isInstanceName);
		//enu.nextElement().toXML(ostr, true);
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::enumerateInstances(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_Bool isPropertyList(false);

	OW_String className = node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);

	if (className.length() == 0)
	{
		OW_THROWCIMMSG( OW_CIMException::NOT_FOUND, "Class was null" );
	}

	OW_StringArray propertyList = node.extractParameterStringArray(
		XMLP_PROPERTYLIST, isPropertyList);

	OW_Bool localOnly = node.extractParameterValue(XMLP_LOCAL, OW_Bool(true));

	OW_Bool deep = node.extractParameterValue(XMLP_DEEP, OW_Bool(true));

	OW_Bool includeQualifiers = node.extractParameterValue(XMLP_QUAL,
		OW_Bool(false));

	OW_Bool includeClassOrigin = node.extractParameterValue(XMLP_ORIGIN,
		OW_Bool(false));

	path.setObjectName(className);
	OW_CIMClass cimClass = hdl.getClass(path, false);

	OW_CIMInstanceEnumeration enu = hdl.enumInstances(path, deep, localOnly);

	//
	// Build result
	//
	OW_CIMClass cc(true);
	while (enu.hasMoreElements())
	{
		OW_CIMInstance cimInstance = enu.nextElement();
		OW_CIMObjectPath cop(cimInstance.getClassName(),
			cimInstance.getKeyValuePairs());

		cop.setNameSpace(path.getNameSpace());

		if(!cc.getName().equalsIgnoreCase(cimInstance.getClassName()))
		{
			cc = hdl.getClass(cop, false);
			if(!cc)
			{
				OW_THROW(OW_Exception, "Have instance for unknown class");
			}
		}

		cimInstance.syncWithClass(cc, includeQualifiers);

		OW_CIMtoXML(cimInstance, ostr, cop,
			localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
			includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
			includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
			propertyList,
			(isPropertyList && propertyList.size() == 0));
		//cimInstance.toXML(ostr, cop, localOnly, includeQualifiers,
		//	includeClassOrigin, propertyList,
		//	(isPropertyList && propertyList.size() == 0));
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::enumerateQualifiers(ostream& ostr, OW_XMLNode& /*node*/,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_CIMQualifierTypeEnumeration enu = hdl.enumQualifierTypes(path);
	while (enu.hasMoreElements())
	{
		OW_CIMQualifierType qual = enu.nextElement();
		OW_CIMtoXML(qual, ostr);
		//qual.toXML(ostr);

	}
	return;
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::getClass(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	if (!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"NULL node given to OW_XMLGetClass::execute");
	}

	OW_Bool isPropertyList;
	OW_String className;
	OW_Array<OW_String> propertyList;
	OW_Bool localOnly;
	OW_Bool includeQualifiers;
	OW_Bool includeClassOrigin;
	OW_CIMClass cimClass;

	className = node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);

	if (className.length() == 0)
	{
		OW_THROWCIM(OW_CIMException::INVALID_CLASS);
	}

	propertyList = node.extractParameterStringArray(XMLP_PROPERTYLIST,
		isPropertyList);

	localOnly = node.extractParameterValue(XMLP_LOCAL, OW_Bool(true));
	includeQualifiers = node.extractParameterValue(XMLP_QUAL, OW_Bool(true));
	includeClassOrigin = node.extractParameterValue(XMLP_ORIGIN, OW_Bool(false));

	path.setObjectName(className);
	cimClass = hdl.getClass(path, localOnly);
	if (!cimClass)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			OW_String("Path=" + path.toString()).c_str());
	}

	OW_CIMtoXML(cimClass, ostr,
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(isPropertyList && propertyList.size() == 0));
	//cimClass.toXML(ostr, localOnly, includeQualifiers,
	//	includeClassOrigin, propertyList,
	//	(isPropertyList && propertyList.size() == 0));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::getInstance(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_Bool isPropertyList;
	OW_Array<OW_String> propertyList;
	OW_Bool localOnly;
	OW_Bool includeQualifiers;
	OW_Bool includeClassOrigin;

	propertyList = node.extractParameterStringArray(XMLP_PROPERTYLIST,
		isPropertyList);
	localOnly = node.extractParameterValue(XMLP_LOCAL, OW_Bool(true));
	includeQualifiers = node.extractParameterValue(XMLP_QUAL, OW_Bool(false));
	includeClassOrigin = node.extractParameterValue(XMLP_ORIGIN, OW_Bool(false));

	for (;;)
	{
		if (!node)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Could not find a parameter named %1",
				XMLP_INSTANCENAME).c_str() );
		}

		node = node.mustFindElement(OW_XMLNode::XML_ELEMENT_IPARAMVALUE);
		OW_String name = node.getAttribute(paramName);
		if(name.equalsIgnoreCase(XMLP_INSTANCENAME))
		{
			break;
		}

		node = node.getNext();
	}

	node = node.getChild();
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Could not find an element with <IPARAMVALUE>");
	}

	OW_String ns = path.getNameSpace();
	path = OW_XMLCIMFactory::createObjectPath(node);
	path.setNameSpace(ns);

	OW_CIMInstance cimInstance = hdl.getInstance(path, localOnly);
	if(!cimInstance)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			format("Path=%1", path.toString()).c_str());
	}

	OW_CIMClass cc = hdl.getClass(path, false);
	cimInstance.syncWithClass(cc, includeQualifiers);

	OW_CIMtoXML(cimInstance, ostr, OW_CIMObjectPath(),
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(isPropertyList && propertyList.size() == 0));
	//cimInstance.toXML(ostr, OW_CIMObjectPath(), localOnly,
	//	includeQualifiers, includeClassOrigin, propertyList,
	//	(isPropertyList && propertyList.size() == 0));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::getProperty(ostream& ostr, OW_XMLNode& propNode,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String propertyName=propNode.mustExtractParameterValue(XMLP_PROPERTYNAME);

	propNode=propNode.findElementAndParameter(XMLP_INSTANCENAME);
	if (!propNode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Could not find a <IPARAMVALUE> with an InstanceName argument");
	}

	OW_String ns = path.getNameSpace();
	path = OW_XMLCIMFactory::createObjectPath(propNode);
	path.setNameSpace(ns);

	OW_CIMValue cv = hdl.getProperty(path,propertyName);

	if (cv)
		OW_CIMtoXML(cv, ostr);
		//cv.toXML(ostr);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::getQualifier(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{

	path.setObjectName(getQualifierName(node));

	OW_CIMQualifierType qual = hdl.getQualifierType(path);
	if (qual)
	{
		OW_CIMtoXML(qual, ostr);
		//qual.toXML(ostr);
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::modifyClass(ostream&	/*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String name=node.mustGetAttribute(paramName);
	if (!name.equalsIgnoreCase(XMLP_MODIFIED_CLASS))
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Parameter name was %1", name).c_str());

	node=node.getChild();
	if (!node)
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"No element within <IPARAMVALUE>");

	//
	// Process <CLASS> element
	//
	OW_CIMClass cimClass = OW_XMLCIMFactory::createClass(node);
	path.setObjectName(cimClass.getName());

	hdl.setClass(path,cimClass);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::modifyInstance(ostream&	/*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	//
	// Check parameter name is "ModifiedInstance"
	//
	OW_String name=node.getAttribute(paramName);
	if (!name.equalsIgnoreCase("modifiedinstance"))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Parameter name was %1", name).c_str() );
	}

	node=node.mustChildElementChild(OW_XMLNode::XML_ELEMENT_VALUE_NAMEDINSTANCE);
	//
	// Fetch <INSTANCENAME> element
	//
	OW_String ns = path.getNameSpace();
	path = OW_XMLCIMFactory::createObjectPath(node);
	path.setNameSpace(ns);
	node=node.getNext();

	OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(node);
	//
	// Much more validation needs to be performed!
	//
	hdl.setInstance(path,cimInstance);
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::referenceNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path,OW_CIMOMHandle& hdl)
{
	OW_String className;

	OW_String role = node.extractParameterValue(XMLP_ROLE, OW_String());
	OW_String ns = path.getNameSpace();
	//
	// Look for ObjectName
	//
	OW_XMLNode tmpNode=node;
	for (;tmpNode; tmpNode=node.getNext())
	{
		if (tmpNode.getToken()!=OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
			continue;
		OW_String attr = tmpNode.getAttribute(paramName);
		if (!attr.equalsIgnoreCase(XMLP_OBJECTNAME))
			continue;
		tmpNode=tmpNode.getChild();
		if (!tmpNode)
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Needed a child for <IPARAMVALUE> when handling "
				"AssociatorNames");

		path = OW_XMLCIMFactory::createObjectPath(tmpNode);
		path.setNameSpace(ns);
		break;
	}
	OW_String resultClass = node.extractParameterValueAttr(XMLP_RESULTCLASS,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);

	OW_CIMObjectPathEnumeration enu = hdl.referenceNames(path,resultClass,role);

	//
	// If you change the below code, you may wish to make similar
	// changes to XMLAssociatorNames...
	//
	while (enu.hasMoreElements())
	{
		OW_CIMObjectPath cop = enu.nextElement();

		OW_CIMtoXML(cop, ostr, OW_CIMtoXMLFlags::isNotInstanceName);
		//cop.toXML(ostr);
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::references(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String className;
	// OW_CIMOMHandle::Enumeration enumerate; // TODO not impl
	OW_Bool isPropertyList;
	OW_Array<OW_String> propertyList;
	OW_Bool includeQualifiers;
	OW_Bool includeClassOrigin;

	OW_String role = OW_String(node.extractParameterValue(
		OW_String(XMLP_ROLE), OW_String("")));

	propertyList = node.extractParameterStringArray(XMLP_PROPERTYLIST,
		isPropertyList);

	includeQualifiers = node.extractParameterValue(XMLP_QUAL, OW_Bool(false));
	includeClassOrigin = node.extractParameterValue(XMLP_ORIGIN, OW_Bool(false));

	OW_String ns = path.getNameSpace();

	OW_XMLNode tmpNode = node;
	for (; tmpNode; tmpNode = tmpNode.getNext())
	{
		if (tmpNode.getToken() != OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
		{
			continue;
		}
		OW_String attr = tmpNode.getAttribute(paramName);
		if (!attr.equalsIgnoreCase(XMLP_OBJECTNAME))
		{
			continue;
		}
		tmpNode = tmpNode.getChild();
		if (!tmpNode)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Needed a child for <IPARAMVALUE> when handling associatorNames");
		}
		path = OW_XMLCIMFactory::createObjectPath(tmpNode);
		path.setNameSpace(ns);
		break;
	}

	OW_String resultClass = node.extractParameterValueAttr(
		XMLP_RESULTCLASS, OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);

	OW_StringArray* pPropList = (isPropertyList) ? &propertyList : NULL;
	OW_CIMInstanceEnumeration enu = hdl.references(path, resultClass,
		role, includeQualifiers, includeClassOrigin, pPropList);

	while (enu.hasMoreElements())
	{
		OW_CIMInstance ci = enu.nextElement();
		ostr << "<VALUE.OBJECTWITHPATH>\r\n";

		OW_CIMtoXML(ci, ostr, OW_CIMObjectPath(ci.getClassName(),ci.
			getKeyValuePairs()),
			OW_CIMtoXMLFlags::notLocalOnly,
			includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
			includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
			propertyList, (isPropertyList && propertyList.size() == 0));
		//ci.toXML(ostr, OW_CIMObjectPath(ci.getClassName(),ci.
		//	getKeyValuePairs()), false, includeQualifiers, includeClassOrigin,
		//	propertyList, (isPropertyList && propertyList.size() == 0));


		ostr << "</VALUE.OBJECTWITHPATH>\r\n";
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::setProperty(ostream&	/*ostr*/, OW_XMLNode& propNode,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String instanceName;
	OW_CIMInstance cimInstance;

	OW_String propertyName = propNode.extractParameterValue(XMLP_PROPERTYNAME,
		OW_String());
	if (propertyName.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Cannot find property name");
	}

	/* IPARAMVALUE cannot have a type, so assume the default is a string
	 * OW_String valueType = propNode.mustGetAttribute(OW_XMLAttribute::TYPE);
	 */
	OW_String valueType = "string";

	OW_CIMValue propValue;
	OW_XMLNode valueNode = propNode.findElementAndParameter(XMLP_NewValue);
	if(valueNode)
	{
		propValue = OW_XMLCIMFactory::createValue(valueNode, valueType);
	}

	propNode = propNode.findElementAndParameter(XMLP_INSTANCENAME);
	if(!propNode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Instance name not specified");
	}

	OW_String ns = path.getNameSpace();
	path = OW_XMLCIMFactory::createObjectPath(propNode);
	path.setNameSpace(ns);
	hdl.setProperty(path, propertyName, propValue);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::execQuery(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String queryLanguage =
		node.extractParameterValue(XMLP_QUERYLANGUAGE, OW_String(""));
	OW_String query = node.extractParameterValue(XMLP_QUERY, OW_String(""));

	if (queryLanguage.length() == 0 || query.length() == 0)
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	OW_CIMInstanceArray cia = hdl.execQuery(path.getFullNameSpace(), query, queryLanguage);

	//
	// Build result
	//
	for (size_t i = 0; i < cia.size(); ++i)
	{
		OW_CIMInstance cimInstance = cia[i];

		OW_CIMObjectPath cop(cimInstance.getClassName(),
			cimInstance.getKeyValuePairs() );

		cop.setNameSpace(path.getNameSpace());

		OW_CIMtoXML(cimInstance, ostr, cop,
			OW_CIMtoXMLFlags::notLocalOnly,
			OW_CIMtoXMLFlags::includeQualifiers,
			OW_CIMtoXMLFlags::includeClassOrigin,
			OW_StringArray());
		//cimInstance.toXML(ostr, cop);
	}
}



//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_XMLExecute::setQualifier(ostream& /*ostr*/, OW_XMLNode& qualNode,
	OW_CIMObjectPath& path, OW_CIMOMHandle& hdl)
{
	OW_String argName = qualNode.mustGetAttribute(paramName);

	if (!argName.equalsIgnoreCase(XMLP_QUALIFIERDECL))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"invalid qualifier xml");
	}

	qualNode = qualNode.mustChildFindElement(
		OW_XMLNode::XML_ELEMENT_QUALIFIER_DECLARATION);

	if (!qualNode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Cannot find qualifier declaration argument");
	}

	OW_CIMQualifierType cimQualifier(OW_Bool(true));
	OW_XMLQualifier::processQualifierDecl(qualNode, cimQualifier);

	path.setObjectName(cimQualifier.getName());
	hdl.setQualifierType(path, cimQualifier);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::processSimpleReq(OW_XMLNode& node, ostream& ostrEntity,
	ostream& ostrError, const OW_String& userName)
{
	try
	{
		ostrEntity << "<SIMPLERSP>";
		OW_XMLNode newnode = node.getChild();
		if (!newnode)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No <METHODCALL> or <IMETHODCALL> element");
		}

		if (newnode.getToken() == OW_XMLNode::XML_ELEMENT_METHODCALL)
		{
			m_isIntrinsic = false;
		}
		else if (newnode.getToken() == OW_XMLNode::XML_ELEMENT_IMETHODCALL)
		{
			m_isIntrinsic = true;
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No <METHODCALL> or <IMETHODCALL> element");
		}

		m_functionName = newnode.mustGetAttribute("NAME");

		newnode = newnode.getChild();
		if (!newnode)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No element in <METHODCALL> or <IMETHODCALL>");
		}

		//OW_ACLInfo acl(userName);
		//OW_CIMOMHandle hdl = OW_Environment::getCIMOMHandle(acl, true);
		OW_CIMOMHandleRef hdl = this->getEnvironment()->getCIMOMHandle(userName, true);

		if (m_isIntrinsic)
		{
			OW_String nameSpace = OW_XMLClass::getNameSpace(newnode.mustElementChild(
				OW_XMLNode::XML_ELEMENT_LOCALNAMESPACEPATH));

			OW_CIMNameSpace ns(OW_Bool(true));
			ns.setNameSpace(nameSpace);
			OW_CIMObjectPath path("", nameSpace);

			newnode = newnode.getNext();
			executeIntrinsic(ostrEntity, newnode, *hdl, path);
		}
		else
		{
			executeExtrinsic(ostrEntity, newnode, *hdl);
		}
		ostrEntity << "</SIMPLERSP>\r\n";
	}
	catch (OW_CIMException& ce)
	{
		OW_LOGDEBUG(format("XMLExecute::processSimpleReq caught CIM "
			"exception:\nCode: %1\nFile: %2\n Line: %3\nMessage: %4 \n %5",
			ce.getErrNo(), ce.getFile(), ce.getLine(), ce.getMessage(),
			ce.getStackTrace()));

		m_hasError = true;
		outputError(ce, ostrError);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doLogDebug(const OW_String& message)
{
	OW_LOGDEBUG(message);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doLogCustInfo(const OW_String& message)
{
	OW_LOGCUSTINFO(message);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doLogError(const OW_String& message)
{
	OW_LOGERROR(message);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doOptions(OW_CIMFeatures& cf)
{
	//OW_ACLInfo acl("");
	//cf = OW_Environment::getCIMOMHandle(acl, false).getServerFeatures();
	cf = this->getEnvironment()->getCIMOMHandle("", false)->getServerFeatures();
}

//////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFCRef
OW_XMLExecute::clone() const
{
	OW_ASSERT(!m_ostrEntity);
	OW_ASSERT(!m_ostrError);
	return OW_RequestHandlerIFCRef(new OW_XMLExecute(*this));
}


//////////////////////////////////////////////////////////////////////////////
OW_REQUEST_HANDLER_FACTORY(OW_XMLExecute);


