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
 * @author Dan Nuffer
 */

#ifndef OW_PROVIDER_ENVIRONMENT_HPP_
#define OW_PROVIDER_ENVIRONMENT_HPP_
#include "OW_config.h"
#include "blocxx/IntrusiveCountableBase.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_CommonFwd.hpp"

#include "OW_Logger.hpp" // TODO: Remove this once everybody has been warned to use OW_CppProviderIncludes.hpp instead.
#include "OW_CIMOMHandleIFC.hpp" // TODO: Remove this once everybody has been warned to use OW_CppProviderIncludes.hpp instead.

namespace OW_NAMESPACE
{

/**
 * Note that some derived classes do not carry their own OperationContext,
 * instead carrying only a reference to one.  In such a case, the
 * ProviderEnvironmentIFC object is valid only for the lifetime of the
 * referenced OperationContext.
 */
class OW_PROVIDER_API ProviderEnvironmentIFC : public IntrusiveCountableBase
{
public:
	virtual ~ProviderEnvironmentIFC();

	/** 
	 * This function returns a regular cimom handle that does access
	 * checking and may call providers.  WARNING: The return value
	 * is valid only as long as this object is valid (see class note).
	 * Each operation done on the cimom handle does appropriate locking.
	 */
	virtual CIMOMHandleIFCRef getCIMOMHandle() const = 0;

	/** 
	 * This function returns a cimom handle that directly accesses
	 * the repository (CIMServer is bypassed).  No providers will be
	 * called.  This function should only be called if getCIMOMHandle()
	 * is insufficent.  WARNING: The return value is valid only as long
	 * as this object is valid (see class note).
	 * Each operation done on the cimom handle does appropriate locking.
	 */
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const = 0;

	/** 
	 * This function returns a reference to the repository.  This
	 * function should only be called if getCIMOMHandle() and
	 * getRepositoryCIMOMHandle() are insufficient.
	 * WARNING: The return value is valid only as long as this object
	 * is valid (see class note).
	 */
	virtual RepositoryIFCRef getRepository() const = 0;

	/** 
	* Like getRepository(), but all operations get checked to see if the
	* user is authorized to do the operation.
	*/
	virtual RepositoryIFCRef getAuthorizingRepository() const = 0;

	enum EInitialLockFlag
	{
		E_READ,
		E_WRITE
	};
	/** 
	 * This function returns a regular cimom handle that does access
	 * checking and may call providers.  WARNING: The return value
	 * is valid only as long as this object is valid (see class note).
	 * Each operation done on the cimom handle does appropriate locking.
	 * In addition, the read/write lock is acquired before 
	 * getLockedCIMOMHandle() returns. initialLock specifies whether the
	 * read or write lock is acquired. The lock is released by the
	 * destructor of the returned CIMOMHandleIFC, thus the caller should
	 * take care to ensure that the lock is not held for too long.
	 * 
	 * @throws TimeoutException If the lock can't be acquired in time.
	 * @throws ProviderEnvironmentException If this function is not
	 *         implemented.
	 */
	virtual CIMOMHandleIFCRef getLockedCIMOMHandle(EInitialLockFlag initialLock) const;

	virtual LoggerRef getLogger() const OW_DEPRECATED; // in 3.1.0
	virtual LoggerRef getLogger(const String& componentName) const;
	virtual String getConfigItem(const String &name, const String& defRetVal="") const = 0;
	virtual StringArray getMultiConfigItem(const String &itemName, 
		const StringArray& defRetVal, const char* tokenizeSeparator = 0) const = 0;

	virtual String getUserName() const = 0;
	virtual OperationContext& getOperationContext() = 0;

	/**
	 * When a provider is passed a ProviderEnvironmentIFCRef, the particular instance is only valid for the lifetime of the particular
	 * call.  If a provider needs to save a ProviderEnvironmentIFCRef for later use (e.g. in another thread), it must call clone() on
	 * the original ProviderEnvironmentIFC and save the result for later use.
	 */
	virtual ProviderEnvironmentIFCRef clone() const = 0;
};
									

} // end namespace OW_NAMESPACE

#endif									
