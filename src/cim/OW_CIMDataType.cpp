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
#include "OW_CIMDataType.hpp"
#include "OW_StringStream.hpp"
#include "OW_Assertion.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_StrictWeakOrdering.hpp"

namespace OpenWBEM
{

using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
struct CIMDataType::DTData
{
	DTData() :
		m_type(CIMDataType::CIMNULL), m_numberOfElements(0), m_sizeRange(0)
	{}
	CIMDataType::Type m_type;
	Int32 m_numberOfElements;
	Int32 m_sizeRange;
	String m_reference;
	DTData* clone() const { return new DTData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMDataType::DTData& x, const CIMDataType::DTData& y)
{
	return StrictWeakOrdering(
		x.m_type, y.m_type,
		x.m_numberOfElements, y.m_numberOfElements,
		x.m_sizeRange, y.m_sizeRange,
		x.m_reference, y.m_reference);
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType() :
	CIMBase(), m_pdata(new DTData)
{
	m_pdata->m_type = CIMNULL;
	m_pdata->m_numberOfElements = 0;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(CIMNULL_t) :
	CIMBase(), m_pdata(NULL)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(CIMDataType::Type type) :
	CIMBase(), m_pdata(new DTData)
{
	OW_ASSERT(type >= CIMNULL && type < MAXDATATYPE);
	m_pdata->m_type = type;
	m_pdata->m_numberOfElements = 1;
	m_pdata->m_sizeRange = SIZE_SINGLE;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(CIMDataType::Type type, Int32 size) :
	CIMBase(), m_pdata(new DTData)
{
	OW_ASSERT(type >= CIMNULL && type < MAXDATATYPE);
	m_pdata->m_type = type;
	m_pdata->m_numberOfElements = (size < 1) ? -1 : size;
	m_pdata->m_sizeRange = m_pdata->m_numberOfElements >= 1 ? SIZE_LIMITED
		: SIZE_UNLIMITED;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(const CIMDataType& arg) :
	CIMBase(), m_pdata(arg.m_pdata) {}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::~CIMDataType()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDataType::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType&
CIMDataType::operator = (const CIMDataType& arg)
{
	m_pdata = arg.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::isArrayType() const
{
	return (m_pdata->m_sizeRange != SIZE_SINGLE);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::isNumericType() const
{
	return isNumericType(m_pdata->m_type);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
CIMDataType::isNumericType(CIMDataType::Type type)
{
	switch(type)
	{
		case UINT8:
		case SINT8:
		case UINT16:
		case SINT16:
		case UINT32:
		case SINT32:
		case UINT64:
		case SINT64:
		case REAL32:
		case REAL64:
			return true;
		default:
			return false;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::isReferenceType() const
{
	return (m_pdata->m_type == REFERENCE);
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::Type
CIMDataType::getType() const
{
	return m_pdata->m_type;
}
//////////////////////////////////////////////////////////////////////////////
Int32
CIMDataType::getSize() const
{
	return m_pdata->m_numberOfElements;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::getRefClassName() const
{
	return m_pdata->m_reference;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::operator CIMDataType::safe_bool () const
{
	safe_bool cc = 0;
	if(!m_pdata.isNull())
	{
		cc = int(m_pdata->m_type != CIMNULL && m_pdata->m_type != INVALID)
			? &dummy::nonnull : 0;
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::safe_bool
CIMDataType::operator!() const
{
	safe_bool cc = &dummy::nonnull;
	if(!m_pdata.isNull())
	{
		cc = int(m_pdata->m_type != CIMNULL && m_pdata->m_type != INVALID)
			? 0: &dummy::nonnull;
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::setToArrayType(Int32 size)
{
	m_pdata->m_numberOfElements = (size < 1) ? -1 : size;
	m_pdata->m_sizeRange = m_pdata->m_numberOfElements >= 1 ? SIZE_LIMITED
		: SIZE_UNLIMITED;
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::syncWithValue(const CIMValue& value)
{
	if(!value && !(*this))
		return false;
	bool rv(false);
	if(m_pdata.isNull())
	{
		m_pdata = new DTData;
		m_pdata->m_type = CIMNULL;
	}
	if(!value)
	{
		m_pdata->m_type = CIMNULL;
		m_pdata->m_numberOfElements = 0;
		m_pdata->m_sizeRange = SIZE_SINGLE;
		rv = true;
	}
	else
	{
		if((m_pdata->m_type != value.getType())
			|| (isArrayType() != value.isArray()))
		{
			m_pdata->m_type = value.getType();
			m_pdata->m_sizeRange = (value.isArray()) ? SIZE_UNLIMITED : SIZE_SINGLE;
			m_pdata->m_numberOfElements = (m_pdata->m_sizeRange == SIZE_UNLIMITED)
				? -1 : 1;
			rv = true;
		}
	}
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
CIMDataType::CIMDataType(const String& refClassName) :
	CIMBase(), m_pdata(new DTData)
{
	m_pdata->m_type = REFERENCE;
	m_pdata->m_numberOfElements = 1;
	m_pdata->m_sizeRange = SIZE_SINGLE;
	m_pdata->m_reference = refClassName;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMDataType::equals(const CIMDataType& arg) const
{
	return (m_pdata->m_type == arg.m_pdata->m_type
		&& m_pdata->m_sizeRange == arg.m_pdata->m_sizeRange);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDataType::readObject(istream &istrm)
{
	UInt32 type;
	UInt32 numberOfElements;
	UInt32 sizeRange;
	String ref;
	CIMBase::readSig( istrm, OW_CIMDATATYPESIG );
	BinarySerialization::readLen(istrm, type);
	BinarySerialization::readLen(istrm, numberOfElements);
	BinarySerialization::readLen(istrm, sizeRange);
	ref.readObject(istrm);
	if(m_pdata.isNull())
	{
		m_pdata = new DTData;
	}
	m_pdata->m_type = CIMDataType::Type(type);
	m_pdata->m_numberOfElements = numberOfElements;
	m_pdata->m_sizeRange = sizeRange;
	m_pdata->m_reference = ref;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMDataType::writeObject(ostream &ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMDATATYPESIG );
	BinarySerialization::writeLen(ostrm, m_pdata->m_type);
	BinarySerialization::writeLen(ostrm, m_pdata->m_numberOfElements);
	BinarySerialization::writeLen(ostrm, m_pdata->m_sizeRange);
	m_pdata->m_reference.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::toString() const
{
	switch(m_pdata->m_type)
	{
		case UINT8: return "uint8"; break;
		case SINT8: return "sint8"; break;
		case UINT16: return "uint16"; break;
		case SINT16: return "sint16"; break;
		case UINT32: return "uint32"; break;
		case SINT32: return "sint32"; break;
		case UINT64: return "uint64"; break;
		case SINT64: return "sint64"; break;
		case REAL64: return "real64"; break;
		case REAL32: return "real32"; break;
		case CHAR16: return "char16"; break;
		case STRING: return "string"; break;
		case BOOLEAN: return "boolean"; break;
		case DATETIME: return "datetime"; break;
		case REFERENCE: return "REF"; break;
		case EMBEDDEDCLASS: case EMBEDDEDINSTANCE: return "string"; break;
		default:
			return "** INVALID DATA TYPE IN CIMDATATYPE - toString **";
	}
}
//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::toMOF() const
{
	if (m_pdata->m_type == REFERENCE)
	{
		return m_pdata->m_reference + " REF";
	}
	else
	{
		return toString();
	}
}

//////////////////////////////////////////////////////////////////////////////
String
CIMDataType::getArrayMOF() const
{
	if (!isArrayType())
	{
		return String();
	}
	if (m_pdata->m_sizeRange == SIZE_UNLIMITED)
	{
		return "[]";
	}
	StringBuffer rv("[");
	rv += getSize();
	rv += ']';
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMDataType::Type
CIMDataType::strToSimpleType(const String& strType)
{
// If you want to know what this function does, here's the old slow way it
// used to be written.
#if 0
	if(strType_.empty())
	{
		return INVALID;
	}
	OW_String strType(strType_);
	strType.toLowerCase();
	if(strType == "uint8")
		return UINT8;
	else if(strType == "sint8")
		return SINT8;
	else if(strType == "uint16")
		return UINT16;
	else if(strType == "sint16")
		return SINT16;
	else if(strType == "uint32")
		return UINT32;
	else if(strType == "sint32")
		return SINT32;
	else if(strType == "uint64")
		return UINT64;
	else if(strType == "sint64")
		return SINT64;
	else if(strType == "real64")
		return REAL64;
	else if(strType == "real32")
		return REAL32;
	else if(strType == "char16")
		return CHAR16;
	else if(strType == "string")
		return STRING;
	else if(strType == "boolean")
		return BOOLEAN;
	else if(strType == "datetime")
		return DATETIME;
	else if(strType == "ref")
		return REFERENCE;
	else if(strType == "reference")
		return REFERENCE;
	return INVALID;
#endif
// This function is heavily used, and needs to be as fast as possible, so it's
// been re-written to use a lexer fsm.  The code was generated by re2c and
// subsequently modified to not dereference the char one past the terminating
// '\0' of the string (the commented out lines, if you care).
// Yes, I know this is a maintenance nightmare, and if you ever need to change
// this function it'll be a big PITA, but at the moment, I don't think this
// function is really ever going to need to be modified, and it's quite a
// hotspot performance wise, so I think the change will be worth it.
// It's generated from the following re2c input:
#if 0
	#define YYCTYPE char
	#define YYCURSOR		begin
	#define YYLIMIT		 end
	#define YYMARKER		q
	#define YYFILL(n)

start:
	/*!re2c
	END = [\000];
	any = [\000-\377];
		[uU][iI][nN][tT]"8"END { return UINT8; }
		[sS][iI][nN][tT]"8"END { return SINT8; }
		[uU][iI][nN][tT]"16"END { return UINT16; }
		[sS][iI][nN][tT]"16"END { return SINT16; }
		[uU][iI][nN][tT]"32"END { return UINT32; }
		[sS][iI][nN][tT]"32"END { return SINT32; }
		[uU][iI][nN][tT]"64"END { return UINT64; }
		[sS][iI][nN][tT]"64"END { return SINT64; }
		[rR][eE][aA][lL]"32"END { return REAL32; }
		[rR][eE][aA][lL]"64"END { return REAL64; }
		[cC][hH][aA][rR]"16"END { return CHAR16; }
		[sS][tT][rR][iI][nN][gG]END { return STRING; }
		[bB][oO][oO][lL][eE][aA][nN]END { return BOOLEAN; }
		[dD][aA][tT][eE][tT][iI][mM][eE]END { return DATETIME; }
		[rR][eE][fF]END { return REFERENCE; }
		[rR][eE][fF][eE][rR][eE][nN][cC][eE]END { return REFERENCE; }
		any { return INVALID; }
	*/
#endif

	const char* begin = strType.c_str();
	const char* q;

	#define YYCTYPE char
	#define YYCURSOR		begin
	#define YYLIMIT		 begin
	#define YYMARKER		q
	#define YYFILL(n)

start:
	{
	YYCTYPE yych;
	unsigned int yyaccept;
	goto yy0;
yy1:	++YYCURSOR;
yy0:
	if((YYLIMIT - YYCURSOR) < 10) YYFILL(10);
	yych = *YYCURSOR;
	switch(yych){
	case 'B':	case 'b':	goto yy7;
	case 'C':	case 'c':	goto yy6;
	case 'D':	case 'd':	goto yy8;
	case 'R':	case 'r':	goto yy5;
	case 'S':	case 's':	goto yy4;
	case 'U':	case 'u':	goto yy2;
	default:	goto yy9;
	}
yy2:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch(yych){
	case 'I':	case 'i':	goto yy82;
	default:	goto yy3;
	}
yy3:
	{ return INVALID; }
yy4:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch(yych){
	case 'I':	case 'i':	goto yy58;
	case 'T':	case 't':	goto yy57;
	default:	goto yy3;
	}
yy5:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch(yych){
	case 'E':	case 'e':	goto yy35;
	default:	goto yy3;
	}
yy6:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch(yych){
	case 'H':	case 'h':	goto yy28;
	default:	goto yy3;
	}
yy7:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch(yych){
	case 'O':	case 'o':	goto yy20;
	default:	goto yy3;
	}
yy8:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch(yych){
	case 'A':	case 'a':	goto yy10;
	default:	goto yy3;
	}
yy9:	//yych = *++YYCURSOR;
	goto yy3;
yy10:	yych = *++YYCURSOR;
	switch(yych){
	case 'T':	case 't':	goto yy12;
	default:	goto yy11;
	}
yy11:	YYCURSOR = YYMARKER;
	switch(yyaccept){
	case 0:	goto yy3;
	}
yy12:	yych = *++YYCURSOR;
	switch(yych){
	case 'E':	case 'e':	goto yy13;
	default:	goto yy11;
	}
yy13:	yych = *++YYCURSOR;
	switch(yych){
	case 'T':	case 't':	goto yy14;
	default:	goto yy11;
	}
yy14:	yych = *++YYCURSOR;
	switch(yych){
	case 'I':	case 'i':	goto yy15;
	default:	goto yy11;
	}
yy15:	yych = *++YYCURSOR;
	switch(yych){
	case 'M':	case 'm':	goto yy16;
	default:	goto yy11;
	}
yy16:	yych = *++YYCURSOR;
	switch(yych){
	case 'E':	case 'e':	goto yy17;
	default:	goto yy11;
	}
yy17:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy18:	//yych = *++YYCURSOR;
yy19:
	{ return DATETIME; }
yy20:	yych = *++YYCURSOR;
	switch(yych){
	case 'O':	case 'o':	goto yy21;
	default:	goto yy11;
	}
yy21:	yych = *++YYCURSOR;
	switch(yych){
	case 'L':	case 'l':	goto yy22;
	default:	goto yy11;
	}
yy22:	yych = *++YYCURSOR;
	switch(yych){
	case 'E':	case 'e':	goto yy23;
	default:	goto yy11;
	}
yy23:	yych = *++YYCURSOR;
	switch(yych){
	case 'A':	case 'a':	goto yy24;
	default:	goto yy11;
	}
yy24:	yych = *++YYCURSOR;
	switch(yych){
	case 'N':	case 'n':	goto yy25;
	default:	goto yy11;
	}
yy25:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy26:	//yych = *++YYCURSOR;
yy27:
	{ return BOOLEAN; }
yy28:	yych = *++YYCURSOR;
	switch(yych){
	case 'A':	case 'a':	goto yy29;
	default:	goto yy11;
	}
yy29:	yych = *++YYCURSOR;
	switch(yych){
	case 'R':	case 'r':	goto yy30;
	default:	goto yy11;
	}
yy30:	yych = *++YYCURSOR;
	switch(yych){
	case '1':	goto yy31;
	default:	goto yy11;
	}
yy31:	yych = *++YYCURSOR;
	switch(yych){
	case '6':	goto yy32;
	default:	goto yy11;
	}
yy32:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy33:	//yych = *++YYCURSOR;
yy34:
	{ return CHAR16; }
yy35:	yych = *++YYCURSOR;
	switch(yych){
	case 'A':	case 'a':	goto yy36;
	case 'F':	case 'f':	goto yy37;
	default:	goto yy11;
	}
yy36:	yych = *++YYCURSOR;
	switch(yych){
	case 'L':	case 'l':	goto yy48;
	default:	goto yy11;
	}
yy37:	yych = *++YYCURSOR;
	switch(yych){
	case '\000':	goto yy39;
	case 'E':	case 'e':	goto yy38;
	default:	goto yy11;
	}
yy38:	yych = *++YYCURSOR;
	switch(yych){
	case 'R':	case 'r':	goto yy41;
	default:	goto yy11;
	}
yy39:	//yych = *++YYCURSOR;
yy40:
	{ return REFERENCE; }
yy41:	yych = *++YYCURSOR;
	switch(yych){
	case 'E':	case 'e':	goto yy42;
	default:	goto yy11;
	}
yy42:	yych = *++YYCURSOR;
	switch(yych){
	case 'N':	case 'n':	goto yy43;
	default:	goto yy11;
	}
yy43:	yych = *++YYCURSOR;
	switch(yych){
	case 'C':	case 'c':	goto yy44;
	default:	goto yy11;
	}
yy44:	yych = *++YYCURSOR;
	switch(yych){
	case 'E':	case 'e':	goto yy45;
	default:	goto yy11;
	}
yy45:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy46:	//yych = *++YYCURSOR;
yy47:
	{ return REFERENCE; }
yy48:	yych = *++YYCURSOR;
	switch(yych){
	case '3':	goto yy49;
	case '6':	goto yy50;
	default:	goto yy11;
	}
yy49:	yych = *++YYCURSOR;
	switch(yych){
	case '2':	goto yy54;
	default:	goto yy11;
	}
yy50:	yych = *++YYCURSOR;
	switch(yych){
	case '4':	goto yy51;
	default:	goto yy11;
	}
yy51:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy52:	//yych = *++YYCURSOR;
yy53:
	{ return REAL64; }
yy54:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy55:	//yych = *++YYCURSOR;
yy56:
	{ return REAL32; }
yy57:	yych = *++YYCURSOR;
	switch(yych){
	case 'R':	case 'r':	goto yy76;
	default:	goto yy11;
	}
yy58:	yych = *++YYCURSOR;
	switch(yych){
	case 'N':	case 'n':	goto yy59;
	default:	goto yy11;
	}
yy59:	yych = *++YYCURSOR;
	switch(yych){
	case 'T':	case 't':	goto yy60;
	default:	goto yy11;
	}
yy60:	yych = *++YYCURSOR;
	switch(yych){
	case '1':	goto yy62;
	case '3':	goto yy63;
	case '6':	goto yy64;
	case '8':	goto yy61;
	default:	goto yy11;
	}
yy61:	yych = *++YYCURSOR;
	if(yych <= '\000')	goto yy74;
	goto yy11;
yy62:	yych = *++YYCURSOR;
	switch(yych){
	case '6':	goto yy71;
	default:	goto yy11;
	}
yy63:	yych = *++YYCURSOR;
	switch(yych){
	case '2':	goto yy68;
	default:	goto yy11;
	}
yy64:	yych = *++YYCURSOR;
	switch(yych){
	case '4':	goto yy65;
	default:	goto yy11;
	}
yy65:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy66:	//yych = *++YYCURSOR;
yy67:
	{ return SINT64; }
yy68:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy69:	//yych = *++YYCURSOR;
yy70:
	{ return SINT32; }
yy71:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy72:	//yych = *++YYCURSOR;
yy73:
	{ return SINT16; }
yy74:	//yych = *++YYCURSOR;
yy75:
	{ return SINT8; }
yy76:	yych = *++YYCURSOR;
	switch(yych){
	case 'I':	case 'i':	goto yy77;
	default:	goto yy11;
	}
yy77:	yych = *++YYCURSOR;
	switch(yych){
	case 'N':	case 'n':	goto yy78;
	default:	goto yy11;
	}
yy78:	yych = *++YYCURSOR;
	switch(yych){
	case 'G':	case 'g':	goto yy79;
	default:	goto yy11;
	}
yy79:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy80:	//yych = *++YYCURSOR;
yy81:
	{ return STRING; }
yy82:	yych = *++YYCURSOR;
	switch(yych){
	case 'N':	case 'n':	goto yy83;
	default:	goto yy11;
	}
yy83:	yych = *++YYCURSOR;
	switch(yych){
	case 'T':	case 't':	goto yy84;
	default:	goto yy11;
	}
yy84:	yych = *++YYCURSOR;
	switch(yych){
	case '1':	goto yy86;
	case '3':	goto yy87;
	case '6':	goto yy88;
	case '8':	goto yy85;
	default:	goto yy11;
	}
yy85:	yych = *++YYCURSOR;
	if(yych <= '\000')	goto yy98;
	goto yy11;
yy86:	yych = *++YYCURSOR;
	switch(yych){
	case '6':	goto yy95;
	default:	goto yy11;
	}
yy87:	yych = *++YYCURSOR;
	switch(yych){
	case '2':	goto yy92;
	default:	goto yy11;
	}
yy88:	yych = *++YYCURSOR;
	switch(yych){
	case '4':	goto yy89;
	default:	goto yy11;
	}
yy89:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy90:	//yych = *++YYCURSOR;
yy91:
	{ return UINT64; }
yy92:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy93:	//yych = *++YYCURSOR;
yy94:
	{ return UINT32; }
yy95:	yych = *++YYCURSOR;
	if(yych >= '\001')	goto yy11;
yy96:	//yych = *++YYCURSOR;
yy97:
	{ return UINT16; }
yy98:	//yych = *++YYCURSOR;
yy99:
	{ return UINT8; }
}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMDataType
CIMDataType::getDataType(const String& strType)
{
	if(strType.empty())
	{
		return CIMDataType();
	}
	Type type = strToSimpleType(strType);
	if(type != INVALID)
	{
		return CIMDataType(type);
	}
	return CIMDataType(CIMNULL);
}
bool operator<(const CIMDataType& x, const CIMDataType& y)
{
	return *x.m_pdata < *y.m_pdata;
}

} // end namespace OpenWBEM

