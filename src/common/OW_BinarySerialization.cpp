/*******************************************************************************
* Copyright (C) 2001-3 Center 7, Inc All rights reserved.
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
#include "OW_BinarySerialization.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_CIMBase.hpp"

namespace OpenWBEM
{

namespace BinarySerialization
{
//////////////////////////////////////////////////////////////////////////////
template <typename Handler, typename ReaderFunc>
static inline void readEnum(std::istream& istrm, Handler& result,
	const ReaderFunc& read, const Int32 beginsig, const Int32 endsig)
{
	verifySignature(istrm, beginsig);
	bool done = false;
	while (!done)
	{
		try
		{
			result.handle(read(istrm));
		}
		catch (const BadCIMSignatureException& e)
		{
			// read threw because we've read all the objects
			verifySignature(istrm, endsig);
			done = true;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
readObjectPathEnum(std::istream& istrm, CIMObjectPathResultHandlerIFC& result)
{
	readEnum(istrm, result, &readObjectPath, BINSIG_OPENUM, END_OPENUM);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
readClassEnum(std::istream& istrm, CIMClassResultHandlerIFC& result)
{
	readEnum(istrm, result, &readClass, BINSIG_CLSENUM, END_CLSENUM);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
readInstanceEnum(std::istream& istrm, CIMInstanceResultHandlerIFC& result)
{
	readEnum(istrm, result, &readInstance, BINSIG_INSTENUM, END_INSTENUM);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
readQualifierTypeEnum(std::istream& istrm, CIMQualifierTypeResultHandlerIFC& result)
{
	readEnum(istrm, result, &readQual, BINSIG_QUALENUM, END_QUALENUM);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
readStringEnum(std::istream& istrm, StringResultHandlerIFC& result)
{
	readEnum(istrm, result, &readString, BINSIG_STRINGENUM, END_STRINGENUM);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
writeLen(std::ostream& ostrm, UInt32 len)
{
	// This is ASN.1 length encoding
	/*
	 * short len if it's less than 128 - one byte giving the len,
	 * with bit 8 0.
	 */
	if ( len <= 127 )
	{
		UInt8 length_byte = static_cast<UInt8>(len);
		write(ostrm, &length_byte, 1);
		return;
	}
	/*
	 * long len otherwise - one byte with bit 8 set, giving the
	 * length of the length, followed by the length itself.
	 */
	/* find the first non-all-zero byte */
	UInt8 lenlen;
	if (len <= 255)
	{
		lenlen = 1;
	}
	else if (len <= 65536)
	{
		lenlen = 2;
	}
	else if (len <= 16777216)
	{
		lenlen = 3;
	}
	else
	{
		lenlen = 4;
	}
	UInt8 netlenlen = lenlen | 0x80UL;
	/* write the length of the length */
	write(ostrm, &netlenlen, 1);
	UInt8 netlen[sizeof(len)];
	for (int j = 0; j < lenlen; j++)
	{
		netlen[(sizeof(len)-1) - j] = static_cast<UInt8>(len & 0xffU);
		len >>= 8;
	}
	/* write the length itself */
	write(ostrm, static_cast<void *>(&netlen[sizeof(len)-lenlen]), lenlen);
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
readLen(std::istream& istrm, UInt32& len)
{
	// This is ASN.1 length encoding
	UInt8 lc;
	read(istrm, lc);
	if (lc & 0x80U) 
	{
		UInt8 noctets = lc & 0x7fU;
		if ( noctets > sizeof(len) ) {
			OW_THROW(IOException, "Failed reading data: length length is too large");
		}
		UInt8 netlen[sizeof(len)];
		read(istrm, static_cast<void *>(netlen), noctets);
		len = 0;
		for(int i = 0; i < noctets; i++ ) {
			len <<= 8;
			len |= netlen[i];
		}
	} else {
		len = lc;
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
write(std::ostream& ostrm, const void* dataOut,
	int dataOutLen)
{
	if(!ostrm.write(reinterpret_cast<const char*>(dataOut), dataOutLen))
	{
		OW_THROW(IOException, "Failed writing data");
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
verifySignature(std::istream& istrm, UInt8 validSig)
{
	UInt8 val;
	read(istrm, val);
	if(val != validSig)
	{
		OW_THROW(BadCIMSignatureException,
			format("Received invalid signature. Got: %1 Expected: %2", Int32(val),
				Int32(validSig)).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
writeStringArray(std::ostream& ostrm,
	const StringArray* propertyList)
{
	bool nullPropertyList = (propertyList == 0);
	writeBool(ostrm, nullPropertyList);
	if(!nullPropertyList)
	{
		writeStringArray(ostrm, *propertyList);
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
read(std::istream& istrm, void* dataIn, int dataInLen)
{
	if(!istrm.read(reinterpret_cast<char*>(dataIn), dataInLen))
	{
		OW_THROW(IOException, "Failed reading data");
	}
}
}

} // end namespace OpenWBEM

