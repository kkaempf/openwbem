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
#ifndef OW_ACLINFO_HPP_INCLUDE_GUARD_
#define OW_ACLINFO_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_String.hpp"

/**
 * The OW_ACLInfo class is used to encapsulate information about the user
 * making a request of the CIMOM. This is information is retreived at
 * the time the user authenticates to the CIMOM. If an OW_ACLInfo object
 * contains a user name, then read/write request will be verified against
 * the access rights of that user. If an OW_ACLInfo object does NOT contain
 * user a user name and the m_internal flag is set, then access rights are
 * not checked on read/write operations. Only select components of the
 * CIMOM are allowed to construct this type of an OW_ACLInfo object.
 */
class OW_ACLInfo
{
public:
	/**
	 *	Create a new OW_ACLInfo object.
	 * @param userName	The user name to use when checking access rights for
	 * 						read/write operations.
	 */
	OW_ACLInfo(const OW_String& userName) :
		m_userName(userName), m_internal(false) {}

	/**
	 * Create a new OW_ACLInfo object that is a copy of another.
	 * @param arg	The OW_ACLInfo object that this object will be a copy of.
	 */
	OW_ACLInfo(const OW_ACLInfo& arg) :
		m_userName(arg.m_userName), m_internal(arg.m_internal) {}

	/**
	 * Assign another OW_ACLInfo object to this one.
	 * @param arg	The OW_ACLInfo object to assign to this one.
	 * @return A references to this OW_ACLInfo object after the assignment is
	 * made.
	 */
	OW_ACLInfo& operator= (const OW_ACLInfo& arg)
	{
		m_userName = arg.m_userName;
		m_internal = arg.m_internal;
		return *this;
	}

	/**
	 * @return The user name that is associated with this OW_ACLInfo object.
	 */
	OW_String getUserName() const { return m_userName; }


private:

	/**
	 * Create a new OW_ACLInfo object that access rights will not be checked on.
	 * This method is private so only friend class are allowed to create this
	 * type of OW_ACLInfo object.
	 */
	OW_ACLInfo() :
		m_userName(), m_internal(true) {}

	/**
	 * @return true if access rights are not to be checked on this OW_ACLInfo
	 * object. Otherwise false.
	 */
	OW_Bool getInternal() const { return m_internal; }

	/**
	 * Set this OW_ACLInfo object to internal
	 */
	void setInternal() { m_internal = true; }

	/** The user name associated with this OW_ACLInfo object. */
	OW_String m_userName;

	/** Internal flags. If true access rights are not checked. */
	OW_Bool m_internal;

	//--------------------------------------------------------------------------
	// Friend classes that are allowed to construct OW_ACLInfo objects that
	// access rights are not checked on.
	//--------------------------------------------------------------------------
	friend class OW_CIMServer;
	friend class OW_AccessMgr;
	friend class OW_IndicationRepLayerImpl;
	friend class OW_PollingManager;
	friend class OW_IndicationServerImpl;
	friend class OW_CIMOMEnvironment;
	friend class OW_CIMRepository;
};

#endif

