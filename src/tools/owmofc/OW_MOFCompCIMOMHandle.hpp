/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc. All rights reserved.
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
#ifndef OW_MOFCOMPCIMOMHANDLE_HPP_INCLUDE_GUARD_
#define OW_MOFCOMPCIMOMHANDLE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_RepositoryCIMOMHandle.hpp"

namespace OpenWBEM
{

/**
 * The MOFCompCIMOMHandle class is a derivitive of the CIMOMHandleIFC that
 * is used by all components that need access to CIM Services.
 */
class MOFCompCIMOMHandle : public RepositoryCIMOMHandle
{
public:
	/**
	 * Create a new MOFCompCIMOMHandle with a given repository interface
	 * and user access control information.
	 * @param env A reference to an CIMOMEnvironment object.
	 * @param pRepos A reference to an Repository that will be used by this
	 *		MOFCompCIMOMHandle.
	 * @param context The operation context that will be associated with
	 *		this MOFCompCIMOMHandle.
	 */
	MOFCompCIMOMHandle(RepositoryIFCRef pRepos,
		OperationContext& context);
};

typedef Reference<MOFCompCIMOMHandle> MOFCompCIMOMHandleRef;

} // end namespace OpenWBEM

#endif
