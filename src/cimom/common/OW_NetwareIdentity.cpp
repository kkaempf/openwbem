/*******************************************************************************
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"

#ifdef OW_NETWARE

#include "OW_NetwareIdentity.hpp"

#include <iostream>
using std::cerr;
using std::endl;

extern "C"
{
#include <client.h>
#include <fsio.h>
}

namespace OW_NAMESPACE
{

namespace
{

int
doSetcwd(NXPathCtx_t pathCtx)
{
	int cc = setcwd2(pathCtx);
	if (cc != 0)
	{
		cerr << "** NetwareIdentity; setcwd2 failed: ";
		switch (cc)
		{
			case 4:
				cerr << "Invalid file descriptor" << endl;
				break;
			case 5:
				cerr << "Insufficient memory" << endl;
				break;
			case 9:
				cerr << "Invalid parameter" << endl;
				break;
			case 28:
				cerr << "Volume not mounted or file system error" << endl;
				break;
			case 65:
				cerr << "File system name for a path component is too long"
					<< endl;
				break;
			case 67:
				cerr << "Object is not a directory" << endl;
				break;
			case 79:
				cerr << "Operation is not supported in the manner requested"
					<< endl;
				break;
			case 105:
				cerr << "thread context is invalid or not available"
					<< endl;
				break;
			case 106:
				cerr << "Name space is invalid" << endl;
				break;
			case 107:
				cerr << "NCP connection is not valid" << endl;
				break;
			case 114:
				cerr << "Security badge is invalid" << endl;
				break;
			default:
				cerr << "Error = " << cc << endl;
				break;
		}
	}

	return cc;
}

}	// End of anonymous namespace

//////////////////////////////////////////////////////////////////////////////
NetwareIdentity::NetwareIdentity()
	: OperationContext::Data()
	, m_identity(0)
	, m_pathContext(0)
	, m_userName()
	, m_userDN()
	, m_valid(false)
	, m_isAdmin(false)
{
}

//////////////////////////////////////////////////////////////////////////////
NetwareIdentity::NetwareIdentity(int identity, NXPathCtx_t pathContext,
	const String& userName, const String& userDN, bool m_isAdmin)
	: OperationContext::Data()
	, m_identity(identity)
	, m_pathContext(pathContext)
	, m_userName(userName)
	, m_userDN(userDN)
	, m_valid(true)
	, m_isAdmin(false)
{
	int error;
	if (!is_valid_identity(m_identity, &error))
	{
		m_valid = false;
		cerr << "** Invalid identity passed to NetwareIdentity CTOR: ";
		switch (error)
		{
			case 22:
				cerr << "Bad identity. Could not be found" << endl;
				break;
			case 105:
				cerr << "Thread has no context" << endl;
			default:
				cerr << "Error = " << error << endl;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
NetwareIdentity::~NetwareIdentity()
{
	if (m_valid && !m_isAdmin)
	{
		int cc = NXFreePathContext(m_pathContext);
		if(cc != 0)
		{
			cerr << "** NXFreePathContext Failed: ";
			switch (cc)
			{
				case 4:
					cerr << "Invalid path context" << endl;
					break;
				case 105:
					cerr << "Thread has no NKS context" << endl;
				default:
					cerr << "Error = " << cc << endl;
			}
		}

		delete_identity(m_identity);
	}
}

//////////////////////////////////////////////////////////////////////////////
bool 
NetwareIdentity::setContextToUser() const 
{
	bool cc = (m_isAdmin) ? m_isAdmin : m_valid;

	if (cc)
	{
		if(doSetcwd(m_pathContext) != 0)
		{
			cc = false;
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
bool 
NetwareIdentity::setContextToAdmin() const
{ 
	bool cc = (m_isAdmin) ? m_isAdmin : m_valid;

	if (cc)
	{
		if (doSetcwd(0) != 0)
		{
			cc = false;
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
String 
NetwareIdentity::getUserDN() const 
{ 
	return m_userDN; 
}

//////////////////////////////////////////////////////////////////////////////
String 
NetwareIdentity::getUserName() const 
{ 
	return m_userName; 
}

//////////////////////////////////////////////////////////////////////////////
void 
NetwareIdentity::setAdminFlag(bool isAdmin)
{
	m_isAdmin = isAdmin;
}

}	// End of namespace OpenWBEM

#endif	// OW_NETWARE

