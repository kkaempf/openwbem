/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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
#include "OW_Semaphore.hpp"

//////////////////////////////////////////////////////////////////////////////
class OW_IndicationServer : public OW_Thread
{
public:

	OW_IndicationServer()
		: OW_Thread(true)
	{}

	virtual ~OW_IndicationServer() {}

	virtual void init(OW_CIMOMEnvironmentRef env) = 0;

	virtual void shutdown() = 0;

	virtual void processIndication(const OW_CIMInstance& instance,
		const OW_String& instNS) = 0;

	virtual void setStartedSemaphore(OW_Semaphore* sem) = 0;

	// these functions are call by the CIM_IndicationSubscription pass-thru provider.
	virtual void deleteSubscription(const OW_String& ns, const OW_CIMObjectPath& subPath) = 0;
	virtual void createSubscription(const OW_String& ns, const OW_CIMInstance& subInst) = 0;
	virtual void modifySubscription(const OW_String& ns, const OW_CIMInstance& subInst) = 0;
protected:
};

typedef OW_SharedLibraryReference<OW_IndicationServer> OW_IndicationServerRef;

#endif


