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
#ifndef __OW_BINIFCIO_HPP__
#define __OW_BINIFCIO_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_CIMFwd.hpp"
#include "OW_Bool.hpp"
#include <iosfwd>

//////////////////////////////////////////////////////////////////////////////
class OW_BinIfcIO
{
public:
	static OW_Bool write(std::ostream& ostrm, const void* dataOut,
		int dataOutLen, OW_Bool errorAsException=false);

	static void verifySignature(std::istream& istrm, OW_Int32 validSig);

	static OW_Bool write(std::ostream& ostrm, OW_Int32 val,
		OW_Bool throwOnError=false);

	static OW_Bool write(std::ostream& ostrm, const char* str,
		OW_Bool throwOnError=false);

	static OW_Bool write(std::ostream& ostrm, const OW_String& str,
		OW_Bool throwOnError=false);

	static void writeObject(std::ostream& ostrm, OW_Int32 sig,
		const OW_CIMBase& obj);

	static void writeObjectPath(std::ostream& ostrm,
		const OW_CIMObjectPath& op);

	static void writeNameSpace(std::ostream& ostrm,
		const OW_CIMNameSpace& ns);

	static void writeBool(std::ostream& ostrm,
		OW_Bool arg);

	static void writeClass(std::ostream& ostrm, const OW_CIMClass& cc);

	static void writeInstance(std::ostream& ostrm, const OW_CIMInstance& ci);

	static void writeQual(std::ostream& ostrm, const OW_CIMQualifierType& qt);

	static void writeValue(std::ostream& ostrm, const OW_CIMValue& value);

	static void writeString(std::ostream& ostrm, const OW_String& str);

	static void writeStringArray(std::ostream& ostrm,
		const OW_StringArray& stra);

	static OW_Bool read(std::istream& istrm, void* dataIn, int dataInLen,
		OW_Bool errorAsException=false);

	static OW_Bool read(std::istream& istrm, OW_String& arg,
		OW_Bool throwOnError=false);

	static OW_Bool read(std::istream& istrm, OW_Int32& val,
		OW_Bool throwOnError=false);

	static void readObject(std::istream& istrm, OW_Int32 validSig,
		OW_CIMBase& obj);

	static OW_CIMObjectPath readObjectPath(std::istream& istrm);

	static OW_CIMInstance readInstance(std::istream& istrm);

	static OW_Bool readBool(std::istream& istrm);

	static OW_CIMNameSpace readNameSpace(std::istream& istrm);

	static OW_CIMClass readClass(std::istream& istrm);

	static OW_String readString(std::istream& istrm);

	static OW_CIMQualifierType readQual(std::istream& istrm);

	static OW_CIMValue readValue(std::istream& istrm);

	static OW_StringArray readStringArray(std::istream& istrm);

	static OW_CIMObjectPathEnumeration readObjectPathEnum(std::istream& istrm);

	static OW_CIMClassEnumeration readClassEnum(std::istream& istrm);

	static OW_CIMInstanceEnumeration readInstanceEnum(std::istream& istrm);

	static OW_CIMQualifierTypeEnumeration readQualifierTypeEnum(std::istream& istrm);


};

// Values for local API calls

#define OW_IPC_AUTHENTICATE			3000	// Authenticate
#define OW_IPC_FUNCTIONCALL			4000	// Regular function call
#define OW_IPC_CLOSECONN			9999	// Close connection

#define OW_BIN_DELETECLS			40		// Delete class
#define OW_BIN_DELETEINST			50		// Delete instance
#define OW_BIN_DELETEQUAL			60		// Delete qualifier type
#define OW_BIN_ENUMCLSS				70		// Enum class
#define OW_BIN_ENUMCLSNAMES			80		// Enum class names
#define OW_BIN_ENUMINSTS			90		// Enum instances
#define OW_BIN_ENUMINSTNAMES		100		// Enum instance names
#define OW_BIN_ENUMQUALS			110		// Enum qualifiers types
#define OW_BIN_GETCLS				120		// Get class
#define OW_BIN_GETINST				130		// Get instance
#define OW_BIN_INVMETH				140		// Invoke method
#define OW_BIN_GETQUAL				150		// Get qualifier type
#define OW_BIN_SETQUAL				160		// Set qualifier type
#define OW_BIN_MODIFYCLS			170		// Modify class
#define OW_BIN_CREATECLS			180		// Create class
#define OW_BIN_MODIFYINST			190		// Modify instances
#define OW_BIN_CREATEINST			200		// Create instance
#define OW_BIN_GETPROP				210		// Get property
#define OW_BIN_SETPROP				220		// Set property
#define OW_BIN_ASSOCNAMES			230		// Associator names
#define OW_BIN_ASSOCIATORS			240		// Associators
#define OW_BIN_REFNAMES				250		// Reference names
#define OW_BIN_REFERENCES			260		// References
#define OW_BIN_EXECQUERY			270		// Execute query
#define OW_BIN_GETSVRFEATURES		280		// Get Server Features

#define OW_BIN_OK					0		// Success returned from server
#define OW_BIN_ERROR				-1		// Error returned from server
#define OW_BIN_EXCEPTION			-2		// CIM Exception returned from server

#define OW_BINSIG_NS				0xa0000001
#define OW_BINSIG_OP				0xa0000002
#define OW_BINSIG_CLS				0xa0000003
#define OW_BINSIG_INST				0xa0000004
#define OW_BINSIG_BOOL				0xa0000005
#define OW_BINSIG_CLSENUM			0xa0000006
#define OW_BINSIG_STR				0xa0000007
#define OW_BINSIG_STRARRAY			0xa0000008
#define OW_BINSIG_QUAL				0xa0000009
#define OW_BINSIG_VALUE				0xa000000a
#define OW_BINSIG_OPENUM			0xa000000b
#define OW_BINSIG_INSTENUM			0xa000000c
#define OW_BINSIG_QUALENUM			0xa000000d
#define OW_BINSIG_VALUEARRAY		0xa000000e
#define OW_BINSIG_INSTARRAY			0xa000000f

#endif	// __OW_BINIFCIO_HPP__

