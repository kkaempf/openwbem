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
 * @author Dan Nuffer
 */

#include "OWBI1_config.h"
#include "OWBI1_String.hpp"
#include "OWBI1_CIMName.hpp"
#include "OWBI1_CIMNameRep.hpp"
#if defined(OWBI1_HAVE_OSTREAM)
#include <ostream>
#else
#include <iostream>
#endif
/* #endif defined(OWBI1_HAVE_OSTREAM)*/

namespace OWBI1
{

using namespace detail;

///////////////////////////////////////////////////////////////////////////
CIMName::CIMName()
	: m_rep(new CIMNameRep(OpenWBEM::CIMName()))
{
}

///////////////////////////////////////////////////////////////////////////
CIMName::CIMName(const String& name)
	: m_rep(new CIMNameRep(name.c_str()))
{
}
	
///////////////////////////////////////////////////////////////////////////
CIMName::CIMName(const char* name)
	: m_rep(new CIMNameRep(name))
{
}
	
///////////////////////////////////////////////////////////////////////////
CIMName::CIMName(const CIMName& name)
	: SerializableIFC(name)
	, m_rep(name.m_rep)
{
}
	
///////////////////////////////////////////////////////////////////////////
CIMName::~CIMName()
{
}

///////////////////////////////////////////////////////////////////////////
CIMName&
CIMName::operator=(const String& name)
{
	m_rep = new CIMNameRep(name.c_str());
	return *this;
}

///////////////////////////////////////////////////////////////////////////
CIMName&
CIMName::operator=(const char* name)
{
	m_rep = new CIMNameRep(name);
	return *this;
}

///////////////////////////////////////////////////////////////////////////
CIMName&
CIMName::operator=(const CIMName& other)
{
	m_rep = other.m_rep;
	return *this;
}

///////////////////////////////////////////////////////////////////////////
String
CIMName::toString() const
{
	return m_rep->name.toString().c_str();
}

//////////////////////////////////////////////////////////////////////////////
const char*
CIMName::c_str() const
{
	return m_rep->name.toString().c_str();
}

//////////////////////////////////////////////////////////////////////////////
void
CIMName::readObject(std::istream &istrm)
{
	m_rep->name.readObject(istrm);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMName::writeObject(std::ostream &ostrm) const
{
	m_rep->name.writeObject(ostrm);
}

///////////////////////////////////////////////////////////////////////////
bool 
CIMName::isValid() const
{
	return m_rep->name.isValid();
}

///////////////////////////////////////////////////////////////////////////
CIMName::operator CIMName::safe_bool () const
{
	return isValid() ? &CIMName::m_rep : 0;
}
	
///////////////////////////////////////////////////////////////////////////
bool
CIMName::operator!() const
{
	return !isValid();
}

///////////////////////////////////////////////////////////////////////////
detail::CIMNameRepRef
CIMName::getRep() const
{
	return m_rep;
}

///////////////////////////////////////////////////////////////////////////
bool operator<(const CIMName& x, const CIMName& y)
{
	return x.m_rep->name < y.m_rep->name;
}

///////////////////////////////////////////////////////////////////////////
bool operator==(const CIMName& x, const CIMName& y)
{
	return x.m_rep->name == y.m_rep->name;
}

//////////////////////////////////////////////////////////////////////////////
bool operator<=(const CIMName& x, const CIMName& y)
{
    return !(y < x);
}

//////////////////////////////////////////////////////////////////////////////
bool operator>(const CIMName& x, const CIMName& y)
{
    return y < x;
}

//////////////////////////////////////////////////////////////////////////////
bool operator>=(const CIMName& x, const CIMName& y)
{
    return !(x < y);
}

//////////////////////////////////////////////////////////////////////////////
bool operator!=(const CIMName& x, const CIMName& y)
{
	return !(x == y);
}

//////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& ostr, const CIMName& name)
{
	ostr << name.toString();
	return ostr;
}

} // end namespace OWBI1





