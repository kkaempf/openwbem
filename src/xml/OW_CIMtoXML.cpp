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
#include "OW_CIMtoXML.hpp"
#include "OW_String.hpp"
#include "OW_CIM.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_Assertion.hpp"
#include "OW_StringStream.hpp"

#include <iostream>
#include <algorithm>

using std::ostream;

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMNameSpace const& ns, ostream& ostr,
	OW_CIMtoXMLFlags::do_local_flag const& dolocal)
{
	OW_String name = ns.getNameSpace();

	if(name.length() == 0)
		OW_THROW(OW_CIMMalformedUrlException, "Namespace not set");

	if(dolocal == OW_CIMtoXMLFlags::dontDoLocal)
	{
		ostr
			<< "<NAMESPACEPATH><HOST>"
			<< OW_XMLEscape(ns.getHostUrl().getHost())
			<< "</HOST>";
	}

	ostr << "<LOCALNAMESPACEPATH>";

	for (;;)
	{
		int index = name.indexOf('/');

		if (index == -1)
			break;

		if(index != 0)
		{
			ostr
				<< "<NAMESPACE NAME=\""
				<< OW_XMLEscape(name.substring(0, index))
            << "\"></NAMESPACE>";
		}

		name = name.substring(index+1);
	}
	
	ostr
		<< "<NAMESPACE NAME=\""
		<< OW_XMLEscape(name)
		<< "\"></NAMESPACE>"
		<< "</LOCALNAMESPACEPATH>";
	
	if (dolocal == OW_CIMtoXMLFlags::dontDoLocal)
	{
		ostr << "</NAMESPACEPATH>";
	}
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMQualifierType const& cqt, ostream& ostr)
{
	OW_CIMFlavor fv;

	if(cqt.getName().length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "qualifierType must have a name");
	}

	//
	// If type isn't set then the CIMOM has stored a qualifier
	// thats bad and an exception is generated
	//
	if(!cqt.getDataType())
	{
		OW_String msg("QualifierType (");
		msg += cqt.getName();
		msg += ") does not have a data type set";
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	ostr
		<< "<QUALIFIER.DECLARATION NAME=\""
		<< cqt.getName()
		<< "\" TYPE=\"";

	OW_CIMtoXML(cqt.getDataType(), ostr);
	ostr << "\" ";

	if(cqt.getDataType().isArrayType())
	{
		ostr << "ISARRAY=\"true\" ";
	}
	else
	{
		ostr << "ISARRAY=\"false\" ";
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE);
	if(cqt.hasFlavor(fv))
	{
		// NOT NECESSARY, default is TRUE
		// text += fv.toXML() + "=\"true\" ";
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE);
		if(cqt.hasFlavor(fv))
		{
			//fv.toXML(ostr);
			OW_CIMtoXML(fv, ostr);
			ostr << "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS);
	if(cqt.hasFlavor(fv))
	{
		// NOT NECESSARY, default is TRUE
		// text += fv.toXML() + "=\"true\" ";
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::RESTRICTED);
		if(cqt.hasFlavor(fv))
		{
			//fv.toXML(ostr);
			OW_CIMtoXML(fv, ostr);
			ostr << "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE);
	if(cqt.hasFlavor(fv))
	{
		//fv.toXML(ostr);
		OW_CIMtoXML(fv, ostr);
		ostr << "=\"true\" ";
	}
	else
	{
		//
		// Not needed, because TOINSTANCE defaults to false!
		//text += fv.toXML() + "=\"false\" ";
	}


	fv = OW_CIMFlavor(OW_CIMFlavor::TRANSLATE);
	if(cqt.hasFlavor(fv))
	{
		//fv.toXML(ostr);
		OW_CIMtoXML(fv, ostr);
		ostr << "=\"true\" ";
	}
	//
	// NOT NECESSARY, default is FALSE
	//else
	//  text += fv.toXML() + "=\"false\" ";

	ostr << "><SCOPE ";

	//
	// Write scope information
	//
	OW_String scope;
	OW_Bool scopeWritten = false;
	OW_Bool any = cqt.hasScope(OW_CIMScope(OW_CIMScope::ANY));
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::CLASS)))
	{
		ostr << "CLASS=\"true\" ";
		scopeWritten = true;
	}
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::INSTANCE)))
	{
		ostr << "INSTANCE=\"true\" ";
		scopeWritten = true;
	}
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::ASSOCIATION)))
	{
		ostr << "ASSOCIATION=\"true\" ";
		scopeWritten = true;
	}
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::REFERENCE)))
	{
		ostr << "REFERENCE=\"true\" ";
		scopeWritten = true;
	}
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::PROPERTY)))
	{
		ostr << "PROPERTY=\"true\" ";
		scopeWritten = true;
	}
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::METHOD)))
	{
		ostr << "METHOD=\"true\" ";
		scopeWritten = true;
	}
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::PARAMETER)))
	{
		ostr << "PARAMETER=\"true\" ";
		scopeWritten = true;
	}
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::INDICATION)))
	{
		ostr << "INDICATION=\"true\" ";
		scopeWritten = true;
	}

	if(!scopeWritten)
	{
		OW_String msg("Scope not set on qaulifier type: ");
		msg += cqt.getName();
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	ostr << "></SCOPE>";

	if(cqt.getDefaultValue())
	{
		//m_pdata->m_defaultValue.toXML(ostr);
		OW_CIMtoXML(cqt.getDefaultValue(), ostr);
	}

	ostr << "</QUALIFIER.DECLARATION>";
}

static void
outputKEYVALUE(ostream& ostr, const OW_CIMProperty& cp)
{
	OW_CIMDataType dtype = cp.getDataType();
	OW_String type;

	if(dtype.isArrayType())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"An array cannot be a KEY");
	}

	if(dtype.isReferenceType())
	{
		//cp.getValue().toXML(ostr);
		OW_CIMtoXML(cp.getValue(),ostr);
		return;
	}

	//
	// Regular key value
	switch(dtype.getType())
	{
		case OW_CIMDataType::CHAR16:
		case OW_CIMDataType::DATETIME:
		case OW_CIMDataType::STRING:
			type = "string";
			break;
		case OW_CIMDataType::BOOLEAN:
			type = "boolean";
			break;
		default:
			type = "numeric";
	}

	OW_CIMValue keyValue = cp.getValue();
	if(!keyValue)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "No key value");
	}

	ostr
		<< "<KEYVALUE VALUETYPE=\""
		<<  type
		<< "\">"
		<< OW_XMLEscape(keyValue.toString())
		<< "</KEYVALUE>";
}


//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMObjectPath const& cop, ostream& ostr,
	OW_CIMtoXMLFlags::is_instance_name_flag const& isInstanceName)
{
	//
	// Instance path
	//
	if (isInstanceName == OW_CIMtoXMLFlags::isNotInstanceName)
	{
		ostr << "<INSTANCEPATH>";
		//m_pdata->m_nameSpace.toXML(ostr, false);
		OW_CIMtoXML(cop.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::dontDoLocal);
	}

	ostr << "<INSTANCENAME CLASSNAME=\"";
	ostr << cop.getObjectName() << "\">";


	if(cop.getKeys().size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"No keys available in object path");
	}

	size_t numkeys = cop.getKeys().size();

	//
	// If keys > 1 then must use KEYBINDING - we also use it for
	// the key == 1 case - most implementations can't cope with
	// a single KEYVALUE without a KEYBINDING element
	//
	if(numkeys > 0)
	{
		for(size_t i = 0; i < numkeys; i++)
		{
			OW_CIMProperty cp = cop.getKeys()[i];
			ostr << "<KEYBINDING NAME=\"";
			ostr << cp.getName() << "\">";
			outputKEYVALUE(ostr, cp);
			ostr << "</KEYBINDING>";
		}
	}
	else
	{
		//
		// No keys, so no instances
		//
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"No instance path because no keys");
	}

	ostr << "</INSTANCENAME>";

	if (isInstanceName == OW_CIMtoXMLFlags::isNotInstanceName)
	{
		ostr << "</INSTANCEPATH>";
	}
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMClass const& cc, ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin,
	OW_StringArray const& propertyList,
	bool noProps)
{
	if(cc.getName().length() == 0)
	{
		OW_THROW(OW_CIMMalformedUrlException, "class must have name");
	}

	ostr << "<CLASS NAME=\"";
	ostr << cc.getName();
	if(cc.getSuperClass().length() != 0)
	{
		ostr << "\" SUPERCLASS=\"";
		ostr << cc.getSuperClass();
	}
	ostr << "\">";

	//
	// Process qualifiers
	//
	/*
	 * If it is an association, we ignore the localOnly flag
	 * we probably should do the same with indications, but currently
	 * have no isIndication() flag!  TODO
	 */
	//OW_CIMQualifierArray::const_iterator iter = std::find(
	//	cc.getQualifiers().begin(), cc.getQualifiers().end(),
	//	OW_CIMQualifier(OW_CIMQualifier::CIM_QUAL_ASSOCIATION));
	if (cc.isAssociation() &&
		std::find(cc.getQualifiers().begin(), cc.getQualifiers().end(),
		OW_CIMQualifier(OW_CIMQualifier::CIM_QUAL_ASSOCIATION)) == cc.getQualifiers().end())
	{
		ostr << "<QUALIFIER NAME=\"Association\" TYPE=\"boolean\" ";
		if (localOnly == OW_CIMtoXMLFlags::localOnly)
		{
			ostr << "PROPAGATED=\"true\" ";
		}
		ostr << "OVERRIDABLE=\"false\" ><VALUE>true</VALUE> </QUALIFIER>";
	}
	if(includeQualifiers == OW_CIMtoXMLFlags::includeQualifiers)
	{
		for(size_t i = 0; i < cc.getQualifiers().size(); i++)
		{
			OW_CIMQualifier q(cc.getQualifiers()[i]);
			/*
			if (q.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_ASSOCIATION))
			{
				if (q.getValue() && q.getValue().getType() == OW_CIMDataType::BOOLEAN)
				{
					OW_Bool b;
					q.getValue.get(b);
					if (b)
					{
						continue;
					}
				}
			}
			*/
			OW_CIMtoXML(q, ostr, localOnly);
		}
	}

	if(!noProps)
	{
		for(size_t i = 0; i < cc.getAllProperties().size(); i++)
		{
			OW_CIMProperty prop = cc.getAllProperties()[i];

			// If the given property list has any elements, then ensure this
			// property name is in the property list before including it's xml
			if(propertyList.size() > 0)
			{
				OW_String pName = prop.getName();
				for(size_t j = 0; j < propertyList.size(); j++)
				{
					if(pName.equalsIgnoreCase(propertyList[j]))
					{
						//prop.toXML(ostr, localOnly, includeQualifiers,
						//	includeClassOrigin);
						OW_CIMtoXML(prop, ostr, localOnly, includeQualifiers,
							includeClassOrigin);
						break;
					}
				}
			}
			else
			{
				//prop.toXML(ostr, localOnly, includeQualifiers,
				//	includeClassOrigin);
				OW_CIMtoXML(prop, ostr, localOnly, includeQualifiers,
					includeClassOrigin);
			}
		}
	}

	// Process methods
	for(size_t i = 0; i < cc.getAllMethods().size(); i++)
	{
		//m_pdata->m_methods[i].toXML(ostr, localOnly, includeQualifiers,
		//	includeClassOrigin);
		OW_CIMtoXML(cc.getAllMethods()[i], ostr, localOnly,includeQualifiers,
			includeClassOrigin);
	}

	ostr << "</CLASS>";
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMInstance const& ci, ostream& ostr,
	OW_CIMObjectPath const& cop,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin,
	OW_StringArray const& propertyList,
	bool noProps)
{
	if(cop)
	{
		OW_CIMtoXML(cop, ostr, OW_CIMtoXMLFlags::isNotInstanceName);
	}

	//
	// NOTE: don't write the object path here!
	//
	if(ci.getClassName().length() != 0)
	{
		ostr << "<INSTANCE CLASSNAME=\"";
		ostr << ci.getClassName() << "\">";
	}
	else
	{
		OW_THROW(OW_CIMMalformedUrlException, "instance has no class name");
	}

	//
	// Process qualifiers
	//
	if(includeQualifiers == OW_CIMtoXMLFlags::includeQualifiers)
	{
		for(size_t i = 0; i < ci.getQualifiers().size(); i++)
		{
			//m_pdata->m_qualifiers[i].toXML(ostr, localOnly);
			OW_CIMtoXML(ci.getQualifiers()[i], ostr, localOnly);
		}
	}

	if(!noProps)
	{
		OW_CIMPropertyArray pra = ci.getProperties();
		for(size_t i = 0; i < pra.size(); i++)
		{
			OW_CIMProperty prop = pra[i];

			//
			// If propertyList is not NULL then check this is a request property
			//
			if(propertyList.size() != 0)
			{
				OW_String pName = prop.getName();
				for(size_t j = 0; j < propertyList.size(); j++)
				{
					if(pName.equalsIgnoreCase(propertyList[j]))
					{
						//prop.toXML(ostr, localOnly, includeQualifiers,
						//	includeClassOrigin);
						OW_CIMtoXML(prop,ostr,localOnly ,includeQualifiers,
							includeClassOrigin);
						break;
					}
				}
			}
			else
			{
				//prop.toXML(ostr, localOnly, includeQualifiers,
				//	includeClassOrigin);
				OW_CIMtoXML(prop,ostr,localOnly ,includeQualifiers,
					includeClassOrigin);
			}
		}
	}

	ostr << "</INSTANCE>";
}


//////////////////////////////////////////////////////////////////////////////
template<class T>
void raToXml(ostream& out, const OW_Array<T>& ra)
{
	out << "<VALUE.ARRAY>\n";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << ra[i];
		out << "</VALUE>\n";
	}
	out << "</VALUE.ARRAY>\n";
}

template <typename T>
static void valueToXML(T const& x, ostream& out)
{
	OW_CIMtoXML(x, out);
}

static void valueToXML(OW_CIMObjectPath const& x, ostream& out)
{
	OW_CIMtoXML(x, out, OW_CIMtoXMLFlags::isNotInstanceName);
}

static void raToXmlCOP(ostream& out, const OW_Array<OW_CIMObjectPath>& ra)
{
	out << "<VALUE.REFARRAY>";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE.REFERENCE>";
		valueToXML(ra[i], out);
		out << "</VALUE.REFERENCE>";
	}
	out << "</VALUE.REFARRAY>";
}

static void raToXmlSA(ostream& out, const OW_Array<OW_String>& ra)
{
	out << "<VALUE.ARRAY>\n";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << OW_XMLEscape(ra[i]);
		out << "</VALUE>\n";
	}
	out << "</VALUE.ARRAY>\n";
}

static void raToXmlChar16(ostream& out, const OW_Array<OW_Char16>& ra)
{
	char bfr[20];
	out << "<VALUE.ARRAY>\n";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << OW_Char16::xmlExcape(ra[i].getValue(), bfr);
		out << "</VALUE>\n";
	}
	out << "</VALUE.ARRAY>\n";
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMValue const& cv, ostream& out)
{
	if(cv.isArray())
	{
		switch(cv.getType())
		{
			case OW_CIMDataType::BOOLEAN:
			{
				OW_BoolArray a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::UINT8:
			{
				OW_UInt8Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::SINT8:
			{
				OW_Int8Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			// ATTN: UTF8
			case OW_CIMDataType::CHAR16:
			{
				OW_Char16Array a;
				cv.get(a);
				raToXmlChar16(out, a);
				break;
			}

			case OW_CIMDataType::UINT16:
			{
				OW_UInt16Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::SINT16:
			{
				OW_Int16Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::UINT32:
			{
				OW_UInt32Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::SINT32:
			{
				OW_Int32Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::UINT64:
			{
				OW_UInt64Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::SINT64:
			{
				OW_Int64Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::REAL32:
			{
				OW_Real32Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::REAL64:
			{
				OW_Real64Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::STRING:
			{
				OW_StringArray a;
				cv.get(a);
				raToXmlSA(out, a);
				break;
			}

			case OW_CIMDataType::DATETIME:
			{
				OW_CIMDateTimeArray a;
				cv.get(a);
				raToXml(out, a);
				break;
			}

			case OW_CIMDataType::REFERENCE:
			{
				OW_CIMObjectPathArray a;
				cv.get(a);
				raToXmlCOP(out, a);
				break;
			}
			
			case OW_CIMDataType::EMBEDDEDCLASS:
			{
				OW_CIMClassArray ca;
				cv.get(ca);
				OW_StringArray sa;
				for (size_t i = 0; i < ca.size(); ++i)
				{
					OW_StringStream ss;
					OW_CIMtoXML(ca[i], ss, OW_CIMtoXMLFlags::notLocalOnly,
						OW_CIMtoXMLFlags::includeQualifiers,
						OW_CIMtoXMLFlags::includeClassOrigin,
						OW_StringArray());
					sa.push_back(ss.toString());
				}
				raToXmlSA(out, sa);
				break;
			}
			
			case OW_CIMDataType::EMBEDDEDINSTANCE:
			{
				OW_CIMInstanceArray ia;
				cv.get(ia);
				OW_StringArray sa;
				for (size_t i = 0; i < ia.size(); ++i)
				{
					OW_StringStream ss;
					OW_CIMtoXML(ia[i],ss,OW_CIMObjectPath(),
						OW_CIMtoXMLFlags::notLocalOnly,
						OW_CIMtoXMLFlags::includeQualifiers,
						OW_CIMtoXMLFlags::includeClassOrigin,
						OW_StringArray());
					sa.push_back(ss.toString());
				}
				raToXmlSA(out, sa);
				break;
			}
			default:
				OW_ASSERT(0);
		}
	}
	else if(cv.getType() == OW_CIMDataType::REFERENCE)
	{
		out << "<VALUE.REFERENCE>";
		OW_CIMObjectPath a;
		cv.get(a);
		OW_CIMtoXML(a, out, OW_CIMtoXMLFlags::isNotInstanceName);
		out << "</VALUE.REFERENCE>";
	}
	else
	{
		out << "<VALUE>";

		switch(cv.getType())
		{
			case OW_CIMDataType::BOOLEAN:
			{
				OW_Bool a;
				cv.get(a);
				out << a.toString();
				break;
			}

			case OW_CIMDataType::UINT8:
			{
				OW_UInt8 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::SINT8:
			{
				OW_Int8 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::CHAR16:
			{
				char bfr[20];
				OW_Char16 a;
				cv.get(a);
				out << OW_Char16::xmlExcape(a.getValue(), bfr);
				break;
			}

			case OW_CIMDataType::UINT16:
			{
				OW_UInt16 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::SINT16:
			{
				OW_Int16 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::UINT32:
			{
				OW_UInt32 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::SINT32:
			{
				OW_Int32 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::UINT64:
			{
				OW_UInt64 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::SINT64:
			{
				OW_Int64 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::REAL32:
			{
				OW_Real32 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::REAL64:
			{
				OW_Real64 a;
				cv.get(a);
				out << a;
				break;
			}

			case OW_CIMDataType::STRING:
			{
				OW_String a;
				cv.get(a);
				out << OW_XMLEscape(a);
				break;
			}

			case OW_CIMDataType::DATETIME:
			{
				OW_CIMDateTime a;
				cv.get(a);
				out << a.toString();
				break;
			}
			
			case OW_CIMDataType::EMBEDDEDCLASS:
			{
				OW_CIMClass cc;
				cv.get(cc);
				OW_String s;
				OW_StringStream ss;
				OW_CIMtoXML(cc, ss, OW_CIMtoXMLFlags::notLocalOnly,
					OW_CIMtoXMLFlags::includeQualifiers,
					OW_CIMtoXMLFlags::includeClassOrigin,
					OW_StringArray());
				out << OW_XMLEscape(ss.toString());
				break;
			}
			
			case OW_CIMDataType::EMBEDDEDINSTANCE:
			{
				OW_CIMInstance i;
				cv.get(i);
				OW_String s;
				OW_StringStream ss;
				OW_CIMtoXML(i,ss,OW_CIMObjectPath(),
					OW_CIMtoXMLFlags::notLocalOnly,
					OW_CIMtoXMLFlags::includeQualifiers,
					OW_CIMtoXMLFlags::includeClassOrigin,
					OW_StringArray());
				out << OW_XMLEscape(ss.toString());
				break;
			}
			default:
				OW_ASSERT(0);
		}

		out << "</VALUE>\n";
	}
}

/////////////////////////////////////////////////////////////
void
OW_CIMtoXML(OW_CIMDataType const& cdt, ostream& ostr)
{
	if (cdt.getType() == OW_CIMDataType::INVALID)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Invalid data type for toXML operation");
	}
	ostr << cdt.toString();
}

/////////////////////////////////////////////////////////////
void
OW_CIMtoXML(OW_CIMFlavor const& cf, ostream& ostr)
{
	const char* strf;
	switch(cf.getFlavor())
	{
		case OW_CIMFlavor::ENABLEOVERRIDE: strf = "OVERRIDABLE"; break;
		case OW_CIMFlavor::DISABLEOVERRIDE: strf = "OVERRIDABLE"; break;
		case OW_CIMFlavor::TOSUBCLASS: strf = "TOSUBCLASS"; break;
		case OW_CIMFlavor::RESTRICTED: strf = "TOSUBCLASS"; break;
		case OW_CIMFlavor::TRANSLATE: strf = "TRANSLATABLE"; break;
		default: strf = "BAD FLAVOR"; break;
	}

	ostr << strf;
}

/////////////////////////////////////////////////////////////
void
OW_CIMtoXML(OW_CIMQualifier const& cq, ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly)
{
	OW_CIMFlavor fv;
	OW_CIMDataType dt;

	if(cq.getName().length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "qualifier must have a name");
	}


	//
	// If only local definitions are required and this is a propagated
	// qualifier then nothing to return. Never ignore the association qualifier.
	//
	if(localOnly == OW_CIMtoXMLFlags::localOnly && cq.getPropagated() &&
	   !cq.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_ASSOCIATION))
	{
		return;
	}

	OW_CIMValue dv = cq.getDefaults().getDefaultValue();
	dt = cq.getDefaults().getDataType();
	OW_CIMValue cv = cq.getValue();
	if(!cv)
	{
		cv = dv;
	}

	if(cv)
	{
		if(cv.isArray())
		{
			dt = OW_CIMDataType(cv.getType(),cv.getArraySize());
		}
		else
		{
			dt = OW_CIMDataType(cv.getType());
		}
	}

	OW_ASSERT(dt);

	ostr
		<< "<QUALIFIER NAME=\""
		<< cq.getName()
		<< "\" TYPE=\"";

	OW_CIMtoXML(dt,ostr);
	ostr << "\" ";

	if(cq.getPropagated())
	{
		ostr << "PROPAGATED=\"true\" ";
	}

	//
	// Create flavors
	//
	fv = OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE);
	if(cq.hasFlavor(fv))
	{
		//
		// Not needed, because OVERRIDABLE defaults to true!
		//text += fv.toXml() + "=\"true\" ";
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE);
		if (cq.hasFlavor(fv))
		{
			OW_CIMtoXML(fv, ostr);
			ostr <<  "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS);
	if(cq.hasFlavor(fv))
	{
		//
		// Not needed, because TOSUBCLASS defaults to true!
		//text += fv.toXML() + "=\"true\" ";
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::RESTRICTED);
		if (cq.hasFlavor(fv))
		{
			OW_CIMtoXML(fv, ostr);
			ostr <<  "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE);
	if(cq.hasFlavor(fv))
	{
		OW_CIMtoXML(fv, ostr);
		ostr << "=\"true\" ";
	}
	else
	{
		//
		// Not needed, because TOINSTANCE defaults to false!
		//text += fv.toXML() + "=\"false\" ";
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TRANSLATE);
	if(cq.hasFlavor(fv))
	{
		OW_CIMtoXML(fv, ostr);
		ostr << "=\"true\" ";
	}
	else
	{
		//
		// Not needed, because TRANSLATABLE defaults to false!
		//text += fv.toXML() + "=\"false\" ";
	}

	ostr << '>';

	if(cv)
	{
		OW_CIMtoXML(cv, ostr);
	}

	ostr << "</QUALIFIER>";
}

/////////////////////////////////////////////////////////////
void
OW_CIMtoXML(OW_CIMProperty const& cp, ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin)
{
	OW_Bool isArray = false;
	OW_Bool isRef = false;

	if(cp.getName().length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "property must have a name");
	}

	//
	// If only local definitions are required and this is a propagated
	// property then nothing to return
	//
	if(localOnly == OW_CIMtoXMLFlags::localOnly && cp.getPropagated())
	{
		return;
	}	

	if(cp.getDataType())
	{
		isArray = cp.getDataType().isArrayType();
		isRef = cp.getDataType().isReferenceType();

		if(isArray)
		{
			ostr
				<<  "<PROPERTY.ARRAY NAME=\""
				<<  cp.getName()
				<< "\" TYPE=\"";

			OW_CIMtoXML(cp.getDataType(), ostr);
			ostr << "\" ";

			if(cp.getDataType().getSize() != -1)
			{
				ostr
					<< "ARRAYSIZE=\""
					<< cp.getDataType().getSize()
					<< "\" ";
			}
		}
		else if(isRef)
		{
			ostr
				<< "<PROPERTY.REFERENCE NAME=\""
				<< cp.getName()
				<< "\" REFERENCECLASS=\""
				<< cp.getDataType().getRefClassName()
				<< "\" ";
		}
		else
		{
			ostr
				<< "<PROPERTY NAME=\""
				<< cp.getName()
				<< "\" TYPE=\"";

			OW_CIMtoXML(cp.getDataType(), ostr);
			ostr << "\" ";
		}
	}
	else
	{
		OW_String msg("Property ");
		msg += cp.getName();
		msg += " has no type defined";
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	if(includeClassOrigin == OW_CIMtoXMLFlags::includeClassOrigin
	   && cp.getOriginClass().length() > 0)
	{
		ostr
			<< "CLASSORIGIN=\""
			<< cp.getOriginClass()
			<< "\" ";
	}

	if(cp.getPropagated())
	{
		ostr << "PROPAGATED=\"true\" ";
	}

	ostr << '>';

	if(cp.getQualifiers().size() > 0
	   && includeQualifiers == OW_CIMtoXMLFlags::includeQualifiers)
	{
		for(size_t i = 0; i < cp.getQualifiers().size(); i++)
		{
			OW_CIMtoXML(cp.getQualifiers()[i], ostr, localOnly);
		}
	}

	if(cp.getValue())
	{
		OW_CIMtoXML(cp.getValue(), ostr);
	}

	if(isArray)
	{
		ostr << "</PROPERTY.ARRAY>";
	}
	else if(isRef)
	{
		ostr << "</PROPERTY.REFERENCE>";
	}
	else
	{
		ostr << "</PROPERTY>";
	}
}
				
/////////////////////////////////////////////////////////////
void
OW_CIMtoXML(OW_CIMMethod const& cm, ostream& ostr,
	OW_CIMtoXMLFlags::local_only_flag const& localOnly,
	OW_CIMtoXMLFlags::include_qualifiers_flag const& includeQualifiers,
	OW_CIMtoXMLFlags::include_class_origin_flag const& includeClassOrigin)
{
	//
	// If only local definitions are required and this is a propagated
	// method then nothing to return
	//
	if(localOnly == OW_CIMtoXMLFlags::localOnly && cm.getPropagated())
	{
		return;
	}

	ostr << "<METHOD ";

	if(cm.getName().length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "method must have a name");
	}

	ostr
		<< "NAME=\""
		<< cm.getName()
		<< "\" ";

	if(cm.getReturnType())
	{
		ostr << "TYPE=\"";
		OW_CIMtoXML(cm.getReturnType(),ostr);
		ostr << "\" ";
	}

	if(includeClassOrigin == OW_CIMtoXMLFlags::includeClassOrigin
	   && cm.getOriginClass().length() > 0)
	{
		ostr
			<< "CLASSORIGIN=\""
			<< cm.getOriginClass()
			<< "\" ";
	}

	if(cm.getPropagated())
	{
		ostr << "PROPAGATED=\"true\" ";
	}

	ostr << '>';

	if(includeQualifiers == OW_CIMtoXMLFlags::includeQualifiers)
	{
		for(size_t i = 0; i < cm.getQualifiers().size(); i++)
		{
			OW_CIMtoXML(cm.getQualifiers()[i], ostr,
				OW_CIMtoXMLFlags::notLocalOnly);
		}
	}

	for(size_t i = 0; i < cm.getParameters().size(); i++)
	{
		OW_CIMtoXML(cm.getParameters()[i], ostr);
	}

	ostr << "</METHOD>";
}
				
/////////////////////////////////////////////////////////////				
static void
qualifierXML(OW_CIMParameter const& cp, ostream& ostr)
{
	if(cp.getQualifiers().size() > 0)
	{
		int sz = cp.getQualifiers().size();
		for(int i = 0; i < sz; i++)
		{
			OW_CIMtoXML(cp.getQualifiers()[i], ostr,
				OW_CIMtoXMLFlags::notLocalOnly);
		}
	}
}

/////////////////////////////////////////////////////////////
void
OW_CIMtoXML(OW_CIMParameter const& cp, ostream& ostr)
{
	if(cp.getName().length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"parameter must have a name");
	}

	if(!cp.getType())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"parameter must have a valid data type");
	}

	OW_Bool isArray = cp.getType().isArrayType();

	if(cp.getType().isReferenceType())
	{
		//
		// Data type is a reference
		//
		OW_String classref = cp.getType().getRefClassName();

		if(classref.length() > 0)
		{
			classref = "REFERENCECLASS=\"" + classref + "\"";
		}

		if(isArray)
		{
			if(cp.getType().getSize() == -1)
			{
				ostr
					<< "<PARAMETER.REFARRAY "
					<< classref
					<< " NAME=\""
					<< cp.getName()
					<< "\">";

				qualifierXML(cp, ostr);
				ostr << "</PARAMETER.REFARRAY>";
			}
			else
			{
				ostr
					<< "<PARAMETER.REFARRAY "
					<< classref
					<< " NAME=\""
					<< cp.getName()
					<< "\""
					<< " ARRAYSIZE=\""
					<< cp.getType().getSize()
					<< "\">";

				qualifierXML(cp, ostr);
				ostr << "</PARAMETER.REFARRAY>";
			}
		}
		else
		{
			ostr
				<< "<PARAMETER.REFERENCE "
				<< classref
				<< " NAME=\""
				<< cp.getName()
				<< "\">";

			qualifierXML(cp, ostr);
			ostr << "</PARAMETER.REFERENCE>";
		}
	}
	else
	{
		// Data type is not a ref
		if(isArray)
		{
			ostr << "<PARAMETER.ARRAY TYPE=\"";
			OW_CIMtoXML(cp.getType(), ostr);
			ostr << "\" NAME=\"" << cp.getName();

			if(cp.getType().getSize() != -1)
			{
				ostr
					<< "\" ARRAYSIZE=\""
					<< cp.getType().getSize();
			}
			ostr << "\">";

			qualifierXML(cp, ostr);
			ostr << "</PARAMETER.ARRAY>";
		}
		else
		{
			ostr << "<PARAMETER TYPE=\"";
			OW_CIMtoXML(cp.getType(), ostr);
			ostr
				<< "\"  NAME=\""
				<< cp.getName()
				<< "\">";

			qualifierXML(cp, ostr);
			ostr << "</PARAMETER>";
		}
	}
}

