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
#ifndef OW_BINIFCIO_HPP_
#define OW_BINIFCIO_HPP_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_CIMFwd.hpp"
#include "OW_Bool.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMClass.hpp"
#include "OW_Format.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMValue.hpp"

#if defined(OW_HAVE_OSTREAM) && defined(OW_HAVE_ISTREAM)
#include <ostream>
#include <istream>
#else
#include <iostream>
#endif


// Values for local API calls
const OW_UInt32 OW_BinaryProtocolVersion = 3000001;

const OW_UInt8 OW_BIN_OK =				0;		// Success returned from server
const OW_UInt8 OW_BIN_ERROR =			1;		// Error returned from server
const OW_UInt8 OW_BIN_EXCEPTION =		2;		// CIM Exception returned from server

const OW_UInt8 OW_IPC_AUTHENTICATE =	10;	// Authenticate
const OW_UInt8 OW_IPC_FUNCTIONCALL =	11;	// Regular function call
const OW_UInt8 OW_IPC_CLOSECONN =		12;	// Close connection

const OW_UInt8 OW_BIN_DELETECLS =		20;		// Delete class
const OW_UInt8 OW_BIN_DELETEINST =		21;		// Delete instance
const OW_UInt8 OW_BIN_DELETEQUAL =		22;		// Delete qualifier type
const OW_UInt8 OW_BIN_ENUMCLSS =		23;		// Enum class
const OW_UInt8 OW_BIN_ENUMCLSNAMES =	24;		// Enum class names
const OW_UInt8 OW_BIN_ENUMINSTS =		25;		// Enum instances
const OW_UInt8 OW_BIN_ENUMINSTNAMES =	26;		// Enum instance names
const OW_UInt8 OW_BIN_ENUMQUALS =		27;		// Enum qualifiers types
const OW_UInt8 OW_BIN_GETCLS =			28;		// Get class
const OW_UInt8 OW_BIN_GETINST =			29;		// Get instance
const OW_UInt8 OW_BIN_INVMETH =			30;		// Invoke method
const OW_UInt8 OW_BIN_GETQUAL =			31;		// Get qualifier type
const OW_UInt8 OW_BIN_SETQUAL =			32;		// Set qualifier type
const OW_UInt8 OW_BIN_MODIFYCLS =		33;		// Modify class
const OW_UInt8 OW_BIN_CREATECLS =		34;		// Create class
const OW_UInt8 OW_BIN_MODIFYINST =		35;		// Modify instances
const OW_UInt8 OW_BIN_CREATEINST =		36;		// Create instance
const OW_UInt8 OW_BIN_GETPROP =			37;		// Get property
const OW_UInt8 OW_BIN_SETPROP =			38;		// Set property
const OW_UInt8 OW_BIN_ASSOCNAMES =		39;		// Associator names
const OW_UInt8 OW_BIN_ASSOCIATORS =		40;		// Associators
const OW_UInt8 OW_BIN_REFNAMES =		41;		// Reference names
const OW_UInt8 OW_BIN_REFERENCES =		42;		// References
const OW_UInt8 OW_BIN_EXECQUERY =		43;		// Execute query
const OW_UInt8 OW_BIN_GETSVRFEATURES =	44;		// Get Server Features

const OW_UInt8 OW_BINSIG_NS	=			100;
const OW_UInt8 OW_BINSIG_OP =			101;
const OW_UInt8 OW_BINSIG_CLS =			102;
const OW_UInt8 OW_BINSIG_INST =			103;
const OW_UInt8 OW_BINSIG_BOOL =			104;
const OW_UInt8 OW_BINSIG_CLSENUM =		105;
const OW_UInt8 OW_BINSIG_STR =			106;
const OW_UInt8 OW_BINSIG_STRARRAY =		107;
const OW_UInt8 OW_BINSIG_QUAL =			108;
const OW_UInt8 OW_BINSIG_VALUE =		109;
const OW_UInt8 OW_BINSIG_OPENUM =		110;
const OW_UInt8 OW_BINSIG_INSTENUM =		111;
const OW_UInt8 OW_BINSIG_QUALENUM =		112;
const OW_UInt8 OW_BINSIG_VALUEARRAY =	113;
const OW_UInt8 OW_BINSIG_PARAMVALUEARRAY =	114;

const OW_UInt8 OW_END_CLSENUM =			150;
const OW_UInt8 OW_END_OPENUM =			151;
const OW_UInt8 OW_END_INSTENUM =		152;
const OW_UInt8 OW_END_QUALENUM =		153;


//////////////////////////////////////////////////////////////////////////////
// TODO: Refactor this and put these in a more common place, since they're used in more than just the binary protocol
class OW_BinIfcIO
{
public:

	static void write(std::ostream& ostrm, const void* dataOut,
		int dataOutLen)
	{
		if(!ostrm.write(reinterpret_cast<const char*>(dataOut), dataOutLen))
		{
			OW_THROW(OW_IOException, "Failed writing data");
		}
	}


	static void verifySignature(std::istream& istrm, OW_UInt8 validSig)
	{
		OW_UInt8 val;
		OW_BinIfcIO::read(istrm, val);
	
		if(val != validSig)
		{
			OW_THROW(OW_BadCIMSignatureException,
				format("Received invalid signature. Got: %1 Expected: %2", OW_Int32(val),
					OW_Int32(validSig)).c_str());
		}
	
	}


	static void write(std::ostream& ostrm, OW_Int32 val)
	{
		val = OW_hton32(val);
		OW_BinIfcIO::write(ostrm, &val, sizeof(val));
	}

	static void write(std::ostream& ostrm, OW_UInt32 val)
	{
		val = OW_hton32(val);
		OW_BinIfcIO::write(ostrm, &val, sizeof(val));
	}

	static void writeLen(std::ostream& ostrm, OW_UInt32 len);

	static void write(std::ostream& ostrm, OW_UInt8 val)
	{
		OW_BinIfcIO::write(ostrm, &val, sizeof(val));
	}

	static void write(std::ostream& ostrm, OW_UInt16 val)
	{
		val = OW_hton16(val);
		OW_BinIfcIO::write(ostrm, &val, sizeof(val));
	}


	static void write(std::ostream& ostrm, const OW_String& str)
	{
		str.writeObject(ostrm);
	}


	static void writeObject(std::ostream& ostrm, OW_UInt8 sig,
		const OW_CIMBase& obj)
	{
		OW_BinIfcIO::write(ostrm, sig);
		obj.writeObject(ostrm);
	}


	static void writeObjectPath(std::ostream& ostrm,
		const OW_CIMObjectPath& op)
	{
		OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_OP, op);
	}


	static void writeNameSpace(std::ostream& ostrm,
		const OW_CIMNameSpace& ns)
	{
		OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_NS, ns);
	}


	static void writeBool(std::ostream& ostrm,
		OW_Bool arg)
	{
		OW_BinIfcIO::write(ostrm, OW_BINSIG_BOOL);
		arg.writeObject(ostrm);
	}


	static void writeClass(std::ostream& ostrm, const OW_CIMClass& cc)
	{
		OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_CLS, cc);
	}


	static void writeInstance(std::ostream& ostrm, const OW_CIMInstance& ci)
	{
		OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_INST, ci);
	}


	static void writeQual(std::ostream& ostrm, const OW_CIMQualifierType& qt)
	{
		OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_QUAL, qt);
	}


	static void writeValue(std::ostream& ostrm, const OW_CIMValue& value)
	{
		OW_BinIfcIO::writeObject(ostrm, OW_BINSIG_VALUE, value);
	}


	static void writeString(std::ostream& ostrm, const OW_String& str)
	{
		OW_BinIfcIO::write(ostrm, OW_BINSIG_STR);
		str.writeObject(ostrm);
	}


	static void writeStringArray(std::ostream& ostrm,
		const OW_StringArray& stra)
	{
		OW_BinIfcIO::write(ostrm, OW_BINSIG_STRARRAY);
		stra.writeObject(ostrm);
	}

	static void writeStringArray(std::ostream& ostrm,
		const OW_StringArray* propertyList)
	{
		OW_Bool nullPropertyList = (propertyList == 0);
		OW_BinIfcIO::writeBool(ostrm, nullPropertyList);
		if(!nullPropertyList)
		{
			OW_BinIfcIO::writeStringArray(ostrm, *propertyList);
		}
	}
	
	
	static void read(std::istream& istrm, void* dataIn, int dataInLen)
	{
		if(!istrm.read(reinterpret_cast<char*>(dataIn), dataInLen))
		{
			OW_THROW(OW_IOException, "Failed reading data");
		}
	}


	static void read(std::istream& istrm, OW_String& arg)
	{
		arg.readObject(istrm);
	}


	static void read(std::istream& istrm, OW_Int32& val)
	{
		OW_BinIfcIO::read(istrm, &val, sizeof(val));
		val = OW_ntoh32(val);
	}

	static void read(std::istream& istrm, OW_UInt32& val)
	{
		OW_BinIfcIO::read(istrm, &val, sizeof(val));
		val = OW_ntoh32(val);
	}

	static void readLen(std::istream& istrm, OW_UInt32& len);

	static void read(std::istream& istrm, OW_UInt16& val)
	{
		OW_BinIfcIO::read(istrm, &val, sizeof(val));
		val = OW_ntoh16(val);
	}

	static void read(std::istream& istrm, OW_UInt8& val)
	{
		OW_BinIfcIO::read(istrm, &val, sizeof(val));
	}


	static void readObject(std::istream& istrm, OW_UInt8 validSig,
		OW_CIMBase& obj)
	{
		OW_BinIfcIO::verifySignature(istrm, validSig);
		obj.readObject(istrm);
	}


	static OW_CIMObjectPath readObjectPath(std::istream& istrm)
	{
		OW_CIMObjectPath op(OW_CIMNULL);
		OW_BinIfcIO::readObject(istrm, OW_BINSIG_OP, op);
		return op;
	}


	static OW_CIMInstance readInstance(std::istream& istrm)
	{
		OW_CIMInstance ci(OW_CIMNULL);
		OW_BinIfcIO::readObject(istrm, OW_BINSIG_INST, ci);
		return ci;
	}


	static OW_Bool readBool(std::istream& istrm)
	{
		OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_BOOL);
		OW_Bool b;
		b.readObject(istrm);
		return b;
	}


	static OW_CIMNameSpace readNameSpace(std::istream& istrm)
	{
		OW_CIMNameSpace ns(OW_CIMNULL);
		OW_BinIfcIO::readObject(istrm, OW_BINSIG_NS, ns);
		return ns;
	}


	static OW_CIMClass readClass(std::istream& istrm)
	{
		OW_CIMClass cc(OW_CIMNULL);
		OW_BinIfcIO::readObject(istrm, OW_BINSIG_CLS, cc);
		return cc;
	}


	static OW_String readString(std::istream& istrm)
	{
		OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_STR);
		OW_String rv;
		rv.readObject(istrm);
		return rv;
	}


	static OW_CIMQualifierType readQual(std::istream& istrm)
	{
		OW_CIMQualifierType qt(OW_CIMNULL);
		OW_BinIfcIO::readObject(istrm, OW_BINSIG_QUAL, qt);
		return qt;
	}


	static OW_CIMValue readValue(std::istream& istrm)
	{
		OW_CIMValue value(OW_CIMNULL);
		OW_BinIfcIO::readObject(istrm, OW_BINSIG_VALUE, value);
		return value;
	}


	static OW_StringArray readStringArray(std::istream& istrm)
	{
		OW_BinIfcIO::verifySignature(istrm, OW_BINSIG_STRARRAY);
		OW_StringArray stra;
		stra.readObject(istrm);
		return stra;
	}


	static void readObjectPathEnum(std::istream& istrm, OW_CIMObjectPathResultHandlerIFC& result);

	static void readClassEnum(std::istream& istrm, OW_CIMClassResultHandlerIFC& result);

	static void readInstanceEnum(std::istream& istrm, OW_CIMInstanceResultHandlerIFC& result);

	static void readQualifierTypeEnum(std::istream& istrm, OW_CIMQualifierTypeResultHandlerIFC& result);


};


#endif	// OW_BINIFCIO_HPP_

