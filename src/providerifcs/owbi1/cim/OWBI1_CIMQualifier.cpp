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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OWBI1_config.h"
#include "OWBI1_CIMQualifier.hpp"
#include "OWBI1_CIMQualifierRep.hpp"
#include "OWBI1_CIMName.hpp"
#include "OWBI1_CIMNameRep.hpp"
#include "OWBI1_CIMQualifierType.hpp"
#include "OWBI1_CIMQualifierTypeRep.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMValueRep.hpp"
#include "OWBI1_NULLValueException.hpp"
#include "OWBI1_CIMFlavor.hpp"
#include "OWBI1_CIMFlavorRep.hpp"
#include "OWBI1_CIMDetail.hpp"
#include "OW_Assertion.hpp"

namespace OWBI1
{

using namespace detail;
using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier() 
	: m_rep(new CIMQualifierRep(OpenWBEM::CIMQualifier()))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(CIMNULL_t) 
	: m_rep(new CIMQualifierRep(OpenWBEM::CIMQualifier(OpenWBEM::CIMNULL)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const CIMName& name) 
	: m_rep(new CIMQualifierRep(OpenWBEM::CIMQualifier(name.getRep()->name)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const char* name) 
	: m_rep(new CIMQualifierRep(OpenWBEM::CIMQualifier(name)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const CIMQualifierType& cgt) 
	: m_rep(new CIMQualifierRep(OpenWBEM::CIMQualifier(cgt.getRep()->qualtype)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const CIMQualifier& x) 
	: CIMElement(x)
	, m_rep(x.m_rep)
{
}

//////////////////////////////////////////////////////////////////////////////
CIMQualifier::CIMQualifier(const CIMQualifierRepRef& rep)
	: m_rep(rep)
{
}

//////////////////////////////////////////////////////////////////////////////
CIMQualifier::~CIMQualifier()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::setNull()
{
	m_rep->qual.setNull();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::operator= (const CIMQualifier& x)
{
	m_rep = x.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::isKeyQualifier() const
{
	return m_rep->qual.isKeyQualifier();
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::isAssociationQualifier() const
{
	return m_rep->qual.isAssociationQualifier();
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMQualifier::getValue() const
{
	return CIMValue(CIMValueRepRef(new CIMValueRep(m_rep->qual.getValue())));
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMQualifier::getValueT() const
{
	CIMValue rv = getValue();
	if (!rv)
	{
		OWBI1_THROW(NULLValueException, "");
	}
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::setValue(const CIMValue& value)
{
	m_rep->qual.setValue(value.getRep()->value);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::setDefaults(const CIMQualifierType& qtype)
{
	m_rep->qual.setDefaults(qtype.getRep()->qualtype);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMQualifier::getDefaults() const
{
	return CIMQualifierType(new CIMQualifierTypeRep(m_rep->qual.getDefaults()));
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::hasFlavor(const CIMFlavor& flavor) const
{
	return m_rep->qual.hasFlavor(flavor.getRep()->flavor);
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::addFlavor(const CIMFlavor& flavor)
{
	m_rep->qual.addFlavor(flavor.getRep()->flavor);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::removeFlavor(Int32 flavor)
{
	m_rep->qual.removeFlavor(flavor);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::hasValue() const
{
	return m_rep->qual.hasValue();
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::equals(const CIMQualifier& arg) const
{
	return m_rep->qual.equals(arg.getRep()->qual);
}
//////////////////////////////////////////////////////////////////////////////
CIMFlavorArray
CIMQualifier::getFlavors() const
{
	return wrapArray<CIMFlavorArray>(m_rep->qual.getFlavor());
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier&
CIMQualifier::setPropagated(bool propagated)
{
	m_rep->qual.setPropagated(propagated);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::getPropagated() const
{
	return m_rep->qual.getPropagated();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier& 
CIMQualifier::setLanguage(const String& language)
{
	OW_ASSERT(hasFlavor(CIMFlavor::TRANSLATE));
	m_rep->qual.setLanguage(language.c_str());
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifier::getLanguage() const
{
	return m_rep->qual.getLanguage().c_str();
}
//////////////////////////////////////////////////////////////////////////////
CIMName
CIMQualifier::getName() const
{
	return m_rep->qual.getName().c_str();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::setName(const CIMName& name)
{
	m_rep->qual.setName(name.getRep()->name);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::readObject(istream &istrm)
{
	m_rep->qual.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMQualifier::writeObject(ostream &ostrm) const
{
	m_rep->qual.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifier::toMOF() const
{
	return m_rep->qual.toMOF().c_str();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMQualifier::toString() const
{
	return m_rep->qual.toString().c_str();
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
CIMQualifier
CIMQualifier::createKeyQualifier()
{
	return CIMQualifier(new CIMQualifierRep(OpenWBEM::CIMQualifier::createKeyQualifier()));
}

//////////////////////////////////////////////////////////////////////////////
CIMQualifierRepRef
CIMQualifier::getRep() const
{
	return m_rep;
}

//////////////////////////////////////////////////////////////////////////////
CIMQualifier::operator safe_bool () const
{  
	return m_rep->qual ? &CIMQualifier::m_rep : 0; 
}

//////////////////////////////////////////////////////////////////////////////
bool
CIMQualifier::operator!() const
{  
	return !m_rep->qual; 
}


//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMQualifier& x, const CIMQualifier& y)
{
	return *x.m_rep < *y.m_rep;
}

// Meta qualifiers
const char* const CIMQualifier::CIM_QUAL_ASSOCIATION	= "Association";
const char* const CIMQualifier::CIM_QUAL_INDICATION		= "Indication";
// Standard qualifiers
const char* const CIMQualifier::CIM_QUAL_ABSTRACT 		= "Abstract";
const char* const CIMQualifier::CIM_QUAL_AGGREGATE 		= "Aggregate";
const char* const CIMQualifier::CIM_QUAL_AGGREGATION 	= "Aggregation";
const char* const CIMQualifier::CIM_QUAL_ALIAS 			= "Alias";
const char* const CIMQualifier::CIM_QUAL_ARRAYTYPE 		= "ArrayType";
const char* const CIMQualifier::CIM_QUAL_BITMAP 		= "BitMap";
const char* const CIMQualifier::CIM_QUAL_BITVALUES 		= "BitValues";
const char* const CIMQualifier::CIM_QUAL_COMPOSITION    = "Composition";
const char* const CIMQualifier::CIM_QUAL_COUNTER 		= "Counter";
const char* const CIMQualifier::CIM_QUAL_DELETE         = "Delete";
const char* const CIMQualifier::CIM_QUAL_DEPRECATED     = "Deprecated";
const char* const CIMQualifier::CIM_QUAL_DESCRIPTION 	= "Description";
const char* const CIMQualifier::CIM_QUAL_DISPLAYNAME 	= "DisplayName";
const char* const CIMQualifier::CIM_QUAL_DN             = "DN";
const char* const CIMQualifier::CIM_QUAL_EMBEDDEDOBJECT = "EmbeddedObject";
const char* const CIMQualifier::CIM_QUAL_EXCEPTION      = "Exception";
const char* const CIMQualifier::CIM_QUAL_EXPENSIVE      = "Expensive";
const char* const CIMQualifier::CIM_QUAL_EXPERIMENTAL   = "Experimental";
const char* const CIMQualifier::CIM_QUAL_GAUGE 			= "Gauge";
const char* const CIMQualifier::CIM_QUAL_IFDELETED      = "Ifdeleted";
const char* const CIMQualifier::CIM_QUAL_IN             = "In";
const char* const CIMQualifier::CIM_QUAL_INVISIBLE      = "Invisible";
const char* const CIMQualifier::CIM_QUAL_KEY            = "Key";
const char* const CIMQualifier::CIM_QUAL_LARGE          = "Large";
const char* const CIMQualifier::CIM_QUAL_MAPPINGSTRINGS	= "MappingStrings";
const char* const CIMQualifier::CIM_QUAL_MAX 			= "Max";
const char* const CIMQualifier::CIM_QUAL_MAXLEN         = "MaxLen";
const char* const CIMQualifier::CIM_QUAL_MAXVALUE 		= "MaxValue";
const char* const CIMQualifier::CIM_QUAL_MIN 			= "Min";
const char* const CIMQualifier::CIM_QUAL_MINLEN         = "MinLen";
const char* const CIMQualifier::CIM_QUAL_MINVALUE 		= "MinValue";
const char* const CIMQualifier::CIM_QUAL_MODELCORRESPONDENCE = "ModelCorrespondence";
const char* const CIMQualifier::CIM_QUAL_NONLOCAL 		= "Nonlocal";
const char* const CIMQualifier::CIM_QUAL_NONLOCALTYPE 	= "NonlocalType";
const char* const CIMQualifier::CIM_QUAL_NULLVALUE 		= "NullValue";
const char* const CIMQualifier::CIM_QUAL_OCTETSTRING    = "Octetstring";
const char* const CIMQualifier::CIM_QUAL_OUT 			= "Out";
const char* const CIMQualifier::CIM_QUAL_OVERRIDE 		= "Override";
const char* const CIMQualifier::CIM_QUAL_PROPAGATED 	= "Propagated";
const char* const CIMQualifier::CIM_QUAL_PROPERTYUSAGE  = "PropertyUsage";
const char* const CIMQualifier::CIM_QUAL_PROVIDER       = "Provider";
const char* const CIMQualifier::CIM_QUAL_READ 			= "Read";
const char* const CIMQualifier::CIM_QUAL_REQUIRED 		= "Required";
const char* const CIMQualifier::CIM_QUAL_REVISION 		= "Revision";
const char* const CIMQualifier::CIM_QUAL_SCHEMA         = "Schema";
const char* const CIMQualifier::CIM_QUAL_SOURCE         = "Source";
const char* const CIMQualifier::CIM_QUAL_SOURCETYPE     = "SourceType";
const char* const CIMQualifier::CIM_QUAL_STATIC         = "Static";
const char* const CIMQualifier::CIM_QUAL_SYNTAX         = "Syntax";
const char* const CIMQualifier::CIM_QUAL_SYNTAXTYPE     = "SyntaxType";
const char* const CIMQualifier::CIM_QUAL_TERMINAL 		= "Terminal";
const char* const CIMQualifier::CIM_QUAL_TRIGGERTYPE    = "TriggerType";
const char* const CIMQualifier::CIM_QUAL_UNITS 			= "Units";
const char* const CIMQualifier::CIM_QUAL_UNKNOWNVALUES  = "UnknownValues";
const char* const CIMQualifier::CIM_QUAL_UNSUPPORTEDVALUES = "UnsupportedValues";
const char* const CIMQualifier::CIM_QUAL_VALUEMAP 		= "ValueMap";
const char* const CIMQualifier::CIM_QUAL_VALUES         = "Values";
const char* const CIMQualifier::CIM_QUAL_VERSION 		= "Version";
const char* const CIMQualifier::CIM_QUAL_WEAK 			= "Weak";
const char* const CIMQualifier::CIM_QUAL_WRITE 			= "Write";

} // end namespace OWBI1

