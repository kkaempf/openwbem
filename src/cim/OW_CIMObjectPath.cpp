/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_CIMObjectPath.hpp"
#include "blocxx/AutoPtr.hpp"
#include "blocxx/Format.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMUrl.hpp"
#include "blocxx/Array.hpp"
#include "OW_CIMException.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_NoSuchPropertyException.hpp"
#include "blocxx/StrictWeakOrdering.hpp"
#include "blocxx/Assertion.hpp"
#include "OW_CIMValueCast.hpp"
#include "blocxx/COWIntrusiveCountableBase.hpp"

#include <cstring>
#include <cctype>
#include <climits>

namespace OW_NAMESPACE
{

using std::streambuf;
using namespace blocxx;
//////////////////////////////////////////////////////////////////////////////
struct CIMObjectPath::OPData : public COWIntrusiveCountableBase
{
	virtual ~OPData();
	CIMNameSpace m_nameSpace;
	CIMName m_objectName;
	CIMPropertyArray m_keys;
	OPData* clone() const { return new OPData(*this); }
};
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::OPData::~OPData()
{
}
//////////////////////////////////////////////////////////////////////////////
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
CIMObjectPath::CIMObjectPath(const CIMName& oname) :
	CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const CIMName& oname,
	const String& nspace) :
	CIMBase(), m_pdata(new OPData)
{
	m_pdata->m_objectName = oname;
	m_pdata->m_nameSpace.setNameSpace(nspace);
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath::CIMObjectPath(const CIMName& className,
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
CIMPropertyArray
CIMObjectPath::getKeys() const
{
	return m_pdata->m_keys;
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMObjectPath::getKey(const CIMName& keyName) const
{
	for (size_t i = 0; i < m_pdata->m_keys.size(); ++i)
	{
		if (m_pdata->m_keys[i].getName() == keyName)
		{
			return m_pdata->m_keys[i];
		}
	}
	return CIMProperty(CIMNULL);
}
//////////////////////////////////////////////////////////////////////////////
CIMProperty
CIMObjectPath::getKeyT(const CIMName& keyName) const
{
	CIMProperty p = getKey(keyName);
	if (!p)
	{
		OW_THROW(NoSuchPropertyException, keyName.toString().c_str());
	}
	return p;
}
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMObjectPath::getKeyValue(const CIMName& name) const
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
CIMObjectPath::keyHasValue(const CIMName& name) const
{
	CIMProperty p = this->getKey(name);
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
	BLOCXX_ASSERT(instance);
	setKeys(instance.getKeyValuePairs());
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath&
CIMObjectPath::setKeyValue(const CIMName& name, const CIMValue& value)
{
	BLOCXX_ASSERT(value);
	for (size_t i = 0; i < m_pdata->m_keys.size(); ++i)
	{
		if (m_pdata->m_keys[i].getName() == name)
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
CIMObjectPath::getClassName() const
{
	return m_pdata->m_objectName.toString();
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
CIMObjectPath::setClassName(const CIMName& className)
{
	m_pdata->m_objectName = className;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
CIMObjectPath::equals(const CIMObjectPath& cop) const
{
	if (!m_pdata && !cop.m_pdata)
	{
		return true;
	}
	if (!m_pdata->m_nameSpace.getNameSpace().equalsIgnoreCase(
		cop.m_pdata->m_nameSpace.getNameSpace()))
	{
		return false;
	}
	if (m_pdata->m_objectName != cop.m_pdata->m_objectName)
	{
		return false;
	}
	//
	// An instance path
	//
	if (m_pdata->m_keys.size() != cop.m_pdata->m_keys.size())
	{
		return false;
	}
	int maxNoKeys = m_pdata->m_keys.size();
	for (int i = 0; i < maxNoKeys; i++)
	{
		CIMProperty cp1 = m_pdata->m_keys[i];
		bool found = false;
		for (int j = 0; j < maxNoKeys; j++)
		{
			CIMProperty cp2 = cop.m_pdata->m_keys[j];
			if (cp1.getName().equalsIgnoreCase(cp2.getName()))
			{
				if (cp1.getValue().equal(cp2.getValue()))
				{
					found = true;
					break;
				}
			}
		}
		if (!found)
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
	StringBuffer rv(m_pdata->m_objectName.toString());
	for (size_t i = 0; i < m_pdata->m_keys.size(); i++)
	{
		CIMProperty cp = m_pdata->m_keys[i];
		if (i > 0)
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
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
CIMObjectPath::toString() const
{
	StringBuffer rv;
	CIMUrl url = getNameSpaceUrl();
	if (!url.isLocal())
	{
		rv += m_pdata->m_nameSpace.getProtocol();
		if (rv.length() == 0)
		{
			rv += "HTTP";
		}
		rv += "://";
		String str = m_pdata->m_nameSpace.getHost();
		if (str.empty())
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
//	if (strns.empty())
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
	StringBuffer rv;
	rv = "\"";
	rv += escape(toString());
	rv += "\"";
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMObjectPath::readObject(streambuf & istrm)
{
	CIMNameSpace nameSpace(CIMNULL);
	CIMName objectName;
	CIMPropertyArray keys;
	CIMBase::readSig( istrm, OW_CIMOBJECTPATHSIG );
	nameSpace.readObject(istrm);
	objectName.readObject(istrm);
	BinarySerialization::readArray(istrm, keys);
	if (!m_pdata)
	{
		m_pdata = new OPData;
	}
	m_pdata->m_nameSpace = nameSpace;
	m_pdata->m_objectName = objectName;
	m_pdata->m_keys = keys;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMObjectPath::writeObject(streambuf & ostrm) const
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
	if (instanceName.empty())
	{
		return CIMObjectPath(CIMNULL);
	}
	String protocol = "HTTP";
	String host = "localhost";
	Int32 port = 5988;
	size_t ndx = instanceName.indexOf("://");
	if (ndx != String::npos)
	{
		protocol = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+3);
		ndx = instanceName.indexOf('/');
		if (ndx == String::npos)
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Invalid instance name: %1", instanceNameArg).c_str());
		}
		host = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+1);
		ndx = host.indexOf(':');
		if (ndx != String::npos)
		{
			try
			{
				port = host.substring(ndx+1).toInt32();
			}
			catch (const StringConversionException&)
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					Format("Invalid port (%1) in Object Path", host.substring(ndx+1)).c_str());
			}
			host = host.substring(0, ndx);
		}
	}
	else
	{
		if (instanceName.charAt(0) == '/')
		{
			instanceName.erase(0, 1);
		}
	}
	String nameSpace = "root";
	ndx = instanceName.indexOf(':');
	if (ndx != String::npos)
	{
		nameSpace = instanceName.substring(0, ndx);
		instanceName.erase(0, ndx+1);
	}
	CIMName className;
	ndx = instanceName.indexOf('.');
	if (ndx == String::npos)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("class name not found in instance name: %1",
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
	if (valuesLen == 0)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("No key values found in instance name: %1",
				instanceNameArg).c_str());
	}
	// set singleKey to TRUE if it is intended that a property value can be
	// specified without its name when there is only one key.
	bool singleKey = false;
	const char* values = instanceName.c_str();
	String keyvalue;
	bool inquote = false;
	int equalspos = 0;
	CIMPropertyArray tmpkeys;

	//
	// Now extract keys
	//
	int i = 0;
	int keystart = 0;
	while (i < valuesLen)
	{
		char ch = values[i];
		// Skip escaped characters
		if (i < valuesLen-1 && ch == '\\')
		{
			i += 2;
			continue;
		}
		// Check for quotes
		if (ch == '\"')
		{
			inquote = !inquote;
		}
		if (inquote)
		{
			++i;
			continue;
		}
		if (ch == '=')
		{
			equalspos = i+1;
		}
		if (ch == ',' || (i+1 == valuesLen))
		{
			if ((i+1 == valuesLen) && equalspos == 0 && singleKey)
			{
				// This is the special case of when its classname.value
				CIMProperty cp("test it");
				//
				// Generally there will be quotes but for integer values
				// they are not strictly necessary so check for them
				//
				if (values[keystart] != '\"')
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
			if (i+1 == valuesLen)
			{
				i++;
			}
			if (equalspos == 0)
			{
				OW_THROWCIMMSG(CIMException::NOT_FOUND,
					Format("Bad key in string (%1)", instanceName).c_str());
			}
			CIMName keyprop = String(&values[keystart], equalspos-keystart-1);
			//
			// Generally there will be quotes but for integer values
			// they are not strictly necessary so check for them
			//
			if (values[equalspos] != '\"')
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
		++i;
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
	int valuesLen = static_cast<int>(inString.length());
	if (valuesLen == 0)
	{
		return inString;
	}
	StringBuffer rv(valuesLen);
	const char* values = inString.c_str();
	for (int i = 0; i < valuesLen; i++)
	{
		switch(values[i])
		{
		case '\b': rv += "\\b"; break;
		case '\t': rv += "\\t"; break;
		case '\n': rv += "\\n"; break;
		case '\f': rv += "\\f"; break;
		case '\r': rv += "\\r"; break;
		case '\"': rv += "\\\""; break;
		case '\\': rv += "\\\\"; break;
		default:
			rv += values[i];
		}
	}
	return rv.releaseString();
}

namespace // anonymous
{
	// This is passed the xNNNN portion of a \xNNNN escape sequence at
	// the supplied offset.  The offset is modified to point to the
	// next character after the hex sequence.
	String parseHexString(const char* ptr, size_t maxLen, size_t& offset)
	{
		String s;

		UInt16 hex = 0;
		size_t j = 1;
		for (; (j <= 4) && (offset + j < maxLen) ; ++j)
		{
			char c = ptr[offset + j];
			if (isdigit(c))
			{
				hex <<= 4;
				hex |= c - '0';
			}
			else if (isxdigit(c))
			{
				hex <<= 4;
				c = toupper(c);
				hex |= c - 'A' + 0xA;
			}
			else
			{
				break;
			}
		}

		if( j <= 1 )
		{
			// No hex digits.  Ignore for now for compatbility, and lack of
			// a expectation for throwing exceptions.
		}
		else if (hex > CHAR_MAX)
		{
			// Out of range.  Use the UTF-8 version of the
			// "replacement" character.
			s += "\xef\xbf\xbd";
		}
		else
		{
			s += static_cast<char>(hex);
		}
		offset += j - 1;

		return s;
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
String
CIMObjectPath::unEscape(const String& inString)
{
	if (inString.empty())
	{
		return inString;
	}

	const size_t valuesLen = inString.length();
	const char* values = inString.c_str();

	StringBuffer rv(valuesLen);

	for( size_t i = 0; i < valuesLen; ++i )
	{
		if ( values[i] == '\\' )
		{
			if ((i + 1) < valuesLen)
			{
				++i;
				switch(values[i])
				{
				case 'b': rv += '\b'; break;
				case 't': rv += '\t'; break;
				case 'n': rv += '\n'; break;
				case 'f': rv += '\f'; break;
				case 'r': rv += '\r'; break;
				case '\"': rv += '\"'; break;
				case '\'': rv += '\''; break;
				case '\\': rv += '\\'; break;
				case 'x':
				case 'X':
					rv += parseHexString(values, valuesLen, i);
					break;
				default:
					// Bad escape characters.
					// NOTE: The old code dropped the escape here, which seems messed up.
					rv += '\\';
					rv += values[i];
				}
			}
			else
			{
				// Trailing backslash...  The old code ignored this.
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
	if (!theClass || isClassPath())
	{
		return *this;
	}
	CIMName propName;
	CIMPropertyArray classProps = theClass.getKeys();
	CIMPropertyArray copProps = getKeys();
	// Remove properties that are not defined in the class
	size_t i = 0;
	while (i < copProps.size())
	{
		propName = copProps[i].getName();
		if (!theClass.getProperty(propName))
		{
			copProps.remove(i);
		}
		else
		{
			++i;
		}
	}
	// Ensure existing properties have the right type
	for (size_t i = 0; i < classProps.size(); i++)
	{
		bool found = false;
		const CIMProperty& cprop = classProps[i];
		propName = cprop.getName();
		for (size_t j = 0; j < copProps.size(); j++)
		{
			CIMProperty iprop = copProps[j];
			if (iprop.getName() == propName)
			{
				CIMValue cv = iprop.getValue();
				iprop = cprop;
				if (cv)
				{
					if (cv.getType() != iprop.getDataType().getType())
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
		if (!found)
		{
			copProps.append(classProps[i]);
		}
	}
	setKeys(copProps);
	return *this;
}

} // end namespace OW_NAMESPACE

