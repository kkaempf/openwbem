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
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_XMLUnescape.hpp"

#include <algorithm> // for std::lower_bound

///////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::prime()
{
	if (!(m_good = m_parser.next(m_curTok)))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Empty XML");
	}
	if (m_curTok.type == OW_XMLToken::XML_DECLARATION)
	{
		if (!(m_good = m_parser.next(m_curTok)))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Empty XML");
		}
		skipData();
	}
	if (m_curTok.type == OW_XMLToken::DOCTYPE)
	{
		if (!(m_good = m_parser.next(m_curTok)))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Empty XML");
		}
		skipData();
	}
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMXMLParser::OW_CIMXMLParser(const OW_String& str)
	: m_ptfs(new OW_TempFileStream())
	, m_parser()
	, m_curTok()
	, m_good(true)
{
	*m_ptfs << str;
	m_parser.setInput(*m_ptfs);
	prime();
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMXMLParser::OW_CIMXMLParser(std::istream& istr)
	: m_ptfs()
	, m_parser(istr)
	, m_curTok()
	, m_good(true)
{
	prime();
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMXMLParser::OW_CIMXMLParser()
	: m_good(false)
{
}


// This needs to be sorted alphabetically.
// Also, if you add an entry, make sure and update the size and g_elemsEnd.
OW_CIMXMLParser::ElemEntry OW_CIMXMLParser::g_elems[63] =
{
	{ "CIM", OW_CIMXMLParser::E_CIM },
	{ "CLASS", OW_CIMXMLParser::E_CLASS },
	{ "CLASSNAME", OW_CIMXMLParser::E_CLASSNAME },
	{ "CLASSPATH", OW_CIMXMLParser::E_CLASSPATH },
	{ "DECLARATION", OW_CIMXMLParser::E_DECLARATION },
	{ "DECLGROUP", OW_CIMXMLParser::E_DECLGROUP },
	{ "DECLGROUP.WITHNAME", OW_CIMXMLParser::E_DECLGROUP_WITHNAME },
	{ "DECLGROUP.WITHPATH", OW_CIMXMLParser::E_DECLGROUP_WITHPATH },
	{ "ERROR", OW_CIMXMLParser::E_ERROR },
	{ "EXPMETHODCALL", OW_CIMXMLParser::E_EXPMETHODCALL },
	{ "EXPMETHODRESPONSE", OW_CIMXMLParser::E_EXPMETHODRESPONSE },
	{ "EXPPARAMVALUE", OW_CIMXMLParser::E_EXPPARAMVALUE },
	{ "HOST", OW_CIMXMLParser::E_HOST },
	{ "IMETHODCALL", OW_CIMXMLParser::E_IMETHODCALL },
	{ "IMETHODRESPONSE", OW_CIMXMLParser::E_IMETHODRESPONSE },
	{ "IMPLICITKEY", OW_CIMXMLParser::E_IMPLICITKEY },
	{ "INSTANCE", OW_CIMXMLParser::E_INSTANCE },
	{ "INSTANCENAME", OW_CIMXMLParser::E_INSTANCENAME },
	{ "INSTANCEPATH", OW_CIMXMLParser::E_INSTANCEPATH },
	{ "IPARAMVALUE", OW_CIMXMLParser::E_IPARAMVALUE },
	{ "IRETURNVALUE", OW_CIMXMLParser::E_IRETURNVALUE },
	{ "KEYBINDING", OW_CIMXMLParser::E_KEYBINDING },
	{ "KEYVALUE", OW_CIMXMLParser::E_KEYVALUE },
	{ "LOCALCLASSPATH", OW_CIMXMLParser::E_LOCALCLASSPATH },
	{ "LOCALINSTANCEPATH", OW_CIMXMLParser::E_LOCALINSTANCEPATH },
	{ "LOCALNAMESPACEPATH", OW_CIMXMLParser::E_LOCALNAMESPACEPATH },
	{ "MESSAGE", OW_CIMXMLParser::E_MESSAGE },
	{ "METHOD", OW_CIMXMLParser::E_METHOD },
	{ "METHODCALL", OW_CIMXMLParser::E_METHODCALL },
	{ "METHODRESPONSE", OW_CIMXMLParser::E_METHODRESPONSE },
	{ "MULTIEXPREQ", OW_CIMXMLParser::E_MULTIEXPREQ },
	{ "MULTIEXPRSP", OW_CIMXMLParser::E_MULTIEXPRSP },
	{ "MULTIREQ", OW_CIMXMLParser::E_MULTIREQ },
	{ "MULTIRSP", OW_CIMXMLParser::E_MULTIRSP },
	{ "NAMESPACE", OW_CIMXMLParser::E_NAMESPACE },
	{ "NAMESPACEPATH", OW_CIMXMLParser::E_NAMESPACEPATH },
	{ "OBJECTPATH", OW_CIMXMLParser::E_OBJECTPATH },
	{ "PARAMETER", OW_CIMXMLParser::E_PARAMETER },
	{ "PARAMETER.ARRAY", OW_CIMXMLParser::E_PARAMETER_ARRAY },
	{ "PARAMETER.REFARRAY", OW_CIMXMLParser::E_PARAMETER_REFARRAY },
	{ "PARAMETER.REFERENCE", OW_CIMXMLParser::E_PARAMETER_REFERENCE },
	{ "PARAMVALUE", OW_CIMXMLParser::E_PARAMVALUE },
	{ "PROPERTY", OW_CIMXMLParser::E_PROPERTY },
	{ "PROPERTY.ARRAY", OW_CIMXMLParser::E_PROPERTY_ARRAY },
	{ "PROPERTY.REFERENCE", OW_CIMXMLParser::E_PROPERTY_REFERENCE },
	{ "QUALIFIER", OW_CIMXMLParser::E_QUALIFIER },
	{ "QUALIFIER.DECLARATION", OW_CIMXMLParser::E_QUALIFIER_DECLARATION },
	{ "RETURNVALUE", OW_CIMXMLParser::E_RETURNVALUE },
	{ "SCOPE", OW_CIMXMLParser::E_SCOPE },
	{ "SIMPLEEXPREQ", OW_CIMXMLParser::E_SIMPLEEXPREQ },
	{ "SIMPLEEXPRSP", OW_CIMXMLParser::E_SIMPLEEXPRSP },
	{ "SIMPLEREQ", OW_CIMXMLParser::E_SIMPLEREQ },
	{ "SIMPLERSP", OW_CIMXMLParser::E_SIMPLERSP },
	{ "VALUE", OW_CIMXMLParser::E_VALUE },
	{ "VALUE.ARRAY", OW_CIMXMLParser::E_VALUE_ARRAY },
	{ "VALUE.NAMEDINSTANCE", OW_CIMXMLParser::E_VALUE_NAMEDINSTANCE },
	{ "VALUE.NAMEDOBJECT", OW_CIMXMLParser::E_VALUE_NAMEDOBJECT },
	{ "VALUE.OBJECT", OW_CIMXMLParser::E_VALUE_OBJECT },
	{ "VALUE.OBJECTWITHLOCALPATH", OW_CIMXMLParser::E_VALUE_OBJECTWITHLOCALPATH },
	{ "VALUE.OBJECTWITHPATH", OW_CIMXMLParser::E_VALUE_OBJECTWITHPATH },
	{ "VALUE.REFARRAY", OW_CIMXMLParser::E_VALUE_REFARRAY },
	{ "VALUE.REFERENCE", OW_CIMXMLParser::E_VALUE_REFERENCE },
	{ "garbage", OW_CIMXMLParser::E_UNKNOWN }
};

		
OW_CIMXMLParser::ElemEntry* OW_CIMXMLParser::g_elemsEnd = &OW_CIMXMLParser::g_elems[62];

//////////////////////////////////////////////////////////////////////////////
inline bool
OW_CIMXMLParser::elemEntryCompare(const OW_CIMXMLParser::ElemEntry& f1,
	const OW_CIMXMLParser::ElemEntry& f2)
{
	return (strcmp(f1.name, f2.name) < 0);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMXMLParser::tokenId
OW_CIMXMLParser::getTokenFromName(const char* name)
{
	ElemEntry e = { 0, OW_CIMXMLParser::E_UNKNOWN };
	e.name = name;
	ElemEntry* i = std::lower_bound(g_elems, g_elemsEnd, e, elemEntryCompare);
	if (i == g_elemsEnd || strcmp((*i).name, name) != 0)
	{
		return OW_CIMXMLParser::E_UNKNOWN;
	}
	else
	{
		return i->id;
	}
}


const char* const OW_CIMXMLParser::A_ARRAY_SIZE					= "ARRAYSIZE";
const char* const OW_CIMXMLParser::A_ASSOC_CLASS				= "ASSOCCLASS";
const char* const OW_CIMXMLParser::A_CLASS_NAME					= "CLASSNAME";
const char* const OW_CIMXMLParser::A_CLASS_ORIGIN				= "CLASSORIGIN";
const char* const OW_CIMXMLParser::A_DEEP_INHERITANCE			= "DEEPINHERITANCE";
const char* const OW_CIMXMLParser::A_INCLUDE_CLASS_ORIGIN	= "INCLUDECLASSORIGIN";
const char* const OW_CIMXMLParser::A_INCLUDE_QUALIFIERS		= "INCLUDEQUALIFIERS";
const char* const OW_CIMXMLParser::A_INSTANCE_NAME				= "INSTANCENAME";
const char* const OW_CIMXMLParser::A_LOCAL_ONLY					= "LOCALONLY";
const char* const OW_CIMXMLParser::A_MODIFIED_CLASS			= "MODIFIEDCLASS";
const char* const OW_CIMXMLParser::A_NAME							= "NAME";
const char* const OW_CIMXMLParser::A_NEW_VALUE					= "NEWVALUE";
const char* const OW_CIMXMLParser::A_OBJECT_NAME				= "OBJECTNAME";
const char* const OW_CIMXMLParser::A_OVERRIDABLE				= "OVERRIDABLE";
const char* const OW_CIMXMLParser::A_PARAMTYPE							= "PARAMTYPE";
const char* const OW_CIMXMLParser::A_PROPAGATED					= "PROPAGATED";
const char* const OW_CIMXMLParser::A_PROPERTY_LIST				= "PROPERTYLIST";
const char* const OW_CIMXMLParser::A_PROPERTY_NAME				= "PROPERTYNAME";
const char* const OW_CIMXMLParser::A_REFERENCE_CLASS			= "REFERENCECLASS";
const char* const OW_CIMXMLParser::A_RESULT_CLASS				= "RESULTCLASS";
const char* const OW_CIMXMLParser::A_RESULT_ROLE				= "RESULTROLE";
const char* const OW_CIMXMLParser::A_ROLE							= "ROLE";
const char* const OW_CIMXMLParser::A_SUPER_CLASS				= "SUPERCLASS";
const char* const OW_CIMXMLParser::A_TOINSTANCE					= "TOINSTANCE";
const char* const OW_CIMXMLParser::A_TOSUBCLASS					= "TOSUBCLASS";
const char* const OW_CIMXMLParser::A_TRANSLATABLE				= "TRANSLATABLE";
const char* const OW_CIMXMLParser::A_TYPE							= "TYPE";
const char* const OW_CIMXMLParser::A_VALUE_TYPE					= "VALUETYPE";
const char* const OW_CIMXMLParser::A_CIMVERSION = "CIMVERSION";
const char* const OW_CIMXMLParser::A_DTDVERSION = "DTDVERSION";
const char* const OW_CIMXMLParser::A_MSG_ID = "ID";
const char* const OW_CIMXMLParser::A_PROTOCOLVERSION = "PROTOCOLVERSION";
const char* const OW_CIMXMLParser::AV_CIMVERSION_VALUE = "2.0";
const char* const OW_CIMXMLParser::AV_DTDVERSION_VALUE = "2.0";
const char* const OW_CIMXMLParser::AV_PROTOCOLVERSION_VALUE = "1.0";
const char* const OW_CIMXMLParser::A_PARAMERRORCODE = "CODE";
const char* const OW_CIMXMLParser::A_PARAMERRORDESCRIPTION = "DESCRIPTION";


OW_String
OW_CIMXMLParser::getAttribute(const char* const attrId, bool throwIfError)
{
	OW_ASSERT(m_curTok.type == OW_XMLToken::START_TAG);
	for (unsigned i = 0; i < m_curTok.attributeCount; i++)
	{
		OW_XMLToken::Attribute& attr = m_curTok.attributes[i];

		// Should this be case insensentive? NO
		if (attr.name.equals(attrId))
		{
			return OW_XMLUnescape(attr.value.c_str(), attr.value.length());
		}
	}

	if (throwIfError)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("Failed to find "
						"attribute: %1 in node: %2", attrId, m_curTok.text).c_str() );
	}

	return OW_String();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetChild(OW_CIMXMLParser::tokenId tId)
{
	if (!m_good)
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"CIMXMLParser has reached EOF");
	}

	getChild();
	if (!m_good || !tokenIs(tId))
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::mustGetChild(OW_CIMXMLParser::tokenId tId=%1) failed.  parser = %2",
				g_elems[tId].name, *this).c_str());
	}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetChild()
{
	if (!m_good)
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::mustGetChild() failed.  parser = %1",
				*this).c_str());

	}

	getChild();
	if (!m_good)
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::mustGetChild() failed.  parser = %1",
				*this).c_str());
	}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::getChild()
{
	if (!m_good)
	{
		return;
	}

	for(;;)
	{
		nextToken();
		if (!m_good)
		{
			return;
		}
		switch (m_curTok.type)
		{
			case OW_XMLToken::END_TAG: // hit the end, no children
				m_good = false;
			case OW_XMLToken::START_TAG: // valid token for a child
				return;
			default:
				break;
		}
	}
}



//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustTokenIs(OW_CIMXMLParser::tokenId tId) const
{
//	cout << "tokenIs(" << g_elems[tId].name << ") = " << tokenIs(g_elems[tId].name) << "\n";
	if (!tokenIs(g_elems[tId].name))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::mustTokenIs(OW_CIMXMLParser::tokenId tId=%1) failed.  parser = %2",
				g_elems[tId].name, *this).c_str());
	}
}


//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::getNextTag(bool throwIfError)
{
	nextToken();
	skipData();
	if (!m_good && throwIfError)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::getNext() failed.  parser = %1",
				*this).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::getNext(bool throwIfError)
{
	nextToken();
	if (!m_good && throwIfError)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::getNext() failed.  parser = %1",
				*this).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::getNext(OW_CIMXMLParser::tokenId beginTok, bool throwIfError)
{
	while (m_good)
	{
		if (m_curTok.type == OW_XMLToken::START_TAG)
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
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::getNext(OW_CIMXMLParser::tokenId beginTok=%1) failed.  parser = %2",
				g_elems[beginTok].name, *this).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetEndTag()
{
	skipData();
	if (m_curTok.type != OW_XMLToken::END_TAG)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("OW_CIMXMLParser::mustGetEndTag() failed.  parser = %1",
				*this).c_str());
	}
	getNext();
	skipData();
}


//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMXMLParser::getName() const
{
	OW_ASSERT(m_curTok.type == OW_XMLToken::START_TAG || m_curTok.type == OW_XMLToken::END_TAG);
	return OW_XMLUnescape(m_curTok.text.c_str(), m_curTok.text.length());
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMXMLParser::getData() const
{
	OW_ASSERT(m_curTok.type == OW_XMLToken::CONTENT || m_curTok.type == OW_XMLToken::CDATA);
	return OW_XMLUnescape(m_curTok.text.c_str(), m_curTok.text.length());
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_CIMXMLParser::isData() const
{
	return m_curTok.type == OW_XMLToken::CONTENT || m_curTok.type == OW_XMLToken::CDATA;
}


///////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::nextToken()
{
	do
	{
		m_good = m_parser.next(m_curTok);
	} while (m_curTok.type == OW_XMLToken::COMMENT && m_good);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::skipData()
{
	while (isData() && m_good)
	{
		nextToken();
	}
}

//////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& ostr, const OW_CIMXMLParser& p)
{
	ostr << "m_good = " << p.m_good << '\n';
	switch (p.m_curTok.type)
	{
		case OW_XMLToken::INVALID:
			ostr << "*INVALID*\n";
			break;
		case OW_XMLToken::XML_DECLARATION:
			ostr << "<xml>\n";
			break;
		case OW_XMLToken::START_TAG:
			ostr << '<' << p.m_curTok.text << ' ';
			for (unsigned int x = 0; x < p.m_curTok.attributeCount; ++x)
			{
				ostr << p.m_curTok.attributes[x].name << "=\"" <<
					p.m_curTok.attributes[x].value << "\" ";
			}
			ostr << ">\n";
			break;
		case OW_XMLToken::END_TAG:
			ostr << "</" << p.m_curTok.text << ">\n";
			break;
		case OW_XMLToken::COMMENT:
			ostr << "<--" << p.m_curTok.text << "-->\n";
			break;
		case OW_XMLToken::CDATA:
			ostr << "<CDATA[[" << p.m_curTok.text << "]]>\n";
			break;
		case OW_XMLToken::DOCTYPE:
			ostr << "<DOCTYPE>\n";
			break;
		case OW_XMLToken::CONTENT:
			ostr << "CONTENT: " << p.m_curTok.text << '\n';
			break;
		default:
			ostr << "Unknown token type\n";
	}
	return ostr;
}
