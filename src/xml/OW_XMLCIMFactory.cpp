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

#include <algorithm> // for find_if

static void
getLocalNameSpacePathAndSet(OW_CIMObjectPath& cop, OW_CIMXMLParser& parser)
{
	// <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
	if (!parser.tokenIs(OW_CIMXMLParser::E_LOCALNAMESPACEPATH))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Expected <LOCALNAMESPACEPATH>");
	}
	parser.mustGetChild(OW_CIMXMLParser::E_NAMESPACE);

	// <!ELEMENT NAMESPACE EMPTY>
	// <!ATTLIST NAMESPACE %CIMName;>
	OW_String ns;
	while(parser.tokenIs(OW_CIMXMLParser::E_NAMESPACE))	
	{
		OW_String nscomp = parser.mustGetAttribute(OW_CIMXMLParser::A_NAME);
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
getNameSpacePathAndSet(OW_CIMObjectPath& cop, OW_CIMXMLParser& parser)
{
	// <!ELEMENT NAMESPACEPATH (HOST,LOCALNAMESPACEPATH)>
	// <!ELEMENT HOST (#PCDATA)>

	parser.mustGetChild(OW_CIMXMLParser::E_HOST);
	parser.mustGetNext();
	if (!parser.isData())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "invalid <HOST> in <NAMESPACEPATH>");
	}
	cop.setHost(parser.getData());
	parser.mustGetNextTag();
	parser.mustGetEndTag(); // pass </HOST>
	getLocalNameSpacePathAndSet(cop, parser);
	parser.mustGetEndTag(); // pass </NAMESPACEPATH>
}

static void getKeyValue(OW_CIMXMLParser& parser, OW_CIMValue& value)
{
	OW_String valuetype = parser.mustGetAttribute(OW_CIMXMLParser::A_VALUE_TYPE);
	parser.mustGetNext();
	if (!parser.isData())
	{
		value = OW_CIMValue("");
	}
	else
	{
		value = OW_CIMValue(parser.getData());
		parser.mustGetNextTag();
	}
	parser.mustGetEndTag();

	// cast the value to the correct type, if not a string
	if (valuetype == "boolean")
	{
		value = OW_CIMValueCast::castValueToDataType(value,OW_CIMDataType::BOOLEAN);
	}
	else if (valuetype == "numeric")
	{
		try
		{
			value = OW_CIMValueCast::castValueToDataType(value,OW_CIMDataType::SINT64);
		}
		catch (const OW_CIMException& e)
		{
			value = OW_CIMValueCast::castValueToDataType(value,OW_CIMDataType::REAL64);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void getInstanceName(OW_CIMXMLParser& parser, OW_CIMObjectPath& cimPath)
{
	// <!ELEMENT INSTANCENAME (KEYBINDING*|KEYVALUE?|VALUE.REFERENCE?)>
	// <!ATTLIST INSTANCENAME %ClassName;>

	OW_CIMPropertyArray propertyArray;
	OW_CIMProperty cp(OW_CIMNULL);

	OW_ASSERT(parser.tokenIs(OW_CIMXMLParser::E_INSTANCENAME));

	OW_String thisClassName = parser.getAttribute(OW_CIMXMLParser::A_CLASS_NAME);
	cimPath.setObjectName(thisClassName);

	//parser.getChild();
	parser.getNextTag();
	if (parser.tokenIs(OW_CIMXMLParser::E_KEYBINDING))
	{
		do
		{
			// <!ELEMENT KEYBINDING (KEYVALUE|VALUE.REFERENCE)>
			// <!ATTLIST KEYBINDING %CIMName;>
			//
			// <!ELEMENT KEYVALUE (#PCDATA)>
			// <!ATTLIST KEYVALUE
			//          VALUETYPE    (string|boolean|numeric)  'string'>

			OW_CIMValue value(OW_CIMNULL);
			OW_String name;
			OW_CIMXMLParser keyval;

			name = parser.mustGetAttribute(OW_CIMXMLParser::A_NAME);

			parser.mustGetChild();

			switch(parser.getToken())
			{
				case OW_CIMXMLParser::E_KEYVALUE:
					getKeyValue(parser,value);
					break;
				case OW_CIMXMLParser::E_VALUE_REFERENCE:
					value = OW_XMLCIMFactory::createValue(parser, "REF");
					break;
				default:
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
						"Not a valid instance declaration");
			}

			cp = OW_CIMProperty(name, value);
			propertyArray.push_back(cp);
			parser.mustGetEndTag(); // pass </KEYBINDING>
		} while (parser.tokenIs(OW_CIMXMLParser::E_KEYBINDING));
	}

	else if (parser.tokenIs(OW_CIMXMLParser::E_KEYVALUE))
	{
		OW_CIMValue value(OW_CIMNULL);
		cp = OW_CIMProperty();
		getKeyValue(parser,value);
		cp.setDataType(value.getCIMDataType());
		cp.setValue(value);
		propertyArray.push_back(cp);
	}
	else if (parser.tokenIs(OW_CIMXMLParser::E_VALUE_REFERENCE))
	{
		OW_CIMValue value = OW_XMLCIMFactory::createValue(parser, "REF");
		cp = OW_CIMProperty();
		cp.setDataType(OW_CIMDataType::REFERENCE);
		cp.setValue(value);
		propertyArray.push_back(cp);
	}
	// No. Any of the sub-elements are optional.  If none are found, then the
	// path is to a singleton.
	//else
	//{
	//	OW_THROWCIMMSG(OW_CIMException::FAILED,
	//		"not a valid instance declaration");
	//}

	parser.mustGetEndTag();

	cimPath.setKeys(propertyArray);

}

///////////////////////////////////
OW_CIMObjectPath
OW_XMLCIMFactory::createObjectPath(OW_CIMXMLParser& parser)
{
	OW_CIMObjectPath rval;

	int token = parser.getToken();

	switch(token)
	{
		case OW_CIMXMLParser::E_LOCALCLASSPATH:
			parser.mustGetChild(OW_CIMXMLParser::E_LOCALNAMESPACEPATH);
			getLocalNameSpacePathAndSet(rval, parser);
			parser.mustGetNext(OW_CIMXMLParser::E_CLASSNAME);
			rval.setObjectName(parser.mustGetAttribute(OW_CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			parser.mustGetEndTag(); // pass </LOCALCLASSPATH>
			return rval;

		case OW_CIMXMLParser::E_CLASSPATH:
			parser.mustGetChild(OW_CIMXMLParser::E_NAMESPACEPATH);
			getNameSpacePathAndSet(rval, parser);
			parser.mustGetNext(OW_CIMXMLParser::E_CLASSNAME);
			rval.setObjectName(parser.mustGetAttribute(OW_CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			parser.mustGetEndTag(); // pass </LOCALCLASSPATH>
			return rval;

		case OW_CIMXMLParser::E_CLASSNAME:
			rval.setObjectName(parser.mustGetAttribute(OW_CIMXMLParser::A_NAME));
			parser.mustGetNextTag();
			parser.mustGetEndTag(); // pass </CLASSNAME>
			return rval;

		case OW_CIMXMLParser::E_INSTANCENAME:
			getInstanceName(parser, rval);
			return rval;

		case OW_CIMXMLParser::E_LOCALINSTANCEPATH:
			parser.mustGetChild(OW_CIMXMLParser::E_LOCALNAMESPACEPATH);
			getLocalNameSpacePathAndSet(rval, parser);
			break;

		case OW_CIMXMLParser::E_INSTANCEPATH:
			parser.mustGetChild(OW_CIMXMLParser::E_NAMESPACEPATH);
			getNameSpacePathAndSet(rval, parser);
			break;

		default:
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
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
OW_CIMClass
OW_XMLCIMFactory::createClass(OW_CIMXMLParser& parser)
{
	OW_CIMClass rval;
	OW_String superClassName;
	
	if (!parser.tokenIs(OW_CIMXMLParser::E_CLASS))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not class XML");
	}
	
	OW_String inClassName = parser.mustGetAttribute(OW_XMLParameters::paramName);
	rval.setName(inClassName);

	superClassName = parser.getAttribute(OW_XMLParameters::paramSuperName);
	if(!superClassName.empty())
	{
		rval.setSuperClass(superClassName);
	}

	//
	// Find qualifier information
	//
	parser.mustGetNextTag();
	while (parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER))
	{
		OW_CIMQualifier cq = createQualifier(parser);
//         if(cq.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_ASSOCIATION))
//         {
//             if (!cq.getValue()
//                 || cq.getValue() != OW_CIMValue(false))
//             {
//                 rval.setIsAssociation(true);
//             }
//         }

		rval.addQualifier(cq);
	}

	//
	// Load properties
	//
	while (parser.tokenIs(OW_CIMXMLParser::E_PROPERTY)
		   || parser.tokenIs(OW_CIMXMLParser::E_PROPERTY_ARRAY)
		   || parser.tokenIs(OW_CIMXMLParser::E_PROPERTY_REFERENCE))
	{
		rval.addProperty(createProperty(parser));
	}

	//
	// Load methods
	//
	while (parser.tokenIs(OW_CIMXMLParser::E_METHOD))
	{
		rval.addMethod(createMethod(parser));
	}

	parser.mustGetEndTag();
	return rval;
}

///////////////////////////////////
OW_CIMInstance
OW_XMLCIMFactory::createInstance(OW_CIMXMLParser& parser)
{
	OW_CIMInstance rval;
	if (!parser.tokenIs(OW_CIMXMLParser::E_INSTANCE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not instance XML");
	}
		
	rval.setClassName(parser.mustGetAttribute(OW_CIMXMLParser::A_CLASS_NAME));

	//
	// Find qualifier information
	//
	OW_CIMQualifierArray quals;
	parser.getChild();
	while (parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER))
	{
		quals.append(createQualifier(parser));
	}
	rval.setQualifiers(quals);

	//
	// Load properties
	//
	OW_CIMPropertyArray props;
	while (parser.tokenIs(OW_CIMXMLParser::E_PROPERTY)
		   || parser.tokenIs(OW_CIMXMLParser::E_PROPERTY_ARRAY)
		   || parser.tokenIs(OW_CIMXMLParser::E_PROPERTY_REFERENCE))
	{
		props.append(createProperty(parser));
	}

	rval.setProperties(props);
	parser.mustGetEndTag();

	return rval;
}

static inline void StringToType(const OW_String& s, OW_Byte& b)
{
	b = s.toUInt8();
}
static inline void StringToType(const OW_String& s, OW_Int8& b)
{
	b = s.toInt8();
}
static inline void StringToType(const OW_String& s, OW_UInt16& b)
{
	b = s.toUInt16();
}
static inline void StringToType(const OW_String& s, OW_Int16& b)
{
	b = s.toInt16();
}
static inline void StringToType(const OW_String& s, OW_UInt32& b)
{
	b = s.toUInt32();
}
static inline void StringToType(const OW_String& s, OW_Int32& b)
{
	b = s.toInt32();
}
static inline void StringToType(const OW_String& s, OW_UInt64& b)
{
	b = s.toUInt64();
}
static inline void StringToType(const OW_String& s, OW_Int64& b)
{
	b = s.toInt64();
}
static inline void StringToType(const OW_String& s, OW_String& b)
{
	b = s;
}
static inline void StringToType(const OW_String& s, OW_Real32& b)
{
	b = s.toReal32();
}
static inline void StringToType(const OW_String& s, OW_Real64& b)
{
	b = s.toReal64();
}
static inline void StringToType(const OW_String& s, OW_Char16& b)
{
	b = s.toChar16();
}
static inline void StringToType(const OW_String& s, OW_CIMDateTime& b)
{
	b = s.toDateTime();
}
///////////////////////////////////
template <class T>
static inline void
convertCimType(OW_Array<T>& ra, OW_CIMXMLParser& parser)
{
	// start out possibly pointing at <VALUE>
	while(parser.tokenIs(OW_CIMXMLParser::E_VALUE))
	{
		parser.mustGetNext();
		if (parser.isData())
		{
			OW_String vstr = parser.getData();
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
OW_CIMValue
OW_XMLCIMFactory::createValue(OW_CIMXMLParser& parser,
	OW_String const& valueType)
{
	OW_CIMValue rval(OW_CIMNULL);
	try
	{
	
		int token = parser.getToken();
	
		switch(token)
		{
			// <VALUE> elements
			case OW_CIMXMLParser::E_VALUE:
				{
					parser.mustGetNext();
					if (parser.isData())
					{
						OW_String vstr = parser.getData();
						rval = OW_CIMValue::createSimpleValue(valueType, vstr);
						parser.mustGetNextTag(); // pass text
					}
					else
					{
						rval = OW_CIMValue::createSimpleValue(valueType, "");
					}
					parser.mustGetEndTag(); // get </VALUE>
					break;
				}
	
			// <VALUE.ARRAY> elements
			case OW_CIMXMLParser::E_VALUE_ARRAY:
				{
					int type = OW_CIMDataType::strToSimpleType(valueType);
					if(type == OW_CIMDataType::INVALID)
					{
						OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"Invalid data type on node");
					}

					parser.mustGetNextTag();
	
					switch(type)
					{
						case OW_CIMDataType::UINT8:
						{
							OW_UInt8Array ra;
							convertCimType(ra, parser);
							rval = OW_CIMValue(ra);
							break;
						}
	
						case OW_CIMDataType::SINT8:
							{
								OW_Int8Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::UINT16:
							{
								OW_UInt16Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::SINT16:
							{
								OW_Int16Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::UINT32:
							{
								OW_UInt32Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::SINT32:
							{
								OW_Int32Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::UINT64:
							{
								OW_UInt64Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::SINT64:
							{
								OW_Int64Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::BOOLEAN:
							{
								OW_BoolArray ra;
								OW_StringArray sra;
								convertCimType(sra, parser);
								for(size_t i = 0; i < sra.size(); i++)
								{
									OW_Bool bv = sra[i].equalsIgnoreCase("TRUE");
									ra.append(bv);
								}
	
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::REAL32:
							{
								OW_Real32Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::REAL64:
							{
								OW_Real64Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::CHAR16:
							{
								OW_Char16Array ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::DATETIME:
							{
								OW_CIMDateTimeArray ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::STRING:
							{
								OW_StringArray ra;
								convertCimType(ra, parser);
								rval = OW_CIMValue(ra);
								break;
							}
	
						default:
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								"Invalid data type on node");
					}
	
					parser.mustGetEndTag(); // pass </VALUE.ARRAY>

					break;
				}
	
			case OW_CIMXMLParser::E_VALUE_REFARRAY:
				{
					OW_CIMObjectPathArray opArray;
					parser.getNextTag();
	
					while(parser.tokenIs(OW_CIMXMLParser::E_VALUE_REFERENCE))
					{
						OW_CIMObjectPath cop(OW_CIMNULL);
						OW_CIMValue v = createValue(parser, valueType);
						v.get(cop);
	
						opArray.append(cop);
					}
	
					rval = OW_CIMValue(opArray);
					parser.mustGetEndTag(); // pass <VALUE.REFARRAY>
					break;
				}
	
			case OW_CIMXMLParser::E_VALUE_REFERENCE:
				{
					parser.mustGetChild();
					OW_CIMObjectPath cop = createObjectPath(parser);
					parser.mustGetEndTag(); // pass </VALUE.REFERENCE>

					rval = OW_CIMValue(cop);
					break;
				}
	
			default:
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					"Not value XML");
		}
	
	}
	catch (const OW_StringConversionException& e)
	{
		// Workaround for SNIA client which sends <VALUE>NULL</VALUE> for NULL values
		if (parser.getData().equalsIgnoreCase("NULL"))
		{
			rval = OW_CIMValue(OW_CIMNULL);
			parser.getNextTag();
			parser.mustGetEndTag(); // pass <VALUE.REFARRAY>
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, e.getMessage());
		}
	}

	return rval;
}

///////////////////////////////////
OW_CIMQualifier
OW_XMLCIMFactory::createQualifier(OW_CIMXMLParser& parser)
{
	if (!parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not qualifier XML");
	}

	OW_CIMDataType dt(OW_CIMNULL);

	OW_String name = parser.mustGetAttribute(OW_XMLParameters::paramName);

	OW_String cimType = parser.getAttribute(OW_XMLParameters::paramTypeAssign);
	OW_String propagate = parser.getAttribute(OW_XMLParameters::paramPropagated);
	OW_String tosubclass = parser.getAttribute(OW_XMLParameters::paramToSubClass);

	OW_String overridable = parser.getAttribute(
		OW_XMLParameters::paramOverridable);

	//OW_String toinstance = parser.getAttribute(
	//	OW_XMLParameters::paramToInstance);

	OW_String translatable = parser.getAttribute(
		OW_XMLParameters::paramTranslatable);

	//
	// Build qualifier
	//
	if(!cimType.empty())
	{
		dt = OW_CIMDataType::getDataType(cimType);
	}

	if(!dt)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Qualifier not assigned a data type: %1", name).c_str());
	}

	OW_CIMQualifierType cqt;
	cqt.setDataType(dt);
	cqt.setName(name);
	
	OW_CIMQualifier rval(cqt);

	if(overridable.equalsIgnoreCase("false"))
	{
		rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE));
	}
	else
	{
		rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE));
	}

	if(tosubclass.equalsIgnoreCase("false"))
	{
		rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::RESTRICTED));
	}
	else
	{
		rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS));
	}

	//if(toinstance.equalsIgnoreCase("true"))
	//{
	//	rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE));
	//}

	if(translatable.equalsIgnoreCase("true"))
	{
		rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TRANSLATE));
	}

	rval.setPropagated(propagate.equalsIgnoreCase("true"));

	parser.mustGetNextTag();

	if(parser.tokenIs(OW_CIMXMLParser::E_VALUE_ARRAY)
		|| parser.tokenIs(OW_CIMXMLParser::E_VALUE))
	{
		rval.setValue(createValue(parser, cimType));
	}

	parser.mustGetEndTag();
	return rval;
}

///////////////////////////////////
OW_CIMMethod
OW_XMLCIMFactory::createMethod(OW_CIMXMLParser& parser)
{
	if(!parser.tokenIs(OW_CIMXMLParser::E_METHOD))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not method XML");
	}

	OW_String methodName = parser.mustGetAttribute(OW_XMLParameters::paramName);
	OW_String cimType = parser.getAttribute(OW_XMLParameters::paramTypeAssign);

	OW_String classOrigin = parser.getAttribute(
		OW_XMLParameters::paramClassOrigin);

	OW_String propagate = parser.getAttribute(OW_XMLParameters::paramPropagated);

	//
	// A method name must be given
	//
	if(methodName.empty())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"No method name in XML");
	}

	OW_CIMMethod rval;

	//
	// If no return data type, then method returns nothing (void)
	//
	if(!cimType.empty())
	{
		rval.setReturnType(OW_CIMDataType::getDataType(cimType));
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
	while (parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER))
	{
		rval.addQualifier(createQualifier(parser));
	}

	//
	// Handle parameters
	while (parser.tokenIs(OW_CIMXMLParser::E_PARAMETER)
		|| parser.tokenIs(OW_CIMXMLParser::E_PARAMETER_REFERENCE)
		|| parser.tokenIs(OW_CIMXMLParser::E_PARAMETER_ARRAY)
		|| parser.tokenIs(OW_CIMXMLParser::E_PARAMETER_REFARRAY))
	{
		rval.addParameter(createParameter(parser));
	}

	parser.mustGetEndTag();
	return rval;
}

static OW_CIMValue
convertXMLtoEmbeddedObject(const OW_String& str)
{
	OW_CIMValue rval(OW_CIMNULL);
	// try to convert the string to an class or instance
	OW_TempFileStream ostr;
	ostr << str;
	ostr.rewind();
	OW_CIMXMLParser parser(ostr);

	try
	{
		if (parser)
		{
			try
			{
				OW_CIMClass cc = OW_XMLCIMFactory::createClass(parser);
				rval = OW_CIMValue(cc);
			}
			catch (const OW_CIMException&)
			{
				// XML wasn't a class, so try an instance
				try
				{
					OW_CIMInstance ci = OW_XMLCIMFactory::createInstance(parser);
					rval = OW_CIMValue(ci);
				}
				catch (const OW_CIMException&)
				{
					// XML isn't a class or an instance, so just leave it alone
				}
			}
		}
	}
	catch (OW_XMLParseException& xmlE)
	{
	}
	return rval;
}

namespace
{
struct valueIsEmbeddedInstance
{
	bool operator()(const OW_CIMValue& v)
	{
		return v.getType() == OW_CIMDataType::EMBEDDEDINSTANCE;
	}
};

struct valueIsEmbeddedClass
{
	bool operator()(const OW_CIMValue& v)
	{
		return v.getType() == OW_CIMDataType::EMBEDDEDCLASS;
	}
};

bool isKnownEmbeddedObjectName(OW_String name)
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
OW_CIMProperty
OW_XMLCIMFactory::createProperty(OW_CIMXMLParser& parser)
{
	int token = parser.getToken();

	if(token != OW_CIMXMLParser::E_PROPERTY
		&& token != OW_CIMXMLParser::E_PROPERTY_ARRAY
		&& token != OW_CIMXMLParser::E_PROPERTY_REFERENCE)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "not property XML");
	}

	OW_String superClassName;
	OW_String inClassName;

	OW_String propName = parser.mustGetAttribute(OW_XMLParameters::paramName);
	OW_String cimType = parser.getAttribute(OW_XMLParameters::paramTypeAssign);
	OW_String classOrigin = parser.getAttribute(
		OW_XMLParameters::paramClassOrigin);
	OW_String propagate = parser.getAttribute(OW_XMLParameters::paramPropagated);

	OW_CIMProperty rval(propName);

	//
	// If no return data type, then property isn't properly defined
	//
	if(token == OW_CIMXMLParser::E_PROPERTY_REFERENCE)
	{
		rval.setDataType(OW_CIMDataType(parser.getAttribute(
			OW_XMLParameters::paramReferenceClass)));
	}
	else if(!cimType.empty())
	{
		rval.setDataType(OW_CIMDataType::getDataType(cimType));
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"property has null data type");
	}

	//
	// Array type property
	//
	if(token == OW_CIMXMLParser::E_PROPERTY_ARRAY)
	{
		OW_String arraySize = parser.getAttribute(
			OW_XMLParameters::paramArraySize);

		OW_CIMDataType dt = rval.getDataType();
		if (!arraySize.empty())
		{
			OW_Int32 aSize = 0;
			try
			{
				aSize = arraySize.toInt32();
			}
			catch (const OW_StringConversionException&)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED, format("Array size: \"%1\" is invalid", arraySize).c_str());
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

	while (parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER))
	{
		rval.addQualifier(createQualifier(parser));
	}

	if (parser.tokenIs(OW_CIMXMLParser::E_VALUE)
		|| parser.tokenIs(OW_CIMXMLParser::E_VALUE_ARRAY)
		|| parser.tokenIs(OW_CIMXMLParser::E_VALUE_REFERENCE))
	{
		rval.setValue(createValue(parser,cimType));
		
// Shouldn't need to cast this.
//         if(rval.getDataType().getType() != rval.getValue().getType()
//             || rval.getDataType().isArrayType() !=
//             rval.getValue().isArray())
//         {
//             rval.setValue(OW_CIMValueCast::castValueToDataType(
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
	if ((rval.hasTrueQualifier(OW_CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT) &&
		rval.getDataType().getType() == OW_CIMDataType::STRING &&
		rval.getValue()) ||
		isKnownEmbeddedObjectName(rval.getName())
		)
	{
		if (rval.getDataType().isArrayType())
		{
			OW_StringArray xmlstrings;
			rval.getValue().get(xmlstrings);
			OW_CIMValueArray values;
			for (size_t i = 0; i < xmlstrings.size(); ++i)
			{
				OW_CIMValue v = convertXMLtoEmbeddedObject(xmlstrings[i]);
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
					OW_CIMClassArray classes;
					for (size_t i = 0; i < values.size(); ++i)
					{
						OW_CIMClass c(OW_CIMNULL);
						values[i].get(c);
						classes.push_back(c);
					}
					rval.setValue(OW_CIMValue(classes));
					OW_CIMDataType dt(OW_CIMDataType::EMBEDDEDCLASS, rval.getDataType().getSize());
					rval.setDataType(dt);
				}
				else if (std::find_if(values.begin(), values.end(), valueIsEmbeddedClass()) == values.end())
				{
					// no classes, the all must be instances
					OW_CIMInstanceArray instances;
					for (size_t i = 0; i < values.size(); ++i)
					{
						OW_CIMInstance c(OW_CIMNULL);
						values[i].get(c);
						instances.push_back(c);
					}
					rval.setValue(OW_CIMValue(instances));
					OW_CIMDataType dt(OW_CIMDataType::EMBEDDEDINSTANCE, rval.getDataType().getSize());
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
			OW_CIMValue v = convertXMLtoEmbeddedObject(rval.getValue().toString());
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
OW_CIMParameter
OW_XMLCIMFactory::createParameter(OW_CIMXMLParser& parser)
{
	int paramToken = parser.getToken();
	
	if(paramToken != OW_CIMXMLParser::E_PARAMETER
		&& paramToken != OW_CIMXMLParser::E_PARAMETER_REFERENCE
		&& paramToken != OW_CIMXMLParser::E_PARAMETER_ARRAY
		&& paramToken != OW_CIMXMLParser::E_PARAMETER_REFARRAY)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not parameter XML");
	}
	
	//
	// Fetch name
	//
	OW_CIMParameter rval(parser.mustGetAttribute(OW_XMLParameters::paramName));
	
	//
	// Get parameter type
	//
	switch(paramToken)
	{
		case OW_CIMXMLParser::E_PARAMETER:
		{
			rval.setDataType(OW_CIMDataType::getDataType(
				parser.mustGetAttribute(OW_XMLParameters::paramTypeAssign)));
			break;
		}
	
		case OW_CIMXMLParser::E_PARAMETER_REFERENCE:
		{
			rval.setDataType(OW_CIMDataType(
				parser.getAttribute(OW_XMLParameters::paramRefClass)));
			break;
		}
	
		case OW_CIMXMLParser::E_PARAMETER_ARRAY:
		{
			OW_CIMDataType dt = OW_CIMDataType::getDataType(
				parser.mustGetAttribute(OW_XMLParameters::paramTypeAssign));
	
			if(!dt)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					"invalid parameter data type");
			}
	
			try
			{
				dt.setToArrayType(
					parser.getAttribute(OW_XMLParameters::paramArraySize).toInt32());
			}
			catch (const OW_StringConversionException&)
			{
				dt.setToArrayType(0);
			}
			rval.setDataType(dt);
			break;
		}
	
		case OW_CIMXMLParser::E_PARAMETER_REFARRAY:
		{
			OW_CIMDataType dt = OW_CIMDataType(
				parser.getAttribute(OW_XMLParameters::paramRefClass));
	
			try
			{
				dt.setToArrayType(
					parser.getAttribute(OW_XMLParameters::paramArraySize).toInt32());
			}
			catch (const OW_StringConversionException&)
			{
				dt.setToArrayType(0);
			}
			rval.setDataType(dt);
			break;
		}
	
		default:
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"could not decode parameter XML");
	}
	
	//
	// See if there are qualifiers
	//
	OW_CIMQualifierArray qualArray;
	parser.mustGetNextTag();
	while (parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER))
	{
		qualArray.append(createQualifier(parser));
	}
	rval.setQualifiers(qualArray);

	parser.mustGetEndTag();
	return rval;
}



