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
#ifndef OW_EXCEPTION_HPP_INCLUDE_GUARD_
#define OW_EXCEPTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include <iosfwd>
#include <exception>

namespace OpenWBEM
{

class Mutex;
class Exception : public std::exception
{
public:
	Exception();
	Exception(const char* file, int line, const char* msg);
	explicit Exception(const char* msg);
	Exception(const Exception& e);
	Exception& operator= (Exception rhs);
	void swap(Exception& x);
	virtual ~Exception() throw();
	virtual const char* type() const {  return "Exception"; }
	virtual const char* getMessage() const;
	const char* getFile() const;
	int getLine() const {  return m_line; }
	virtual const char* what() const throw();
protected:
	char* m_file;
	int m_line;
	char* m_msg;
private:
	static Mutex* m_mutex;
};
std::ostream& operator<< (std::ostream& os, const Exception& e);
#define OW_THROW(exType, msg) throw exType(__FILE__, __LINE__, msg)
#define OW_THROWL(exType, line, msg) throw exType(__FILE__, line, msg)
#define DECLARE_EXCEPTION(NAME) \
class NAME##Exception : public Exception \
{ \
public: \
	NAME##Exception(); \
	NAME##Exception(const char* file, int line, const char* msg); \
	NAME##Exception(const char* msg); \
	virtual ~NAME##Exception() throw(); \
	virtual const char* type() const; \
};
#define DEFINE_EXCEPTION(NAME) \
NAME##Exception::NAME##Exception() : Exception() {} \
NAME##Exception::NAME##Exception(const char* file, int line, const char* msg) \
	: Exception(file, line, msg) {} \
NAME##Exception::NAME##Exception(const char* msg) : Exception(msg) {} \
NAME##Exception::~NAME##Exception() throw() { } \
\
const char* NAME##Exception::type() const { return #NAME "Exception"; } \

} // end namespace OpenWBEM

typedef OpenWBEM::Exception OW_Exception;

#endif
