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
#include "OW_StringBuffer.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_String.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMQualifier.hpp"

#include <algorithm> // for std::sort

using std::ostream;
using std::istream;

//////////////////////////////////////////////////////////////////////////////
struct OW_CIMInstance::INSTData
{
	OW_String m_owningClassName;
	OW_CIMPropertyArray m_keys;
	OW_CIMPropertyArray m_properties;
	OW_CIMQualifierArray m_qualifiers;

    INSTData* clone() const { return new INSTData(*this); }
};

bool operator<(const OW_CIMInstance::INSTData& x, const OW_CIMInstance::INSTData& y)
{
	return OW_StrictWeakOrdering(
		x.m_owningClassName, y.m_owningClassName,
		x.m_properties, y.m_properties,
		x.m_keys, y.m_keys,
		x.m_qualifiers, y.m_qualifiers);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance::OW_CIMInstance() :
	OW_CIMElement(), m_pdata(new INSTData)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance::OW_CIMInstance(OW_CIMNULL_t) :
	OW_CIMElement(), m_pdata(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance::OW_CIMInstance(const char* name) :
	OW_CIMElement(), m_pdata(new INSTData)
{
	m_pdata->m_owningClassName = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance::OW_CIMInstance(const OW_String& name) :
	OW_CIMElement(), m_pdata(new INSTData)
{
	m_pdata->m_owningClassName = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance::OW_CIMInstance(const OW_CIMInstance& x) :
		OW_CIMElement(), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance::~OW_CIMInstance()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMInstance::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance& OW_CIMInstance::operator= (const OW_CIMInstance& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::setKeys(const OW_CIMPropertyArray& keys)
{
	m_pdata->m_keys = keys;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMInstance::getClassName() const
{
	return m_pdata->m_owningClassName;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::setClassName(const OW_String& name)
{
	m_pdata->m_owningClassName = name;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierArray
OW_CIMInstance::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier
OW_CIMInstance::getQualifier(const OW_String& qualName) const
{
	for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		OW_CIMQualifier qual = m_pdata->m_qualifiers[i];
		if(qual.getName().equalsIgnoreCase(qualName))
		{
			return qual;
		}
	}

	return OW_CIMQualifier(OW_CIMNULL);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::removeQualifier(const OW_String& qualName)
{
	for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		if(m_pdata->m_qualifiers[i].getName().equalsIgnoreCase(qualName))
		{
			m_pdata->m_qualifiers.remove(i);
			break;
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::setQualifier(const OW_CIMQualifier& qual)
{
	if(qual)
	{
		OW_String qualName = qual.getName();
		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			if(m_pdata->m_qualifiers[i].getName().equalsIgnoreCase(qualName))
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
OW_CIMInstance&
OW_CIMInstance::setQualifiers(const OW_CIMQualifierArray& quals)
{
	m_pdata->m_qualifiers = quals;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMPropertyArray
OW_CIMInstance::getProperties(OW_Int32 valueDataType) const
{
	if(valueDataType == OW_CIMDataType::INVALID)
	{
		return m_pdata->m_properties;
	}

	OW_CIMPropertyArray pra;
	for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
	{
		OW_CIMProperty prop = m_pdata->m_properties[i];
		if(prop.getDataType().getType() == valueDataType)
		{
			pra.append(prop);
		}
	}

	return pra;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::setProperties(const OW_CIMPropertyArray& props)
{
	m_pdata->m_properties = props;
	_buildKeys();
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMInstance::getProperty(const OW_String& name,
	const OW_String& originClass) const
{
	int tsize = m_pdata->m_properties.size();
	for(int i = 0; i < tsize; i++)
	{
		OW_CIMProperty cp = m_pdata->m_properties[i];

		if(originClass.equalsIgnoreCase(cp.getOriginClass())
			&& name.equalsIgnoreCase(cp.getName()))
		{
			return(cp);
		}
	}

	return OW_CIMProperty(OW_CIMNULL);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMInstance::getProperty(const OW_String& propertyName) const
{
	if(propertyName.empty())
		return OW_CIMProperty(OW_CIMNULL);

	int tsize = m_pdata->m_properties.size();
	for(int i = 0; i < tsize; i++)
	{
		OW_CIMProperty cp = m_pdata->m_properties[i];

		OW_String pname = cp.getName();
		if(propertyName.equalsIgnoreCase(cp.getName()))
			return(cp);
	}

	return OW_CIMProperty(OW_CIMNULL);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMInstance::getPropertyT(const OW_String& propertyName) const
{
	OW_CIMProperty p = getProperty(propertyName);
	if (!p)
	{
		OW_THROW(OW_NoSuchPropertyException, propertyName.c_str());
	}
	return p;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE.
// Assumes writelock has been obtained.
void
OW_CIMInstance::_buildKeys()
{
	m_pdata->m_keys.clear();
	int tsize = m_pdata->m_properties.size();
	for(int i = 0; i < tsize; i++)
	{
		OW_CIMProperty cp = m_pdata->m_properties[i];
		if(cp.isKey())
		{
			m_pdata->m_keys.append(cp.clone(true, true));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMPropertyArray
OW_CIMInstance::getKeyValuePairs() const
{
	return m_pdata->m_keys;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::updatePropertyValues(const OW_CIMPropertyArray& props)
{
	int tsize = props.size();
	for(int i = 0; i < tsize; i++)
	{
		updatePropertyValue(props[i]);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::updatePropertyValue(const OW_CIMProperty& prop)
{
	OW_Bool buildTheKeys = false;

	if(prop)
	{
		OW_String name = prop.getName();

		int tsize = m_pdata->m_properties.size();
		for(int i = 0; i < tsize; i++)
		{
			OW_CIMProperty cp = m_pdata->m_properties[i];
			OW_String rname = cp.getName();

			if(rname.equalsIgnoreCase(name))
			{
				m_pdata->m_properties[i].setValue(prop.getValue());
				if(cp.isKey() || prop.isKey())
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

		if(buildTheKeys)
		{
			_buildKeys();
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::setProperty(const OW_String& name, const OW_CIMValue& cv)
{
	int tsize = m_pdata->m_properties.size();

	for(int i = 0; i < tsize; i++)
	{
		OW_CIMProperty cp = m_pdata->m_properties[i];
		OW_String rname = cp.getName();

		if(rname.equalsIgnoreCase(name))
		{
			m_pdata->m_properties[i].setValue(cv);
			if(cp.isKey())
			{
				_buildKeys();
			}
			return *this;
		}
	}

	//
	// Not found so add it
	//
	OW_CIMProperty cp(name);
	cp.setValue(cv);
	if(cv)
	{
		cp.setDataType(cv.getCIMDataType());
	}
	else
	{
		cp.setDataType(OW_CIMDataType::CIMNULL);
	}

	m_pdata->m_properties.append(cp);
	//
	// We don't worry about building the keys here, because the
	// property doesn't have the key qualifier (or any other for that matter)
	//
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::setProperty(const OW_CIMProperty& prop)
{
	if(prop)
	{
		OW_String propName = prop.getName();

		int tsize = m_pdata->m_properties.size();
		for(int i = 0; i < tsize; i++)
		{
			OW_CIMProperty cp = m_pdata->m_properties[i];
			OW_String rname = cp.getName();

			if(rname.equalsIgnoreCase(propName))
			{
				m_pdata->m_properties[i] = prop;

				// If property was a key or is a key, then rebuild the
				// key values
				if(cp.isKey() || prop.isKey())
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
		if(prop.isKey())
		{
			_buildKeys();
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::removeProperty(const OW_String& propName)
{
	int tsize = m_pdata->m_properties.size();
	for(int i = 0; i < tsize; i++)
	{
		OW_CIMProperty cp = m_pdata->m_properties[i];
		if(cp.getName().equalsIgnoreCase(propName))
		{
			m_pdata->m_properties.remove(i);

			// If this property was a key, then rebuild the key values
			if(cp.isKey())
			{
				_buildKeys();
			}
			break;
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMInstance::clone(OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray* propertyList) const
{
	OW_StringArray lproplist;
	OW_Bool noprops = false;
	if(propertyList)
	{
		if(propertyList->size() == 0)
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
OW_CIMInstance
OW_CIMInstance::clone(OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray& propertyList,
	OW_Bool noProps) const
{
	OW_CIMInstance ci;
	ci.m_pdata->m_owningClassName = m_pdata->m_owningClassName;
	ci.m_pdata->m_keys = m_pdata->m_keys;

	//
	// Process qualifiers
	//
	if(includeQualifiers)
	{
		OW_CIMQualifierArray qra;
		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			OW_CIMQualifier cq = m_pdata->m_qualifiers[i];
			if(localOnly && cq.getPropagated())
			{
				continue;
			}

			qra.append(cq);
		}

		ci.m_pdata->m_qualifiers = qra;
	}

	if(!noProps)
	{
		OW_CIMPropertyArray props;
		for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			OW_CIMProperty prop = m_pdata->m_properties[i];
			if(localOnly && prop.getPropagated())
			{
				continue;
			}

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
OW_CIMInstance
OW_CIMInstance::clone(OW_Bool localOnly, OW_Bool deep, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
	const OW_CIMClass& requestedClass, const OW_CIMClass& cimClass) const
{
	OW_CIMInstance ci(*this);
	ci.syncWithClass(cimClass, true);
	ci = ci.clone(false, includeQualifiers,
		includeClassOrigin, propertyList);

	// do processing of deep & localOnly
	// don't filter anything if (deep == true && localOnly == false)
	if (deep != true || localOnly != false)
	{
		OW_CIMPropertyArray props = ci.getProperties();
		OW_CIMPropertyArray newprops;
		OW_CIMInstance newInst(ci);
		OW_String requestedClassName = requestedClass.getName();
		for (size_t i = 0; i < props.size(); ++i)
		{
			OW_CIMProperty p = props[i];
			OW_CIMProperty clsp = requestedClass.getProperty(p.getName());
			if (clsp)
			{
				if (clsp.getOriginClass().equalsIgnoreCase(requestedClassName))
				{
					newprops.push_back(p);
					continue;
				}
			}
			if (deep == true)
			{
				if (!clsp
					|| !p.getOriginClass().equalsIgnoreCase(clsp.getOriginClass()))
				{
					// the property is from a derived class
					newprops.push_back(p);
					continue;
				}
			}
			if (localOnly == false)
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
OW_CIMInstance
OW_CIMInstance::filterProperties(const OW_StringArray& propertyList,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	OW_Bool ignorePropertyList) const
{
	OW_Bool noprops(propertyList.size() == 0 && ignorePropertyList == false);
	return clone(false, includeQualifiers, includeClassOrigin, propertyList,
		noprops);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMInstance::getName() const
{
	return m_pdata->m_owningClassName;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance&
OW_CIMInstance::syncWithClass(const OW_CIMClass& theClass,
	OW_Bool includeQualifiers)
{
	if(!theClass)
	{
		return *this;
	}
/*
	if(includeQualifiers)
	{
		// Ensure all class qualifiers are on the instance
		OW_CIMQualifierArray classQuals = theClass.getQualifiers();
		for(size_t i = 0; i < classQuals.size(); i++)
		{
			OW_CIMQualifier qual = classQuals[i];
			OW_String clsQualName = qual.getName();
			if(!getQualifier(clsQualName))
			{
				if(qual.hasFlavor(OW_CIMFlavor::TOINSTANCE))
				{
					// Qualifier is to be propagated to instances
					setQualifier(qual);
				}
			}
			else
			{
				if(!qual.hasFlavor(OW_CIMFlavor::TOINSTANCE))
				{
					// Qualifier is not to be propagated to instances
					removeQualifier(clsQualName);
				}
				else
				{
					if(!qual.hasFlavor(OW_CIMFlavor::ENABLEOVERRIDE))
					{
						// Override not allowed. Sync with class qualifier
						setQualifier(qual);
					}
				}
			}
		}
	}
*/

	OW_String propName;
	OW_CIMPropertyArray classProps = theClass.getAllProperties();
	OW_CIMPropertyArray instProps = getProperties();

	// Remove properties that are not defined in the class
	for(int i = 0; i < int(instProps.size()); i++)
	{
		propName = instProps[i].getName();
		if(!theClass.getProperty(propName))
		{
			instProps.remove(i);
			i--;
		}
	}

	// Add missing properties and ensure existing have right class origin,
	// and qualifiers
	for(size_t i = 0; i < classProps.size(); i++)
	{
		OW_Bool found = false;
		OW_CIMProperty cprop = classProps[i];
		propName = cprop.getName();
		for(size_t j = 0; j < instProps.size(); j++)
		{
			OW_CIMProperty iprop = instProps[j];
			if(iprop.getName().equalsIgnoreCase(propName))
			{
				OW_CIMValue cv = iprop.getValue();
				iprop = cprop;
				if(cv)
				{
					if(cv.getType() != iprop.getDataType().getType())
					{
						cv = OW_CIMValueCast::castValueToDataType(cv,
							iprop.getDataType());
					}

					iprop.setValue(cv);
				}

				instProps[j] = iprop;
				found = true;
				break;
			}
		}

		if(!found)
		{
			instProps.append(classProps[i]);
		}
	}

	setProperties(instProps);

	if(!includeQualifiers)
	{
		for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			m_pdata->m_properties[i].clearQualifiers();
		}
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMInstance::createModifiedInstance(
	const OW_CIMInstance& previousInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_CIMClass& theClass) const
{
	OW_CIMInstance newInst(*this);

	if (!includeQualifiers)
	{
		newInst.setQualifiers(previousInstance.getQualifiers());
	}

	if (propertyList)
	{
		newInst.setProperties(previousInstance.getProperties());
		for (OW_StringArray::const_iterator i = propertyList->begin();
			 i != propertyList->end(); ++i)
		{
			OW_CIMProperty p = this->getProperty(*i);
			if (p)
			{
				if (!includeQualifiers)
				{
					OW_CIMProperty cp = theClass.getProperty(*i);
					if (cp)
					{
						p.setQualifiers(cp.getQualifiers());
					}
				}
				newInst.setProperty(p);
			}
			else
			{
				OW_CIMProperty cp = theClass.getProperty(*i);
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
OW_CIMInstance::setName(const OW_String& name)
{
	m_pdata->m_owningClassName = name;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMInstance::readObject(istream &istrm)
{
	OW_String owningClassName;
	OW_CIMPropertyArray properties;
	OW_CIMPropertyArray keys;
	OW_CIMQualifierArray qualifiers;

	OW_CIMBase::readSig(istrm, OW_CIMINSTANCESIG);
	owningClassName.readObject(istrm);
	OW_BinIfcIO::readArray(istrm, keys);
	OW_BinIfcIO::readArray(istrm, properties);
	OW_BinIfcIO::readArray(istrm, qualifiers);

	if(m_pdata.isNull())
	{
		m_pdata = new INSTData;
	}

	m_pdata->m_owningClassName = owningClassName;
	m_pdata->m_keys = keys;
	m_pdata->m_properties = properties;
	m_pdata->m_qualifiers = qualifiers;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMInstance::writeObject(std::ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMINSTANCESIG );
	m_pdata->m_owningClassName.writeObject(ostrm);
	OW_BinIfcIO::writeArray(ostrm, m_pdata->m_keys);
    OW_BinIfcIO::writeArray(ostrm, m_pdata->m_properties);
	OW_BinIfcIO::writeArray(ostrm, m_pdata->m_qualifiers);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMInstance::toMOF() const
{
	size_t i;
	OW_StringBuffer rv;

	if(m_pdata->m_qualifiers.size() > 0)
	{
		rv += "[\n";

		for(i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			if(i > 0)
			{
				rv += ',';
			}

			rv += m_pdata->m_qualifiers[i].toMOF();
		}
		rv += "]\n";
	}

	rv += "INSTANCE OF ";
	rv += m_pdata->m_owningClassName;

	rv += "\n{\n";

	for(i = 0; i < m_pdata->m_properties.size(); i++)
	{
		rv += m_pdata->m_properties[i].toMOF();
	}

	rv += "};\n";
	return rv.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMInstance::toString() const
{
	size_t i;
	OW_StringBuffer temp;
	OW_String outVal;

	temp += "instance of ";
	temp += m_pdata->m_owningClassName + " {\n";

	for(i = 0; i < m_pdata->m_properties.size(); i++)
	{
		OW_CIMProperty cp = m_pdata->m_properties[i];
		OW_CIMValue val = cp.getValue();
		if(!val)
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
bool operator<(const OW_CIMInstance& x, const OW_CIMInstance& y)
{
	return *x.m_pdata < *y.m_pdata;
}

//////////////////////////////////////////////////////////////////////////////
bool OW_CIMInstance::propertiesAreEqualTo(const OW_CIMInstance& other) const
{
	OW_CIMPropertyArray props1(getProperties());
	OW_CIMPropertyArray props2(other.getProperties());
	if (props1.size() != props2.size())
	{
		return false;
	}
	std::sort(props1.begin(), props1.end());
	std::sort(props2.begin(), props2.end());
	OW_CIMPropertyArray::iterator i1 = props1.begin();
	OW_CIMPropertyArray::iterator i2 = props2.begin();
	while (i1 != props1.end())
	{
		OW_CIMProperty p1 = *i1;
		OW_CIMProperty p2 = *i2;
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

