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


#ifndef _OW_CIMXMLPARSER_HPP__
#define _OW_CIMXMLPARSER_HPP__

#include "OW_config.h"
#include "OW_AutoPtr.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_String.hpp"
#include "OW_XMLParser.hpp"

#ifdef OW_HAVE_ISTREAM
#include <istream>
#else
#include <iostream>
#endif

class OW_CIMXMLParser
{

public:

	// These must be sorted alphabetically!!!!!!!!!!!!!!!!!!!!!!!!!
	enum tokenId
	{
		XML_ELEMENT_CIM								,
		XML_ELEMENT_CLASS								,
		XML_ELEMENT_CLASSNAME						,
		XML_ELEMENT_CLASSPATH						,
		XML_ELEMENT_DECLARATION						,
		XML_ELEMENT_DECLGROUP_WITHNAME			,
		XML_ELEMENT_DECLGROUP_WITHPATH			,
		XML_ELEMENT_ERROR								,
		XML_ELEMENT_EXPMETHODCALL 					,
		XML_ELEMENT_EXPMETHODRESPONSE				,
		XML_ELEMENT_HOST								,
		XML_ELEMENT_IMETHODCALL						,
		XML_ELEMENT_IMETHODRESPONSE				,
		XML_ELEMENT_IMPLICITKEY						,
		XML_ELEMENT_INSTANCE							,
		XML_ELEMENT_INSTANCENAME					,
		XML_ELEMENT_INSTANCEPATH					,
		XML_ELEMENT_IPARAMVALUE						,
		XML_ELEMENT_IRETURNVALUE					,
		XML_ELEMENT_KEYBINDING						,
		XML_ELEMENT_KEYVALUE							,
		XML_ELEMENT_LOCALCLASSPATH					,
		XML_ELEMENT_LOCALINSTANCEPATH				,
		XML_ELEMENT_LOCALNAMESPACEPATH			,
		XML_ELEMENT_MESSAGE							,
		XML_ELEMENT_METHOD							,
		XML_ELEMENT_METHODCALL						,
		XML_ELEMENT_METHODRESPONSE					,
		XML_ELEMENT_MULTIEXPREQ 					,
		XML_ELEMENT_MULTIEXPRSP						,
		XML_ELEMENT_MULTIREQ							,
		XML_ELEMENT_MULTIRSP							,
		XML_ELEMENT_NAMESPACE						,
		XML_ELEMENT_NAMESPACEPATH					,
		XML_ELEMENT_OBJECTPATH						,
		XML_ELEMENT_PARAMETER						,
		XML_ELEMENT_PARAMETER_ARRAY				,
		XML_ELEMENT_PARAMETER_REFARRAY			,
		XML_ELEMENT_PARAMETER_REFERENCE			,
		XML_ELEMENT_PARAMVALUE						,
		XML_ELEMENT_PROPERTY							,
		XML_ELEMENT_PROPERTY_ARRAY					,
		XML_ELEMENT_PROPERTY_REF					,
		XML_ELEMENT_QUALIFIER						,
		XML_ELEMENT_QUALIFIER_DECLARATION		,
		XML_ELEMENT_RETURNVALUE						,
		XML_ELEMENT_SCOPE								,
		XML_ELEMENT_SIMPLEEXPREQ 					,
		XML_ELEMENT_SIMPLEEXPRSP					,
		XML_ELEMENT_SIMPLEREQ						,
		XML_ELEMENT_SIMPLERSP						,
		XML_ELEMENT_UNKNOWN							,
		XML_ELEMENT_VALUE								,
		XML_ELEMENT_VALUE_ARRAY						,
		XML_ELEMENT_VALUE_NAMEDINSTANCE			,
		XML_ELEMENT_VALUE_NAMEDOBJECT				,
		XML_ELEMENT_VALUE_OBJECT					,
		XML_ELEMENT_VALUE_OBJECTWITHLOCALPATH	,
		XML_ELEMENT_VALUE_OBJECTWITHPATH			,
		XML_ELEMENT_VALUE_REFARRAY					,
		XML_ELEMENT_VALUE_REFERENCE				

//		XML_ELEMENT_EXPPARAMVALUE  				,
	};


	OW_CIMXMLParser(const OW_String& str);
	OW_CIMXMLParser(std::istream& sb);
	OW_CIMXMLParser();



	OW_String mustGetAttribute(const char* const attrId);
	OW_String getAttribute(const char* const attrId, bool throwIfError = false);

	void getChild();
	void mustGetChild(tokenId tId);
	void mustGetChild();
	void getNext(tokenId beginTok, bool throwIfError = false);
	void getNext(bool throwIfError = false);
	void mustGetNext();
	void mustGetNext(tokenId beginTok);
	void mustGetEndTag();

	bool tokenIs(const char* const arg) const;
	bool tokenIs(tokenId tId) const;

	tokenId getToken() const;

	OW_String getText() const;

private:
	OW_XMLParser m_parser;
	OW_AutoPtr<OW_TempFileStream> m_ptfs;
	OW_XMLToken m_curTok;
	bool m_good;

	void prime();
	static tokenId getTokenFromName(const char* name);
	struct ElemEntry
	{
		const char* name;
		tokenId id;
	};
	
	static ElemEntry g_elems[];
	static bool elemEntryCompare(const ElemEntry& f1, const ElemEntry& f2);
	static ElemEntry* g_elemsEnd;

	// unimplemented
	OW_CIMXMLParser(const OW_CIMXMLParser& x);
	OW_CIMXMLParser& operator=(const OW_CIMXMLParser& x);

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return m_good ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return m_good ? 0: &dummy::nonnull; }
};

#endif //#ifndef _OW_CIMXMLPARSER_HPP__





