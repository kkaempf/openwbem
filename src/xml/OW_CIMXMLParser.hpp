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

	// These must be sorted alphabetically, except E_UNKNOWN, which must be last!!!!!!!!!!!!!!!!!!!!!!!!!
	enum tokenId
	{
		E_CIM								,
		E_CLASS								,
		E_CLASSNAME						,
		E_CLASSPATH						,
		E_DECLARATION						,
		E_DECLGROUP			,
		E_DECLGROUP_WITHNAME			,
		E_DECLGROUP_WITHPATH			,
		E_ERROR								,
		E_EXPMETHODCALL 					,
		E_EXPMETHODRESPONSE				,
		E_EXPPARAMVALUE  				,
		E_HOST								,
		E_IMETHODCALL						,
		E_IMETHODRESPONSE				,
		E_IMPLICITKEY						,
		E_INSTANCE							,
		E_INSTANCENAME					,
		E_INSTANCEPATH					,
		E_IPARAMVALUE						,
		E_IRETURNVALUE					,
		E_KEYBINDING						,
		E_KEYVALUE							,
		E_LOCALCLASSPATH					,
		E_LOCALINSTANCEPATH				,
		E_LOCALNAMESPACEPATH			,
		E_MESSAGE							,
		E_METHOD							,
		E_METHODCALL						,
		E_METHODRESPONSE					,
		E_MULTIEXPREQ 					,
		E_MULTIEXPRSP						,
		E_MULTIREQ							,
		E_MULTIRSP							,
		E_NAMESPACE						,
		E_NAMESPACEPATH					,
		E_OBJECTPATH						,
		E_PARAMETER						,
		E_PARAMETER_ARRAY				,
		E_PARAMETER_REFARRAY			,
		E_PARAMETER_REFERENCE			,
		E_PARAMVALUE						,
		E_PROPERTY							,
		E_PROPERTY_ARRAY					,
		E_PROPERTY_REFERENCE					,
		E_QUALIFIER						,
		E_QUALIFIER_DECLARATION		,
		E_RETURNVALUE						,
		E_SCOPE								,
		E_SIMPLEEXPREQ 					,
		E_SIMPLEEXPRSP					,
		E_SIMPLEREQ						,
		E_SIMPLERSP						,
		E_VALUE								,
		E_VALUE_ARRAY						,
		E_VALUE_NAMEDINSTANCE			,
		E_VALUE_NAMEDOBJECT				,
		E_VALUE_OBJECT					,
		E_VALUE_OBJECTWITHLOCALPATH	,
		E_VALUE_OBJECTWITHPATH			,
		E_VALUE_REFARRAY					,
		E_VALUE_REFERENCE				,

		E_UNKNOWN
	};


	OW_CIMXMLParser(const OW_String& str);
	OW_CIMXMLParser(std::istream& sb);
	OW_CIMXMLParser();



	OW_String mustGetAttribute(const char* const attrId)
	{
		return getAttribute(attrId, true);
	}

	OW_String getAttribute(const char* const attrId, bool throwIfError = false);

	void getChild();
	void mustGetChild(tokenId tId);
	void mustGetChild();
	void getNextTag(bool throwIfError = false);
	void getNext(tokenId beginTok, bool throwIfError = false);
	void getNext(bool throwIfError = false);
	void mustGetNext()
	{
		getNext(true);
	}

	void mustGetNextTag()
	{
		getNextTag(true);
	}

	void mustGetNext(tokenId beginTok)
	{
		getNext(beginTok, true);
	}

	void mustGetEndTag();

	bool tokenIs(const char* const arg) const
	{
		return m_curTok.text.equals(arg);
	}

	bool tokenIs(tokenId tId) const
	{
		return tokenIs(g_elems[tId].name);
	}

	void mustTokenIs(tokenId tId) const;

	tokenId getToken() const
	{
		return getTokenFromName(m_curTok.text.c_str());
	}


	OW_String getName() const;
	OW_String getData() const;

	bool isData() const;

private:
	OW_AutoPtr<OW_TempFileStream> m_ptfs;
	OW_XMLParser m_parser;
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

	void nextToken();
	void skipData();

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


	static const char* const A_ARRAY_SIZE;
	static const char* const A_ASSOC_CLASS;
	static const char* const A_CLASS_NAME;
	static const char* const A_CLASS_ORIGIN;
	static const char* const A_DEEP_INHERITANCE;
	static const char* const A_INCLUDE_CLASS_ORIGIN;
	static const char* const A_INCLUDE_QUALIFIERS;
	static const char* const A_INSTANCE_NAME;
	static const char* const A_LOCAL_ONLY;
	static const char* const A_MODIFIED_CLASS;
	static const char* const A_NAME;
	static const char* const A_NEW_VALUE;
	static const char* const A_OBJECT_NAME;
	static const char* const A_OVERRIDABLE;
	static const char* const A_PARAMTYPE;
	static const char* const A_PROPAGATED;
	static const char* const A_PROPERTY_LIST;
	static const char* const A_PROPERTY_NAME;
	static const char* const A_REFERENCE_CLASS;
	static const char* const A_RESULT_CLASS;
	static const char* const A_RESULT_ROLE;
	static const char* const A_ROLE;
	static const char* const A_SUPER_CLASS;
	static const char* const A_TOINSTANCE;
	static const char* const A_TOSUBCLASS;
	static const char* const A_TRANSLATABLE;
	static const char* const A_TYPE;
	static const char* const A_VALUE_TYPE;

	static const char* const A_CIMVERSION;
	static const char* const A_DTDVERSION;
	static const char* const A_MSG_ID;
	static const char* const A_PROTOCOLVERSION;
	static const char* const AV_CIMVERSION_VALUE;
	static const char* const AV_DTDVERSION_VALUE;
	static const char* const AV_PROTOCOLVERSION_VALUE;
	static const char* const A_PARAMERRORCODE;
	static const char* const A_PARAMERRORDESCRIPTION;

	friend std::ostream& operator<<(std::ostream& ostr, const OW_CIMXMLParser& p);
};

#endif //#ifndef _OW_CIMXMLPARSER_HPP__





