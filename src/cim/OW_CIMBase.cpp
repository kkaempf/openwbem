/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "blocxx/Format.hpp"
#include "blocxx/Assertion.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_BinarySerialization.hpp"
#include <cstring>

using namespace blocxx;

namespace OW_NAMESPACE
{
OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID(BadCIMSignature, IOException);

using std::streambuf;
using std::ostream;
//////////////////////////////////////////////////////////////////////////////
// static
void
CIMBase::readSig( streambuf & istr, char const * const sig )
{
	char expected, read;
	BLOCXX_ASSERT( strlen(sig) == 1 );
	expected = sig[0];
	BinarySerialization::read(istr, &read, sizeof(read));
	if (expected != read)
	{
		OW_THROW_ERR(BadCIMSignatureException,
			Format("Signature does not match. In CIMBase::readSig. "
				"signature read: %1, expected: %2",
				int(read), sig).c_str(), E_UNEXPECTED_SIGNATURE);
	}
}
//////////////////////////////////////////////////////////////////////////////
// static
UInt32
CIMBase::readSig(
	streambuf & istr, char const * const sig,
	char const * const verSig, UInt32 maxVersion
)
{
	UInt32 version = 0;
	char ch;
	BLOCXX_ASSERT( strlen(sig) == 1);
	BLOCXX_ASSERT( strlen(verSig) == 1);

	BinarySerialization::read(istr, &ch, sizeof(ch));
	if (sig[0] != ch)
	{
		if (verSig[0] != ch)
		{
			OW_THROW_ERR(BadCIMSignatureException,
				Format("Signature does not match. In CIMBase::readSig. "
					"signature read: %1, expected: %2 or %3",
					int(ch), sig, verSig).c_str(), E_UNEXPECTED_SIGNATURE);
		}

		// Version is ASN.1 length encoded
		BinarySerialization::readLen(istr, version);
		if (version > maxVersion)
		{
			OW_THROW_ERR(BadCIMSignatureException,
				Format("CIMBase::readSig. Unknown version %1, only versions <= %2 are handled.",
				version, maxVersion).c_str(), E_UNKNOWN_VERSION);
		}
	}

	return version;
}

//////////////////////////////////////////////////////////////////////////////
// static
void
CIMBase::writeSig( streambuf & ostr, char const * const sig )
{
	BLOCXX_ASSERT(strlen(sig) == 1);
	BinarySerialization::write(ostr, sig, 1);
}
//////////////////////////////////////////////////////////////////////////////
// static
void
CIMBase::writeSig(streambuf & ostr, char const * const sig, UInt32 version)
{
	BLOCXX_ASSERT(strlen(sig) == 1);
	BinarySerialization::write(ostr, sig, 1);
	// Use ASN.1 length encoding for version
	BinarySerialization::writeLen(ostr, version);
}

//////////////////////////////////////////////////////////////////////////////
ostream & operator<<(ostream & ostr, const CIMBase & cb)
{
	ostr << cb.toString();
	return ostr;
}
//////////////////////////////////////////////////////////////////////////////
CIMBase::~CIMBase()
{
}

} // end namespace OW_NAMESPACE

