/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_ClientCIMOMHandleConnectionPool.hpp"
#include "OW_MutexLock.hpp"
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_String.hpp"

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
ClientCIMOMHandleConnectionPool::ClientCIMOMHandleConnectionPool(unsigned maxConnectionsPerUrl)
	: m_maxConnectionsPerUrl(maxConnectionsPerUrl)
{
}

//////////////////////////////////////////////////////////////////////////////
ClientCIMOMHandleConnectionPool::~ClientCIMOMHandleConnectionPool()
{
}

//////////////////////////////////////////////////////////////////////////////
ClientCIMOMHandleRef
ClientCIMOMHandleConnectionPool::getConnection(const String& url)
{
	MutexLock lock(m_guard);
	pool_t::iterator iter = m_pool.find(url);
	if (iter == m_pool.end())
	{
		return ClientCIMOMHandle::createFromURL(url);
	}
	else
	{
		ClientCIMOMHandleRef rv = iter->second;
		m_pool.erase(iter);
		return rv;
	}
	// Not reachable.
	return ClientCIMOMHandleRef();
}

//////////////////////////////////////////////////////////////////////////////
void
ClientCIMOMHandleConnectionPool::addConnectionToPool(const ClientCIMOMHandleRef& handle, const String& url)
{
	MutexLock lock(m_guard);
	std::pair<pool_t::iterator, pool_t::iterator> range = m_pool.equal_range(url);
	if (std::distance(range.first, range.second) < static_cast<ptrdiff_t>(m_maxConnectionsPerUrl))
	{
		m_pool.insert(pool_t::value_type(url, handle));
	}
	// else don't add the handle to the pool
}

} // end namespace OW_NAMESPACE



