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
*	  this list of conditions and the following disclaimer in the documentation
*	  and/or other materials provided with the distribution.
*
*  - Neither the name of Caldera International nor the names of its
*	  contributors may be used to endorse or promote products derived from this
* 	  software without specific prior written permission.
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
#ifndef OW_CIMDATATYPE_HPP_INCLUDE_GUARD_
#define OW_CIMDATATYPE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Bool.hpp"
#include "OW_COWReference.hpp"
#include "OW_CIMBase.hpp"
#include <iosfwd>

class OW_String;
class OW_CIMValue;

/**
 * The OW_CIMDataType is used to represent the CIM data type of underlying data
 * in other CIM objects, such as OW_CIMValue objects.
 */
class OW_CIMDataType : public OW_CIMBase
{
public:

	struct DTData;
	enum
	{
		SIZE_SINGLE,		// non-array types
		SIZE_UNLIMITED,	// indicates array type unlimited
		SIZE_LIMITED		// array but limited size (NOTSUN)
	};

	enum Type
	{
		CIMNULL			= 0,		// Null type
		UINT8				= 1,		// Unsigned 8-bit integer  				(SUPPORTED)
		SINT8				= 2,		// Signed 8-bit integer    				(SUPPORTED)
		UINT16			= 3,		// Unsigned 16-bit integer 				(SUPPORTED)
		SINT16			= 4,		// Signed 16-bit integer					(SUPPORTED)
		UINT32			= 5,		// Unsigned 32-bit integer					(SUPPORTED)
		SINT32			= 6,		// Signed 32-bit integer					(SUPPORTED)
		UINT64			= 7,		// Unsigned 64-bit integer					(SUPPORTED)
		SINT64			= 8,		// Signed 64-bit integer					(SUPPORTED)
		STRING			= 9,		// Unicode string								(SUPPORTED)
		BOOLEAN			= 10,		// boolean										(SUPPORTED)
		REAL32			= 11,		// IEEE 4-byte floating-point				(SUPPORTED)
		REAL64			= 12,		// IEEE 8-byte floating-point				(SUPPORTED)
		DATETIME			= 13,		// date-time as a string					(SUPPORTED)
		CHAR16			= 14,		// 16-bit UCS-2 character					(SUPPORTED)
		REFERENCE		= 15,		// Reference type								(SUPPORTED)
		EMBEDDEDCLASS	= 16,		// Embedded Class				(SUPPORTED)
		EMBEDDEDINSTANCE	= 17,		// Embedded Instance				(SUPPORTED)
		MAXDATATYPE		= 18,		// Marker for valid checks
		INVALID			= 255		// Invalid type
	};

	/**
	 * Create an OW_CIMDataType
	 * @param notNull	If false, object will have a NULL implementation.
	 * No operations should be attempted on a NULL OW_CIMDataType.
	 */
	explicit OW_CIMDataType(OW_Bool notNull=OW_Bool(false));

	/**
	 * Create a new OW_CIMDataType object represent the given type.
	 * This constructor is for non-array and non-reference types.
	 * @param type	An integer specifying the data type of this object.
	 */
	OW_CIMDataType(Type type);

	/**
	 * Create a new OW_CIMDataType object representing the given type.
	 * @param type		An integer specifying the data type of this object.
	 * @param size		The size of the type if it is an array type.
	 *	Pass 0 if the size is unlimited.
	 * @exception OW_CIMException if the type is invalid for this method.
	 */
	OW_CIMDataType(Type type, OW_Int32 size);

	/**
	 * Create a new OW_CIMDataType object that represents a REFERENCE data type.
	 * @param refClassName	The name of the reference class.
	 */
	explicit OW_CIMDataType(const OW_String& refClassName);

	/**
	 * Copy constructor
	 * @param arg	The OW_CIMDataType to make a copy of
	 */
	OW_CIMDataType(const OW_CIMDataType& arg);
	
	/**
	 * Destroy this OW_CIMDataType object.
	 */
	~OW_CIMDataType();

	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	/**
	 * Assignment operator
	 * @param arg The OW_CIMDataType to assign to this one.
	 * @return A reference to this OW_CIMDataType object.
	 */
	OW_CIMDataType& operator = (const OW_CIMDataType& arg);

	/**
	 * Set this OW_CIMDataType to represent a given OW_CIMValue.
	 * @param value The OW_CIMValue object to sync this OW_CIMDataType with.
	 * @return true if this OW_CIMDataType changed during this method call.
	 * Otherwise false indicates this OW_CIMDataType already represented the
	 * given value.
	 */
	OW_Bool syncWithValue(const OW_CIMValue& value);

	/**
	 * Set this datatype to an array type
	 * @param size If 0 or -1 then the array is marked as unlimited in size.
	 *		Otherwise the size is set at the given size.
	 * @return true on success. Otherwise false.
	 */
	OW_Bool setToArrayType(OW_Int32 size);

	/**
	 * @return true if this data type represents an array type.
	 */
	OW_Bool isArrayType() const;


	/**
	 * @return true if this data type represents a numeric data type
	 */
	OW_Bool isNumericType() const;

	/**
	 * @return true if this data type represents a reference data type.
	 */
	OW_Bool isReferenceType() const;

	/**
	 * @return the type of this OW_CIMDataType
	 */
	Type getType() const;

	/**
	 * @return the number of elements for this OW_CIMDataType
	 */
	OW_Int32 getSize() const;

	/**
	 * @return the class name for reference if this is a REFERENCE type.
	 */
	OW_String getRefClassName() const;

	/**
	 * @return true if this OW_CIMDataType is a valid CIM data type
	 */
private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const;
	safe_bool operator!() const;

	/**
	 * Create an OW_CIMDataType object represented by a given string.
	 * @param arg	An OW_String that contains a valid name of a CIM data type.
	 *		(i.e. "uint8", "sint8", "real32", etc...)
	 * @return The OW_CIMDataType represented by the given string.
	 */
	static OW_CIMDataType getDataType(const OW_String& strType);

	/**
	 * Determine the int data type associated with a name of a data type.
	 * @param strType	An OW_String that contains a valid name of a CIM data type.
	 *		(i.e. "uint8", "sint8", "real32", etc...)
	 * @return An integer representation of the data type (i.e. UINT8, SINT8,
	 * REAL32, etc...)
	 */
	static Type strToSimpleType(const OW_String& strType);

	/**
	 * Check a given OW_CIMDataType with this one for equality.
	 * @param arg The OW_CIMDataType to compare with this one.
	 * @return true if the given OW_CIMDataType is equal to this one.
	 */
	OW_Bool equals(const OW_CIMDataType& arg) const;

	/**
	 * Read this OW_CIMDataType from an inputstream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this OW_CIMDataType to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * @return The string representation of this OW_CIMDataType object.
	 */
	virtual OW_String toString() const;

	/**
	 * @return The MOF representation of this OW_CIMDataType as an OW_String.
	 */
	virtual OW_String toMOF() const;

	/**
	 * Determine if a given data type is numeric.
	 * @param type The data type to check
	 * @return true if 'type' is a numeric data type.
	 */
	static OW_Bool isNumericType(Type type);

private:

	OW_COWReference<DTData> m_pdata;

	friend bool operator<(const OW_CIMDataType& x, const OW_CIMDataType& y);
};

#endif


