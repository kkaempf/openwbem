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

#ifndef OW_CIMOMLOCATOR_HPP_INCLUDE_GUARD_
#define OW_CIMOMLOCATOR_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CIMOMInfo.hpp"
#include "OW_Reference.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include <utility>

DECLARE_EXCEPTION(CIMOMLocator);

typedef std::pair<OW_String, OW_String> OW_CIMOMAttribute;
typedef OW_Array<OW_CIMOMAttribute> OW_CIMOMAttributeArray;

class OW_CIMOMLocator;
typedef OW_Reference<OW_CIMOMLocator> OW_CIMOMLocatorRef;

class OW_CIMOMLocator
{
public:

	virtual ~OW_CIMOMLocator() {}

	/**
	 * Find available CIMOMs.  
	 * @return An Array or OW_CIMOMInfo objects.
	 */
	virtual OW_CIMOMInfoArray findCIMOMs() = 0;

	/**
	 * Factory Function to create a CIMOMLocator.
	 * @param locatorType The type of CIMOMLocator to create.  Valid values are:
	 * 							"slp".
	 * @returns An OW_Reference<OW_CIMOMLocator>.
	 */
	static OW_CIMOMLocatorRef createCIMOMLocator(const OW_String& locatorType);
};


#endif

