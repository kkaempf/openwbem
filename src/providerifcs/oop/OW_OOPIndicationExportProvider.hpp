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

#ifndef OW_OOP_INDICATION_EXPORT_PROVIDER_HPP_INCLUDE_GUARD_
#define OW_OOP_INDICATION_EXPORT_PROVIDER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_OOPProviderInterface.hpp"
#include "OW_OOPFwd.hpp"
#include "OW_OOPProviderBase.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OOPIndicationExportProvider : public IndicationExportProviderIFC, public OOPProviderBase
{
public:
	OOPIndicationExportProvider(const OOPProviderInterface::ProvRegInfo& info,
		const Reference<RWLocker>& guardRef,
		const Reference<ProcessRef>& persistentProcessRef,
		const Reference<String>& persistentProcessUserNameRef
		);
	virtual ~OOPIndicationExportProvider();
	
	virtual StringArray getHandlerClassNames();
	
	virtual void exportIndication(const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& indHandlerInst, const CIMInstance& indicationInst);
	virtual void doShutdown();
	virtual void doCooperativeCancel();
	virtual void doDefinitiveCancel();
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);

};


} // end namespace OW_NAMESPACE


#endif






