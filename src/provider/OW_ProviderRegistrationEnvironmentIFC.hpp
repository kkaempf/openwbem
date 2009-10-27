/*******************************************************************************
* Copyright (C) 2005-2006 Novell, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_PROVIDER_REGISTRATION_ENVIRONMENT_HPP_
#define OW_PROVIDER_REGISTRATION_ENVIRONMENT_HPP_
#include "OW_config.h"
#include "blocxx/IntrusiveCountableBase.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_CommonFwd.hpp"

namespace OW_NAMESPACE
{

/**
 * The ProviderEnvironmentIFC object is valid only for the lifetime of the
 * referenced OperationContext.
 */
class OW_PROVIDER_API ProviderRegistrationEnvironmentIFC : public blocxx::IntrusiveCountableBase
{
public:
	virtual ~ProviderRegistrationEnvironmentIFC();

	/** This function returns a reference to the repository.
	 * WARNING: The return value is valid only as long as this object
	 * is valid (see class note).
	 */
	virtual RepositoryIFCRef getRepository() const = 0;

	virtual blocxx::LoggerRef getLogger(const blocxx::String& componentName) const = 0;
	virtual blocxx::String getConfigItem(const blocxx::String &name, const blocxx::String& defRetVal="") const = 0;
	virtual blocxx::StringArray getMultiConfigItem(const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal, const char* tokenizeSeparator = 0) const = 0;

	/**
	 * When a provider is passed a ProviderEnvironmentIFCRef, the particular instance is only valid for the lifetime of the particular
	 * call.  If a provider needs to save a ProviderEnvironmentIFCRef for later use (e.g. in another thread), it must call clone() on
	 * the original ProviderEnvironmentIFC and save the result for later use.
	 */
	virtual ProviderRegistrationEnvironmentIFCRef clone() const = 0;
};


} // end namespace OW_NAMESPACE

#endif	// OW_PROVIDER_REGISTRATION_ENVIRONMENT_HPP_
