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
#include "OW_Char16.hpp"
#include "OW_String.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_BinIfcIO.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
OW_Char16::OW_Char16(const OW_String& x) :
	m_value(0)
{
	if(x.length() > 0)
	{
		m_value = (OW_UInt16) x.charAt(0);
	}
}

//////////////////////////////////////////////////////////////////////////////
// ATTN: UTF8 support?
// STATIC
const char*
OW_Char16::xmlExcape(OW_UInt16 c16, char bfr[20])
{
	switch ((char)c16)
	{
		case '\n': strcpy(bfr, "\\n"); break;
		case '\t': strcpy(bfr, "\\t"); break;
		case '"': strcpy(bfr, "&quot;"); break;
		case '\'': strcpy(bfr, "&apos;"); break;
		case '<': strcpy(bfr, "&lt;"); break;
		case '>': strcpy(bfr, "&gt;"); break;
		case '&': strcpy(bfr, "&amp;"); break;
		default: bfr[0] = (char)c16; bfr[1] = (char)0; break;
	}

	return bfr;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_Char16::writeObject(ostream& ostrm) const /*throw (OW_IOException)*/
{
	OW_UInt16 v = OW_hton16(m_value);
	OW_BinIfcIO::write(ostrm, &v, sizeof(v));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_Char16::readObject(istream& istrm) /*throw (OW_IOException)*/
{
	OW_BinIfcIO::read(istrm, &m_value, sizeof(m_value));
	m_value = OW_ntoh16(m_value);
}

//////////////////////////////////////////////////////////////////////////////
std::ostream&
operator<< (std::ostream& ostrm, const OW_Char16& arg)
{
	OW_UInt16 val = arg.getValue();

	if(val > 0 && val <= 127)
	{
		ostrm << char(val);
	}
	else
	{
		// Print in hex format:
		char bfr[8];
		sprintf(bfr, "\\x%04X", val);
		ostrm << bfr;
	}

	return ostrm;
}

