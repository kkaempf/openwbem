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
#include "OW_CIMException.hpp"
#include "OW_String.hpp"
#include <cstring>
#include <cstdlib>
#include <algorithm> // for std::swap

namespace OpenWBEM
{

static String getMsg(CIMException::ErrNoType errval, const char* msg = 0);
//////////////////////////////////////////////////////////////////////////////
CIMException::CIMException(CIMException::ErrNoType errval) :
	Exception(), m_errno(errval), m_longmsg(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMException::CIMException(const char* file, int line, CIMException::ErrNoType errval,
	const char* msg) :
	Exception(file, line, msg), m_errno(errval), m_longmsg(0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMException::~CIMException() throw()
{
	if (m_longmsg)
	{
		free(m_longmsg);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMException::CIMException(const CIMException& x)
	: Exception(x)
	, m_errno(x.m_errno)
	, m_longmsg(x.m_longmsg ? strdup(x.m_longmsg) : 0)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMException&
CIMException::operator=(CIMException x)
{
	x.swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMException::swap(CIMException& x)
{
	Exception::swap(x);
	std::swap(m_longmsg, x.m_longmsg);
	std::swap(m_errno, x.m_errno);
}
//////////////////////////////////////////////////////////////////////////////					
const char*
CIMException::getMessage() const
{
	if (!m_longmsg)
	{
		m_longmsg = strdup(getMsg(m_errno, Exception::getMessage()).c_str());
	}
	return m_longmsg;
}
//////////////////////////////////////////////////////////////////////////////
struct MsgRec
{
	CIMException::ErrNoType errval;
	const char* msg;
};
static MsgRec _pmsgs[] =
{
	{ CIMException::SUCCESS, "no error" },
	{ CIMException::FAILED, "general error" },
	{ CIMException::ACCESS_DENIED, "Access to CIM resource unavailable to client" },
	{ CIMException::INVALID_NAMESPACE, "namespace does not exist" },
	{ CIMException::INVALID_PARAMETER, "invalid parameter passed to method" },
	{ CIMException::INVALID_CLASS, "class does not exist" },
	{ CIMException::NOT_FOUND, "requested object could not be found" },
	{ CIMException::NOT_SUPPORTED, "requested operation is not supported" },
	{ CIMException::CLASS_HAS_CHILDREN, "operation cannot be done on class with subclasses" },
	{ CIMException::CLASS_HAS_INSTANCES, "operator cannot be done on class with instances" },
	{ CIMException::INVALID_SUPERCLASS, "specified superclass does not exist" },
	{ CIMException::ALREADY_EXISTS, "object already exists" },
	{ CIMException::NO_SUCH_PROPERTY, "specified property does not exist" },
	{ CIMException::TYPE_MISMATCH, "value supplied is incompatible with type" },
	{ CIMException::QUERY_LANGUAGE_NOT_SUPPORTED, "query language is not recognized or supported" },
	{ CIMException::INVALID_QUERY, "query is not valid for the specified query language" },
	{ CIMException::METHOD_NOT_AVAILABLE, "extrinsic method could not be executed" },
	{ CIMException::METHOD_NOT_FOUND, "extrinsic method does not exist" },
	{ CIMException::SUCCESS, 0 }
};
static String
getMsg(CIMException::ErrNoType err, const char* msg)
{
	const char* p = "unknown error";
	for(int i = 0; _pmsgs[i].msg != NULL; i++)
	{
		if(err == _pmsgs[i].errval)
		{
			p = _pmsgs[i].msg;
			break;
		}
	}
	String rstr(p);
	// avoid multiple appendings of the exception message
	if (rstr == String(msg).substring(0, rstr.length()))
	{
		rstr = msg;
	}
	else if(msg != NULL && strlen(msg) > 0)
	{
		rstr += " (";
		rstr += msg;
		rstr += ")";
	}
	return rstr;
}

} // end namespace OpenWBEM

