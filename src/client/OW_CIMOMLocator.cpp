/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMOMLocator.hpp"
#include "OW_CIMOMLocatorSLP.hpp"
#include "OW_Exception.hpp"
#include "OW_ExceptionIds.hpp"
#include "blocxx/String.hpp"

namespace OW_NAMESPACE
{
using namespace blocxx;

OW_DEFINE_EXCEPTION_WITH_ID(CIMOMLocator);
//////////////////////////////////////////////////////////////////////////////
CIMOMLocator::~CIMOMLocator()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMOMLocatorRef
CIMOMLocator::createCIMOMLocator(const String& locatorType)
{
	if (locatorType.equalsIgnoreCase("slp"))
	{
#ifdef OW_HAVE_SLP_H
		return CIMOMLocatorRef(new CIMOMLocatorSLP());
#else
		OW_THROW(CIMOMLocatorException, "Requested SLP CIMOM Locator, but "
			"not linked with libslp!");
#endif
	}
	else
	{
		OW_THROW(CIMOMLocatorException, "Unknown CIMOMLocator type");
	}
	return CIMOMLocatorRef();
}

} // end namespace OW_NAMESPACE

