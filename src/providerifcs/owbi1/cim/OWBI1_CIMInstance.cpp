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

#include "OWBI1_CIMDetail.hpp"
#include "OWBI1_config.h"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_CIMName.hpp"
#include "OWBI1_CIMInstanceRep.hpp"
#include "OWBI1_CIMPropertyRep.hpp"
#include "OWBI1_CIMNameRep.hpp"
#include "OWBI1_Array.hpp"
#include "OWBI1_CIMProperty.hpp"

namespace OWBI1
{

using namespace OpenWBEM;
using std::ostream;
using std::istream;
using namespace WBEMFlags;
using namespace detail;

//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance() :
	CIMElement(), m_rep(new CIMInstanceRep)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(CIMNULL_t) :
	CIMElement(), m_rep(new CIMInstanceRep(OpenWBEM::CIMInstance(OpenWBEM::CIMNULL)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(const char* name) :
	CIMElement(), m_rep(new CIMInstanceRep(OpenWBEM::CIMInstance(name)))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(const CIMName& name) :
	CIMElement(), m_rep(new CIMInstanceRep(OpenWBEM::CIMInstance(name.c_str())))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(const CIMInstance& x) :
		CIMElement(x), m_rep(x.m_rep)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::~CIMInstance()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMInstance::setNull()
{
	m_rep = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance& CIMInstance::operator= (const CIMInstance& x)
{
	m_rep = x.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setKeys(const CIMPropertyArray& keys)
{
	OpenWBEM::CIMPropertyArray owkeys;
	unwrapArray(owkeys, keys);
	m_rep->inst.setKeys(owkeys);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMName
CIMInstance::getClassName() const
{
	return CIMName(new CIMNameRep(m_rep->inst.getClassName()));
}
//////////////////////////////////////////////////////////////////////////////
String
CIMInstance::getLanguage() const
{
	return m_rep->inst.getLanguage().c_str();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMInstance::setLanguage(const String& language)
{
	m_rep->inst.setLanguage(language.c_str());
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setClassName(const CIMName& name)
{
	m_rep->inst.setClassName(name.getRep()->name);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMPropertyArray
CIMInstance::getProperties(Int32 valueDataType) const
{
	OpenWBEM::CIMPropertyArray owpa = m_rep->inst.getProperties(valueDataType);
	CIMPropertyArray rv;
	unwrapArray(rv, owpa);
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setProperties(const CIMPropertyArray& props)
{
	OpenWBEM::CIMPropertyArray owprops;
	unwrapArray(owprops, props);
	m_rep->inst.setProperties(owprops);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMInstance::getProperty(const CIMName& name,
	const CIMName& originClass) const
{
	return CIMProperty(new CIMPropertyRep(m_rep->inst.getProperty(name.getRep()->name, originClass.getRep()->name)));
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMInstance::getProperty(const CIMName& propertyName) const
{
	int tsize = m_rep->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_rep->m_properties[i];
		if (propertyName == cp.getName())
		{
			return(cp);
		}
	}
	return CIMProperty(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMInstance::getPropertyT(const CIMName& propertyName) const
{
	CIMProperty p = getProperty(propertyName);
	if (!p)
	{
		OWBI1_THROW_ERR(NoSuchPropertyException, propertyName.toString().c_str(), E_INSTANCE_HAS_NO_SUCH_PROPERTY);
	}
	return p;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMInstance::getPropertyValue(const CIMName& name) const
{
	CIMProperty p = this->getProperty(name);
	if (p)
	{
		return p.getValue();
	}
	return CIMValue(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMInstance::propertyHasValue(const CIMName& name) const
{
	CIMProperty p = this->getProperty(name);
	if (p)
	{
		CIMValue v = p.getValue();
		if (v)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE.
void
CIMInstance::_buildKeys()
{
	m_rep->m_keys.clear();
	int tsize = m_rep->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_rep->m_properties[i];
		if (cp.isKey())
		{
			m_rep->m_keys.append(cp);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMPropertyArray
CIMInstance::getKeyValuePairs() const
{
	return m_rep->m_keys;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::updatePropertyValues(const CIMPropertyArray& props)
{
	int tsize = props.size();
	for (int i = 0; i < tsize; i++)
	{
		updatePropertyValue(props[i]);
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::updatePropertyValue(const CIMProperty& prop)
{
	bool buildTheKeys = false;
	if (prop)
	{
		CIMName name = prop.getName();
		int tsize = m_rep->m_properties.size();
		for (int i = 0; i < tsize; i++)
		{
			CIMProperty cp = m_rep->m_properties[i];
			CIMName rname = cp.getName();
			if (rname == name)
			{
				m_rep->m_properties[i].setValue(prop.getValue());
				if (cp.isKey() || prop.isKey())
				{
					//
					// If keys are not null and this is a key
					// rebuild the key list when we're done
					//
					buildTheKeys = true;
				}
				break;
			}
		}
		if (buildTheKeys)
		{
			_buildKeys();
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::updatePropertyValue(const CIMName& name, const CIMValue& value)
{
	return updatePropertyValue(CIMProperty(name, value));
}

//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setProperty(const CIMName& name, const CIMValue& cv)
{
	int tsize = m_rep->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_rep->m_properties[i];
		CIMName rname = cp.getName();
		if (rname == name)
		{
			m_rep->m_properties[i].setValue(cv);
			if (cp.isKey())
			{
				_buildKeys();
			}
			return *this;
		}
	}
	//
	// Not found so add it
	//
	CIMProperty cp(name);
	cp.setValue(cv);
	if (cv)
	{
		cp.setDataType(cv.getCIMDataType());
	}
	else
	{
		cp.setDataType(CIMDataType::CIMNULL);
	}
	m_rep->m_properties.append(cp);
	//
	// We don't worry about building the keys here, because the
	// property doesn't have the key qualifier (or any other for that matter)
	//
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setProperty(const CIMProperty& prop)
{
	if (prop)
	{
		CIMName propName = prop.getName();
		int tsize = m_rep->m_properties.size();
		for (int i = 0; i < tsize; i++)
		{
			CIMProperty cp = m_rep->m_properties[i];
			CIMName rname = cp.getName();
			if (rname == propName)
			{
				m_rep->m_properties[i] = prop;
				// If property was a key or is a key, then rebuild the
				// key values
				if (cp.isKey() || prop.isKey())
				{
					_buildKeys();
				}
				return *this;
			}
		}
		//
		// Not found so add it
		//
		m_rep->m_properties.append(prop);
		if (prop.isKey())
		{
			_buildKeys();
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::removeProperty(const CIMName& propName)
{
	int tsize = m_rep->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_rep->m_properties[i];
		if (cp.getName() == propName)
		{
			m_rep->m_properties.remove(i);
			// If this property was a key, then rebuild the key values
			if (cp.isKey())
			{
				_buildKeys();
			}
			break;
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMInstance::clone(const CIMPropertyList& propertyList) const
{
	CIMInstance ci;
	ci.m_rep->m_owningClassName = m_rep->m_owningClassName;
	ci.m_rep->m_keys = m_rep->m_keys;
	ci.m_rep->m_language = m_rep->m_language;
	ci.m_rep->m_qualifiers = m_rep->m_qualifiers;
	CIMPropertyArray props;
	for (size_t i = 0; i < m_rep->m_properties.size(); i++)
	{
		CIMProperty prop = m_rep->m_properties[i];
		if (propertyList.hasProperty(prop.getName()))
		{
			props.append(prop);
		}
	}
	ci.m_rep->m_properties = props;
	return ci;
}
//////////////////////////////////////////////////////////////////////////////
CIMName
CIMInstance::getName() const
{
	return m_rep->m_owningClassName;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::syncWithClass(const CIMClass& theClass)
{
	if (!theClass)
	{
		return *this;
	}
	CIMName propName;
	CIMPropertyArray classProps = theClass.getAllProperties();
	CIMPropertyArray instProps = getProperties();
	
	// Remove properties that are not defined in the class
	size_t i = 0;
	while (i < instProps.size())
	{
		propName = instProps[i].getName();
		if (!theClass.getProperty(propName))
		{
			instProps.remove(i);
		}
		else
		{
			++i;
		}
	}

	// Add missing properties and ensure existing have right class origin,
	// and qualifiers
	for (size_t i = 0; i < classProps.size(); i++)
	{
		bool found = false;
		CIMProperty cprop = classProps[i];
		propName = cprop.getName();
		for (size_t j = 0; j < instProps.size(); j++)
		{
			CIMProperty iprop = instProps[j];
			if (iprop.getName() == propName)
			{
				CIMValue cv = iprop.getValue();
				iprop = cprop;
				if (cv)
				{
					if (cv.getType() != iprop.getDataType().getType())
					{
						// workaround for the stupid dmtf string/embedded object hack
						if (cv.getType() != CIMDataType::EMBEDDEDCLASS && cv.getType() != CIMDataType::EMBEDDEDINSTANCE)
						{
							cv = CIMValueCast::castValueToDataType(cv,
								iprop.getDataType());
						}
					}
					iprop.setValue(cv);
				}
				instProps[j] = iprop;
				found = true;
				break;
			}
		}
		if (!found)
		{
			instProps.append(classProps[i]);
		}
	}
	setProperties(instProps);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMInstance::createModifiedInstance(
	const CIMInstance& previousInstance,
	const CIMPropertyList& propertyList,
	const CIMClass& theClass) const
{
	CIMInstance newInst(*this);
	newInst.setQualifiers(previousInstance.getQualifiers());
	if (!propertyList.isAllProperties())
	{
		newInst.setProperties(previousInstance.getProperties());
		for (CIMPropertyList::const_iterator i = propertyList.begin();
			 i != propertyList.end(); ++i)
		{
			CIMProperty p = this->getProperty(*i);
			if (p)
			{
				CIMProperty cp = theClass.getProperty(*i);
				if (cp)
				{
					p.setQualifiers(cp.getQualifiers());
				}
				newInst.setProperty(p);
			}
			else
			{
				CIMProperty cp = theClass.getProperty(*i);
				if (cp)
				{
					newInst.setProperty(cp);
				}
				else
				{
					newInst.removeProperty(*i);
				}
			}
		}
	}
	return newInst;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMInstance::setName(const CIMName& name)
{
	m_rep->m_owningClassName = name;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMInstance::readObject(istream &istrm)
{
	CIMName owningClassName;
	CIMPropertyArray properties;
	CIMPropertyArray keys;
	CIMQualifierArray qualifiers;
	String language;

	UInt32 version = CIMBase::readSig(istrm, OWBI1_CIMINSTANCESIG,
		OWBI1_CIMINSTANCESIG_V, CIMInstance::SERIALIZATION_VERSION);

	owningClassName.readObject(istrm);
	BinarySerialization::readArray(istrm, keys);
	BinarySerialization::readArray(istrm, properties);
	BinarySerialization::readArray(istrm, qualifiers);
	// If dealing with versioned format, then read language
	if (version > 0)
	{
		language.readObject(istrm);
	}
	if (!m_rep)
	{
		m_rep = new INSTData;
	}
	m_rep->m_owningClassName = owningClassName;
	m_rep->m_keys = keys;
	m_rep->m_properties = properties;
	m_rep->m_qualifiers = qualifiers;
	m_rep->m_language = language;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMInstance::writeObject(std::ostream &ostrm) const
{
	CIMBase::writeSig(ostrm, OWBI1_CIMINSTANCESIG_V, CIMInstance::SERIALIZATION_VERSION);
	m_rep->m_owningClassName.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_rep->m_keys);
	BinarySerialization::writeArray(ostrm, m_rep->m_properties);
	BinarySerialization::writeArray(ostrm, m_rep->m_qualifiers);
	m_rep->m_language.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMInstance::toMOF() const
{
	size_t i;
	StringBuffer rv;
	if (m_rep->m_qualifiers.size() > 0)
	{
		rv += "[\n";
		for (i = 0; i < m_rep->m_qualifiers.size(); i++)
		{
			if (i > 0)
			{
				rv += ',';
			}
			rv += m_rep->m_qualifiers[i].toMOF();
		}
		rv += "]\n";
	}
	rv += "INSTANCE OF ";
	rv += m_rep->m_owningClassName.toString();
	rv += "\n{\n";
	for (i = 0; i < m_rep->m_properties.size(); i++)
	{
		// note that we can't use CIMProperty::toMOF() since it prints out
		// the data type.
		const CIMProperty& p = m_rep->m_properties[i];
		if (p.hasTrueQualifier(CIMQualifier::CIM_QUAL_INVISIBLE))
		{
			continue;
		}
		CIMValue v = p.getValue();
		if (v)
		{
			// do qualifiers
			CIMQualifierArray qualifiers = p.getQualifiers();
			if (qualifiers.size() > 0)
			{
				rv += "  [";
				for (size_t i = 0; i < qualifiers.size(); i++)
				{
					const CIMQualifier& nq = qualifiers[i];
					if (i > 0)
					{
						rv += ',';
					}
					rv += nq.toMOF();
				}
				rv += "]\n";
			}
			rv += "  ";
			rv += p.getName().toString();
			rv += '=';
			rv += v.toMOF();
			rv += ";\n";
		}
		else
		{
			rv += "  ";
			rv += p.getName().toString();
			rv += "=NULL;\n";
		}
	}
	rv += "};\n";
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMInstance::toString() const
{
	size_t i;
	StringBuffer temp;
	String outVal;
	temp += "instance of ";
	temp += m_rep->m_owningClassName.toString() + " {\n";
	for (i = 0; i < m_rep->m_properties.size(); i++)
	{
		CIMProperty cp = m_rep->m_properties[i];
		if (cp.hasTrueQualifier(CIMQualifier::CIM_QUAL_INVISIBLE))
		{
			continue;
		}

		CIMValue val = cp.getValue();
		if (!val)
		{
			outVal = "null";
		}
		else
		{
			outVal = val.toString();
		}
		temp += cp.getName().toString() + " = " + outVal + ";\n";
	}
	temp += "}\n";
	return temp.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMInstance& x, const CIMInstance& y)
{
	return *x.m_rep < *y.m_rep;
}
//////////////////////////////////////////////////////////////////////////////
bool CIMInstance::propertiesAreEqualTo(const CIMInstance& other) const
{
	CIMPropertyArray props1(getProperties());
	CIMPropertyArray props2(other.getProperties());
	if (props1.size() != props2.size())
	{
		return false;
	}
	std::sort(props1.begin(), props1.end());
	std::sort(props2.begin(), props2.end());
	CIMPropertyArray::iterator i1 = props1.begin();
	CIMPropertyArray::iterator i2 = props2.begin();
	while (i1 != props1.end())
	{
		CIMProperty p1 = *i1;
		CIMProperty p2 = *i2;
		if (p1 != p2) // checks the name
		{
			return false;
		}
		if (p1.getValue() != p2.getValue()) // check the value
		{
			return false;
		}
		++i1;
		++i2;
	}
	return true;
}

CIMInstance::safe_bool
CIMInstance::operator CIMInstance::safe_bool() const
{  
	return m_rep->inst ? &CIMInstance::m_rep : 0; 
}

bool 
CIMInstance::operator!() const
{  
	return !(m_rep->inst); 
}

CIMInstanceRepRef
CIMInstance::getRep() const
{
	return m_rep;
}

} // end namespace OWBI1

