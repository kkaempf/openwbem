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
#include "OW_CIMClass.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_Assertion.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMValue.hpp"
#include "OW_IOException.hpp"

#include <algorithm>
#include <iostream>

using std::ostream;
using std::istream;

struct OW_CIMClass::CLSData
{
	CLSData() :
		m_name(), m_parentClassName(), m_qualifiers(), m_properties(),
		m_methods(), m_associationFlag(false), m_isKeyed(false){  }

	CLSData(const CLSData& x) :
		m_name(x.m_name), m_parentClassName(x.m_parentClassName),
		m_qualifiers(x.m_qualifiers), m_properties(x.m_properties),
		m_methods(x.m_methods), m_associationFlag(x.m_associationFlag),
		m_isKeyed(x.m_isKeyed){  }

	CLSData& operator= (const CLSData& x)
	{
		m_name = x.m_name;
		m_parentClassName = x.m_parentClassName;
		m_qualifiers = x.m_qualifiers;
		m_properties = x.m_properties;
		m_methods = x.m_methods;
		m_associationFlag = x.m_associationFlag;
		m_isKeyed = x.m_isKeyed;
		return *this;
	}

	OW_String m_name;
	OW_String m_parentClassName;
	OW_CIMQualifierArray m_qualifiers;
	OW_CIMPropertyArray m_properties;
	OW_CIMMethodArray m_methods;
	OW_Bool m_associationFlag;
	OW_Bool m_isKeyed;
};

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass::OW_CIMClass(OW_Bool notNull) :
	OW_CIMElement(), m_pdata((notNull) ? new CLSData : NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass::OW_CIMClass(const char* name) :
	OW_CIMElement(), m_pdata(new CLSData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass::OW_CIMClass(const OW_String& name) :
	OW_CIMElement(), m_pdata(new CLSData)
{
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
/*
OW_CIMClass::OW_CIMClass(const OW_XMLNode& node)
	: OW_CIMElement(), m_pdata(new CLSData)
{
	OW_ASSERT(node);
	OW_String superClassName;
	OW_String inClassName;

	OW_XMLNode valueNode =
		node.mustFindElement(OW_XMLNode::XML_ELEMENT_CLASS);

	inClassName = valueNode.mustGetAttribute(OW_XMLParameters::paramName);
	m_pdata->m_name = inClassName;

	superClassName = valueNode.getAttribute(OW_XMLParameters::paramSuperName);
	if(superClassName.length() > 0)
	{
		m_pdata->m_parentClassName = superClassName;
	}

	//
	// Find qualifier information
	//
	for(valueNode = valueNode.getChild();
		 valueNode != 0
		 && valueNode.getToken() == OW_XMLNode::XML_ELEMENT_QUALIFIER;
		 valueNode = valueNode.getNext())
	{
		OW_CIMQualifier cq = OW_CIMQualifier(valueNode);
		if(cq.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_ASSOCIATION))
		{
			m_pdata->m_associationFlag = true;
		}

		m_pdata->m_qualifiers.append(cq);
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
			OW_CIMProperty cp(valueNode);
			m_pdata->m_properties.append(cp);
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
		m_pdata->m_methods.append(OW_CIMMethod(valueNode));
	}
}
*/
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setName(const OW_String& name)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMClass::getSuperClass() const
{
	return m_pdata->m_parentClassName;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setSuperClass(const OW_String& pname)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_parentClassName = pname;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMClass::isKeyed() const
{
	return m_pdata->m_isKeyed;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setIsKeyed(OW_Bool isKeyedParm)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_isKeyed = isKeyedParm;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMPropertyArray
OW_CIMClass::getKeys() const
{
	OW_CIMPropertyArray v;

	for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
	{
		OW_CIMProperty p = m_pdata->m_properties[i];
		if(p.isKey())
		{
			v.append(p);
		}
	}

	return v;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifier
OW_CIMClass::getQualifier(const OW_String& name) const
{
	for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		OW_CIMQualifier q = m_pdata->m_qualifiers[i];
		if(q.getName().equalsIgnoreCase(name))
		{
			return q;
		}
	}

	return OW_CIMQualifier();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMClass::getProperty(const OW_String& prpName) const
{
	OW_String oClass;
	OW_String propName(prpName);

	//
	// See if property name is in the form originClass.propName
	// and if it is, work to find real origin class
	//
	oClass = splitName1(propName);
	if(oClass.length() > 0)
	{
		propName = splitName2(propName);
		for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			OW_CIMProperty cp = m_pdata->m_properties[i];
			if(!cp.getOriginClass().equalsIgnoreCase(oClass)
				&& cp.getName().equalsIgnoreCase(propName))
			{
				return cp;
			}
		}
	}
	else
	{
		for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			OW_CIMProperty cp = m_pdata->m_properties[i];
			if(cp.getName().equalsIgnoreCase(propName))
			{
				return(cp);
			}
		}
	}

	return OW_CIMProperty();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMClass::getProperty(const OW_String& name,
	const OW_String& originClass) const
{
	return getProperty(originClass + "." + name);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMMethod
OW_CIMClass::getMethod(const OW_String& name) const
{
	OW_String oClass;

	//
	// See if method name is in the form originClass.propName
	// and if it is, work to find real origin class
	//
	oClass = splitName1(name);

	if(oClass.length() > 0)
	{
		int tsize = m_pdata->m_methods.size();
		for(int i = 0; i < tsize; i++)
		{
			OW_CIMMethod q = m_pdata->m_methods[i];
			if(!q.getOriginClass().equalsIgnoreCase(oClass)
				&& (q.getName().equalsIgnoreCase(name)))
			{
				return(q);
			}
		}
	}
	else
	{
		int tsize = m_pdata->m_methods.size();
		for(int i = 0; i < tsize; i++)
		{
			OW_CIMMethod q = m_pdata->m_methods[i];
			if(q.getName().equalsIgnoreCase(name))
			{
				return(q);
			}
		}
	}

	return OW_CIMMethod();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMMethod
OW_CIMClass::getMethod(const OW_String& name,
	const OW_String& originClass) const
{
	return getMethod(originClass + "." + name);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMClass::isAssociation() const
{
	return m_pdata->m_associationFlag;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setIsAssociation(OW_Bool isAssocFlag)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_associationFlag = isAssocFlag;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierArray
OW_CIMClass::getQualifiers() const
{
	return m_pdata->m_qualifiers;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMPropertyArray
OW_CIMClass::getAllProperties() const
{
	return m_pdata->m_properties;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMClass::getKeyClass() const
{
	for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
	{
		OW_CIMProperty p = m_pdata->m_properties[i];
		if(p.isKey())
		{
			return p.getOriginClass();
		}
	}

	return OW_String();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMPropertyArray
OW_CIMClass::getProperties() const
{
	OW_CIMPropertyArray prop;

	for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
	{
		OW_CIMProperty cp = m_pdata->m_properties[i];
		if(cp.getQualifier(OW_CIMQualifier::CIM_QUAL_OVERRIDE))
		{
			prop.append(cp);
		}
	}

	return(prop);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMMethodArray
OW_CIMClass::getAllMethods() const
{
	return m_pdata->m_methods;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMMethodArray
OW_CIMClass::getMethods() const
{
	OW_CIMMethodArray meth;

	int tsize = m_pdata->m_methods.size();
	for(int i = 0; i < tsize; i++)
	{
		OW_CIMMethod cm = m_pdata->m_methods[i];
		if(cm.getQualifier(OW_CIMQualifier::CIM_QUAL_OVERRIDE))
		{
			meth.append(cm);
		}
	}

	return meth;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::addProperty(const OW_CIMProperty& prop)
{
	if(prop)
	{
		OW_MutexLock l = m_pdata.getWriteLock();
		m_pdata->m_properties.append(prop);
		if(prop.isKey())
		{
			m_pdata->m_isKeyed = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CIMClass::numberOfProperties() const
{
	return m_pdata->m_properties.size();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setProperties(const OW_CIMPropertyArray& props)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_properties = props;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setProperty(const OW_CIMProperty& prop)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	OW_String argName = prop.getName();
	for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
	{
		if(argName.equalsIgnoreCase(m_pdata->m_properties[i].getName()))
		{
			m_pdata->m_properties[i] = prop;
			return;
		}
	}

	m_pdata->m_properties.append(prop);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setMethod(const OW_CIMMethod& meth)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	OW_String argName = meth.getName();
	for(size_t i = 0; i < m_pdata->m_methods.size(); i++)
	{
		if(argName.equalsIgnoreCase(m_pdata->m_methods[i].getName()))
		{
			m_pdata->m_methods[i] = meth;
			return;
		}
	}

	m_pdata->m_methods.append(meth);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::addQualifier(const OW_CIMQualifier& qual)
{
	if(!qual)
	{
		return;
	}

	OW_MutexLock l = m_pdata.getWriteLock();

	//
	// See if qualifier already present
	//
	for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		OW_CIMQualifier cq = m_pdata->m_qualifiers[i];
		if(cq.equals(qual))
		{
			m_pdata->m_qualifiers.remove(i);
			break;
		}
	}

	if(qual.getName().equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_ASSOCIATION))
	{
		m_pdata->m_associationFlag = true;
	}
	m_pdata->m_qualifiers.append(qual);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMClass::hasQualifier(const OW_CIMQualifier& qual) const
{
	if(qual)
	{
		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			if(m_pdata->m_qualifiers[i].equals(qual))
			{
				return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CIMClass::numberOfQualifiers() const
{
	return m_pdata->m_qualifiers.size();
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMClass::removeQualifier(const OW_CIMQualifier& qual)
{
	OW_Bool cc = false;

	if(qual)
	{
		OW_MutexLock l = m_pdata.getWriteLock();

		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			OW_CIMQualifier cq = m_pdata->m_qualifiers[i];
			if(cq.equals(qual))
			{
				m_pdata->m_qualifiers.remove(i);
				cc = true;
				break;
			}
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMClass::removeQualifier(const OW_String& name)
{
	OW_Bool cc = false;
	OW_MutexLock l = m_pdata.getWriteLock();

	for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
	{
		OW_CIMQualifier cq = m_pdata->m_qualifiers[i];
		if(cq.getName().equalsIgnoreCase(name))
		{
			m_pdata->m_qualifiers.remove(i);
			cc = true;
			break;
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMClass::removeProperty(const OW_String& name)
{
	OW_Bool cc = false;
	OW_MutexLock l = m_pdata.getWriteLock();

	for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
	{
		OW_CIMProperty prop = m_pdata->m_properties[i];
		if(prop.getName().equalsIgnoreCase(name))
		{
			m_pdata->m_properties.remove(i);
			cc = true;
			break;
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setQualifiers(const OW_CIMQualifierArray& quals)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_qualifiers = quals;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setQualifier(const OW_CIMQualifier& qual)
{
	if(qual)
	{
		OW_MutexLock l = m_pdata.getWriteLock();
		OW_Bool found = false;

		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			OW_CIMQualifier cq = m_pdata->m_qualifiers[i];
			if(cq.equals(qual))
			{
				m_pdata->m_qualifiers[i] = qual;
				found = true;
				break;
			}
		}

		if(!found)
		{
			m_pdata->m_qualifiers.append(qual);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::addMethod(const OW_CIMMethod& meth)
{
	if(meth)
	{
		OW_MutexLock l = m_pdata.getWriteLock();
		m_pdata->m_methods.append(meth);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setMethods(const OW_CIMMethodArray& meths)
{
	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_methods = meths;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMClass::newInstance() const
{
	OW_CIMInstance cInstance(OW_Bool(true));
	cInstance.syncWithClass(*this, true);
	cInstance.setClassName(m_pdata->m_name);
	return cInstance;
}

//////////////////////////////////////////////////////////////////////////////
/*
void
OW_CIMClass::toXML(ostream& ostr, OW_Bool includeQualifiers) const
{
	toXML(ostr, false, includeQualifiers, true, OW_StringArray());
}
*/

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMClass::filterProperties(const OW_StringArray& propertyList,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin) const
{
	return clone(false, includeQualifiers, includeClassOrigin, propertyList,
		false);
}

//////////////////////////////////////////////////////////////////////////////
/*
void
OW_CIMClass::toXML(ostream& ostr, OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray& propertyList,
	OW_Bool noProps) const
{
	if(m_pdata->m_name.length() == 0)
	{
		OW_THROW(OW_CIMMalformedUrlException, "class must have name");
	}

	ostr << "<CLASS NAME=\"";
	ostr << m_pdata->m_name;
	if(m_pdata->m_parentClassName.length() != 0)
	{
		ostr << "\" SUPERCLASS=\"";
		ostr << m_pdata->m_parentClassName;
	}
	ostr << "\">";

	//
	// Process qualifiers
	//
	if(includeQualifiers)
	{
		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			m_pdata->m_qualifiers[i].toXML(ostr, localOnly);
		}
	}

	if(!noProps)
	{
		for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			OW_CIMProperty prop = m_pdata->m_properties[i];

			// If the given property list has any elements, then ensure this
			// property name is in the property list before including it's xml
			if(propertyList.size() > 0)
			{
				OW_String pName = prop.getName();
				for(size_t j = 0; j < propertyList.size(); j++)
				{
					if(pName.equalsIgnoreCase(propertyList[j]))
					{
						prop.toXML(ostr, localOnly, includeQualifiers,
							includeClassOrigin);
						break;
					}
				}
			}
			else
			{
				prop.toXML(ostr, localOnly, includeQualifiers,
					includeClassOrigin);
			}
		}
	}

	// Process methods
	for(size_t i = 0; i < m_pdata->m_methods.size(); i++)
	{
		m_pdata->m_methods[i].toXML(ostr, localOnly, includeQualifiers,
			includeClassOrigin);
	}

	ostr << "</CLASS>";
}
*/

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMClass::clone(OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray& propertyList,
	OW_Bool noProps) const
{
	if(m_pdata.isNull())
	{
		return OW_CIMClass();
	}

	OW_CIMClass theClass(true);
	theClass.m_pdata->m_name = m_pdata->m_name;
	theClass.m_pdata->m_parentClassName = m_pdata->m_parentClassName;
	theClass.m_pdata->m_associationFlag = m_pdata->m_associationFlag;
	theClass.m_pdata->m_isKeyed = m_pdata->m_isKeyed;

	//
	// Process qualifiers
	//
	if(includeQualifiers)
	{
		OW_CIMQualifierArray qra;
		for(size_t i = 0; i < m_pdata->m_qualifiers.size(); i++)
		{
			OW_CIMQualifier cq = m_pdata->m_qualifiers[i];
			if(localOnly && cq.getPropagated() == true)
			{
				continue;
			}
			qra.append(cq);
		}
		theClass.m_pdata->m_qualifiers = qra;
	}

	if(!noProps)
	{
		OW_CIMPropertyArray pra;
		for(size_t i = 0; i < m_pdata->m_properties.size(); i++)
		{
			OW_CIMProperty prop = m_pdata->m_properties[i];

			if(localOnly && prop.getPropagated() == true)
			{
				continue;
			}

			// If the given property list has any elements, then ensure this
			// property name is in the property list before including it
			if(propertyList.size() > 0)
			{
				OW_String pName = prop.getName();
				for(size_t j = 0; j < propertyList.size(); j++)
				{
					if(pName.equalsIgnoreCase(propertyList[j]))
					{
						pra.append(prop.clone(includeQualifiers,
							includeClassOrigin));
						break;
					}
				}
			}
			else
			{
				pra.append(prop.clone(includeQualifiers,
					includeClassOrigin));
			}
		}
		theClass.m_pdata->m_properties = pra;
	}

	// Process methods
	OW_CIMMethodArray mra;
	for(size_t i = 0; i < m_pdata->m_methods.size(); i++)
	{
		OW_CIMMethod meth = m_pdata->m_methods[i];
		if(localOnly && meth.getPropagated() == true)
		{
			continue;
		}

		mra.append(meth.clone(includeQualifiers, includeClassOrigin));
	}

	theClass.m_pdata->m_methods = mra;

	return theClass;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::readObject(istream &istrm)
{
	int len;
	OW_String name;
	OW_String pcName;
	OW_CIMQualifierArray qra;
	OW_CIMPropertyArray pra;
	OW_CIMMethodArray mra;
	OW_Bool isAssocFlag;
	OW_Bool isK;

	OW_CIMBase::readSig( istrm, OW_CIMCLASSSIG );

	// Read name of class
	name.readObject(istrm);

	// Read name of parent class
	pcName.readObject(istrm);

	// Read association flag
	isAssocFlag.readObject(istrm);

	// Read is key flag
	isK.readObject(istrm);

	// Read len of qualifier array
	if(!istrm.read((char*)&len, sizeof(len)))
		OW_THROW(OW_IOException, "Failed to read size of qualifier array");

	len = OW_ntoh32(len);

	// Read qualifier array
	for(int i = 0; i < len; i++)
	{
		OW_CIMQualifier qual;
		qual.readObject(istrm);
		qra.append(qual);
	}

	// Read len of properties array
	if(!istrm.read((char*)&len, sizeof(len)))
		OW_THROW(OW_IOException, "Failed to read size of properties array");

	len = OW_ntoh32(len);

	// Read properties array
	for(int i = 0; i < len; i++)
	{
		OW_CIMProperty prop;
		prop.readObject(istrm);
		pra.append(prop);
	}

	// Read len of method array
	if(!istrm.read((char*)&len, sizeof(len)))
		OW_THROW(OW_IOException, "Failed to read size of method array");

	len = OW_ntoh32(len);

	// Read properties array
	for(int i = 0; i < len; i++)
	{
		OW_CIMMethod meth;
		meth.readObject(istrm);
		mra.append(meth);
	}

	if(m_pdata.isNull())
	{
		m_pdata = new CLSData;
	}

	OW_MutexLock l = m_pdata.getWriteLock();
	m_pdata->m_name = name;
	m_pdata->m_parentClassName = pcName;
	m_pdata->m_associationFlag = isAssocFlag;
	m_pdata->m_isKeyed = isK;
	m_pdata->m_qualifiers = qra;
	m_pdata->m_properties = pra;
	m_pdata->m_methods = mra;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMCLASSSIG );
	m_pdata->m_name.writeObject(ostrm);
	m_pdata->m_parentClassName.writeObject(ostrm);
	m_pdata->m_associationFlag.writeObject(ostrm);
	m_pdata->m_isKeyed.writeObject(ostrm);

	// Write len of quailifer array
	int len = m_pdata->m_qualifiers.size();
	int nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
	{
		OW_THROW(OW_IOException, "Failed to write size of qualifier array");
	}

	// Write qualifier array
	for(int i = 0; i < len; i++)
	{
		m_pdata->m_qualifiers[i].writeObject(ostrm);
	}

	// Write len of properties array
	len = m_pdata->m_properties.size();
	nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
	{
		OW_THROW(OW_IOException, "Failed to write size of properties array");
	}

	// Write properties array
	for(int i = 0; i < len; i++)
	{
		m_pdata->m_properties[i].writeObject(ostrm);
	}

	// Write len of method array
	len = m_pdata->m_methods.size();
	nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
	{
		OW_THROW(OW_IOException, "Failed to write size of methods array");
	}

	// Write method array
	for(int i = 0; i < len; i++)
	{
		m_pdata->m_methods[i].writeObject(ostrm);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMClass::toMOF() const
{
	size_t i;
	OW_StringBuffer rv;

	if(m_pdata->m_qualifiers.size() != 0)
	{
		rv += "[\n";

		OW_CIMQualifierArray qra = m_pdata->m_qualifiers;

		/*
		 * The association needs to be at the beginning according to 
		 * the MOF grammar.
		 */

		OW_CIMQualifierArray::iterator iter = std::find(
			qra.begin(), qra.end(), 
			OW_CIMQualifier(OW_CIMQualifier::CIM_QUAL_ASSOCIATION));
		if (iter != qra.end())
		{
			if (iter != qra.begin())
			{
				OW_CIMQualifier tmp = *iter;
				qra.erase(iter);
				qra.insert(qra.begin(), tmp);
			}
		}
		else
		{
			if(m_pdata->m_associationFlag)
			{
				OW_CIMQualifierType cqt(OW_CIMQualifier::CIM_QUAL_ASSOCIATION);
				cqt.setDataType(OW_CIMDataType(OW_CIMDataType::BOOLEAN));
				OW_CIMQualifier cq(OW_CIMQualifier::CIM_QUAL_ASSOCIATION, cqt);
				cq.setValue(OW_CIMValue(OW_Bool(true)));
				qra.insert(qra.begin(), cq);
			}
		}

		iter = std::find( qra.begin(), qra.end(), 
			OW_CIMQualifier(OW_CIMQualifier::CIM_QUAL_INDICATION));
		if (iter != qra.end())
		{
			std::swap(*iter, *qra.begin());
		}

		for(i = 0; i < qra.size(); i++)
		{
			if(i > 0)
			{
				rv += ',';
			}

			rv += qra[i].toMOF();
		}

		rv += "]\n";
	}

	rv += "class ";
	rv += getName();

	if(m_pdata->m_parentClassName.length() > 0)
	{
		rv += ':';
		rv += m_pdata->m_parentClassName;
	}

	rv += "\n{\n";

	for(i = 0; i < m_pdata->m_properties.size(); i++)
	{
		rv += m_pdata->m_properties[i].toMOF();
	}

	for(i = 0; i < m_pdata->m_methods.size(); i++)
	{
		rv += m_pdata->m_methods[i].toMOF();
	}

	rv += "};\n";
	return rv.toString();
}

//////////////////////////////////////////////////////////////////////////////
/*
void
OW_CIMClass::toXML(ostream& ostr) const
{
	toXML(ostr, false, true, true, OW_StringArray());
}
*/

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMClass::toString() const
{
	return toMOF();
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_String
OW_CIMClass::splitName1(const OW_String& inName)
{
	int i = inName.indexOf('.');
	return (i != -1) ? inName.substring(0, i) : OW_String();
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_String
OW_CIMClass::splitName2(const OW_String& inName)
{
	int i = inName.indexOf('.');
	return (i != -1) ? inName.substring(i+1) : inName;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass::~OW_CIMClass()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass::OW_CIMClass(const OW_CIMClass& x)
	: OW_CIMElement(), m_pdata(x.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMClass::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass&
OW_CIMClass::operator= (const OW_CIMClass& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMClass::getName() const
{
	return m_pdata->m_name;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass::operator void*() const
{
	return (void*)(!m_pdata.isNull());
}

//////////////////////////////////////////////////////////////////////////////
const char* const OW_CIMClass::NAMESPACECLASS = "__Namespace";
