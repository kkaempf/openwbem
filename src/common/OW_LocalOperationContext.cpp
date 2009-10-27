/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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

#include "OW_config.h"
#include "OW_LocalOperationContext.hpp"
#include "blocxx/GlobalMutex.hpp"
#include "blocxx/MutexLock.hpp"

using namespace blocxx;

namespace OW_NAMESPACE
{

namespace
{

GlobalMutex g_mtx = BLOCXX_GLOBAL_MUTEX_INIT();
UInt64 g_operationCount = 0;
UInt64 getNewOperationId()
{
	MutexLock lock(g_mtx);
	return g_operationCount++;
}

} // end anonymous namespace

LocalOperationContext::LocalOperationContext()
: m_operationId(getNewOperationId())
{

}

LocalOperationContext::~LocalOperationContext()
{
}

bool
LocalOperationContext::doGetData(const String& key, DataRef& data) const
{
	SortedVectorMap<String, DataRef>::const_iterator ci = m_data.find(key);
	if (ci != m_data.end())
	{
		data = ci->second;
		return true;
	}
	return false;
}

void
LocalOperationContext::doSetData(const String& key, const DataRef& data)
{
	m_data[key] = data;
}

void
LocalOperationContext::doRemoveData(const String& key)
{
	m_data.erase(key);
}

bool
LocalOperationContext::doKeyHasData(const String& key) const
{
	return m_data.count(key) > 0;
}

UInt64
LocalOperationContext::doGetOperationId() const
{
	return m_operationId;
}


} // end namespace OW_NAMESPACE





