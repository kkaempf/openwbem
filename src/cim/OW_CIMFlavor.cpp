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

#include "OW_config.h"
#include "OW_CIMFlavor.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_String.hpp"
#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

using std::ostream;
using std::istream;
//////////////////////////////////////////////////////////////////////////////		
String
CIMFlavor::toString() const
{
	String s("FLAVOR(");
	s += toMOF() + ")";
	return s;
}
//////////////////////////////////////////////////////////////////////////////		
String
CIMFlavor::toMOF() const
{
	switch(m_flavor)
	{
		case ENABLEOVERRIDE: 
		return "EnableOverride"; 

		case DISABLEOVERRIDE: 
		return "DisableOverride";

		case RESTRICTED: 
		return "Restricted";

		case TOSUBCLASS: 
		return "ToSubclass";

		case TRANSLATE: 
		return "Translatable";

		default: 
		return "BAD FLAVOR";
	}
}
//////////////////////////////////////////////////////////////////////////////		
void
CIMFlavor::readObject(istream &istrm)
{
	// Don't do this, it'll double the size CIMBase::readSig( istrm, CIMFLAVORSIG );
	UInt32 f;
	BinarySerialization::readLen(istrm, f);
	m_flavor = f;
}

//////////////////////////////////////////////////////////////////////////////		
void
CIMFlavor::writeObject(ostream &ostrm) const
{
	// Don't do this, it'll double the size CIMBase::writeSig( ostrm, CIMFLAVORSIG );
	BinarySerialization::writeLen(ostrm, m_flavor);
}

//////////////////////////////////////////////////////////////////////////////		
void
CIMFlavor::setNull()
{
	m_flavor = INVALID;
}

} // end namespace OpenWBEM

