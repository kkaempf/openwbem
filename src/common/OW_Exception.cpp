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
#include "OW_Exception.hpp"
#include "OW_StackTrace.hpp"

#include <cstring>
#include <cstdlib>
#include <iostream>

OW_Mutex OW_Exception::m_mutex;

//////////////////////////////////////////////////////////////////////////////					
static void freeBuf(char** ptr)
{
	if(*ptr)
	{
		delete [] *ptr;
		*ptr = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////					
static char* dupString(const char* str)
{
	char* rv = new char[strlen(str)+1];
	strcpy(rv, str);
	return rv;
}

//////////////////////////////////////////////////////////////////////////////					
OW_Exception::OW_Exception()
	: std::exception()
	, m_file(0)
	, m_line(0)
	, m_msg(0)
	, m_stackTrace(OW_StackTrace::getStackTrace())
{
	m_mutex.acquire();
}

//////////////////////////////////////////////////////////////////////////////					
OW_Exception::OW_Exception(const char* file, int line, const char* msg)
	: std::exception()
	, m_file(0)
	, m_line(line)
	, m_msg(0)
	, m_stackTrace(OW_StackTrace::getStackTrace())
{
	m_mutex.acquire();
	if(file)
	{
		m_file = dupString(file);
	}

	if(msg)
	{
		m_msg = dupString(msg);
	}
}

//////////////////////////////////////////////////////////////////////////////					
OW_Exception::OW_Exception(const char* msg)
	: std::exception()
	, m_file(0)
	, m_line(0)
	, m_msg(0)
	, m_stackTrace(OW_StackTrace::getStackTrace())
{
	m_mutex.acquire();
	if(msg)
	{
		m_msg = dupString(msg);
	}
}

//////////////////////////////////////////////////////////////////////////////					
OW_Exception::OW_Exception( const OW_Exception& e )
	: std::exception(e)
	, m_file(0)
	, m_line(e.m_line)
	, m_msg(0)
	, m_stackTrace(0)
{
	m_mutex.acquire();
	if(e.m_file)
	{
		m_file = dupString(e.m_file);
	}

	if(e.m_msg)
	{
		m_msg = dupString(e.m_msg);
	}

	if(e.m_stackTrace)
	{
		m_stackTrace = new OW_StackTrace(*e.m_stackTrace);
	}
}

//////////////////////////////////////////////////////////////////////////////					
OW_Exception::~OW_Exception()
{
	freeBuf(&m_file);
	freeBuf(&m_msg);
	delete m_stackTrace;
	m_mutex.release();
}

//////////////////////////////////////////////////////////////////////////////					
OW_Exception&
OW_Exception::operator=( const OW_Exception& rhs )
{
	if (this != &rhs)
	{
		freeBuf(&m_file);
		freeBuf(&m_msg);
		delete m_stackTrace;

		if(rhs.m_file)
		{
			m_file = new char[strlen(rhs.m_file)+1];
			strcpy(m_file, rhs.m_file);
		}

		if(rhs.m_msg)
		{
			m_msg = new char[strlen(rhs.m_msg)+1];
			strcpy(m_msg, rhs.m_msg);
		}

		if(rhs.m_stackTrace)
		{
			m_stackTrace = new OW_StackTrace(*rhs.m_stackTrace);
		}
	}

	return *this;
}
		
//////////////////////////////////////////////////////////////////////////////					
const char*
OW_Exception::getMessage() const
{
	return (m_msg != NULL) ? m_msg : "";
}

//////////////////////////////////////////////////////////////////////////////					
const char*
OW_Exception::getFile() const
{
	return (m_file != NULL) ? m_file : "";
}

//////////////////////////////////////////////////////////////////////////////					
const char*
OW_Exception::getStackTrace() const
{
	return (m_stackTrace != NULL) ? m_stackTrace->c_str() : "";
}

//////////////////////////////////////////////////////////////////////////////					
std::ostream&
operator<<(std::ostream& os, const OW_Exception& e)
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

	if(*e.getStackTrace() == '\0')
	{
		os << e.getStackTrace();
	}

	return os;
}

//////////////////////////////////////////////////////////////////////////////					
const char*
OW_Exception::what() const throw()
{
	return getMessage();
}

