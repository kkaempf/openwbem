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
#include "OW_String.hpp"
#include "OW_Char16.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Array.hpp"
#include "OW_IOException.hpp"
#include "OW_StringStream.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <cerrno>
#include <iostream>

using std::istream;
using std::ostream;

//////////////////////////////////////////////////////////////////////////////
inline int
strcmpi(const char* s1, const char* s2)
{
	OW_String ls1(s1);
	OW_String ls2(s2);
	ls1.toUpperCase();
	ls2.toUpperCase();
	return ls1.compareTo(ls2);
}

//////////////////////////////////////////////////////////////////////////////
inline int
strncmpi(const char* s1, const char* s2, size_t n)
{
	OW_String ls1(s1, n);
	OW_String ls2(s2, n);
	ls1.toUpperCase();
	ls2.toUpperCase();
	return ls1.compareTo(ls2);
}


class OW_String::ByteBuf
{
public:

	ByteBuf(const char* s) :
		m_len(::strlen(s)), m_buf(NULL)
	{
		m_buf = new char[m_len+1];
		strcpy(m_buf, s);
	}

	ByteBuf(const ByteBuf& arg) :
		m_len(arg.m_len), m_buf(NULL)
	{
		m_buf = new char[m_len+1];
		strcpy(m_buf, arg.m_buf);
	}

	ByteBuf(char* bfr, size_t len)
		: m_len(len), m_buf(bfr)
	{
	}

	~ByteBuf() { delete [] m_buf; }
	ByteBuf& operator= (const ByteBuf& arg)
	{
		delete [] m_buf;
		m_len = arg.m_len;
		m_buf = new char[m_len+1];
		strcpy(m_buf, arg.m_buf);
		return *this;
	}

	size_t length() { return m_len; }
	char* data() { return m_buf; }

private:
	size_t m_len;
	char* m_buf;
};

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String() :
	m_buf(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_Bool parm) :
	m_buf(parm.toString().m_buf)
{
}

//////////////////////////////////////////////////////////////////////////////
// ATTN: UTF8
OW_String::OW_String(const OW_Char16& parm) :
	m_buf(NULL)
{
	char bfr[2];
	bfr[0] = (char)parm.getValue();
	bfr[1] = (char)0;
	m_buf = new ByteBuf(bfr);
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_Int32 val) :
	m_buf(NULL)
{
	char tmpbuf[32];
	int len = snprintf(tmpbuf, sizeof(tmpbuf), "%d", val);
	char* bfr = new char[len+1];
	::snprintf(bfr, len+1, "%d", val);
	m_buf = new ByteBuf(bfr, len);
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_UInt32 val) :
	m_buf(NULL)
{
	char tmpbuf[32];
	int len = ::snprintf(tmpbuf, sizeof(tmpbuf), "%u", val);
	char* bfr = new char[len+1];
	::snprintf(bfr, len+1, "%u", val);
	m_buf = new ByteBuf(bfr, len);
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_Int64 val) :
	m_buf(NULL)
{
	OW_StringStream ss(33);
	ss << val;
	m_buf = new ByteBuf(ss.c_str());

}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_UInt64 val) :
	m_buf(NULL)
{
	OW_StringStream ss(33);
	ss << val;
	m_buf = new ByteBuf(ss.c_str());

}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_Real64 val) :
	m_buf(NULL)
{
	OW_StringStream ss;
	ss << val;
	m_buf = new ByteBuf(ss.c_str());

}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(const char* str) :
	m_buf(NULL)
{
	if(NULL == str)
	{
		m_buf = 0;
	}
	else
	{
		m_buf = new ByteBuf(str);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_Bool /*takeOwnerShipTag*/, char* allocatedMemory) :
	m_buf(NULL)
{
	if(NULL == allocatedMemory)
	{
		m_buf = 0;
	}
	else
	{
		m_buf = new ByteBuf(allocatedMemory, ::strlen(allocatedMemory));
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(const char* str, size_t len) :
	m_buf(NULL)
{
	if(NULL == str)
	{
		m_buf = 0;
	}
	else
	{
		char* bfr = new char[len+1];
		::memcpy(bfr, str, len);
		bfr[len] = (char)0;
		m_buf = new ByteBuf(bfr, len);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(const OW_String& arg) :
	m_buf(arg.m_buf)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(const OW_CIMDateTime& parm) :
	m_buf(NULL)
{
	OW_String s = parm.toString();
	m_buf = s.m_buf;
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(const OW_CIMObjectPath& parm) :
	m_buf(NULL)
{
	OW_String s = parm.toString();
	m_buf = s.m_buf;
}

//////////////////////////////////////////////////////////////////////////////
// ATTN: UTF8
OW_String::OW_String(const OW_Char16Array& ra) :
	m_buf(NULL)
{
	int sz = ra.size();
	if(sz > 0)
	{
		char* bfr = new char[sz+1];
		for(int i = 0; i < sz; i++)
		{
			bfr[i] = (char)ra[i].getValue();
		}
		bfr[sz] = (char)0;
		m_buf = new ByteBuf(bfr, sz);
	}
	else
	{
		m_buf = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
// ATTN: UTF8 ?
OW_String::OW_String(char c) :
	m_buf(NULL)
{
	if(c != (char)0)
	{
		char bfr[2];
		bfr[0] = c;
		bfr[1] = (char)0;
		m_buf = new ByteBuf(bfr);
	}
	else
	{
		m_buf = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String::~OW_String()	
{
}

//////////////////////////////////////////////////////////////////////////////
char*
OW_String::allocateCString(size_t extraBytes) const
{
	size_t len = length() + + extraBytes + 1;
	char* str = new char[len];
	::memset(str, 0, len);
	::strcpy(str, c_str());
	return str;
}

//////////////////////////////////////////////////////////////////////////////
size_t
OW_String::length() const
{
	if (m_buf.getPtr())
	{
		return m_buf->length();
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::format(const char* fmt, ...)
{
	int n, size = 64;
	char *p;
	char *np;
	
	va_list ap;

	if ((p = new char[size]) == NULL)
	{
		return -1;
	}
	
	// Try to print in the allocated space
	while(true)
	{
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);                // If that worked, return the string.

		if(n > -1 && n < size)
		{
			m_buf = new ByteBuf(p, n);
			return (int)length();
		}

		if (n > -1)    // glibc 2.1
			size = n+1; // precisely what is needed
		else           // glibc 2.0
			size *= 2;  // twice the old size

		if((np = new char[size]) == NULL)
		{
			delete [] p;
			return -1;
		}

		p = np;
	}
}

//////////////////////////////////////////////////////////////////////////////
char
OW_String::charAt(size_t ndx) const
{
	return (m_buf.getPtr()) ? m_buf->data()[ndx] : (char)0;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::compareTo(const OW_String& arg) const
{
	const char* lhs = "";
	const char* rhs = "";
	if (m_buf.getPtr())
	{
		lhs = m_buf->data();
	}
	if (arg.m_buf.getPtr())
	{
		rhs = arg.m_buf->data();
	}

	return ::strcmp(lhs, rhs);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::compareToIgnoreCase(const OW_String& arg) const
{
	const char* lhs = "";
	const char* rhs = "";
	if (m_buf.getPtr())
	{
		lhs = m_buf->data();
	}
	if (arg.m_buf.getPtr())
	{
		rhs = arg.m_buf->data();
	}

	return strcmpi(lhs, rhs);
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::concat(const OW_String& arg)
{
	if(arg.length() > 0)
	{
		size_t len = length() + arg.length();
		char* bfr = new char[len+1];
		bfr[0] = 0;
		if (m_buf.getPtr())
		{
			::strcpy(bfr, m_buf->data());
		}
		if (arg.m_buf.getPtr())
		{
			::strcat(bfr, arg.m_buf->data());
		}
		m_buf = new ByteBuf(bfr, len);
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_String::endsWith(const OW_String& arg, OW_Bool ignoreCase) const
{
	if(arg.length() == 0)
	{
		return (arg.length() == length());
	}

	int ndx = length();
	ndx -= (int)arg.length();

	if(ndx < 0)
	{
		return false;
	}

	OW_Bool cc;
	
	const char* lhs = "";
	const char* rhs = "";
	if (m_buf.getPtr())
	{
		lhs = m_buf->data()+ndx;
	}
	if (arg.m_buf.getPtr())
	{
		rhs = arg.m_buf->data();
	}

	if(ignoreCase)
	{
		cc = (::strcmpi(lhs, rhs) == 0);
	}
	else
	{
		cc = (::strcmp(lhs, rhs) == 0);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_String::equals(const OW_String& arg) const
{
	return(compareTo(arg) == 0);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_String::equalsIgnoreCase(const OW_String& arg) const
{
	return(compareToIgnoreCase(arg) == 0);
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_String::hashCode() const
{
	OW_UInt32 hash = 0;

	size_t len = length();
	for(size_t i = 0; i < len; i++)
	{
		// Don't need to check if m_buf is null, because if it is, len == 0,
		// and this loop won't be executed.
		const char temp = m_buf->data()[i];
		hash = (hash << 4) + (temp * 13);
		OW_UInt32 g = hash & 0xf0000000;

		if(g)
		{
			hash ^= (g >> 24);
			hash ^= g;
		}
	}
	return hash;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::indexOf(char ch, int fromIndex) const
{
	if(fromIndex < 0)
	{
		fromIndex = 0;
	}

	int cc = -1;
	if((size_t)fromIndex < length())
	{
		// Don't need to check m_buf for NULL, because if length() == 0,
		// this code won't be executed.
		char* p = OW_String::strchr(m_buf->data()+fromIndex, ch);
		if(p != NULL)
		{
			cc = p - m_buf->data();
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::indexOf(const OW_String& arg, int fromIndex) const
{
	if(fromIndex < 0)
	{
		fromIndex = 0;
	}

	int cc = -1;
	if((size_t)fromIndex < length())
	{
		// Don't need to check m_buf for NULL, because if length() == 0,
		// this code won't be executed, but we do need to check arg.m_buf
		char* p;
		if (arg.m_buf.getPtr())
		{
			p = ::strstr(m_buf->data()+fromIndex, arg.m_buf->data());
		}
		else
		{
			p = m_buf->data()+fromIndex;
		}

		if(p != NULL)
		{
			cc = p - m_buf->data();
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::lastIndexOf(char ch, int fromIndex) const
{
	if(fromIndex < 0)
	{
		if((fromIndex = length()-1) < 0)
			return -1;
	}

	int cc = -1;
	if((size_t)fromIndex < length())
	{
		for(int i = (int)fromIndex; i >= 0; i--)
		{
			// Don't need to check m_buf for NULL, because if length() == 0,
			// this code won't be executed.
			if(m_buf->data()[i] == ch)
			{
				cc = i;
				break;
			}
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::lastIndexOf(const OW_String& arg, int fromIndex) const
{
	if(fromIndex < 0 || (size_t)fromIndex >= length())
	{
		if((fromIndex = length()-1) < 0)
			return -1;
	}

	if((fromIndex -= arg.length() - 1) < 0)
	{
		return -1;
	}

	if (!arg.m_buf.getPtr())
	{
		return length() - 1;
	}

	while(fromIndex >= 0)
	{
		// Don't need to check m_buf for NULL, because if length() == 0,
		// this code won't be executed.
		if(::strncmp(m_buf->data()+fromIndex, arg.m_buf->data(),
			arg.length()) == 0)
		{
			break;
		}

		fromIndex--;
	}

	return fromIndex;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_String::startsWith(const OW_String& arg, OW_Bool ignoreCase) const
{
	OW_Bool cc = false;

	if(arg.length() == 0)
	{
		return (arg.length() == length());
	}

	if(arg.length() <= length())
	{
		// Don't need to check m_buf for NULL, because if length() == 0,
		// this code won't be executed.
		if(ignoreCase)
		{
			cc = (::strncmpi(m_buf->data(), arg.m_buf->data(), arg.length()) == 0);
		}
		else
		{
			cc = (::strncmp(m_buf->data(), arg.m_buf->data(), arg.length()) == 0);
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_String::substring(size_t beginIndex, size_t len) const
{
	OW_String nil;
	size_t count = len;

	if(0 == length())
	{
		return nil;
	}

	if(beginIndex >= length())
	{
		return nil;
	}
	else if(0 == len)
	{
		return nil;
	}
	else if(-1 == int(len))
	{
		count = length() - beginIndex;
	}

	// Don't need to check m_buf for NULL, because if length() == 0,
	// this code won't be executed.
	return OW_String(static_cast<const char*>(m_buf->data()+beginIndex), count);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_String::isSpaces() const
{
	if (!m_buf.getPtr())
	{
		return true;
	}

	char* p = m_buf->data();
	while(isspace(*p) && *p != (char)0)
	{
		p++;
	}

	return (*p == (char)0);
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::ltrim()
{
	if (!m_buf.getPtr())
	{
		return *this;
	}

	char* s1 = m_buf->data();
	while(isspace(*s1) && *s1 != (char)0)
	{
		s1++;
	}

	if(s1 == m_buf->data())
		return *this;

	*this = OW_String((const char*)s1);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::rtrim()
{
	if(length() == 0)
		return *this;

	char* s1 = m_buf->data() + (length()-1);
	while(isspace(*s1) && s1 >= m_buf->data())
	{
		s1--;
	}

	if(s1 == (m_buf->data() + (length()-1)))
		return *this;

	if(s1 < m_buf->data())
	{
		*this = OW_String();
		return *this;
	}

	size_t len = (s1 - m_buf->data()) + 1;
	*this = OW_String((const char*)m_buf->data(), len);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::trim()
{
	if(length() == 0)
	{
		return *this;
	}

	char* s1 = m_buf->data();
	while(isspace(*s1) && *s1 != (char)0)
	{
		s1++;
	}

	if(*s1 == (char)0)
	{
		// String is all spaces
		*this = OW_String();
		return *this;
	}

	char* p2 = OW_String::strchr(s1, (char)0);
	char* s2 = p2 - 1;

	while(isspace(*s2))
	{
		s2--;
	}

	if(s1 == m_buf->data() && s2 == p2)
	{
		// String has no leading or trailing spaces
		return *this;
	}

	size_t len = (s2 - s1) + 1;
	*this = OW_String((const char*)s1, len);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::erase()
{
	m_buf = 0;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::erase(size_t idx, size_t len)
{
	if( idx >= length() )
		return *this;

	*this = substring(0, idx) + substring(idx + len);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::toLowerCase()
{
	if (length())
	{
		OW_MutexLock l = m_buf.getWriteLock();
		for(size_t i = 0; i < length(); i++)
		{
			// Don't need to check m_buf for NULL, because if length() == 0,
			// this code won't be executed.
			m_buf->data()[i] = tolower(m_buf->data()[i]);
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::toUpperCase()
{
	if (length())
	{
		OW_MutexLock l = m_buf.getWriteLock();
		for(size_t i = 0; i < length(); i++)
		{
			// Don't need to check m_buf for NULL, because if length() == 0,
			// this code won't be executed.
			m_buf->data()[i] = toupper(m_buf->data()[i]);
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_String::readObject(istream& istrm) /*throw (OW_IOException)*/
{
	int len;
	if(!istrm.read((char*)&len, sizeof(len)))
		OW_THROW(OW_IOException, "OW_String failed to read len");

	len = OW_ntoh32(len);
	char* bfr = new char[len];
	if(!istrm.read(bfr, len))
		OW_THROW(OW_IOException, "OW_String failed to read string");

	m_buf = new ByteBuf(bfr, len-1);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_String::writeObject(ostream& ostrm) const /*throw (OW_IOException)*/
{
	int len = length()+1;
	int nl = OW_hton32(len);
	if(!ostrm.write((const char*)&nl, sizeof(nl)))
		OW_THROW(OW_IOException, "OW_String failed to write len");

	const char* p = "";
	if (m_buf.getPtr())
	{
		p = m_buf->data();
	}
	if(!ostrm.write(p, len))
		OW_THROW(OW_IOException, "OW_String failed to write string");
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::operator= (const OW_String& arg)
{
	if(m_buf.getPtr() != arg.m_buf.getPtr())
	{
		m_buf = arg.m_buf;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////////
const char*
OW_String::c_str() const
{
	if (m_buf.getPtr())
	{
		return m_buf->data();
	}
	else
	{
		return "";
	}
}

//////////////////////////////////////////////////////////////////////////////
char
OW_String::operator[] (size_t ndx) const
{
	// Don't need to check m_buf for NULL, because if length() == 0,
	// m_buf->data() won't be executed.
	return (ndx < length()) ? m_buf->data()[ndx] : 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_String::toString() const
{
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
char
OW_String::toChar() const
{
	char c = 0;
	if(length() > 0)
	{
		c = charAt(0);
	}
	return c;
}

//////////////////////////////////////////////////////////////////////////////
OW_Char16
OW_String::toChar16() const
{
	OW_Char16 c16;

	if(length() > 0)
	{
		c16 = OW_Char16((OW_UInt16) charAt(0));
	}
	return c16;
}

//////////////////////////////////////////////////////////////////////////////
OW_String::operator OW_Char16() const
{
	return toChar16();
}

//////////////////////////////////////////////////////////////////////////////
OW_Real32
OW_String::toReal32() const
{
	if (m_buf.getPtr())
	{
		return (OW_Real32) ::strtod(m_buf->data(), (char**)NULL);
	}
	else
	{
		return 0.0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Real64
OW_String::toReal64() const
{
	if (m_buf.getPtr())
	{
		return (OW_Real64) ::strtod(m_buf->data(), (char**)NULL);
	}
	else
	{
		return 0.0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_String::toBool() const
{
	OW_Bool rv(true);
	if(equalsIgnoreCase("false")
		|| equalsIgnoreCase("0")
		|| equalsIgnoreCase("0.0")
		|| length() == 0)
	{
		rv = false;
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt8
OW_String::toUInt8() const
{
	if (m_buf.getPtr())
	{
		return (OW_UInt8)::strtoul(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Int8
OW_String::toInt8() const
{
	if (m_buf.getPtr())
	{
		return (OW_Int8)::strtol(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt16
OW_String::toUInt16() const
{
	if (m_buf.getPtr())
	{
		return (OW_UInt16)::strtoul(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Int16
OW_String::toInt16() const
{
	if (m_buf.getPtr())
	{
		return (OW_Int16)::strtol(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_String::toUInt32() const
{
	if (m_buf.getPtr())
	{
		return (OW_UInt32)::strtoul(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_String::toInt32() const
{
	if (m_buf.getPtr())
	{
		return (OW_Int32)::strtol(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt64
OW_String::toUInt64() const
{
	if (m_buf.getPtr())
	{
		return (OW_UInt64)OW_String::strtoull(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Int64
OW_String::toInt64() const
{
	if (m_buf.getPtr())
	{
		return (OW_Int64)OW_String::strtoll(m_buf->data(), NULL, 10);
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime
OW_String::toDateTime() const
{
	return OW_CIMDateTime(*this);
}

//////////////////////////////////////////////////////////////////////////////
OW_String::operator OW_CIMDateTime() const
{
	return toDateTime();
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_String::tokenize(const char* delims, OW_Bool returnTokens) const
{
	OW_StringArray ra;

	if(length() == 0 || delims == 0)
	{
		return ra;
	}

	if(delims == NULL)
	{
		ra.append(*this);
		return ra;
	}

	// Don't need to check m_buf for NULL, because if length() == 0,
	// this code won't be executed.
	char* pstr = m_buf->data();
	char* data = new char[m_buf->length()+1];
	data[0] = 0;

	int i = 0;
	while(*pstr)
	{
		if(OW_String::strchr(delims, *pstr))
		{
			if(data[0] != 0)
			{
				ra.append(OW_String(data));
				data[0] = 0;
			}

			if(returnTokens)
			{
				ra.append(OW_String(*pstr));
			}

			i = 0;
		}
		else
		{
			data[i++] = *pstr;
			data[i] = 0;
		}

		pstr++;
	}

	if(data[0] != 0)
	{
		ra.append(OW_String(data));
	}

	delete [] data;
	return ra;
}

#ifdef OW_HAVE_STRTOLL
long long int
OW_String::strtoll(const char* nptr, char** endptr, int base)
{
	return ::strtoll(nptr, endptr, base);
}
#else

#ifndef LLONG_MAX
#if OW_SIZEOF_LONG_LONG_INT == 8
#define LLONG_MAX 9223372036854775807LL
#else
#define LLONG_MAX 2147483647LL
#endif
#define LLONG_MIN (-LLONG_MAX - 1LL)
#endif

long long int
OW_String::strtoll(const char* nptr, char** endptr, int base)
{
	const char *s;
	long long acc, cutoff;
	int c;
	int neg, any, cutlim;

	// Skip white space and pick up leading +/- sign if any.
	// If base is 0, allow 0x for hex and 0 for octal, else
	// assume decimal; if base is already 16, allow 0x.
	s = nptr;
	do
	{
		c = (unsigned char) *s++;
	} while(isspace(c));

	if(c == '-')
	{
		neg = 1;
		c = *s++;
	}
	else
	{
		neg = 0;
		if(c == '+')
		{
			c = *s++;
		}
	}

	if((base == 0 || base == 16)
		&& c == '0'
		&& (*s == 'x' || *s == 'X'))
	{
		c = s[1];
		s += 2;
		base = 16;
	}
	if(base == 0)
	{
		base = c == '0' ? 8 : 10;
	}

	// Compute the cutoff value between legal numbers and illegal
	// numbers.  That is the largest legal value, divided by the
	// base.  An input number that is greater than this value, if
	// followed by a legal input character, is too big.  One that
	// is equal to this value may be valid or not; the limit
	// between valid and invalid numbers is then based on the last
	// digit.  For instance, if the range for longs is
	// [-2147483648..2147483647] and the input base is 10,
	// cutoff will be set to 214748364 and cutlim to either
	// 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	// a value > 214748364, or equal but the next digit is > 7 (or 8),
	// the number is too big, and we will return a range error.
	//
	// Set any if any `digits' consumed; make it negative to indicate
	// overflow.
	cutoff = neg ? LLONG_MIN : LLONG_MAX;
	cutlim = cutoff % base;
	cutoff /= base;
	if(neg)
	{
		if(cutlim > 0)
		{
			cutlim -= base;
			cutoff += 1;
		}
		cutlim = -cutlim;
	}
	for(acc = 0, any = 0;; c = (unsigned char) *s++)
	{
		if(isdigit(c))
			c -= '0';
		else if(isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;

		if(c >= base)
			break;
		if(any < 0)
			continue;
		if(neg)
		{
			if(acc < cutoff || acc == cutoff && c > cutlim)
			{
				any = -1;
				acc = LLONG_MIN;
				errno = ERANGE;
			}
			else
			{
				any = 1;
				acc *= base;
				acc -= c;
			}
		}
		else
		{
			if(acc > cutoff || acc == cutoff && c > cutlim)
			{
				any = -1;
				acc = LLONG_MAX;
				errno = ERANGE;
			}
			else
			{
				any = 1;
				acc *= base;
				acc += c;
			}
		}
	}
	if(endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return(acc);
}

#endif	// #ifdef OW_HAVE_STRTOLL

#ifdef OW_HAVE_STRTOULL
unsigned long long int
OW_String::strtoull(const char* nptr, char** endptr, int base)
{
	return ::strtoull(nptr, endptr, base);
}

#else

#ifndef ULLONG_MAX
#if OW_SIZEOF_LONG_LONG_INT == 8
#define ULLONG_MAX 18446744073709551615ULL
#else
#define ULLONG_MAX 4294967295ULL
#endif
#endif

unsigned long long int
OW_String::strtoull(const char* nptr, char** endptr, int base)
{
	const char *s;
	unsigned long long acc, cutoff, cutlim;
	unsigned int c;
	int neg, any;

	s = nptr;
	do
	{
		c = (unsigned char) *s++;
	} while(isspace(c));

	if(c == '-')
	{
		neg = 1;
		c = *s++;
	}
	else
	{
		neg = 0;
		if(c == '+')
			c = *s++;
	}

	if((base == 0 || base == 16)
		&& c == '0'
		&& (*s == 'x' || *s == 'X'))
	{
		c = s[1];
		s += 2;
		base = 16;
	}
	if(base == 0)
		base = c == '0' ? 8 : 10;

	cutoff = ULLONG_MAX / (unsigned long long)base;
	cutlim = ULLONG_MAX % (unsigned long long)base;
	for(acc = 0, any = 0;; c = (unsigned char) *s++)
	{
		if(isdigit(c))
			c -= '0';
		else if(isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;

		if(c >= (unsigned int)base)
			break;
		if(any < 0)
			continue;
		if(acc > cutoff || acc == cutoff && c > cutlim)
		{
			any = -1;
			acc = ULLONG_MAX;
			errno = ERANGE;
		}
		else
		{
			any = 1;
			acc *= (unsigned long)base;
			acc += c;
		}
	}

	if(neg && any > 0)
		acc = -acc;

	if(endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return(acc);
}

#endif	// #ifdef OW_HAVE_STRTOULL

//////////////////////////////////////////////////////////////////////////////
OW_String
operator + (const OW_String& s1, const OW_String& s2)
{
	OW_String rstr(s1);
	rstr += s2;
	return rstr;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
operator + (const char* p, const OW_String& s)
{
	OW_String rstr(p);
	rstr += s;
	return rstr;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
operator + (const OW_String& s, const char* p)
{
	OW_String rstr(s);
	rstr += p;
	return rstr;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
operator + (char c, const OW_String& s)
{
	OW_String rstr(c);
	rstr += s;
	return rstr;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
operator + (const OW_String& s, char c)
{
	OW_String rstr(s);
	rstr += OW_String(c);
	return rstr;
}

//////////////////////////////////////////////////////////////////////////////
ostream&
operator<< (ostream& ostr, const OW_String& arg)
{
	ostr << arg.c_str();
	return ostr;
}

//////////////////////////////////////////////////////////////////////////////
// static
OW_String
OW_String::getLine(istream& is)
{
	OW_StringBuffer rv(80);

	if(is)
	{
		size_t count = 0;
        std::streambuf *sb = is.rdbuf();
		
		while(1)
		{
			int ch = sb->sbumpc();
			if(ch == EOF)
			{
				is.setstate(count == 0
					? (std::ios::failbit | std::ios::eofbit) : std::ios::eofbit);
				break;
			}
			
			++count;
			
			if(ch == '\n')
				break;
			
			rv += (char) ch;
		}
	}

	OW_String rstr = rv.toString();
	int ndx = rstr.indexOf('\r');
	if(ndx != -1)
	{
		rstr = rstr.substring(0, ndx);
	}

	return rstr;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
char*
OW_String::strchr(const char* theStr, int c)
{
	const char* tmpChar = theStr;
	for (; *tmpChar && *tmpChar != c; tmpChar++)
	{
		// empty
	}
	return ((*tmpChar) == c ? (char*) tmpChar : (char*)0);
}



