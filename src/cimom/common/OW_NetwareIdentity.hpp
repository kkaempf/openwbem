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

/**
 * @author Jon Carey
 */

#ifndef __OW_NETWAREIDENTITY_HPP__
#define __OW_NETWAREIDENTITY_HPP__

#include "OW_config.h"

#ifdef OW_NETWARE
#define NOVELL_IDENTITY_KEY "NoVeLlIdEnTiTyKeY"
#include "OW_OperationContext.hpp"
#include "OW_String.hpp"

extern "C"
{
#include <nks/dirio.h>
}

namespace OpenWBEM
{

class NetwareIdentity : public OperationContext::Data
{
public:
	NetwareIdentity();
	NetwareIdentity(int identity, NXPathCtx_t pathContext,
		const String& userName, const String& userDN, 
		bool isAdmin);
	~NetwareIdentity();

	/**
	 * @return true if this Netware Identity is valid. Otherwise
	 *		return false.
	 */
	bool isValid() const { return m_valid; }

	bool isAdmin() const { return m_isAdmin; }

	bool setContextToUser() const;
	bool setContextToAdmin() const;
	String getUserDN() const;
	String getUserName() const;
	void setAdminFlag(bool isAdmin=true);

private:

	int m_identity;
	NXPathCtx_t m_pathContext;
	String m_userName;
	String m_userDN;
	bool m_valid;
	bool m_isAdmin;
};

typedef IntrusiveReference<NetwareIdentity> NetwareIdentityRef;

}	// End of namespace OpenWBEM

#endif	// OW_NETWARE

#endif	// __OW_NETWAREIDENTITY_HPP__


