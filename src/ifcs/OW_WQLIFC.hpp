/*******************************************************************************
* Copyright (C) 2001, 2002 Center 7, Inc All rights reserved.
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

#ifndef OW_WQLIFC_HPP_
#define OW_WQLIFC_HPP_

#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_CIMOMHandleIFC.hpp"

// this class is part of the wqlcommon library, which is not in libopenwbem
class OW_WQLSelectStatement;

class OW_WQLIFC
{
public:
	virtual ~OW_WQLIFC()
	{
	}

	virtual void evaluate(const OW_String& nameSpace,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& query, const OW_String& queryLanguage,
                OW_Reference<OW_CIMOMHandleIFC> hdl) = 0;

	virtual OW_WQLSelectStatement createSelectStatement(const OW_String& query) = 0;

	virtual bool supportsQueryLanguage(const OW_String& lang) = 0;

};

typedef OW_SharedLibraryReference<OW_WQLIFC> OW_WQLIFCRef;
		
#define OW_WQLFACTORY(derived) \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
} \
extern "C" OW_WQLIFC* \
createWQL() \
{ \
	return new derived; \
}


#endif
