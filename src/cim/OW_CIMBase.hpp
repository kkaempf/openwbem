/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#ifndef OW_CIMBASE_HPP_
#define OW_CIMBASE_HPP_
#include "OW_config.h"
#include "OW_SerializableIFC.hpp"
#include <iosfwd>

namespace OpenWBEM
{

class String;
/**
 * The CIMBase class is used as the base class for all CIM related classes.
 * (i.e. CIMClass, CIMInstance, etc...). It ensures that all derived
 * class will support the specified interface.
 */
class CIMBase : public SerializableIFC
{
public:
	/**
	 * Destroy this CIMBase object.
	 */
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
	/**
	 * Read the specified signature from the input stream. Each class derived
	 * from CIMBase must have a unique signature associated with it. If the
	 * signature that is being read from the stream doesn't match the one
	 * provided, an IOException will be thrown.
	 *
	 * @param istrm The input stream to read the signature from.
	 *
	 * @param sig The signature to compare the signature read to. This must be
	 *		a NULL terminated string. If the signature read does not match this
	 *		string, an IOException will be thrown.
	 */
	static void readSig(std::istream& istrm, const char* const sig);
	/**
	 * Write the given class signature to an output stream.
	 *
	 * @param ostrm The output stream to write the signature to.
	 *
	 * @param sig The signature to write to the output stream as a NULL
	 *		terminated string.
	 */
	static void writeSig(std::ostream& ostrm, const char* const sig);
};
std::ostream& operator<<(std::ostream& ostr, const CIMBase& cb);
///////////////////////////////////////////////////////////////////////////////
// signatures to be passed to readSig and writeSig
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

} // end namespace OpenWBEM

typedef OpenWBEM::CIMBase OW_CIMBase OW_DEPRECATED;

#endif	// OW_CIMBASE_HPP_
