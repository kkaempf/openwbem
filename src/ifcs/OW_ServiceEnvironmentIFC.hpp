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

#ifndef OW_SERVICE_ENVIRONMENTIFC_HPP_INCLUDE_GUARD_
#define OW_SERVICE_ENVIRONMENTIFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/IntrusiveCountableBase.hpp"
#include "blocxx/ArrayFwd.hpp"
#include "blocxx/String.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_CIMFwd.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

/**
 * This is an internal interface which is used by the various OpenWBEM services to interface with their "environment"
 */
class OW_COMMON_API ServiceEnvironmentIFC : virtual public blocxx::IntrusiveCountableBase
{
public:
	virtual ~ServiceEnvironmentIFC();

	virtual blocxx::String getConfigItem(const blocxx::String& name, const blocxx::String& defRetVal="") const;

	/**
	 * Retrieve itemName values from configItems. If it's not present, defRetVal will be returned.
	 * @param tokenizeSeparator If non-null, then each item will be tokenized using the specified separator chars and returned as separate items.
	 */
	virtual blocxx::StringArray getMultiConfigItem(const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal, const char* tokenizeSeparator = 0) const;

	enum EOverwritePreviousFlag
	{
		E_PRESERVE_PREVIOUS,
		E_OVERWRITE_PREVIOUS
	};

	virtual void setConfigItem(const blocxx::String& item, const blocxx::String& value, EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS);

	virtual void addSelectable(const blocxx::SelectableIFCRef& obj, const blocxx::SelectableCallbackIFCRef& cb);
	virtual void removeSelectable(const blocxx::SelectableIFCRef& obj);

	virtual RequestHandlerIFCRef getRequestHandler(const blocxx::String& id) const;

	virtual bool authenticate(blocxx::String& userName, const blocxx::String& info, blocxx::String& details, OperationContext& context) const;

	enum EBypassProvidersFlag
	{
		E_USE_PROVIDERS,
		E_BYPASS_PROVIDERS
	};
	/**
	 * WARNING: The return value is valid only for the lifetime of context.
	 */
	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext& context,
		EBypassProvidersFlag bypassProviders = E_USE_PROVIDERS) const;

	enum EInitialLockFlag
	{
		E_NONE,
		E_READ,
		E_WRITE
	};
	/**
	 * WARNING: The return value is valid only for the lifetime of context.
	 */
	virtual CIMOMHandleIFCRef getLockedCIMOMHandle(OperationContext& context,
		EInitialLockFlag initialLock) const;

	CIMOMHandleIFCRef getRepositoryCIMOMHandle(OperationContext& context) const;

	virtual RepositoryIFCRef getRepository() const;

	virtual RepositoryIFCRef getAuthorizingRepository() const;

	virtual WQLIFCRef getWQLRef() const;
};

} // end namespace OW_NAMESPACE

#endif
