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

#ifndef __OW_CIMVALUE_HPP__
#define __OW_CIMVALUE_HPP__

#include "OW_config.h"
#include "OW_CIMBase.hpp"
#include "OW_COWReference.hpp"
#include "OW_Exception.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_Array.hpp"
#include "OW_CIMDataType.hpp"


//////////////////////////////////////////////////////////////////////////////
class OW_ValueCastException : public OW_Exception
{
public:
	OW_ValueCastException() : OW_Exception() {}
	OW_ValueCastException(const char* file, int line, const char* msg)
	: OW_Exception(file, line, msg) {}
	OW_ValueCastException(const char* msg) : OW_Exception(msg)
		{  }
	virtual const char* type() const
		{  return "OW_ValueCastException"; }
};

/**
 * The OW_CIMValue class is an abstraction for all CIM data types.
 * OW_CIMValue objects are ref counted and copy on write.
 */
class OW_CIMValue : public OW_CIMBase
{
	class OW_CIMValueImpl;

public:
	/**
	 * Create an OW_CIMValue object of a specified type from a string.
	 * @param cimtype The string representation of the data type to convert to.
	 *		(e.g. "uint8", "real32", etc...)
	 * @param value The string representation of the CIM value.
	 * @return An OW_CIMValue object of the specified data type with the
	 * specified value.
	 */
	static OW_CIMValue createSimpleValue(const OW_String& cimtype,
		const OW_String& value);

	/**
	 * Create a new OW_CIMValue object without any data or implementation.
	 */
	OW_CIMValue();

	/**
	 * Copy constructor
	 * @param x	The OW_CIMValue object to make this object a copy of.
	 */
	OW_CIMValue(const OW_CIMValue& x);

	/**
	 * Create a new OW_CIMValue that represents a boolean data type.
	 * @param x The boolean value this object will contain.
	 */
	explicit OW_CIMValue(OW_Bool x);

	/**
	 * Create a new OW_CIMValue that represents a boolean data type.
	 * @param x The boolean value this object will contain.
	 */
	explicit OW_CIMValue(bool x);

	/**
	 * Create a new OW_CIMValue that represents a uint8 data type.
	 * @param x The uint8 value this object will contain.
	 */
	explicit OW_CIMValue(OW_UInt8 x);

	/**
	 * Create a new OW_CIMValue that represents a sint8 data type.
	 * @param x The sint8 value this object will contain.
	 */
	explicit OW_CIMValue(OW_Int8 x);

	/**
	 * Create a new OW_CIMValue that represents a uint16 data type.
	 * @param x The uint16 value this object will contain.
	 */
	explicit OW_CIMValue(OW_UInt16 x);

	/**
	 * Create a new OW_CIMValue that represents a sint16 data type.
	 * @param x The sint16 value this object will contain.
	 */
	explicit OW_CIMValue(OW_Int16 x);

	/**
	 * Create a new OW_CIMValue that represents a uint32 data type.
	 * @param x The uint32 value this object will contain.
	 */
	explicit OW_CIMValue(OW_UInt32 x);

	/**
	 * Create a new OW_CIMValue that represents a sint32 data type.
	 * @param x The sint32 value this object will contain.
	 */
	explicit OW_CIMValue(OW_Int32 x);

	/**
	 * Create a new OW_CIMValue that represents a uint64 data type.
	 * @param x The uint64 value this object will contain.
	 */
	explicit OW_CIMValue(OW_UInt64 x);

	/**
	 * Create a new OW_CIMValue that represents a sint64 data type.
	 * @param x The sint64 value this object will contain.
	 */
	explicit OW_CIMValue(OW_Int64 x);

	/**
	 * Create a new OW_CIMValue that represents a real32 data type.
	 * @param x The real32 value this object will contain.
	 */
	explicit OW_CIMValue(OW_Real32 x);

	/**
	 * Create a new OW_CIMValue that represents a real64 data type.
	 * @param x The boolean value this object will contain.
	 */
	explicit OW_CIMValue(OW_Real64 x);

	/**
	 * Create a new OW_CIMValue that represents a char16 data type.
	 * @param x The char16 value this object will contain.
	 */
	explicit OW_CIMValue(const OW_Char16& x);

	/**
	 * Create a new OW_CIMValue that represents a string data type.
	 * @param x The string value this object will contain.
	 */
	explicit OW_CIMValue(const OW_String& x);

	/**
	 * Create a new OW_CIMValue that represents a string data type.
	 * @param x The string value this object will contain.
	 */
	explicit OW_CIMValue(const char* x);

	/**
	 * Create a new OW_CIMValue that represents a CIM datetime data type.
	 * @param x The CIM datetime value this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMDateTime& x);

	/**
	 * Create a new OW_CIMValue that represents a ref data type.
	 * @param x The Object path value this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMObjectPath& x);

	/**
	 * Create a new OW_CIMValue that represents an embedded class object data type.
	 * @param x The class this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMClass& x);

	/**
	 * Create a new OW_CIMValue that represents an embedded instance object data type.
	 * @param x The instance this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMInstance& x);

	/**
	 * Create a new OW_CIMValue that represents a boolean array data type.
	 * @param x The boolean array this object will contain.
	 */
	explicit OW_CIMValue(const OW_BoolArray& x);

	/**
	 * Create a new OW_CIMValue that represents a char16 array data type.
	 * @param x The char16 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_Char16Array& x);

	/**
	 * Create a new OW_CIMValue that represents a uint8 array data type.
	 * @param x The uint8 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_UInt8Array& x);

	/**
	 * Create a new OW_CIMValue that represents a sint8 array data type.
	 * @param x The sint8 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_Int8Array& x);

	/**
	 * Create a new OW_CIMValue that represents a uint16 array data type.
	 * @param x The uint16 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_UInt16Array& x);

	/**
	 * Create a new OW_CIMValue that represents a sint16 array data type.
	 * @param x The sint16 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_Int16Array& x);

	/**
	 * Create a new OW_CIMValue that represents a uint32 array data type.
	 * @param x The uint32 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_UInt32Array& x);

	/**
	 * Create a new OW_CIMValue that represents a sint32 array data type.
	 * @param x The sint32 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_Int32Array& x);

	/**
	 * Create a new OW_CIMValue that represents a uint64 array data type.
	 * @param x The uint64 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_UInt64Array& x);

	/**
	 * Create a new OW_CIMValue that represents a sint64 array data type.
	 * @param x The sint64 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_Int64Array& x);

	/**
	 * Create a new OW_CIMValue that represents a real32 array data type.
	 * @param x The real32 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_Real64Array& x);

	/**
	 * Create a new OW_CIMValue that represents a real64 array data type.
	 * @param x The real64 array this object will contain.
	 */
	explicit OW_CIMValue(const OW_Real32Array& x);

	/**
	 * Create a new OW_CIMValue that represents a string array data type.
	 * @param x The string array this object will contain.
	 */
	explicit OW_CIMValue(const OW_StringArray& x);

	/**
	 * Create a new OW_CIMValue that represents a CIM datetime array data type.
	 * @param x The datetime array this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMDateTimeArray& x);

	/**
	 * Create a new OW_CIMValue that represents a CIM Object Path array.
	 * @param x The object path array this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMObjectPathArray& x);

	/**
	 * Create a new OW_CIMValue that represents a embedded class array.
	 * @param x The class array this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMClassArray& x);

	/**
	 * Create a new OW_CIMValue that represents a embedded instance array.
	 * @param x The instance array this object will contain.
	 */
	explicit OW_CIMValue(const OW_CIMInstanceArray& x);

	/**
	 * Destroy this OW_CIMValue object.
	 */
	~OW_CIMValue();

	/**
	 * @return The size of the array this OW_CIMValue contains if it contains
	 * an array.
	 */
	OW_Int32 getArraySize() const;

	/**
	 * Get a boolean value from this OW_CIMValue.
	 */
	void get(OW_Bool& x) const;

	/**
	 * Get a char16 value from this OW_CIMValue.
	 */
	void get(OW_Char16& x) const;

	/**
	 * Get a uint8 value from this OW_CIMValue.
	 */
	void get(OW_UInt8& x) const;

	/**
	 * Get a sing8 value from this OW_CIMValue.
	 */
	void get(OW_Int8& x) const;

	/**
	 * Get a uint16 value from this OW_CIMValue.
	 */
	void get(OW_UInt16& x) const;

	/**
	 * Get a sint16 value from this OW_CIMValue.
	 */
	void get(OW_Int16& x) const;

	/**
	 * Get a uint32 value from this OW_CIMValue.
	 */
	void get(OW_UInt32& x) const;

	/**
	 * Get a sint32 value from this OW_CIMValue.
	 */
	void get(OW_Int32& x) const;

	/**
	 * Get a uint64 value from this OW_CIMValue.
	 */
	void get(OW_UInt64& x) const;

	/**
	 * Get a sint64 value from this OW_CIMValue.
	 */
	void get(OW_Int64& x) const;

	/**
	 * Get a real32 value from this OW_CIMValue.
	 */
	void get(OW_Real32& x) const;

	/**
	 * Get a real64 value from this OW_CIMValue.
	 */
	void get(OW_Real64& x) const;

	/**
	 * Get a string value from this OW_CIMValue.
	 */
	void get(OW_String& x) const;

	/**
	 * Get a CIM datetime value from this OW_CIMValue.
	 */
	void get(OW_CIMDateTime& x) const;

	/**
	 * Get a reference (Object path) value from this OW_CIMValue.
	 */
	void get(OW_CIMObjectPath& x) const;

	/**
	 * Get a embedded class value from this OW_CIMValue.
	 */
	void get(OW_CIMClass& x) const;

	/**
	 * Get a embedded instance value from this OW_CIMValue.
	 */
	void get(OW_CIMInstance& x) const;

	/**
	 * Get a reference (Object path) array value from this OW_CIMValue.
	 */
	void get(OW_CIMObjectPathArray& x) const;

	/**
	 * Get a char16 array from this OW_CIMValue.
	 */
	void get(OW_Char16Array& x) const;

	/**
	 * Get a uint8 array from this OW_CIMValue.
	 */
	void get(OW_UInt8Array& x) const;

	/**
	 * Get a sint8 array from this OW_CIMValue.
	 */
	void get(OW_Int8Array& x) const;

	/**
	 * Get a uint16 array from this OW_CIMValue.
	 */
	void get(OW_UInt16Array& x) const;

	/**
	 * Get a sint16 array from this OW_CIMValue.
	 */
	void get(OW_Int16Array& x) const;

	/**
	 * Get a uint32 array from this OW_CIMValue.
	 */
	void get(OW_UInt32Array& x) const;

	/**
	 * Get a sint32 array from this OW_CIMValue.
	 */
	void get(OW_Int32Array& x) const;

	/**
	 * Get a uint64 array from this OW_CIMValue.
	 */
	void get(OW_UInt64Array& x) const;

	/**
	 * Get a sint64 array from this OW_CIMValue.
	 */
	void get(OW_Int64Array& x) const;

	/**
	 * Get a real64 array from this OW_CIMValue.
	 */
	void get(OW_Real64Array& x) const;

	/**
	 * Get a real32 array from this OW_CIMValue.
	 */
	void get(OW_Real32Array& x) const;

	/**
	 * Get a string array from this OW_CIMValue.
	 */
	void get(OW_StringArray& x) const;

	/**
	 * Get a boolean array from this OW_CIMValue.
	 */
	void get(OW_BoolArray& x) const;

	/**
	 * Get a CIM datetime array from this OW_CIMValue.
	 */
	void get(OW_CIMDateTimeArray& x) const;

	/**
	 * Get a CIM class array from this OW_CIMValue.
	 */
	void get(OW_CIMClassArray& x) const;

	/**
	 * Get a CIM instance array from this OW_CIMValue.
	 */
	void get(OW_CIMInstanceArray& x) const;

	// TODO: Have these function call ValueCast?
	OW_CIMObjectPath toCIMObjectPath() const;
	OW_StringArray toStringArray() const;
	OW_Bool toBool() const;
	OW_UInt64 toUInt64() const;
	
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

private:
	struct dummy
	{
		void nonnull() {};
	};

	typedef void (dummy::*safe_bool)();

public:
	operator safe_bool () const
		{  return (!m_impl.isNull()) ? &dummy::nonnull : 0; }
	safe_bool operator!() const
		{  return (!m_impl.isNull()) ? 0: &dummy::nonnull; }

	/**
	 * Assign another OW_CIMValue to this one.
	 * @param x The OW_CIMValue to assign to this one.
	 * @return A reference to this OW_CIMValue object.
	 */
	OW_CIMValue& set(const OW_CIMValue& x);

	/**
	 * Assignment operator
	 * @param x The OW_CIMValue to assign to this one.
	 * @return A reference to this OW_CIMValue object.
	 */
	OW_CIMValue& operator= (const OW_CIMValue& x);

	/**
	 * Determine equality of this OW_CIMValue object with another.
	 * @param x The OW_CIMValue object to check for equality with.
	 * @return true If this OW_CIMValue is equal to the given OW_CIMValue.
	 */
	OW_Bool equal(const OW_CIMValue& x) const;

	/**
	 * Equality operator
	 * @param x The OW_CIMValue to check for equality with.
	 * @return true if the specifed OW_CIMValue is equal to this one. Otherwise
	 * false.
	 */
	OW_Bool operator== (const OW_CIMValue& x) const;

	/**
	 * Inequality operator
	 * @param x The OW_CIMValue to check for inequality with.
	 * @return true if the specifed OW_CIMValue is not equal to this one.
	 * Otherwise false.
	 */
	OW_Bool operator!= (const OW_CIMValue& x) const;

	/**
	 * Less than or equal operator
	 * @param x The OW_CIMValue to compare this one with.
	 * @return true if The specifed OW_CIMValue is less than or equal to this
	 * one. Otherwise false.
	 */
	OW_Bool operator<= (const OW_CIMValue& x) const;

	/**
	 * Greater than or equal operator
	 * @param x The OW_CIMValue to compare this one with.
	 * @return true if The specifed OW_CIMValue is greater than or equal to this
	 * one. Otherwise false.
	 */
	OW_Bool operator>= (const OW_CIMValue& x) const;

	/**
	 * Less than operator
	 * @param x The OW_CIMValue to compare this one with.
	 * @return true if The specifed OW_CIMValue is less than this one. Otherwise
	 * false.
	 */
	OW_Bool operator< (const OW_CIMValue& x) const;

	/**
	 * Greater than operator
	 * @param x The OW_CIMValue to compare this one with.
	 * @return true if The specifed OW_CIMValue is greater than this one.
	 * Otherwise false.
	 */
	OW_Bool operator> (const OW_CIMValue& x) const;

	/**
	 * @return The integral representation of the data type for this OW_CIMValue.
	 * (see class OW_CIMDataType)
	 */
	OW_CIMDataType::Type getType() const;

	/**
	 * @return The type for this OW_CIMValue.
	 * (see class OW_CIMDataType)
	 */
	OW_CIMDataType getCIMDataType() const;

	/**
	 * Determine if another OW_CIMValue has the same data type as this one.
	 * @param x The OW_CIMValue to compare this object's data type with.
	 * @return true If the datatypes are the same. Otherwise false.
	 */
	OW_Bool sameType(const OW_CIMValue& x) const;

	/**
	 * @return true If this OW_CIMValue contains an array.
	 */
	OW_Bool isArray() const;

	/**
	 * Read this object from an input stream.
	 * @param istrm The input stream to read this object from.
	 */
	virtual void readObject(std::istream &istrm);

	/**
	 * Write this object to an output stream.
	 * @param ostrm The output stream to write this object to.
	 */
	virtual void writeObject(std::ostream &ostrm) const;

	/**
	 * @return The string representation of this OW_CIMValue object.
	 */
	virtual OW_String toString() const;

	/**
	 * @return The MOF representation of this OW_CIMValue as an OW_String.
	 */
	virtual OW_String toMOF() const;

	/**
	 * @return true if this OW_CIMValue object contains a numeric type.
	 */
	OW_Bool isNumericType() const;

private:

	OW_COWReference<OW_CIMValueImpl> m_impl;
};


#endif	// __OW_CIMVALUE_HPP__
