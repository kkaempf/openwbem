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

#ifndef OW_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#define OW_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Exception.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

class UserInfo;

OW_DECLARE_APIEXCEPTION(ContextDataNotFound, OW_COMMON_API);

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
class OW_COMMON_API OperationContext
{
public:
	
	class OW_COMMON_API Data : public IntrusiveCountableBase
	{
	public:
		virtual ~Data(); // subclasses can clean-up & free memory
	};

	typedef IntrusiveReference<Data> DataRef;
	
	OperationContext();

	/**
	 * Caller creats a subclass of Data and passes it in.
	 * @param key
	 */
	void setData(const String& key, const DataRef& data);

	/**
	 * Remove the data identified by key.  It is not an error if key has not
	 * already been added to the context with setData().
	 * @param key Identifies the data to remove.
	 */
	void removeData(const String& key);
	
	/**
	 * caller uses IntrusiveReference::cast_to<>() on the return value to attempt to
	 * recover the original type passed into storeData.
	 * @return The same DataRef associated with key that was passed to setData().
	 *  A NULL DataRef if key is not valid.
	 */
	DataRef getData(const String& key) const;

	/**
	 * Test whether there is data for the key.
	 * @param key The key to test.
	 * @return true if there is data for the key.
	 */
	bool keyHasData(const String& key) const;
	
	/**
	 * These are for convenience, and are implemented in terms of
	 * the first 2 functions.
	 */
	void setStringData(const String& key, const String& str);

	/**
	 * @throws ContextDataNotFoundException if key is not found
	 */
	String getStringData(const String& key) const;

	/**
	 * @returns def if key is not found
	 */
	String getStringDataWithDefault(const String& key, const String& def = String() ) const;
	
	// Keys values we use.
	static const char* const USER_NAME;
	static const char* const USER_PASSWD;
	static const char* const HTTP_PATH;
	static const char* const CURUSER_UIDKEY;
	static const char* const BYPASS_LOCKERKEY; 



	UserInfo getUserInfo() const;

private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	SortedVectorMap<String, DataRef> m_data;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	// non-copyable
	OperationContext(const OperationContext&);
	OperationContext& operator=(const OperationContext&);
};

} // end namespace OpenWBEM

#endif


