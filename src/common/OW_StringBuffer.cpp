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

#include "OW_config.h"
#include "OW_StringBuffer.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_Char16.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMDateTime.hpp"
#include <cstring>
#include <cstdio>

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer::OW_StringBuffer(int allocSize) :
	m_len(0), m_incSize(allocSize > 0 ? allocSize : DEFAULT_ALLOCATION_UNIT),
	m_allocated(m_incSize), m_bfr(0)
{
	m_bfr = new char[m_allocated];
	m_bfr[0] = 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer::OW_StringBuffer(const OW_String& arg) :
	m_len(arg.length()), m_incSize(m_len + DEFAULT_ALLOCATION_UNIT),
	m_allocated(m_incSize), m_bfr(0)
{
	m_bfr = new char[m_allocated];
	::strcpy(m_bfr, arg.c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer::OW_StringBuffer(const OW_StringBuffer& arg) :
	m_len(arg.m_len), m_incSize(arg.m_incSize), m_allocated(arg.m_allocated),
	m_bfr(0)
{
	m_bfr = new char[m_allocated];
	::memmove(m_bfr, arg.m_bfr, m_allocated);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer&
OW_StringBuffer::operator= (const OW_String& arg)
{
	int len = arg.length();
	clear(len);
	if(len > 0)
	{
		m_len = len;
		::memmove(m_bfr, arg.c_str(), len);
		m_bfr[m_len] = (char)0;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator= (const char* str)
{
	if(str == NULL || *str == (char)0)
	{
		clear();
		return *this;
	}

	int len = ::strlen(str);
	clear(len);
	::strcpy(m_bfr, str);
	m_len = len;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer&
OW_StringBuffer::operator =(const OW_StringBuffer& arg)
{
	return operator =(arg.c_str());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_StringBuffer::reset()
{
	m_len = 0;
	m_bfr[0] = (char)0;
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_StringBuffer::clear(int newSize)
{
	m_len = 0;
	int toAlloc = m_incSize;
	if(newSize > 0)
	{
		if(newSize >= m_allocated)
		{
			checkAvail(newSize+1);
			m_bfr[0] = (char)0;
			return;
		}
		else
		{
			if(newSize > m_incSize)
			{
				toAlloc = newSize / m_incSize;
				if(newSize % m_incSize)
					toAlloc++;

				toAlloc *= m_incSize;
			}
		}
	}

	if(toAlloc != m_allocated)
	{
		delete[] m_bfr;
		m_bfr = new char[toAlloc];
		m_allocated = toAlloc;
	}
	m_bfr[0] = (char)0;
}

//////////////////////////////////////////////////////////////////////////////
char 
OW_StringBuffer::operator[] (int ndx) const
{
	return (ndx > m_len) ? 0 : m_bfr[ndx];
}

//////////////////////////////////////////////////////////////////////////////
// This operator must write "TRUE"/"FALSE" to support the CIMValue toXML
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_Bool v)
{
	return append(v.toString());
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (const OW_CIMDateTime& arg) 
{ 
	return append(arg.toString());
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (const OW_CIMObjectPath& arg) 
{ 
	return append(arg.toString());
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_UInt8 v)
{
	char bfr[6];
	::snprintf(bfr, sizeof(bfr), "%u", OW_UInt32(v));
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_Int8 v)
{
	char bfr[6];
	::snprintf(bfr, sizeof(bfr), "%d", OW_Int32(v));
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_UInt16 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%u", OW_UInt32(v));
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_Int16 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%d", OW_Int32(v));
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_UInt32 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%u", v);
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_Int32 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%d", v);
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_UInt64 v)
{
	char bfr[28];
	::snprintf(bfr, sizeof(bfr), "%llu", v);
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_Int64 v)
{
	char bfr[28];
	::snprintf(bfr, sizeof(bfr), "%lld", v);
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_Real32 v)
{
	char bfr[32];
	::snprintf(bfr, sizeof(bfr), "%f", v);
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::operator += (OW_Real64 v)
{
	char bfr[32];
	::snprintf(bfr, sizeof(bfr), "%f", v);
	return append((const char*)bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer& 
OW_StringBuffer::append(char c)
{
	checkAvail();
	m_bfr[m_len++] = c;
	m_bfr[m_len] = (char)0;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer&
OW_StringBuffer::append(const char* str)
{
	int len = ::strlen(str);
	checkAvail(len+1);
	::strcpy(m_bfr+m_len, str);
	m_len += len;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringBuffer&
OW_StringBuffer::append(const char* str, const size_t len)
{
	checkAvail(len+1);
	::strncpy(m_bfr+m_len, str, len);
	m_len += len;
    m_bfr[m_len] = '\0';
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_StringBuffer::checkAvail(int len)
{
	int freeSpace = m_allocated - (m_len+1);

	if(len > freeSpace)
	{
		int nlen = len - freeSpace;
		int neededBlocks = nlen / m_incSize;
		if(nlen % m_incSize)
			neededBlocks++;

		int toalloc = m_allocated + (neededBlocks * m_incSize);
		char* bfr = new char[toalloc];
		::memmove(bfr, m_bfr, m_allocated);
		delete [] m_bfr;
		m_allocated = toalloc;
		m_bfr = bfr;
	}
}

//////////////////////////////////////////////////////////////////////////////
bool 
OW_StringBuffer::equals(const char* arg) const
{
	return strcmp(arg, m_bfr) == 0;
}

