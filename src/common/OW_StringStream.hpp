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
#ifndef OW_STRINGSTREAM_HPP_INCLUDE_GUARD_
#define OW_STRINGSTREAM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_StringBuffer.hpp"
#include "OW_BaseStreamBuffer.hpp"
#if defined(OW_HAVE_OSTREAM)
#include <ostream>
#else
#include <iostream>
#endif
#ifdef OW_HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

namespace OpenWBEM
{

class StringStreamBuf : public BaseStreamBuffer
{
public:
	StringStreamBuf(size_t size);
	virtual ~StringStreamBuf();
	String toString() const;
	size_t length() const;
	const char* c_str() const;
	void reset();
protected:
	virtual int buffer_to_device(const char *c, int n);
private:
	StringBuffer m_buf;
	friend class StringStream;
};
///////////////////////////////////////////////////////////////////////////////
class StringStreamBase
{
public:
	StringStreamBase(size_t sz);
	mutable StringStreamBuf m_buf;
};
///////////////////////////////////////////////////////////////////////////////
class StringStream : private StringStreamBase, public std::ostream
{
public:
	StringStream(size_t size = 256);
	~StringStream();
	String toString() const;
	size_t length() const;
	const char* c_str() const;
	void reset();
private:
	// not implemented
	StringStream(const StringStream&);
	StringStream& operator=(const StringStream&);
};

} // end namespace OpenWBEM

typedef OpenWBEM::StringStreamBuf OW_StringStreamBuf;
typedef OpenWBEM::StringStream OW_StringStream;

#endif
