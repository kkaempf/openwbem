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

//
// OW_XMLAttribute.hpp
//
//
#ifndef OW_XMLATTRIBUTE_HPP_
#define OW_XMLATTRIBUTE_HPP_

#include	"OW_config.h"
#include	"OW_String.hpp"
#include	"OW_Array.hpp"

class OW_XMLAttribute;

class OW_XMLAttributeImpl
{
public:                                                  
	OW_XMLAttributeImpl() : m_name(), m_value() 
	{
	}
	OW_XMLAttributeImpl(const OW_String& name, const OW_String& value);
	OW_String getName();
	OW_String getValue();
	OW_Bool equals(const OW_XMLAttribute& attr);

private:
	OW_String m_name;
	OW_String m_value;
};

class OW_XMLAttribute
{
public:
	static const char* const ARRAY_SIZE;
	static const char* const ASSOC_CLASS;
	static const char* const CLASS_NAME;
	static const char* const CLASS_ORIGIN;
	static const char* const DEEP_INHERITANCE;
	static const char* const INCLUDE_CLASS_ORIGIN;
	static const char* const INCLUDE_QUALIFIERS;
	static const char* const INSTANCE_NAME;
	static const char* const LOCAL_ONLY;
	static const char* const MODIFIED_CLASS;
	static const char* const NAME;
	static const char* const NEW_VALUE;
	static const char* const OBJECT_NAME;
	static const char* const OVERRIDABLE;
	static const char* const PROPAGATED;
	static const char* const PROPERTY_LIST;
	static const char* const PROPERTY_NAME;
	static const char* const REFERENCE_CLASS;
	static const char* const RESULT_CLASS;
	static const char* const RESULT_ROLE;
	static const char* const ROLE;
	static const char* const SUPER_CLASS;
	static const char* const TOINSTANCE;
	static const char* const TOSUBCLASS;
	static const char* const TRANSLATABLE;
	static const char* const TYPE;
	static const char* const VALUE_TYPE;

	OW_XMLAttribute() : 
		m_impl(new OW_XMLAttributeImpl) 
		{
		}

	OW_XMLAttribute(const OW_String& name, const OW_String& value) : 
		m_impl(new OW_XMLAttributeImpl(name, value)) 
		{
		}

	OW_XMLAttribute(const OW_XMLAttribute& arg) : 
		m_impl(arg.m_impl) 
		{
		}

	OW_XMLAttribute& operator= (const OW_XMLAttribute& arg)
	{
		m_impl = arg.m_impl; return *this;
	}

	OW_String getName() const
	{
		return m_impl->getName();
	}
	OW_String getValue() const
	{
		return m_impl->getValue();
	}
	OW_Bool equals(const OW_XMLAttribute& attr) const
	{
		return m_impl->equals(attr);
	}

private:
	OW_Reference<OW_XMLAttributeImpl> m_impl;
};

typedef OW_Array<OW_XMLAttribute> OW_XMLAttrArray;


#endif  // OW_XMLATTRIBUTE_HPP_
