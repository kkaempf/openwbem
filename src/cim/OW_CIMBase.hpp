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

#ifndef OW_CIMBASE_HPP_
#define OW_CIMBASE_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_SerializableIFC.hpp"
#include "OW_CommonFwd.hpp"
#include <iosfwd>

namespace OW_NAMESPACE
{

/**
 * The CIMBase class is used as the base class for all CIM related classes.
 * (i.e. CIMClass, CIMInstance, etc...). It ensures that all derived
 * class will support the specified interface.
 */
class OW_COMMON_API CIMBase : public SerializableIFC
{
public:
	virtual ~CIMBase();
	/**
	 * Set this object to NULL. This should invalidate the CIMBase object, so
	 * that subsequent operation will fail.
	 */
	virtual void setNull() = 0;
	/**
	 * @return The String representation of this object.
	 */
	virtual String toString() const = 0;
	/**
	 * @return The MOF representation of this object as an String.
	 */
	virtual String toMOF() const = 0;

	enum EErrorCodes
	{
		E_UNEXPECTED_SIGNATURE,
		E_UNKNOWN_VERSION
	};

	/**
	 * Read the specified signature from the input stream. Each class derived
	 * from CIMBase must have a unique signature associated with it. If the
	 * signature that is being read from the stream doesn't match the one
	 * provided, an BadCIMSignatureException will be thrown.
	 *
	 * @param istrm The input stream to read the signature from.
	 *
	 * @param sig The signature to compare the signature read to. This must be
	 *		a NULL terminated string. If the signature read does not match this
	 *		string, an BadCIMSignatureException will be thrown.
	 *
	 * @throws BadCIMSignatureException
	 */
	static void readSig(std::streambuf & istrm, const char * const sig);

	/**
	 * Read one of two given signatures from the input stream. Each class
	 * derived from CIMBase must have a uniqe signature associated with it.
	 * Older class signatures represent a data format that do not have a format
	 * version associated with it. If the older signature is read then the
	 * format version is assumed to be zero. If the new signature is read, then
	 * the version is read as well. This version will be given to the caller so
	 * the object can be read in it's proper form.
	 *
	 * @param istrm The input stream to read the signature from.
	 * @param sig The old signature. This will be the signature that was used
	 *  before versioning was implemented on the derived class. Must be a
	 *  NULL terminated string. If the signature read does not match then
	 *  verSig will be checked.
	 * @param verSig The new signature. This will be the signature that is
	 *  used after implementing versioning on the derived class. Must be a
	 *  NULL terminated string. If the signature read does not match then
	 *  a BadCIMSignatureException will be thrown. Otherwise this method
	 *  will read the version data that follows the signature.
	 * @param maxVersion The largest version that is supported.
	 * @return Version number for the derived class. If version returned == 0,
	 *  then the old signature was the match. If version returned > 0,
	 *  then the derived class has implemented versioning.
	 * @throws BadCIMSignatureException If the signature doesn't match sig or
	 *  verSig, or if the version read is > maxVersion.
	 */
	static UInt32 readSig(
		std::streambuf & istrm, char const * const sig,
		char const * const verSig, UInt32 maxVersion
	);

	/**
	 * Write the given class signature to an output stream.
	 *
	 * @param ostrm The output stream to write the signature to.
	 *
	 * @param sig The signature to write to the output stream as a NULL
	 *		terminated string.
	 */
	static void writeSig(std::streambuf & ostrm, char const * const sig);

	/**
	 * Write the given class signature and version to the output stream.
	 * @param ostrm The output stream to write the signature to.
	 * @param sig The signature to write to the output stream as a NULL
	 *		terminated string.
	 * @param version The version value to write to the output stream.
	 */
	static void writeSig(
		std::streambuf & ostrm, char const * const sig, UInt32 version
	);

};
OW_COMMON_API std::ostream& operator<<(std::ostream& ostr, const CIMBase& cb);
///////////////////////////////////////////////////////////////////////////////
// signatures for non-versioned format of objects. These are pass to the
// readSig and writeSig methods.
#define OW_CIMCLASSSIG					"C"	// OW_CIMClass
#define OW_CIMINSTANCESIG				"I"	// OW_CIMInstance
#define OW_CIMMETHODSIG					"M"	// OW_CIMMethod
#define OW_CIMPARAMETERSIG				"P"	// OW_CIMParameter
#define OW_CIMPARAMVALSIG				"A"	// OW_CIMParamValue
#define OW_CIMPROPERTYSIG				"R"	// OW_CIMProperty
#define OW_CIMQUALIFIERSIG				"Q"	// OW_CIMQualifier
#define OW_CIMQUALIFIERTYPESIG			"T"	// OW_CIMQualifierType
#define OW_CIMDATATYPESIG				"D"	// OW_CIMDataType
#define OW_CIMFLAVORSIG					"F"	// OW_CIMFlavor
#define OW_CIMNAMESPACESIG				"N"	// OW_CIMNameSpace
#define OW_CIMOBJECTPATHSIG				"O"	// OW_CIMObjectPath
#define OW_CIMSCOPESIG					"S"	// OW_CIMScope
#define OW_CIMVALUESIG					"V"	// OW_CIMValue
#define OW_CIMURLSIG					"U"	// OW_CIMUrl
#define OW_INTERNNAMESPACESIG			"E"	// Internval namespace class
#define OW_CIMPARAMVALUESIG				"L"	// OW_CIMParamValue

///////////////////////////////////////////////////////////////////////////////
// signatures for versioned format of objects. These are pass to the
// readSig and writeSig methods.
#define OW_CIMCLASSSIG_V   				"c"	// OW_CIMClass
#define OW_CIMINSTANCESIG_V				"i"	// OW_CIMInstance
#define OW_CIMMETHODSIG_V				"m"	// OW_CIMMethod
#define OW_CIMPARAMETERSIG_V 			"p"	// OW_CIMParameter
#define OW_CIMPARAMVALSIG_V				"a"	// OW_CIMParamValue
#define OW_CIMPROPERTYSIG_V				"r"	// OW_CIMProperty
#define OW_CIMQUALIFIERSIG_V			"q"	// OW_CIMQualifier
#define OW_CIMQUALIFIERTYPESIG_V		"t"	// OW_CIMQualifierType
#define OW_CIMDATATYPESIG_V				"d"	// OW_CIMDataType
#define OW_CIMFLAVORSIG_V				"f"	// OW_CIMFlavor
#define OW_CIMNAMESPACESIG_V			"n"	// OW_CIMNameSpace
#define OW_CIMOBJECTPATHSIG_V			"o"	// OW_CIMObjectPath
#define OW_CIMSCOPESIG_V				"s"	// OW_CIMScope
#define OW_CIMVALUESIG_V				"v"	// OW_CIMValue
#define OW_CIMURLSIG_V					"u"	// OW_CIMUrl
#define OW_INTERNNAMESPACESIG_V			"e"	// Internval namespace class
#define OW_CIMPARAMVALUESIG_V			"l"	// OW_CIMParamValue


} // end namespace OW_NAMESPACE

#endif	// OW_CIMBASE_HPP_
