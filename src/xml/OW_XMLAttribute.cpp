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
// XMLAttribute.hpp
//
//
#include	"OW_config.h"
#include	"OW_XMLAttribute.hpp"

OW_XMLAttributeImpl::OW_XMLAttributeImpl(const OW_String& name, 
	const OW_String& value) : m_name(name), m_value(value)
{
}
    
OW_String
OW_XMLAttributeImpl::getName()
{
	return m_name;
}

OW_String
OW_XMLAttributeImpl::getValue()
{
	return m_value;
}
    
OW_Bool
OW_XMLAttributeImpl::equals(const OW_XMLAttribute& attr)
{
	return(attr.getName().equalsIgnoreCase(m_name) 
			  && (attr.getValue() == m_value));
}

const char* const OW_XMLAttribute::ARRAY_SIZE					= "ARRAYSIZE";
const char* const OW_XMLAttribute::ASSOC_CLASS				= "ASSOCCLASS";
const char* const OW_XMLAttribute::CLASS_NAME					= "CLASSNAME";
const char* const OW_XMLAttribute::CLASS_ORIGIN				= "CLASSORIGIN";
const char* const OW_XMLAttribute::DEEP_INHERITANCE			= "DEEPINHERITANCE";
const char* const OW_XMLAttribute::INCLUDE_CLASS_ORIGIN	= "INCLUDECLASSORIGIN";
const char* const OW_XMLAttribute::INCLUDE_QUALIFIERS		= "INCLUDEQUALIFIERS";
const char* const OW_XMLAttribute::INSTANCE_NAME				= "INSTANCENAME";
const char* const OW_XMLAttribute::LOCAL_ONLY					= "LOCALONLY";
const char* const OW_XMLAttribute::MODIFIED_CLASS			= "MODIFIEDCLASS";
const char* const OW_XMLAttribute::NAME							= "NAME";
const char* const OW_XMLAttribute::NEW_VALUE					= "NEWVALUE";
const char* const OW_XMLAttribute::OBJECT_NAME				= "OBJECTNAME";
const char* const OW_XMLAttribute::OVERRIDABLE				= "OVERRIDABLE";
const char* const OW_XMLAttribute::PROPAGATED					= "PROPAGATED";
const char* const OW_XMLAttribute::PROPERTY_LIST				= "PROPERTYLIST";
const char* const OW_XMLAttribute::PROPERTY_NAME				= "PROPERTYNAME";
const char* const OW_XMLAttribute::REFERENCE_CLASS			= "REFERENCECLASS";
const char* const OW_XMLAttribute::RESULT_CLASS				= "RESULTCLASS";
const char* const OW_XMLAttribute::RESULT_ROLE				= "RESULTROLE";
const char* const OW_XMLAttribute::ROLE							= "ROLE";
const char* const OW_XMLAttribute::SUPER_CLASS				= "SUPERCLASS";
const char* const OW_XMLAttribute::TOINSTANCE					= "TOINSTANCE";
const char* const OW_XMLAttribute::TOSUBCLASS					= "TOSUBCLASS";
const char* const OW_XMLAttribute::TRANSLATABLE				= "TRANSLATABLE";
const char* const OW_XMLAttribute::TYPE							= "TYPE";
const char* const OW_XMLAttribute::VALUE_TYPE					= "VALUETYPE";
