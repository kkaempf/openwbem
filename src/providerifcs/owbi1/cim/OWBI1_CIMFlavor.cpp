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
#include "OWBI1_CIMFlavor.hpp"
#include "OWBI1_CIMFlavorRep.hpp"
#include "OWBI1_String.hpp"
#include "OW_String.hpp"
#if defined(OWBI1_HAVE_ISTREAM) && defined(OWBI1_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OWBI1
{

using namespace detail;
using std::ostream;
using std::istream;
//////////////////////////////////////////////////////////////////////////////		
String
CIMFlavor::toString() const
{
	return m_rep->flavor.toString().c_str();
}
//////////////////////////////////////////////////////////////////////////////		
String
CIMFlavor::toMOF() const
{
	return m_rep->flavor.toMOF().c_str();
}
//////////////////////////////////////////////////////////////////////////////		
void
CIMFlavor::readObject(istream &istrm)
{
	m_rep->flavor.readObject(istrm);
}

//////////////////////////////////////////////////////////////////////////////		
void
CIMFlavor::writeObject(ostream &ostrm) const
{
	m_rep->flavor.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////		
void
CIMFlavor::setNull()
{
	m_rep->flavor.setNull();
}

//////////////////////////////////////////////////////////////////////////////		
CIMFlavor::operator safe_bool () const
{  
	return m_rep->flavor ? &CIMFlavor::m_rep : 0; 
}

//////////////////////////////////////////////////////////////////////////////		
bool 
CIMFlavor::operator!() const
{  
	return !m_rep->flavor; 
}

//////////////////////////////////////////////////////////////////////////////		
CIMFlavor::CIMFlavor(Flavor iflavor) 
	: m_rep(new CIMFlavorRep(OpenWBEM::CIMFlavor(OpenWBEM::CIMFlavor::Flavor(iflavor))))
{
}

//////////////////////////////////////////////////////////////////////////////		
CIMFlavor::CIMFlavor()
	: m_rep(new CIMFlavorRep(OpenWBEM::CIMFlavor()))
{
}

//////////////////////////////////////////////////////////////////////////////		
CIMFlavor::CIMFlavor(const CIMFlavor& arg)
	: CIMBase(arg)
	, m_rep(arg.m_rep)
{
}

//////////////////////////////////////////////////////////////////////////////		
CIMFlavor::CIMFlavor(const detail::CIMFlavorRepRef& rep)
	: m_rep(rep)
{
}

//////////////////////////////////////////////////////////////////////////////		
CIMFlavor& 
CIMFlavor::operator= (const CIMFlavor& arg)
{
	m_rep = arg.m_rep;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////		
bool 
CIMFlavor::isValid()
{
	return m_rep->flavor.isValid();
}

//////////////////////////////////////////////////////////////////////////////		
bool 
CIMFlavor::equals(const CIMFlavor& arg)
{
	return m_rep->flavor.equals(arg.m_rep->flavor);
}
//////////////////////////////////////////////////////////////////////////////		
bool 
CIMFlavor::operator== (const CIMFlavor& arg)
{
	return equals(arg);
}
//////////////////////////////////////////////////////////////////////////////		
bool 
CIMFlavor::operator!= (const CIMFlavor& arg)
{
	return (equals(arg) == false);
}
//////////////////////////////////////////////////////////////////////////////		
Int32 
CIMFlavor::getFlavor() const 
{  
	return m_rep->flavor.getFlavor(); 
}

//////////////////////////////////////////////////////////////////////////////		
bool operator<(const CIMFlavor& x, const CIMFlavor& y)
{
	return *x.getRep() < *y.getRep();
}

//////////////////////////////////////////////////////////////////////////////		
// static 
bool
CIMFlavor::validFlavor(Int32 iflavor)
{
	return OpenWBEM::CIMFlavor::validFlavor(iflavor);
}

//////////////////////////////////////////////////////////////////////////////		
detail::CIMFlavorRepRef 
CIMFlavor::getRep() const
{
	return m_rep;
}

} // end namespace OWBI1

