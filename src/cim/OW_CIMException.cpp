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

static OW_String getMsg(OW_CIMException::ErrNoType errval, const char* msg = 0);

//////////////////////////////////////////////////////////////////////////////
OW_CIMException::OW_CIMException(OW_CIMException::ErrNoType errval) :
	OW_Exception(), m_errno(errval), m_longmsg(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMException::OW_CIMException(const char* file, int line, OW_CIMException::ErrNoType errval,
	const char* msg) :
	OW_Exception(file, line, msg), m_errno(errval), m_longmsg(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMException::~OW_CIMException() throw()
{
	if (m_longmsg)
	{
		free(m_longmsg);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMException::OW_CIMException(const OW_CIMException& x)
	: OW_Exception(x)
	, m_errno(x.m_errno)
	, m_longmsg(x.m_longmsg ? strdup(x.m_longmsg) : 0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMException&
OW_CIMException::operator=(OW_CIMException x)
{
	x.swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMException::swap(OW_CIMException& x)
{
	OW_Exception::swap(x);
	std::swap(m_longmsg, x.m_longmsg);
	std::swap(m_errno, x.m_errno);
}

//////////////////////////////////////////////////////////////////////////////					
const char*
OW_CIMException::getMessage() const
{
	if (!m_longmsg)
	{
		m_longmsg = strdup(getMsg(m_errno, OW_Exception::getMessage()).c_str());
	}
	return m_longmsg;
}


//////////////////////////////////////////////////////////////////////////////
struct MsgRec
{
	OW_CIMException::ErrNoType errval;
	const char* msg;
};

static MsgRec _pmsgs[] =
{
	{ OW_CIMException::SUCCESS, "no error" },
	{ OW_CIMException::FAILED, "general error" },
	{ OW_CIMException::ACCESS_DENIED, "Access to CIM resource unavailable to client" },
	{ OW_CIMException::INVALID_NAMESPACE, "namespace does not exist" },
	{ OW_CIMException::INVALID_PARAMETER, "invalid parameter passed to method" },
	{ OW_CIMException::INVALID_CLASS, "class does not exist" },
	{ OW_CIMException::NOT_FOUND, "requested object could not be found" },
	{ OW_CIMException::NOT_SUPPORTED, "requested operation is not supported" },
	{ OW_CIMException::CLASS_HAS_CHILDREN, "operation cannot be done on class with subclasses" },
	{ OW_CIMException::CLASS_HAS_INSTANCES, "operator cannot be done on class with instances" },
	{ OW_CIMException::INVALID_SUPERCLASS, "specified superclass does not exist" },
	{ OW_CIMException::ALREADY_EXISTS, "object already exists" },
	{ OW_CIMException::NO_SUCH_PROPERTY, "specified property does not exist" },
	{ OW_CIMException::TYPE_MISMATCH, "value supplied is incompatible with type" },
	{ OW_CIMException::QUERY_LANGUAGE_NOT_SUPPORTED, "query language is not recognized or supported" },
	{ OW_CIMException::INVALID_QUERY, "query is not valid for the specified query language" },
	{ OW_CIMException::METHOD_NOT_AVAILABLE, "extrinsic method could not be executed" },
	{ OW_CIMException::METHOD_NOT_FOUND, "extrinsic method does not exist" },
	{ OW_CIMException::SUCCESS, 0 }
};

static OW_String
getMsg(OW_CIMException::ErrNoType err, const char* msg)
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

	OW_String rstr(p);
	// avoid multiple appendings of the exception message
	if (rstr == OW_String(msg).substring(0, rstr.length()))
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

