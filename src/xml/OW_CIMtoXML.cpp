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
#ifdef OW_HAVE_OSTREAM
#include <ostream>
#else
#include <iostream>
#endif
#include <algorithm>

namespace OpenWBEM
{

using std::ostream;
//////////////////////////////////////////////////////////////////////////////
void CIMNameSpacetoXML(CIMNameSpace const& ns, ostream& ostr)
{
	ostr
		<< "<NAMESPACEPATH><HOST>"
		<< XMLEscape(ns.getHostUrl().getHost())
		<< "</HOST>";
	LocalCIMNameSpacetoXML(ns, ostr);
	
	ostr << "</NAMESPACEPATH>";
}
//////////////////////////////////////////////////////////////////////////////
void LocalCIMNameSpacetoXML(CIMNameSpace const& ns, ostream& ostr)
{
	String name = ns.getNameSpace();
	if (name.empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Namespace not set");
	}
	ostr << "<LOCALNAMESPACEPATH>";
	for (;;)
	{
		size_t index = name.indexOf('/');
		if (index == String::npos)
		{
			break;
		}
		if (index != 0)
		{
			ostr
				<< "<NAMESPACE NAME=\""
				<< XMLEscape(name.substring(0, index))
				<< "\"></NAMESPACE>";
		}
		name = name.substring(index+1);
	}
	
	ostr
		<< "<NAMESPACE NAME=\""
		<< XMLEscape(name)
		<< "\"></NAMESPACE>"
		<< "</LOCALNAMESPACEPATH>";
}
//////////////////////////////////////////////////////////////////////////////
void CIMtoXML(CIMQualifierType const& cqt, ostream& ostr)
{
	CIMFlavor fv;
	if (cqt.getName().empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "qualifierType must have a name");
	}
	//
	// If type isn't set then the CIMOM has stored a qualifier
	// thats bad and an exception is generated
	//
	if (!cqt.getDataType())
	{
		String msg("QualifierType (");
		msg += cqt.getName();
		msg += ") does not have a data type set";
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	ostr
		<< "<QUALIFIER.DECLARATION NAME=\""
		<< cqt.getName()
		<< "\" TYPE=\"";
	CIMtoXML(cqt.getDataType(), ostr);
	ostr << "\" ";
	if (cqt.getDataType().isArrayType())
	{
		ostr << "ISARRAY=\"true\" ";
	}
	else
	{
		ostr << "ISARRAY=\"false\" ";
	}
	fv = CIMFlavor(CIMFlavor::ENABLEOVERRIDE);
	if (cqt.hasFlavor(fv))
	{
		// NOT NECESSARY, default is TRUE
	}
	else
	{
		fv = CIMFlavor(CIMFlavor::DISABLEOVERRIDE);
		if (cqt.hasFlavor(fv))
		{
			CIMtoXML(fv, ostr);
			ostr << "=\"false\" ";
		}
	}
	fv = CIMFlavor(CIMFlavor::TOSUBCLASS);
	if (cqt.hasFlavor(fv))
	{
		// NOT NECESSARY, default is TRUE
	}
	else
	{
		fv = CIMFlavor(CIMFlavor::RESTRICTED);
		if (cqt.hasFlavor(fv))
		{
			CIMtoXML(fv, ostr);
			ostr << "=\"false\" ";
		}
	}
	fv = CIMFlavor(CIMFlavor::TRANSLATE);
	if (cqt.hasFlavor(fv))
	{
		CIMtoXML(fv, ostr);
		ostr << "=\"true\" ";
	}
	//
	//else
	// NOT NECESSARY, default is FALSE
	ostr << "><SCOPE ";
	//
	// Write scope information
	//
	String scope;
	bool scopeWritten = false;
	bool any = cqt.hasScope(CIMScope(CIMScope::ANY));
	if (any || cqt.hasScope(CIMScope(CIMScope::CLASS)))
	{
		ostr << "CLASS=\"true\" ";
		scopeWritten = true;
	}
	if (any || cqt.hasScope(CIMScope(CIMScope::ASSOCIATION)))
	{
		ostr << "ASSOCIATION=\"true\" ";
		scopeWritten = true;
	}
	if (any || cqt.hasScope(CIMScope(CIMScope::REFERENCE)))
	{
		ostr << "REFERENCE=\"true\" ";
		scopeWritten = true;
	}
	if (any || cqt.hasScope(CIMScope(CIMScope::PROPERTY)))
	{
		ostr << "PROPERTY=\"true\" ";
		scopeWritten = true;
	}
	if (any || cqt.hasScope(CIMScope(CIMScope::METHOD)))
	{
		ostr << "METHOD=\"true\" ";
		scopeWritten = true;
	}
	if (any || cqt.hasScope(CIMScope(CIMScope::PARAMETER)))
	{
		ostr << "PARAMETER=\"true\" ";
		scopeWritten = true;
	}
	if (any || cqt.hasScope(CIMScope(CIMScope::INDICATION)))
	{
		ostr << "INDICATION=\"true\" ";
		scopeWritten = true;
	}
	if (!scopeWritten)
	{
		String msg("Scope not set on qaulifier type: ");
		msg += cqt.getName();
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	ostr << "></SCOPE>";
	if (cqt.getDefaultValue())
	{
		CIMtoXML(cqt.getDefaultValue(), ostr);
	}
	ostr << "</QUALIFIER.DECLARATION>";
}
static void
outputKEYVALUE(ostream& ostr, const CIMProperty& cp)
{
	CIMDataType dtype = cp.getDataType();
	String type;
	if (dtype.isArrayType())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"An array cannot be a KEY");
	}
	if (dtype.isReferenceType())
	{
		CIMProperty lcp(cp);
		// This is sort of a bad thing to do, basically we are taking advantage
		// of a side effect of setDataType.  If the type isn't correct then
		// the value will be cast to the correct type.	This is to work around
		// a problem that may happen if a provider writer sets the value of a
		// reference property to a String instead of an CIMObjectPath.
		// If the value is a string, the xml that is output will be malformed,
		// and the client will throw an exception.
		lcp.setDataType(lcp.getDataType());
		CIMtoXML(lcp.getValue(), ostr);
		return;
	}
	//
	// Regular key value
	switch (dtype.getType())
	{
		case CIMDataType::CHAR16:
		case CIMDataType::DATETIME:
		case CIMDataType::STRING:
			type = "string";
			break;
		case CIMDataType::BOOLEAN:
			type = "boolean";
			break;
		default:
			type = "numeric";
	}
	CIMValue keyValue = cp.getValue();
	if (!keyValue)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "No key value");
	}
	ostr
		<< "<KEYVALUE VALUETYPE=\""
		<<  type
		<< "\">"
		<< XMLEscape(keyValue.toString())
		<< "</KEYVALUE>";
}
//////////////////////////////////////////////////////////////////////////////
void CIMClassPathtoXML(CIMObjectPath const& cop, ostream& ostr)
{
	if (!cop.isClassPath())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "cop is an instance path, not a class path as expected.");
	}
	if (!cop.getNameSpace().empty())
	{
		// do <CLASSPATH>
		ostr << "<CLASSPATH>";
		CIMNameSpacetoXML(cop.getFullNameSpace(),ostr);
		ostr << "<CLASSNAME NAME=\"" << cop.getClassName() << "\"/></CLASSPATH>";
	}
	else
	{
		// do <CLASSNAME>
		ostr << "<CLASSNAME NAME=\"" << cop.getClassName() << "\"/>";
	}
}
//////////////////////////////////////////////////////////////////////////////
// This isn't used.  If we ever need it we can uncomment it.
// void CIMLocalClassPathtoXML(CIMObjectPath const& cop, ostream& ostr)
// {
//     if (!cop.isClassPath())
//     {
//         OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "cop is an instance path, not a class path as expected.");
//     }
//
//     if (!cop.getNameSpace().empty())
//     {
//         // do <LOCALCLASSPATH>
//         ostr << "<LOCALCLASSPATH>";
//         CIMtoXML(cop.getFullNameSpace(),ostr,CIMtoXMLFlags::doLocal);
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
// CIMClassPathtoXML(CIMObjectPath const& cop, std::ostream& ostr)
// {
//     if (!cop.isClassPath())
//     {
//         OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "cop is an instance path, not a class path as expected.");
//     }
//
//     ostr << "<CLASSPATH>";
//     CIMtoXML(cop.getFullNameSpace(), ostr, CIMtoXMLFlags::dontDoLocal);
//
//     ostr << "<CLASSNAME NAME=\"";
//     ostr << cop.getObjectName() << "\">";
//
//     ostr << "</CLASSNAME>";
//
//     ostr << "</CLASSPATH>\n";
// }
//////////////////////////////////////////////////////////////////////////////
void CIMInstancePathtoXML(CIMObjectPath const& cop, ostream& ostr)
{
	//
	// Instance path
	//
	bool outputInstancePath = !cop.getNameSpace().empty();
	if (outputInstancePath)
	{
		ostr << "<INSTANCEPATH>";
		CIMNameSpacetoXML(cop.getFullNameSpace(), ostr);
	}
	CIMInstanceNametoXML(cop, ostr);
	if (outputInstancePath)
	{
		ostr << "</INSTANCEPATH>";
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIMLocalInstancePathtoXML(CIMObjectPath const& cop, ostream& ostr)
{
	//
	// Local Instance path
	//
	ostr << "<LOCALINSTANCEPATH>";
	LocalCIMNameSpacetoXML(cop.getFullNameSpace(), ostr);
	CIMInstanceNametoXML(cop, ostr);
	ostr << "</LOCALINSTANCEPATH>";
}
//////////////////////////////////////////////////////////////////////////////
void CIMInstanceNametoXML(CIMObjectPath const& cop, ostream& ostr)
{
	ostr << "<INSTANCENAME CLASSNAME=\"";
	ostr << cop.getClassName() << "\">";
	//
	// If keys > 1 then must use KEYBINDING - we also use it for
	// the key == 1 case - most implementations can't cope with
	// a single KEYVALUE without a KEYBINDING element
	//
	if (cop.isInstancePath())
	{
		size_t numkeys = cop.getKeys().size();
		for (size_t i = 0; i < numkeys; i++)
		{
			CIMProperty cp = cop.getKeys()[i];
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
void CIMtoXML(CIMClass const& cc, ostream& ostr)
{
	if (cc.getName().empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "class must have name");
	}
	ostr << "<CLASS NAME=\"";
	ostr << cc.getName();
	if (!cc.getSuperClass().empty())
	{
		ostr << "\" SUPERCLASS=\"";
		ostr << cc.getSuperClass();
	}

	ostr << "\">";
	const CIMQualifierArray& ccquals = cc.getQualifiers();
	for (size_t i = 0; i < ccquals.size(); i++)
	{
		CIMtoXML(ccquals[i], ostr);
	}
	const CIMPropertyArray& props = cc.getAllProperties();
	for (size_t i = 0; i < props.size(); i++)
	{
		CIMtoXML(props[i], ostr);
	}
	const CIMMethodArray& meths = cc.getAllMethods();
	for (size_t i = 0; i < meths.size(); i++)
	{
		CIMtoXML(meths[i], ostr);
	}
	ostr << "</CLASS>";
}
//////////////////////////////////////////////////////////////////////////////
void CIMInstancetoXML(CIMInstance const& ci, ostream& ostr)
{
	if (ci.getClassName().empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "instance has no class name");
	}
	ostr << "<INSTANCE CLASSNAME=\"";
	ostr << ci.getClassName();

	String lang = ci.getLanguage();
	if (!lang.empty())
	{
		ostr << "\" xml:lang=\"";
		ostr << lang;
	}
	ostr << "\">";

	//
	// Process qualifiers
	//
	for (size_t i = 0; i < ci.getQualifiers().size(); i++)
	{
		CIMtoXML(ci.getQualifiers()[i], ostr);
	}
	CIMPropertyArray pra = ci.getProperties();
	for (size_t i = 0; i < pra.size(); i++)
	{
		CIMtoXML(pra[i],ostr);
	}
	ostr << "</INSTANCE>";
}
//////////////////////////////////////////////////////////////////////////////
void CIMInstanceNameAndInstancetoXML(CIMInstance const& instance,
	ostream& ostr, CIMObjectPath const& instanceName)
{
	CIMInstanceNametoXML(instanceName, ostr);
	CIMInstancetoXML(instance, ostr);
}
//////////////////////////////////////////////////////////////////////////////
void CIMInstancePathAndInstancetoXML(CIMInstance const& instance,
	ostream& ostr, CIMObjectPath const& instancePath)
{
	CIMInstancePathtoXML(instancePath, ostr);
	CIMInstancetoXML(instance, ostr);
}
//////////////////////////////////////////////////////////////////////////////
template<class T>
static void raToXml(ostream& out, const Array<T>& ra)
{
	out << "<VALUE.ARRAY>";
	for (size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << ra[i];
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}

//////////////////////////////////////////////////////////////////////////////
template<class T>
static void realArrayToXml(ostream& out, const Array<T>& ra)
{
	out << "<VALUE.ARRAY>";
	for (size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << String(ra[i]); // operator<< doesn't use the right precision by default,
		// and it's just easier to let the String constructor do it right for us.
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}
//////////////////////////////////////////////////////////////////////////////
template <typename T>
static void valueToXML(T const& x, ostream& out)
{
	CIMtoXML(x, out);
}
static void valueToXML(CIMObjectPath const& x, ostream& out)
{
	if (x.getFullNameSpace().isLocal())
	{
		if (x.getNameSpace().empty())
		{
			CIMInstanceNametoXML(x, out);
		}
		else
		{
			CIMLocalInstancePathtoXML(x, out);
		}
	}
	else
	{
		CIMInstancePathtoXML(x, out);
	}
}
static void raToXmlCOP(ostream& out, const Array<CIMObjectPath>& ra)
{
	out << "<VALUE.REFARRAY>";
	for (size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE.REFERENCE>";
		valueToXML(ra[i], out);
		out << "</VALUE.REFERENCE>";
	}
	out << "</VALUE.REFARRAY>";
}
static void raToXmlSA(ostream& out, const Array<String>& ra)
{
	out << "<VALUE.ARRAY>";
	for (size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << XMLEscape(ra[i]);
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}
static void raToXmlChar16(ostream& out, const Array<Char16>& ra)
{
	out << "<VALUE.ARRAY>";
	for (size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << XMLEscape(ra[i].toString());
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}
void raToXmlNumeric(ostream& out, const Array<Int8>& ra)
{
	out << "<VALUE.ARRAY>";
	for (size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << Int32(ra[i]);
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}
void raToXmlNumeric(ostream& out, const Array<UInt8>& ra)
{
	out << "<VALUE.ARRAY>";
	for (size_t i = 0; i < ra.size(); i++)
	{
		out << "<VALUE>";
		out << UInt32(ra[i]);
		out << "</VALUE>";
	}
	out << "</VALUE.ARRAY>";
}
//////////////////////////////////////////////////////////////////////////////
void CIMtoXML(CIMValue const& cv, ostream& out)
{
	if (!cv)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "CIM value is NULL");
	}
	if (cv.isArray())
	{
		switch (cv.getType())
		{
			case CIMDataType::BOOLEAN:
			{
				BoolArray a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::UINT8:
			{
				UInt8Array a;
				cv.get(a);
				raToXmlNumeric(out, a);
				break;
			}
			case CIMDataType::SINT8:
			{
				Int8Array a;
				cv.get(a);
				raToXmlNumeric(out, a);
				break;
			}
			// ATTN: UTF8
			case CIMDataType::CHAR16:
			{
				Char16Array a;
				cv.get(a);
				raToXmlChar16(out, a);
				break;
			}
			case CIMDataType::UINT16:
			{
				UInt16Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::SINT16:
			{
				Int16Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::UINT32:
			{
				UInt32Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::SINT32:
			{
				Int32Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::UINT64:
			{
				UInt64Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::SINT64:
			{
				Int64Array a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::REAL32:
			{
				Real32Array a;
				cv.get(a);
				realArrayToXml(out, a);
				break;
			}
			case CIMDataType::REAL64:
			{
				Real64Array a;
				cv.get(a);
				realArrayToXml(out, a);
				break;
			}
			case CIMDataType::STRING:
			{
				StringArray a;
				cv.get(a);
				raToXmlSA(out, a);
				break;
			}
			case CIMDataType::DATETIME:
			{
				CIMDateTimeArray a;
				cv.get(a);
				raToXml(out, a);
				break;
			}
			case CIMDataType::REFERENCE:
			{
				CIMObjectPathArray a;
				cv.get(a);
				raToXmlCOP(out, a);
				break;
			}
			
			case CIMDataType::EMBEDDEDCLASS:
			{
				CIMClassArray ca;
				cv.get(ca);
				StringArray sa;
				for (size_t i = 0; i < ca.size(); ++i)
				{
					OStringStream ss;
					CIMtoXML(ca[i], ss);
					sa.push_back(ss.toString());
				}
				raToXmlSA(out, sa);
				break;
			}
			
			case CIMDataType::EMBEDDEDINSTANCE:
			{
				CIMInstanceArray ia;
				cv.get(ia);
				StringArray sa;
				for (size_t i = 0; i < ia.size(); ++i)
				{
					OStringStream ss;
					CIMInstancetoXML(ia[i],ss);
					sa.push_back(ss.toString());
				}
				raToXmlSA(out, sa);
				break;
			}
			default:
				OW_ASSERT(0);
		}
	}
	else if (cv.getType() == CIMDataType::REFERENCE)
	{
		out << "<VALUE.REFERENCE>";
		CIMObjectPath a(CIMNULL);
		cv.get(a);
		if (a.getFullNameSpace().isLocal())
		{
			if (a.getNameSpace().empty())
			{
				CIMInstanceNametoXML(a, out);
			}
			else
			{
				CIMLocalInstancePathtoXML(a, out);
			}
		}
		else
		{
			CIMInstancePathtoXML(a, out);
		}
		out << "</VALUE.REFERENCE>";
	}
	else
	{
		out << "<VALUE>";
		switch (cv.getType())
		{
			case CIMDataType::BOOLEAN:
			case CIMDataType::UINT8:
			case CIMDataType::SINT8:
			case CIMDataType::UINT16:
			case CIMDataType::SINT16:
			case CIMDataType::UINT32:
			case CIMDataType::SINT32:
			case CIMDataType::UINT64:
			case CIMDataType::SINT64:
			case CIMDataType::REAL32:
			case CIMDataType::REAL64:
			case CIMDataType::DATETIME:
			{
				out << cv.toString();
				break;
			}

			case CIMDataType::CHAR16:
			case CIMDataType::STRING:
			{
				out << XMLEscape(cv.toString());
				break;
			}
			
			case CIMDataType::EMBEDDEDCLASS:
			{
				CIMClass cc(CIMNULL);
				cv.get(cc);
				String s;
				OStringStream ss;
				CIMtoXML(cc, ss);
				out << XMLEscape(ss.toString());
				break;
			}
			
			case CIMDataType::EMBEDDEDINSTANCE:
			{
				CIMInstance i(CIMNULL);
				cv.get(i);
				String s;
				OStringStream ss;
				CIMInstancetoXML(i,ss);
				out << XMLEscape(ss.toString());
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
CIMtoXML(CIMDataType const& cdt, ostream& ostr)
{
	switch (cdt.getType())
	{
		case CIMDataType::INVALID:
			OW_THROWCIMMSG(CIMException::FAILED,
				"Invalid data type for toXML operation");
			break;
		// special case for this since CIMDataType::toString() returns REF (appropriate for MOF)
		case CIMDataType::REFERENCE:
			ostr << "reference";
			break;
		default:
			ostr << cdt.toString();
			break;
	}
}
/////////////////////////////////////////////////////////////
void
CIMtoXML(CIMFlavor const& cf, ostream& ostr)
{
	const char* strf;
	switch (cf.getFlavor())
	{
		case CIMFlavor::ENABLEOVERRIDE: strf = "OVERRIDABLE"; break;
		case CIMFlavor::DISABLEOVERRIDE: strf = "OVERRIDABLE"; break;
		case CIMFlavor::TOSUBCLASS: strf = "TOSUBCLASS"; break;
		case CIMFlavor::RESTRICTED: strf = "TOSUBCLASS"; break;
		case CIMFlavor::TRANSLATE: strf = "TRANSLATABLE"; break;
		default: strf = "BAD FLAVOR"; break;
	}
	ostr << strf;
}
/////////////////////////////////////////////////////////////
void
CIMtoXML(CIMQualifier const& cq, ostream& ostr)
{
	CIMFlavor fv;
	
	if (cq.getName().empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "qualifier must have a name");
	}
	CIMValue dv = cq.getDefaults().getDefaultValue();
	CIMDataType dt = cq.getDefaults().getDataType();
	CIMValue cv = cq.getValue();
	if (!cv)
	{
		cv = dv;
	}
	if (cv)
	{
		if (cv.isArray())
		{
			dt = CIMDataType(cv.getType(),cv.getArraySize());
		}
		else
		{
			dt = CIMDataType(cv.getType());
		}
	}
	OW_ASSERT(dt);
	ostr
		<< "<QUALIFIER NAME=\""
		<< cq.getName()
		<< "\" TYPE=\"";
	CIMtoXML(dt,ostr);
	ostr << "\" ";
	if (cq.getPropagated())
	{
		ostr << "PROPAGATED=\"true\" ";
	}
	//
	// Create flavors
	//
	fv = CIMFlavor(CIMFlavor::ENABLEOVERRIDE);
	if (cq.hasFlavor(fv))
	{
		//
		// Not needed, because OVERRIDABLE defaults to true!
	}
	else
	{
		fv = CIMFlavor(CIMFlavor::DISABLEOVERRIDE);
		if (cq.hasFlavor(fv))
		{
			CIMtoXML(fv, ostr);
			ostr <<  "=\"false\" ";
		}
	}
	fv = CIMFlavor(CIMFlavor::TOSUBCLASS);
	if (cq.hasFlavor(fv))
	{
		//
		// Not needed, because TOSUBCLASS defaults to true!
	}
	else
	{
		fv = CIMFlavor(CIMFlavor::RESTRICTED);
		if (cq.hasFlavor(fv))
		{
			CIMtoXML(fv, ostr);
			ostr <<  "=\"false\" ";
		}
	}
	// This is a bug in the spec, but we still support it for backward compatibility.
	//fv = CIMFlavor(CIMFlavor::TOINSTANCE);
	//if (cq.hasFlavor(fv))
	//{
	//	CIMtoXML(fv, ostr);
	//	ostr << "=\"true\" ";
	//}
	//else
	//{
		//
		// Not needed, because TOINSTANCE defaults to false!
	//}
	fv = CIMFlavor(CIMFlavor::TRANSLATE);
	if (cq.hasFlavor(fv))
	{
		CIMtoXML(fv, ostr);
		ostr << "=\"true\" ";
	}
	else
	{
		//
		// Not needed, because TRANSLATABLE defaults to false!
	}

	String lang = cq.getLanguage();
	if (!lang.empty())
	{
		ostr << " xml:lang=\"";
		ostr << lang;
		ostr << '\"';
	}

	ostr << '>';
	if (cv)
	{
		CIMtoXML(cv, ostr);
	}
	ostr << "</QUALIFIER>";
}
/////////////////////////////////////////////////////////////
void
CIMtoXML(CIMProperty const& cp, ostream& ostr)
{
	bool isArray = false;
	bool isRef = false;
	if (cp.getName().empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "property must have a name");
	}
	if (cp.getDataType())
	{
		isArray = cp.getDataType().isArrayType();
		isRef = cp.getDataType().isReferenceType();
		if (isArray)
		{
			ostr
				<<  "<PROPERTY.ARRAY NAME=\""
				<<  cp.getName()
				<< "\" TYPE=\"";
			CIMtoXML(cp.getDataType(), ostr);
			ostr << "\" ";
			if (cp.getDataType().getSize() != -1)
			{
				ostr
					<< "ARRAYSIZE=\""
					<< cp.getDataType().getSize()
					<< "\" ";
			}
		}
		else if (isRef)
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
			CIMtoXML(cp.getDataType(), ostr);
			ostr << "\" ";
		}
	}
	else
	{
		String msg("Property ");
		msg += cp.getName();
		msg += " has no type defined";
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	if (!cp.getOriginClass().empty())
	{
		ostr
			<< "CLASSORIGIN=\""
			<< cp.getOriginClass()
			<< "\" ";
	}
	if (cp.getPropagated())
	{
		ostr << "PROPAGATED=\"true\" ";
	}
	ostr << '>';
	for (size_t i = 0; i < cp.getQualifiers().size(); i++)
	{
		CIMtoXML(cp.getQualifiers()[i], ostr);
	}
	CIMValue val = cp.getValue();
	if (val)
	{
		// if there isn't an EmbeddedObject qualifier on an embedded object, then output one.
		if (val.getType() == CIMDataType::EMBEDDEDINSTANCE || val.getType() == CIMDataType::EMBEDDEDCLASS)
		{
			if (!cp.getQualifier(CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT))
			{
				CIMQualifier embeddedObject(CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT);
				embeddedObject.setValue(CIMValue(true));
				CIMtoXML(embeddedObject, ostr);
			}
		}

		CIMtoXML(val, ostr);
	}
	if (isArray)
	{
		ostr << "</PROPERTY.ARRAY>";
	}
	else if (isRef)
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
CIMtoXML(CIMMethod const& cm, ostream& ostr)
{
	ostr << "<METHOD ";
	if (cm.getName().empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED, "method must have a name");
	}
	ostr
		<< "NAME=\""
		<< cm.getName()
		<< "\" ";
	if (cm.getReturnType())
	{
		ostr << "TYPE=\"";
		CIMtoXML(cm.getReturnType(),ostr);
		ostr << "\" ";
	}
	if (!cm.getOriginClass().empty())
	{
		ostr
			<< "CLASSORIGIN=\""
			<< cm.getOriginClass()
			<< "\" ";
	}
	if (cm.getPropagated())
	{
		ostr << "PROPAGATED=\"true\" ";
	}
	ostr << '>';
	for (size_t i = 0; i < cm.getQualifiers().size(); i++)
	{
		CIMtoXML(cm.getQualifiers()[i], ostr);
	}
	for (size_t i = 0; i < cm.getParameters().size(); i++)
	{
		CIMtoXML(cm.getParameters()[i], ostr);
	}
	ostr << "</METHOD>";
}
				
/////////////////////////////////////////////////////////////				
static void
qualifierXML(CIMParameter const& cp, ostream& ostr)
{
	if (cp.getQualifiers().size() > 0)
	{
		int sz = cp.getQualifiers().size();
		for (int i = 0; i < sz; i++)
		{
			CIMtoXML(cp.getQualifiers()[i], ostr);
		}
	}
}
/////////////////////////////////////////////////////////////
void
CIMtoXML(CIMParameter const& cp, ostream& ostr)
{
	if (cp.getName().empty())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"parameter must have a name");
	}
	if (!cp.getType())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"parameter must have a valid data type");
	}
	bool isArray = cp.getType().isArrayType();
	if (cp.getType().isReferenceType())
	{
		//
		// Data type is a reference
		//
		String classref = cp.getType().getRefClassName();
		if (!classref.empty())
		{
			classref = "REFERENCECLASS=\"" + classref + "\"";
		}
		if (isArray)
		{
			if (cp.getType().getSize() == -1)
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
		if (isArray)
		{
			ostr << "<PARAMETER.ARRAY TYPE=\"";
			CIMtoXML(cp.getType(), ostr);
			ostr << "\" NAME=\"" << cp.getName();
			if (cp.getType().getSize() != -1)
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
			CIMtoXML(cp.getType(), ostr);
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
CIMParamValueToXML(CIMParamValue const& pv, std::ostream& ostr)
{
	ostr << "<PARAMVALUE NAME=\"" << pv.getName() << "\"";
	if (pv.getValue())
	{
		String type = pv.getValue().getCIMDataType().toString();
		if (type == "REF")
		{
			type = "reference";
		}
		ostr << " PARAMTYPE=\"" << type << "\">";
		CIMtoXML(pv.getValue(), ostr);
	}
	else
	{
		ostr << '>';
	}
	ostr << "</PARAMVALUE>";
}

} // end namespace OpenWBEM

