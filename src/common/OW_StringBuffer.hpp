/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_STRINGBUFFER_HPP_INCLUDE_GUARD_
#define OW_STRINGBUFFER_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Char16.hpp"
#include <cstring>

class OW_CIMDateTime;
class OW_CIMObjectPath;

class OW_StringBuffer
{
public:
	static const size_t DEFAULT_ALLOCATION_UNIT = 128;

	OW_StringBuffer(size_t allocSize = DEFAULT_ALLOCATION_UNIT);
	OW_StringBuffer(const char* arg);
	OW_StringBuffer(const OW_String& arg);
	OW_StringBuffer(const OW_StringBuffer& arg);
	~OW_StringBuffer() { delete [] m_bfr; }

	OW_StringBuffer& operator= (OW_StringBuffer arg);
	OW_StringBuffer& operator= (const OW_String& arg);
	OW_StringBuffer& operator= (const char* str);
	void swap(OW_StringBuffer& x);

	OW_StringBuffer& append(char c)
	{
		checkAvail();
		m_bfr[m_len++] = c;
		m_bfr[m_len] = '\0';
		return *this;
	}

	OW_StringBuffer& append(const char* str)
	{
		size_t len = ::strlen(str);
		checkAvail(len+1);
		::strcpy(m_bfr+m_len, str);
		m_len += len;
		return *this;
	}

	OW_StringBuffer& append(const char* str, const size_t len);
	OW_StringBuffer& append(const OW_String& arg) 	
		{ return append(arg.c_str()); }
	OW_StringBuffer& append(const OW_StringBuffer& arg)
	{
		return append(arg.c_str());
	}

	OW_StringBuffer& operator += (char c)
		{ return append(c); }
	OW_StringBuffer& operator += (OW_Char16 c)
		{ return append(c.toUTF8()); }
	OW_StringBuffer& operator += (const char* str)
		{ return append(str); }
	OW_StringBuffer& operator += (const OW_String& arg)
		{ return append(arg); }
	OW_StringBuffer& operator += (OW_Bool v);
	OW_StringBuffer& operator += (OW_UInt8 v);
	OW_StringBuffer& operator += (OW_Int8 v);
	OW_StringBuffer& operator += (OW_UInt16 v);
	OW_StringBuffer& operator += (OW_Int16 v);
	OW_StringBuffer& operator += (OW_UInt32 v);
	OW_StringBuffer& operator += (OW_Int32 v);
	OW_StringBuffer& operator += (OW_UInt64 v);
	OW_StringBuffer& operator += (OW_Int64 v);
	OW_StringBuffer& operator += (OW_Real32 v);
	OW_StringBuffer& operator += (OW_Real64 v);
	OW_StringBuffer& operator += (const OW_CIMDateTime& arg);
	OW_StringBuffer& operator += (const OW_CIMObjectPath& arg);
	OW_StringBuffer& operator += (const OW_StringBuffer& arg)
	{
		return append(arg);
	}


	char operator[] (size_t ndx) const;
	OW_String toString() const
			{ return OW_String(m_bfr); }

	// After calling this function, the OW_StringBuffer is unusable
	OW_String releaseString()
	{
		char * bfr = m_bfr;
		m_bfr = 0;
		return OW_String(OW_String::E_TAKE_OWNERSHIP, bfr, m_len);
	}

	size_t length() const {  return m_len; }
	size_t allocated() const {  return m_allocated; }
	void reset();
	const char* c_str() const {  return m_bfr; }
	bool equals(const char* arg) const;

	friend std::ostream& operator<<(std::ostream& ostr, const OW_StringBuffer& b);

private:
	void checkAvail(size_t len=1)
	{
		size_t freeSpace = m_allocated - (m_len+1);
	
		if(len > freeSpace)
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

#endif


