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
#include "OW_BinIfcIO.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include "OW_StrictWeakOrdering.hpp"

#include <cstring>
#include <cctype>

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
struct OW_CIMObjectPath::OPData
{
	OW_CIMNameSpace m_nameSpace;
	OW_String m_objectName;
	OW_CIMPropertyArray m_keys;

    OPData* clone() const { return new OPData(*this); }
};

bool operator<(const OW_CIMObjectPath::OPData& x, const OW_CIMObjectPath::OPData& y)
{
	return OW_StrictWeakOrdering(
		x.m_nameSpace, y.m_nameSpace,
		x.m_objectName, y.m_objectName,
		x.m_keys, y.m_keys);
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath() :
	OW_CIMBase(), m_pdata(new OPData)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(OW_CIMNULL_t) :
	OW_CIMBase(), m_pdata(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(const char* oname) :
	OW_CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(const OW_String& oname) :
	OW_CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(const OW_String& oname,
	const OW_String& nspace) :
	OW_CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
	m_pdata->m_nameSpace.setNameSpace(nspace);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(const OW_String& className,
	const OW_CIMPropertyArray& keys) :
	OW_CIMBase(), m_pdata(new OPData)
{
	// If there is a namespace but it will be set via CIMClient on
	// the next call
	m_pdata->m_objectName = className;
	m_pdata->m_keys = keys;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(const OW_String& ns,
	const OW_CIMInstance& inst) :
	OW_CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_nameSpace.setNameSpace(ns);
	m_pdata->m_objectName = inst.getClassName();
	m_pdata->m_keys = inst.getKeyValuePairs();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(const OW_CIMInstance& inst) :
	OW_CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = inst.getClassName();
	m_pdata->m_keys = inst.getKeyValuePairs();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::OW_CIMObjectPath(const OW_CIMObjectPath& arg) :
	OW_CIMBase(), m_pdata(arg.m_pdata)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath::~OW_CIMObjectPath()
{
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMObjectPath::setNull()
{
	m_pdata = NULL;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath&
OW_CIMObjectPath::operator= (const OW_CIMObjectPath& x)
{
	m_pdata = x.m_pdata;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath&
OW_CIMObjectPath::addKey(const OW_String& keyname, const OW_CIMValue& value)
{
	if(value)
	{
		OW_CIMProperty cp(keyname, value);
		if(cp)
		{
			cp.setDataType(value.getCIMDataType());
			m_pdata->m_keys.append(cp);
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMPropertyArray
OW_CIMObjectPath::getKeys() const
{
	return m_pdata->m_keys;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMObjectPath::getKey(const OW_String& keyName) const
{
	for (size_t i = 0; i < m_pdata->m_keys.size(); ++i)
	{
		if (m_pdata->m_keys[i].getName().equalsIgnoreCase(keyName))
		{
			return m_pdata->m_keys[i];
		}
	}
	return OW_CIMProperty(OW_CIMNULL);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMProperty
OW_CIMObjectPath::getKeyT(const OW_String& keyName) const
{
	OW_CIMProperty p = getKey(keyName);
	if (!p)
	{
		OW_THROW(OW_NoSuchPropertyException, keyName.c_str());
	}
	return p;
}
//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath&
OW_CIMObjectPath::setKeys(const OW_CIMPropertyArray& newKeys)
{
	m_pdata->m_keys = newKeys;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath&
OW_CIMObjectPath::setKeys(const OW_CIMInstance& instance)
{
	m_pdata->m_keys = instance.getKeyValuePairs();
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMObjectPath::getNameSpace() const
{
	return m_pdata->m_nameSpace.getNameSpace();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMObjectPath::getHost() const
{
	return m_pdata->m_nameSpace.getHost();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMObjectPath::getObjectName() const
{
	return m_pdata->m_objectName;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath&
OW_CIMObjectPath::setHost(const OW_String& host)
{
	m_pdata->m_nameSpace.setHost(host);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath&
OW_CIMObjectPath::setNameSpace(const OW_String& ns)
{
	m_pdata->m_nameSpace.setNameSpace(ns);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath&
OW_CIMObjectPath::setObjectName(const OW_String& objectName)
{
	m_pdata->m_objectName = objectName;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMObjectPath::equals(const OW_CIMObjectPath& cop,
	OW_Bool /*ignoreClassOrigins*/) const
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
		OW_CIMProperty cp1 = m_pdata->m_keys[i];
		OW_Bool found = false;
		for(int j = 0; j < maxNoKeys; j++)
		{
			OW_CIMProperty cp2 = cop.m_pdata->m_keys[j];
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
OW_CIMNameSpace
OW_CIMObjectPath::getFullNameSpace() const
{
	return m_pdata->m_nameSpace;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMObjectPath::modelPath() const
{
	OW_String lowerObjectName(m_pdata->m_objectName);
	OW_StringBuffer rv(lowerObjectName);

	if(m_pdata->m_keys.size() > 0)
	{
		for(size_t i = 0; i < m_pdata->m_keys.size(); i++)
		{
			OW_CIMProperty cp = m_pdata->m_keys[i];
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
				   : OW_String("null")) + "\"";
		}
	}

	return rv.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMObjectPath::toString() const
{
	OW_StringBuffer rv;
	OW_CIMUrl url = getNameSpaceUrl();

	if(!url.isLocal())
	{
		rv += m_pdata->m_nameSpace.getProtocol();
		if(rv.length() == 0)
		{
			rv += "HTTP";
		}

		rv += "://";
		OW_String str = m_pdata->m_nameSpace.getHost();
		if(str.empty())
		{
			str = "localhost";
		}

		rv += str;
		rv += ":";
		rv += m_pdata->m_nameSpace.getPortNumber();
	}

	rv += '/';

	OW_String strns = m_pdata->m_nameSpace.getNameSpace();

	if(strns.empty())
	{
		strns = "root";
	}

	rv += strns;
	rv += ':';
	rv += modelPath();

	return rv.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_CIMObjectPath::toMOF() const
{
	return escape(toString());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMObjectPath::readObject(istream& istrm)
{
	OW_CIMNameSpace nameSpace(OW_CIMNULL);
	OW_String objectName;
	OW_CIMPropertyArray keys;

	OW_CIMBase::readSig( istrm, OW_CIMOBJECTPATHSIG );
	nameSpace.readObject(istrm);
	objectName.readObject(istrm);
	keys.readObject(istrm);

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
OW_CIMObjectPath::writeObject(ostream& ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMOBJECTPATHSIG );
	m_pdata->m_nameSpace.writeObject(ostrm);
	m_pdata->m_objectName.writeObject(ostrm);
	m_pdata->m_keys.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_CIMObjectPath::parse(const OW_String& instanceNameArg)
{
	OW_String instanceName(instanceNameArg);
	instanceName.trim();
	if(instanceName.empty())
	{
		return OW_CIMObjectPath(OW_CIMNULL);
	}

	OW_String protocol = "HTTP";
	OW_String host = "localhost";
	OW_Int32 port = 5988;

	int ndx = instanceName.indexOf("://");
	if(ndx != -1)
	{
		protocol = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+3);
		ndx = instanceName.indexOf('/');
		if(ndx == -1)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Invalid instance name: ", instanceNameArg).c_str());
		}

		host = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+1);
		ndx = host.indexOf(':');
		if(ndx != -1)
		{
			try
			{
				port = host.substring(ndx+1).toInt32();
			}
			catch (const OW_StringConversionException&)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
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

	OW_String nameSpace = "root";
	ndx = instanceName.indexOf(':');
	if(ndx != -1)
	{
		nameSpace = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+1);
	}

	OW_String className;
	ndx = instanceName.indexOf('.');
	if(ndx == -1)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("class name not found in instance name:: ",
				instanceNameArg).c_str());
	}

	className = instanceName.substring(0, ndx);
	instanceName.erase(0, ndx+1);

	OW_CIMUrl url(protocol, host, OW_String(), port);
	OW_CIMNameSpace ns(url, nameSpace);

	OW_CIMObjectPath op(className);
	op.m_pdata->m_nameSpace = ns;
	//op.m_pdata->m_objectName = className;

	int valuesLen = instanceName.length();

	if(valuesLen == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("No key values found in instance name: ",
				instanceNameArg).c_str());
	}

	// set singleKey to TRUE if it is intended that a property value can be
	// specified without its name when there is only one key.
	bool singleKey = false;

	const char* values = instanceName.c_str();

	OW_String keyvalue;
	bool inquote = false;
	int i, keystart;
	int equalspos = 0;
	OW_CIMPropertyArray tmpkeys;

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
				OW_CIMProperty cp("test it");

				//
				// Generally there will be quotes but for integer values
				// they are not strictly necessary so check for them
				//
				if(values[keystart] != '\"')
				{
					keyvalue = unEscape(OW_String(&values[keystart],
						i-keystart+1));
				}
				else
				{
					keyvalue = unEscape(OW_String(&values[keystart+1],
						i-keystart+1-2));
				}

				cp.setValue(OW_CIMValue(keyvalue));
				cp.setDataType(OW_CIMDataType::STRING);
				tmpkeys.append(cp);
				break;
			}

			if(i+1 == valuesLen)
			{
				i++;
			}

			if(equalspos == 0)
			{
				OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
					format("Bad key in string (%1)", instanceName).c_str());
			}

			OW_String keyprop = OW_String(&values[keystart], equalspos-keystart-1);

			//
			// Generally there will be quotes but for integer values
			// they are not strictly necessary so check for them
			//
			if(values[equalspos] != '\"')
			{
				keyvalue = unEscape(OW_String(&values[equalspos], i-equalspos));
			}
			else
			{
				keyvalue = unEscape(OW_String(&values[equalspos+1],
					i-equalspos-2));
			}

			i++;
			equalspos = 0;
			keystart = i;

			OW_CIMProperty cp(keyprop);
			cp.setValue(OW_CIMValue(keyvalue));
			cp.setDataType(OW_CIMDataType::STRING);
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
OW_CIMUrl
OW_CIMObjectPath::getNameSpaceUrl() const
{
	return m_pdata->m_nameSpace.getHostUrl();
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_String
OW_CIMObjectPath::escape(const OW_String& inString)
{

	int valuesLen = int(inString.length());
	if(valuesLen == 0)
	{
		return inString;
	}

	OW_StringBuffer rv(valuesLen);
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
OW_String
OW_CIMObjectPath::unEscape(const OW_String& inString)
{

	int valuesLen = int(inString.length());
	if(valuesLen == 0)
	{
		return inString;
	}

	OW_StringBuffer rv(valuesLen);
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
bool operator<(const OW_CIMObjectPath& lhs, const OW_CIMObjectPath& rhs)
{
	return *lhs.m_pdata < *rhs.m_pdata;
	//return lhs.toString() < rhs.toString();
}

