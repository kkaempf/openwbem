/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#ifndef OW_CACHE_INCLUDE_GUARD_HPP_
#define OW_CACHE_INCLUDE_GUARD_HPP_

#include "OW_config.h"

#include "OW_HashMap.hpp"
#include "OW_String.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include <list>

/** This class encapsulates the functionality of a cache. */
template <typename T>
class OW_Cache
{
public:
    OW_Cache();

    /** Add an item to the cache.
     * @param cc The item to add
     * @param key The key for the item
     * @precondition cc is not already in the cache.  Adding duplicate items into the cache will waste space.
     */
    void addToCache(const T& cc, const OW_String& key);

    /** Get an item from the cache.  Average complexity is constant time. Worst case is linear in the size of the cache.
     * @param key The key for the item to retrieve.
     * @return The item if found, else the item constructed with OW_CIMNULL parameter.
     */
    T getFromCache(const OW_String& key);

    /** Remove an item from the cache.  Average complexity is constant time. Worst case is linear in the size of the cache.
     * It does not matter if the item is not in the cache.
     * @param key The key for the item to remove.
     */
    void removeFromCache(const OW_String& key);

    /** Remove all items from the cache.
     */
    void clearCache();

    /** Set the maximum number of items the cache will hold.
     */
    void setMaxCacheSize(OW_UInt32);

private:
    // a list of items that are cached.  The list is sorted by lru.  The least
    // recently acessed item will be at begin(), and the most recenly acessed
    // class will be at end()--;  The second part of the pair is the key that
    // will be used in the index.
    typedef std::list<std::pair<T, OW_String> > class_cache_t;
    // the index into the cache.  Speeds up finding an item when we need to.
    typedef OW_HashMap<OW_String, typename class_cache_t::iterator> cache_index_t;

    class_cache_t theCache;
    cache_index_t theCacheIndex;
    OW_Mutex cacheGuard;
    OW_UInt32 maxCacheSize;

};


//////////////////////////////////////////////////////////////////////////////
template <typename T>
OW_Cache<T>::OW_Cache()
    : maxCacheSize(100)
{
}

//////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Cache<T>::addToCache(const T& cc, const OW_String& key)
{
	OW_MutexLock l(cacheGuard);

	if(theCacheIndex.size() >= maxCacheSize)
	{
		if (!theCache.empty())
		{
			OW_String key = theCache.begin()->second;
			theCache.pop_front();
			theCacheIndex.erase(key);
		}
	}

	typename class_cache_t::iterator i = theCache.insert(theCache.end(),
		typename class_cache_t::value_type(cc, key));
	theCacheIndex.insert(cache_index_t::value_type(key, i));
}

//////////////////////////////////////////////////////////////////////////////
template <typename T>
T
OW_Cache<T>::getFromCache(const OW_String& key)
{
	OW_MutexLock l(cacheGuard);
	T cc(OW_CIMNULL);
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
OW_Cache<T>::removeFromCache(const OW_String& key)
{
	OW_MutexLock l(cacheGuard);
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
OW_Cache<T>::clearCache()
{
	OW_MutexLock l(cacheGuard);
	theCache.clear();
	theCacheIndex.clear();
}

//////////////////////////////////////////////////////////////////////////////
template <typename T>
void
OW_Cache<T>::setMaxCacheSize(OW_UInt32 max)
{
	OW_MutexLock l(cacheGuard);
    maxCacheSize = max;

	while(theCacheIndex.size() >= maxCacheSize)
	{
		if (!theCache.empty())
		{
			OW_String key = theCache.begin()->second;
			theCache.pop_front();
			theCacheIndex.erase(key);
		}
	}

}




#endif

