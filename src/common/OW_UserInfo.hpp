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

#ifndef OW_USERINFO_HPP_INCLUDE_GUARD_
#define OW_USERINFO_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"

namespace OpenWBEM
{

/**
 * The UserInfo class is used to encapsulate information about the user
 * making a request of the CIMOM. This is information is retreived at
 * the time the user authenticates to the CIMOM. If an UserInfo object
 * contains a user name, then read/write request will be verified against
 * the access rights of that user. If an UserInfo object does NOT contain
 * user a user name and the m_internal flag is set, then access rights are
 * not checked on read/write operations. Only select components of the
 * CIMOM are allowed to construct this type of an UserInfo object.
 * Invariants:
 *  - (getInternal() == false && !getUserName().empty()) || (getInternal() == true && getUserName().empty())
 * Thread safety: read
 * Copy semantics: Value
 * Exception safety: Strong
 */
class UserInfo
{
public:
	/**
	 *	Create a new UserInfo object.
	 * @param userName	The user name to use when checking access rights for
	 * 						read/write operations.
	 */
	UserInfo(const String& userName);
	UserInfo(const UserInfo& arg);
	UserInfo& operator= (const UserInfo& arg);
	/**
	 * @return The user name that is associated with this UserInfo object.
	 */
	String getUserName() const;
private:
	/**
	 * Create a new UserInfo object that access rights will not be checked on.
	 * This method is private so only friend class are allowed to create this
	 * type of UserInfo object.
	 */
	UserInfo();
	/**
	 * @return true if access rights are not to be checked on this UserInfo
	 * object. Otherwise false.
	 */
	bool getInternal() const;
	/**
	 * Set this UserInfo object to internal
	 */
	void setInternal();
	/** The user name associated with this UserInfo object. */
	String m_userName;
	/** Internal flags. If true access rights are not checked. */
	bool m_internal;
	//--------------------------------------------------------------------------
	// Friend classes that are allowed to construct UserInfo objects that
	// access rights are not checked on.
	//--------------------------------------------------------------------------
	friend class OperationContext;
	friend class AccessMgr;
};

} // end namespace OpenWBEM

#endif
