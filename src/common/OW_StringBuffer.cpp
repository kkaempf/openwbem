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
#include "OW_config.h"
#include "OW_StringBuffer.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_Char16.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMDateTime.hpp"
#include <cstring>
#include <cstdio>
#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif
#include <algorithm> // for std::swap

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
StringBuffer::StringBuffer(size_t allocSize) :
	m_len(0),
	m_allocated(allocSize > 0 ? allocSize : OW_DEFAULT_ALLOCATION_UNIT),
	m_bfr(new char[m_allocated])
{
	m_bfr[0] = 0;
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer::StringBuffer(const char* arg) :
	m_len(strlen(arg)),
	m_allocated(m_len + OW_DEFAULT_ALLOCATION_UNIT),
	m_bfr(new char[m_allocated])
{
	::strcpy(m_bfr, arg);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer::StringBuffer(const String& arg) :
	m_len(arg.length()),
	m_allocated(m_len + OW_DEFAULT_ALLOCATION_UNIT),
	m_bfr(new char[m_allocated])
{
	::strcpy(m_bfr, arg.c_str());
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer::StringBuffer(const StringBuffer& arg) :
	m_len(arg.m_len), m_allocated(arg.m_allocated),
	m_bfr(new char[arg.m_allocated])
{
	::memmove(m_bfr, arg.m_bfr, arg.m_len + 1);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator= (const String& arg)
{
	StringBuffer(arg).swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator= (const char* str)
{
	StringBuffer(str).swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator =(StringBuffer arg)
{
	arg.swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
StringBuffer::swap(StringBuffer& x)
{
	std::swap(m_len, x.m_len);
	std::swap(m_allocated, x.m_allocated);
	std::swap(m_bfr, x.m_bfr);
}
//////////////////////////////////////////////////////////////////////////////
void
StringBuffer::reset()
{
	m_len = 0;
	m_bfr[0] = '\0';
}
//////////////////////////////////////////////////////////////////////////////
char
StringBuffer::operator[] (size_t ndx) const
{
	return (ndx > m_len) ? 0 : m_bfr[ndx];
}
//////////////////////////////////////////////////////////////////////////////
// This operator must write "TRUE"/"FALSE" to support the CIMValue toXML
StringBuffer&
StringBuffer::operator += (Bool v)
{
	return append(v.toString());
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (const CIMDateTime& arg)
{
	return append(arg.toString());
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (const CIMObjectPath& arg)
{
	return append(arg.toString());
}
#if defined(OW_WIN32)
#define snprintf _snprintf // stupid windoze...
#endif
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (UInt8 v)
{
	char bfr[6];
	::snprintf(bfr, sizeof(bfr), "%u", UInt32(v));
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (Int8 v)
{
	char bfr[6];
	::snprintf(bfr, sizeof(bfr), "%d", Int32(v));
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (UInt16 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%u", UInt32(v));
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (Int16 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%d", Int32(v));
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (UInt32 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%u", v);
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (Int32 v)
{
	char bfr[16];
	::snprintf(bfr, sizeof(bfr), "%d", v);
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (UInt64 v)
{
	char bfr[28];
#if OW_SIZEOF_LONG_INT == 8
	::snprintf(bfr, sizeof(bfr), "%lu", v);
#else
	::snprintf(bfr, sizeof(bfr), "%llu", v);
#endif
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (Int64 v)
{
	char bfr[28];
#if OW_SIZEOF_LONG_INT == 8
	::snprintf(bfr, sizeof(bfr), "%ld", v);
#else
	::snprintf(bfr, sizeof(bfr), "%lld", v);
#endif
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (Real32 v)
{
	char bfr[32];
	::snprintf(bfr, sizeof(bfr), "%f", v);
	return append(bfr);
}
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::operator += (Real64 v)
{
	char bfr[32];
	::snprintf(bfr, sizeof(bfr), "%f", v);
	return append(bfr);
}
#if defined(OW_WIN32)
#undef snprintf
#endif
//////////////////////////////////////////////////////////////////////////////
StringBuffer&
StringBuffer::append(const char* str, const size_t len)
{
	checkAvail(len+1);
	::strncpy(m_bfr+m_len, str, len);
	m_len += len;
	m_bfr[m_len] = '\0';
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
bool
StringBuffer::equals(const char* arg) const
{
	return strcmp(arg, m_bfr) == 0;
}
//////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& ostr, const StringBuffer& b)
{
	ostr << b.c_str();
	return ostr;
}

} // end namespace OpenWBEM

