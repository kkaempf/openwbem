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
#ifndef OW_CIMXMLPARSER_HPP_INCLUDE_GUARD_
#define OW_CIMXMLPARSER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_AutoPtr.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_String.hpp"
#include "OW_XMLParserCore.hpp"
#ifdef OW_HAVE_ISTREAM
#include <istream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

class CIMXMLParser
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
	CIMXMLParser(const String& str);
	CIMXMLParser(std::istream& sb);
	CIMXMLParser();
	String mustGetAttribute(const char* const attrId)
	{
		return getAttribute(attrId, true);
	}
	String getAttribute(const char* const attrId, bool throwIfError = false);
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
	String getName() const;
	String getData() const;
	bool isData() const;
private:
	AutoPtr<TempFileStream> m_ptfs;
	XMLParserCore m_parser;
	XMLToken m_curTok;
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
	CIMXMLParser(const CIMXMLParser& x);
	CIMXMLParser& operator=(const CIMXMLParser& x);
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

	// naming convention:
	// A_ for attribute name
	// AV_ for attribute value (expected)
	// P_ for {I,E}PARAMVALUE NAME attribute value
	static const char* const A_ASSOCCLASS;
	static const char* const A_CLASSNAME;
	static const char* const A_CLASSORIGIN;
	static const char* const A_DEEPINHERITANCE;
	static const char* const A_INCLUDECLASSORIGIN;
	static const char* const A_INCLUDEQUALIFIERS;
	static const char* const A_INSTANCENAME;
	static const char* const A_LOCALONLY;
	static const char* const A_MODIFIEDCLASS;
	static const char* const A_NAME;
	static const char* const A_NEWVALUE;
	static const char* const A_OBJECTNAME;
	static const char* const A_OVERRIDABLE;
	static const char* const A_PARAMTYPE;
	static const char* const A_PROPAGATED;
	static const char* const A_PROPERTYLIST;
	static const char* const A_PROPERTYNAME;
	static const char* const A_REFERENCECLASS;
	static const char* const A_RESULTCLASS;
	static const char* const A_RESULTROLE;
	static const char* const A_ROLE;
	static const char* const A_SUPERCLASS;
	static const char* const A_TOINSTANCE; // This is a bug in the spec, but we still support it for backward compatibility.
	static const char* const A_TOSUBCLASS;
	static const char* const A_TRANSLATABLE;
	static const char* const A_TYPE;
	static const char* const A_VALUETYPE;
	static const char* const A_CIMVERSION;
	static const char* const A_DTDVERSION;
	static const char* const A_ID;
	static const char* const A_PROTOCOLVERSION;
	static const char* const A_CODE;
	static const char* const A_DESCRIPTION;
	static const char* const A_ARRAYSIZE;
	static const char* const A_ISARRAY;
	static const char* const AV_CIMVERSION20_VALUE;
	static const char* const AV_CIMVERSION21_VALUE;
	static const char* const AV_CIMVERSION22_VALUE;
	static const char* const AV_DTDVERSION20_VALUE;
	static const char* const AV_DTDVERSION21_VALUE;
	static const char* const AV_PROTOCOLVERSION10_VALUE;
	static const char* const AV_PROTOCOLVERSION11_VALUE;
	static const char* const P_ClassName;
	static const char* const P_PropertyList;
	static const char* const P_DeepInheritance;
	static const char* const P_LocalOnly;
	static const char* const P_IncludeQualifiers;
	static const char* const P_IncludeClassOrigin;
	static const char* const P_ModifiedClass;
	static const char* const P_ModifiedInstance;
	static const char* const P_InstanceName;
	static const char* const P_PropertyName;
	static const char* const P_NewValue;
	static const char* const P_Role;
	static const char* const P_ResultRole;
	static const char* const P_ObjectName;
	static const char* const P_AssocClass;
	static const char* const P_ResultClass;
	static const char* const P_QueryLanguage;
	static const char* const P_Query;
	static const char* const P_QualifierName;
	static const char* const P_QualifierDeclaration;


	friend std::ostream& operator<<(std::ostream& ostr, const CIMXMLParser& p);
};

} // end namespace OpenWBEM

typedef OpenWBEM::CIMXMLParser OW_CIMXMLParser;

#endif
