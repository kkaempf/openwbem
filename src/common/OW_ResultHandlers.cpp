/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ResultHandlers.hpp"
#include "OW_Array.hpp"
#include "OW_Enumeration.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"

namespace OW_NAMESPACE
{

/////////////////////////////////////////////////////////////////////////////
void 
CIMClassEnumBuilder::doHandle(const CIMClass &c)
{
	m_e.addElement(c);
}

/////////////////////////////////////////////////////////////////////////////
CIMClassEnumBuilder::~CIMClassEnumBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
CIMClassArrayBuilder::doHandle(const CIMClass &c)
{
	m_a.push_back(c);
}

/////////////////////////////////////////////////////////////////////////////
CIMClassArrayBuilder::~CIMClassArrayBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
StringArrayBuilder::doHandle(const String &s)
{
	m_a.push_back(s);
}

/////////////////////////////////////////////////////////////////////////////
StringArrayBuilder::~StringArrayBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
StringEnumBuilder::doHandle(const String &s)
{
	m_e.addElement(s);
}

/////////////////////////////////////////////////////////////////////////////
StringEnumBuilder::~StringEnumBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
CIMObjectPathEnumBuilder::doHandle(const CIMObjectPath &cop)
{
	m_e.addElement(cop);
}

/////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumBuilder::~CIMObjectPathEnumBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
CIMObjectPathArrayBuilder::doHandle(const CIMObjectPath &cop)
{
	m_a.push_back(cop);
}

/////////////////////////////////////////////////////////////////////////////
CIMObjectPathArrayBuilder::~CIMObjectPathArrayBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
CIMInstanceEnumBuilder::doHandle(const CIMInstance &i)
{
	m_e.addElement(i);
}

/////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumBuilder::~CIMInstanceEnumBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
CIMInstanceArrayBuilder::doHandle(const CIMInstance &i)
{
	m_a.push_back(i);
}

/////////////////////////////////////////////////////////////////////////////
CIMInstanceArrayBuilder::~CIMInstanceArrayBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
CIMQualifierTypeEnumBuilder::doHandle(const CIMQualifierType &qt)
{
	m_e.addElement(qt);
}

/////////////////////////////////////////////////////////////////////////////
CIMQualifierTypeEnumBuilder::~CIMQualifierTypeEnumBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
void 
CIMQualifierTypeArrayBuilder::doHandle(const CIMQualifierType &qt)
{
	m_a.push_back(qt);
}

/////////////////////////////////////////////////////////////////////////////
CIMQualifierTypeArrayBuilder::~CIMQualifierTypeArrayBuilder()
{
}

} // end namespace OW_NAMESPACE



