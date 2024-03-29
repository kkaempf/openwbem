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
 * @author Dan Nuffer
 */

#ifndef OW_INDICATION_SERVER_HPP_INCLUDE_GUARD_
#define OW_INDICATION_SERVER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "blocxx/SharedLibraryReference.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_CimomCommonFwd.hpp"

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
class OW_CIMOMCOMMON_API IndicationServer : public ServiceIFC
{
public:
	IndicationServer();
	virtual ~IndicationServer();

	virtual void processIndication(const CIMInstance& instance,
		const blocxx::String& instNS) = 0;

	// these functions are called by the CIM_IndicationSubscription pass-thru provider.
	virtual void startDeleteSubscription(const blocxx::String& ns, const CIMObjectPath& subPath) = 0;
	virtual void startCreateSubscription(const blocxx::String& ns, const CIMInstance& subInst, const blocxx::String& username) = 0;
	virtual void startModifySubscription(const blocxx::String& ns, const CIMInstance& subInst) = 0;
	virtual void modifyFilter(OperationContext& context, const blocxx::String& ns, const CIMInstance& filterInst, const blocxx::String& userName) = 0;
};

} // end namespace OW_NAMESPACE

#endif
