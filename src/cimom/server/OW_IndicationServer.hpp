/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
#ifndef OW_INDICATION_SERVER_HPP_
#define OW_INDICATION_SERVER_HPP_
#include "OW_config.h"
#include "OW_CIMFwd.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_Thread.hpp"
#include "OW_CIMOMEnvironment.hpp"

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
class IndicationServer : public Thread
{
public:
	IndicationServer()
		: Thread()
	{}
	virtual ~IndicationServer();
	virtual void init(CIMOMEnvironmentRef env) = 0;
	virtual void shutdown() = 0;
	virtual void processIndication(const CIMInstance& instance,
		const String& instNS) = 0;
	virtual void waitUntilReady() = 0;
	// these functions are call by the CIM_IndicationSubscription pass-thru provider.
	virtual void startDeleteSubscription(const String& ns, const CIMObjectPath& subPath) = 0;
	virtual void startCreateSubscription(const String& ns, const CIMInstance& subInst, const String& username) = 0;
	virtual void startModifySubscription(const String& ns, const CIMInstance& subInst) = 0;
	virtual void modifyFilter(const String& ns, const CIMInstance& filterInst) = 0;
protected:
};
typedef SharedLibraryReference< Reference<IndicationServer> > IndicationServerRef;

} // end namespace OpenWBEM

#endif
