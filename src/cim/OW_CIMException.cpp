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
#include <ostream>

namespace OW_NAMESPACE
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
	: Exception(file, line, msg, errval, otherException, ExceptionIds::CIMExceptionId)
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
namespace
{

struct MsgRec
{
	CIMException::ErrNoType errval;
	const char* msg;
};

static MsgRec names[] =
{
	{ CIMException::SUCCESS, "SUCCESS" },
	{ CIMException::FAILED, "CIM_ERR_FAILED" },
	{ CIMException::ACCESS_DENIED, "CIM_ERR_ACCESS_DENIED" },
	{ CIMException::INVALID_NAMESPACE, "CIM_ERR_INVALID_NAMESPACE" },
	{ CIMException::INVALID_PARAMETER, "CIM_ERR_INVALID_PARAMETER" },
	{ CIMException::INVALID_CLASS, "CIM_ERR_INVALID_CLASS" },
	{ CIMException::NOT_FOUND, "CIM_ERR_NOT_FOUND" },
	{ CIMException::NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" },
	{ CIMException::CLASS_HAS_CHILDREN, "CIM_ERR_CLASS_HAS_CHILDREN" },
	{ CIMException::CLASS_HAS_INSTANCES, "CIM_ERR_CLASS_HAS_INSTANCES" },
	{ CIMException::INVALID_SUPERCLASS, "CIM_ERR_INVALID_SUPERCLASS" },
	{ CIMException::ALREADY_EXISTS, "CIM_ERR_ALREADY_EXISTS" },
	{ CIMException::NO_SUCH_PROPERTY, "CIM_ERR_NO_SUCH_PROPERTY" },
	{ CIMException::TYPE_MISMATCH, "CIM_ERR_TYPE_MISMATCH" },
	{ CIMException::QUERY_LANGUAGE_NOT_SUPPORTED, "CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED" },
	{ CIMException::INVALID_QUERY, "CIM_ERR_INVALID_QUERY" },
	{ CIMException::METHOD_NOT_AVAILABLE, "CIM_ERR_METHOD_NOT_AVAILABLE" },
	{ CIMException::METHOD_NOT_FOUND, "CIM_ERR_METHOD_NOT_FOUND" }
};

} // end unnamed namespace

// static
const char*
CIMException::getCodeName(ErrNoType errCode)
{
	if (errCode >= SUCCESS && errCode <= METHOD_NOT_FOUND)
	{
		OW_ASSERT(names[errCode].errval == errCode);
		return names[errCode].msg;
	}
	return "CIM_ERR_UNKNOWN";
}

namespace
{
static MsgRec _pmsgs[] =
{
	{ CIMException::SUCCESS, "No error." },
	{ CIMException::FAILED, "A general error occurred that is not covered by a more specific error code." },
	{ CIMException::ACCESS_DENIED, "Access to a CIM resource is not available to the client." },
	{ CIMException::INVALID_NAMESPACE, "The target namespace does not exist." },
	{ CIMException::INVALID_PARAMETER, "One or more parameter values passed to the method are not valid." },
	{ CIMException::INVALID_CLASS, "The specified class does not exist." },
	{ CIMException::NOT_FOUND, "The requested object cannot be found." },
	{ CIMException::NOT_SUPPORTED, "The requested operation is not supported." },
	{ CIMException::CLASS_HAS_CHILDREN, "The operation cannot be invoked on this class because it has subclasses." },
	{ CIMException::CLASS_HAS_INSTANCES, "The operation cannot be invoked on this class because one or more instances of this class exist." },
	{ CIMException::INVALID_SUPERCLASS, "The operation cannot be invoked because the specified superclass does not exist." },
	{ CIMException::ALREADY_EXISTS, "The operation cannot be invoked because an object already exists." },
	{ CIMException::NO_SUCH_PROPERTY, "The specified property does not exist." },
	{ CIMException::TYPE_MISMATCH, "The value supplied is not compatible with the type." },
	{ CIMException::QUERY_LANGUAGE_NOT_SUPPORTED, "The query language is not recognized or supported." },
	{ CIMException::INVALID_QUERY, "The query is not valid for the specified query language." },
	{ CIMException::METHOD_NOT_AVAILABLE, "The extrinsic method cannot be invoked." },
	{ CIMException::METHOD_NOT_FOUND, "The specified extrinsic method does not exist." }
};

} // end unnamed namespace

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

std::ostream& operator<<(std::ostream& os, const CIMException& e)
{
	if (*e.getFile() == '\0')
	{
		os << "[no file]: ";
	}
	else
	{
		os << e.getFile() << ": ";
	}
	
	if (e.getLine() == 0)
	{
		os << "[no line] ";
	}
	else
	{
		os << e.getLine() << ' ';
	}
	
	os << createLongMessage(e.getErrNo(), e.getMessage());

	const Exception* subEx = e.getSubException();
	if (subEx)
	{
		os << " <" << *subEx << '>';
	}
	return os;
}

} // end namespace OW_NAMESPACE

