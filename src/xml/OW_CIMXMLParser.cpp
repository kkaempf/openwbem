/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_XMLUnescape.hpp"
#include <algorithm> // for std::lower_bound

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
CIMXMLParser::CIMXMLParser(const String& str)
	: XMLPullParser(str)
{
}
///////////////////////////////////////////////////////////////////////////////
CIMXMLParser::CIMXMLParser(std::istream& istr)
	: XMLPullParser(istr)
{
}
///////////////////////////////////////////////////////////////////////////////
CIMXMLParser::CIMXMLParser()
	: XMLPullParser()
{
}

// This needs to be sorted alphabetically.
CIMXMLParser::ElemEntry CIMXMLParser::g_elems[] =
{
	{ "CIM", CIMXMLParser::E_CIM },
	{ "CLASS", CIMXMLParser::E_CLASS },
	{ "CLASSNAME", CIMXMLParser::E_CLASSNAME },
	{ "CLASSPATH", CIMXMLParser::E_CLASSPATH },
	{ "DECLARATION", CIMXMLParser::E_DECLARATION },
	{ "DECLGROUP", CIMXMLParser::E_DECLGROUP },
	{ "DECLGROUP.WITHNAME", CIMXMLParser::E_DECLGROUP_WITHNAME },
	{ "DECLGROUP.WITHPATH", CIMXMLParser::E_DECLGROUP_WITHPATH },
	{ "ERROR", CIMXMLParser::E_ERROR },
	{ "EXPMETHODCALL", CIMXMLParser::E_EXPMETHODCALL },
	{ "EXPMETHODRESPONSE", CIMXMLParser::E_EXPMETHODRESPONSE },
	{ "EXPPARAMVALUE", CIMXMLParser::E_EXPPARAMVALUE },
	{ "HOST", CIMXMLParser::E_HOST },
	{ "IMETHODCALL", CIMXMLParser::E_IMETHODCALL },
	{ "IMETHODRESPONSE", CIMXMLParser::E_IMETHODRESPONSE },
	{ "IMPLICITKEY", CIMXMLParser::E_IMPLICITKEY },
	{ "INSTANCE", CIMXMLParser::E_INSTANCE },
	{ "INSTANCENAME", CIMXMLParser::E_INSTANCENAME },
	{ "INSTANCEPATH", CIMXMLParser::E_INSTANCEPATH },
	{ "IPARAMVALUE", CIMXMLParser::E_IPARAMVALUE },
	{ "IRETURNVALUE", CIMXMLParser::E_IRETURNVALUE },
	{ "KEYBINDING", CIMXMLParser::E_KEYBINDING },
	{ "KEYVALUE", CIMXMLParser::E_KEYVALUE },
	{ "LOCALCLASSPATH", CIMXMLParser::E_LOCALCLASSPATH },
	{ "LOCALINSTANCEPATH", CIMXMLParser::E_LOCALINSTANCEPATH },
	{ "LOCALNAMESPACEPATH", CIMXMLParser::E_LOCALNAMESPACEPATH },
	{ "MESSAGE", CIMXMLParser::E_MESSAGE },
	{ "METHOD", CIMXMLParser::E_METHOD },
	{ "METHODCALL", CIMXMLParser::E_METHODCALL },
	{ "METHODRESPONSE", CIMXMLParser::E_METHODRESPONSE },
	{ "MULTIEXPREQ", CIMXMLParser::E_MULTIEXPREQ },
	{ "MULTIEXPRSP", CIMXMLParser::E_MULTIEXPRSP },
	{ "MULTIREQ", CIMXMLParser::E_MULTIREQ },
	{ "MULTIRSP", CIMXMLParser::E_MULTIRSP },
	{ "NAMESPACE", CIMXMLParser::E_NAMESPACE },
	{ "NAMESPACEPATH", CIMXMLParser::E_NAMESPACEPATH },
	{ "OBJECTPATH", CIMXMLParser::E_OBJECTPATH },
	{ "PARAMETER", CIMXMLParser::E_PARAMETER },
	{ "PARAMETER.ARRAY", CIMXMLParser::E_PARAMETER_ARRAY },
	{ "PARAMETER.REFARRAY", CIMXMLParser::E_PARAMETER_REFARRAY },
	{ "PARAMETER.REFERENCE", CIMXMLParser::E_PARAMETER_REFERENCE },
	{ "PARAMVALUE", CIMXMLParser::E_PARAMVALUE },
	{ "PROPERTY", CIMXMLParser::E_PROPERTY },
	{ "PROPERTY.ARRAY", CIMXMLParser::E_PROPERTY_ARRAY },
	{ "PROPERTY.REFERENCE", CIMXMLParser::E_PROPERTY_REFERENCE },
	{ "QUALIFIER", CIMXMLParser::E_QUALIFIER },
	{ "QUALIFIER.DECLARATION", CIMXMLParser::E_QUALIFIER_DECLARATION },
	{ "RETURNVALUE", CIMXMLParser::E_RETURNVALUE },
	{ "SCOPE", CIMXMLParser::E_SCOPE },
	{ "SIMPLEEXPREQ", CIMXMLParser::E_SIMPLEEXPREQ },
	{ "SIMPLEEXPRSP", CIMXMLParser::E_SIMPLEEXPRSP },
	{ "SIMPLEREQ", CIMXMLParser::E_SIMPLEREQ },
	{ "SIMPLERSP", CIMXMLParser::E_SIMPLERSP },
	{ "VALUE", CIMXMLParser::E_VALUE },
	{ "VALUE.ARRAY", CIMXMLParser::E_VALUE_ARRAY },
	{ "VALUE.NAMEDINSTANCE", CIMXMLParser::E_VALUE_NAMEDINSTANCE },
	{ "VALUE.NAMEDOBJECT", CIMXMLParser::E_VALUE_NAMEDOBJECT },
	{ "VALUE.OBJECT", CIMXMLParser::E_VALUE_OBJECT },
	{ "VALUE.OBJECTWITHLOCALPATH", CIMXMLParser::E_VALUE_OBJECTWITHLOCALPATH },
	{ "VALUE.OBJECTWITHPATH", CIMXMLParser::E_VALUE_OBJECTWITHPATH },
	{ "VALUE.REFARRAY", CIMXMLParser::E_VALUE_REFARRAY },
	{ "VALUE.REFERENCE", CIMXMLParser::E_VALUE_REFERENCE },
	{ "garbage", CIMXMLParser::E_UNKNOWN }
};
		
CIMXMLParser::ElemEntry* CIMXMLParser::g_elemsEnd = &CIMXMLParser::g_elems[0] +
	(sizeof(CIMXMLParser::g_elems)/sizeof(*CIMXMLParser::g_elems)) - 1;
//////////////////////////////////////////////////////////////////////////////
inline bool
CIMXMLParser::elemEntryCompare(const CIMXMLParser::ElemEntry& f1,
	const CIMXMLParser::ElemEntry& f2)
{
	return (strcmp(f1.name, f2.name) < 0);
}
//////////////////////////////////////////////////////////////////////////////
CIMXMLParser::tokenId
CIMXMLParser::getTokenFromName(const char* name)
{
	ElemEntry e = { 0, CIMXMLParser::E_UNKNOWN };
	e.name = name;
	ElemEntry* i = std::lower_bound(g_elems, g_elemsEnd, e, elemEntryCompare);
	if (i == g_elemsEnd || strcmp((*i).name, name) != 0)
	{
		return CIMXMLParser::E_UNKNOWN;
	}
	else
	{
		return i->id;
	}
}

const char* const CIMXMLParser::A_ARRAYSIZE					= "ARRAYSIZE";
const char* const CIMXMLParser::A_ASSOCCLASS				= "ASSOCCLASS";
const char* const CIMXMLParser::A_CLASSNAME					= "CLASSNAME";
const char* const CIMXMLParser::A_CLASSORIGIN				= "CLASSORIGIN";
const char* const CIMXMLParser::A_DEEPINHERITANCE			= "DEEPINHERITANCE";
const char* const CIMXMLParser::A_INCLUDECLASSORIGIN	= "INCLUDECLASSORIGIN";
const char* const CIMXMLParser::A_INCLUDEQUALIFIERS		= "INCLUDEQUALIFIERS";
const char* const CIMXMLParser::A_INSTANCENAME				= "INSTANCENAME";
const char* const CIMXMLParser::A_LOCALONLY					= "LOCALONLY";
const char* const CIMXMLParser::A_MODIFIEDCLASS			= "MODIFIEDCLASS";
const char* const CIMXMLParser::A_NAME							= "NAME";
const char* const CIMXMLParser::A_NEWVALUE					= "NEWVALUE";
const char* const CIMXMLParser::A_OBJECTNAME				= "OBJECTNAME";
const char* const CIMXMLParser::A_OVERRIDABLE				= "OVERRIDABLE";
const char* const CIMXMLParser::A_PARAMTYPE							= "PARAMTYPE";
const char* const CIMXMLParser::A_PROPAGATED					= "PROPAGATED";
const char* const CIMXMLParser::A_PROPERTYLIST				= "PROPERTYLIST";
const char* const CIMXMLParser::A_PROPERTYNAME				= "PROPERTYNAME";
const char* const CIMXMLParser::A_REFERENCECLASS			= "REFERENCECLASS";
const char* const CIMXMLParser::A_RESULTCLASS				= "RESULTCLASS";
const char* const CIMXMLParser::A_RESULTROLE				= "RESULTROLE";
const char* const CIMXMLParser::A_ROLE							= "ROLE";
const char* const CIMXMLParser::A_SUPERCLASS				= "SUPERCLASS";
const char* const CIMXMLParser::A_TOINSTANCE					= "TOINSTANCE"; // This is a bug in the spec
const char* const CIMXMLParser::A_TOSUBCLASS					= "TOSUBCLASS";
const char* const CIMXMLParser::A_TRANSLATABLE				= "TRANSLATABLE";
const char* const CIMXMLParser::A_TYPE							= "TYPE";
const char* const CIMXMLParser::A_VALUETYPE					= "VALUETYPE";
const char* const CIMXMLParser::A_CIMVERSION = "CIMVERSION";
const char* const CIMXMLParser::A_DTDVERSION = "DTDVERSION";
const char* const CIMXMLParser::A_ID = "ID";
const char* const CIMXMLParser::A_PROTOCOLVERSION = "PROTOCOLVERSION";
const char* const CIMXMLParser::A_CODE		= "CODE";
const char* const CIMXMLParser::A_DESCRIPTION		= "DESCRIPTION";
const char* const CIMXMLParser::A_ISARRAY="ISARRAY";
const char* const CIMXMLParser::AV_CIMVERSION20_VALUE = "2.0";
const char* const CIMXMLParser::AV_CIMVERSION21_VALUE = "2.1";
const char* const CIMXMLParser::AV_CIMVERSION22_VALUE = "2.2";
const char* const CIMXMLParser::AV_DTDVERSION20_VALUE = "2.0";
const char* const CIMXMLParser::AV_DTDVERSION21_VALUE = "2.1";
const char* const CIMXMLParser::AV_PROTOCOLVERSION10_VALUE = "1.0";
const char* const CIMXMLParser::AV_PROTOCOLVERSION11_VALUE = "1.1";
const char* const CIMXMLParser::P_ClassName     	= "ClassName";
const char* const CIMXMLParser::P_PropertyList  	= "PropertyList";
const char* const CIMXMLParser::P_DeepInheritance       		= "DeepInheritance";
const char* const CIMXMLParser::P_LocalOnly         	= "LocalOnly";
const char* const CIMXMLParser::P_IncludeQualifiers       		= "IncludeQualifiers";
const char* const CIMXMLParser::P_IncludeClassOrigin     		= "IncludeClassOrigin";
const char* const CIMXMLParser::P_ModifiedClass	= "ModifiedClass";
const char* const CIMXMLParser::P_ModifiedInstance	= "ModifiedInstance";
const char* const CIMXMLParser::P_InstanceName	= "InstanceName";
const char* const CIMXMLParser::P_PropertyName	= "PropertyName";
const char* const CIMXMLParser::P_NewValue			= "NewValue";
const char* const CIMXMLParser::P_Role				= "Role";
const char* const CIMXMLParser::P_ResultRole		= "ResultRole";
const char* const CIMXMLParser::P_ObjectName		= "ObjectName";
const char* const CIMXMLParser::P_AssocClass		= "AssocClass";
const char* const CIMXMLParser::P_ResultClass		= "ResultClass";
const char* const CIMXMLParser::P_QueryLanguage = "QueryLanguage";
const char* const CIMXMLParser::P_Query = "Query";
const char* const CIMXMLParser::P_QualifierName = "QualifierName";
const char* const CIMXMLParser::P_QualifierDeclaration = "QualifierDeclaration";


//////////////////////////////////////////////////////////////////////////////
void
CIMXMLParser::mustGetChildId(CIMXMLParser::tokenId tId)
{
	if (!m_good)
	{
		m_good = false;
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"CIMXMLParser has reached EOF");
	}
	getChild();
	if (!m_good || !tokenIsId(tId))
	{
		m_good = false;
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			format("CIMXMLParser::mustGetChild(CIMXMLParser::tokenId tId=%1) failed.  parser = %2",
				g_elems[tId].name, *this).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLParser::mustTokenIsId(CIMXMLParser::tokenId tId) const
{
	if (!tokenIs(g_elems[tId].name))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			format("CIMXMLParser::mustTokenIs(CIMXMLParser::tokenId tId=%1) failed.  parser = %2",
				g_elems[tId].name, *this).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMXMLParser::getNextId(CIMXMLParser::tokenId beginTok, bool throwIfError)
{
	while (m_good)
	{
		if (m_curTok.type == XMLToken::START_TAG)
		{
			if (m_curTok.text.equals(g_elems[beginTok].name))
			{
				break;
			}
		}
		nextToken();
	}
	if (!m_good && throwIfError)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			format("CIMXMLParser::getNext(CIMXMLParser::tokenId beginTok=%1) failed.  parser = %2",
				g_elems[beginTok].name, *this).c_str());
	}
}

} // end namespace OpenWBEM

