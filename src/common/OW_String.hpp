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

#ifndef OW_STRING_HPP_
#define OW_STRING_HPP_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Reference.hpp"
#include "OW_ArrayFwd.hpp"
#include "OW_Exception.hpp"

#include <iosfwd>

class OW_IOException;
class OW_Char16;
typedef OW_Array<OW_Char16> OW_Char16Array;

class OW_CIMDateTime;
class OW_CIMObjectPath;

class OW_String;
typedef OW_Array<OW_String> OW_StringArray;

DEFINE_EXCEPTION(StringConversion);

/**
 * This OW_String class is an abstract data type that represents as NULL
 * terminated string of characters. OW_String objects are ref counted and
 * copy on write.
 */
class OW_String
{
public:
	class ByteBuf;

	/**
	 * Create a new OW_String object with a length of 0.
	 */
	OW_String();

	/**
	 * Create a new OW_String object that will hold the string representation
	 * of the given OW_Int32 value.
	 * @param val	The 32 bit value this OW_String will hold the string
	 * representation of.
	 */
	explicit OW_String(OW_Int32 val);

	/**
	 * Create a new OW_String object that will hold the string representation
	 * of the given OW_UInt32 value.
	 * @param val	The 32 bit value this OW_String will hold the string
	 * representation of.
	 */
	explicit OW_String(OW_UInt32 val);

	/**
	 * Create a new OW_String object that will hold the string representation
	 * of the given OW_Int64 value.
	 * @param val	The 64 bit value this OW_String will hold the string
	 * representation of.
	 */
	explicit OW_String(OW_Int64 val);

	/**
	 * Create a new OW_String object that will hold the string representation
	 * of the given OW_UInt64 value.
	 * @param val	The 64 bit value this OW_String will hold the string
	 * representation of.
	 */
	explicit OW_String(OW_UInt64 val);

	/**
	 * Create a new OW_String object that will hold the string representation
	 * of the given OW_Real64 value.
	 * @param val	The real value this OW_String will hold the string
	 * representation of.
	 */
	explicit OW_String(OW_Real64 val);

	/**
	 * Create a new OW_String object that will contain a copy of the given
	 * character string.
	 * @param str	The null terminated string to make a copy of.
	 */
	OW_String(const char* str);

	/**
	 * ATTN: UTF8
	 * Create a new OW_String object that will contain a single byte character
	 * representation of the OW_Char16Array.
	 * @param ra	The OW_Char16Array to use to construct the string
	 */
	explicit OW_String(const OW_Char16Array& ra);

	/**
	 * Create a new OW_String object that will contain the representation of
	 * a OW_Bool value ("TRUE" "FALSE").
	 * @param parm	The OW_Bool value this string will represent.
	 */
	explicit OW_String(OW_Bool parm);

	/**
	 * ATTN: UTF8
	 * Create a new OW_String object that will contain the representation of
	 * an OW_Char16 object
	 * @param parm	The OW_Char16 object this string will represent.
	 */
	explicit OW_String(const OW_Char16& parm);

	/**
	 * Create a new OW_String object that will contain the representation of
	 * an OW_CIMDateTime object
	 * @param parm the OW_CIMDateTime object this string will represent.
	 */
	explicit OW_String(const OW_CIMDateTime& parm);

	/**
	 * Create a new OW_String object that will contain the representation of
	 * an OW_CIMObjectPath object.
	 * @param parm the OW_CIMObject path object this string will represent.
	 */
	explicit OW_String(const OW_CIMObjectPath& parm);

	/**
	 * Create a new string object from a dynamically allocated buffer.
	 * The buffer is assumed to contain a valid c string and be
	 * previously allocated with the new operator. The memory given by the
	 * allocatedMemory parameter will be deallocated by the OW_String class.
	 * @param takeOwnerShipTag This parm is not used. It is here to differentiate
	 *									this constructor from the others.
	 * @param allocatedMemory	The dynamically allocated string that will be
	 *									used by this OW_String.  Must not be NULL.
	 * @param len The length of the string allocatedMemory.
	 */
	explicit OW_String(OW_Bool takeOwnerShipTag, char* allocatedMemory, size_t len);

	/**
	 * Create s new OW_String object that will contain a copy of the given
	 * character string for the given length.
	 * @param str	The character array to copy from.
	 * @param len	The number of bytes to copy from the str param.
	 */
	explicit OW_String(const char* str, size_t len);

	/**
	 * Create a new OW_String object that is a copy of another OW_String object.
	 * Upon return, both OW_String objects will point to the same underlying
	 * character buffer. This state will remain until one of the 2 OW_String
	 * objects is modified (copy on write)
	 * @param arg	The OW_String object to make a copy of.
	 */
	OW_String(const OW_String& arg);

	/**
	 * Create a new OW_String object that contains a single character.
	 * @param c		The character that this string will contain.
	 */
	explicit OW_String(char c);

	/**
	 * Destroy this OW_String object.
	 */
	~OW_String();

	/**
	 * Create a null terminated string that contains the contents of this
	 * OW_String. The returned pointer MUST be deleted by the caller using free().
	 * @return A pointer to the newly allocated buffer that contains the
	 *		contents of this OW_String object.
	 */
	char* allocateCString() const;

	/**
	 * @return The length of this OW_String.
	 */
	size_t length() const;

	/**
	 * Format this string according to the given format and variable
	 * argument list (printf style)
	 * @param fmt	The format to use during the operation
	 * @param ...	A variable argument list to be applied to the format
	 * @return The length of the string after the operation on success.
	 * -1 on error.
	 */
	int format(const char* fmt, ...);

	/**
	 * Tokenize this OW_String object using the given delimeters.
	 * @param delims	A pointer to a char array of delimeters that separate
	 * 	the tokens in this OW_String object.
	 * @param returnTokens If this flag is true, then the delimiter characters
	 * 	are also returned as tokens.
	 * @return An OW_StringArray that contains the tokens from this OW_String
	 * object. If there are no tokens the OW_StringArray will be empty.
	 */
	OW_StringArray tokenize(const char* delims = " \n\r\t\v",
		OW_Bool returnTokens=false) const;

	/**
	 * @return The c string representation of this OW_String object. This
	 * will be a null terminated character array.
	 */
	const char* c_str() const;

	/**
	 * @return A pointer to the underlying character buffer. Identical to c_str.
	 */
	const char* getBytes() const {  return c_str(); }

	/**
	 * Get the character at a specified index.
	 * @param ndx	The index of the character to return within the char array.
	 * @return The character at the given index within the array. If the index
	 * is out of bounds, then a 0 is returned.
	 */
	char charAt(size_t ndx) const;

	/**
	 * Compare another OW_String object with this one.
	 * @param arg	The OW_String object to compare with this one.
	 * @return 0 if this OW_String object is equal to arg. Greater than 0 if
	 * this OW_String object is greater than arg. Less than 0 if this OW_String
	 * object is less than arg.
	 */
	int compareTo(const OW_String& arg) const;

	/**
	 * Compare another OW_String object with this one ignoring case.
	 * @param arg	The OW_String object to compare with this one.
	 * @return 0 if this OW_String object is equal to arg. Greater than 0 if
	 * this OW_String object is greater than arg. Less than 0 if this OW_String
	 * object is less than arg.
	 */
	int compareToIgnoreCase(const OW_String& arg) const;

	/**
	 * Append another OW_String object to this OW_String object.
	 * @param arg	The OW_String object to append to this OW_String object.
	 * @return A reference to this OW_String object.
	 */
	OW_String& concat(const OW_String& arg);

	/**
	 * Determine if this OW_String object ends with the same string
	 * represented by another OW_String object.
	 * @param arg	The OW_String object to search the end of this OW_String
	 * 	object for.
	 * @param ignoreCase If true, character case will be ignored.
	 * @return true if this OW_String ends with the given OW_String. Otherwise
	 * return false.
	 */
	OW_Bool endsWith(const OW_String& arg, OW_Bool ignoreCase=false) const;

	/**
	 * Determine if another OW_String object is equal to this OW_String object.
	 * @param arg The OW_String object to check this OW_String object against
	 * 	for equality.
	 * @return true if this OW_String object is equal to the given OW_String
	 * object. Otherwise return false.
	 */
	OW_Bool equals(const OW_String& arg) const;

	/**
	 * Determine if another OW_String object is equal to this OW_String object,
	 * ignoring case in the comparision.
	 * @param arg	The OW_String object to check this OW_String object against
	 * 	for equality.
	 * @return true if this OW_String object is equal to the given OW_String
	 * object. Otherwise return false.
	 */
	OW_Bool equalsIgnoreCase(const OW_String& arg) const;

	/**
	 * @return a 32 bit hashcode of this OW_String object.
	 */
	OW_UInt32 hashCode() const;

	/**
	 * Find the first occurence of a given character in this OW_String object.
	 * @param ch The character to search for.
	 * @param fromIndex The index to start the search from.
	 * @return The index of the given character in the underlying array if it
	 * was found. -1 if the character was not found at or after the given
	 * fromIndex.
	 */
	int indexOf(char ch, int fromIndex=0) const;

	/**
	 * Find the first occurence of a string in this OW_String object.
	 * @param arg			The string to search for.
	 * @param fromIndex	The index to start the search from.
	 * @return The index of the given string in the underlying array if it
	 * was found. -1 if the string was not found at or after the fromIndex.
	 */
	int indexOf(const OW_String& arg, int fromIndex=0) const;

	/**
	 * Find the last occurence of a character in this OW_String object.
	 * @param ch			The character to search for.
	 * @param fromIndex	The index to start the search from.
	 * @return The index of the last occurence of the given character in the
	 * underlying array if it was found. -1 if this OW_String object does not
	 * contain the character before the given fromIndex.
	 */
	int lastIndexOf(char ch, int fromIndex=-1) const;

	/**
	 * Find the last occurence of a string in this OW_String object.
	 * @param arg			The string to search for.
	 * @param fromIndex	The index to start the search from.
	 * @return The index of the last occurence of the string in the underlying
	 * array if it was found. -1 if the this OW_String does not contain the
	 * given substring before the fromIndex.
	 */
	int lastIndexOf(const OW_String& arg, int fromIndex=-1) const;

	/**
	 * Determine if this OW_String object starts with a given substring.
	 * @param arg The OW_String object to search the beginning of this OW_String
	 * object for.
	 * @param ignoreCase	If true, case of the characters will be ignored.
	 * @return true if this OW_String object starts with the given string.
	 * Otherwise false.
	 */
	OW_Bool startsWith(const OW_String& arg, OW_Bool ignoreCase=false) const;

	/**
	 * Create another OW_String object that is comprised of a substring of this
	 * OW_String object.
	 * @param beginIndex		The start index of the substring.
	 * @param length			The length of the substring to use.
	 * @return A new OW_String object representing the substring of this
	 * OW_String object.
	 */
	OW_String substring(size_t beginIndex,
		size_t length=static_cast<size_t>(-1)) const;

	/**
	 * @return true if this OW_String object contains nothing but space
	 * characters.
	 */
	OW_Bool isSpaces() const;

	/**
	 * Convert this OW_String object to lower case characters.
	 * @return A reference to this OW_String object after the conversion has
	 * taken place.
	 */
	OW_String& toLowerCase();

	/**
	 * Convert this OW_String object to upper case characters.
	 * @return A reference to this OW_String object after the conversion has
	 * taken place.
	 */
	OW_String& toUpperCase();

	/**
	 * Strip all leading space characters from this OW_String object.
	 * @return A reference to this OW_String object after the operation has
	 * taken place.
	 */
	OW_String& ltrim();

	/**
	 * Strip all trailing space characters from this OW_String object.
	 * @return A reference to this OW_String object after the operation has
	 * taken place.
	 */
	OW_String& rtrim();

	/**
	 * Strip all leading and trailing space characters from this OW_String
	 * object.
	 * @return A reference to this OW_String object after the operation has
	 * taken place.
	 */
	OW_String& trim();

	/**
	 * Delete all the characters of the string.  Thus, the string is empty
	 * after the call.
	 * @return A referernce to this object.
	 */
	OW_String& erase();
	
	/**
	 * Erases, at most, len characters of *this, starting at index idx.
	 * if len == -1, erase to the end of the string.
	 * @return A referernce to this object.
	 */
	OW_String& erase( size_t idx, size_t len = -1 );

	/**
	 * Assignment operator.
	 * @param arg		The OW_String object to assign to this OW_String.
	 * @return A reference to this OW_String object after the assignment has
	 * taken place.
	 */
	OW_String& operator= (const OW_String & arg);

	/**
	 * Operator [].
	 * @param ndx		The index of the character to retrieve from the underlying
	 * 					character array.
	 * @return The character at the given index within the underlying character
	 * array. If ndx is out of bounds, 0 is returned.
	 */
	char operator[] (size_t ndx) const;

	/**
	 * Concatination operator.
	 * @param arg	The OW_String to append to the end of this OW_String.
	 * @return A reference to this OW_String object after the concatination has
	 * taken place.
	 */
	OW_String& operator+= (const OW_String& arg) { return concat(arg); }

	/**
	 * Read this OW_String object from the given istream. An OW_String must have
	 * been previously written to this location of the stream.
	 * @param istrm	The input stream to read this OW_String from.
	 * @exception OW_IOException
	 */
	void readObject(std::istream& istrm);

	/**
	 * Write this OW_String object to the given ostream.
	 * @param ostrm	The output stream to write this OW_String to.
	 * @exception OW_IOException
	 */
	void writeObject(std::ostream& ostrm) const;

	/**
	 * @return A copy of this OW_String object.
	 */
	OW_String toString() const;

	/**
	 * @return the Char16 value for this OW_String object.
	 */
	OW_Char16 toChar16() const;

	/**
	 * @return The OW_Real32 value of this OW_String object.
	 */
	OW_Real32 toReal32() const;

	/**
	 * @return The OW_Real64 value of this OW_String object.
	 */
	OW_Real64 toReal64() const;

	/**
	 * @return The boolean value of this OW_String object. The string is
	 * assumed to be the value of "TRUE"/"FALSE" ignoring case.
	 */
	OW_Bool toBool() const;

	/**
	 * @return The OW_UInt8 value of this OW_String object.
	 */
	OW_UInt8 toUInt8(int base=10) const;

	/**
	 * @return The OW_Int8 value of this OW_String object.
	 */
	OW_Int8 toInt8(int base=10) const;

	/**
	 * @return The OW_UInt16 value of this OW_String object.
	 */
	OW_UInt16 toUInt16(int base=10) const;

	/**
	 * @return The OW_Int16 value of this OW_String object.
	 */
	OW_Int16 toInt16(int base=10) const;

	/**
	 * @return The OW_UInt32 value of this OW_String object.
	 */
	OW_UInt32 toUInt32(int base=10) const;

	/**
	 * @return The OW_Int32 value of this OW_String object.
	 */
	OW_Int32 toInt32(int base=10) const;

	/**
	 * @return The OW_UInt64 value of this OW_String object.
	 */
	OW_UInt64 toUInt64(int base=10) const;

	/**
	 * @return The OW_Int64 value of this OW_String object.
	 */
	OW_Int64 toInt64(int base=10) const;

	/**
	 * @return The OW_CIMDateTime value of this OW_String object.
	 */
	OW_CIMDateTime toDateTime() const;

	/**
	 * Convert a null terminated string to an unsigned 64 bit value.
	 * @param nptr A pointer to beginning of string to convert.
	 * @param endptr If not NULL, this function stores the address of the 1st
	 *		invalid character in *endptr.
	 * @param base A number that specifies the base for the number pointed to
	 *		by nptr.
	 * @return The unsigned 64 bit value of the number represented in string
	 * form pointed to by nptr.
	 */
	static unsigned long long int strtoull(const char* nptr, char** endptr,
		int base);

	/**
	 * Convert a null terminated string to an signed 64 bit value.
	 * @param nptr A pointer to beginning of string to convert.
	 * @param endptr If not NULL, this function stores the address of the 1st
	 *		invalid character in *endptr.
	 * @param base A number that specifies the base for the number pointed to
	 *		by nptr.
	 * @return The signed 64 bit value of the number represented in string
	 * form pointed to by nptr.
	 */
	static long long int strtoll(const char* nptr, char** endptr, int base);

	/**
	 * Find the first occurrence of a character in a null terminated string.
	 * @param theStr The string to search for the given character.
	 * @param c The character to to search for.
	 * @return a  pointer to the first occurrence of the character c in the
	 * string theStr if found. Otherwise a NULL pointer.
	 */
	static char* strchr(const char* theStr, int c);

	/**
	 * Reads from in input stream until a newline is encountered.
	 * This consumes the newline character (but doesn't put the newline in
	 * the return value), and leaves the file pointer at the char past
	 * the newline.
	 *
	 * @return an OW_String containing the line.
	 */
	static OW_String getLine(std::istream& istr);

private:

	OW_Reference<ByteBuf> m_buf;
};

std::ostream& operator<< (std::ostream& ostr, const OW_String& arg);
OW_String operator + (const OW_String& s1, const OW_String& s2);
OW_String operator + (const char* p, const OW_String& s);
OW_String operator + (const OW_String& s, const char* p);
OW_String operator + (char c, const OW_String& s);
OW_String operator + (const OW_String& s, char c);

inline OW_Bool
operator == (const OW_String& s1, const OW_String& s2)
{
	return (s1.compareTo(s2) == 0);
}

inline OW_Bool
operator == (const OW_String& s, const char* p)
{
	return (s.compareTo(p) == 0);
}

inline OW_Bool
operator == (const char* p, const OW_String& s)
{
	return (s.compareTo(p) == 0);
}

inline OW_Bool
operator != (const OW_String& s1, const OW_String& s2)
{
	return (s1.compareTo(s2) != 0);
}

inline OW_Bool
operator != (const OW_String& s, const char* p)
{
	return (s.compareTo(p) != 0);
}

inline OW_Bool
operator != (const char* p, const OW_String& s)
{
	return (s.compareTo(p) != 0);
}

inline OW_Bool
operator < (const OW_String& s1, const OW_String& s2)
{
	return (s1.compareTo(s2) < 0);
}

inline OW_Bool
operator < (const OW_String& s, const char* p)
{
	return (s.compareTo(p) < 0);
}

inline OW_Bool
operator < (const char* p, const OW_String& s)
{
	return (OW_String(p).compareTo(s) < 0);
}

inline OW_Bool
operator <= (const OW_String& s1, const OW_String& s2)
{
	return (s1.compareTo(s2) <= 0);
}

inline OW_Bool
operator <= (const OW_String& s, const char* p)
{
	return (s.compareTo(p) <= 0);
}

inline OW_Bool
operator <= (const char* p, const OW_String& s)
{
	return (OW_String(p).compareTo(s) <= 0);
}

inline OW_Bool
operator > (const OW_String& s1, const OW_String& s2)
{
	return (s1.compareTo(s2) > 0);
}

inline OW_Bool
operator > (const OW_String& s, const char* p)
{
	return (s.compareTo(p) > 0);
}

inline OW_Bool
operator > (const char* p, const OW_String& s)
{
	return (OW_String(p).compareTo(s) > 0);
}

inline OW_Bool
operator >= (const OW_String& s1, const OW_String& s2)
{
	return (s1.compareTo(s2) >= 0);
}

inline OW_Bool
operator >= (const OW_String& s, const char* p)
{
	return (s.compareTo(p) >= 0);
}

inline OW_Bool
operator >= (const char* p, const OW_String& s)
{
	return (OW_String(p).compareTo(s) >= 0);
}

#endif	// __OW_STRING_HPP__

