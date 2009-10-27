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

/**
 * @author <see corresponding BloCxx header>
 */




#ifndef OW_SafeCString_HPP_INCLUDE_GUARD_
#define OW_SafeCString_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include <blocxx/SafeCString.hpp>

namespace OW_NAMESPACE
{
   using namespace blocxx;
}


	/*
	 * Use the OW_INTSTR macro to insert symbolic constants into format strings.
	 * A typical usage would be for a maximum field width when using the
	 * scanf family of functions.  For example, suppose that buf is an array of
	 * STRMAX + 1 characters, and STRMAX is a manifest constant.  Then one
	 * might write
	 *
	 *   sscanf(inpstr, "Name: %." OW_INTSTR(STRMAX) "s\n", buf);
	*/
#define OW_INTSTR(x) BLOCXX_INTSTR(x)

#warning OW_SafeCString.hpp is deprecated, use "blocxx/SafeCString.hpp" instead. \
Substitute all uses of OW_INTSTR(x) with BLOCXX_INTSTR(x)
#endif
