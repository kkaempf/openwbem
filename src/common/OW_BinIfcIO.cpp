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
#include "OW_IOException.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Format.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_CIMBase.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::write(std::ostream& ostrm, const void* dataOut,
	int dataOutLen)
{
	if(!ostrm.write(reinterpret_cast<const char*>(dataOut), dataOutLen))
	{
		OW_THROW(OW_IOException, "Failed writing data to IPC connection");
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::verifySignature(std::istream& istrm, OW_Int32 validSig)
{
	OW_Int32 val;
	OW_BinIfcIO::read(istrm, val);

	if(val != validSig)
	{
		OW_THROW(OW_IOException,
			format("Received invalid signature. Got: %1  Expected: %2", val,
				validSig).c_str());
	}

}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::write(std::ostream& ostrm, OW_Int32 val)
{
	val = OW_hton32(val);
	OW_BinIfcIO::write(ostrm, (const void*)&val, sizeof(val));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::write(std::ostream& ostrm, const char* str)
{
	OW_Int32 len = ::strlen(str);
	OW_BinIfcIO::write(ostrm, len);
	OW_BinIfcIO::write(ostrm, (const void*)str, len);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::write(std::ostream& ostrm, const OW_String& str)
{
	OW_BinIfcIO::write(ostrm, str.c_str());
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeObject(std::ostream& ostrm, OW_Int32 sig,
	const OW_CIMBase& obj)
{
	OW_BinIfcIO::write(ostrm, sig);
	obj.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeObjectPath(std::ostream& ostrm, const OW_CIMObjectPath& op)
{
	OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_OP, op);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeNameSpace(std::ostream& ostrm, const OW_CIMNameSpace& ns)
{
	OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_NS, ns);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeBool(std::ostream& ostrm, OW_Bool arg)
{
	OW_BinIfcIO::write(ostrm, OW_BINSIG_BOOL);
	arg.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeClass(std::ostream& ostrm, const OW_CIMClass& cc)
{
	OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_CLS, cc);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeInstance(std::ostream& ostrm, const OW_CIMInstance& ci)
{
	OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_INST, ci);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeQual(std::ostream& ostrm, const OW_CIMQualifierType& qt)
{
	OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_QUAL, qt);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeValue(std::ostream& ostrm, const OW_CIMValue& value)
{
	OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_VALUE, value);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeString(std::ostream& ostrm, const OW_String& str)
{
	OW_BinIfcIO::write(ostrm, OW_BINSIG_STR);
	str.writeObject(ostrm);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::writeStringArray(std::ostream& ostrm, const OW_StringArray& stra)
{
	OW_BinIfcIO::write(ostrm, OW_BINSIG_STRARRAY);
	OW_BinIfcIO::write(ostrm, OW_Int32(stra.size()));
	for(size_t i = 0; i < stra.size(); i++)
	{
		OW_BinIfcIO::writeString(ostrm, stra[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::read(std::istream& istrm, void* dataIn, int dataInLen)
{
	if(!istrm.read(reinterpret_cast<char*>(dataIn), dataInLen))
	{
		OW_THROW(OW_IOException, "Failed reading data from IPC connection");
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::read(std::istream& istrm, OW_String& arg)
{
	OW_Int32 len;
	OW_BinIfcIO::read(istrm, len);

	OW_AutoPtrVec<char> bfr(new char[len+1]);
	OW_BinIfcIO::read(istrm, (void*)bfr.get(), len);
	bfr.get()[len] = 0;
	arg = OW_String(OW_Bool(true), bfr.release());
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::read(std::istream& istrm, OW_Int32& val)
{
	OW_BinIfcIO::read(istrm, (void*)&val, sizeof(val));
	val = OW_ntoh32(val);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_BinIfcIO::readObject(std::istream& istrm, OW_Int32 validSig, OW_CIMBase& obj)
{
	OW_BinIfcIO::verifySignature(istrm, validSig);
	obj.readObject(istrm);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMObjectPath
OW_BinIfcIO::readObjectPath(std::istream& istrm)
{
	OW_CIMObjectPath op;
	OW_BinIfcIO::readObject(istrm, OW_BINSIG_OP, op);
	return op;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMInstance
OW_BinIfcIO::readInstance(std::istream& istrm)
{
	OW_CIMInstance ci;
	OW_BinIfcIO::readObject(istrm, OW_BINSIG_INST, ci);
	return ci;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_BinIfcIO::readBool(std::istream& istrm)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_BOOL);
	OW_Bool b;
	b.readObject(istrm);
	return b;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMNameSpace
OW_BinIfcIO::readNameSpace(std::istream& istrm)
{
	OW_CIMNameSpace ns;
	OW_BinIfcIO::readObject(istrm, OW_BINSIG_NS, ns);
	return ns;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMClass
OW_BinIfcIO::readClass(std::istream& istrm)
{
	OW_CIMClass cc;
	OW_BinIfcIO::readObject(istrm, OW_BINSIG_CLS, cc);
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_String
OW_BinIfcIO::readString(std::istream& istrm)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_STR);
	OW_String rv;
	rv.readObject(istrm);
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMQualifierType
OW_BinIfcIO::readQual(std::istream& istrm)
{
	OW_CIMQualifierType qt;
	OW_BinIfcIO::readObject(istrm, OW_BINSIG_QUAL, qt);
	return qt;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMValue
OW_BinIfcIO::readValue(std::istream& istrm)
{
	OW_CIMValue value;
	OW_BinIfcIO::readObject(istrm, OW_BINSIG_VALUE, value);
	return value;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_StringArray
OW_BinIfcIO::readStringArray(std::istream& istrm)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_STRARRAY);
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size);

	OW_StringArray stra;
	while(size)
	{
		stra.append(OW_BinIfcIO::readString(istrm));
		size--;
	}

	return stra;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMObjectPathEnumeration
OW_BinIfcIO::readObjectPathEnum(std::istream& istrm)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_OPENUM);
	OW_CIMObjectPathEnumeration en;
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size);

	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMObjectPathEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readObjectPath(istrm));
			size--;
		}
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMClassEnumeration
OW_BinIfcIO::readClassEnum(std::istream& istrm)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_CLSENUM);
	OW_CIMClassEnumeration en;
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size);
	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMClassEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readClass(istrm));
			size--;
		}
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMInstanceEnumeration
OW_BinIfcIO::readInstanceEnum(std::istream& istrm)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_INSTENUM);
	OW_CIMInstanceEnumeration en;
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size);
	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMInstanceEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readInstance(istrm));
			size--;
		}
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_CIMQualifierTypeEnumeration
OW_BinIfcIO::readQualifierTypeEnum(std::istream& istrm)
{
	OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_QUALENUM);
	OW_CIMQualifierTypeEnumeration en;
	OW_Int32 size;
	OW_BinIfcIO::read(istrm, size);
	if(size == -1)
	{
		OW_String fname = OW_BinIfcIO::readString(istrm);
		en = OW_CIMQualifierTypeEnumeration(fname);
	}
	else
	{
		while(size)
		{
			en.addElement(OW_BinIfcIO::readQual(istrm));
			size--;
		}
	}
	return en;
}


