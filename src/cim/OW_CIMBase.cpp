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
#include "OW_CIMBase.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_StringStream.hpp"
#include "OW_IOException.hpp"
#include "OW_BinIfcIO.hpp"

#include <iostream>
using std::istream;
using std::ostream;

#include <cstring>

//////////////////////////////////////////////////////////////////////////////		
union OW_CIMSignature
{
	OW_UInt32	val;
	char		chars[4];
};

//////////////////////////////////////////////////////////////////////////////		
// static
void
OW_CIMBase::readSig( istream& istr, const char* const sig )
{
	OW_CIMSignature expected, read;
	OW_ASSERT( strlen(sig) == 4 );

	memcpy(expected.chars, sig, sizeof(expected.chars));

	OW_BinIfcIO::read(istr, read.chars, sizeof(read.val));

	if(expected.val != read.val)
	{
		OW_THROW(OW_IOException,
			format("Signature does not match. In OW_CIMBase::readSig. "
				"signature read: %1, expected: %2",
				// use the special OW_String constructor because the chars
				// aren't null terminated.
				OW_String(read.chars, sizeof(read.chars)), sig).c_str() );
	}
}




//////////////////////////////////////////////////////////////////////////////		
// static
void OW_CIMBase::writeSig( ostream& ostr, const char* const sig )
{
	OW_ASSERT(strlen(sig) == 4);
	OW_BinIfcIO::write(ostr, sig, 4);
}


