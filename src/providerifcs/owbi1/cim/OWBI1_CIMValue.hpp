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

#ifndef OWBI1_CIMVALUE_HPP_INCLUDE_GUARD_
#define OWBI1_CIMVALUE_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_CIMBase.hpp"
#include "OWBI1_COWIntrusiveReference.hpp"
#include "OWBI1_CIMFwd.hpp"
#include "OWBI1_Array.hpp"
#include "OWBI1_CIMDataType.hpp"
#include "OWBI1_CIMNULL.hpp"

namespace OWBI1
{

/**
 * The CIMValue class is an abstraction for all CIM data types.
 * CIMValue objects are ref counted and copy on write.
 */
class OWBI1_OWBI1PROVIFC_API CIMValue : public CIMBase
{
	class CIMValueImpl;
public:
	/**
	 * Create an CIMValue object of a specified type from a string.
	 * @param cimtype The string representation of the data type to convert to.
	 *		(e.g. "uint8", "real32", etc...)
	 * @param value The string representation of the CIM value.
	 * @return An CIMValue object of the specified data type with the
	 * specified value.
	 */
	static CIMValue createSimpleValue(const String& cimtype,
		const String& value);
	/**
	 * Create a new NULL CIMValue object without any data or implementation.
	 */
	explicit CIMValue(CIMNULL_t);
	/**
	 * Copy constructor
	 * @param x	The CIMValue object to make this object a copy of.
	 */
	CIMValue(const CIMValue& x);
	/**
	 * Create a new CIMValue that represents a boolean data type.
	 * @param x The boolean value this object will contain.
	 */
	explicit CIMValue(Bool x);
	/**
	 * Create a new CIMValue that represents a boolean data type.
	 * @param x The boolean value this object will contain.
	 */
	explicit CIMValue(bool x);
	/**
	 * Create a new CIMValue that represents a uint8 data type.
	 * @param x The uint8 value this object will contain.
	 */
	explicit CIMValue(UInt8 x);
	/**
	 * Create a new CIMValue that represents a sint8 data type.
	 * @param x The sint8 value this object will contain.
	 */
	explicit CIMValue(Int8 x);
	/**
	 * Create a new CIMValue that represents a uint16 data type.
	 * @param x The uint16 value this object will contain.
	 */
	explicit CIMValue(UInt16 x);
	/**
	 * Create a new CIMValue that represents a sint16 data type.
	 * @param x The sint16 value this object will contain.
	 */
	explicit CIMValue(Int16 x);
	/**
	 * Create a new CIMValue that represents a uint32 data type.
	 * @param x The uint32 value this object will contain.
	 */
	explicit CIMValue(UInt32 x);
	/**
	 * Create a new CIMValue that represents a sint32 data type.
	 * @param x The sint32 value this object will contain.
	 */
	explicit CIMValue(Int32 x);
	/**
	 * Create a new CIMValue that represents a uint64 data type.
	 * @param x The uint64 value this object will contain.
	 */
	explicit CIMValue(UInt64 x);
	/**
	 * Create a new CIMValue that represents a sint64 data type.
	 * @param x The sint64 value this object will contain.
	 */
	explicit CIMValue(Int64 x);
	/**
	 * Create a new CIMValue that represents a real32 data type.
	 * @param x The real32 value this object will contain.
	 */
	explicit CIMValue(Real32 x);
	/**
	 * Create a new CIMValue that represents a real64 data type.
	 * @param x The boolean value this object will contain.
	 */
	explicit CIMValue(Real64 x);
	/**
	 * Create a new CIMValue that represents a char16 data type.
	 * @param x The char16 value this object will contain.
	 */
	explicit CIMValue(const Char16& x);
	/**
	 * Create a new CIMValue that represents a string data type.
	 * @param x The string value this object will contain.
	 */
	explicit CIMValue(const String& x);
	/**
	 * Create a new CIMValue that represents a string data type.
	 * @param x The string value this object will contain.
	 */
	explicit CIMValue(const char* x);
	/**
	 * Create a new CIMValue that represents a CIM datetime data type.
	 * @param x The CIM datetime value this object will contain.
	 */
	explicit CIMValue(const CIMDateTime& x);
	/**
	 * Create a new CIMValue that represents a ref data type.
	 * @param x The Object path value this object will contain.
	 */
	explicit CIMValue(const CIMObjectPath& x);
	/**
	 * Create a new CIMValue that represents an embedded class object data type.
	 * @param x The class this object will contain.
	 */
	explicit CIMValue(const CIMClass& x);
	/**
	 * Create a new CIMValue that represents an embedded instance object data type.
	 * @param x The instance this object will contain.
	 */
	explicit CIMValue(const CIMInstance& x);
	/**
	 * Create a new CIMValue that represents a boolean array data type.
	 * @param x The boolean array this object will contain.
	 */
	explicit CIMValue(const BoolArray& x);
	/**
	 * Create a new CIMValue that represents a char16 array data type.
	 * @param x The char16 array this object will contain.
	 */
	explicit CIMValue(const Char16Array& x);
	/**
	 * Create a new CIMValue that represents a uint8 array data type.
	 * @param x The uint8 array this object will contain.
	 */
	explicit CIMValue(const UInt8Array& x);
	/**
	 * Create a new CIMValue that represents a sint8 array data type.
	 * @param x The sint8 array this object will contain.
	 */
	explicit CIMValue(const Int8Array& x);
	/**
	 * Create a new CIMValue that represents a uint16 array data type.
	 * @param x The uint16 array this object will contain.
	 */
	explicit CIMValue(const UInt16Array& x);
	/**
	 * Create a new CIMValue that represents a sint16 array data type.
	 * @param x The sint16 array this object will contain.
	 */
	explicit CIMValue(const Int16Array& x);
	/**
	 * Create a new CIMValue that represents a uint32 array data type.
	 * @param x The uint32 array this object will contain.
	 */
	explicit CIMValue(const UInt32Array& x);
	/**
	 * Create a new CIMValue that represents a sint32 array data type.
	 * @param x The sint32 array this object will contain.
	 */
	explicit CIMValue(const Int32Array& x);
	/**
	 * Create a new CIMValue that represents a uint64 array data type.
	 * @param x The uint64 array this object will contain.
	 */
	explicit CIMValue(const UInt64Array& x);
	/**
	 * Create a new CIMValue that represents a sint64 array data type.
	 * @param x The sint64 array this object will contain.
	 */
	explicit CIMValue(const Int64Array& x);
	/**
	 * Create a new CIMValue that represents a real32 array data type.
	 * @param x The real32 array this object will contain.
	 */
	explicit CIMValue(const Real64Array& x);
	/**
	 * Create a new CIMValue that represents a real64 array data type.
	 * @param x The real64 array this object will contain.
	 */
	explicit CIMValue(const Real32Array& x);
	/**
	 * Create a new CIMValue that represents a string array data type.
	 * @param x The string array this object will contain.
	 */
	explicit CIMValue(const StringArray& x);
	/**
	 * Create a new CIMValue that represents a CIM datetime array data type.
	 * @param x The datetime array this object will contain.
	 */
	explicit CIMValue(const CIMDateTimeArray& x);
	/**
	 * Create a new CIMValue that represents a CIM Object Path array.
	 * @param x The object path array this object will contain.
	 */
	explicit CIMValue(const CIMObjectPathArray& x);
	/**
	 * Create a new CIMValue that represents a embedded class array.
	 * @param x The class array this object will contain.
	 */
	explicit CIMValue(const CIMClassArray& x);
	/**
	 * Create a new CIMValue that represents a embedded instance array.
	 * @param x The instance array this object will contain.
	 */
	explicit CIMValue(const CIMInstanceArray& x);
private:
	// These are private/unimplemented to help prevent unintended errors of
	// passing a pointer to the constructor.
	explicit CIMValue(const void*);
	explicit CIMValue(void*);
	explicit CIMValue(volatile const void*);
	explicit CIMValue(volatile void*);
public:
	/**
	 * Destroy this CIMValue object.
	 */
	~CIMValue();
	/**
	 * @return The size of the array this CIMValue contains if it contains
	 * an array.
	 */
	UInt32 getArraySize() const;
	/**
	 * Get a boolean value from this CIMValue.
	 */
	void get(Bool& x) const;
	/**
	 * Get a char16 value from this CIMValue.
	 */
	void get(Char16& x) const;
	/**
	 * Get a uint8 value from this CIMValue.
	 */
	void get(UInt8& x) const;
	/**
	 * Get a sing8 value from this CIMValue.
	 */
	void get(Int8& x) const;
	/**
	 * Get a uint16 value from this CIMValue.
	 */
	void get(UInt16& x) const;
	/**
	 * Get a sint16 value from this CIMValue.
	 */
	void get(Int16& x) const;
	/**
	 * Get a uint32 value from this CIMValue.
	 */
	void get(UInt32& x) const;
	/**
	 * Get a sint32 value from this CIMValue.
	 */
	void get(Int32& x) const;
	/**
	 * Get a uint64 value from this CIMValue.
	 */
	void get(UInt64& x) const;
	/**
	 * Get a sint64 value from this CIMValue.
	 */
	void get(Int64& x) const;
	/**
	 * Get a real32 value from this CIMValue.
	 */
	void get(Real32& x) const;
	/**
	 * Get a real64 value from this CIMValue.
	 */
	void get(Real64& x) const;
	/**
	 * Get a string value from this CIMValue.
	 */
	void get(String& x) const;
	/**
	 * Get a CIM datetime value from this CIMValue.
	 */
	void get(CIMDateTime& x) const;
	/**
	 * Get a reference (Object path) value from this CIMValue.
	 */
	void get(CIMObjectPath& x) const;
	/**
	 * Get a embedded class value from this CIMValue.
	 */
	void get(CIMClass& x) const;
	/**
	 * Get a embedded instance value from this CIMValue.
	 */
	void get(CIMInstance& x) const;
	/**
	 * Get a reference (Object path) array value from this CIMValue.
	 */
	void get(CIMObjectPathArray& x) const;
	/**
	 * Get a char16 array from this CIMValue.
	 */
	void get(Char16Array& x) const;
	/**
	 * Get a uint8 array from this CIMValue.
	 */
	void get(UInt8Array& x) const;
	/**
	 * Get a sint8 array from this CIMValue.
	 */
	void get(Int8Array& x) const;
	/**
	 * Get a uint16 array from this CIMValue.
	 */
	void get(UInt16Array& x) const;
	/**
	 * Get a sint16 array from this CIMValue.
	 */
	void get(Int16Array& x) const;
	/**
	 * Get a uint32 array from this CIMValue.
	 */
	void get(UInt32Array& x) const;
	/**
	 * Get a sint32 array from this CIMValue.
	 */
	void get(Int32Array& x) const;
	/**
	 * Get a uint64 array from this CIMValue.
	 */
	void get(UInt64Array& x) const;
	/**
	 * Get a sint64 array from this CIMValue.
	 */
	void get(Int64Array& x) const;
	/**
	 * Get a real64 array from this CIMValue.
	 */
	void get(Real64Array& x) const;
	/**
	 * Get a real32 array from this CIMValue.
	 */
	void get(Real32Array& x) const;
	/**
	 * Get a string array from this CIMValue.
	 */
	void get(StringArray& x) const;
	/**
	 * Get a boolean array from this CIMValue.
	 */
	void get(BoolArray& x) const;
	/**
	 * Get a CIM datetime array from this CIMValue.
	 */
	void get(CIMDateTimeArray& x) const;
	/**
	 * Get a CIM class array from this CIMValue.
	 */
	void get(CIMClassArray& x) const;
	/**
	 * Get a CIM instance array from this CIMValue.
	 */
	void get(CIMInstanceArray& x) const;
	Bool toBool() const;
	Char16 toChar16() const;
	UInt8 toUInt8() const;
	Int8 toInt8() const;
	UInt16 toUInt16() const;
	Int16 toInt16() const;
	UInt32 toUInt32() const;
	Int32 toInt32() const;
	UInt64 toUInt64() const;
	Int64 toInt64() const;
	Real32 toReal32() const;
	Real64 toReal64() const;
	CIMDateTime toCIMDateTime() const;
	CIMObjectPath toCIMObjectPath() const;
	CIMClass toCIMClass() const;
	CIMInstance toCIMInstance() const;
	CIMObjectPathArray toCIMObjectPathArray() const;
	Char16Array toChar16Array() const;
	UInt8Array toUInt8Array() const;
	Int8Array toInt8Array() const;
	UInt16Array toUInt16Array() const;
	Int16Array toInt16Array() const;
	UInt32Array toUInt32Array() const;
	Int32Array toInt32Array() const;
	UInt64Array toUInt64Array() const;
	Int64Array toInt64Array() const;
	Real64Array toReal64Array() const;
	Real32Array toReal32Array() const;
	StringArray toStringArray() const;
	BoolArray toBoolArray() const;
	CIMDateTimeArray toCIMDateTimeArray() const;
	CIMClassArray toCIMClassArray() const;
	CIMInstanceArray toCIMInstanceArray() const;
	
	/**
	 * Set this to a null object.
	 */
	virtual void setNull();

	typedef COWIntrusiveReference<CIMValueImpl> CIMValue::*safe_bool;
	operator safe_bool () const
		{  return (m_impl) ? &CIMValue::m_impl : 0; }
	bool operator!() const
		{  return !m_impl; }
	/**
	 * Assign another CIMValue to this one.
	 * @param x The CIMValue to assign to this one.
	 * @return A reference to this CIMValue object.
	 */
	CIMValue& set(const CIMValue& x);
	/**
	 * Assignment operator
	 * @param x The CIMValue to assign to this one.
	 * @return A reference to this CIMValue object.
	 */
	CIMValue& operator= (const CIMValue& x);
	/**
	 * Determine equality of this CIMValue object with another.
	 * @param x The CIMValue object to check for equality with.
	 * @return true If this CIMValue is equal to the given CIMValue.
	 */
	bool equal(const CIMValue& x) const;
	/**
	 * Equality operator
	 * @param x The CIMValue to check for equality with.
	 * @return true if the specifed CIMValue is equal to this one. Otherwise
	 * false.
	 */
	bool operator== (const CIMValue& x) const;
	/**
	 * Inequality operator
	 * @param x The CIMValue to check for inequality with.
	 * @return true if the specifed CIMValue is not equal to this one.
	 * Otherwise false.
	 */
	bool operator!= (const CIMValue& x) const;
	/**
	 * Less than or equal operator
	 * @param x The CIMValue to compare this one with.
	 * @return true if The specifed CIMValue is less than or equal to this
	 * one. Otherwise false.
	 */
	bool operator<= (const CIMValue& x) const;
	/**
	 * Greater than or equal operator
	 * @param x The CIMValue to compare this one with.
	 * @return true if The specifed CIMValue is greater than or equal to this
	 * one. Otherwise false.
	 */
	bool operator>= (const CIMValue& x) const;
	/**
	 * Less than operator
	 * @param x The CIMValue to compare this one with.
	 * @return true if The specifed CIMValue is less than this one. Otherwise
	 * false.
	 */
	bool operator< (const CIMValue& x) const;
	/**
	 * Greater than operator
	 * @param x The CIMValue to compare this one with.
	 * @return true if The specifed CIMValue is greater than this one.
	 * Otherwise false.
	 */
	bool operator> (const CIMValue& x) const;
	/**
	 * @return The integral representation of the data type for this CIMValue.
	 * (see class CIMDataType)
	 */
	CIMDataType::Type getType() const;
	/**
	 * @return The type for this CIMValue.
	 * (see class CIMDataType)
	 */
	CIMDataType getCIMDataType() const;
	/**
	 * Determine if another CIMValue has the same data type as this one.
	 * @param x The CIMValue to compare this object's data type with.
	 * @return true If the datatypes are the same. Otherwise false.
	 */
	bool sameType(const CIMValue& x) const;
	/**
	 * @return true If this CIMValue contains an array.
	 */
	bool isArray() const;
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
	 * @return The string representation of this CIMValue object.
	 */
	virtual String toString() const;
	/**
	 * @return The MOF representation of this CIMValue as an String.
	 */
	virtual String toMOF() const;
	/**
	 * @return true if this CIMValue object contains a numeric type.
	 */
	bool isNumericType() const;
private:

#ifdef OWBI1_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	COWIntrusiveReference<CIMValueImpl> m_impl;

#ifdef OWBI1_WIN32
#pragma warning (pop)
#endif

};

} // end namespace OWBI1

#endif
