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
#include "OW_BinIfcIO.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readObjectPathEnum(std::istream& istrm, OW_CIMObjectPathResultHandlerIFC& result)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_OPENUM);
	bool done = false;
	while (!done)
	{
		try
		{
			result.handleObjectPath(OW_BinIfcIO::readObjectPath(istrm));
		}
		catch (const OW_BadCIMSignatureException& e)
		{
			// OW_CIMObjectPath::readObject threw because we've read all the classes
			OW_BinIfcIO::verifySignature(istrm, OW_END_OPENUM);
			done = true;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readClassEnum(std::istream& istrm, OW_CIMClassResultHandlerIFC& result)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_CLSENUM);
	bool done = false;
	while (!done)
	{
		try
		{
			result.handleClass(OW_BinIfcIO::readClass(istrm));
		}
		catch (const OW_BadCIMSignatureException& e)
		{
			// CIMClass.readObject threw because we've read all the classes
			OW_BinIfcIO::verifySignature(istrm, OW_END_CLSENUM);
			done = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readInstanceEnum(std::istream& istrm, OW_CIMInstanceResultHandlerIFC& result)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_INSTENUM);
	bool done = false;
	while (!done)
	{
		try
		{
			result.handleInstance(OW_BinIfcIO::readInstance(istrm));
		}
		catch (const OW_BadCIMSignatureException& e)
		{
			// OW_CIMInstance::readObject threw because we've read all the instances
			OW_BinIfcIO::verifySignature(istrm, OW_END_INSTENUM);
			done = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readQualifierTypeEnum(std::istream& istrm, OW_CIMQualifierTypeResultHandlerIFC& result)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_QUALENUM);
	bool done = false;
	while (!done)
	{
		try
		{
			result.handleQualifierType(OW_BinIfcIO::readQual(istrm));
		}
		catch (const OW_BadCIMSignatureException& e)
		{
			// OW_CIMInstance::readObject threw because we've read all the instances
			OW_BinIfcIO::verifySignature(istrm, OW_END_QUALENUM);
			done = true;
		}
	}
}


