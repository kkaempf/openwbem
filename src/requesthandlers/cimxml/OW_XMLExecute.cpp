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

#include <algorithm>

using std::ostream;


#define OW_LOGDEBUG(msg) this->getEnvironment()->getLogger()->logDebug(msg)
#define OW_LOGCUSTINFO(msg) this->getEnvironment()->getLogger()->logCustInfo(msg)
#define OW_LOGERROR(msg) this->getEnvironment()->getLogger()->logError(msg)

OW_XMLExecute::FuncEntry OW_XMLExecute::g_funcs[24] =
{
	{ "associatornames", &OW_XMLExecute::associatorNames },
	{ "associators", &OW_XMLExecute::associators },
	{ "createclass", &OW_XMLExecute::createClass },
	{ "createinstance", &OW_XMLExecute::createInstance },
	{ "deleteclass", &OW_XMLExecute::deleteClass },
	{ "deleteinstance", &OW_XMLExecute::deleteInstance },
	{ "deletequalifier", &OW_XMLExecute::deleteQualifier },
	{ "enumerateclasses", &OW_XMLExecute::enumerateClasses },
	{ "enumerateclassnames", &OW_XMLExecute::enumerateClassNames },
	{ "enumerateinstancenames", &OW_XMLExecute::enumerateInstanceNames },
	{ "enumerateinstances", &OW_XMLExecute::enumerateInstances },
	{ "enumeratequalifiers", &OW_XMLExecute::enumerateQualifiers },
	{ "execquery", &OW_XMLExecute::execQuery },
	{ "getclass", &OW_XMLExecute::getClass },
	{ "getinstance", &OW_XMLExecute::getInstance },
	{ "getproperty", &OW_XMLExecute::getProperty },
	{ "getqualifier", &OW_XMLExecute::getQualifier },
	{ "modifyclass", &OW_XMLExecute::modifyClass },
	{ "modifyinstance", &OW_XMLExecute::modifyInstance },
	{ "referencenames", &OW_XMLExecute::referenceNames },
	{ "references", &OW_XMLExecute::references },
	{ "setproperty", &OW_XMLExecute::setProperty },
	{ "setqualifier", &OW_XMLExecute::setQualifier },
	{ "garbage", 0 }
};

OW_XMLExecute::FuncEntry* OW_XMLExecute::g_funcsEnd = &OW_XMLExecute::g_funcs[23];

//////////////////////////////////////////////////////////////////////////////
bool
OW_XMLExecute::funcEntryCompare(const OW_XMLExecute::FuncEntry& f1,
	const OW_XMLExecute::FuncEntry& f2)
{
	return (strcmp(f1.name, f2.name) < 0);
}

/*
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
*/


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
// Private
int
OW_XMLExecute::executeXML(OW_XMLNode& node, ostream* ostrEntity,
	ostream* ostrError, const OW_String& userName)
{
	m_hasError = false;
	m_ostrEntity = ostrEntity;
	m_ostrError = ostrError;
	m_isIntrinsic = false;
	OW_String messageId = node.mustGetAttribute(OW_XMLOperationGeneric::MSG_ID);

	node = node.getChild();

	if (!node)
	{
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
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


	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::executeIntrinsic(ostream& ostr,
	OW_XMLNode node, OW_CIMOMHandleIFC& hdl,
	OW_CIMObjectPath& path)
{

	OW_String functionNameLC = m_functionName;
	functionNameLC.toLowerCase();

	OW_LOGDEBUG(format("Got function name. calling function %1",
		functionNameLC));

	FuncEntry fe = { 0, 0 };
	fe.name = functionNameLC.c_str();
	FuncEntry* i = std::lower_bound(g_funcs, g_funcsEnd, fe, funcEntryCompare);

	if(i == g_funcsEnd)
	{
		// they sent over an intrinsic method call we don't know about
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
	}
	else
	{
		ostr << "<IMETHODRESPONSE NAME=\"" << m_functionName <<
			"\"><IRETURNVALUE>\r\n";

		// call the member function that was found
		(this->*((*i).func))(ostr, node, path, hdl);
		ostr << "</IRETURNVALUE></IMETHODRESPONSE>\r\n";
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::executeExtrinsic(ostream& ostr, OW_XMLNode node,
	OW_CIMOMHandleIFC& lch)
{
	ostr << "<METHODRESPONSE NAME=\"" << m_functionName <<
		"\"><RETURNVALUE>";

	doInvokeMethod(ostr, node, m_functionName, lch);

	ostr << "</RETURNVALUE></METHODRESPONSE>\r\n";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::doInvokeMethod(ostream& ostr, OW_XMLNode& node,
	const OW_String& methodName, OW_CIMOMHandleIFC& hdl)
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

	ostr << "<ERROR CODE=\"" << errorCode << "\" DESCRIPTION=\"" <<
		OW_XMLEscape(ce.getMessage()) <<
		"\"></ERROR>\r\n";

	if (m_isIntrinsic)
		ostr << "</IMETHODRESPONSE>\r\n";
	else
		ostr << "</METHODRESPONSE>\r\n";

	ostr << "</SIMPLERSP>\r\n";
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::associatorNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
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
	}
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::associators(ostream& ostr,
	OW_XMLNode& node, OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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

		ostr << "</VALUE.OBJECTWITHPATH>\r\n";
	}
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::createClass(ostream& /*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	node = node.getChild();
	if (!node)
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Could not find child on <IPARAMVALUE> element");

	hdl.createClass( path, OW_XMLCIMFactory::createClass(node) );
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::createInstance(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	node = node.getChild();		// Point node to <INSTANCE> tag

	if (!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Could not find child on <IPARAMVALUE> element");
	}

	OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(node);
	OW_String className = cimInstance.getClassName();
	OW_CIMObjectPath realPath = OW_CIMObjectPath(className, path.getNameSpace());

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

	OW_CIMObjectPath newPath = hdl.createInstance(realPath, cimInstance);
	// do we still need to do this below?
	newPath.setNameSpace(path.getNameSpace());
	OW_CIMtoXML(newPath, ostr, OW_CIMtoXMLFlags::isInstanceName);
}

//////////////////////////////////////////////////////////////////////////////
void OW_XMLExecute::deleteClass(ostream& /*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String className = node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName );

	if ( className == "" )
		OW_THROWCIM( OW_CIMException::INVALID_CLASS );

	path.setObjectName( className );
	hdl.deleteClass( path );
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::deleteInstance(ostream&	/*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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
void
OW_XMLExecute::deleteQualifier(ostream& /*ostr*/, OW_XMLNode& qualNode,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String qname = getQualifierName(qualNode);
	path.setObjectName(qname);
	hdl.deleteQualifierType(path);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class ClassNameXMLWriter : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		ClassNameXMLWriter(std::ostream& ostr_) : ostr(ostr_) {}
	protected:
		virtual void doHandleObjectPath(const OW_CIMObjectPath &cop)
		{
			ostr << "<CLASSNAME NAME=\"" << cop.getObjectName() <<
				"\"/>\r\n";
		}
	private:
		std::ostream& ostr;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateClassNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String className;
	OW_Bool deepInheritance;

	className=node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);
	deepInheritance=node.extractParameterValue(XMLP_DEEP,OW_Bool(false));

	path.setObjectName(className);

	ClassNameXMLWriter handler(ostr);
	hdl.enumClassNames(path, handler, deepInheritance);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassXMLOutputter : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassXMLOutputter(ostream& ostr_, bool localOnly_,
			bool includeQualifiers_, bool includeClassOrigin_)
		: ostr(ostr_)
		, localOnly(localOnly_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		{}
	protected:
		virtual void doHandleClass(const OW_CIMClass &c)
		{
			OW_CIMtoXML(c, ostr,
				localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				OW_StringArray());
		}
	private:
		ostream& ostr;
		bool localOnly, includeQualifiers, includeClassOrigin;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateClasses( ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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

	CIMClassXMLOutputter handler(ostr, localOnly, includeQualifiers,
		includeClassOrigin);
	hdl.enumClass(path, handler, deep, false);

	// TODO: Switch to this.  It doesn't seem to work though (long make check fails.)
	//hdl.enumClass(path, deep, localOnly,
		//includeQualifiers, includeClassOrigin);

}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceNameXMLOutputter : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		CIMInstanceNameXMLOutputter(ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandleObjectPath(const OW_CIMObjectPath &cop)
		{
			OW_CIMtoXML(cop, ostr, OW_CIMtoXMLFlags::isInstanceName);
		}
	private:
		ostream& ostr;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateInstanceNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String className = node.extractParameterValueAttr(XMLP_CLASSNAME,
		OW_XMLNode::XML_ELEMENT_CLASSNAME, paramName);

	if (className.length() == 0)
	{
		OW_THROWCIM(OW_CIMException::NOT_FOUND);
	}

	path.setObjectName(className);
	
	CIMInstanceNameXMLOutputter handler(ostr);
	// Note that while the API allows deep the XML encodings don't.
	hdl.enumInstanceNames(path, handler);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMInstanceXMLOutputter : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceXMLOutputter(
			std::ostream& ostr_,
			OW_CIMObjectPath& path_,
			bool localOnly_, bool includeQualifiers_, bool includeClassOrigin_, bool isPropertyList_,
			OW_StringArray& propertyList_)
		: ostr(ostr_)
		, path(path_)
		, localOnly(localOnly_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, isPropertyList(isPropertyList_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandleInstance(const OW_CIMInstance &i)
		{
			const OW_CIMInstance& cimInstance = i;
			OW_CIMObjectPath cop(cimInstance.getClassName(),
				cimInstance.getKeyValuePairs());

			cop.setNameSpace(path.getNameSpace());

			OW_CIMtoXML(cimInstance, ostr, cop,
				localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
				includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
				includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
				propertyList,
				(isPropertyList && propertyList.size() == 0));
		}
		std::ostream& ostr;
		OW_CIMObjectPath& path;
		bool localOnly, includeQualifiers, includeClassOrigin, isPropertyList;
		OW_StringArray& propertyList;

	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateInstances(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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

	OW_StringArray* pPropList = (isPropertyList) ? &propertyList : NULL;

	CIMInstanceXMLOutputter handler(ostr, path, localOnly, includeQualifiers,
		includeClassOrigin, isPropertyList, propertyList);
	hdl.enumInstances(path, handler, deep, localOnly,
		includeQualifiers, includeClassOrigin, pPropList);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMQualifierTypeXMLOutputter : public OW_CIMQualifierTypeResultHandlerIFC
	{
	public:
		CIMQualifierTypeXMLOutputter(
			std::ostream& ostr_)
		: ostr(ostr_)
		{}
	protected:
		virtual void doHandleQualifierType(const OW_CIMQualifierType &i)
		{
			OW_CIMtoXML(i, ostr);
		}
		std::ostream& ostr;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::enumerateQualifiers(ostream& ostr, OW_XMLNode& /*node*/,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	CIMQualifierTypeXMLOutputter handler(ostr);
	hdl.enumQualifierTypes(path, handler);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getClass(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	if (!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"No parameters specified for GetClass method call. "
			"ClassName must be given.");
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
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Invalid ClassName");
	}

	propertyList = node.extractParameterStringArray(XMLP_PROPERTYLIST,
		isPropertyList);

	localOnly = node.extractParameterValue(XMLP_LOCAL, OW_Bool(true));
	includeQualifiers = node.extractParameterValue(XMLP_QUAL, OW_Bool(true));
	includeClassOrigin = node.extractParameterValue(XMLP_ORIGIN, OW_Bool(false));

	path.setObjectName(className);

	cimClass = hdl.getClass(path, localOnly, includeQualifiers,
		includeClassOrigin,
		isPropertyList ? &propertyList : 0);
	
	OW_CIMtoXML(cimClass, ostr,
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(isPropertyList && propertyList.size() == 0));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getInstance(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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

	OW_CIMInstance cimInstance = hdl.getInstance(path, localOnly,
		includeQualifiers, includeClassOrigin,
		isPropertyList ? &propertyList : 0);

	OW_CIMtoXML(cimInstance, ostr, OW_CIMObjectPath(),
		localOnly ? OW_CIMtoXMLFlags::localOnly : OW_CIMtoXMLFlags::notLocalOnly,
		includeQualifiers ? OW_CIMtoXMLFlags::includeQualifiers : OW_CIMtoXMLFlags::dontIncludeQualifiers,
		includeClassOrigin ? OW_CIMtoXMLFlags::includeClassOrigin : OW_CIMtoXMLFlags::dontIncludeClassOrigin,
		propertyList,
		(isPropertyList && propertyList.size() == 0));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getProperty(ostream& ostr, OW_XMLNode& propNode,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String propertyName=propNode.mustExtractParameterValue(XMLP_PROPERTYNAME);

	propNode=propNode.findElementAndParameter("InstanceName");
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
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::getQualifier(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{

	path.setObjectName(getQualifierName(node));

	OW_CIMQualifierType qual = hdl.getQualifierType(path);
	if (qual)
	{
		OW_CIMtoXML(qual, ostr);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::modifyClass(ostream&	/*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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

	hdl.modifyClass(path,cimClass);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::modifyInstance(ostream&	/*ostr*/, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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
	// TODO: Much more validation needs to be performed!
	//
	hdl.modifyInstance(path,cimInstance);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::referenceNames(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path,OW_CIMOMHandleIFC& hdl)
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
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::references(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
{
	OW_String className;
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

		ostr << "</VALUE.OBJECTWITHPATH>\r\n";
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::setProperty(ostream&	/*ostr*/, OW_XMLNode& propNode,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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

	propNode = propNode.findElementAndParameter("InstanceName");
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
void
OW_XMLExecute::execQuery(ostream& ostr, OW_XMLNode& node,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLExecute::setQualifier(ostream& /*ostr*/, OW_XMLNode& qualNode,
	OW_CIMObjectPath& path, OW_CIMOMHandleIFC& hdl)
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

		m_functionName = newnode.mustGetAttribute(OW_XMLAttribute::NAME);

		newnode = newnode.getChild();

		OW_CIMOMHandleIFCRef hdl = this->getEnvironment()->getCIMOMHandle(userName, true);

		if (m_isIntrinsic)
		{
			if (!newnode)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					"Missing <LOCALNAMESPACEPATH> in <IMETHODCALL>");
			}

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
			if (!newnode)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					"Missing <LOCALINSTANCEPATH> or <LOCALCLASSPATH> in <METHODCALL>");
			}

			executeExtrinsic(ostrEntity, newnode, *hdl);
		}
		ostrEntity << "</SIMPLERSP>\r\n";
	}
	catch (OW_CIMException& ce)
	{
		OW_LOGDEBUG(format("XMLExecute::processSimpleReq caught CIM "
			"exception:\nCode: %1\nFile: %2\n Line: %3\nMessage: %4",
			ce.getErrNo(), ce.getFile(), ce.getLine(), ce.getMessage()));

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
OW_XMLExecute::doOptions(OW_CIMFeatures& cf,
	const OW_SortedVector<OW_String, OW_String>& /*handlerVars*/)
{
	cf = this->getEnvironment()->getCIMOMHandle("", false)->getServerFeatures();
}

//////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFC*
OW_XMLExecute::clone() const
{
	OW_ASSERT(!m_ostrEntity);
	OW_ASSERT(!m_ostrError);
	return new OW_XMLExecute(*this);
}


//////////////////////////////////////////////////////////////////////////////
OW_REQUEST_HANDLER_FACTORY(OW_XMLExecute);


