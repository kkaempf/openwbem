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

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_SERVICE_ENVIRONMENTIFC_HPP_INCLUDE_GUARD_
#define OW_SERVICE_ENVIRONMENTIFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_SelectableIFC.hpp"
#include "OW_SelectableCallbackIFC.hpp"
#include "OW_SharedLibraryReference.hpp"
#include "OW_ArrayFwd.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_CIMFwd.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

/**
 * This is an internal interface which is used by the various OpenWBEM services to interface with their "environment"
 */
class OW_COMMON_API ServiceEnvironmentIFC : public IntrusiveCountableBase
{
public:
	virtual ~ServiceEnvironmentIFC();
	virtual String getConfigItem(const String& name, const String& defRetVal="") const = 0;
	enum EOverwritePreviousFlag
	{
		E_PRESERVE_PREVIOUS,
		E_OVERWRITE_PREVIOUS
	};
	
	virtual void setConfigItem(const String& item,
		const String& value, EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS) = 0;
	virtual void addSelectable(const SelectableIFCRef& obj,
		const SelectableCallbackIFCRef& cb) = 0;
	virtual void removeSelectable(const SelectableIFCRef& obj) = 0;
	virtual RequestHandlerIFCRef getRequestHandler(
		const String& id) = 0;
	virtual LoggerRef getLogger() const OW_DEPRECATED = 0; // in 3.1.0
	virtual LoggerRef getLogger(const String& componentName) const = 0;
	virtual bool authenticate(String& userName,
		const String& info, String& details, OperationContext& context) = 0;
	enum ESendIndicationsFlag
	{
		E_DONT_SEND_INDICATIONS,
		E_SEND_INDICATIONS
	};
	enum EBypassProvidersFlag
	{
		E_USE_PROVIDERS,
		E_BYPASS_PROVIDERS
	};
	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext& context,
		ESendIndicationsFlag doIndications = E_SEND_INDICATIONS,
		EBypassProvidersFlag bypassProviders = E_USE_PROVIDERS) = 0;

	virtual CIMInstanceArray getInteropInstances(const String& className) const = 0;
	virtual void setInteropInstance(const CIMInstance& inst) = 0;
};

} // end namespace OpenWBEM

#endif
