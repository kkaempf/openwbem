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
#ifndef OW_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#define OW_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Reference.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_Exception.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

class UserInfo;

OW_DECLARE_EXCEPTION(ContextDataNotFound);

/////////////////////////////////////////////////////////////////////////////
class OperationContext
{
public:
	
	class Data {
	public:
		virtual ~Data(); // subclasses can clean-up & free memory
	};

	typedef Reference<Data> DataRef;
	
	OperationContext();

	// caller creats a subclass of Data and passes it in.
	void setData(const String& key, const DataRef& data);

	/**
	 * Remove the data identified by key.  It is not an error if key has not
	 * already been added to the context with setData().
	 * @param key Identifies the data to remove.
	 */
	void removeData(const String& key);
	
	// caller uses Reference::cast_to<>() on the return value to attempt to
	// recover the original type passed into storeData.
	DataRef getData(const String& key) const;
	
	// These are for convenience, and are implemented in terms of
	// the first 2 functions.
	void setStringData(const String& key, const String& str);
	// @throws ContextDataNotFoundException if key is not found
	String getStringData(const String& key) const;
	
	// Keys values we use.
	static const char* const USER_NAME;
	static const char* const HTTP_PATH;
	static const char* const CIMOM_UIDKEY;
	static const char* const CURUSER_UIDKEY;



	UserInfo getUserInfo() const;

private:
	SortedVectorMap<String, DataRef> m_data;


	// non-copyable
	OperationContext(const OperationContext&);
	OperationContext& operator=(const OperationContext&);
};

} // end namespace OpenWBEM

#endif


