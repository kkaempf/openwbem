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

#ifndef __OW_STRINGBUFFER_HPP__
#define __OW_STRINGBUFFER_HPP__

#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Char16.hpp"

class OW_CIMDateTime;
class OW_CIMObjectPath;

class OW_StringBuffer
{
public:
	static const int DEFAULT_ALLOCATION_UNIT = 128;

	OW_StringBuffer(int allocSize = DEFAULT_ALLOCATION_UNIT);
	OW_StringBuffer(const OW_String& arg);
	OW_StringBuffer(const OW_StringBuffer& arg);
	~OW_StringBuffer() { delete [] m_bfr; }

	OW_StringBuffer& operator= (const OW_StringBuffer& arg);
	OW_StringBuffer& operator= (const OW_String& arg);
	OW_StringBuffer& operator= (const char* str);

	OW_StringBuffer& append(char c);
	OW_StringBuffer& append(const char* str);
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
		{ return append(char(c)); }
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

	OW_StringBuffer& operator<< (char c) 
		{ return append(c); }
	OW_StringBuffer& operator<< (OW_Char16 c) 	
		{ return append(char(c)); }
	OW_StringBuffer& operator<< (const char* str) 
		{ return append(str); }
	OW_StringBuffer& operator<< (const OW_String& arg) 
		{ return append(arg); }
	OW_StringBuffer& operator<< (OW_Bool v) 
		{ return operator+=(v); }
	OW_StringBuffer& operator<< (OW_UInt8 v) 
		{  return operator+=(v); }
	OW_StringBuffer& operator<< (OW_Int8 v) 
		{ return operator+=(v); }
	OW_StringBuffer& operator<< (OW_UInt16 v) 
		{  return operator+=(v); }
	OW_StringBuffer& operator<< (OW_Int16 v) 
		{  return operator+=(v); }
	OW_StringBuffer& operator<< (OW_UInt32 v) 
		{ return operator+=(v); }
	OW_StringBuffer& operator<< (OW_Int32 v) 
		{ return operator+=(v); }
	OW_StringBuffer& operator<< (OW_UInt64 v) 
		{ return operator+=(v); }
	OW_StringBuffer& operator<< (OW_Int64 v) 
		{ return operator+=(v); }
	OW_StringBuffer& operator<< (OW_Real32 v) 
		{ return operator+=(v); }
	OW_StringBuffer& operator<< (OW_Real64 v) 
		{ return operator+=(v); }

	OW_StringBuffer& operator<< (const OW_CIMDateTime& arg) 
	{ 
		return operator+=(arg);
	}

	OW_StringBuffer& operator<< (const OW_CIMObjectPath& arg) 
	{ 
		return operator+=(arg);
	}

	char operator[] (int ndx) const;
	OW_String toString() const 
			{ return OW_String(m_bfr); }

	int length() const {  return m_len; }
	int allocated() const {  return m_allocated; }
	int getIncSize() const {  return m_incSize; }
	void clear(int newSize=0);
	void reset();
	const char* c_str() const {  return m_bfr; }

private:
	void checkAvail(int len=1);

	int m_len;
	int m_incSize;
	int m_allocated;
	char* m_bfr;
};

#endif	// __OW_STRINGBUFFER_HPP__

