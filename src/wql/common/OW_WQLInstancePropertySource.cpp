/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_WQLInstancePropertySource.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_Bool.hpp"

namespace OW_NAMESPACE
{

///////////////////////////////////////////////////////////////////////////////	
WQLInstancePropertySource::~WQLInstancePropertySource()
{
}
///////////////////////////////////////////////////////////////////////////////	
bool WQLInstancePropertySource::evaluateISA(const String &propertyName, const String &className) const
{
	StringArray propNames = propertyName.tokenize(".");
	if (propNames.empty())
	{
		return false;
	}
	if (propNames[0] == ci.getClassName())
	{
		propNames.remove(0);
	}
	return evaluateISAAux(ci, propNames, className);
}
///////////////////////////////////////////////////////////////////////////////	
bool WQLInstancePropertySource::getValue(const String &propertyName, WQLOperand &value) const
{
	StringArray propNames = propertyName.tokenize(".");
	if (propNames.empty())
	{
		return false;
	}
	if (propNames[0].equalsIgnoreCase(ci.getClassName()))
	{
		propNames.remove(0);
	}
	return getValueAux(ci, propNames, value);
}
///////////////////////////////////////////////////////////////////////////////	
bool WQLInstancePropertySource::evaluateISAAux(const CIMInstance& ci, StringArray propNames, const String &className) const
{
	if (propNames.empty())
	{
		return classIsDerivedFrom(ci.getClassName(), className);
	}
	CIMProperty p = ci.getProperty(propNames[0]);
	if (!p)
	{
		return false;
	}
	CIMValue v = p.getValue();
	switch (v.getType())
	{
		case CIMDataType::EMBEDDEDINSTANCE:
		{
			propNames.remove(0);
			CIMInstance embed;
			v.get(embed);
			if (!embed)
			{
				return false;
			}
			return evaluateISAAux(embed, propNames, className);
		}
		default:
			return false;
	}
}
///////////////////////////////////////////////////////////////////////////////	
bool WQLInstancePropertySource::classIsDerivedFrom(const String& cls, const String& className) const
{
	CIMName curClassName = cls;
	while (curClassName != CIMName())
	{
		if (curClassName == className)
		{
			return true;
		}
		// didn't match, so try the superclass of curClassName
		CIMClass cls2 = m_hdl->getClass(m_ns, curClassName.toString());
		curClassName = cls2.getSuperClass();
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////	
bool WQLInstancePropertySource::getValueAux(const CIMInstance& ci, const StringArray& propNames, WQLOperand& value)
{
	if (propNames.empty())
	{
		return false;
	}
	CIMProperty p = ci.getProperty(propNames[0]);
	if (!p)
	{
		return false;
	}
	CIMValue v = p.getValue();
	switch (v.getType())
	{
		case CIMDataType::DATETIME:
		case CIMDataType::CIMNULL:
			value = WQLOperand();
			break;
		case CIMDataType::UINT8:
		case CIMDataType::SINT8:
		case CIMDataType::UINT16:
		case CIMDataType::SINT16:
		case CIMDataType::UINT32:
		case CIMDataType::SINT32:
		case CIMDataType::UINT64:
		case CIMDataType::SINT64:
		case CIMDataType::CHAR16:
		{
			Int64 x;
			CIMValueCast::castValueToDataType(v, CIMDataType::SINT64).get(x);
			value = WQLOperand(x, WQL_INTEGER_VALUE_TAG);
			break;
		}
		case CIMDataType::STRING:
			value = WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
			break;
		case CIMDataType::BOOLEAN:
		{
			Bool b;
			v.get(b);
			value = WQLOperand(b, WQL_BOOLEAN_VALUE_TAG);
			break;
		}
		case CIMDataType::REAL32:
		case CIMDataType::REAL64:
		{
			Real64 x;
			CIMValueCast::castValueToDataType(v, CIMDataType::REAL64).get(x);
			value = WQLOperand(x, WQL_DOUBLE_VALUE_TAG);
			break;
		}
		case CIMDataType::REFERENCE:
			value = WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
			break;
		case CIMDataType::EMBEDDEDCLASS:
			value = WQLOperand();
			break;
		case CIMDataType::EMBEDDEDINSTANCE:
		{
			CIMInstance embed;
			v.get(embed);
			if (!embed)
			{
				return false;
			}
			StringArray newPropNames(propNames.begin() + 1, propNames.end());
			return getValueAux(embed, newPropNames, value);
		}
		break;
		default:
			value = WQLOperand();
			break;
	}
	return true;
}

} // end namespace OW_NAMESPACE

