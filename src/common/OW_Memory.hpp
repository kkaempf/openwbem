/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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
#include "OW_Types.h"
#include <cstring>

template<class T>
inline void OW_zeros(T* items, OW_UInt32 size)
{
	::memset(items, 0, sizeof(T) * size);
}

template<class T>
inline void OW_destroy(T* items, OW_UInt32 size)
{
	while(size--)
	{
		items->~T();
		items++;
	}
}

inline void OW_destroy(bool* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_UInt8* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_Int8* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_UInt16* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_Int16* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_UInt32* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_Int32* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_UInt64* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_Int64* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_Real32* /*items*/, OW_UInt32 /*size*/) {  }
inline void OW_destroy(OW_Real64* /*items*/, OW_UInt32 /*size*/) {  }

#ifdef OW_NEW
#undef new
#endif

template<class T, class U>
inline void OW_copyToRaw(T* to, const U* from, OW_UInt32 size)
{
	while(size--)
	{
		new(to++) T(*from++);
	}
}

template<class T>
inline void OW_initializeRaw(T* items, OW_UInt32 size)
{
	while(size--)
	{
		new(items++) T();
	}
}

#ifdef OW_NEW
#define new OW_NEW
#endif

inline void OW_copyToRaw(bool* to, const bool* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(bool) * size);
}

inline void OW_copyToRaw(OW_UInt8* to, const OW_UInt8* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_UInt8) * size);
}

inline void OW_copyToRaw(OW_Int8* to, const OW_Int8* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_Int8) * size);
}

inline void OW_copyToRaw(OW_UInt16* to, const OW_UInt16* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_UInt16) * size);
}

inline void OW_copyToRaw(OW_Int16* to, const OW_Int16* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_Int16) * size);
}

inline void OW_copyToRaw(OW_UInt32* to, const OW_UInt32* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_UInt32) * size);
}

inline void OW_copyToRaw(OW_Int32* to, const OW_Int32* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_Int32) * size);
}

inline void OW_copyToRaw(OW_UInt64* to, const OW_UInt64* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_UInt64) * size);
}

inline void OW_copyToRaw(OW_Int64* to, const OW_Int64* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_Int64) * size);
}

inline void OW_copyToRaw(OW_Real32* to, const OW_Real32* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_Real32) * size);
}

inline void OW_copyToRaw(OW_Real64* to, const OW_Real64* from, OW_UInt32 size)
{
	::memcpy(to, from, sizeof(OW_Real64) * size);
}

inline void OW_initializeRaw(bool* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_UInt8* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_Int8* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_UInt16* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_Int16* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_UInt32* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_Int32* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_UInt64* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_Int64* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_Real32* items, OW_UInt32 size)
{
	OW_zeros(items, size);
}

inline void OW_initializeRaw(OW_Real64* items, OW_UInt32 size) 
{
	OW_zeros(items, size);
}

#endif

	
