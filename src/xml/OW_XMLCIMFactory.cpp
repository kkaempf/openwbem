/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

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
#include "OW_XMLParseException.hpp"
#include <algorithm> // for find_if

namespace OW_NAMESPACE
{

namespace XMLCIMFactory
{

static void
getLocalNameSpacePathAndSet(CIMObjectPath& cop, CIMXMLParser& parser)
{
	// <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
	if (!parser.tokenIsId(CIMXMLParser::E_LOCALNAMESPACEPATH))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Expected <LOCALNAMESPACEPATH>");
	}
	parser.mustGetChildId(CIMXMLParser::E_NAMESPACE);
	// <!ELEMENT NAMESPACE EMPTY>
	// <!ATTLIST NAMESPACE %CIMName;>
	String ns;
	while (parser.tokenIsId(CIMXMLParser::E_NAMESPACE))	
	{
		String nscomp = parser.mustGetAttribute(CIMXMLParser::A_NAME);
		if (!nscomp.empty())
		{
			if (!ns.empty() )
			{
				ns += "/";
			}
			ns += nscomp;
		}
		parser.mustGetNextTag();
		parser.mustGetEndTag(); // pass </NAMESPACE>
	}
	if (ns.empty())
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
	parser.mustGetChildId(CIMXMLParser::E_HOST);
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
	String valuetype = parser.mustGetAttribute(CIMXMLParser::A_VALUETYPE);
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
	OW_ASSERT(parser.tokenIsId(CIMXMLParser::E_INSTANCENAME));
	String thisClassName = parser.getAttribute(CIMXMLParser::A_CLASSNAME);
	cimPath.setClassName(thisClassName);
	//parser.getChild();
	parser.getNextTag();
	if (parser.tokenIsId(CIMXMLParser::E_KEYBINDING))
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
			switch (parser.getToken())
			{
				case CIMXMLParser::E_KEYVALUE:
					getKeyValue(parser,value);
					break;
				case CIMXMLParser::E_VALUE_REFERENCE:
					value = XMLCIMFactory::createValue(parser, "REF", E_VALUE_NOT_EMBEDDED_OBJECT);
					break;
				default:
					OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
						"Not a valid instance declaration");
			}
			cp = CIMProperty(name, value);
			propertyArray.push_back(cp);
			parser.mustGetEndTag(); // pass </KEYBINDING>
		} while (parser.tokenIsId(CIMXMLParser::E_KEYBINDING));
	}
	else if (parser.tokenIsId(CIMXMLParser::E_KEYVALUE))
	{
		CIMValue value(CIMNULL);
		cp = CIMProperty();
		getKeyValue(parser,value);
		cp.setDataType(value.getCIMDataType());
		cp.setValue(value);
		propertyArray.push_back(cp);
	}
	else if (parser.tokenIsId(CIMXMLParser::E_VALUE_REFERENCE))
	{
		CIMValue value = XMLCIMFactory::createValue(parser, "REF", E_VALUE_NOT_EMBEDDED_OBJECT);
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
createObjectPath(CIMXMLParser& parser)
{
	CIMObjectPath rval;
	int token = parser.getToken();
	switch (token)
	{
		case CIMXMLParser::E_OBJECTPATH:
			parser.mustGetChild();
			rval = createObjectPath(parser);
			parser.mustGetEndTag(); // pass </OBJECTPATH>
			return rval;
		case CIMXMLParser::E_LOCALCLASSPATH:
			parser.mustGetChildId(CIMXMLParser::E_LOCALNAMESPACEPATH);
			getLocalNameSpacePathAndSet(rval, parser);
			parser.mustGetNextId(CIMXMLParser::E_CLASSNAME);
			rval.setClassName(parser.mustGetAttribute(CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			parser.mustGetEndTag(); // pass </LOCALCLASSPATH>
			return rval;
		case CIMXMLParser::E_CLASSPATH:
			parser.mustGetChildId(CIMXMLParser::E_NAMESPACEPATH);
			getNameSpacePathAndSet(rval, parser);
			parser.mustGetNextId(CIMXMLParser::E_CLASSNAME);
			rval.setClassName(parser.mustGetAttribute(CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			parser.mustGetEndTag(); // pass </LOCALCLASSPATH>
			return rval;
		case CIMXMLParser::E_CLASSNAME:
			rval.setClassName(parser.mustGetAttribute(CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			return rval;
		case CIMXMLParser::E_INSTANCENAME:
			getInstanceName(parser, rval);
			return rval;
		case CIMXMLParser::E_LOCALINSTANCEPATH:
			parser.mustGetChildId(CIMXMLParser::E_LOCALNAMESPACEPATH);
			getLocalNameSpacePathAndSet(rval, parser);
			break;
		case CIMXMLParser::E_INSTANCEPATH:
			parser.mustGetChildId(CIMXMLParser::E_NAMESPACEPATH);
			getNameSpacePathAndSet(rval, parser);
			break;
		default:
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					Format("Invalid XML for Object path construction.  Node "
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
createClass(CIMXMLParser& parser)
{
	CIMClass rval;
	CIMName superClassName;
	
	if (!parser.tokenIsId(CIMXMLParser::E_CLASS))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not class XML");
	}
	
	String inClassName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	rval.setName(inClassName);
	superClassName = parser.getAttribute(CIMXMLParser::A_SUPERCLASS);
	if (superClassName != "")
	{
		rval.setSuperClass(superClassName);
	}

	//
	// Find qualifier information
	//
	parser.mustGetNextTag();
	while (parser.tokenIsId(CIMXMLParser::E_QUALIFIER))
	{
		CIMQualifier cq = createQualifier(parser);
//         if (cq.getName().equalsIgnoreCase(CIMQualifier::CIM_QUAL_ASSOCIATION))
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
	while (parser.tokenIsId(CIMXMLParser::E_PROPERTY)
		   || parser.tokenIsId(CIMXMLParser::E_PROPERTY_ARRAY)
		   || parser.tokenIsId(CIMXMLParser::E_PROPERTY_REFERENCE))
	{
		rval.addProperty(createProperty(parser));
	}
	//
	// Load methods
	//
	while (parser.tokenIsId(CIMXMLParser::E_METHOD))
	{
		rval.addMethod(createMethod(parser));
	}
	parser.mustGetEndTag();
	return rval;
}
///////////////////////////////////
CIMInstance
createInstance(CIMXMLParser& parser)
{
	CIMInstance rval;
	if (!parser.tokenIsId(CIMXMLParser::E_INSTANCE))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not instance XML");
	}
		
	rval.setClassName(parser.mustGetAttribute(CIMXMLParser::A_CLASSNAME));

	String language = parser.getAttribute(CIMXMLParser::A_XML_LANG, false);
	if (!language.empty())
	{
		rval.setLanguage(language);
	}

	//
	// Find qualifier information
	//
	CIMQualifierArray quals;
	parser.getChild();
	while (parser.tokenIsId(CIMXMLParser::E_QUALIFIER))
	{
		quals.append(createQualifier(parser));
	}
	rval.setQualifiers(quals);
	//
	// Load properties
	//
	CIMPropertyArray props;
	while (parser.tokenIsId(CIMXMLParser::E_PROPERTY)
		   || parser.tokenIsId(CIMXMLParser::E_PROPERTY_ARRAY)
		   || parser.tokenIsId(CIMXMLParser::E_PROPERTY_REFERENCE))
	{
		props.append(createProperty(parser));
	}
	rval.setProperties(props);
	parser.mustGetEndTag();
	return rval;
}
namespace /* anonymous */
{
inline void StringToType(const String& s, UInt8& b)
{
	b = s.toUInt8();
}
inline void StringToType(const String& s, Int8& b)
{
	b = s.toInt8();
}
inline void StringToType(const String& s, UInt16& b)
{
	b = s.toUInt16();
}
inline void StringToType(const String& s, Int16& b)
{
	b = s.toInt16();
}
inline void StringToType(const String& s, UInt32& b)
{
	b = s.toUInt32();
}
inline void StringToType(const String& s, Int32& b)
{
	b = s.toInt32();
}
inline void StringToType(const String& s, UInt64& b)
{
	b = s.toUInt64();
}
inline void StringToType(const String& s, Int64& b)
{
	b = s.toInt64();
}
inline void StringToType(const String& s, String& b)
{
	b = s;
}
inline void StringToType(const String& s, Real32& b)
{
	b = s.toReal32();
}
inline void StringToType(const String& s, Real64& b)
{
	b = s.toReal64();
}
inline void StringToType(const String& s, Char16& b)
{
	b = Char16(s);
}
inline void StringToType(const String& s, CIMDateTime& b)
{
	b = CIMDateTime(s);
}
} /* end anonymous namespace */

///////////////////////////////////
template <class T>
static inline void
convertCimType(Array<T>& ra, CIMXMLParser& parser)
{
	// start out possibly pointing at <VALUE>
	while (parser.tokenIsId(CIMXMLParser::E_VALUE))
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
createValue(CIMXMLParser& parser, String const& valueType)
{
	return createValue(parser, valueType, E_VALUE_NOT_EMBEDDED_OBJECT);
}

namespace
{

CIMValue
convertXMLtoEmbeddedObject(const String& str)
{
	CIMValue rval(CIMNULL);
	// try to convert the string to an class or instance
	TempFileStream ostr;
	ostr << str;
	try
	{
		try
		{
			ostr.rewind();
			CIMXMLParser parser(ostr);
			CIMInstance ci = XMLCIMFactory::createInstance(parser);
			rval = CIMValue(ci);
		}
		catch (const CIMException&)
		{
			// XML wasn't an instance, so try an class
			try
			{
				ostr.rewind();
				CIMXMLParser parser(ostr);
				CIMClass cc = XMLCIMFactory::createClass(parser);
				rval = CIMValue(cc);
			}
			catch (const CIMException&)
			{
				// XML isn't a class or an instance, so just leave it alone
			}
		}
	}
	catch (XMLParseException& xmlE)
	{
	}
	return rval;
}

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

bool isKnownEmbeddedObjectName(const String& name)
{
	// This is a bad hack, hopefully EmbeddedObject will become a real
	// data type someday.
	// This is all property names in the CIM Schema (as of 2.7.1) that have
	// an EmbeddedObject(true) qualifier.
	// If this list gets much bigger, use a HashMap
	String lname(name);
	lname.toLowerCase();
	return
		lname.equals("sourceinstance") ||
		lname.equals("previousinstance") ||
		lname.equals("methodparameters") ||
		lname.equals("classdefinition") ||
		lname.equals("previousclassdefinition") ||
		lname.equals("indobject");
}

} // anonymous namespace


///////////////////////////////////
CIMValue
createValue(CIMXMLParser& parser, String const& valueType, EEmbeddedObjectFlag embeddedObjectFlag)
{
	CIMValue rval(CIMNULL);
	try
	{
	
		int token = parser.getToken();
	
		switch (token)
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
					if (type == CIMDataType::INVALID)
					{
						OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							"Invalid data type on node");
					}
					parser.mustGetNextTag();
	
					switch (type)
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
								for (size_t i = 0; i < sra.size(); i++)
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
	
					while (parser.tokenIsId(CIMXMLParser::E_VALUE_REFERENCE))
					{
						CIMObjectPath cop(CIMNULL);
						CIMValue v = createValue(parser, valueType, E_VALUE_NOT_EMBEDDED_OBJECT);
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


		// handle embedded class or instance
		if (embeddedObjectFlag == E_VALUE_IS_EMBEDDED_INSTANCE || embeddedObjectFlag == E_VALUE_IS_EMBEDDED_OBJECT)
		{
			if (rval.getCIMDataType().isArrayType())
			{
				StringArray xmlstrings;
				rval.get(xmlstrings);
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
					if (std::find_if (values.begin(), values.end(), valueIsEmbeddedInstance()) == values.end())
					{
						// no instances, so they all must be classes
						CIMClassArray classes;
						for (size_t i = 0; i < values.size(); ++i)
						{
							CIMClass c(CIMNULL);
							values[i].get(c);
							classes.push_back(c);
						}
						rval = CIMValue(classes);
					}
					else if (std::find_if (values.begin(), values.end(), valueIsEmbeddedClass()) == values.end())
					{
						// no classes, the all must be instances
						CIMInstanceArray instances;
						for (size_t i = 0; i < values.size(); ++i)
						{
							CIMInstance c(CIMNULL);
							values[i].get(c);
							instances.push_back(c);
						}
						rval = CIMValue(instances);
					}
					else
					{
						// there are both classes and instances - we cannot handle this!
						// we'll just leave the property alone (as a string)
					}
				}
				else if (values.size() == 0)
				{
					// it was an empty array.  Assume it was instances, not classes.
					rval = CIMValue(CIMInstanceArray());
				}
			}
			else
			{
				CIMValue v = convertXMLtoEmbeddedObject(rval.toString());
				if (v)
				{
					rval = v;
				}
			}
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
createQualifier(CIMXMLParser& parser)
{
	if (!parser.tokenIsId(CIMXMLParser::E_QUALIFIER))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not qualifier XML");
	}
	CIMDataType dt(CIMNULL);
	String name = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	String cimType = parser.getAttribute(CIMXMLParser::A_TYPE);
	String propagate = parser.getAttribute(CIMXMLParser::A_PROPAGATED);
	String tosubclass = parser.getAttribute(CIMXMLParser::A_TOSUBCLASS);
	String overridable = parser.getAttribute(
		CIMXMLParser::A_OVERRIDABLE);
	//String toinstance = parser.getAttribute(
	//	XMLParameters::paramToInstance);
	String translatable = parser.getAttribute(
		CIMXMLParser::A_TRANSLATABLE);

	String language = parser.getAttribute(CIMXMLParser::A_XML_LANG, false);

	//
	// Build qualifier
	//
	if (!cimType.empty())
	{
		dt = CIMDataType::getDataType(cimType);
	}
	if (!dt)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("Qualifier not assigned a data type: %1", name).c_str());
	}
	CIMQualifierType cqt;
	cqt.setDataType(dt);
	cqt.setName(name);
	
	CIMQualifier rval(cqt);
	if (overridable.equalsIgnoreCase("false"))
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::DISABLEOVERRIDE));
	}
	else
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::ENABLEOVERRIDE));
	}
	if (tosubclass.equalsIgnoreCase("false"))
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::RESTRICTED));
	}
	else
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::TOSUBCLASS));
	}
	//if (toinstance.equalsIgnoreCase("true"))
	//{
	//	rval.addFlavor(CIMFlavor(CIMFlavor::TOINSTANCE));
	//}
	if (translatable.equalsIgnoreCase("true"))
	{
		rval.addFlavor(CIMFlavor(CIMFlavor::TRANSLATE));
	}
	rval.setPropagated(propagate.equalsIgnoreCase("true"));
	
	if (!language.empty())
	{
		rval.setLanguage(language);
	}

	parser.mustGetNextTag();
	if (parser.tokenIsId(CIMXMLParser::E_VALUE_ARRAY)
		|| parser.tokenIsId(CIMXMLParser::E_VALUE))
	{
		rval.setValue(createValue(parser, cimType, E_VALUE_NOT_EMBEDDED_OBJECT));
	}
	parser.mustGetEndTag();
	return rval;
}
///////////////////////////////////
CIMMethod
createMethod(CIMXMLParser& parser)
{
	if (!parser.tokenIsId(CIMXMLParser::E_METHOD))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not method XML");
	}
	String methodName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	String cimType = parser.getAttribute(CIMXMLParser::A_TYPE);
	String classOrigin = parser.getAttribute(
		CIMXMLParser::A_CLASSORIGIN);
	String propagate = parser.getAttribute(CIMXMLParser::A_PROPAGATED);
	//
	// A method name must be given
	//
	if (methodName.empty())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"No method name in XML");
	}
	CIMMethod rval;
	//
	// If no return data type, then method returns nothing (void)
	//
	if (!cimType.empty())
	{
		rval.setReturnType(CIMDataType::getDataType(cimType));
	}
	rval.setName(methodName);
	if (!classOrigin.empty())
	{
		rval.setOriginClass(classOrigin);
	}
	rval.setPropagated(propagate.equalsIgnoreCase("true"));
	parser.mustGetNextTag();
	//
	// See if there are qualifiers
	//
	while (parser.tokenIsId(CIMXMLParser::E_QUALIFIER))
	{
		rval.addQualifier(createQualifier(parser));
	}
	//
	// Handle parameters
	while (parser.tokenIsId(CIMXMLParser::E_PARAMETER)
		|| parser.tokenIsId(CIMXMLParser::E_PARAMETER_REFERENCE)
		|| parser.tokenIsId(CIMXMLParser::E_PARAMETER_ARRAY)
		|| parser.tokenIsId(CIMXMLParser::E_PARAMETER_REFARRAY))
	{
		rval.addParameter(createParameter(parser));
	}
	parser.mustGetEndTag();
	return rval;
}

EEmbeddedObjectFlag getEmbeddedObjectType(const CIMXMLParser& parser)
{
	EEmbeddedObjectFlag embeddedObjectType = E_VALUE_NOT_EMBEDDED_OBJECT;
	String embeddedObjectValue = parser.getAttribute(CIMXMLParser::A_EMBEDDEDOBJECT);
	if (embeddedObjectValue == CIMXMLParser::AV_EMBEDDEDOBJECT_OBJECT_VALUE)
	{
		embeddedObjectType = E_VALUE_IS_EMBEDDED_OBJECT;
	}
	else if (embeddedObjectValue == CIMXMLParser::AV_EMBEDDEDOBJECT_INSTANCE_VALUE)
	{
		embeddedObjectType = E_VALUE_IS_EMBEDDED_INSTANCE;
	}
	return embeddedObjectType;
}

///////////////////////////////////
CIMProperty
createProperty(CIMXMLParser& parser)
{
	int token = parser.getToken();
	if (token != CIMXMLParser::E_PROPERTY
		&& token != CIMXMLParser::E_PROPERTY_ARRAY
		&& token != CIMXMLParser::E_PROPERTY_REFERENCE)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "not property XML");
	}
	String superClassName;
	String inClassName;
	String propName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	String cimType = parser.getAttribute(CIMXMLParser::A_TYPE);
	String classOrigin = parser.getAttribute(
		CIMXMLParser::A_CLASSORIGIN);
	String propagate = parser.getAttribute(CIMXMLParser::A_PROPAGATED);
	
	EEmbeddedObjectFlag embeddedObjectType = getEmbeddedObjectType(parser);

	CIMProperty rval(propName);
	//
	// If no return data type, then property isn't properly defined
	//
	if (token == CIMXMLParser::E_PROPERTY_REFERENCE)
	{
		rval.setDataType(CIMDataType(parser.getAttribute(
			CIMXMLParser::A_REFERENCECLASS)));
	}
	else if (!cimType.empty())
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
	if (token == CIMXMLParser::E_PROPERTY_ARRAY)
	{
		String arraySize = parser.getAttribute(
			CIMXMLParser::A_ARRAYSIZE);
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
				OW_THROWCIMMSG(CIMException::FAILED, Format("Array size: \"%1\" is invalid", arraySize).c_str());
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
	while (parser.tokenIsId(CIMXMLParser::E_QUALIFIER))
	{
		rval.addQualifier(createQualifier(parser));
	}

	if (parser.tokenIsId(CIMXMLParser::E_VALUE)
		|| parser.tokenIsId(CIMXMLParser::E_VALUE_ARRAY)
		|| parser.tokenIsId(CIMXMLParser::E_VALUE_REFERENCE))
	{
		// embeddedObjectType would only be set to E_VALUE_IS_EMBEDDED_OBJECT or E_VALUE_IS_EMBEDDED_INSTANCE if the xml contains the EmbeddedObject attribute.
		if (embeddedObjectType == E_VALUE_NOT_EMBEDDED_OBJECT)
		{
			// check for old ways to enable the embedded decoding
			if ((rval.hasTrueQualifier(CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT) &&
				 rval.getDataType().getType() == CIMDataType::STRING) ||
				isKnownEmbeddedObjectName(rval.getName())
				)
			{
				embeddedObjectType = E_VALUE_IS_EMBEDDED_OBJECT;
			}
		}

		CIMDataType dt = rval.getDataType();
		CIMValue val = createValue(parser, cimType, embeddedObjectType);
		if (val)
		{
			dt.syncWithValue(val);
		}
		else if (embeddedObjectType == E_VALUE_IS_EMBEDDED_OBJECT || embeddedObjectType == E_VALUE_IS_EMBEDDED_INSTANCE)
		{
			if (dt.isArrayType())
			{
				dt = CIMDataType(CIMDataType::EMBEDDEDINSTANCE, dt.getSize());
			}
			else
			{
				dt = CIMDataType(CIMDataType::EMBEDDEDINSTANCE);
			}
		}
		rval.setDataType(dt);

		rval.setValue(val);
	}
	parser.mustGetEndTag();
	return rval;
}

///////////////////////////////////
CIMParameter
createParameter(CIMXMLParser& parser)
{
	int paramToken = parser.getToken();
	
	if (paramToken != CIMXMLParser::E_PARAMETER
		&& paramToken != CIMXMLParser::E_PARAMETER_REFERENCE
		&& paramToken != CIMXMLParser::E_PARAMETER_ARRAY
		&& paramToken != CIMXMLParser::E_PARAMETER_REFARRAY)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Not parameter XML");
	}
	
	//
	// Fetch name
	//
	CIMParameter rval(parser.mustGetAttribute(CIMXMLParser::A_NAME));
	
	//
	// Get parameter type
	//
	switch (paramToken)
	{
		case CIMXMLParser::E_PARAMETER:
		{
			rval.setDataType(CIMDataType::getDataType(
				parser.mustGetAttribute(CIMXMLParser::A_TYPE)));
			break;
		}
	
		case CIMXMLParser::E_PARAMETER_REFERENCE:
		{
			rval.setDataType(CIMDataType(
				parser.getAttribute(CIMXMLParser::A_REFERENCECLASS)));
			break;
		}
	
		case CIMXMLParser::E_PARAMETER_ARRAY:
		{
			CIMDataType dt = CIMDataType::getDataType(
				parser.mustGetAttribute(CIMXMLParser::A_TYPE));
	
			if (!dt)
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					"invalid parameter data type");
			}
	
			try
			{
				dt.setToArrayType(
					parser.getAttribute(CIMXMLParser::A_ARRAYSIZE).toInt32());
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
				parser.getAttribute(CIMXMLParser::A_REFERENCECLASS));
	
			try
			{
				dt.setToArrayType(
					parser.getAttribute(CIMXMLParser::A_ARRAYSIZE).toInt32());
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
	while (parser.tokenIsId(CIMXMLParser::E_QUALIFIER))
	{
		qualArray.append(createQualifier(parser));
	}
	rval.setQualifiers(qualArray);
	parser.mustGetEndTag();
	return rval;
}

} // end namespace XMLCIMFactory
} // end namespace OW_NAMESPACE

