/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "OW_BinarySerialization.hpp"

#include <cstring>

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////		
// static
void
OW_CIMBase::readSig( istream& istr, const char* const sig )
{
	char expected, read;
	OW_ASSERT( strlen(sig) == 1 );

	expected = sig[0];
	OW_BinarySerialization::read(istr, &read, sizeof(read));

	if(expected != read)
	{
		OW_THROW(OW_BadCIMSignatureException,
			format("Signature does not match. In OW_CIMBase::readSig. "
				"signature read: %1, expected: %2",
				read, sig).c_str() );
	}
}




//////////////////////////////////////////////////////////////////////////////		
// static
void OW_CIMBase::writeSig( ostream& ostr, const char* const sig )
{
	OW_ASSERT(strlen(sig) == 1);
	OW_BinarySerialization::write(ostr, sig, 1);
}


//////////////////////////////////////////////////////////////////////////////		
std::ostream& operator<<(std::ostream& ostr, const OW_CIMBase& cb)
{
	ostr << cb.toString();
	return ostr;
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMBase::~OW_CIMBase() 
{
}

