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

#ifndef OW_BinarySerialization_HPP_
#define OW_BinarySerialization_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
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
#include "OW_ByteSwap.hpp"

#if defined(OW_HAVE_OSTREAM) && defined(OW_HAVE_ISTREAM)
#include <ostream>
#include <istream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

// Values for local API calls
// This should be kept in sync with the repository version in HDBCommon.hpp
// The general idea is to have it be a concatenation of release numbers with
// a revision on the end (to prevent problems during development)
// So 3000003 originated from version 3.0.0 rev 4
//
// 8/21/2003 - Changed from 3000003 to 3000004 because of a change in the
//   structure of CIMInstance.  The name and alias were removed.
// 10/25/2003 - 3000005. Changed enumClassNames to send over an enum of strings
//   instead of object paths.
// 2/6/2004 - 3000006. Changed CIMDateTime::{read,write}Object to write each
//   item individually so struct packing doesn't cause incompatibilities.
// 2/20/2004 - 3000007. Changed to not send an extra unnecessary signature,
//   since all the CIM objects already send a signature.  Note that the 
//   repository version was not incremented.
const UInt32 BinaryProtocolVersion = 3000007;
// These values are all used by the binary protocol
const UInt8 BIN_OK =				0;		// Success returned from server
const UInt8 BIN_ERROR =			1;		// Error returned from server
const UInt8 BIN_EXCEPTION =		2;		// CIM Exception returned from server
const UInt8 IPC_AUTHENTICATE =	10;	// Authenticate
const UInt8 IPC_FUNCTIONCALL =	11;	// Regular function call
const UInt8 IPC_CLOSECONN =		12;	// Close connection
const UInt8 BIN_DELETECLS =		20;		// Delete class
const UInt8 BIN_DELETEINST =		21;		// Delete instance
const UInt8 BIN_DELETEQUAL =		22;		// Delete qualifier type
const UInt8 BIN_ENUMCLSS =		23;		// Enum class
const UInt8 BIN_ENUMCLSNAMES =	24;		// Enum class names
const UInt8 BIN_ENUMINSTS =		25;		// Enum instances
const UInt8 BIN_ENUMINSTNAMES =	26;		// Enum instance names
const UInt8 BIN_ENUMQUALS =		27;		// Enum qualifiers types
const UInt8 BIN_GETCLS =			28;		// Get class
const UInt8 BIN_GETINST =			29;		// Get instance
const UInt8 BIN_INVMETH =			30;		// Invoke method
const UInt8 BIN_GETQUAL =			31;		// Get qualifier type
const UInt8 BIN_SETQUAL =			32;		// Set qualifier type
const UInt8 BIN_MODIFYCLS =		33;		// Modify class
const UInt8 BIN_CREATECLS =		34;		// Create class
const UInt8 BIN_MODIFYINST =		35;		// Modify instances
const UInt8 BIN_CREATEINST =		36;		// Create instance
const UInt8 BIN_GETPROP =			37;		// Get property
const UInt8 BIN_SETPROP =			38;		// Set property
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const UInt8 BIN_ASSOCNAMES =		39;		// Associator names
const UInt8 BIN_ASSOCIATORS =		40;		// Associators
const UInt8 BIN_REFNAMES =		41;		// Reference names
const UInt8 BIN_REFERENCES =		42;		// References
#endif
const UInt8 BIN_EXECQUERY =		43;		// Execute query
const UInt8 BIN_GETSVRFEATURES =	44;		// Get Server Features
const UInt8 BINSIG_NS	=			100;
const UInt8 BINSIG_OP =			101;
const UInt8 BINSIG_CLS =			102;
const UInt8 BINSIG_INST =			103;
const UInt8 BINSIG_BOOL =			104;
const UInt8 BINSIG_CLSENUM =		105;
const UInt8 BINSIG_STR =			106;
const UInt8 BINSIG_STRARRAY =		107;
const UInt8 BINSIG_QUAL_TYPE =			108;
const UInt8 BINSIG_VALUE =		109;
const UInt8 BINSIG_OPENUM =		110;
const UInt8 BINSIG_INSTENUM =		111;
const UInt8 BINSIG_QUAL_TYPEENUM =		112;
const UInt8 BINSIG_VALUEARRAY =	113;
const UInt8 BINSIG_PARAMVALUEARRAY =	114;
const UInt8 BINSIG_STRINGENUM =	115;
const UInt8 END_CLSENUM =			150;
const UInt8 END_OPENUM =			151;
const UInt8 END_INSTENUM =		152;
const UInt8 END_QUALENUM =		153;
const UInt8 END_STRINGENUM =		154;
//////////////////////////////////////////////////////////////////////////////
namespace BinarySerialization
{
	void write(std::ostream& ostrm, const void* dataOut,
		int dataOutLen);
	void verifySignature(std::istream& istrm, UInt8 validSig);
	inline void write(std::ostream& ostrm, Int32 val)
	{
		val = hton32(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}
	inline void write(std::ostream& ostrm, UInt32 val)
	{
		val = hton32(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}
	void writeLen(std::ostream& ostrm, UInt32 len);
	inline void write(std::ostream& ostrm, UInt8 val)
	{
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}
	inline void write(std::ostream& ostrm, UInt16 val)
	{
		val = hton16(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}
	inline void write(std::ostream& ostrm, Int16 val)
	{
		val = hton16(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}
	inline void write(std::ostream& ostrm, const String& str)
	{
		str.writeObject(ostrm);
	}
	inline void writeObject(std::ostream& ostrm, UInt8 sig,
		const CIMBase& obj)
	{
		obj.writeObject(ostrm);
	}
	inline void writeObjectPath(std::ostream& ostrm,
		const CIMObjectPath& op)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_OP, op);
	}
	inline void writeNameSpace(std::ostream& ostrm,
		const CIMNameSpace& ns)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_NS, ns);
	}
	inline void writeBool(std::ostream& ostrm,
		Bool arg)
	{
		BinarySerialization::write(ostrm, BINSIG_BOOL);
		arg.writeObject(ostrm);
	}
	inline void writeClass(std::ostream& ostrm, const CIMClass& cc)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_CLS, cc);
	}
	inline void writeInstance(std::ostream& ostrm, const CIMInstance& ci)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_INST, ci);
	}
	inline void writeQualType(std::ostream& ostrm, const CIMQualifierType& qt)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_QUAL_TYPE, qt);
	}
	inline void writeValue(std::ostream& ostrm, const CIMValue& value)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_VALUE, value);
	}
	inline void writeString(std::ostream& ostrm, const String& str)
	{
		BinarySerialization::write(ostrm, BINSIG_STR);
		str.writeObject(ostrm);
	}
	void readLen(std::istream& istrm, UInt32& len);
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline void
	readArray(std::istream& istr, Array<T>& a)
	{
		a.clear();
		UInt32 len;
		BinarySerialization::readLen(istr, len);
		
		a.reserve(len);
		for(UInt32 i = 0; i < len; i++)
		{
			T x;
			x.readObject(istr);
			a.push_back(x);
		}
	
	}
	
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline void
	writeArray(std::ostream& ostrm, const T& a)
	{
		UInt32 len = static_cast<UInt32>(a.size());
		BinarySerialization::writeLen(ostrm, len);
		for(UInt32 i = 0; i < len; i++)
		{
			a.operator[](i).writeObject(ostrm);
		}
	}
	inline void writeStringArray(std::ostream& ostrm,
		const StringArray& stra)
	{
		BinarySerialization::write(ostrm, BINSIG_STRARRAY);
		writeArray(ostrm, stra);
	}
	void writeStringArray(std::ostream& ostrm,
		const StringArray* propertyList);
	
	
	void read(std::istream& istrm, void* dataIn, int dataInLen);
	inline void read(std::istream& istrm, String& arg)
	{
		arg.readObject(istrm);
	}
	inline void read(std::istream& istrm, Int32& val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = ntoh32(val);
	}
	inline void read(std::istream& istrm, UInt32& val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = ntoh32(val);
	}
	inline void read(std::istream& istrm, UInt16& val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = ntoh16(val);
	}
	inline void read(std::istream& istrm, Int16& val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = ntoh16(val);
	}
	inline void read(std::istream& istrm, UInt8& val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
	}
	inline void readObject(std::istream& istrm, UInt8 validSig,
		CIMBase& obj)
	{
		obj.readObject(istrm);
	}
	inline CIMObjectPath readObjectPath(std::istream& istrm)
	{
		CIMObjectPath op(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_OP, op);
		return op;
	}
	inline CIMInstance readInstance(std::istream& istrm)
	{
		CIMInstance ci(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_INST, ci);
		return ci;
	}
	inline Bool readBool(std::istream& istrm)
	{
		BinarySerialization::verifySignature(istrm, BINSIG_BOOL);
		Bool b;
		b.readObject(istrm);
		return b;
	}
	inline CIMNameSpace readNameSpace(std::istream& istrm)
	{
		CIMNameSpace ns(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_NS, ns);
		return ns;
	}
	inline CIMClass readClass(std::istream& istrm)
	{
		CIMClass cc(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_CLS, cc);
		return cc;
	}
	inline String readString(std::istream& istrm)
	{
		BinarySerialization::verifySignature(istrm, BINSIG_STR);
		String rv;
		rv.readObject(istrm);
		return rv;
	}
	inline CIMQualifierType readQualType(std::istream& istrm)
	{
		CIMQualifierType qt(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_QUAL_TYPE, qt);
		return qt;
	}
	inline CIMValue readValue(std::istream& istrm)
	{
		CIMValue value(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_VALUE, value);
		return value;
	}
	inline StringArray readStringArray(std::istream& istrm)
	{
		BinarySerialization::verifySignature(istrm, BINSIG_STRARRAY);
		StringArray stra;
		readArray(istrm, stra);
		return stra;
	}
	void readObjectPathEnum(std::istream& istrm, CIMObjectPathResultHandlerIFC& result);
	void readClassEnum(std::istream& istrm, CIMClassResultHandlerIFC& result);
	void readInstanceEnum(std::istream& istrm, CIMInstanceResultHandlerIFC& result);
	void readQualifierTypeEnum(std::istream& istrm, CIMQualifierTypeResultHandlerIFC& result);
	void readStringEnum(std::istream& istrm, StringResultHandlerIFC& result);
} // end namespace BinarySerialization

} // end namespace OpenWBEM

#endif	// OW_BinarySerialization_HPP_
