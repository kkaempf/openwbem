/*******************************************************************************
* Copyright (C) 2001, 2002 Vintela, Inc. All rights reserved.
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
#ifndef OW_WQLIFC_HPP_
#define OW_WQLIFC_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_CIMOMHandleIFC.hpp"

namespace OpenWBEM
{

// this class is part of the wqlcommon library, which is not in libopenwbem
class WQLSelectStatement;
class WQLIFC
{
public:
	virtual ~WQLIFC()
	{
	}
	virtual void evaluate(const String& nameSpace,
		CIMInstanceResultHandlerIFC& result,
		const String& query, const String& queryLanguage,
				Reference<CIMOMHandleIFC> hdl) = 0;
	virtual WQLSelectStatement createSelectStatement(const String& query) = 0;
	virtual bool supportsQueryLanguage(const String& lang) = 0;
};
typedef SharedLibraryReference< Reference<WQLIFC> > WQLIFCRef;

#if !defined(OW_STATIC_SERVICES)
#define OW_WQLFACTORY(derived,wqlname) \
extern "C" const char* \
getOWVersion() \
{ \
	return OW_VERSION; \
} \
extern "C" OpenWBEM::WQLIFC* \
createWQL() \
{ \
	return new derived; \
}
#else
#define OW_WQLFACTORY(derived,wqlname) \
extern "C" OpenWBEM::WQLIFC* \
createWQL_##wqlname() \
{ \
	return new derived; \
}
#endif /* !defined(OW_STATIC_SERVICES) */

} // end namespace OpenWBEM

typedef OpenWBEM::WQLIFC OW_WQLIFC OW_DEPRECATED;

#endif
