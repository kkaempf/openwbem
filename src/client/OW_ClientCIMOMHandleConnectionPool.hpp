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

#ifndef OW_CLIENT_CIMOM_HANDLE_CONNECTION_POOL_HPP_INCLUDE_GUARD_
#define OW_CLIENT_CIMOM_HANDLE_CONNECTION_POOL_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_Mutex.hpp"
#include "OW_String.hpp"
#include "OW_ClientCIMOMHandle.hpp"

#include <map>

namespace OpenWBEM
{

class ClientCIMOMHandleConnectionPool;
typedef Reference<ClientCIMOMHandleConnectionPool> ClientCIMOMHandleConnectionPoolRef;

/**
 * This class is responsible for pooling ClientCIMOMHandleRef instances.
 * Each separate url is considered a different cimom.  Connections are
 * persistent.
 * 
 * Invariant: For each given url, a maximum of m_maxConnectionsPerUrl will be 
 *  stored. 
 * Thread safety: read/write
 * Copy semantics: Non-copyable
 * Exception safety: Strong
 */
class ClientCIMOMHandleConnectionPool
{
public:
	/**
	 * @param maxConnectionsPerUrl The maximum number of connections that may
	 * be stored for each given url.
	 */
	ClientCIMOMHandleConnectionPool(unsigned maxConnectionsPerUrl);

	/**
	 * Get a new ClientCIMOMHandleRef connected to url. If there is one in the
	 * pool, it will be retrieved, otherwise a new connection will be created.
	 * The http protocol only authenticates when the connection is established,
	 * so each existing connection will already be authenticated.  For this
	 * reason, the credentials should be part of the url, and a 
	 * ClientAuthCBIFCRef will not be supplied to the ClientCIMOMHandleRef.
	 * 
	 * @param url The url.
	 * 
	 * @return A ClientCIMOMHandleRef connected to url.
	 * 
	 * @throws MalformedURLException If the url is bad
	 * @throws std::bad_alloc
	 * @throws SocketException If an SSL connection was requested, but support for SSL is not available.
	 */
	ClientCIMOMHandleRef getConnection(const String& url);

	/**
	 * Add a ClientCIMOMHandleRef back to the pool.  handle must be connected
	 * to url. If there are already m_maxConnectionsPerUrl ClientCIMOMHandleRef
	 * instances for url already in the pool, it won't be added to the pool.
	 * After calling this function, the caller can no longer use handle.
	 * 
	 * @param handle The ClientCIMOMHandleRef to add to the pool.
	 * @param url The url handle is connected to.
	 */
	void addConnectionToPool(const ClientCIMOMHandleRef& handle, const String& url);

	/**
	 * This class facilitates usage of RAII when using a ClientCIMOMHandleConnectionPool
	 */
	class HandleReturner
	{
	public:
		HandleReturner(ClientCIMOMHandleRef& hdl, ClientCIMOMHandleConnectionPoolRef& pool, const String& url)
			: m_hdl(hdl)
			, m_pool(pool)
			, m_url(url)
		{
		}
	
		~HandleReturner()
		{
			m_pool->addConnectionToPool(m_hdl, m_url);
		}
	private:
		ClientCIMOMHandleRef& m_hdl;
		ClientCIMOMHandleConnectionPoolRef& m_pool;
		const String& m_url;
	};


private:

	// unimplemented
	ClientCIMOMHandleConnectionPool(const ClientCIMOMHandleConnectionPool&);
	ClientCIMOMHandleConnectionPool& operator=(const ClientCIMOMHandleConnectionPool&);

	unsigned m_maxConnectionsPerUrl;
	Mutex m_guard;

	typedef std::multimap<String, ClientCIMOMHandleRef> pool_t;
	pool_t m_pool;
};

}

#endif



