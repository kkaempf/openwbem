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
#include "OW_String.hpp"
#include "OW_Char16.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_Array.hpp"
#include "OW_StringStream.hpp"
#include "OW_Format.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_Assertion.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Bool.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <cerrno>

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

#include <cmath> // for HUGE_VAL

using std::istream;
using std::ostream;

DEFINE_EXCEPTION(StringConversion);

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


// class invariant: m_buf points to a null-terminated sequence of characters. m_buf is m_len+1 bytes long.
class OW_String::ByteBuf
{
public:

	ByteBuf(const char* s) :
		m_len(::strlen(s)), m_buf(new char[m_len+1])
	{
		strcpy(m_buf, s);
	}

	ByteBuf(const ByteBuf& arg) :
		m_len(arg.m_len), m_buf(new char[m_len+1])
	{
		strcpy(m_buf, arg.m_buf);
	}

	ByteBuf(char* bfr, size_t len)
		: m_len(len), m_buf(bfr)
	{
	}

	~ByteBuf() { delete [] m_buf; }
	ByteBuf& operator= (const ByteBuf& arg)
	{
		char* buf = new char[arg.m_len+1];
		strcpy(buf, arg.m_buf);
		delete [] m_buf;
		m_buf = buf;
		m_len = arg.m_len;
		return *this;
	}

	size_t length() const { return m_len; }
	char* data() const { return m_buf; }

	ByteBuf* clone() const { return new ByteBuf(*this); }

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
OW_String::OW_String(const OW_Char16& parm) :
	m_buf(NULL)
{
	this->m_buf = parm.toUTF8().m_buf;
}

#if defined(OW_WIN32)
#define snprintf _snprintf // stupid windoze...
#endif
//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_Int32 val) :
	m_buf(NULL)
{
	char tmpbuf[32];
	int len = snprintf(tmpbuf, sizeof(tmpbuf), "%d", val);
	OW_AutoPtrVec<char> bfr(new char[len+1]);
	::snprintf(bfr.get(), len+1, "%d", val);
	m_buf = new ByteBuf(bfr.release(), len);
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(OW_UInt32 val) :
	m_buf(NULL)
{
	char tmpbuf[32];
	int len = ::snprintf(tmpbuf, sizeof(tmpbuf), "%u", val);
	OW_AutoPtrVec<char> bfr(new char[len+1]);
	::snprintf(bfr.get(), len+1, "%u", val);
	m_buf = new ByteBuf(bfr.release(), len);
}
#if defined(OW_WIN32)
#undef snprintf
#endif

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
OW_String::OW_String(ETakeOwnershipFlag, char* allocatedMemory, size_t len) :
	m_buf(NULL)
{
	OW_ASSERT(allocatedMemory != 0);
	m_buf = new ByteBuf(allocatedMemory, len);
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
		OW_AutoPtrVec<char> bfr(new char[len+1]);
		::memcpy(bfr.get(), str, len);
		bfr[len] = '\0';
		m_buf = new ByteBuf(bfr.release(), len);
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
OW_String::OW_String(const OW_Char16Array& ra) :
	m_buf(NULL)
{
	size_t sz = ra.size();
	if(sz > 0)
	{
		OW_StringBuffer buf(sz * 2);
		for(size_t i = 0; i < sz; i++)
		{
			buf += ra[i].toUTF8();
		}
		m_buf = buf.releaseString().m_buf;
	}
	else
	{
		m_buf = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_String::OW_String(char c) :
	m_buf(NULL)
{
	if(c != '\0')
	{
		char bfr[2];
		bfr[0] = c;
		bfr[1] = '\0';
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
OW_String::allocateCString() const
{
	size_t len = length() + 1;
	char* str = static_cast<char*>(malloc(len));
	::strcpy(str, c_str());
	return str;
}

//////////////////////////////////////////////////////////////////////////////
size_t
OW_String::length() const
{
	if (m_buf)
	{
		return m_buf->length();
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
size_t
OW_String::UTF8Length() const
{
// ATTN: UTF8 ?
// TODO: FIXME this is broken wrt i18n
	return length();
}

//////////////////////////////////////////////////////////////////////////////

#ifdef OW_WIN32
#define vsnprintf _vsnprintf // stupid windoze
#endif

int
OW_String::format(const char* fmt, ...)
{
	int n, size = 64;
	OW_AutoPtrVec<char> p(new char[size]);
	
	va_list ap;
	
	// Try to print in the allocated space
	while(true)
	{
		va_start(ap, fmt);
		n = vsnprintf(p.get(), size, fmt, ap);
		va_end(ap);                // If that worked, return the string.

		if(n > -1 && n < size)
		{
			m_buf = new ByteBuf(p.release(), n);
			return static_cast<int>(length());
		}

		if (n > -1)    // glibc 2.1
			size = n+1; // precisely what is needed
		else           // glibc 2.0
			size *= 2;  // twice the old size

		p = new char[size];
	}
}
#ifdef OW_WIN32
#undef vsnprintf // stupid windoze
#endif

//////////////////////////////////////////////////////////////////////////////
char
OW_String::charAt(size_t ndx) const
{
	return (m_buf) ? m_buf->data()[ndx] : '\0';
}

//////////////////////////////////////////////////////////////////////////////
int
OW_String::compareTo(const OW_String& arg) const
{
	const char* lhs = "";
	const char* rhs = "";
	if (m_buf)
	{
		lhs = m_buf->data();
	}
	if (arg.m_buf)
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
	if (m_buf)
	{
		lhs = m_buf->data();
	}
	if (arg.m_buf)
	{
		rhs = arg.m_buf->data();
	}

	return strcmpi(lhs, rhs);
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::concat(const OW_String& arg)
{
	if(!arg.empty())
	{
		size_t len = length() + arg.length();
		OW_AutoPtrVec<char> bfr(new char[len+1]);
		bfr[0] = 0;
		if (m_buf)
		{
			::strcpy(bfr.get(), m_buf->data());
		}
		if (arg.m_buf)
		{
			::strcat(bfr.get(), arg.m_buf->data());
		}
		m_buf = new ByteBuf(bfr.release(), len);
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::concat(char arg)
{
    return concat(OW_String(arg));
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_String::endsWith(const OW_String& arg, EIgnoreCaseFlag ignoreCase) const
{
	if(arg.empty())
	{
		return (arg.length() == length());
	}

	int ndx = static_cast<int>(length() - arg.length());

	if(ndx < 0)
	{
		return false;
	}

	bool cc;
	
	const char* lhs = "";
	const char* rhs = "";
	if (m_buf)
	{
		lhs = m_buf->data()+ndx;
	}
	if (arg.m_buf)
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
bool
OW_String::equals(const OW_String& arg) const
{
	return(compareTo(arg) == 0);
}

//////////////////////////////////////////////////////////////////////////////
bool
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
size_t
OW_String::indexOf(char ch, size_t fromIndex) const
{
	//if(fromIndex < 0)
	//{
	//	fromIndex = 0;
	//}

	size_t cc = npos;
	if(fromIndex < length())
	{
		// Don't need to check m_buf for NULL, because if length() == 0,
		// this code won't be executed.
		const char* p = OW_String::strchr(m_buf->data()+fromIndex, ch);
		if(p)
		{
			cc = p - m_buf->data();
		}
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
size_t
OW_String::indexOf(const OW_String& arg, size_t fromIndex) const
{
	//if(fromIndex < 0)
	//{
	//	fromIndex = 0;
	//}

	int cc = npos;
	if(fromIndex < length())
	{
		// Don't need to check m_buf for NULL, because if length() == 0,
		// this code won't be executed, but we do need to check arg.m_buf
		char* p;
		if (arg.m_buf)
		{
			p = ::strstr(m_buf->data()+fromIndex, arg.m_buf->data());
		}
		else
		{
			p = m_buf->data()+fromIndex;
		}

		if(p != NULL)
		{
			cc = static_cast<int>(p - m_buf->data());
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
size_t
OW_String::lastIndexOf(char ch, size_t fromIndex) const
{
	if(fromIndex == npos)
	{
		if((fromIndex = length()-1) == npos)
			return npos;
	}

	size_t cc = npos;
	if(fromIndex < length())
	{
		for(size_t i = fromIndex; i != npos; i--)
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
size_t
OW_String::lastIndexOf(const OW_String& arg, size_t fromIndex) const
{
	if(fromIndex == npos || fromIndex >= length())
	{
		if(static_cast<int>(fromIndex = length()-1) < 0)
			return npos;
	}

	if(static_cast<int>(fromIndex -= arg.length() - 1) < 0)
	{
		return npos;
	}

	if (!arg.m_buf)
	{
		return length() - 1;
	}

	while(fromIndex != npos)
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
bool
OW_String::startsWith(const OW_String& arg, EIgnoreCaseFlag ignoreCase) const
{
	bool cc = false;

	if(arg.empty())
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
	size_t l = length();

	if(0 == l)
	{
		return nil;
	}

	if(beginIndex >= l)
	{
		return nil;
	}
	else if(0 == len)
	{
		return nil;
	}
	else if(len == npos)
	{
		count = l - beginIndex;
	}
	if (count + beginIndex > l)
	{
		count = l - beginIndex;
	}

	// Don't need to check m_buf for NULL, because if length() == 0,
	// this code won't be executed.
	return OW_String(static_cast<const char*>(m_buf->data()+beginIndex), count);
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_String::isSpaces() const
{
	if (!m_buf)
	{
		return true;
	}

	char* p = m_buf->data();
	while(isspace(*p) && *p != '\0')
	{
		p++;
	}

	return (*p == '\0');
}

//////////////////////////////////////////////////////////////////////////////
OW_String&
OW_String::ltrim()
{
	if (!m_buf)
	{
		return *this;
	}

	char* s1 = m_buf->data();
	while(isspace(*s1) && *s1 != '\0')
	{
		s1++;
	}

	if(s1 == m_buf->data())
		return *this;

	*this = OW_String(s1);
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
	*this = OW_String(m_buf->data(), len);
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
	while(isspace(*s1) && *s1 != '\0')
	{
		s1++;
	}

	if(*s1 == '\0')
	{
		// String is all spaces
		*this = OW_String();
		return *this;
	}

	const char* p2 = OW_String::strchr(s1, '\0');
	const char* s2 = p2 - 1;

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
	*this = OW_String(s1, len);
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
// ATTN: UTF8 ?
// TODO: FIXME this is broken wrt i18n
OW_String&
OW_String::toLowerCase()
{
	if (length())
	{
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
// ATTN: UTF8 ?
// TODO: FIXME this is broken wrt i18n
OW_String&
OW_String::toUpperCase()
{
	if (length())
	{
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
	OW_UInt32 len;
	OW_BinarySerialization::readLen(istrm, len);

	OW_AutoPtrVec<char> bfr(new char[len+1]);
	OW_BinarySerialization::read(istrm, bfr.get(), len);
	bfr[len] = '\0';

	m_buf = new ByteBuf(bfr.release(), len);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_String::writeObject(ostream& ostrm) const /*throw (OW_IOException)*/
{
	OW_UInt32 len = static_cast<OW_UInt32>(length());
	OW_BinarySerialization::writeLen(ostrm, len);

	if (len)
	{
        OW_BinarySerialization::write(ostrm, m_buf->data(), len);
	}
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
	if (m_buf)
	{
		return m_buf->data();
	}
	else
	{
		return "";
	}
}

//////////////////////////////////////////////////////////////////////////////
static const char cnullChar = '\0';
const char&
OW_String::operator[] (size_t ndx) const
{
#ifdef OW_DEBUG
    OW_ASSERT(ndx <= length());
#endif
	// Don't need to check m_buf for NULL, because if length() == 0,
	// m_buf->data() won't be executed.
	//return (ndx <= length()) ? *(m_buf->data() + ndx) : cnullChar;
	if (ndx <= length()) 
        return *(m_buf->data() + ndx);
    else
        return cnullChar;
}

//////////////////////////////////////////////////////////////////////////////
static char nullChar = '\0';
char&
OW_String::operator[] (size_t ndx)
{
#ifdef OW_DEBUG
    OW_ASSERT(ndx <= length());
#endif
	// Don't need to check m_buf for NULL, because if length() == 0,
	// m_buf->data() won't be executed.
	return (ndx <= length()) ? m_buf->data()[ndx] : nullChar;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_String::toString() const
{
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
static inline void
throwStringConversion(const OW_COWReference<OW_String::ByteBuf>& m_buf, const char* type)
{
	OW_THROW(OW_StringConversionException, format("Unable to convert \"%1\" into %2", m_buf->data(), type).c_str());
}

//////////////////////////////////////////////////////////////////////////////
static inline void
throwStringConversion(const char* str, const char* type)
{
	OW_THROW(OW_StringConversionException, format("Unable to convert \"%1\" into %2", str, type).c_str());
}

//////////////////////////////////////////////////////////////////////////////
OW_Char16
OW_String::toChar16() const
{
	if(UTF8Length() != 1)
	{
		throwStringConversion(c_str(), "OW_Char16");
	}
	return OW_Char16(*this);
}

template <typename T>
static inline
T convertToRealType(const OW_COWReference<OW_String::ByteBuf>& m_buf, const char* type)
{
	if (m_buf)
	{
		char* endptr;
		double v = ::strtod(m_buf->data(), &endptr);
		T rv = static_cast<T>(v);
		if (*endptr != '\0' || errno == ERANGE || rv == HUGE_VAL || rv == -HUGE_VAL)
		{
			throwStringConversion(m_buf, type);
		}
		return rv;
	}
	else
	{
		throwStringConversion("", type);
	}
	return T(); // to make compiler happy
}



//////////////////////////////////////////////////////////////////////////////
OW_Real32
OW_String::toReal32() const
{
	return convertToRealType<OW_Real32>(m_buf, "OW_Real32");
}

//////////////////////////////////////////////////////////////////////////////
OW_Real64
OW_String::toReal64() const
{
	return convertToRealType<OW_Real64>(m_buf, "OW_Real64");
}

//////////////////////////////////////////////////////////////////////////////
bool
OW_String::toBool() const
{
	if (equalsIgnoreCase("true"))
	{
		return true;
	}
	else if (equalsIgnoreCase("false"))
	{
		return false;
	}
	else
	{
		throwStringConversion(c_str(), "bool");
	}
	return false; // to make compiler happy
}

template <typename T, typename FP, typename FPRT>
static inline
T doConvertToIntType(const OW_COWReference<OW_String::ByteBuf>& m_buf, const char* type, FP fp, int base)
{
	if (m_buf)
	{
		char* endptr;
		FPRT v = fp(m_buf->data(), &endptr, base);
		T rv = static_cast<T>(v);
		if (*endptr != '\0' || errno == ERANGE || FPRT(rv) != v)
		{
			throwStringConversion(m_buf, type);
		}
		return rv;
	}
	else
	{
		throwStringConversion("", type);
	}
	return T(); // to make compiler happy
}

typedef unsigned long int (*strtoulfp_t)(const char *, char **,int);
typedef long int (*strtolfp_t)(const char *, char **,int);
typedef unsigned long long int (*strtoullfp_t)(const char *, char **,int);
typedef long long int (*strtollfp_t)(const char *, char **,int);


template <typename T>
static inline
T convertToUIntType(const OW_COWReference<OW_String::ByteBuf>& m_buf, const char* msg, int base)
{
	return doConvertToIntType<T, strtoulfp_t, unsigned long int>(m_buf, msg, &strtoul, base);
}

template <typename T>
static inline
T convertToIntType(const OW_COWReference<OW_String::ByteBuf>& m_buf, const char* msg, int base)
{
	return doConvertToIntType<T, strtolfp_t, long int>(m_buf, msg, &strtol, base);
}

template <typename T>
static inline
T convertToUInt64Type(const OW_COWReference<OW_String::ByteBuf>& m_buf, const char* msg, int base)
{
	return doConvertToIntType<T, strtoullfp_t, unsigned long long int>(m_buf, msg, &OW_String::strtoull, base);
}

template <typename T>
static inline
T convertToInt64Type(const OW_COWReference<OW_String::ByteBuf>& m_buf, const char* msg, int base)
{
	return doConvertToIntType<T, strtollfp_t, long long int>(m_buf, msg, &OW_String::strtoll, base);
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt8
OW_String::toUInt8(int base) const
{
	return convertToUIntType<OW_UInt8>(m_buf, "OW_UInt8", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_Int8
OW_String::toInt8(int base) const
{
	return convertToIntType<OW_Int8>(m_buf, "OW_Int8", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt16
OW_String::toUInt16(int base) const
{
	return convertToUIntType<OW_UInt16>(m_buf, "OW_UInt16", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_Int16
OW_String::toInt16(int base) const
{
	return convertToIntType<OW_Int16>(m_buf, "OW_Int16", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt32
OW_String::toUInt32(int base) const
{
	return convertToUIntType<OW_UInt32>(m_buf, "OW_UInt32", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_String::toInt32(int base) const
{
	return convertToIntType<OW_Int32>(m_buf, "OW_Int32", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_UInt64
OW_String::toUInt64(int base) const
{
	return convertToUInt64Type<OW_UInt64>(m_buf, "OW_UInt64", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_Int64
OW_String::toInt64(int base) const
{
	return convertToInt64Type<OW_Int64>(m_buf, "OW_Int64", base);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMDateTime
OW_String::toDateTime() const
{
	return OW_CIMDateTime(*this);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_String::tokenize(const char* delims, EReturnTokensFlag returnTokens) const
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
	OW_AutoPtrVec<char> data(new char[m_buf->length()+1]);
	data[0] = 0;

	int i = 0;
	while(*pstr)
	{
		if(OW_String::strchr(delims, *pstr))
		{
			if(data[0] != 0)
			{
				ra.append(OW_String(data.get()));
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
		ra.append(OW_String(data.get()));
	}

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
	cutlim = static_cast<int>(cutoff % base);
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
			
			rv += static_cast<char>(ch);
		}
	}

	OW_String rstr = rv.releaseString();
	size_t ndx = rstr.indexOf('\r');
	if(ndx != npos)
	{
		rstr = rstr.substring(0, ndx);
	}

	return rstr;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
const char*
OW_String::strchr(const char* theStr, int c)
{
	const char* tmpChar = theStr;
	for (; *tmpChar && *tmpChar != c; tmpChar++)
	{
		// empty
	}
	return ((*tmpChar) == c ? tmpChar : 0);
}

