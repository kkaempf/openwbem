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
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_CommonFwd.hpp"

#include "OW_Logger.hpp" // TODO: Remove this once everybody has been warned to use OW_CppProviderIncludes.hpp instead.
#include "OW_CIMOMHandleIFC.hpp" // TODO: Remove this once everybody has been warned to use OW_CppProviderIncludes.hpp instead.

namespace OpenWBEM
{

/**
 * Note that some derived classes do not carry their own OperationContext,
 * instead carrying only a reference to one.  In such a case, the
 * ProviderEnvironmentIFC object is valid only for the lifetime of the
 * referenced OperationContext.
 */
class ProviderEnvironmentIFC : public IntrusiveCountableBase
{
public:
	virtual ~ProviderEnvironmentIFC();

	/** This function returns a regular cimom handle that does access
	 * checking and may call providers.  WARNING: The return value
	 * is valid only as long as this object is valid (see class note).
	 */
	virtual CIMOMHandleIFCRef getCIMOMHandle() const = 0;

	/** This function returns a cimom handle that directly accesses
	 * the repository (CIMServer is bypassed).  No providers will be
	 * called.  This function should only be called if getCIMOMHandle()
	 * is insufficent.  WARNING: The return value is valid only as long
	 * as this object is valid (see class note).
	 */
	virtual CIMOMHandleIFCRef getRepositoryCIMOMHandle() const = 0;

	/** This function returns a reference to the repository.  This
	 * function should only be called if getCIMOMHandle() and
	 * getRepositoryCIMOMHandle() are insufficient.
	 * WARNING: The return value is valid only as long as this object
	 * is valid (see class note).
	 */
	virtual RepositoryIFCRef getRepository() const = 0;

	virtual LoggerRef getLogger() const OW_DEPRECATED = 0; // in 3.1.0
	virtual LoggerRef getLogger(const String& componentName) const = 0;
	virtual String getConfigItem(const String &name, const String& defRetVal="") const = 0;
	virtual String getUserName() const = 0;
	virtual OperationContext& getOperationContext() = 0;
};
									

} // end namespace OpenWBEM

#endif									
