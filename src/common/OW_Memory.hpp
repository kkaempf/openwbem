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
////////////////////////////////////////////////////////////////////////////////
//
// Memory.h
//
//	This file contains assorted memory-oriented routines:
//
//	    OW_zeros(): fills memory with OW_zeros.
//	    OW_destroy(): destructs multiple objects in contiguous memory.
//	    OW_copyToRaw(): copies multiple objects to raw memory.
//	    OW_initializeRaw(): default constructs mulitple object over raw memory.
//
//	Each of these is a template but specializations are provide for
//	efficiency (which in some cases removes uncessary loops).
//
////////////////////////////////////////////////////////////////////////////////
#ifndef OW_MEMORY_HPP_INCLUDE_GUARD_
#define OW_MEMORY_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Types.hpp"
#include <cstring>

namespace OpenWBEM
{

template<class T>
inline void zeros(T* items, UInt32 size)
{
	::memset(items, 0, sizeof(T) * size);
}
template<class T>
inline void destroy(T* items, UInt32 size)
{
	while(size--)
	{
		items->~T();
		items++;
	}
}
inline void destroy(bool* /*items*/, UInt32 /*size*/) {  }
inline void destroy(UInt8* /*items*/, UInt32 /*size*/) {  }
inline void destroy(Int8* /*items*/, UInt32 /*size*/) {  }
inline void destroy(UInt16* /*items*/, UInt32 /*size*/) {  }
inline void destroy(Int16* /*items*/, UInt32 /*size*/) {  }
inline void destroy(UInt32* /*items*/, UInt32 /*size*/) {  }
inline void destroy(Int32* /*items*/, UInt32 /*size*/) {  }
inline void destroy(UInt64* /*items*/, UInt32 /*size*/) {  }
inline void destroy(Int64* /*items*/, UInt32 /*size*/) {  }
inline void destroy(Real32* /*items*/, UInt32 /*size*/) {  }
inline void destroy(Real64* /*items*/, UInt32 /*size*/) {  }
template<class T, class U>
inline void copyToRaw(T* to, const U* from, UInt32 size)
{
	while(size--)
	{
		new(to++) T(*from++);
	}
}
template<class T>
inline void initializeRaw(T* items, UInt32 size)
{
	while(size--)
	{
		new(items++) T();
	}
}
inline void copyToRaw(bool* to, const bool* from, UInt32 size)
{
	::memcpy(to, from, sizeof(bool) * size);
}
inline void copyToRaw(UInt8* to, const UInt8* from, UInt32 size)
{
	::memcpy(to, from, sizeof(UInt8) * size);
}
inline void copyToRaw(Int8* to, const Int8* from, UInt32 size)
{
	::memcpy(to, from, sizeof(Int8) * size);
}
inline void copyToRaw(UInt16* to, const UInt16* from, UInt32 size)
{
	::memcpy(to, from, sizeof(UInt16) * size);
}
inline void copyToRaw(Int16* to, const Int16* from, UInt32 size)
{
	::memcpy(to, from, sizeof(Int16) * size);
}
inline void copyToRaw(UInt32* to, const UInt32* from, UInt32 size)
{
	::memcpy(to, from, sizeof(UInt32) * size);
}
inline void copyToRaw(Int32* to, const Int32* from, UInt32 size)
{
	::memcpy(to, from, sizeof(Int32) * size);
}
inline void copyToRaw(UInt64* to, const UInt64* from, UInt32 size)
{
	::memcpy(to, from, sizeof(UInt64) * size);
}
inline void copyToRaw(Int64* to, const Int64* from, UInt32 size)
{
	::memcpy(to, from, sizeof(Int64) * size);
}
inline void copyToRaw(Real32* to, const Real32* from, UInt32 size)
{
	::memcpy(to, from, sizeof(Real32) * size);
}
inline void copyToRaw(Real64* to, const Real64* from, UInt32 size)
{
	::memcpy(to, from, sizeof(Real64) * size);
}
inline void initializeRaw(bool* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(UInt8* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(Int8* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(UInt16* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(Int16* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(UInt32* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(Int32* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(UInt64* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(Int64* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(Real32* items, UInt32 size)
{
	zeros(items, size);
}
inline void initializeRaw(Real64* items, UInt32 size) 
{
	zeros(items, size);
}

} // end namespace OpenWBEM

#endif
	
