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

#ifndef OW_BinarySerialization_HPP_
#define OW_BinarySerialization_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_CIMFwd.hpp"
#include "blocxx/Bool.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMValue.hpp"
#include "blocxx/ByteSwap.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"

#include <iosfwd>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

namespace BinarySerialization
{

// This should generally be kept in sync with the repository version in OW_HDBCommon.hpp
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
// 7/22/2004 - 3000008. Changed signatures and added versioning. Now all
//   readObject() calls will be able to read older versions as well as the
//   current.  Introduced MinBinaryProtocolVersion which is the oldest version
//   we can sucessfully read.
// 9/01/2005 - only HDB version to 4000000. Changed key format to use : instead of / to fix a bug.
// 10/12/2005 - only HDB version to 4000001. Fixed association and instance key format wrt associations.
// 4/17/2006 - 4000002. Added namespace field to CIMInstance.
const blocxx::UInt32 BinaryProtocolVersion = 4000002;

// This is the oldest version the code can handle.
const blocxx::UInt32 MinBinaryProtocolVersion = 3000007;

// These values are all used by the binary protocol
const blocxx::UInt8 BIN_OK =				0;		// Success returned from server
const blocxx::UInt8 BIN_ERROR =			1;		// Error returned from server
const blocxx::UInt8 BIN_EXCEPTION =		2;		// CIM Exception returned from server
const blocxx::UInt8 BIN_END =			3;	// Final sentinel

const blocxx::UInt8 IPC_AUTHENTICATE =	10;	// Authenticate
const blocxx::UInt8 IPC_FUNCTIONCALL =	11;	// Regular function call
const blocxx::UInt8 IPC_CLOSECONN =		12;	// Close connection
const blocxx::UInt8 BIN_DELETECLS =		20;		// Delete class
const blocxx::UInt8 BIN_DELETEINST =		21;		// Delete instance
const blocxx::UInt8 BIN_DELETEQUAL =		22;		// Delete qualifier type
const blocxx::UInt8 BIN_ENUMCLSS =		23;		// Enum class
const blocxx::UInt8 BIN_ENUMCLSNAMES =	24;		// Enum class names
const blocxx::UInt8 BIN_ENUMINSTS =		25;		// Enum instances
const blocxx::UInt8 BIN_ENUMINSTNAMES =	26;		// Enum instance names
const blocxx::UInt8 BIN_ENUMQUALS =		27;		// Enum qualifiers types
const blocxx::UInt8 BIN_GETCLS =			28;		// Get class
const blocxx::UInt8 BIN_GETINST =			29;		// Get instance
const blocxx::UInt8 BIN_INVMETH =			30;		// Invoke method
const blocxx::UInt8 BIN_GETQUAL =			31;		// Get qualifier type
const blocxx::UInt8 BIN_SETQUAL =			32;		// Set qualifier type
const blocxx::UInt8 BIN_MODIFYCLS =		33;		// Modify class
const blocxx::UInt8 BIN_CREATECLS =		34;		// Create class
const blocxx::UInt8 BIN_MODIFYINST =		35;		// Modify instances
const blocxx::UInt8 BIN_CREATEINST =		36;		// Create instance
const blocxx::UInt8 BIN_GETPROP =			37;		// Get property
const blocxx::UInt8 BIN_SETPROP =			38;		// Set property
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const blocxx::UInt8 BIN_ASSOCNAMES =		39;		// Associator names
const blocxx::UInt8 BIN_ASSOCIATORS =		40;		// Associators
const blocxx::UInt8 BIN_REFNAMES =		41;		// Reference names
const blocxx::UInt8 BIN_REFERENCES =		42;		// References
#endif
const blocxx::UInt8 BIN_EXECQUERY =		43;		// Execute query
const blocxx::UInt8 BIN_GETSVRFEATURES =	44;		// Get Server Features
const blocxx::UInt8 BIN_LOG_MESSAGE =		45;		// log message
const blocxx::UInt8 OPERATION_CONTEXT_GET_DATA = 46;
const blocxx::UInt8 OPERATION_CONTEXT_SET_DATA = 47;
const blocxx::UInt8 OPERATION_CONTEXT_REMOVE_DATA = 48;
const blocxx::UInt8 OPERATION_CONTEXT_KEY_HAS_DATA = 49;
const blocxx::UInt8 CIMOM_HANDLE_REQUEST = 50;
const blocxx::UInt8 REPOSITORY_CIMOM_HANDLE_REQUEST = 51;
const blocxx::UInt8 GET_CONFIG_ITEM = 52;
const blocxx::UInt8 GET_MULTI_CONFIG_ITEM = 53;
const blocxx::UInt8 POLL = 54;
const blocxx::UInt8 GET_INITIAL_POLLING_INTERVAL = 55;
const blocxx::UInt8 SET_PERSISTENT = 56;
const blocxx::UInt8 MUST_POLL = 57;
const blocxx::UInt8 AUTHORIZE_FILTER = 58;
const blocxx::UInt8 ACTIVATE_FILTER = 59;
const blocxx::UInt8 DEACTIVATE_FILTER = 60;
const blocxx::UInt8 EXPORT_INDICATION = 61;
const blocxx::UInt8 PROVIDER_ENVIRONMENT_REQUEST = 62;
const blocxx::UInt8 SHUTTING_DOWN = 63;
const blocxx::UInt8 QUERY_INSTANCES = 64;
const blocxx::UInt8 OPERATION_CONTEXT_GET_OPERATION_ID = 65;
const blocxx::UInt8 SET_LOG_LEVEL = 66;
const blocxx::UInt8 LOCKED_CIMOM_HANDLE_REQUEST = 67;


const blocxx::UInt8 BINSIG_NS	=			100;
const blocxx::UInt8 BINSIG_OP =			101;
const blocxx::UInt8 BINSIG_CLS =			102;
const blocxx::UInt8 BINSIG_INST =			103;
const blocxx::UInt8 BINSIG_BOOL =			104;
const blocxx::UInt8 BINSIG_CLSENUM =		105;
const blocxx::UInt8 BINSIG_STR =			106;
const blocxx::UInt8 BINSIG_STRARRAY =		107;
const blocxx::UInt8 BINSIG_QUAL_TYPE =			108;
const blocxx::UInt8 BINSIG_VALUE =		109;
const blocxx::UInt8 BINSIG_OPENUM =		110;
const blocxx::UInt8 BINSIG_INSTENUM =		111;
const blocxx::UInt8 BINSIG_QUAL_TYPEENUM =		112;
const blocxx::UInt8 BINSIG_VALUEARRAY =	113;
const blocxx::UInt8 BINSIG_PARAMVALUEARRAY =	114;
const blocxx::UInt8 BINSIG_STRINGENUM =	115;
const blocxx::UInt8 BINSIG_WQL_SELECT_STATEMENT = 116;

const blocxx::UInt8 END_CLSENUM =			150;
const blocxx::UInt8 END_OPENUM =			151;
const blocxx::UInt8 END_INSTENUM =		152;
const blocxx::UInt8 END_QUALENUM =		153;
const blocxx::UInt8 END_STRINGENUM =		154;

//////////////////////////////////////////////////////////////////////////////
	OW_COMMON_API void write(
		std::streambuf & ostrm, const void * dataOut, size_t dataOutLen
	);

	OW_COMMON_API void verifySignature(std::streambuf & istrm, blocxx::UInt8 validSig);

	inline void write(std::streambuf & ostrm, blocxx::Int32 val)
	{
		val = blocxx::hton32(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}

	inline void write(std::streambuf & ostrm, blocxx::UInt32 val)
	{
		val = blocxx::hton32(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}

	OW_COMMON_API void writeLen(std::streambuf & ostrm, blocxx::UInt32 len);

	inline void write(std::streambuf & ostrm, blocxx::UInt8 val)
	{
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}

	inline void write(std::streambuf & ostrm, blocxx::UInt16 val)
	{
		val = blocxx::hton16(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}

	inline void write(std::streambuf & ostrm, blocxx::Int16 val)
	{
		val = blocxx::hton16(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}

	inline void write(std::streambuf & ostrm, blocxx::UInt64 val)
	{
		val = blocxx::hton64(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}

	inline void write(std::streambuf & ostrm, blocxx::Int64 val)
	{
		val = blocxx::hton64(val);
		BinarySerialization::write(ostrm, &val, sizeof(val));
	}

	inline void write(std::streambuf & ostrm, const blocxx::String & str)
	{
		str.writeObject(ostrm);
	}

	inline void writeObject(
		std::streambuf & ostrm, blocxx::UInt8 sig, const CIMBase & obj
	)
	{
		obj.writeObject(ostrm);
	}

	inline void writeObjectPath(std::streambuf & ostrm, const CIMObjectPath & op)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_OP, op);
	}

	inline void writeNameSpace(std::streambuf & ostrm, const CIMNameSpace & ns)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_NS, ns);
	}

	inline void writeBool(std::streambuf & ostrm, blocxx::Bool arg)
	{
		BinarySerialization::write(ostrm, BINSIG_BOOL);
		arg.writeObject(ostrm);
	}

	inline void writeClass(std::streambuf & ostrm, const CIMClass & cc)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_CLS, cc);
	}

	inline void writeInstance(std::streambuf & ostrm, const CIMInstance & ci)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_INST, ci);
	}

	inline void writeQualType(
		std::streambuf & ostrm, const CIMQualifierType & qt
	)
	{
		BinarySerialization::writeObject(ostrm, BINSIG_QUAL_TYPE, qt);
	}

	inline void writeValue(std::streambuf & ostrm, const CIMValue & value)
	{
		blocxx::Bool isValue = value ? true : false;
		BinarySerialization::writeBool(ostrm, isValue);
		if (isValue)
		{
			BinarySerialization::writeObject(ostrm, BINSIG_VALUE, value);
		}
	}

	inline void writeString(std::streambuf & ostrm, const blocxx::String & str)
	{
		BinarySerialization::write(ostrm, BINSIG_STR);
		str.writeObject(ostrm);
	}

	inline void writeWQLSelectStatement(std::streambuf& ostrm, const WQLSelectStatement& wss)
	{
		BinarySerialization::write(ostrm, BINSIG_WQL_SELECT_STATEMENT);
		wss.writeObject(ostrm);
	}

	OW_COMMON_API void readLen(std::streambuf & istrm, blocxx::UInt32 & len);

	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline void
	readArray(std::streambuf & istr, T & a)
	{
		a.clear();
		blocxx::UInt32 len;
		BinarySerialization::readLen(istr, len);

		a.reserve(len);
		for (blocxx::UInt32 i = 0; i < len; i++)
		{
			typename T::value_type x;
			x.readObject(istr);
			a.push_back(x);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline void
	writeArray(std::streambuf & ostrm, const T & a)
	{
		blocxx::UInt32 len = static_cast<blocxx::UInt32>(a.size());
		BinarySerialization::writeLen(ostrm, len);
		for (blocxx::UInt32 i = 0; i < len; i++)
		{
			a.operator[](i).writeObject(ostrm);
		}
	}

	inline void writeStringArray(
		std::streambuf & ostrm, const blocxx::StringArray & stra
	)
	{
		BinarySerialization::write(ostrm, BINSIG_STRARRAY);
		writeArray(ostrm, stra);
	}

	OW_COMMON_API void writeStringArray(
		std::streambuf & ostrm, const blocxx::StringArray * propertyList
	);


	OW_COMMON_API void read(
		std::streambuf & istrm, void * dataIn, size_t dataInLen
	);

	inline void read(std::streambuf & istrm, blocxx::String & arg)
	{
		arg.readObject(istrm);
	}

	inline void read(std::streambuf & istrm, blocxx::UInt64 & val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = blocxx::ntoh64(val);
	}

	inline void read(std::streambuf & istrm, blocxx::Int64 & val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = blocxx::ntoh64(val);
	}

	inline void read(std::streambuf & istrm, blocxx::Int32 & val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = blocxx::ntoh32(val);
	}

	inline void read(std::streambuf & istrm, blocxx::UInt32 & val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = blocxx::ntoh32(val);
	}

	inline void read(std::streambuf & istrm, blocxx::UInt16 & val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = blocxx::ntoh16(val);
	}

	inline void read(std::streambuf & istrm, blocxx::Int16 & val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
		val = blocxx::ntoh16(val);
	}

	inline void read(std::streambuf & istrm, blocxx::UInt8 & val)
	{
		BinarySerialization::read(istrm, &val, sizeof(val));
	}

	inline void readObject(
		std::streambuf & istrm, blocxx::UInt8 validSig, CIMBase & obj
	)
	{
		obj.readObject(istrm);
	}

	inline CIMObjectPath readObjectPath(std::streambuf & istrm)
	{
		CIMObjectPath op(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_OP, op);
		return op;
	}

	inline CIMInstance readInstance(std::streambuf & istrm)
	{
		CIMInstance ci(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_INST, ci);
		return ci;
	}

	inline blocxx::Bool readBool(std::streambuf & istrm)
	{
		BinarySerialization::verifySignature(istrm, BINSIG_BOOL);
		blocxx::Bool b;
		b.readObject(istrm);
		return b;
	}

	inline CIMNameSpace readNameSpace(std::streambuf & istrm)
	{
		CIMNameSpace ns(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_NS, ns);
		return ns;
	}

	inline CIMClass readClass(std::streambuf & istrm)
	{
		CIMClass cc(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_CLS, cc);
		return cc;
	}

	inline blocxx::String readString(std::streambuf & istrm)
	{
		BinarySerialization::verifySignature(istrm, BINSIG_STR);
		blocxx::String rv;
		rv.readObject(istrm);
		return rv;
	}

	inline CIMQualifierType readQualType(std::streambuf & istrm)
	{
		CIMQualifierType qt(CIMNULL);
		BinarySerialization::readObject(istrm, BINSIG_QUAL_TYPE, qt);
		return qt;
	}

	inline CIMValue readValue(std::streambuf & istrm)
	{
		CIMValue value(CIMNULL);
		blocxx::Bool isValue(BinarySerialization::readBool(istrm));
		if (isValue)
		{
			BinarySerialization::readObject(istrm, BINSIG_VALUE, value);
		}
		return value;
	}

	inline WQLSelectStatement readWQLSelectStatement(std::streambuf & istrm)
	{
		BinarySerialization::verifySignature(istrm, BINSIG_WQL_SELECT_STATEMENT);
		WQLSelectStatement rv;
		rv.readObject(istrm);
		return rv;
	}

	inline blocxx::StringArray readStringArray(std::streambuf & istrm)
	{
		BinarySerialization::verifySignature(istrm, BINSIG_STRARRAY);
		blocxx::StringArray stra;
		readArray(istrm, stra);
		return stra;
	}

	OW_COMMON_API void readObjectPathEnum(
		std::streambuf & istrm, CIMObjectPathResultHandlerIFC & result
	);

	OW_COMMON_API void readClassEnum(
		std::streambuf & istrm, CIMClassResultHandlerIFC& result
	);

	OW_COMMON_API void readInstanceEnum(
		std::streambuf & istrm, CIMInstanceResultHandlerIFC& result
	);

	OW_COMMON_API void readQualifierTypeEnum(
		std::streambuf & istrm, CIMQualifierTypeResultHandlerIFC& result
	);

	OW_COMMON_API void readStringEnum(
		std::streambuf & istrm, StringResultHandlerIFC& result
	);

	OW_COMMON_API void readPropertyList(
		std::streambuf & istrm, blocxx::StringArray & propList,
		blocxx::StringArray * & propListPtr
	);
} // end namespace BinarySerialization

} // end namespace OW_NAMESPACE

#endif	// OW_BinarySerialization_HPP_
