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

#ifndef OW_HASH_MAP_HPP_INCLUDE_GUARD_
#define OW_HASH_MAP_HPP_INCLUDE_GUARD_

#include "OW_config.h"

#ifdef OW_HAVE_HASH_MAP
	#include <hash_map> // hash_map is better for the cache than OW_SortedVectorMap
	#define OW_HASH_MAP_NS std
	#define OW_HashMap std::hash_map
#elif OW_HAVE_EXT_HASH_MAP
	#include <ext/hash_map> // hash_map is better for the cache than OW_SortedVectorMap
	#define OW_HASH_MAP_NS __gnu_cxx
	#define OW_HashMap __gnu_cxx::hash_map
#else
	#include "OW_SortedVectorMap.hpp"
	#define OW_HASH_MAP_NS
	#define OW_HashMap OW_SortedVectorMap
#endif

#ifndef OW_HASH_SPECIALIZED_INCLUDE_GUARD_
#define OW_HASH_SPECIALIZED_INCLUDE_GUARD_
#if defined(OW_HAVE_HASH_MAP) || defined(OW_HAVE_EXT_HASH_MAP)
#include "OW_String.hpp"
// need to specialize hash
namespace OW_HASH_MAP_NS
{
template<> struct hash<OW_String>
{
	size_t operator()(const OW_String& s) const
	{
		return hash<const char*>()(s.c_str());
	}
};
}
#endif
#endif

#undef OW_HASH_MAP_NS

#endif


