/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_StringStream.hpp"

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
StringStreamBuf::StringStreamBuf(size_t size)
	: BaseStreamBuffer(size, "out"), m_buf(size) 
{
}
///////////////////////////////////////////////////////////////////////////////
StringStreamBuf::~StringStreamBuf() 
{
}
///////////////////////////////////////////////////////////////////////////////
String StringStreamBuf::toString() const 
{ 
	return m_buf.toString(); 
}
///////////////////////////////////////////////////////////////////////////////
size_t StringStreamBuf::length() const 
{ 
	return m_buf.length(); 
}
///////////////////////////////////////////////////////////////////////////////
const char* StringStreamBuf::c_str() const 
{ 
	return m_buf.c_str(); 
}
///////////////////////////////////////////////////////////////////////////////
void StringStreamBuf::reset() 
{ 
	m_buf.reset(); 
}
///////////////////////////////////////////////////////////////////////////////
int StringStreamBuf::buffer_to_device(const char *c, int n)
{
	m_buf.append(c, n);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
StringStreamBase::StringStreamBase(size_t sz) 
	: m_buf(sz) 
{
}
///////////////////////////////////////////////////////////////////////////////
StringStream::StringStream(size_t size)
	: StringStreamBase(size), std::ostream(&m_buf)
{
}
///////////////////////////////////////////////////////////////////////////////
StringStream::~StringStream()
{
}
///////////////////////////////////////////////////////////////////////////////
String StringStream::toString() const 
{ 
	m_buf.sync(); 
	return m_buf.toString(); 
}
///////////////////////////////////////////////////////////////////////////////
size_t StringStream::length() const 
{ 
	m_buf.sync(); 
	return m_buf.length(); 
}
///////////////////////////////////////////////////////////////////////////////
const char* StringStream::c_str() const 
{ 
	m_buf.sync(); 
	return m_buf.c_str(); 
}
///////////////////////////////////////////////////////////////////////////////
void StringStream::reset() 
{ 
	m_buf.reset(); 
}

} // end namespace OpenWBEM

