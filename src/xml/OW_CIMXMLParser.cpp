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
#include <algorithm> // for std::lower_bound

void
OW_CIMXMLParser::prime()
{
	if (!m_parser.next(m_curTok))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Empty XML");
	}
	if (m_curTok.type == OW_XMLToken::XML_DECLARATION)
	{
		if (!m_parser.next(m_curTok))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Empty XML");
		}
	}
	if (m_curTok.type == OW_XMLToken::DOCTYPE)
	{
		if (!m_parser.next(m_curTok))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Empty XML");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMXMLParser::OW_CIMXMLParser(const OW_String& str)
	: m_ptfs(new OW_TempFileStream())
	, m_parser()
	, m_curTok()
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
{
	prime();
}


// It would appear that this needs to be sorted alphabetically,
// although dan didn't put any such comment here.  :)
OW_CIMXMLParser::ElemEntry OW_CIMXMLParser::g_elems[62] =
{
	{ "CIM", OW_CIMXMLParser::XML_ELEMENT_CIM },
	{ "CLASS", OW_CIMXMLParser::XML_ELEMENT_CLASS },
	{ "CLASSNAME", OW_CIMXMLParser::XML_ELEMENT_CLASSNAME },
	{ "CLASSPATH", OW_CIMXMLParser::XML_ELEMENT_CLASSPATH },
	{ "DECLARATION", OW_CIMXMLParser::XML_ELEMENT_DECLARATION },
	{ "DECLGROUP", OW_CIMXMLParser::XML_ELEMENT_DECLGROUP },
	{ "DECLGROUP.WITHNAME", OW_CIMXMLParser::XML_ELEMENT_DECLGROUP_WITHNAME },
	{ "DECLGROUP.WITHPATH", OW_CIMXMLParser::XML_ELEMENT_DECLGROUP_WITHPATH },
	{ "ERROR", OW_CIMXMLParser::XML_ELEMENT_ERROR },
	{ "EXPMETHODCALL", OW_CIMXMLParser::XML_ELEMENT_EXPMETHODCALL },
	{ "EXPMETHODRESPONSE", OW_CIMXMLParser::XML_ELEMENT_EXPMETHODRESPONSE },
//	{ "EXPPARAMVALUE", OW_CIMXMLParser::XML_ELEMENT_EXPPARAMVALUE },
	{ "HOST", OW_CIMXMLParser::XML_ELEMENT_HOST },
	{ "IMETHODCALL", OW_CIMXMLParser::XML_ELEMENT_IMETHODCALL },
	{ "IMETHODRESPONSE", OW_CIMXMLParser::XML_ELEMENT_IMETHODRESPONSE },
	{ "IMPLICITKEY", OW_CIMXMLParser::XML_ELEMENT_IMPLICITKEY },
	{ "INSTANCE", OW_CIMXMLParser::XML_ELEMENT_INSTANCE },
	{ "INSTANCENAME", OW_CIMXMLParser::XML_ELEMENT_INSTANCENAME },
	{ "INSTANCEPATH", OW_CIMXMLParser::XML_ELEMENT_INSTANCEPATH },
	{ "IPARAMVALUE", OW_CIMXMLParser::XML_ELEMENT_IPARAMVALUE },
	{ "IRETURNVALUE", OW_CIMXMLParser::XML_ELEMENT_IRETURNVALUE },
	{ "KEYBINDING", OW_CIMXMLParser::XML_ELEMENT_KEYBINDING },
	{ "KEYVALUE", OW_CIMXMLParser::XML_ELEMENT_KEYVALUE },
	{ "LOCALCLASSPATH", OW_CIMXMLParser::XML_ELEMENT_LOCALCLASSPATH },
	{ "LOCALINSTANCEPATH", OW_CIMXMLParser::XML_ELEMENT_LOCALINSTANCEPATH },
	{ "LOCALNAMESPACEPATH", OW_CIMXMLParser::XML_ELEMENT_LOCALNAMESPACEPATH },
	{ "MESSAGE", OW_CIMXMLParser::XML_ELEMENT_MESSAGE },
	{ "METHOD", OW_CIMXMLParser::XML_ELEMENT_METHOD },
	{ "METHODCALL", OW_CIMXMLParser::XML_ELEMENT_METHODCALL },
	{ "METHODRESPONSE", OW_CIMXMLParser::XML_ELEMENT_METHODRESPONSE },
	{ "MULTIEXPREQ", OW_CIMXMLParser::XML_ELEMENT_MULTIEXPREQ },
	{ "MULTIEXPRSP", OW_CIMXMLParser::XML_ELEMENT_MULTIEXPRSP },
	{ "MULTIREQ", OW_CIMXMLParser::XML_ELEMENT_MULTIREQ },
	{ "MULTIRSP", OW_CIMXMLParser::XML_ELEMENT_MULTIRSP },
	{ "NAMESPACE", OW_CIMXMLParser::XML_ELEMENT_NAMESPACE },
	{ "NAMESPACEPATH", OW_CIMXMLParser::XML_ELEMENT_NAMESPACEPATH },
	{ "OBJECTPATH", OW_CIMXMLParser::XML_ELEMENT_OBJECTPATH },
	{ "PARAMETER", OW_CIMXMLParser::XML_ELEMENT_PARAMETER },
	{ "PARAMETER.ARRAY", OW_CIMXMLParser::XML_ELEMENT_PARAMETER_ARRAY },
	{ "PARAMETER.REFARRAY", OW_CIMXMLParser::XML_ELEMENT_PARAMETER_REFARRAY },
	{ "PARAMETER.REFERENCE", OW_CIMXMLParser::XML_ELEMENT_PARAMETER_REFERENCE },
	{ "PARAMVALUE", OW_CIMXMLParser::XML_ELEMENT_PARAMVALUE },
	{ "PROPERTY", OW_CIMXMLParser::XML_ELEMENT_PROPERTY },
	{ "PROPERTY.ARRAY", OW_CIMXMLParser::XML_ELEMENT_PROPERTY_ARRAY },
	{ "PROPERTY.REFERENCE", OW_CIMXMLParser::XML_ELEMENT_PROPERTY_REF },
	{ "QUALIFIER", OW_CIMXMLParser::XML_ELEMENT_QUALIFIER },
	{ "QUALIFIER.DECLARATION", OW_CIMXMLParser::XML_ELEMENT_QUALIFIER_DECLARATION },
	{ "RETURNVALUE", OW_CIMXMLParser::XML_ELEMENT_RETURNVALUE },
	{ "SCOPE", OW_CIMXMLParser::XML_ELEMENT_SCOPE },
	{ "SIMPLEEXPREQ", OW_CIMXMLParser::XML_ELEMENT_SIMPLEEXPREQ },
	{ "SIMPLEEXPRSP", OW_CIMXMLParser::XML_ELEMENT_SIMPLEEXPRSP },
	{ "SIMPLEREQ", OW_CIMXMLParser::XML_ELEMENT_SIMPLEREQ },
	{ "SIMPLERSP", OW_CIMXMLParser::XML_ELEMENT_SIMPLERSP },
	{ "VALUE", OW_CIMXMLParser::XML_ELEMENT_VALUE },
	{ "VALUE.ARRAY", OW_CIMXMLParser::XML_ELEMENT_VALUE_ARRAY },
	{ "VALUE.NAMEDINSTANCE", OW_CIMXMLParser::XML_ELEMENT_VALUE_NAMEDINSTANCE },
	{ "VALUE.NAMEDOBJECT", OW_CIMXMLParser::XML_ELEMENT_VALUE_NAMEDOBJECT },
	{ "VALUE.OBJECT", OW_CIMXMLParser::XML_ELEMENT_VALUE_OBJECT },
	{ "VALUE.OBJECTWITHLOCALPATH", OW_CIMXMLParser::XML_ELEMENT_VALUE_OBJECTWITHLOCALPATH },
	{ "VALUE.OBJECTWITHPATH", OW_CIMXMLParser::XML_ELEMENT_VALUE_OBJECTWITHPATH },
	{ "VALUE.REFARRAY", OW_CIMXMLParser::XML_ELEMENT_VALUE_REFARRAY },
	{ "VALUE.REFERENCE", OW_CIMXMLParser::XML_ELEMENT_VALUE_REFERENCE },
	{ "garbage", OW_CIMXMLParser::XML_ELEMENT_UNKNOWN }
};

		
OW_CIMXMLParser::ElemEntry* OW_CIMXMLParser::g_elemsEnd = &OW_CIMXMLParser::g_elems[61];

//////////////////////////////////////////////////////////////////////////////
bool
OW_CIMXMLParser::elemEntryCompare(const OW_CIMXMLParser::ElemEntry& f1,
	const OW_CIMXMLParser::ElemEntry& f2)
{
	return (strcmp(f1.name, f2.name) < 0);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMXMLParser::tokenId
OW_CIMXMLParser::getTokenFromName(const char* name)
{
	ElemEntry e = { 0, OW_CIMXMLParser::XML_ELEMENT_UNKNOWN };
	e.name = name;
	ElemEntry* i = std::lower_bound(g_elems, g_elemsEnd, e, elemEntryCompare);
	if (i == g_elemsEnd)
	{
		return OW_CIMXMLParser::XML_ELEMENT_UNKNOWN;
	}
	else
	{
		return i->id;
	}
}


OW_String
OW_CIMXMLParser::getAttribute(const char* const attrId, bool throwIfError)
{

	for (int i = 0; i < m_curTok.attributeCount; i++)
	{
		OW_XMLToken::Attribute attr = m_curTok.attributes[i]

		// Should this be case insensentive? NO
		if (attr.name.equals(attrId))
		{
			return attr.value.toString();
		}
	}

	if (throwIFError)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("Failed to find "
						"attribute: %1 in node: %2", name, m_strName).c_str() );
	}

	return OW_String();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMXMLParser::mustGetAttribute(const char* const attrId)
{
	return getAttribute(attrId, true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetChild(OW_CIMXMLParser::tokenId tId)
{
	if (!m_good)
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER);
	}

	getChild();
	if (!m_good || !tokenIs(tId))
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER);
	}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetChild()
{
	if (!m_good)
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER);
	}

	getChild();
	if (!m_good)
	{
		m_good = false;
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER);
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

	if (m_curTok.type == OW_XMLToken::EMPTY_TAG)
		// an empty tag can't have children
	{
		m_good = false;
		return;
	}
	for(;;)
	{
		m_good = m_parser.next(m_curTok);
		if (!m_good)
		{
			return;
		}
		switch (m_curTok.type)
		{
			case OW_XMLToken::END_TAG: // hit the end, no children
				m_good = false;
			case OW_XMLToken::EMPTY_TAG: // these are
			case OW_XMLToken::START_TAG: // valid tokens for a child
				return;
			default:
				break;
		}
	}
}



//////////////////////////////////////////////////////////////////////////////
bool
OW_CIMXMLParser::tokenIs(const char* const arg) const
{
	return m_curTok.text.equals(arg);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_CIMXMLParser::tokenIs(OW_CIMXMLParser::tokenId tId) const
{
	return tokenIs(g_elems[tId].name);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetNext()
{
	getNext(true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetNext(OW_CIMXMLParser::tokenId beginTok)
{
	getNext(beginTok, true);
}

//////////////////////////////////////////////////////////////////////////////

void
OW_CIMXMLParser::getNext(bool throwIfError)
{
	m_good = m_parser.next(m_curTok);
	if (!m_good && throwIfError)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::getNext(OW_CIMXMLParser::tokenId beginTok, bool throwIfError)
{
	while(m_good )
	{
		if (m_curTok.type == OW_XMLToken::START_TAG
			 || m_curTok.type == OW_XMLToken::EMPTY_TAG)
		{
			if (m_curTok.text.equals(g_elems[beginTok].name))
			{
				break;
			}
		}
		m_good = m_parser.next(m_curTok);
	}
	if (!m_good && throwIfError)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMXMLParser::mustGetEndTag()
{
	if (m_curTok.type != OW_XMLToken::END_TAG)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER);
	}
	getNext();
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMXMLParser::tokenId
OW_CIMXMLParser::getToken() const
{
	return getTokenFromName(m_curTok.text.c_str());
}


//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMXMLParser::getText() const
{
	return m_curTok.text.toString();
}
