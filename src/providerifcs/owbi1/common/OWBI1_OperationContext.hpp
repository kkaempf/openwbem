/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OWBI1_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#define OWBI1_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#include "OWBI1_config.h"
#include "OWBI1_IntrusiveCountableBase.hpp"
#include "OWBI1_CommonFwd.hpp"
#include "OWBI1_Exception.hpp"
#include "OWBI1_String.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OWBI1
{

OWBI1_DECLARE_APIEXCEPTION(ContextDataNotFound, OWBI1_COMMON_API);

/**
 * This class is used to store general information (the context) of a
 * WBEM operation. It works like an associative container, with a String key,
 * and the data is an OperationContext::DataRef. The idea is for a client
 * of this class to create a subclass of OperationContext::Data and then
 * pass a smart pointer to it into setData().
 * Convenience functions (and a subclass) are provided to use a String for
 * the Data.
 *
 * Thread safety: None
 * Copy Semantics: Non-copyable
 */
class OWBI1_OWBI1PROVIFC_API OperationContext : public IntrusiveCountableBase
{
public:
	
	OperationContext();
	virtual ~OperationContext();

	/**
	 * Remove the data identified by key.  It is not an error if key has not
	 * already been added to the context with setData().
	 * @param key Identifies the data to remove.
	 */
	virtual void removeData(const String& key) = 0;
	
	/**
	 * Test whether there is data for the key.
	 * @param key The key to test.
	 * @return true if there is data for the key.
	 */
	virtual bool keyHasData(const String& key) const = 0;
	
	/**
	 * These are for convenience, and are implemented in terms of
	 * the first 2 functions.
	 */
	virtual void setStringData(const String& key, const String& str) = 0;

	/**
	 * @throws ContextDataNotFoundException if key is not found
	 */
	virtual String getStringData(const String& key) const = 0;

	/**
	 * @returns def if key is not found
	 */
	virtual String getStringDataWithDefault(const String& key, const String& def = String() ) const = 0;
	
	// Keys values we use.
	static const char* const USER_NAME;
	static const char* const USER_PASSWD;
	static const char* const HTTP_PATH;
	static const char* const CURUSER_UIDKEY;
	static const char* const BYPASS_LOCKERKEY;
	static const char* const SESSION_LANGUAGE_KEY;
	static const char* const HTTP_ACCEPT_LANGUAGE_KEY;
	static const char* const CLIENT_IPADDR;


	// non-copyable
	OperationContext(const OperationContext&);
	OperationContext& operator=(const OperationContext&);
};

} // end namespace OWBI1

#endif


