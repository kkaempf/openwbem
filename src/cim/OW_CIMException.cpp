/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ExceptionIds.hpp"
#include "OW_CIMException.hpp"
#include "OW_String.hpp"
#include "OW_Assertion.hpp"
#include "OW_StringBuffer.hpp"

#include <cstring>
#include <cstdlib>
#include <algorithm> // for std::swap

namespace OpenWBEM
{

namespace
{
String createLongMessage(CIMException::ErrNoType errval, const char* msg)
{
	const char* rv(0);
	try
	{
		StringBuffer codeDesc(CIMException::getCodeDescription(errval));
		String msgStr(msg);
        // avoid multiple appendings of the exception message
		if (codeDesc == msgStr.substring(0, codeDesc.length()))
		{
			codeDesc = msgStr;
		}
		else if (!msgStr.empty())
		{
			codeDesc += " (";
			codeDesc += msgStr;
			codeDesc += ')';
		}
		return codeDesc.releaseString();
	}
	catch (std::bad_alloc&)
	{
		return String();
	}
}

}

//////////////////////////////////////////////////////////////////////////////
CIMException::CIMException(const char* file, int line, CIMException::ErrNoType errval,
	const char* msg, const Exception* otherException)
	: Exception(file, line, createLongMessage(errval, msg).c_str(), errval, otherException, ExceptionIds::CIMExceptionId)
	, m_description(Exception::dupString(msg))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMException::~CIMException() throw()
{
	if (m_description)
	{
		delete[] m_description;
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMException::CIMException(const CIMException& x)
	: Exception(x)
	, m_description(Exception::dupString(x.m_description))
{
}
//////////////////////////////////////////////////////////////////////////////
CIMException&
CIMException::operator=(const CIMException& x)
{
	CIMException(x).swap(*this);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMException::swap(CIMException& x)
{
	std::swap(m_description, x.m_description);
	Exception::swap(x);
}
//////////////////////////////////////////////////////////////////////////////					
const char*
CIMException::type() const
{
	return "CIMException";
}

//////////////////////////////////////////////////////////////////////////////					
const char*
CIMException::getDescription() const
{
	return m_description;
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
	{ CIMException::METHOD_NOT_FOUND, "extrinsic method does not exist" }
};

// static
const char*
CIMException::getCodeDescription(ErrNoType errCode)
{
	if (errCode >= SUCCESS && errCode <= METHOD_NOT_FOUND)
	{
		OW_ASSERT(_pmsgs[errCode].errval == errCode);
		return _pmsgs[errCode].msg;
	}
	return "unknown error";
}

CIMException*
CIMException::clone() const throw()
{
	return new(std::nothrow) CIMException(*this);
}

} // end namespace OpenWBEM

