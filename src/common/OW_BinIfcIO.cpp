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
#include "OW_BinIfcIO.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_CIMBase.hpp"

//////////////////////////////////////////////////////////////////////////////
template <typename Handler, typename ReaderFunc>
static inline void readEnum(std::istream& istrm, Handler& result,
	const ReaderFunc& read, const OW_Int32 beginsig, const OW_Int32 endsig)
{
	OW_BinIfcIO::verifySignature(istrm, beginsig);
	bool done = false;
	while (!done)
	{
		try
		{
			result.handle(read(istrm));
		}
		catch (const OW_BadCIMSignatureException& e)
		{
			// read threw because we've read all the objects
			OW_BinIfcIO::verifySignature(istrm, endsig);
			done = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readObjectPathEnum(std::istream& istrm, OW_CIMObjectPathResultHandlerIFC& result)
{
	readEnum(istrm, result, &OW_BinIfcIO::readObjectPath, OW_BINSIG_OPENUM, OW_END_OPENUM);
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readClassEnum(std::istream& istrm, OW_CIMClassResultHandlerIFC& result)
{
	readEnum(istrm, result, &OW_BinIfcIO::readClass, OW_BINSIG_CLSENUM, OW_END_CLSENUM);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readInstanceEnum(std::istream& istrm, OW_CIMInstanceResultHandlerIFC& result)
{
	readEnum(istrm, result, &OW_BinIfcIO::readInstance, OW_BINSIG_INSTENUM, OW_END_INSTENUM);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readQualifierTypeEnum(std::istream& istrm, OW_CIMQualifierTypeResultHandlerIFC& result)
{
	readEnum(istrm, result, &OW_BinIfcIO::readQual, OW_BINSIG_QUALENUM, OW_END_QUALENUM);
}


