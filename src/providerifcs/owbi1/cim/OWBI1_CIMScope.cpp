/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OWBI1_config.h"
#include "OWBI1_CIMScope.hpp"
#include "OWBI1_CIMScopeRep.hpp"
#include "OWBI1_String.hpp"
#include "OW_String.hpp"

namespace OWBI1
{

using namespace detail;
using std::istream;
using std::ostream;

/////////////////////////////////////////////////////////////////////////////
CIMScope::CIMScope() 
	: m_rep(new CIMScopeRep(OpenWBEM::CIMScope()))
{
}

/////////////////////////////////////////////////////////////////////////////
CIMScope::CIMScope(Scope scopeVal) 
	: m_rep(new CIMScopeRep(OpenWBEM::CIMScope(OpenWBEM::CIMScope::Scope(scopeVal))))
{
}
/////////////////////////////////////////////////////////////////////////////
CIMScope::CIMScope(const CIMScope& arg) 
	: CIMBase(arg)
	, m_rep(arg.m_rep) 
{
}

/////////////////////////////////////////////////////////////////////////////
CIMScope::CIMScope(const detail::CIMScopeRepRef& rep)
	: m_rep(rep)
{
}

/////////////////////////////////////////////////////////////////////////////
CIMScope::~CIMScope()
{
}

/////////////////////////////////////////////////////////////////////////////
CIMScope& 
CIMScope::operator= (const CIMScope& arg)
{
	m_rep = arg.m_rep;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
CIMScope::Scope 
CIMScope::getScope() const 
{  
	return Scope(m_rep->scope.getScope()); 
}

/////////////////////////////////////////////////////////////////////////////
bool 
CIMScope::equals(const CIMScope& arg) const
{
	return m_rep->scope.equals(arg.m_rep->scope);
}
/////////////////////////////////////////////////////////////////////////////
bool 
CIMScope::operator == (const CIMScope& arg) const
{
	return equals(arg);
}
/////////////////////////////////////////////////////////////////////////////
bool 
CIMScope::operator != (const CIMScope& arg) const
{
	return !equals(arg);
}

/////////////////////////////////////////////////////////////////////////////
CIMScope::operator safe_bool () const
{  
	return m_rep->scope ? &CIMScope::m_rep : 0; 
}
/////////////////////////////////////////////////////////////////////////////
bool 
CIMScope::operator!() const
{  
	return !m_rep->scope; 
}
/////////////////////////////////////////////////////////////////////////////
bool operator<(const CIMScope& x, const CIMScope& y)
{
	return *x.getRep() < *y.getRep();
}

//////////////////////////////////////////////////////////////////////////////					
String
CIMScope::toString() const
{
	return toMOF();
}
//////////////////////////////////////////////////////////////////////////////					
String
CIMScope::toMOF() const
{
	return m_rep->scope.toMOF().c_str();
}
//////////////////////////////////////////////////////////////////////////////					
void
CIMScope::readObject(istream &istrm)
{
	m_rep->scope.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////					
void
CIMScope::writeObject(ostream &ostrm) const
{
	m_rep->scope.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////					
void
CIMScope::setNull() 
{
	m_rep->scope.setNull();
}

//////////////////////////////////////////////////////////////////////////////					
CIMScopeRepRef
CIMScope::getRep() const
{
	return m_rep;
}

} // end namespace OWBI1

