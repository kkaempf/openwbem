/*******************************************************************************
* Copyright (C) 2001-3 Vintela, Inc. All rights reserved.
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
#ifndef OW_ASSERTION_HPP_INCLUDE_GUARD_
#define OW_ASSERTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(Assertion);

// for backward compatibility
typedef AssertionException Assertion OW_DEPRECATED;

/**
 * OW_ASSERT works similar to the assert() macro.  But instead of calling 
 * abort(), instead it throws an AssertionException.
 * @param CON The condition which will be evaluated.
 */
#ifdef OW_DEBUG
#define OW_ASSERT(CON) if(!(CON)) throw AssertionException(__FILE__, __LINE__, \
		#CON)
#else
#define OW_ASSERT(CON)
#endif

/**
 * OW_ASSERTMSG works the same as OW_ASSERT, but with a second string parameter
 * that will be added to the exception message on failure.
 * @param CON The condition which will be evaluated.
 * @param MSG The extra message.  A C string literal.
 */
#ifdef OW_DEBUG
// MSG should be a string
#define OW_ASSERTMSG(CON, MSG) if(!(CON)) throw AssertionException(__FILE__, __LINE__, \
		#CON ":" MSG)
#else
#define OW_ASSERTMSG(CON, MSG)
#endif

} // end namespace OpenWBEM

#endif
