/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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

#ifndef OWBI1_OW_FWD_HPP_INCLUDE_GUARD_
#define OWBI1_OW_FWD_HPP_INCLUDE_GUARD_

#include "OWBI1_config.h"

// Need this first macro because ## doesn't expand vars, and we need an intermediate step
#define OWBI1_NAMESPACE_CAT(ow, ver) ow ## ver 
#define OWBI1_NAMESPACE_AUX(ver) OWBI1_NAMESPACE_CAT(OpenWBEM, ver) 
#define OW_NAMESPACE OWBI1_NAMESPACE_AUX(OWBI1_OPENWBEM_LIBRARY_VERSION) 

// need this to set up an alias.
namespace OW_NAMESPACE
{
}

namespace OpenWBEM = OW_NAMESPACE;


namespace OW_NAMESPACE
{
	class TempFileStream;
}

#undef OW_NAMESPACE
#undef OWBI1_NAMESPACE_AUX
#undef OWBI1_NAMESPACE_CAT

#endif

