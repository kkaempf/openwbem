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

#ifndef OW_CIMBASE_HPP_
#define OW_CIMBASE_HPP_

#include "OW_config.h"
#include "OW_ByteSwap.hpp"
#include "OW_Blob.hpp"

#include <iosfwd>

class OW_String;

/**
 * The OW_CIMBase class is used as the base class for all CIM related classes.
 * (i.e. OW_CIMClass, OW_CIMInstance, etc...). It ensures that all derived
 * class will support the specified interface.
 */
class OW_CIMBase
{
public:
	/**
	 * Destroy this OW_CIMBase object.
	 */
	virtual ~OW_CIMBase() {  }

	/**
	 * Set this object to NULL. This should invalidate the OW_CIMBase object, so
	 * that subsequent operation will fail.
	 */
	virtual void setNull() = 0;

	/**
	 * @return The OW_String representation of this object.
	 */
	virtual OW_String toString() const = 0;

	/**
	 * @return The MOF representation of this object as an OW_String.
	 */
	virtual OW_String toMOF() const = 0;

	/**
	 * Write the XML representation of this object out to the output stream
	 * according to the appropriate XML DTD specified for the object type.
	 *
	 * @param ostrm The output stream to write the XML to.
	 */
	//virtual void toXML(std::ostream& ostrm) const = 0;

	/**
	 * Read this object from an input stream. The object must have been
	 * previously written through a call to writeObject.
	 *
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream& istrm) = 0;

	/**
	 * Write this object to an output stream. The intent is for the object to
	 * be retrieved later through a call to readObject.
	 *
	 * @param ostrm	The output stream to write the object to.
	 */
	virtual void writeObject(std::ostream& ostrm) const = 0;

	/**
	 * Read the specified signature from the input stream. Each class derived
	 * from OW_CIMBase must have a unique signature associated with it. If the
	 * signature that is being read from the stream doesn't match the one
	 * provided, an OW_IOException will be thrown.
	 *
	 * @param istrm The input stream to read the signature from.
	 *
	 * @param sig The signature to compare the signature read to. This must be
	 *		a NULL terminated string. If the signature read does not match this
	 *		string, an OW_IOException will be thrown.
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

	/**
	 * This is a convenience method, that allows the caller to get the XML
	 * representation of this class without an output stream.
	 *
	 * @return The XML representation of this class as an OW_String.
	 */
	//OW_String convertToXML() const;

	/**
	 * Convert this object to an OW_Blob object.
	 * @return A pointer to an OW_Blob object. The caller is responsible for
	 *		freeing the memory returned from this call with ::free not delete.
	 */
	 virtual OW_Blob* toBlob() const;

	 /**
	  * Create this object from the contents of an OW_Blob
	  * @param blob The OW_Blob object to initialize this object with.
	  */
	 virtual void fromBlob(OW_Blob* blob);
};

///////////////////////////////////////////////////////////////////////////////
// signatures to be passed to readSig and writeSig
#define OW_CIMCLASSSIG					"CLAS"	// OW_CIMClass
#define OW_CIMINSTANCESIG				"INST"	// OW_CIMInstance
#define OW_CIMMETHODSIG					"METH"	// OW_CIMMethod
#define OW_CIMPARAMETERSIG				"PARM"	// OW_CIMParameter
#define OW_CIMPARAMVALSIG				"PVAL"	// OW_CIMParamValue
#define OW_CIMPROPERTYSIG				"PROP"	// OW_CIMProperty
#define OW_CIMQUALIFIERSIG				"QUAL"	// OW_CIMQualifier
#define OW_CIMQUALIFIERTYPESIG			"QTYP"	// OW_CIMQualifierType
#define OW_CIMDATATYPESIG				"DTYP"	// OW_CIMDataType
#define OW_CIMFLAVORSIG					"FLAV"	// OW_CIMFlavor
#define OW_CIMNAMESPACESIG				"NMSP"	// OW_CIMNameSpace
#define OW_CIMOBJECTPATHSIG				"OPTH"	// OW_CIMObjectPath
#define OW_CIMSCOPESIG					"SCOP"	// OW_CIMScope
#define OW_CIMVALUESIG					"VALU"	// OW_CIMValue
#define OW_CIMURLSIG					"CURL"	// OW_CIMUrl
#define OW_INTERNNAMESPACESIG			"INNS"	// Internval namespace class

#endif	// OW_CIMBASE_HPP_
