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
#include "OW_Exception.hpp"
#include "OW_StackTrace.hpp"
#include "OW_Mutex.hpp"
#include <cstring>
#include <cstdlib>
#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif
#include <algorithm> // for std::swap

namespace OpenWBEM
{

Mutex* Exception::m_mutex = new Mutex();
//////////////////////////////////////////////////////////////////////////////					
static void freeBuf(char** ptr)
{
	delete [] *ptr;
	*ptr = NULL;
}
//////////////////////////////////////////////////////////////////////////////					
static char* dupString(const char* str)
{
	if (!str)
	{
		return 0;
	}
	char* rv = new char[strlen(str)+1];
	strcpy(rv, str);
	return rv;
}
//////////////////////////////////////////////////////////////////////////////					
Exception::Exception()
	: std::exception()
	, m_file(0)
	, m_line(0)
	, m_msg(0)
{
#ifdef OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS
	StackTrace::getStackTrace();
#endif
	m_mutex->acquire();
}
//////////////////////////////////////////////////////////////////////////////					
Exception::Exception(const char* file, int line, const char* msg)
	: std::exception()
	, m_file(0)
	, m_line(line)
	, m_msg(0)
{
#ifdef OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS
	StackTrace::getStackTrace();
#endif
	m_mutex->acquire();
	m_file = dupString(file);
	m_msg = dupString(msg);
}
//////////////////////////////////////////////////////////////////////////////					
Exception::Exception(const char* msg)
	: std::exception()
	, m_file(0)
	, m_line(0)
	, m_msg(0)
{
#ifdef OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS
	StackTrace::getStackTrace();
#endif
	m_mutex->acquire();
	m_msg = dupString(msg);
}
//////////////////////////////////////////////////////////////////////////////					
Exception::Exception( const Exception& e )
	: std::exception(e)
	, m_file(0)
	, m_line(e.m_line)
	, m_msg(0)
{
	m_mutex->acquire();
	m_file = dupString(e.m_file);
	m_msg = dupString(e.m_msg);
}
//////////////////////////////////////////////////////////////////////////////					
Exception::~Exception() throw()
{
	try
	{
		freeBuf(&m_file);
		freeBuf(&m_msg);
		m_mutex->release();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////					
Exception&
Exception::operator=(Exception rhs)
{
	rhs.swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////					
void
Exception::swap(Exception& rhs)
{
	std::swap(m_file, rhs.m_file);
	std::swap(m_line, rhs.m_line);
	std::swap(m_msg, rhs.m_msg);
}
		
//////////////////////////////////////////////////////////////////////////////					
const char*
Exception::getMessage() const
{
	return (m_msg != NULL) ? m_msg : "";
}
//////////////////////////////////////////////////////////////////////////////					
const char*
Exception::getFile() const
{
	return (m_file != NULL) ? m_file : "";
}
//////////////////////////////////////////////////////////////////////////////					
std::ostream&
operator<<(std::ostream& os, const Exception& e)
{
	if(*e.getFile() == '\0')
	{
		os << "[no file]: ";
	}
	else
	{
		os << e.getFile() << ": ";
	}
	if(e.getLine() == 0)
	{
		os << "[no line] ";
	}
	else
	{
		os << e.getLine() << ' ';
	}
	os << e.type() << ": ";
	if(*e.getMessage() == '\0')
	{
		os << "[no message]";
	}
	else
	{
		os << e.getMessage();
	}
	return os;
}
//////////////////////////////////////////////////////////////////////////////					
const char*
Exception::what() const throw()
{
	return getMessage();
}

} // end namespace OpenWBEM

