/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_WQLInstancePropertySource.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMValueCast.hpp"

///////////////////////////////////////////////////////////////////////////////	
OW_WQLInstancePropertySource::~OW_WQLInstancePropertySource()
{
}

///////////////////////////////////////////////////////////////////////////////	
bool OW_WQLInstancePropertySource::evaluateISA(const OW_String &propertyName, const OW_String &className) const 
{
	OW_StringArray propNames = propertyName.tokenize(".");
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
bool OW_WQLInstancePropertySource::getValue(const OW_String &propertyName, OW_WQLOperand &value) const 
{
	OW_StringArray propNames = propertyName.tokenize(".");
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
bool OW_WQLInstancePropertySource::evaluateISAAux(const OW_CIMInstance& ci, OW_StringArray propNames, const OW_String &className) const 
{
	if (propNames.empty())
	{
		return classIsDerivedFrom(ci.getClassName(), className);
	}

	OW_CIMProperty p = ci.getProperty(propNames[0]);
	if (!p)
	{
		return false;
	}

	OW_CIMValue v = p.getValue();
	switch (v.getType())
	{
		case OW_CIMDataType::EMBEDDEDINSTANCE:
		{
			propNames.remove(0);
			OW_CIMInstance embed;
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
bool OW_WQLInstancePropertySource::classIsDerivedFrom(const OW_String& cls, const OW_String& className) const
{
	OW_String curClassName = cls;
	while (!curClassName.empty())
	{
		if (curClassName.equalsIgnoreCase(className))
		{
			return true;
		}
		// didn't match, so try the superclass of curClassName
		OW_CIMClass cls2 = m_hdl->getClass(m_ns, curClassName);
		curClassName = cls2.getSuperClass();

	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////	
bool OW_WQLInstancePropertySource::getValueAux(const OW_CIMInstance& ci, OW_StringArray propNames, OW_WQLOperand& value)
{
	if (propNames.empty())
	{
		return false;
	}

	OW_CIMProperty p = ci.getProperty(propNames[0]);
	if (!p)
	{
		return false;
	}

	OW_CIMValue v = p.getValue();
	switch (v.getType())
	{
		case OW_CIMDataType::DATETIME:
		case OW_CIMDataType::CIMNULL:
			value = OW_WQLOperand();
			break;
		case OW_CIMDataType::UINT8:
		case OW_CIMDataType::SINT8:
		case OW_CIMDataType::UINT16:
		case OW_CIMDataType::SINT16:
		case OW_CIMDataType::UINT32:
		case OW_CIMDataType::SINT32:
		case OW_CIMDataType::UINT64:
		case OW_CIMDataType::SINT64:
		case OW_CIMDataType::CHAR16:
		{
			OW_Int64 x;
			OW_CIMValueCast::castValueToDataType(v, OW_CIMDataType::SINT64).get(x);
			value = OW_WQLOperand(x, WQL_INTEGER_VALUE_TAG);
			break;
		}
		case OW_CIMDataType::STRING:
			value = OW_WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
			break;
		case OW_CIMDataType::BOOLEAN:
		{
			OW_Bool b;
			v.get(b);
			value = OW_WQLOperand(b, WQL_BOOLEAN_VALUE_TAG);
			break;
		}
		case OW_CIMDataType::REAL32:
		case OW_CIMDataType::REAL64:
		{
			OW_Real64 x;
			OW_CIMValueCast::castValueToDataType(v, OW_CIMDataType::REAL64).get(x);
			value = OW_WQLOperand(x, WQL_DOUBLE_VALUE_TAG);
			break;
		}
		case OW_CIMDataType::REFERENCE:
			value = OW_WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
			break;
		case OW_CIMDataType::EMBEDDEDCLASS:
			value = OW_WQLOperand();
			break;
		case OW_CIMDataType::EMBEDDEDINSTANCE:
		{
			propNames.remove(0);
			OW_CIMInstance embed;
			v.get(embed);
			if (!embed)
			{
				return false;
			}
			return getValueAux(embed, propNames, value);
		}
		break;
		default:
			value = OW_WQLOperand();
			break;
	}

	return true;
}

