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

#ifndef OW_CACHE_INCLUDE_GUARD_HPP_
#define OW_CACHE_INCLUDE_GUARD_HPP_
#include "OW_config.h"
#include "OW_HashMap.hpp"
#include "OW_String.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_CIMNULL.hpp"
#include <list>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

/** This class encapsulates the functionality of a cache. */
template <typename T>
class Cache
{
public:
	Cache();

	/** Add an item to the cache.
	 * @param cc The item to add
	 * @param key The key for the item
	 * @precondition cc is not already in the cache.  Adding duplicate items into the cache will waste space.
	 * @postconfition getFromCache(key) returns cc.
	 */
	void addToCache(const T& cc, const String& key);
	
	/** Get an item from the cache.  Average complexity is constant time. Worst case is linear in the size of the cache.
	 * @param key The key for the item to retrieve.
	 * @return The item if found, else the item constructed with CIMNULL parameter.
	 */
	T getFromCache(const String& key);
	
	/** Remove an item from the cache.  Average complexity is constant time. Worst case is linear in the size of the cache.
	 * It does not matter if the item is not in the cache.
	 * @postcondition getFromCache(key) will return T(CIMNULL)
	 * @param key The key for the item to remove.
	 */
	void removeFromCache(const String& key);
	
	/** Remove all items from the cache.
	 */
	void clearCache();
	
	/** Set the maximum number of items the cache will hold.  This will shrink
	 * the cache if necessary.
	 */
	void setMaxCacheSize(UInt32);

private:
	// a list of items that are cached.  The list is sorted by lru.  The least
	// recently acessed item will be at begin(), and the most recenly acessed
	// class will be at end()--;  The second part of the pair is the key that
	// will be used in the index.
	// This is a list because we can quickly O(1) move items from the middle
	// to the end of the list when they're accessed. Also we need iterators
	// into the list to be stable.  We can re-arrange items in the list
	// without having to update the HashMap index.
	typedef std::list<std::pair<T, String> > class_cache_t;
	
	// the index into the cache.  Speeds up finding an item when we need to.
	typedef HashMap<String, typename class_cache_t::iterator> cache_index_t;
	class_cache_t theCache;
	cache_index_t theCacheIndex;
	Mutex cacheGuard;
	UInt32 maxCacheSize;
};
//////////////////////////////////////////////////////////////////////////////
template <typename T>
Cache<T>::Cache()
	: maxCacheSize(100)
{
}
//////////////////////////////////////////////////////////////////////////////
template <typename T>
void
Cache<T>::addToCache(const T& cc, const String& key)
{
	MutexLock l(cacheGuard);
	if (theCacheIndex.size() >= maxCacheSize)
	{
		if (!theCache.empty())
		{
			String key = theCache.begin()->second;
			theCache.pop_front();
			theCacheIndex.erase(key);
		}
	}
	typename class_cache_t::iterator i = theCache.insert(theCache.end(),
		typename class_cache_t::value_type(cc, key));
	theCacheIndex.insert(typename cache_index_t::value_type(key, i));
}
//////////////////////////////////////////////////////////////////////////////
template <typename T>
T
Cache<T>::getFromCache(const String& key)
{
	MutexLock l(cacheGuard);
	T cc(CIMNULL);
	// look up key in the index
	typename cache_index_t::iterator ii = theCacheIndex.find(key);
	if (ii != theCacheIndex.end())
	{
		// we've got it, now get the iterator
		typename class_cache_t::iterator i = ii->second;
		// get the class
		cc = i->first;
		// now move the class to the end of the list
		theCache.splice(theCache.end(),theCache,i);
		// because splice doesn't actually move the elements, we don't have to
		// update the iterator in theCacheIndex
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
template <typename T>
void
Cache<T>::removeFromCache(const String& key)
{
	MutexLock l(cacheGuard);
	typename cache_index_t::iterator i = theCacheIndex.find(key);
	if (i != theCacheIndex.end())
	{
		typename class_cache_t::iterator ci = i->second;
		theCacheIndex.erase(i);
		theCache.erase(ci);
	}
}
//////////////////////////////////////////////////////////////////////////////
template <typename T>
void
Cache<T>::clearCache()
{
	MutexLock l(cacheGuard);
	theCache.clear();
	theCacheIndex.clear();
}
//////////////////////////////////////////////////////////////////////////////
template <typename T>
void
Cache<T>::setMaxCacheSize(UInt32 max)
{
	MutexLock l(cacheGuard);
	maxCacheSize = max;
	if (max != 0)
	{
		while (theCacheIndex.size() >= maxCacheSize)
		{
			if (!theCache.empty())
			{
				String key = theCache.begin()->second;
				theCache.pop_front();
				theCacheIndex.erase(key);
			}
		}
	}
}

} // end namespace OpenWBEM

#endif
