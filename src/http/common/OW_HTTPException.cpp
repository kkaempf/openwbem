/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_HTTPException.hpp"

namespace OW_NAMESPACE
{

using namespace blocxx;
/////////////////////////////////////////////////////////////////////////////
HTTPException::HTTPException(const char* file, int line, const char* msg)
	: Exception(file, line, msg, Exception::UNKNOWN_ERROR_CODE, 0, ExceptionIds::HTTPExceptionId), m_response(-1)
{
}
/////////////////////////////////////////////////////////////////////////////
HTTPException::HTTPException(const char* file, int line, const char* msg, int response)
	: Exception(file, line, msg, response, 0, ExceptionIds::HTTPExceptionId), m_response(response)
{
}
/////////////////////////////////////////////////////////////////////////////
HTTPException::~HTTPException() throw()
{
}
/////////////////////////////////////////////////////////////////////////////
const char*
HTTPException::getId()
{
	return getMessage();
}
/////////////////////////////////////////////////////////////////////////////
int
HTTPException::getErrorCode()
{
	return m_response;
}
/////////////////////////////////////////////////////////////////////////////
const char*
HTTPException::type() const
{
	return "HTTPException";
}
/////////////////////////////////////////////////////////////////////////////
HTTPException*
HTTPException::clone() const throw()
{
	return new (std::nothrow)HTTPException(*this);
}

} // end namespace OW_NAMESPACE


