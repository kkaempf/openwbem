/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RepositoryIFC.hpp"
#include "OW_CIMInstance.hpp"
#include "blocxx/Array.hpp"
#include "OW_Logger.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_WQLIFC.hpp"

namespace OW_NAMESPACE
{

///////////////////////////////////////////////////////////////////////////////
ServiceEnvironmentIFC::~ServiceEnvironmentIFC()
{
}

///////////////////////////////////////////////////////////////////////////////
RepositoryIFCRef
ServiceEnvironmentIFC::getRepository() const
{
	OW_ASSERTMSG(0, "getRepository Not Implemented");
	return RepositoryIFCRef();
}

///////////////////////////////////////////////////////////////////////////////
RepositoryIFCRef
ServiceEnvironmentIFC::getAuthorizingRepository() const
{
	OW_ASSERTMSG(0, "getAuthorizingRepository Not Implemented");
	return RepositoryIFCRef();
}

///////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
ServiceEnvironmentIFC::getRepositoryCIMOMHandle(OperationContext& context) const
{
	return getCIMOMHandle(context, E_BYPASS_PROVIDERS);
}

///////////////////////////////////////////////////////////////////////////////
void
ServiceEnvironmentIFC::addSelectable(const SelectableIFCRef& obj, const SelectableCallbackIFCRef& cb)
{
	OW_ASSERTMSG(0, "addSelectable Not Implemented");
}

///////////////////////////////////////////////////////////////////////////////
void
ServiceEnvironmentIFC::removeSelectable(const SelectableIFCRef& obj)
{
	OW_ASSERTMSG(0, "removeSelectable Not Implemented");
}

///////////////////////////////////////////////////////////////////////////////
String
ServiceEnvironmentIFC::getConfigItem(const String& name, const String& defRetVal) const
{
	return defRetVal;
}

///////////////////////////////////////////////////////////////////////////////
StringArray
ServiceEnvironmentIFC::getMultiConfigItem(const String &itemName, 
	const StringArray& defRetVal, const char* tokenizeSeparator) const
{
	return defRetVal;
}
///////////////////////////////////////////////////////////////////////////////
void
ServiceEnvironmentIFC::setConfigItem(const String& item, const String& value, EOverwritePreviousFlag overwritePrevious)
{
	OW_ASSERTMSG(0, "setConfigItem Not Implemented");
}

///////////////////////////////////////////////////////////////////////////////
RequestHandlerIFCRef
ServiceEnvironmentIFC::getRequestHandler(const String& id) const
{
	OW_ASSERTMSG(0, "getRequestHandler Not Implemented");
	return RequestHandlerIFCRef();
}

///////////////////////////////////////////////////////////////////////////////
bool
ServiceEnvironmentIFC::authenticate(String& userName, const String& info, String& details, OperationContext& context) const
{
	OW_ASSERTMSG(0, "authenticate Not Implemented");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
ServiceEnvironmentIFC::getCIMOMHandle(OperationContext& context,
	EBypassProvidersFlag bypassProviders) const
{
	OW_ASSERTMSG(0, "getCIMOMHandle Not Implemented");
	return CIMOMHandleIFCRef();
}

///////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFCRef
ServiceEnvironmentIFC::getLockedCIMOMHandle(OperationContext& context,
	EInitialLockFlag initialLock) const
{
	OW_ASSERTMSG(0, "getLockedCIMOMHandle Not Implemented");
	return CIMOMHandleIFCRef();
}


///////////////////////////////////////////////////////////////////////////////
WQLIFCRef
ServiceEnvironmentIFC::getWQLRef() const
{
	OW_ASSERTMSG(0, "getWQLRef Not Implemented");
	return WQLIFCRef();
}

} // end namespace OW_NAMESPACE

