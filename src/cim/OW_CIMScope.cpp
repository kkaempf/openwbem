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
#include "OW_CIM.hpp"

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////					
OW_String
OW_CIMScope::toString() const
{
	return toMOF();
}

//////////////////////////////////////////////////////////////////////////////					
OW_String
OW_CIMScope::toMOF() const
{
	OW_String rv;

	switch(m_val)
	{
		case SCHEMA: rv = "SCHEMA"; break;
		case CLASS: rv = "CLASS"; break;
		case ASSOCIATION: rv = "ASSOCIATION";
		case INDICATION: rv = "INDICATION"; break;
		case PROPERTY: rv = "PROPERTY"; break;
		case REFERENCE: rv = "REFERENCE"; break;
		case METHOD: rv = "METHOD"; break;
		case PARAMETER: rv = "PARAMETER"; break;
		case ANY: rv = "ANY"; break;
		default: rv = "BAD SCOPE"; break;
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////					
void
OW_CIMScope::readObject(istream &istrm)
{
	OW_CIMBase::readSig( istrm, OW_CIMSCOPESIG );
	if(!istrm.read((char*)&m_val, sizeof(m_val)))
	{
		OW_THROW(OW_IOException, "failed to read scope value");
	}
	m_val = OW_ntoh32(m_val);
}

//////////////////////////////////////////////////////////////////////////////					
void
OW_CIMScope::writeObject(ostream &ostrm) const
{
	OW_CIMBase::writeSig( ostrm, OW_CIMSCOPESIG );
	OW_Int32 nv = OW_hton32(m_val);
	if(!ostrm.write((const char*)&nv, sizeof(nv)))
	{
		OW_THROW(OW_IOException, "failed to write scope value");
	}
}

