/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of
*       Quest Software, Inc.,
*       nor Novell, Inc.,
*       nor the names of its contributors or employees may be used to
*       endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2000 Andrei Alexandrescu
// Copyright (c) 2000 Petru Marginean
// Copyright (c) 2005 Joshua Lehrer
//
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////


#ifndef OW_ScopeGuard_HPP_INCLUDE_GUARD_
#define OW_ScopeGuard_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include <blocxx/ScopeGuard.hpp>

namespace OW_NAMESPACE
{
   using namespace blocxx;
}

#warning OW_ScopeGuard.hpp is deprecated, use "blocxx/ScopeGuard.hpp" instead. \
All uses of: \
	OW_CONCATENATE_DIRECT(s1, s2) should be replaced by BLOCXX_CONCATENATE_DIRECT(s1, s2) \
	OW_CONCATENATE(s1, s2)        should be replaced by BLOCXX_CONCATENATE(s1, s2) \
	OW_ANONYMOUS_VARIABLE(str)    should be replaced by BLOCXX_ANONYMOUS_VARIABLE(str) \
	OW_ON_BLOCK_EXIT should be replaced by BLOCXX_ON_BLOCK_EXIT				\
	OW_ON_BLOCK_EXIT_OBJ should be replaced by BLOCXX_ON_BLOCK_EXIT_OBJ

#define OW_CONCATENATE_DIRECT(s1, s2)  BLOCXX_CONCATENATE_DIRECT(s1, s2)
#define OW_CONCATENATE(s1, s2)         BLOCXX_CONCATENATE(s1, s2)
#define OW_ANONYMOUS_VARIABLE(str) BLOCXX_ANONYMOUS_VARIABLE(str)

#define OW_ON_BLOCK_EXIT BLOCXX_ON_BLOCK_EXIT
#define OW_ON_BLOCK_EXIT_OBJ BLOCXX_ON_BLOCK_EXIT_OBJ

#endif



