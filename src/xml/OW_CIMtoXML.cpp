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
#include "OW_CIMtoXML.hpp"
#include "OW_String.hpp"
#include "OW_XMLEscape.hpp"
#include "OW_Assertion.hpp"
#include "OW_StringStream.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMParameter.hpp"

#include <iostream>
#include <algorithm>

using std::ostream;

//////////////////////////////////////////////////////////////////////////////
void OW_CIMNameSpacetoXML(OW_CIMNameSpace const& ns, ostream& ostr)
{
	ostr
		<< "<NAMESPACEPATH><HOST>"
		<< OW_XMLEscape(ns.getHostUrl().getHost())
		<< "</HOST>";

	OW_LocalCIMNameSpacetoXML(ns, ostr);
	
	ostr << "</NAMESPACEPATH>";
}

//////////////////////////////////////////////////////////////////////////////
void OW_LocalCIMNameSpacetoXML(OW_CIMNameSpace const& ns, ostream& ostr)
{
	OW_String name = ns.getNameSpace();

	if(name.empty())
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Namespace not set");

	ostr << "<LOCALNAMESPACEPATH>";

	for (;;)
	{
		size_t index = name.indexOf('/');

		if (index == OW_String::npos)
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
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMQualifierType const& cqt, ostream& ostr)
{
	OW_CIMFlavor fv;

	if(cqt.getName().empty())
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
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE);
		if(cqt.hasFlavor(fv))
		{
			OW_CIMtoXML(fv, ostr);
			ostr << "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS);
	if(cqt.hasFlavor(fv))
	{
		// NOT NECESSARY, default is TRUE
	}
	else
	{
		fv = OW_CIMFlavor(OW_CIMFlavor::RESTRICTED);
		if(cqt.hasFlavor(fv))
		{
			OW_CIMtoXML(fv, ostr);
			ostr << "=\"false\" ";
		}
	}

	fv = OW_CIMFlavor(OW_CIMFlavor::TRANSLATE);
	if(cqt.hasFlavor(fv))
	{
		OW_CIMtoXML(fv, ostr);
		ostr << "=\"true\" ";
	}
	//
	//else
	// NOT NECESSARY, default is FALSE

	ostr << "><SCOPE ";

	//
	// Write scope information
	//
	OW_String scope;
	bool scopeWritten = false;
	bool any = cqt.hasScope(OW_CIMScope(OW_CIMScope::ANY));
	if(any || cqt.hasScope(OW_CIMScope(OW_CIMScope::CLASS)))
	{
		ostr << "CLASS=\"true\" ";
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
		OW_CIMProperty lcp(cp);

		// This is sort of a bad thing to do, basically we are taking advantage 
		// of a side effect of setDataType.  If the type isn't correct then
		// the value will be cast to the correct type.	This is to work around
		// a problem that may happen if a provider writer sets the value of a
		// reference property to a OW_String instead of an OW_CIMObjectPath.
		// If the value is a string, the xml that is output will be malformed,
		// and the client will throw an exception.
		lcp.setDataType(lcp.getDataType()); 
		OW_CIMtoXML(lcp.getValue(), ostr);
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
void OW_CIMClassPathtoXML(OW_CIMObjectPath const& cop, ostream& ostr)
{
	if (!cop.isClassPath())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "cop is an instance path, not a class path as expected.");
	}

	if (!cop.getNameSpace().empty())
	{
		// do <CLASSPATH>
		ostr << "<CLASSPATH>";
		OW_CIMNameSpacetoXML(cop.getFullNameSpace(),ostr);
		ostr << "<CLASSNAME NAME=\"" << cop.getObjectName() << "\"/></CLASSPATH>";
	}
	else
	{
		// do <CLASSNAME>
		ostr << "<CLASSNAME NAME=\"" << cop.getObjectName() << "\"/>";
	}
}

//////////////////////////////////////////////////////////////////////////////
// This isn't used.  If we ever need it we can uncomment it.
// void OW_CIMLocalClassPathtoXML(OW_CIMObjectPath const& cop, ostream& ostr)
// {
//     if (!cop.isClassPath())
//     {
//         OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "cop is an instance path, not a class path as expected.");
//     }
//
//     if (!cop.getNameSpace().empty())
//     {
//         // do <LOCALCLASSPATH>
//         ostr << "<LOCALCLASSPATH>";
//         OW_CIMtoXML(cop.getFullNameSpace(),ostr,OW_CIMtoXMLFlags::doLocal);
//         ostr << "<CLASSNAME NAME=\"" << cop.getObjectName() << "\"/></LOCALCLASSPATH>";
//     }
//     else
//     {
//         // do <CLASSNAME>
//         ostr << "<CLASSNAME NAME=\"" << cop.getObjectName() << "\"/>";
//     }
// }

/////////////////////////////////////////////////////////////
// void
// OW_CIMClassPathtoXML(OW_CIMObjectPath const& cop, std::ostream& ostr)
// {
//     if (!cop.isClassPath())
//     {
//         OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "cop is an instance path, not a class path as expected.");
//     }
//
//     ostr << "<CLASSPATH>";
//     OW_CIMtoXML(cop.getFullNameSpace(), ostr, OW_CIMtoXMLFlags::dontDoLocal);
//
//     ostr << "<CLASSNAME NAME=\"";
//     ostr << cop.getObjectName() << "\">";
//
//     ostr << "</CLASSNAME>";
//
//     ostr << "</CLASSPATH>\n";
// }


//////////////////////////////////////////////////////////////////////////////
void OW_CIMInstancePathtoXML(OW_CIMObjectPath const& cop, ostream& ostr)
{
	//
	// Instance path
	//
	bool outputInstancePath = !cop.getNameSpace().empty();
	if (outputInstancePath)
	{
		ostr << "<INSTANCEPATH>";
		OW_CIMNameSpacetoXML(cop.getFullNameSpace(), ostr);
	}

	OW_CIMInstanceNametoXML(cop, ostr);

	if (outputInstancePath)
	{
		ostr << "</INSTANCEPATH>";
	}
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMInstanceNametoXML(OW_CIMObjectPath const& cop, ostream& ostr)
{
	ostr << "<INSTANCENAME CLASSNAME=\"";
	ostr << cop.getObjectName() << "\">";


	//
	// If keys > 1 then must use KEYBINDING - we also use it for
	// the key == 1 case - most implementations can't cope with
	// a single KEYVALUE without a KEYBINDING element
	//
	if(cop.isInstancePath())
	{
		size_t numkeys = cop.getKeys().size();
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
		// A singleton, a class without keys
	}

	ostr << "</INSTANCENAME>";
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMClass const& cc, ostream& ostr)
{
	if(cc.getName().empty())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "class must have name");
	}

	ostr << "<CLASS NAME=\"";
	ostr << cc.getName();
	if(!cc.getSuperClass().empty())
	{
		ostr << "\" SUPERCLASS=\"";
		ostr << cc.getSuperClass();
	}
	ostr << "\">";

	const OW_CIMQualifierArray& ccquals = cc.getQualifiers();
	for(size_t i = 0; i < ccquals.size(); i++)
	{
		OW_CIMtoXML(ccquals[i], ostr);
	}

	const OW_CIMPropertyArray& props = cc.getAllProperties();
	for(size_t i = 0; i < props.size(); i++)
	{
		OW_CIMtoXML(props[i], ostr);
	}

	const OW_CIMMethodArray& meths = cc.getAllMethods();
	for(size_t i = 0; i < meths.size(); i++)
	{
		OW_CIMtoXML(meths[i], ostr);
	}

	ostr << "</CLASS>";
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMInstancetoXML(OW_CIMInstance const& ci, ostream& ostr)
{
	if(ci.getClassName().empty())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "instance has no class name");
	}

	ostr << "<INSTANCE CLASSNAME=\"";
	ostr << ci.getClassName() << "\">";
	//
	// Process qualifiers
	//
	for(size_t i = 0; i < ci.getQualifiers().size(); i++)
	{
		OW_CIMtoXML(ci.getQualifiers()[i], ostr);
	}

	OW_CIMPropertyArray pra = ci.getProperties();
	for(size_t i = 0; i < pra.size(); i++)
	{
		OW_CIMtoXML(pra[i],ostr);
	}

	ostr << "</INSTANCE>";
}


//////////////////////////////////////////////////////////////////////////////
void OW_CIMInstanceNameAndInstancetoXML(OW_CIMInstance const& instance, 
	ostream& ostr, OW_CIMObjectPath const& instanceName)
{
	OW_CIMInstanceNametoXML(instanceName, ostr);
	OW_CIMInstancetoXML(instance, ostr);
}


//////////////////////////////////////////////////////////////////////////////
void OW_CIMInstancePathAndInstancetoXML(OW_CIMInstance const& instance, 
	ostream& ostr, OW_CIMObjectPath const& instancePath)
{
	OW_CIMInstancePathtoXML(instancePath, ostr);
	OW_CIMInstancetoXML(instance, ostr);
}


//////////////////////////////////////////////////////////////////////////////
template<class T>
void raToXml(ostream& out, const OW_Array<T>& ra)
{
	out << "<VALUE.ARRAY>";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << ra[i];
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}

template <typename T>
static void valueToXML(T const& x, ostream& out)
{
	OW_CIMtoXML(x, out);
}

static void valueToXML(OW_CIMObjectPath const& x, ostream& out)
{
	OW_CIMInstancePathtoXML(x, out);
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
	out << "<VALUE.ARRAY>";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << OW_XMLEscape(ra[i]);
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}

static void raToXmlChar16(ostream& out, const OW_Array<OW_Char16>& ra)
{
	out << "<VALUE.ARRAY>";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << OW_XMLEscape(ra[i].toUTF8());
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}

void raToXmlNumeric(ostream& out, const OW_Array<OW_Int8>& ra)
{
	out << "<VALUE.ARRAY>";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << OW_Int32(ra[i]);
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}

void raToXmlNumeric(ostream& out, const OW_Array<OW_UInt8>& ra)
{
	out << "<VALUE.ARRAY>";
	for(size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << OW_UInt32(ra[i]);
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}

//////////////////////////////////////////////////////////////////////////////
void OW_CIMtoXML(OW_CIMValue const& cv, ostream& out)
{
	if (!cv)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "CIM value is NULL");
	}
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
				raToXmlNumeric(out, a);
				break;
			}

			case OW_CIMDataType::SINT8:
			{
				OW_Int8Array a;
				cv.get(a);
				raToXmlNumeric(out, a);
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
					OW_CIMtoXML(ca[i], ss);
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
					OW_CIMInstancetoXML(ia[i],ss);
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
		OW_CIMObjectPath a(OW_CIMNULL);
		cv.get(a);
		OW_CIMInstancePathtoXML(a, out);
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
				out << OW_UInt32(a);
				break;
			}

			case OW_CIMDataType::SINT8:
			{
				OW_Int8 a;
				cv.get(a);
				out << OW_Int32(a);
				break;
			}

			case OW_CIMDataType::CHAR16:
			{
				OW_Char16 a;
				cv.get(a);
				out << OW_XMLEscape(a.toUTF8());
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
				OW_CIMDateTime a(OW_CIMNULL);
				cv.get(a);
				out << a.toString();
				break;
			}
			
			case OW_CIMDataType::EMBEDDEDCLASS:
			{
				OW_CIMClass cc(OW_CIMNULL);
				cv.get(cc);
				OW_String s;
				OW_StringStream ss;
				OW_CIMtoXML(cc, ss);
				out << OW_XMLEscape(ss.toString());
				break;
			}
			
			case OW_CIMDataType::EMBEDDEDINSTANCE:
			{
				OW_CIMInstance i(OW_CIMNULL);
				cv.get(i);
				OW_String s;
				OW_StringStream ss;
				OW_CIMInstancetoXML(i,ss);
				out << OW_XMLEscape(ss.toString());
				break;
			}
			default:
				OW_ASSERT(0);
		}

		out << "</VALUE>";
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
OW_CIMtoXML(OW_CIMQualifier const& cq, ostream& ostr)
{
	OW_CIMFlavor fv;
	
	if(cq.getName().empty())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "qualifier must have a name");
	}

	OW_CIMValue dv = cq.getDefaults().getDefaultValue();
	OW_CIMDataType dt = cq.getDefaults().getDataType();
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

	// This is a bug in the spec, but we still support it for backward compatibility.
	//fv = OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE);
	//if(cq.hasFlavor(fv))
	//{
	//	OW_CIMtoXML(fv, ostr);
	//	ostr << "=\"true\" ";
	//}
	//else
	//{
		//
		// Not needed, because TOINSTANCE defaults to false!
	//}

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
OW_CIMtoXML(OW_CIMProperty const& cp, ostream& ostr)
{
	bool isArray = false;
	bool isRef = false;

	if(cp.getName().empty())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "property must have a name");
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

	if(!cp.getOriginClass().empty())
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

	for(size_t i = 0; i < cp.getQualifiers().size(); i++)
	{
		OW_CIMtoXML(cp.getQualifiers()[i], ostr);
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
OW_CIMtoXML(OW_CIMMethod const& cm, ostream& ostr)
{
	ostr << "<METHOD ";

	if(cm.getName().empty())
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

	if(!cm.getOriginClass().empty())
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

	for(size_t i = 0; i < cm.getQualifiers().size(); i++)
	{
		OW_CIMtoXML(cm.getQualifiers()[i], ostr);
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
			OW_CIMtoXML(cp.getQualifiers()[i], ostr);
		}
	}
}

/////////////////////////////////////////////////////////////
void
OW_CIMtoXML(OW_CIMParameter const& cp, ostream& ostr)
{
	if(cp.getName().empty())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"parameter must have a name");
	}

	if(!cp.getType())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"parameter must have a valid data type");
	}

	bool isArray = cp.getType().isArrayType();

	if(cp.getType().isReferenceType())
	{
		//
		// Data type is a reference
		//
		OW_String classref = cp.getType().getRefClassName();

		if(!classref.empty())
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

/////////////////////////////////////////////////////////////
void
OW_CIMParamValueToXML(OW_CIMParamValue const& pv, std::ostream& ostr)
{
	ostr << "<PARAMVALUE NAME=\"" << pv.getName() << "\"";

	if (pv.getValue())
	{
		OW_String type = pv.getValue().getCIMDataType().toString();
		if (type == "REF")
		{
			type = "reference";
		}
		ostr << " PARAMTYPE=\"" << type << "\">";
		OW_CIMtoXML(pv.getValue(), ostr);
	}
	else
	{
		ostr << '>';
	}
	ostr << "</PARAMVALUE>";
}


