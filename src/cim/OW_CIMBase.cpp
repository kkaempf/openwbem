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
#include "OW_CIMBase.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_StringStream.hpp"
#include "OW_IOException.hpp"
#include "OW_BinarySerialization.hpp"
#include <cstring>

namespace OpenWBEM
{

using std::istream;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////		
// static
void
CIMBase::readSig( istream& istr, const char* const sig )
{
	char expected, read;
	OW_ASSERT( strlen(sig) == 1 );
	expected = sig[0];
	BinarySerialization::read(istr, &read, sizeof(read));
	if(expected != read)
	{
		OW_THROW(BadCIMSignatureException,
			Format("Signature does not match. In CIMBase::readSig. "
				"signature read: %1, expected: %2",
				read, sig).c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////////		
// static
UInt32 
CIMBase::readSig(std::istream& istr, const char* const sig,
	const char* const verSig, UInt32 maxVersion)
{
	UInt32 version = 0;
	char ch;
	OW_ASSERT( strlen(sig) == 1);
	OW_ASSERT( strlen(verSig) == 1);

	BinarySerialization::read(istr, &ch, sizeof(ch));
	if(sig[0] != ch)
	{
		if(verSig[0] != ch)
		{
			OW_THROW(BadCIMSignatureException,
				Format("Signature does not match. In CIMBase::readSig. "
					"signature read: %1, expected: %2 or %3",
					ch, sig, verSig).c_str() );
		}

		// Version is ASN.1 length encoded
		BinarySerialization::readLen(istr, version);
		if (version > maxVersion)
		{
			OW_THROW(BadCIMSignatureException,
				Format("CIMBase::readSig. Unknown version %1, only versions <= %2 are handled.",
				version, maxVersion).c_str() );
		}
	}

	return version;
}

//////////////////////////////////////////////////////////////////////////////		
// static
void
CIMBase::writeSig( ostream& ostr, const char* const sig )
{
	OW_ASSERT(strlen(sig) == 1);
	BinarySerialization::write(ostr, sig, 1);
}
//////////////////////////////////////////////////////////////////////////////		
// static
void 
CIMBase::writeSig(std::ostream& ostr, const char* const sig, UInt32 version)
{
	OW_ASSERT(strlen(sig) == 1);
	BinarySerialization::write(ostr, sig, 1);
	// Use ASN.1 length encoding for version
	BinarySerialization::writeLen(ostr, version);
}

//////////////////////////////////////////////////////////////////////////////		
std::ostream& operator<<(std::ostream& ostr, const CIMBase& cb)
{
	ostr << cb.toString();
	return ostr;
}
//////////////////////////////////////////////////////////////////////////////		
CIMBase::~CIMBase() 
{
}

} // end namespace OpenWBEM

