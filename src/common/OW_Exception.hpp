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

#ifndef OW_EXCEPTION_HPP_
#define OW_EXCEPTION_HPP_

#include "OW_config.h"
#include "OW_Mutex.hpp"
#include <iosfwd>
#include <exception>


class OW_Exception : public std::exception
{
public:
	OW_Exception();
	OW_Exception(const char* file, int line, const char* msg);
	OW_Exception(const char* msg);
	OW_Exception(const OW_Exception& e);
	OW_Exception& operator= (const OW_Exception& rhs);
	virtual ~OW_Exception() throw();
	virtual const char* type() const {  return "OW_Exception"; }
	const char* getMessage() const;
	const char* getFile() const;
	int getLine() const {  return m_line; }

	virtual const char* what() const throw();

protected:
	char* m_file;
	int m_line;
	char* m_msg;

private:
	static OW_Mutex m_mutex;
};

std::ostream& operator<< (std::ostream& os, const OW_Exception& e);

#define OW_THROW(exType, msg) throw exType(__FILE__, __LINE__, msg)
#define OW_THROWL(exType, line, msg) throw exType(__FILE__, line, msg)

#define DEFINE_EXCEPTION(NAME) \
class OW_##NAME##Exception : public OW_Exception \
{ \
public: \
	OW_##NAME##Exception() : OW_Exception() {} \
	OW_##NAME##Exception(const char* file, int line, const char* msg) \
		: OW_Exception(file, line, msg) {} \
	OW_##NAME##Exception(const char* msg) : OW_Exception(msg) {} \
	virtual ~OW_##NAME##Exception() throw() { } \
  \
	virtual const char* type() const { return "OW_" #NAME "Exception"; } \
};

#endif	// __OW_EXCEPTION_HPP__


