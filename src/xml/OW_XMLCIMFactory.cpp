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
#include "OW_config.h"
#include "OW_XMLCIMFactory.hpp"
#include "OW_String.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_XMLClass.hpp"
#include "OW_Format.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_Bool.hpp"
#include <algorithm> // for find_if

namespace OpenWBEM
{

static void
getLocalNameSpacePathAndSet(CIMObjectPath& cop, CIMXMLParser& parser)
{
	// <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
	if (!parser.tokenIs(CIMXMLParser::E_LOCALNAMESPACEPATH))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Expected <LOCALNAMESPACEPATH>");
	}
	parser.mustGetChild(CIMXMLParser::E_NAMESPACE);
	// <!ELEMENT NAMESPACE EMPTY>
	// <!ATTLIST NAMESPACE %CIMName;>
	String ns;
	while(parser.tokenIs(CIMXMLParser::E_NAMESPACE))	
	{
		String nscomp = parser.mustGetAttribute(CIMXMLParser::A_NAME);
		if(!nscomp.empty())
		{
			if(!ns.empty() )
			{
				ns += "/";
			}
			ns += nscomp;
		}
		parser.mustGetNextTag();
		parser.mustGetEndTag(); // pass </NAMESPACE>
	}
	if(ns.empty())
	{
		ns = "root";
	}
	cop.setNameSpace(ns);
	parser.mustGetEndTag(); // pass </LOCALNAMESPACEPATH>
}
//////////////////////////////////////////////////////////////////////////////
static void
getNameSpacePathAndSet(CIMObjectPath& cop, CIMXMLParser& parser)
{
	// <!ELEMENT NAMESPACEPATH (HOST,LOCALNAMESPACEPATH)>
	// <!ELEMENT HOST (#PCDATA)>
	parser.mustGetChild(CIMXMLParser::E_HOST);
	parser.mustGetNext();
	if (!parser.isData())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "invalid <HOST> in <NAMESPACEPATH>");
	}
	cop.setHost(parser.getData());
	parser.mustGetNextTag();
	parser.mustGetEndTag(); // pass </HOST>
	getLocalNameSpacePathAndSet(cop, parser);
	parser.mustGetEndTag(); // pass </NAMESPACEPATH>
}
static void getKeyValue(CIMXMLParser& parser, CIMValue& value)
{
	String valuetype = parser.mustGetAttribute(CIMXMLParser::A_VALUE_TYPE);
	parser.mustGetNext();
	if (!parser.isData())
	{
		value = CIMValue("");
	}
	else
	{
		value = CIMValue(parser.getData());
		parser.mustGetNextTag();
	}
	parser.mustGetEndTag();
	// cast the value to the correct type, if not a string
	if (valuetype == "boolean")
	{
		value = CIMValueCast::castValueToDataType(value,CIMDataType::BOOLEAN);
	}
	else if (valuetype == "numeric")
	{
		try
		{
			value = CIMValueCast::castValueToDataType(value,CIMDataType::SINT64);
		}
		catch (const CIMException& e)
		{
			value = CIMValueCast::castValueToDataType(value,CIMDataType::REAL64);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
static void getInstanceName(CIMXMLParser& parser, CIMObjectPath& cimPath)
{
	// <!ELEMENT INSTANCENAME (KEYBINDING*|KEYVALUE?|VALUE.REFERENCE?)>
	// <!ATTLIST INSTANCENAME %ClassName;>
	CIMPropertyArray propertyArray;
	CIMProperty cp(CIMNULL);
	OW_ASSERT(parser.tokenIs(CIMXMLParser::E_INSTANCENAME));
	String thisClassName = parser.getAttribute(CIMXMLParser::A_CLASS_NAME);
	cimPath.setObjectName(thisClassName);
	//parser.getChild();
	parser.getNextTag();
	if (parser.tokenIs(CIMXMLParser::E_KEYBINDING))
	{
		do
		{
			// <!ELEMENT KEYBINDING (KEYVALUE|VALUE.REFERENCE)>
			// <!ATTLIST KEYBINDING %CIMName;>
			//
			// <!ELEMENT KEYVALUE (#PCDATA)>
			// <!ATTLIST KEYVALUE
			//          VALUETYPE    (string|boolean|numeric)  'string'>
			CIMValue value(CIMNULL);
			String name;
			CIMXMLParser keyval;
			name = parser.mustGetAttribute(CIMXMLParser::A_NAME);
			parser.mustGetChild();
			switch(parser.getToken())
			{
				case CIMXMLParser::E_KEYVALUE:
					getKeyValue(parser,value);
					break;
				case CIMXMLParser::E_VALUE_REFERENCE:
					value = XMLCIMFactory::createValue(parser, "REF");
					break;
				default:
					OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
						"Not a valid instance declaration");
			}
			cp = CIMProperty(name, value);
			propertyArray.push_back(cp);
			parser.mustGetEndTag(); // pass </KEYBINDING>
		} while (parser.tokenIs(CIMXMLParser::E_KEYBINDING));
	}
	else if (parser.tokenIs(CIMXMLParser::E_KEYVALUE))
	{
		CIMValue value(CIMNULL);
		cp = CIMProperty();
		getKeyValue(parser,value);
		cp.setDataType(value.getCIMDataType());
		cp.setValue(value);
		propertyArray.push_back(cp);
	}
	else if (parser.tokenIs(CIMXMLParser::E_VALUE_REFERENCE))
	{
		CIMValue value = XMLCIMFactory::createValue(parser, "REF");
		cp = CIMProperty();
		cp.setDataType(CIMDataType::REFERENCE);
		cp.setValue(value);
		propertyArray.push_back(cp);
	}
	// No. Any of the sub-elements are optional.  If none are found, then the
	// path is to a singleton.
	//else
	//{
	//	OW_THROWCIMMSG(CIMException::FAILED,
	//		"not a valid instance declaration");
	//}
	parser.mustGetEndTag();
	cimPath.setKeys(propertyArray);
}
///////////////////////////////////
CIMObjectPath
XMLCIMFactory::createObjectPath(CIMXMLParser& parser)
{
	CIMObjectPath rval;
	int token = parser.getToken();
	switch(token)
	{
		case CIMXMLParser::E_OBJECTPATH:
			parser.mustGetChild();
			rval = createObjectPath(parser);
			parser.mustGetEndTag(); // pass </OBJECTPATH>
			return rval;
		case CIMXMLParser::E_LOCALCLASSPATH:
			parser.mustGetChild(CIMXMLParser::E_LOCALNAMESPACEPATH);
			getLocalNameSpacePathAndSet(rval, parser);
			parser.mustGetNext(CIMXMLParser::E_CLASSNAME);
			rval.setObjectName(parser.mustGetAttribute(CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			parser.mustGetEndTag(); // pass </LOCALCLASSPATH>
			return rval;
		case CIMXMLParser::E_CLASSPATH:
			parser.mustGetChild(CIMXMLParser::E_NAMESPACEPATH);
			getNameSpacePathAndSet(rval, parser);
			parser.mustGetNext(CIMXMLParser::E_CLASSNAME);
			rval.setObjectName(parser.mustGetAttribute(CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			parser.mustGetEndTag(); // pass </LOCALCLASSPATH>
			return rval;
		case CIMXMLParser::E_CLASSNAME:
			rval.setObjectName(parser.mustGetAttribute(CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			return rval;
		case CIMXMLParser::E_INSTANCENAME:
			getInstanceName(parser, rval);
			return rval;
		case CIMXMLParser::E_LOCALINSTANCEPATH:
			parser.mustGetChild(CIMXMLParser::E_LOCALNAMESPACEPATH);
			getLocalNameSpacePathAndSet(rval, parser);
			break;
		case CIMXMLParser::E_INSTANCEPATH:
			parser.mustGetChild(CIMXMLParser::E_NAMESPACEPATH);
			getNameSpacePathAndSet(rval, parser);
			break;
		default:
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					format("Invalid XML for Object path construction.  Node "
						"name = %1", parser.getName()).c_str());
				break;
	}
	// read <INSTANCENAME>
	getInstanceName(parser, rval);
	parser.mustGetEndTag(); // pass </LOCALINSTANCEPATH> or </INSTANCEPATH>
	return rval;
}
///////////////////////////////////
CIMClass
XMLCIMFactory::createClass(CIMXMLParser& parser)
{
	CIMClass rval;
	String superClassName;
	
	if (!parser.tokenIs(CIMXMLParser::E_CLASS))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not class XML");
	}
	
	String inClassName = parser.mustGetAttribute(XMLParameters::paramName);
	rval.setName(inClassName);
	superClassName = parser.getAttribute(XMLParameters::paramSuperName);
	if(!superClassName.empty())
	{
		rval.setSuperClass(superClassName);
	}
	//
	// Find qualifier information
	//
	parser.mustGetNextTag();
	while (parser.tokenIs(CIMXMLParser::E_QUALIFIER))
	{
		CIMQualifier cq = createQualifier(parser);
//         if(cq.getName().equalsIgnoreCase(CIMQualifier::CIM_QUAL_ASSOCIATION))
//         {
//             if (!cq.getValue()
//                 || cq.getValue() != CIMValue(false))
//             {
//                 rval.setIsAssociation(true);
//             }
//         }
		rval.addQualifier(cq);
	}
	//
	// Load properties
	//
	while (parser.tokenIs(CIMXMLParser::E_PROPERTY)
		   || parser.tokenIs(CIMXMLParser::E_PROPERTY_ARRAY)
		   || parser.tokenIs(CIMXMLParser::E_PROPERTY_REFERENCE))
	{
		rval.addProperty(createProperty(parser));
	}
	//
	// Load methods
	//
	while (parser.tokenIs(CIMXMLParser::E_METHOD))
	{
		rval.addMethod(createMethod(parser));
	}
	parser.mustGetEndTag();
	return rval;
}
///////////////////////////////////
CIMInstance
XMLCIMFactory::createInstance(CIMXMLParser& parser)
{
	CIMInstance rval;
	if (!parser.tokenIs(CIMXMLParser::E_INSTANCE))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not instance XML");
	}
		
	rval.setClassName(parser.mustGetAttribute(CIMXMLParser::A_CLASS_NAME));
	//
	// Find qualifier information
	//
	CIMQualifierArray quals;
	parser.getChild();
	while (parser.tokenIs(CIMXMLParser::E_QUALIFIER))
	{
		quals.append(createQualifier(parser));
	}
	rval.setQualifiers(quals);
	//
	// Load properties
	//
	CIMPropertyArray props;
	while (parser.tokenIs(CIMXMLParser::E_PROPERTY)
		   || parser.tokenIs(CIMXMLParser::E_PROPERTY_ARRAY)
		   || parser.tokenIs(CIMXMLParser::E_PROPERTY_REFERENCE))
	{
		props.append(createProperty(parser));
	}
	rval.setProperties(props);
	parser.mustGetEndTag();
	return rval;
}
static inline void StringToType(const String& s, UInt8& b)
{
	b = s.toUInt8();
}
static inline void StringToType(const String& s, Int8& b)
{
	b = s.toInt8();
}
static inline void StringToType(const String& s, UInt16& b)
{
	b = s.toUInt16();
}
static inline void StringToType(const String& s, Int16& b)
{
	b = s.toInt16();
}
static inline void StringToType(const String& s, UInt32& b)
{
	b = s.toUInt32();
}
static inline void StringToType(const String& s, Int32& b)
{
	b = s.toInt32();
}
static inline void StringToType(const String& s, UInt64& b)
{
	b = s.toUInt64();
}
static inline void StringToType(const String& s, Int64& b)
{
	b = s.toInt64();
}
static inline void StringToType(const String& s, String& b)
{
	b = s;
}
static inline void StringToType(const String& s, Real32& b)
{
	b = s.toReal32();
}
static inline void StringToType(const String& s, Real64& b)
{
	b = s.toReal64();
}
static inline void StringToType(const String& s, Char16& b)
{
	b = s.toChar16();
}
static inline void StringToType(const String& s, CIMDateTime& b)
{
	b = s.toDateTime();
}
///////////////////////////////////
template <class T>
static inline void
convertCimType(Array<T>& ra, CIMXMLParser& parser)
{
	// start out possibly pointing at <VALUE>
	while(parser.tokenIs(CIMXMLParser::E_VALUE))
	{
		parser.mustGetNext();
		if (parser.isData())
		{
			String vstr = parser.getData();
			T val;
			StringToType(vstr, val);
			ra.append(val);
			parser.mustGetNextTag();
		}
		else
		{
			T val;
			StringToType("", val);
			ra.append(val);
		}
		parser.mustGetEndTag();
	}
}
///////////////////////////////////
CIMValue
XMLCIMFactory::createValue(CIMXMLParser& parser,
	String const& valueType)
{
	CIMValue rval(CIMNULL);
	try
	{
	
		int token = parser.getToken();
	
		switch(token)
		{
			// <VALUE> elements
			case CIMXMLParser::E_VALUE:
				{
					parser.mustGetNext();
					if (parser.isData())
					{
						String vstr = parser.getData();
						rval = CIMValue::createSimpleValue(valueType, vstr);
						parser.mustGetNextTag(); // pass text
					}
					else
					{
						rval = CIMValue::createSimpleValue(valueType, "");
					}
					parser.mustGetEndTag(); // get </VALUE>
					break;
				}
	
			// <VALUE.ARRAY> elements
			case CIMXMLParser::E_VALUE_ARRAY:
				{
					int type = CIMDataType::strToSimpleType(valueType);
					if(type == CIMDataType::INVALID)
					{
						OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							"Invalid data type on node");
					}
					parser.mustGetNextTag();
	
					switch(type)
					{
						case CIMDataType::UINT8:
						{
							UInt8Array ra;
							convertCimType(ra, parser);
							rval = CIMValue(ra);
							break;
						}
	
						case CIMDataType::SINT8:
							{
								Int8Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::UINT16:
							{
								UInt16Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::SINT16:
							{
								Int16Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::UINT32:
							{
								UInt32Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::SINT32:
							{
								Int32Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::UINT64:
							{
								UInt64Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::SINT64:
							{
								Int64Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::BOOLEAN:
							{
								BoolArray ra;
								StringArray sra;
								convertCimType(sra, parser);
								for(size_t i = 0; i < sra.size(); i++)
								{
									Bool bv = sra[i].equalsIgnoreCase("TRUE");
									ra.append(bv);
								}
	
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::REAL32:
							{
								Real32Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::REAL64:
							{
								Real64Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::CHAR16:
							{
								Char16Array ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::DATETIME:
							{
								CIMDateTimeArray ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						case CIMDataType::STRING:
							{
								StringArray ra;
								convertCimType(ra, parser);
								rval = CIMValue(ra);
								break;
							}
	
						default:
							OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
								"Invalid data type on node");
					}
	
					parser.mustGetEndTag(); // pass </VALUE.ARRAY>
					break;
				}
	
			case CIMXMLParser::E_VALUE_REFARRAY:
				{
					CIMObjectPathArray opArray;
					parser.getNextTag();
	
					while(parser.tokenIs(CIMXMLParser::E_VALUE_REFERENCE))
					{
						CIMObjectPath cop(CIMNULL);
						CIMValue v = createValue(parser, valueType);
						v.get(cop);
	
						opArray.append(cop);
					}
	
					rval = CIMValue(opArray);
					parser.mustGetEndTag(); // pass <VALUE.REFARRAY>
					break;
				}
	
			case CIMXMLParser::E_VALUE_REFERENCE:
				{
					parser.mustGetChild();
					CIMObjectPath cop = createObjectPath(parser);
					parser.mustGetEndTag(); // pass </VALUE.REFERENCE>
					rval = CIMValue(cop);
					break;
				}
	
			default:
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					"Not value XML");
		}
	
	}
	catch (const StringConversionException& e)
	{
		// Workaround for SNIA client which sends <VALUE>NULL</VALUE> for NULL values
		if (parser.isData())
		{
			if (parser.getData().equalsIgnoreCase("NULL"))
			{
				rval = CIMValue(CIMNULL);
				parser.getNextTag();
				parser.mustGetEndTag();
			}
			else
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, e.getMessage());
			}
		}
		else
		{
			rval = CIMValue(CIMNULL);
			parser.mustGetEndTag();
		}
	}
	return rval;
}
///////////////////////////////////
CIMQualifier
XMLCIMFactory::createQualifier(CIMXMLParser& parser)
{
	if (!parser.tokenIs(CIMXMLParser::E_QUALIFIER))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not qualifier XML");
	}
	CIMDataType dt(CIMNULL);
	String name = parser.mustGetAttribute(XMLParameters::paramName);
	String cimType = parser.getAttribute(XMLParameters::paramTypeAssign);
	String propagate = parser.getAttribute(XMLParameters::paramPropagated);
	String tosubclass = parser.getAttribute(XMLParameters::paramToSubClass);
	String overridable = parser.getAttribute(
		XMLParameters::paramOverridable);
	//String toinstance = parser.getAttribute(
	//	XMLParameters::paramToInstance);
	String translatable = parser.getAttribute(
		XMLParameters::paramTranslatable);
	//
	// Build qualifier
	//
	if(!cimType.empty())
	{
		dt = CIMDataType::getDataType(cimType);
	}
	if(!dt)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			format("Qualifier not assigned a data type: %1", name).c_str());
	}
	CIMQualifierType cqt;
	cqt.setDataType(dt);
	cqt.setName(name);
	
	CIMQualifier rval(cqt);
	if(overridable.equalsIgnoreCase("false"))
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::DISABLEOVERRIDE));
	}
	else
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::ENABLEOVERRIDE));
	}
	if(tosubclass.equalsIgnoreCase("false"))
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::RESTRICTED));
	}
	else
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::TOSUBCLASS));
	}
	//if(toinstance.equalsIgnoreCase("true"))
	//{
	//	rval.addFlavor(CIMFlavor(CIMFlavor::TOINSTANCE));
	//}
	if(translatable.equalsIgnoreCase("true"))
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::TRANSLATE));
	}
	rval.setPropagated(propagate.equalsIgnoreCase("true"));
	parser.mustGetNextTag();
	if(parser.tokenIs(CIMXMLParser::E_VALUE_ARRAY)
		|| parser.tokenIs(CIMXMLParser::E_VALUE))
	{
		rval.setValue(createValue(parser, cimType));
	}
	parser.mustGetEndTag();
	return rval;
}
///////////////////////////////////
CIMMethod
XMLCIMFactory::createMethod(CIMXMLParser& parser)
{
	if(!parser.tokenIs(CIMXMLParser::E_METHOD))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not method XML");
	}
	String methodName = parser.mustGetAttribute(XMLParameters::paramName);
	String cimType = parser.getAttribute(XMLParameters::paramTypeAssign);
	String classOrigin = parser.getAttribute(
		XMLParameters::paramClassOrigin);
	String propagate = parser.getAttribute(XMLParameters::paramPropagated);
	//
	// A method name must be given
	//
	if(methodName.empty())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"No method name in XML");
	}
	CIMMethod rval;
	//
	// If no return data type, then method returns nothing (void)
	//
	if(!cimType.empty())
	{
		rval.setReturnType(CIMDataType::getDataType(cimType));
	}
	rval.setName(methodName);
	if(!classOrigin.empty())
	{
		rval.setOriginClass(classOrigin);
	}
	rval.setPropagated(propagate.equalsIgnoreCase("true"));
	parser.mustGetNextTag();
	//
	// See if there are qualifiers
	//
	while (parser.tokenIs(CIMXMLParser::E_QUALIFIER))
	{
		rval.addQualifier(createQualifier(parser));
	}
	//
	// Handle parameters
	while (parser.tokenIs(CIMXMLParser::E_PARAMETER)
		|| parser.tokenIs(CIMXMLParser::E_PARAMETER_REFERENCE)
		|| parser.tokenIs(CIMXMLParser::E_PARAMETER_ARRAY)
		|| parser.tokenIs(CIMXMLParser::E_PARAMETER_REFARRAY))
	{
		rval.addParameter(createParameter(parser));
	}
	parser.mustGetEndTag();
	return rval;
}
static CIMValue
convertXMLtoEmbeddedObject(const String& str)
{
	CIMValue rval(CIMNULL);
	// try to convert the string to an class or instance
	TempFileStream ostr;
	ostr << str;
	ostr.rewind();
	CIMXMLParser parser(ostr);
	try
	{
		if (parser)
		{
			try
			{
				CIMClass cc = XMLCIMFactory::createClass(parser);
				rval = CIMValue(cc);
			}
			catch (const CIMException&)
			{
				// XML wasn't a class, so try an instance
				try
				{
					CIMInstance ci = XMLCIMFactory::createInstance(parser);
					rval = CIMValue(ci);
				}
				catch (const CIMException&)
				{
					// XML isn't a class or an instance, so just leave it alone
				}
			}
		}
	}
	catch (XMLParseException& xmlE)
	{
	}
	return rval;
}
namespace
{
struct valueIsEmbeddedInstance
{
	bool operator()(const CIMValue& v)
	{
		return v.getType() == CIMDataType::EMBEDDEDINSTANCE;
	}
};
struct valueIsEmbeddedClass
{
	bool operator()(const CIMValue& v)
	{
		return v.getType() == CIMDataType::EMBEDDEDCLASS;
	}
};
bool isKnownEmbeddedObjectName(String name)
{
	// This is a bad hack, hopefully EmbeddedObject will become a real
	// data type someday.
	// This is all property names in the CIM Schema (as of 2.7.1) that have
	// an EmbeddedObject(true) qualifier.
	// If this list gets much bigger, use a HashMap
	name.toLowerCase();
	return 
		name.equals("sourceinstance") ||
		name.equals("previousinstance") ||
		name.equals("methodparameters") ||
		name.equals("classdefinition") ||
		name.equals("previousclassdefinition") ||
		name.equals("indobject");
}
}
///////////////////////////////////
CIMProperty
XMLCIMFactory::createProperty(CIMXMLParser& parser)
{
	int token = parser.getToken();
	if(token != CIMXMLParser::E_PROPERTY
		&& token != CIMXMLParser::E_PROPERTY_ARRAY
		&& token != CIMXMLParser::E_PROPERTY_REFERENCE)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "not property XML");
	}
	String superClassName;
	String inClassName;
	String propName = parser.mustGetAttribute(XMLParameters::paramName);
	String cimType = parser.getAttribute(XMLParameters::paramTypeAssign);
	String classOrigin = parser.getAttribute(
		XMLParameters::paramClassOrigin);
	String propagate = parser.getAttribute(XMLParameters::paramPropagated);
	CIMProperty rval(propName);
	//
	// If no return data type, then property isn't properly defined
	//
	if(token == CIMXMLParser::E_PROPERTY_REFERENCE)
	{
		rval.setDataType(CIMDataType(parser.getAttribute(
			XMLParameters::paramReferenceClass)));
	}
	else if(!cimType.empty())
	{
		rval.setDataType(CIMDataType::getDataType(cimType));
	}
	else
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"property has null data type");
	}
	//
	// Array type property
	//
	if(token == CIMXMLParser::E_PROPERTY_ARRAY)
	{
		String arraySize = parser.getAttribute(
			XMLParameters::paramArraySize);
		CIMDataType dt = rval.getDataType();
		if (!arraySize.empty())
		{
			Int32 aSize = 0;
			try
			{
				aSize = arraySize.toInt32();
			}
			catch (const StringConversionException&)
			{
				OW_THROWCIMMSG(CIMException::FAILED, format("Array size: \"%1\" is invalid", arraySize).c_str());
			}
			dt.setToArrayType(aSize);
		}
		else
		{
			// no limit
			dt.setToArrayType(0);
		}
		rval.setDataType(dt);
	}
	rval.setOriginClass(classOrigin);
	rval.setPropagated(!propagate.empty() && propagate.equalsIgnoreCase("true"));
	//
	// See if there are qualifiers
	//
	parser.mustGetNextTag();
	while (parser.tokenIs(CIMXMLParser::E_QUALIFIER))
	{
		rval.addQualifier(createQualifier(parser));
	}
	if (parser.tokenIs(CIMXMLParser::E_VALUE)
		|| parser.tokenIs(CIMXMLParser::E_VALUE_ARRAY)
		|| parser.tokenIs(CIMXMLParser::E_VALUE_REFERENCE))
	{
		rval.setValue(createValue(parser,cimType));
		
// Shouldn't need to cast this.
//         if(rval.getDataType().getType() != rval.getValue().getType()
//             || rval.getDataType().isArrayType() !=
//             rval.getValue().isArray())
//         {
//             rval.setValue(CIMValueCast::castValueToDataType(
//                 rval.getValue(), rval.getDataType()));
//         }
	}
	// handle embedded class or instance
	// This checks for prescense of EmbeddedObject(true) qualifier
	// or if the name of the property is a known embedded object from the
	// CIM Schema (ClassDefinition, PreviousClassDefinition, SourceInstance,
	// PreviousInstance, MethodParameters, IndObject)
	// Unfortunately qualifiers usually aren't on instances, and we don't
	// want to always try to convert any string property to an embedded
	// instance, so we just settle for known cases.  SourceInstance is
	// the name of the embedded 
	if ((rval.hasTrueQualifier(CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT) &&
		rval.getDataType().getType() == CIMDataType::STRING &&
		rval.getValue()) ||
		isKnownEmbeddedObjectName(rval.getName())
		)
	{
		if (rval.getDataType().isArrayType())
		{
			StringArray xmlstrings;
			rval.getValue().get(xmlstrings);
			CIMValueArray values;
			for (size_t i = 0; i < xmlstrings.size(); ++i)
			{
				CIMValue v = convertXMLtoEmbeddedObject(xmlstrings[i]);
				if (!v)
				{
					break;
				}
				values.push_back(v);
			}
			if (values.size() == xmlstrings.size() && values.size() > 0)
			{
				if (std::find_if(values.begin(), values.end(), valueIsEmbeddedInstance()) == values.end())
				{
					// no instances, so they all must be classes
					CIMClassArray classes;
					for (size_t i = 0; i < values.size(); ++i)
					{
						CIMClass c(CIMNULL);
						values[i].get(c);
						classes.push_back(c);
					}
					rval.setValue(CIMValue(classes));
					CIMDataType dt(CIMDataType::EMBEDDEDCLASS, rval.getDataType().getSize());
					rval.setDataType(dt);
				}
				else if (std::find_if(values.begin(), values.end(), valueIsEmbeddedClass()) == values.end())
				{
					// no classes, the all must be instances
					CIMInstanceArray instances;
					for (size_t i = 0; i < values.size(); ++i)
					{
						CIMInstance c(CIMNULL);
						values[i].get(c);
						instances.push_back(c);
					}
					rval.setValue(CIMValue(instances));
					CIMDataType dt(CIMDataType::EMBEDDEDINSTANCE, rval.getDataType().getSize());
					rval.setDataType(dt);
				}
				else
				{
					// there are both classes and instances - we cannot handle this!
					// we'll just leave the property alone (as a string)
				}
			}
		}
		else
		{
			CIMValue v = convertXMLtoEmbeddedObject(rval.getValue().toString());
			if (v)
			{
				rval.setDataType(rval.getDataType());
				rval.setValue(v);
			}
		}
	}
	parser.mustGetEndTag();
	return rval;
}
///////////////////////////////////
CIMParameter
XMLCIMFactory::createParameter(CIMXMLParser& parser)
{
	int paramToken = parser.getToken();
	
	if(paramToken != CIMXMLParser::E_PARAMETER
		&& paramToken != CIMXMLParser::E_PARAMETER_REFERENCE
		&& paramToken != CIMXMLParser::E_PARAMETER_ARRAY
		&& paramToken != CIMXMLParser::E_PARAMETER_REFARRAY)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not parameter XML");
	}
	
	//
	// Fetch name
	//
	CIMParameter rval(parser.mustGetAttribute(XMLParameters::paramName));
	
	//
	// Get parameter type
	//
	switch(paramToken)
	{
		case CIMXMLParser::E_PARAMETER:
		{
			rval.setDataType(CIMDataType::getDataType(
				parser.mustGetAttribute(XMLParameters::paramTypeAssign)));
			break;
		}
	
		case CIMXMLParser::E_PARAMETER_REFERENCE:
		{
			rval.setDataType(CIMDataType(
				parser.getAttribute(XMLParameters::paramRefClass)));
			break;
		}
	
		case CIMXMLParser::E_PARAMETER_ARRAY:
		{
			CIMDataType dt = CIMDataType::getDataType(
				parser.mustGetAttribute(XMLParameters::paramTypeAssign));
	
			if(!dt)
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					"invalid parameter data type");
			}
	
			try
			{
				dt.setToArrayType(
					parser.getAttribute(XMLParameters::paramArraySize).toInt32());
			}
			catch (const StringConversionException&)
			{
				dt.setToArrayType(0);
			}
			rval.setDataType(dt);
			break;
		}
	
		case CIMXMLParser::E_PARAMETER_REFARRAY:
		{
			CIMDataType dt = CIMDataType(
				parser.getAttribute(XMLParameters::paramRefClass));
	
			try
			{
				dt.setToArrayType(
					parser.getAttribute(XMLParameters::paramArraySize).toInt32());
			}
			catch (const StringConversionException&)
			{
				dt.setToArrayType(0);
			}
			rval.setDataType(dt);
			break;
		}
	
		default:
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				"could not decode parameter XML");
	}
	
	//
	// See if there are qualifiers
	//
	CIMQualifierArray qualArray;
	parser.mustGetNextTag();
	while (parser.tokenIs(CIMXMLParser::E_QUALIFIER))
	{
		qualArray.append(createQualifier(parser));
	}
	rval.setQualifiers(qualArray);
	parser.mustGetEndTag();
	return rval;
}

} // end namespace OpenWBEM

