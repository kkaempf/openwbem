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

#include "OW_config.h"
#include "OW_CIMFlavor.hpp"
#include "OW_BinIfcIO.hpp"
#include "OW_String.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif


using std::ostream;
using std::istream;

//////////////////////////////////////////////////////////////////////////////		
OW_String
OW_CIMFlavor::toString() const
{
	OW_String s("FLAVOR(");
	s += toMOF() + ")";
	return s;
}

//////////////////////////////////////////////////////////////////////////////		
OW_String
OW_CIMFlavor::toMOF() const
{
	const char* strf;
	switch(m_flavor)
	{
		case ENABLEOVERRIDE: strf = "ENABLEOVERRIDE"; break;
		case DISABLEOVERRIDE: strf = "DISABLEOVERRIDE"; break;
		case RESTRICTED: strf = "RESTRICTED"; break;
		case TOSUBCLASS: strf = "TOSUBCLASS"; break;
		case TRANSLATE: strf = "TRANSLATE"; break;
		default: strf = "BAD FLAVOR"; break;
	}
	
	return OW_String(strf);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CIMFlavor::readObject(istream &istrm)
{
	OW_CIMBase::readSig( istrm, OW_CIMFLAVORSIG );

	OW_BinIfcIO::read(istrm, &m_flavor, sizeof(m_flavor));

	m_flavor = OW_ntoh32(m_flavor);
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_CIMFlavor::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMFLAVORSIG );

	OW_Int32 nflavor = OW_hton32(m_flavor);
	OW_BinIfcIO::write(ostrm, &nflavor, sizeof(nflavor));
}


