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

#ifndef OW_STRINGBUFFER_HPP_INCLUDE_GUARD_
#define OW_STRINGBUFFER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Char16.hpp"
#include "OW_Bool.hpp"
#include <iosfwd>
#include <cstring>

namespace OpenWBEM
{

class CIMDateTime;
class CIMObjectPath;
class StringBuffer
{
public:
#if defined(OW_AIX)
	static const size_t OW_DEFAULT_ALLOCATION_UNIT;
#else
	static const size_t OW_DEFAULT_ALLOCATION_UNIT = 128;  
#endif // OW_AIX
	StringBuffer(size_t allocSize = OW_DEFAULT_ALLOCATION_UNIT);
	StringBuffer(const char* arg);
	StringBuffer(const String& arg);
	StringBuffer(const StringBuffer& arg);
	~StringBuffer() { delete [] m_bfr; }
	StringBuffer& operator= (const StringBuffer& arg);
	StringBuffer& operator= (const String& arg);
	StringBuffer& operator= (const char* str);
	void swap(StringBuffer& x);
	StringBuffer& append(char c)
	{
		checkAvail();
		m_bfr[m_len++] = c;
		m_bfr[m_len] = '\0';
		return *this;
	}
	StringBuffer& append(const char* str)
	{
		size_t len = ::strlen(str);
		checkAvail(len+1);
		::strcpy(m_bfr+m_len, str);
		m_len += len;
		return *this;
	}
	StringBuffer& append(const char* str, const size_t len);
	StringBuffer& append(const String& arg) 	
		{ return append(arg.c_str()); }
	StringBuffer& append(const StringBuffer& arg)
	{
		return append(arg.c_str());
	}
	StringBuffer& operator += (char c)
		{ return append(c); }
	StringBuffer& operator += (Char16 c)
		{ return append(c.toString()); }
	StringBuffer& operator += (const char* str)
		{ return append(str); }
	StringBuffer& operator += (const String& arg)
		{ return append(arg); }
	StringBuffer& operator += (Bool v);
	StringBuffer& operator += (UInt8 v);
	StringBuffer& operator += (Int8 v);
	StringBuffer& operator += (UInt16 v);
	StringBuffer& operator += (Int16 v);
	StringBuffer& operator += (UInt32 v);
	StringBuffer& operator += (Int32 v);
	StringBuffer& operator += (UInt64 v);
	StringBuffer& operator += (Int64 v);
	StringBuffer& operator += (Real32 v);
	StringBuffer& operator += (Real64 v);
	/**
	 * DEPRECATED in favor of CIMDateTime::toString() in 3.1.0.
	 */
	StringBuffer& operator += (const CIMDateTime& arg) OW_DEPRECATED;
	/**
	 * DEPRECATED in favor of CIMObjectPath::toString() in 3.1.0.
	 */
	StringBuffer& operator += (const CIMObjectPath& arg) OW_DEPRECATED;
	StringBuffer& operator += (const StringBuffer& arg)
	{
		return append(arg);
	}
	char operator[] (size_t ndx) const;
	String toString() const
			{ return String(m_bfr); }
	// After calling this function, the StringBuffer is unusable
	String releaseString()
	{
		char * bfr = m_bfr;
		m_bfr = 0;
		return String(String::E_TAKE_OWNERSHIP, bfr, m_len);
	}
	size_t length() const {  return m_len; }

	/**
	 * Truncate the string at the given index. This doesn't change anything
	 * before the given index and does not free up any memory.
	 * @param index The index to truncate the string buffer at. If this value
	 * is greater than the current length of the StringBuffer, then nothing
	 * is done.
	 */
	void truncate(size_t index);

	/**
	 * Fill this StringBuffer object with the next line from the given
	 * input stream.
	 * @param is The input stream to retrieve the next line from
	 * @return const char* pointer to the line of text just read.
	 */
	const char* getLine(std::istream& is, bool resetBuffer=true);

	bool endsWith(char ch) const;
	bool startsWith(char ch) const;
	void chop();
	void trim();

	size_t allocated() const {  return m_allocated; }
	void reset();
	const char* c_str() const {  return m_bfr; }
	bool equals(const char* arg) const;
	friend std::ostream& operator<<(std::ostream& ostr, const StringBuffer& b);
private:
	void checkAvail(size_t len=1)
	{
		size_t freeSpace = m_allocated - (m_len+1);
	
		if (len > freeSpace)
		{
			size_t toalloc = m_allocated * 2 + len;
			char* bfr = new char[toalloc];
			::memmove(bfr, m_bfr, m_len);
			delete [] m_bfr;
			m_allocated = toalloc;
			m_bfr = bfr;
		}
	}
	size_t m_len;
	size_t m_allocated;
	char* m_bfr;
};

} // end namespace OpenWBEM

typedef OpenWBEM::StringBuffer OW_StringBuffer OW_DEPRECATED; // in 3.0.0

#endif
