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

#include "OW_config.h"
#include "OW_StringBuffer.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_String.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMName.hpp"
#include "OW_COWIntrusiveCountableBase.hpp"
#include <algorithm> // for std::sort

namespace OpenWBEM
{

using std::ostream;
using std::istream;
using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
struct CIMInstance::INSTData : public COWIntrusiveCountableBase
{
	CIMName m_owningClassName;
	CIMPropertyArray m_keys;
	CIMPropertyArray m_properties;
	CIMQualifierArray m_qualifiers;
	String m_language;
	INSTData* clone() const { return new INSTData(*this); }
};
bool operator<(const CIMInstance::INSTData& x, const CIMInstance::INSTData& y)
{
	return StrictWeakOrdering(
		x.m_owningClassName, y.m_owningClassName,
		x.m_properties, y.m_properties,
		x.m_keys, y.m_keys,
		x.m_qualifiers, y.m_qualifiers);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance() :
	CIMElement(), m_pdata(new INSTData)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(CIMNULL_t) :
	CIMElement(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(const char* name) :
	CIMElement(), m_pdata(new INSTData)
{
	m_pdata->m_owningClassName = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(const CIMName& name) :
	CIMElement(), m_pdata(new INSTData)
{
	m_pdata->m_owningClassName = name;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance::CIMInstance(const CIMInstance& x) :
		CIMElement(), m_pdata(x.m_pdata)
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
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance& CIMInstance::operator= (const CIMInstance& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setKeys(const CIMPropertyArray& keys)
{
	m_pdata->m_keys = keys;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMInstance::getClassName() const
{
	return m_pdata->m_owningClassName.toString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMInstance::getLanguage() const
{
	return m_pdata->m_language;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMInstance::setLanguage(const String& language)
{
	m_pdata->m_language = language;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setClassName(const CIMName& name)
{
	m_pdata->m_owningClassName = name;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierArray
CIMInstance::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifier
CIMInstance::getQualifier(const CIMName& qualName) const
{
	for (size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		CIMQualifier qual = m_pdata->m_qualifiers[i];
		if (qual.getName() == qualName)
		{
			return qual;
		}
	}
	return CIMQualifier(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::removeQualifier(const CIMName& qualName)
{
	for (size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		if (m_pdata->m_qualifiers[i].getName() == qualName)
		{
			m_pdata->m_qualifiers.remove(i);
			break;
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setQualifier(const CIMQualifier& qual)
{
	if (qual)
	{
		CIMName qualName = qual.getName();
		for (size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			if (m_pdata->m_qualifiers[i].getName() == qualName)
			{
				m_pdata->m_qualifiers[i] = qual;
				return *this;
			}
		}
		m_pdata->m_qualifiers.append(qual);
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setQualifiers(const CIMQualifierArray& quals)
{
	m_pdata->m_qualifiers = quals;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMPropertyArray
CIMInstance::getProperties(Int32 valueDataType) const
{
	if (valueDataType == CIMDataType::INVALID)
	{
		return m_pdata->m_properties;
	}
	CIMPropertyArray pra;
	for (size_t i = 0; i < m_pdata->m_properties.size(); i++)
	{
		CIMProperty prop = m_pdata->m_properties[i];
		if (prop.getDataType().getType() == valueDataType)
		{
			pra.append(prop);
		}
	}
	return pra;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::setProperties(const CIMPropertyArray& props)
{
	m_pdata->m_properties = props;
	_buildKeys();
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMInstance::getProperty(const CIMName& name,
	const CIMName& originClass) const
{
	int tsize = m_pdata->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_pdata->m_properties[i];
		if (originClass == cp.getOriginClass()
			&& name == cp.getName())
		{
			return(cp);
		}
	}
	return CIMProperty(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMInstance::getProperty(const CIMName& propertyName) const
{
	int tsize = m_pdata->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_pdata->m_properties[i];
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
		OW_THROW_ERR(NoSuchPropertyException, propertyName.toString().c_str(), E_INSTANCE_HAS_NO_SUCH_PROPERTY);
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
	m_pdata->m_keys.clear();
	int tsize = m_pdata->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_pdata->m_properties[i];
		if (cp.isKey())
		{
			m_pdata->m_keys.append(cp);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMPropertyArray
CIMInstance::getKeyValuePairs() const
{
	return m_pdata->m_keys;
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
		int tsize = m_pdata->m_properties.size();
		for (int i = 0; i < tsize; i++)
		{
			CIMProperty cp = m_pdata->m_properties[i];
			CIMName rname = cp.getName();
			if (rname == name)
			{
				m_pdata->m_properties[i].setValue(prop.getValue());
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
	int tsize = m_pdata->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_pdata->m_properties[i];
		CIMName rname = cp.getName();
		if (rname == name)
		{
			m_pdata->m_properties[i].setValue(cv);
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
	m_pdata->m_properties.append(cp);
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
		int tsize = m_pdata->m_properties.size();
		for (int i = 0; i < tsize; i++)
		{
			CIMProperty cp = m_pdata->m_properties[i];
			CIMName rname = cp.getName();
			if (rname == propName)
			{
				m_pdata->m_properties[i] = prop;
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
		m_pdata->m_properties.append(prop);
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
	int tsize = m_pdata->m_properties.size();
	for (int i = 0; i < tsize; i++)
	{
		CIMProperty cp = m_pdata->m_properties[i];
		if (cp.getName() == propName)
		{
			m_pdata->m_properties.remove(i);
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
CIMInstance::clone(ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList) const
{
	StringArray lproplist;
	bool noprops = false;
	if (propertyList)
	{
		if (propertyList->size() == 0)
		{
			noprops = true;
		}
		else
		{
			lproplist = *propertyList;
		}
	}
	return clone(localOnly, includeQualifiers, includeClassOrigin, lproplist,
		noprops);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMInstance::clone(ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray& propertyList,
	bool noProps) const
{
	CIMInstance ci;
	ci.m_pdata->m_owningClassName = m_pdata->m_owningClassName;
	ci.m_pdata->m_keys = m_pdata->m_keys;
	ci.m_pdata->m_language = m_pdata->m_language;
	//
	// Process qualifiers
	//
	if (includeQualifiers == E_INCLUDE_QUALIFIERS)
	{
		CIMQualifierArray qra;
		for (size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			CIMQualifier cq = m_pdata->m_qualifiers[i];
			if ((localOnly == E_LOCAL_ONLY) && cq.getPropagated())
			{
				continue;
			}
			qra.append(cq);
		}
		ci.m_pdata->m_qualifiers = qra;
	}
	if (!noProps)
	{
		CIMPropertyArray props;
		for (size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			CIMProperty prop = m_pdata->m_properties[i];
			if ((localOnly == E_LOCAL_ONLY) && prop.getPropagated())
			{
				continue;
			}
			//
			// If propertyList is not NULL then check this is a request property
			//
			if (propertyList.size() != 0)
			{
				CIMName pName = prop.getName();
				for (size_t j = 0; j < propertyList.size(); j++)
				{
					if (pName == propertyList[j])
					{
						props.append(prop.clone(includeQualifiers,
							includeClassOrigin));
						break;
					}
				}
			}
			else
			{
				props.append(prop.clone(includeQualifiers,
					includeClassOrigin));
			}
		}
		ci.m_pdata->m_properties = props;
	}
	return ci;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMInstance::clone(ELocalOnlyFlag localOnly, EDeepFlag deep, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
	const CIMClass& requestedClass, const CIMClass& cimClass) const
{
	CIMInstance ci(*this);
	ci.syncWithClass(cimClass, E_INCLUDE_QUALIFIERS);
	ci = ci.clone(E_NOT_LOCAL_ONLY, includeQualifiers,
		includeClassOrigin, propertyList);
	// do processing of deep & localOnly
	// don't filter anything if (deep == E_DEEP && localOnly == E_NOT_LOCAL_ONLY)
	if (deep != E_DEEP || localOnly != E_NOT_LOCAL_ONLY)
	{
		CIMPropertyArray props = ci.getProperties();
		CIMPropertyArray newprops;
		CIMInstance newInst(ci);
		CIMName requestedClassName = requestedClass.getName();
		for (size_t i = 0; i < props.size(); ++i)
		{
			CIMProperty p = props[i];
			CIMProperty clsp = requestedClass.getProperty(p.getName());
			if (clsp)
			{
				if (clsp.getOriginClass() == requestedClassName)
				{
					newprops.push_back(p);
					continue;
				}
			}
			if (deep == E_DEEP)
			{
				if (!clsp
					|| p.getOriginClass() != clsp.getOriginClass())
				{
					// the property is from a derived class
					newprops.push_back(p);
					continue;
				}
			}
			if (localOnly == E_NOT_LOCAL_ONLY)
			{
				if (clsp)
				{
					// the property has to be from a superclass
					newprops.push_back(p);
					continue;
				}
			}
		}
		newInst.setProperties(newprops);
		newInst.setKeys(ci.getKeyValuePairs());
		ci = newInst;
	}
	return ci;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMInstance::filterProperties(const StringArray& propertyList,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	bool ignorePropertyList) const
{
	bool noprops(propertyList.size() == 0 && ignorePropertyList == false);
	return clone(E_NOT_LOCAL_ONLY, includeQualifiers, includeClassOrigin, propertyList,
		noprops);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMInstance::getName() const
{
	return m_pdata->m_owningClassName.toString();
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance&
CIMInstance::syncWithClass(const CIMClass& theClass,
	EIncludeQualifiersFlag includeQualifiers)
{
	if (!theClass)
	{
		return *this;
	}
/*
	if (includeQualifiers)
	{
		// Ensure all class qualifiers are on the instance
		CIMQualifierArray classQuals = theClass.getQualifiers();
		for (size_t i = 0; i < classQuals.size(); i++)
		{
			CIMQualifier qual = classQuals[i];
			CIMName clsQualName = qual.getName();
			if (!getQualifier(clsQualName))
			{
				if (qual.hasFlavor(CIMFlavor::TOINSTANCE))
				{
					// Qualifier is to be propagated to instances
					setQualifier(qual);
				}
			}
			else
			{
				if (!qual.hasFlavor(CIMFlavor::TOINSTANCE))
				{
					// Qualifier is not to be propagated to instances
					removeQualifier(clsQualName);
				}
				else
				{
					if (!qual.hasFlavor(CIMFlavor::ENABLEOVERRIDE))
					{
						// Override not allowed. Sync with class qualifier
						setQualifier(qual);
					}
				}
			}
		}
	}
*/
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
	if (!includeQualifiers)
	{
		for (size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			m_pdata->m_properties[i].clearNonKeyQualifiers();
		}
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMInstance::createModifiedInstance(
	const CIMInstance& previousInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	const CIMClass& theClass) const
{
	CIMInstance newInst(*this);
	if (!includeQualifiers)
	{
		newInst.setQualifiers(previousInstance.getQualifiers());
	}
	if (propertyList)
	{
		newInst.setProperties(previousInstance.getProperties());
		for (StringArray::const_iterator i = propertyList->begin();
			 i != propertyList->end(); ++i)
		{
			CIMProperty p = this->getProperty(*i);
			if (p)
			{
				if (!includeQualifiers)
				{
					CIMProperty cp = theClass.getProperty(*i);
					if (cp)
					{
						p.setQualifiers(cp.getQualifiers());
					}
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
	m_pdata->m_owningClassName = name;
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

	UInt32 version = CIMBase::readSig(istrm, OW_CIMINSTANCESIG,
		OW_CIMINSTANCESIG_V, CIMInstance::SERIALIZATION_VERSION);

	owningClassName.readObject(istrm);
	BinarySerialization::readArray(istrm, keys);
	BinarySerialization::readArray(istrm, properties);
	BinarySerialization::readArray(istrm, qualifiers);
	// If dealing with versioned format, then read language
	if (version > 0)
	{
		language.readObject(istrm);
	}
	if (!m_pdata)
	{
		m_pdata = new INSTData;
	}
	m_pdata->m_owningClassName = owningClassName;
	m_pdata->m_keys = keys;
	m_pdata->m_properties = properties;
	m_pdata->m_qualifiers = qualifiers;
	m_pdata->m_language = language;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMInstance::writeObject(std::ostream &ostrm) const
{
	CIMBase::writeSig(ostrm, OW_CIMINSTANCESIG_V, CIMInstance::SERIALIZATION_VERSION);
	m_pdata->m_owningClassName.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_pdata->m_keys);
	BinarySerialization::writeArray(ostrm, m_pdata->m_properties);
	BinarySerialization::writeArray(ostrm, m_pdata->m_qualifiers);
	m_pdata->m_language.writeObject(ostrm);
}
//////////////////////////////////////////////////////////////////////////////
String
CIMInstance::toMOF() const
{
	size_t i;
	StringBuffer rv;
	if (m_pdata->m_qualifiers.size() > 0)
	{
		rv += "[\n";
		for (i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			if (i > 0)
			{
				rv += ',';
			}
			rv += m_pdata->m_qualifiers[i].toMOF();
		}
		rv += "]\n";
	}
	rv += "INSTANCE OF ";
	rv += m_pdata->m_owningClassName.toString();
	rv += "\n{\n";
	for (i = 0; i < m_pdata->m_properties.size(); i++)
	{
		// note that we can't use CIMProperty::toMOF() since it prints out
		// the data type.
		const CIMProperty& p = m_pdata->m_properties[i];
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
			rv += p.getName();
			rv += '=';
			rv += v.toMOF();
			rv += ";\n";
		}
		else
		{
			rv += "  ";
			rv += p.getName();
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
	temp += m_pdata->m_owningClassName.toString() + " {\n";
	for (i = 0; i < m_pdata->m_properties.size(); i++)
	{
		CIMProperty cp = m_pdata->m_properties[i];
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
		temp += cp.getName() + " = " + outVal + ";\n";
	}
	temp += "}\n";
	return temp.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMInstance& x, const CIMInstance& y)
{
	return *x.m_pdata < *y.m_pdata;
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

} // end namespace OpenWBEM

