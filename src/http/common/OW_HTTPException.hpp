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

#ifndef __OW_HTTPEXCEPTION_HPP__
#define __OW_HTTPEXCEPTION_HPP__

#include "OW_config.h"

#include "OW_HTTPStatusCodes.hpp"
#include "OW_Exception.hpp"

#define HTTP_ERR_NEEDCONTENTLEN "HTTP_ERR_NEEDCONTENTLEN"
#define HTTP_ERR_OPTIONSREQUESTED "HTTP_ERR_OPTIONSREQUESTED"
#define HTTP_ERR_AUTHENTICATE "HTTP_ERR_AUTHENTICATE"
#define HTTP_ERR_UNKNOWN "HTTP_ERR_UNKNOWN"
#define HTTP_ERR_TRUNCATED "HTTP_ERR_TRUNCATED"
#define HTTP_ERR_ILLEGALRESPONSE "HTTP_ERR_ILLEGALRESPONSE"

class OW_HTTPException : public OW_Exception
{
public:
	OW_HTTPException() : OW_Exception(), m_response(-1) {}
	OW_HTTPException(const char* file, int line, const char* msg)
		: OW_Exception(file, line, msg), m_response(-1){}
	OW_HTTPException(const char* file, int line, const char* msg, int response)
		: OW_Exception(file, line, msg), m_response(response) {}
	OW_HTTPException(const char* msg) : OW_Exception(msg),
		m_response(-1) {}
	const char* getId() { return getMessage(); }
	int getErrorCode() { return m_response; }

	virtual const char* type() const { return "OW_HTTPException"; }
private:
	int m_response;
};

#define OW_HTTP_THROW(exType, msg, code) throw exType(__FILE__, __LINE__, msg, code)


#endif //__HTTPEXCEPTION_HPP__


