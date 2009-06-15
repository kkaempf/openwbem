/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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

#ifndef OW_LOCAL_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#define OW_LOCAL_OPERATION_CONTEXT_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_OperationContext.hpp"
#include "blocxx/SortedVectorMap.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_COMMON_API LocalOperationContext: public OperationContext
{
public:
	
	LocalOperationContext();
	virtual ~LocalOperationContext();

private:

	// derived class interface

	/**
	 * overwrite data and return true if key is found, else return false.
	 */
	virtual bool doGetData(const String& key, DataRef& data) const;
	/**
	 * Caller creats a subclass of Data and passes it in.
	 * @param key
	 */
	virtual void doSetData(const String& key, const DataRef& data);

	/**
	 * Remove the data identified by key.  It is not an error if key has not
	 * already been added to the context with setData().
	 * @param key Identifies the data to remove.
	 */
	virtual void doRemoveData(const String& key);
	
	/**
	 * Test whether there is data for the key.
	 * @param key The key to test.
	 * @return true if there is data for the key.
	 */
	virtual bool doKeyHasData(const String& key) const;

	/**
	 * @return The operation id. Each operation has a unique id.
	 */
	virtual UInt64 doGetOperationId() const;

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	SortedVectorMap<String, DataRef> m_data;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	UInt64 m_operationId;

	// non-copyable
	LocalOperationContext(const LocalOperationContext&);
	LocalOperationContext& operator=(const LocalOperationContext&);
};

} // end namespace OW_NAMESPACE

#endif



