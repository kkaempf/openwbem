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
#include "OWBI1_Char16.hpp"
#include "OWBI1_String.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_UTF8Utils.hpp"
#include <cstdio>
#include <cstring>
#if defined(OWBI1_HAVE_ISTREAM) && defined(OWBI1_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace OWBI1
{

using namespace OpenWBEM;
using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
Char16::Char16(const String& x) :
	m_value(0)
{
	m_value = UTF8Utils::UTF8toUCS2(x.c_str());
}
//////////////////////////////////////////////////////////////////////////////
String
Char16::toString() const
{
	return String(UTF8Utils::UCS2toUTF8(m_value).c_str());
}
//////////////////////////////////////////////////////////////////////////////
void
Char16::writeObject(ostream& ostrm) const
{
	BinarySerialization::write(ostrm, m_value);
}
//////////////////////////////////////////////////////////////////////////////
void
Char16::readObject(istream& istrm)
{
	BinarySerialization::read(istrm, m_value);
}
//////////////////////////////////////////////////////////////////////////////
std::ostream&
operator<< (std::ostream& ostrm, const Char16& arg)
{
	UInt16 val = arg.getValue();
	if (val > 0 && val <= 127)
	{
		ostrm << static_cast<char>(val);
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

} // end namespace OWBI1

