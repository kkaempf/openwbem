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
#include "OW_XMLCIMFactory.hpp"
#include "OW_XMLNode.hpp"
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
#include "OW_XMLParser.hpp"
#include "OW_XMLException.hpp"
#include "OW_TempFileStream.hpp"

#include <algorithm> // for find_if

static void
getLocalNameSpacePathAndSet(OW_CIMObjectPath& cop, const OW_XMLNode& node)
{
	OW_XMLNode lnode = node.mustChildFindElement(
		OW_XMLNode::XML_ELEMENT_LOCALNAMESPACEPATH);

	lnode = lnode.mustChildElement(OW_XMLNode::XML_ELEMENT_NAMESPACE);

	OW_String ns;
	while(lnode)	
	{
		OW_String nscomp = lnode.mustGetAttribute(OW_XMLAttribute::NAME);
		if(nscomp.length() > 0)
		{
			if(ns.length() != 0)
			{
				ns += "/";
			}

			ns += nscomp;
		}

		lnode = lnode.getNext();
	}

	if(ns.length() == 0)
	{
		ns = "root";
	}

	cop.setNameSpace(ns);
}

//////////////////////////////////////////////////////////////////////////////
static void
getNameSpacePathAndSet(OW_CIMObjectPath& cop, const OW_XMLNode& node)
{
	OW_XMLNode lnode = node.mustChildElement(
		OW_XMLNode::XML_ELEMENT_NAMESPACEPATH);

	OW_XMLNode cnode = lnode.mustChildElement(OW_XMLNode::XML_ELEMENT_HOST);
	cop.setHost(cnode.getText());
	getLocalNameSpacePathAndSet(cop, lnode);
}


///////////////////////////////////
OW_CIMObjectPath
OW_XMLCIMFactory::createObjectPath(OW_XMLNode const& pnode)
{
	if(!pnode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"NULL XML node on constructor");
	}

	OW_CIMObjectPath rval(true);

	OW_XMLNode node = pnode;

	int token = node.getToken();

	switch(token)
	{
		case OW_XMLNode::XML_ELEMENT_LOCALCLASSPATH:
			getLocalNameSpacePathAndSet(rval, node);
			node = node.mustChildFindElement(OW_XMLNode::XML_ELEMENT_CLASSNAME);
			rval.setObjectName(node.mustGetAttribute(OW_XMLAttribute::NAME));
			return rval;
		case OW_XMLNode::XML_ELEMENT_CLASSPATH:
			getNameSpacePathAndSet(rval, node); return rval;
		case OW_XMLNode::XML_ELEMENT_CLASSNAME:
			rval.setObjectName(node.mustGetAttribute(OW_XMLAttribute::NAME)); return rval;
		case OW_XMLNode::XML_ELEMENT_INSTANCENAME:
			OW_XMLClass::getInstanceName(node, rval); return rval;
		case OW_XMLNode::XML_ELEMENT_LOCALINSTANCEPATH:
			getLocalNameSpacePathAndSet(rval, node); break;
		case OW_XMLNode::XML_ELEMENT_INSTANCEPATH:
				getNameSpacePathAndSet(rval, node); break;
		default:
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("Invalid XMLNode for Object path construction.  Node "
						"name = %1", node.getNodeName()).c_str());
				break;
	}

	node = node.mustChildFindElement(OW_XMLNode::XML_ELEMENT_INSTANCENAME);

	OW_XMLClass::getInstanceName(node, rval);

	return rval;
}

///////////////////////////////////
OW_CIMClass
OW_XMLCIMFactory::createClass(OW_XMLNode const& node)
{
	OW_CIMClass rval(true);
	OW_ASSERT(node);
	OW_String superClassName;
	OW_String inClassName;

	OW_XMLNode valueNode =
		node.mustFindElement(OW_XMLNode::XML_ELEMENT_CLASS);

	inClassName = valueNode.mustGetAttribute(OW_XMLParameters::paramName);
	rval.setName(inClassName);

	superClassName = valueNode.getAttribute(OW_XMLParameters::paramSuperName);
	if(superClassName.length() > 0)
	{
		rval.setSuperClass(superClassName);
	}

	//
	// Find qualifier information
	//
	for(valueNode = valueNode.getChild();
		 valueNode != 0
		 && valueNode.getToken() == OW_XMLNode::XML_ELEMENT_QUALIFIER;
		 valueNode = valueNode.getNext())
	{
		OW_CIMQualifier cq = createQualifier(valueNode);
		if(cq.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_ASSOCIATION))
		{
			if (!cq.getValue()
				|| cq.getValue() != OW_CIMValue(false))
			{
				rval.setIsAssociation(true);
			}
		}

		rval.addQualifier(cq);
	}

	//
	// Load properties
	//
	for(;valueNode != 0; valueNode = valueNode.getNext())
	{
		int token = valueNode.getToken();

		if(token == OW_XMLNode::XML_ELEMENT_PROPERTY
			|| token == OW_XMLNode::XML_ELEMENT_PROPERTY_ARRAY
			|| token == OW_XMLNode::XML_ELEMENT_PROPERTY_REF)
		{
			rval.addProperty(createProperty(valueNode));
		}
		else
		{
			break;
		}
	}

	//
	// Load methods
	//
	for(;valueNode && valueNode.getToken() == OW_XMLNode::XML_ELEMENT_METHOD;
		 valueNode = valueNode.getNext())
	{
		rval.addMethod(createMethod(valueNode));
	}

	return rval;
}

///////////////////////////////////
OW_CIMInstance
OW_XMLCIMFactory::createInstance(OW_XMLNode const& node)
{
	OW_CIMInstance rval(true);
	if(node.getToken() != OW_XMLNode::XML_ELEMENT_INSTANCE)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not instance XML");
	}
		
	rval.setClassName(node.mustGetAttribute(OW_XMLAttribute::CLASS_NAME));

	//
	// Find qualifier information
	//
	OW_CIMQualifierArray quals;
	OW_XMLNode valueNode = node.getChild();
	for(; valueNode && valueNode.getToken() == OW_XMLNode::XML_ELEMENT_QUALIFIER;
		 valueNode = valueNode.getNext())
	{
		quals.append(createQualifier(valueNode));
	}
	rval.setQualifiers(quals);

	//
	// Load properties
	//
	OW_CIMPropertyArray props;
	for(;valueNode; valueNode = valueNode.getNext())
	{
		int token = valueNode.getToken();

		if(token != OW_XMLNode::XML_ELEMENT_PROPERTY
			&& token != OW_XMLNode::XML_ELEMENT_PROPERTY_ARRAY
			&& token != OW_XMLNode::XML_ELEMENT_PROPERTY_REF)
		{
			break;
		}

		props.append(createProperty(valueNode));
	}

	rval.setProperties(props);
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
convertCimType(OW_Array<T>& ra, const OW_XMLNode& node)
{
	OW_XMLNode valueNode = node.getChild();
	while(valueNode != 0)
	{
		if(valueNode.getToken() == OW_XMLNode::XML_ELEMENT_VALUE)
		{
			OW_String vstr = valueNode.getText();
			T val;
			StringToType(vstr, val);
			ra.append(val);
		}
		valueNode = valueNode.getNext();
	}
}

///////////////////////////////////
OW_CIMValue
OW_XMLCIMFactory::createValue(OW_XMLNode const& nodeArg,
	OW_String const& valueType)
{
	OW_CIMValue rval;

	if(!nodeArg)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Can't construct OW_CIMValue from NULL xml node");
	}

	try
	{
	
		OW_XMLNode node = nodeArg;
		int token = node.getToken();
	
		switch(token)
		{
			// <VALUE> elements
			case OW_XMLNode::XML_ELEMENT_VALUE:
				{
					OW_String vstr = node.getText();
						rval = OW_CIMValue::createSimpleValue(valueType, vstr);
					return rval;
				}
	
			// <VALUE.ARRAY> elements
			case OW_XMLNode::XML_ELEMENT_VALUE_ARRAY:
				{
					int type = OW_CIMDataType::strToSimpleType(valueType);
					if(type == OW_CIMDataType::INVALID)
					{
						OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"Invalid data type on node");
					}
	
					switch(type)
					{
						case OW_CIMDataType::UINT8:
						{
							OW_UInt8Array ra;
							convertCimType(ra, node);
							rval = OW_CIMValue(ra);
							break;
						}
	
						case OW_CIMDataType::SINT8:
							{
								OW_Int8Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::UINT16:
							{
								OW_UInt16Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::SINT16:
							{
								OW_Int16Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::UINT32:
							{
								OW_UInt32Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::SINT32:
							{
								OW_Int32Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::UINT64:
							{
								OW_UInt64Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::SINT64:
							{
								OW_Int64Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::BOOLEAN:
							{
								OW_BoolArray ra;
								OW_StringArray sra;
								convertCimType(sra, node);
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
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::REAL64:
							{
								OW_Real64Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::CHAR16:
							{
								OW_Char16Array ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::DATETIME:
							{
								OW_CIMDateTimeArray ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						case OW_CIMDataType::STRING:
							{
								OW_StringArray ra;
								convertCimType(ra, node);
								rval = OW_CIMValue(ra);
								break;
							}
	
						default:
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								"Invalid data type on node");
					}
	
					break;
				}
	
			case OW_XMLNode::XML_ELEMENT_VALUE_REFARRAY:
				{
					OW_CIMObjectPathArray opArray;
					node = node.mustChildElement(
						OW_XMLNode::XML_ELEMENT_VALUE_REFERENCE);
	
					while(node)
					{
						OW_CIMObjectPath cop(OW_Bool(true));
						OW_XMLNode valueNode = node.mustGetChild();
	
						token = valueNode.getToken();
						switch(token)
						{
							case OW_XMLNode::XML_ELEMENT_CLASSPATH:
							case OW_XMLNode::XML_ELEMENT_LOCALCLASSPATH:
							case OW_XMLNode::XML_ELEMENT_INSTANCEPATH:
							case OW_XMLNode::XML_ELEMENT_LOCALINSTANCEPATH:
								cop = createObjectPath(valueNode);
								break;
							case OW_XMLNode::XML_ELEMENT_CLASSNAME:
								cop.setObjectName(node.mustGetAttribute(OW_XMLAttribute::NAME));
								break;
							case OW_XMLNode::XML_ELEMENT_INSTANCENAME:
								OW_XMLClass::getInstanceName(valueNode, cop);
								break;
							default:
								OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
									"Attempting to extract object path");
						}
	
						opArray.append(cop);
						node = node.getNext();
					}
	
					rval = OW_CIMValue(opArray);
				}
	
			case OW_XMLNode::XML_ELEMENT_VALUE_REFERENCE:
				{
					OW_CIMObjectPath cop(OW_Bool(true));
					OW_XMLNode valueNode = node.getChild();
	
					token = valueNode.getToken();
					switch(token)
					{
						case OW_XMLNode::XML_ELEMENT_CLASSPATH:
						case OW_XMLNode::XML_ELEMENT_LOCALCLASSPATH:
						case OW_XMLNode::XML_ELEMENT_INSTANCEPATH:
						case OW_XMLNode::XML_ELEMENT_LOCALINSTANCEPATH:
							cop = createObjectPath(valueNode);
							break;
						case OW_XMLNode::XML_ELEMENT_CLASSNAME:
							cop.setObjectName(node.mustGetAttribute(OW_XMLAttribute::NAME));
							break;
						case OW_XMLNode::XML_ELEMENT_INSTANCENAME:
							OW_XMLClass::getInstanceName(valueNode, cop);
							break;
						default:
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								"Attempting to extract object path");
					}
	
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
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, e.getMessage());
	}

	return rval;
}

///////////////////////////////////
OW_CIMQualifier
OW_XMLCIMFactory::createQualifier(OW_XMLNode const& node)
{
	OW_CIMQualifier rval(true);

	OW_XMLNode qnode = node;
	OW_CIMDataType dt;

	if(qnode.getToken() != OW_XMLNode::XML_ELEMENT_QUALIFIER)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not qualifier XML");
	}

	OW_String name = qnode.getAttribute(OW_XMLParameters::paramName);

	OW_String cimType = qnode.getAttribute(OW_XMLParameters::paramTypeAssign);
	OW_String propagate = qnode.getAttribute(OW_XMLParameters::paramPropagated);
	OW_String tosubclass = qnode.getAttribute(OW_XMLParameters::paramToSubClass);

	OW_String overridable = qnode.getAttribute(
		OW_XMLParameters::paramOverridable);

	OW_String toinstance = qnode.getAttribute(
		OW_XMLParameters::paramToInstance);

	OW_String translatable = qnode.getAttribute(
		OW_XMLParameters::paramTranslatable);

	//
	// Build qualifier
	//
	if(cimType.length() > 0)
	{
		dt = OW_CIMDataType::getDataType(cimType);
	}

	if(!dt)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Qualifier not assigned a data type: %1", name).c_str());
	}

	OW_CIMQualifierType cqt(OW_Bool(true));
	cqt.setDataType(dt);
	cqt.setName(name);
	rval = OW_CIMQualifier(cqt);

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

	if(toinstance.equalsIgnoreCase("true"))
	{
		rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE));
	}

	if(translatable.equalsIgnoreCase("true"))
	{
		rval.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TRANSLATE));
	}

	rval.setPropagated(propagate.equalsIgnoreCase("true"));

	qnode = qnode.getChild();
	if(!qnode)
	{
		return rval; // no values
	}

	OW_XMLNode valueNode;
	if((valueNode = qnode.findElement(OW_XMLNode::XML_ELEMENT_VALUE_ARRAY)))
	{
		rval.setValue(createValue(valueNode, cimType));
	}
	else if((valueNode = qnode.findElement(OW_XMLNode::XML_ELEMENT_VALUE)))
	{
		rval.setValue(createValue(valueNode, cimType));
	}
	else if((valueNode = qnode.findElement(
		OW_XMLNode::XML_ELEMENT_VALUE_REFERENCE)))
	{
		rval.setValue(createValue(valueNode, cimType));
	}

	return rval;
}

///////////////////////////////////
OW_CIMMethod
OW_XMLCIMFactory::createMethod(OW_XMLNode const& node)
{
	OW_CIMMethod rval(true);

	if(!node || node.getToken() != OW_XMLNode::XML_ELEMENT_METHOD)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not method XML");
	}

	OW_XMLNode nextchild;

	OW_String methodName = node.getAttribute(OW_XMLParameters::paramName);
	OW_String cimType = node.getAttribute(OW_XMLParameters::paramTypeAssign);

	OW_String classOrigin = node.getAttribute(
		OW_XMLParameters::paramClassOrigin);

	OW_String propagate = node.getAttribute(OW_XMLParameters::paramPropagated);

	//
	// A method name must be given
	//
	if(methodName.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"No method name in XML");
	}

	//
	// If no return data type, then method returns nothing (void)
	//
	if(cimType.length() != 0)
	{
		rval.setReturnType(OW_CIMDataType::getDataType(cimType));
	}

	rval.setName(methodName);

	if(classOrigin.length() != 0)
	{
		rval.setOriginClass(classOrigin);
	}

	rval.setPropagated(propagate.equalsIgnoreCase("true"));

	//
	// See if there are qualifiers
	//
	for(nextchild = node.getChild();
		 nextchild && nextchild.getToken() == OW_XMLNode::XML_ELEMENT_QUALIFIER;
		 nextchild = nextchild.getNext())
	{
		rval.addQualifier(createQualifier(nextchild));
	}

	//
	// Handle parameters
	//OW_XMLCreateClass::execute
	for(;nextchild; nextchild = nextchild.getNext())
	{
		int paramToken = nextchild.getToken();

		if(paramToken == OW_XMLNode::XML_ELEMENT_PARAMETER
			|| paramToken == OW_XMLNode::XML_ELEMENT_PARAMETER_REFERENCE
			|| paramToken == OW_XMLNode::XML_ELEMENT_PARAMETER_ARRAY
			|| paramToken == OW_XMLNode::XML_ELEMENT_PARAMETER_REFARRAY)
		{
			rval.addParameter(createParameter(nextchild));
		}
		else
		{
			break;
		}
	}

	return rval;
}

static OW_CIMValue
convertXMLtoEmbeddedObject(const OW_String& str)
{
	OW_CIMValue rval;
	// try to convert the string to an class or instance
	OW_TempFileStream ostr;
	ostr << str;
	ostr.rewind();
	OW_XMLParser parser(&ostr);

	OW_XMLNode node;
	try
	{
		node = parser.parse();
	}
	catch (OW_XMLException& xmlE)
	{
		// XML is no good, leave node null
	}
	if (node)
	{
		try
		{
			OW_CIMClass cc = OW_XMLCIMFactory::createClass(node);
			rval = OW_CIMValue(cc);
		}
		catch (const OW_CIMException&)
		{
			// XML wasn't a class, so try an instance
			try
			{
				OW_CIMInstance ci = OW_XMLCIMFactory::createInstance(node);
				rval = OW_CIMValue(ci);
			}
			catch (const OW_CIMException&)
			{
				// XML isn't a class or an instance, so just leave it alone
			}
		}
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

}

///////////////////////////////////
OW_CIMProperty
OW_XMLCIMFactory::createProperty(OW_XMLNode const& node)
{
	OW_CIMProperty rval(true);

	OW_String superClassName;
	OW_String inClassName;
	OW_XMLNode qnode = node;

	int token = qnode.getToken();

	if(token != OW_XMLNode::XML_ELEMENT_PROPERTY
		&& token != OW_XMLNode::XML_ELEMENT_PROPERTY_ARRAY
		&& token != OW_XMLNode::XML_ELEMENT_PROPERTY_REF)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "not property XML");
	}

	OW_String propName = qnode.getAttribute(OW_XMLParameters::paramName);
	OW_String cimType = qnode.getAttribute(OW_XMLParameters::paramTypeAssign);
	OW_String classOrigin = qnode.getAttribute(
		OW_XMLParameters::paramClassOrigin);
	OW_String propagate = qnode.getAttribute(OW_XMLParameters::paramPropagated);

	if(propName.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "no property name");
	}

	rval.setName(propName);

	//
	// If no return data type, then property isn't properly defined
	//
	if(token == OW_XMLNode::XML_ELEMENT_PROPERTY_REF)
	{
		rval.setDataType(OW_CIMDataType(qnode.getAttribute(
			OW_XMLParameters::paramReferenceClass)));
	}
	else if(cimType.length() != 0)
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
	if(token == OW_XMLNode::XML_ELEMENT_PROPERTY_ARRAY)
	{
		OW_String arraySize = qnode.getAttribute(
			OW_XMLParameters::paramArraySize);

		OW_CIMDataType dt = rval.getDataType();
		if (arraySize.length())
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
	rval.setPropagated(propagate.length() != 0 && propagate.equalsIgnoreCase("true"));

	//
	// See if there are qualifiers
	//
	for(qnode = qnode.getChild();
		 qnode && qnode.getToken() == OW_XMLNode::XML_ELEMENT_QUALIFIER;
		 qnode = qnode.getNext())
	{
		rval.addQualifier(createQualifier(qnode));
	}

	if(qnode)
	{
		rval.setValue(createValue(qnode,cimType));
		
		if(rval.getDataType().getType() != rval.getValue().getType()
			|| rval.getDataType().isArrayType() !=
			rval.getValue().isArray())
		{
			rval.setValue(OW_CIMValueCast::castValueToDataType(
				rval.getValue(), rval.getDataType()));
		}
	}

	// handle embedded class or instance
	if (rval.getQualifier(OW_CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT) &&
		rval.getDataType().getType() == OW_CIMDataType::STRING &&
		rval.getValue())
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
						OW_CIMClass c;
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
						OW_CIMInstance c;
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

	return rval;
}


///////////////////////////////////
OW_CIMParameter
OW_XMLCIMFactory::createParameter(OW_XMLNode const& node)
{
	OW_CIMParameter rval(true);

	OW_XMLNode qnode = node;
	int paramToken = qnode.getToken();
	
	if(paramToken != OW_XMLNode::XML_ELEMENT_PARAMETER
		&& paramToken != OW_XMLNode::XML_ELEMENT_PARAMETER_REFERENCE
		&& paramToken != OW_XMLNode::XML_ELEMENT_PARAMETER_ARRAY
		&& paramToken != OW_XMLNode::XML_ELEMENT_PARAMETER_REFARRAY)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Not parameter XML");
	}
	
	//
	// Fetch name
	//
	rval.setName(qnode.getAttribute(OW_XMLParameters::paramName));
	
	//
	// Get parameter type
	//
	switch(paramToken)
	{
		case OW_XMLNode::XML_ELEMENT_PARAMETER:
		{
			rval.setDataType(OW_CIMDataType::getDataType(
				qnode.getAttribute(OW_XMLParameters::paramTypeAssign)));
			break;
		}
	
		case OW_XMLNode::XML_ELEMENT_PARAMETER_REFERENCE:
		{
			rval.setDataType(OW_CIMDataType(
				qnode.getAttribute(OW_XMLParameters::paramRefClass)));
			break;
		}
	
		case OW_XMLNode::XML_ELEMENT_PARAMETER_ARRAY:
		{
			OW_CIMDataType dt = OW_CIMDataType::getDataType(
				qnode.getAttribute(OW_XMLParameters::paramTypeAssign));
	
			if(!dt)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					"invalid parameter data type");
			}
	
			try
			{
				dt.setToArrayType(
					qnode.getAttribute(OW_XMLParameters::paramArraySize).toInt32());
			}
			catch (const OW_StringConversionException&)
			{
				dt.setToArrayType(0);
			}
			rval.setDataType(dt);
			break;
		}
	
		case OW_XMLNode::XML_ELEMENT_PARAMETER_REFARRAY:
		{
			OW_CIMDataType dt = OW_CIMDataType(
				qnode.getAttribute(OW_XMLParameters::paramRefClass));
	
			try
			{
				dt.setToArrayType(
					qnode.getAttribute(OW_XMLParameters::paramArraySize).toInt32());
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
	for(qnode = qnode.getChild();
		 qnode && qnode.getToken() == OW_XMLNode::XML_ELEMENT_QUALIFIER;
		 qnode = qnode.getNext())
	{
		qualArray.append(createQualifier(qnode));
	}
	rval.setQualifiers(qualArray);

	return rval;
}



