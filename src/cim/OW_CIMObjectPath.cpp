/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#include "OW_CIMObjectPath.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Format.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_Array.hpp"
#include "OW_CIMException.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include "OW_StrictWeakOrdering.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMValueCast.hpp"

#include <cstring>
#include <cctype>

namespace OpenWBEM
{

using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
struct CIMObjectPath::OPData
{
	CIMNameSpace m_nameSpace;
	String m_objectName;
	CIMPropertyArray m_keys;
	OPData* clone() const { return new OPData(*this); }
};
bool operator<(const CIMObjectPath::OPData& x, const CIMObjectPath::OPData& y)
{
	return StrictWeakOrdering(
		x.m_nameSpace, y.m_nameSpace,
		x.m_objectName, y.m_objectName,
		x.m_keys, y.m_keys);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath() :
	CIMBase(), m_pdata(new OPData)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(CIMNULL_t) :
	CIMBase(), m_pdata(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const char* oname) :
	CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const String& oname) :
	CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const String& oname,
	const String& nspace) :
	CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
	m_pdata->m_nameSpace.setNameSpace(nspace);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const String& className,
	const CIMPropertyArray& keys) :
	CIMBase(), m_pdata(new OPData)
{
	// If there is a namespace but it will be set via CIMClient on
	// the next call
	m_pdata->m_objectName = className;
	setKeys(keys);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const String& ns,
	const CIMInstance& inst) :
	CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_nameSpace.setNameSpace(ns);
	m_pdata->m_objectName = inst.getClassName();
	setKeys(inst.getKeyValuePairs());
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const CIMObjectPath& arg) :
	CIMBase(), m_pdata(arg.m_pdata)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::~CIMObjectPath()
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMObjectPath::setNull()
{
	m_pdata = NULL;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::operator= (const CIMObjectPath& x)
{
	m_pdata = x.m_pdata;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::addKey(const String& keyname, const CIMValue& value)
{
    if(value)
    {
        CIMProperty cp(keyname, value);
        cp.setDataType(value.getCIMDataType());
        m_pdata->m_keys.append(cp);
    }
    return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::addKey(const CIMProperty& key)
{
    OW_ASSERT(key);
    OW_ASSERT(key.getValue());
	CIMProperty key2(key);
	key2.clearQualifiers();
    m_pdata->m_keys.append(key2);
    return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMPropertyArray
CIMObjectPath::getKeys() const
{
	return m_pdata->m_keys;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMObjectPath::getKey(const String& keyName) const
{
	for (size_t i = 0; i < m_pdata->m_keys.size(); ++i)
	{
		if (m_pdata->m_keys[i].getName().equalsIgnoreCase(keyName))
		{
			return m_pdata->m_keys[i];
		}
	}
	return CIMProperty(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMObjectPath::getKeyT(const String& keyName) const
{
	CIMProperty p = getKey(keyName);
	if (!p)
	{
		OW_THROW(NoSuchPropertyException, keyName.c_str());
	}
	return p;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMObjectPath::getKeyValue(const String& name) const
{
	CIMProperty p = this->getKey(name);
	if (p)
	{
		return p.getValue();
	}
	return CIMValue(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMObjectPath::keyHasValue(const String& name) const
{
	CIMProperty p = this->getKey(name);
	if (p)
	{
		CIMValue v = p.getValue();
		if (v)
			return true;
		else
			return false;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::setKeys(const CIMPropertyArray& newKeys)
{
	m_pdata->m_keys = newKeys;
	for (size_t i = 0; i < m_pdata->m_keys.size(); ++i)
	{
		m_pdata->m_keys[i].clearQualifiers();
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::setKeys(const CIMInstance& instance)
{
	OW_ASSERT(instance);
	setKeys(instance.getKeyValuePairs());
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath& 
CIMObjectPath::setKeyValue(const String& name, const CIMValue& value)
{
	OW_ASSERT(value);
	for (size_t i = 0; i < m_pdata->m_keys.size(); ++i)
	{
		if (m_pdata->m_keys[i].getName().equalsIgnoreCase(name))
		{
			m_pdata->m_keys[i].setValue(value);
			return *this;
		}
	}
	// didn't find it
    m_pdata->m_keys.append(CIMProperty(name, value));
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::getNameSpace() const
{
	return m_pdata->m_nameSpace.getNameSpace();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::getHost() const
{
	return m_pdata->m_nameSpace.getHost();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::getObjectName() const
{
	return m_pdata->m_objectName;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::getClassName() const
{
	return m_pdata->m_objectName;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::setHost(const String& host)
{
	m_pdata->m_nameSpace.setHost(host);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::setNameSpace(const String& ns)
{
	m_pdata->m_nameSpace.setNameSpace(ns);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::setObjectName(const String& className)
{
	m_pdata->m_objectName = className;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::setClassName(const String& className)
{
	m_pdata->m_objectName = className;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMObjectPath::equals(const CIMObjectPath& cop) const
{
	if(m_pdata.isNull() && cop.m_pdata.isNull())
	{
		return true;
	}
	if(!m_pdata->m_nameSpace.getNameSpace().equalsIgnoreCase(
		cop.m_pdata->m_nameSpace.getNameSpace()))
	{
		return false;
	}
	if(!m_pdata->m_objectName.equalsIgnoreCase(cop.m_pdata->m_objectName))
	{
		return false;
	}
	//
	// An instance path
	//
	if(m_pdata->m_keys.size() != cop.m_pdata->m_keys.size())
	{
		return false;
	}
	int maxNoKeys = m_pdata->m_keys.size();
	for(int i = 0; i < maxNoKeys; i++)
	{
		CIMProperty cp1 = m_pdata->m_keys[i];
		bool found = false;
		for(int j = 0; j < maxNoKeys; j++)
		{
			CIMProperty cp2 = cop.m_pdata->m_keys[j];
			if(cp1.getName().equalsIgnoreCase(cp2.getName()))
			{
				if(cp1.getValue().equal(cp2.getValue()))
				{
					found = true;
					break;
				}
			}
		}
		if(!found)
		{
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameSpace
CIMObjectPath::getFullNameSpace() const
{
	return m_pdata->m_nameSpace;
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::modelPath() const
{
	StringBuffer rv(m_pdata->m_objectName);
	if(m_pdata->m_keys.size() > 0)
	{
		for(size_t i = 0; i < m_pdata->m_keys.size(); i++)
		{
			CIMProperty cp = m_pdata->m_keys[i];
			if(i > 0)
			{
				rv += ',';
			}
			else
			{
				rv += '.';
			}
			rv += cp.getName();
			rv += "=\"";
			rv += (cp.getValue()
				   ? escape(cp.getValue().toString())
				   : String("null")) + "\"";
		}
	}
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::toString() const
{
	StringBuffer rv;
	CIMUrl url = getNameSpaceUrl();
	if(!url.isLocal())
	{
		rv += m_pdata->m_nameSpace.getProtocol();
		if(rv.length() == 0)
		{
			rv += "HTTP";
		}
		rv += "://";
		String str = m_pdata->m_nameSpace.getHost();
		if(str.empty())
		{
			str = "localhost";
		}
		rv += str;
		if (m_pdata->m_nameSpace.getPortNumber() != 5988)
		{
			rv += ":";
			rv += m_pdata->m_nameSpace.getPortNumber();
		}
	}
	rv += '/';
	String strns = m_pdata->m_nameSpace.getNameSpace();
//	if(strns.empty())
//	{
//		strns = "root";
//	}
	rv += strns;
	rv += ':';
	rv += modelPath();
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::toMOF() const
{
	return escape(toString());
}
//////////////////////////////////////////////////////////////////////////////
void
CIMObjectPath::readObject(istream& istrm)
{
	CIMNameSpace nameSpace(CIMNULL);
	String objectName;
	CIMPropertyArray keys;
	CIMBase::readSig( istrm, OW_CIMOBJECTPATHSIG );
	nameSpace.readObject(istrm);
	objectName.readObject(istrm);
	BinarySerialization::readArray(istrm, keys);
	if(m_pdata.isNull())
	{
		m_pdata = new OPData;
	}
	m_pdata->m_nameSpace = nameSpace;
	m_pdata->m_objectName = objectName;
	m_pdata->m_keys = keys;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMObjectPath::writeObject(ostream& ostrm) const
{
	CIMBase::writeSig( ostrm, OW_CIMOBJECTPATHSIG );
	m_pdata->m_nameSpace.writeObject(ostrm);
	m_pdata->m_objectName.writeObject(ostrm);
	BinarySerialization::writeArray(ostrm, m_pdata->m_keys);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
CIMObjectPath::parse(const String& instanceNameArg)
{
	String instanceName(instanceNameArg);
	instanceName.trim();
	if(instanceName.empty())
	{
		return CIMObjectPath(CIMNULL);
	}
	String protocol = "HTTP";
	String host = "localhost";
	Int32 port = 5988;
	size_t ndx = instanceName.indexOf("://");
	if(ndx != String::npos)
	{
		protocol = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+3);
		ndx = instanceName.indexOf('/');
		if(ndx == String::npos)
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Invalid instance name: ", instanceNameArg).c_str());
		}
		host = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+1);
		ndx = host.indexOf(':');
		if(ndx != String::npos)
		{
			try
			{
				port = host.substring(ndx+1).toInt32();
			}
			catch (const StringConversionException&)
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					"Invalid port in Object Path");
			}
			host = host.substring(0, ndx);
		}
	}
	else
	{
		if(instanceName.charAt(0) == '/')
		{
			instanceName.erase(0, 1);
		}
	}
	String nameSpace = "root";
	ndx = instanceName.indexOf(':');
	if(ndx != String::npos)
	{
		nameSpace = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+1);
	}
	String className;
	ndx = instanceName.indexOf('.');
	if(ndx == String::npos)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("class name not found in instance name:: ",
				instanceNameArg).c_str());
	}
	className = instanceName.substring(0, ndx);
	instanceName.erase(0, ndx+1);
	CIMUrl url(protocol, host, String(), port);
	CIMNameSpace ns(url, nameSpace);
	CIMObjectPath op(className);
	op.m_pdata->m_nameSpace = ns;
	//op.m_pdata->m_objectName = className;
	int valuesLen = instanceName.length();
	if(valuesLen == 0)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("No key values found in instance name: ",
				instanceNameArg).c_str());
	}
	// set singleKey to TRUE if it is intended that a property value can be
	// specified without its name when there is only one key.
	bool singleKey = false;
	const char* values = instanceName.c_str();
	String keyvalue;
	bool inquote = false;
	int i, keystart;
	int equalspos = 0;
	CIMPropertyArray tmpkeys;
	//
	// Now extract keys
	//
	for(i = 0, keystart = 0; i < valuesLen; i++)
	{
		char ch = values[i];
		// Skip escaped characters
		if(i < valuesLen-1 && ch == '\\')
		{
			i++;
			continue;
		}
		// Check for quotes
		if(ch == '\"')
		{
			inquote = !inquote;
		}
		if(inquote)
		{
			continue;
		}
		if(ch == '=')
		{
			equalspos = i+1;
		}
		if(ch == ',' || (i+1 == valuesLen))
		{
			if((i+1 == valuesLen) && equalspos == 0 && singleKey)
			{
				// This is the special case of when its classname.value
				CIMProperty cp("test it");
				//
				// Generally there will be quotes but for integer values
				// they are not strictly necessary so check for them
				//
				if(values[keystart] != '\"')
				{
					keyvalue = unEscape(String(&values[keystart],
						i-keystart+1));
				}
				else
				{
					keyvalue = unEscape(String(&values[keystart+1],
						i-keystart+1-2));
				}
				cp.setValue(CIMValue(keyvalue));
				cp.setDataType(CIMDataType::STRING);
				tmpkeys.append(cp);
				break;
			}
			if(i+1 == valuesLen)
			{
				i++;
			}
			if(equalspos == 0)
			{
				OW_THROWCIMMSG(CIMException::NOT_FOUND,
					Format("Bad key in string (%1)", instanceName).c_str());
			}
			String keyprop = String(&values[keystart], equalspos-keystart-1);
			//
			// Generally there will be quotes but for integer values
			// they are not strictly necessary so check for them
			//
			if(values[equalspos] != '\"')
			{
				keyvalue = unEscape(String(&values[equalspos], i-equalspos));
			}
			else
			{
				keyvalue = unEscape(String(&values[equalspos+1],
					i-equalspos-2));
			}
			i++;
			equalspos = 0;
			keystart = i;
			CIMProperty cp(keyprop);
			cp.setValue(CIMValue(keyvalue));
			cp.setDataType(CIMDataType::STRING);
			tmpkeys.append(cp);
			singleKey = false;
		}
	}
	//
	// Now assign the values to this instance
	//
	op.setKeys(tmpkeys);
	return op;
}
//////////////////////////////////////////////////////////////////////////////
CIMUrl
CIMObjectPath::getNameSpaceUrl() const
{
	return m_pdata->m_nameSpace.getHostUrl();
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
String
CIMObjectPath::escape(const String& inString)
{
	int valuesLen = int(inString.length());
	if(valuesLen == 0)
	{
		return inString;
	}
	StringBuffer rv(valuesLen);
	const char* values = inString.c_str();
	for(int i = 0; i < valuesLen; i++)
	{
		char ch = values[i];
		if(ch == '\\')
		{
			rv += '\\';
			rv += ch;
		}
		else if(ch == '"')
		{
			rv += '\\';
			rv += '"';
		}
		else if(ch == '\n')
		{
			rv += "\\n";
		}
		else
		{
			rv += ch;
		}
	}
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
String
CIMObjectPath::unEscape(const String& inString)
{
	int valuesLen = int(inString.length());
	if(valuesLen == 0)
	{
		return inString;
	}
	StringBuffer rv(valuesLen);
	const char* values = inString.c_str();
	for(int i = 0; i < valuesLen; i++)
	{
		char ch = values[i];
		if(ch == '\\')
		{
			if(i+1 < valuesLen)
			{
				i++;
				rv += values[i];
			}
		}
		else
		{
			rv += values[i];
		}
	}
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMObjectPath& lhs, const CIMObjectPath& rhs)
{
	return *lhs.m_pdata < *rhs.m_pdata;
	//return lhs.toString() < rhs.toString();
}
//////////////////////////////////////////////////////////////////////////////
bool CIMObjectPath::isClassPath() const
{
	return getKeys().size() == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool CIMObjectPath::isInstancePath() const
{
	return getKeys().size() != 0;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::syncWithClass(const CIMClass& theClass)
{
	if(!theClass || isClassPath())
	{
		return *this;
	}
	String propName;
	CIMPropertyArray classProps = theClass.getKeys();
	CIMPropertyArray copProps = getKeys();
	// Remove properties that are not defined in the class
	for(size_t i = 0; i < copProps.size(); i++)
	{
		propName = copProps[i].getName();
		if(!theClass.getProperty(propName))
		{
			copProps.remove(i);
			i--;
		}
	}
	// Ensure existing properties have the right type
	for(size_t i = 0; i < classProps.size(); i++)
	{
		bool found = false;
		const CIMProperty& cprop = classProps[i];
		propName = cprop.getName();
		for(size_t j = 0; j < copProps.size(); j++)
		{
			CIMProperty iprop = copProps[j];
			if(iprop.getName().equalsIgnoreCase(propName))
			{
				CIMValue cv = iprop.getValue();
				iprop = cprop;
				if(cv)
				{
					if(cv.getType() != iprop.getDataType().getType())
					{
						cv = CIMValueCast::castValueToDataType(cv,
							iprop.getDataType());
					}
					iprop.setValue(cv);
				}
				copProps[j] = iprop;
				found = true;
				break;
			}
		}
		if(!found)
		{
			copProps.append(classProps[i]);
		}
	}
	setKeys(copProps);
	return *this;
}

} // end namespace OpenWBEM

