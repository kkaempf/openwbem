/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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

#ifndef OW_PROVIDERAGENTENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_PROVIDERAGENTENVIRONMENT_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_AuthenticatorIFC.hpp"
#include "blocxx/Map.hpp"
#include "OW_ClientCIMOMHandleConnectionPool.hpp"
#include "OW_Cache.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "blocxx/Reference.hpp"
#include "OW_ProviderAgentLockerIFC.hpp"
#include "blocxx/Array.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

typedef std::pair<blocxx::SelectableIFCRef, blocxx::SelectableCallbackIFCRef> SelectablePair_t;

class ProviderAgentEnvironment : public ServiceEnvironmentIFC
{
public:
	enum EClassRetrievalFlag
	{
		E_DONT_RETRIEVE_CLASSES,
		E_RETRIEVE_CLASSES
	};

	enum EConnectionCredentialsUsageFlag
	{
		E_DONT_USE_CONNECTION_CREDENTIALS,
		E_USE_CONNECTION_CREDENTIALS
	};

	ProviderAgentEnvironment(const ConfigFile::ConfigMap& configMap,
		const blocxx::Array<CppProviderBaseIFCRef>& providers,
		const blocxx::Array<CIMClass>& cimClasses,
		const AuthenticatorIFCRef& authenticator,
		const blocxx::Array<RequestHandlerIFCRef>& requestHandlers,
		const blocxx::String& callbackURL,
		const blocxx::Reference<blocxx::Array<SelectablePair_t> >& selectables,
		const ProviderAgentLockerIFCRef& locker);
	virtual ~ProviderAgentEnvironment();
	virtual bool authenticate(blocxx::String &userName,
		const blocxx::String &info, blocxx::String &details, OperationContext& context) const;
	virtual void addSelectable(const blocxx::SelectableIFCRef& obj,
		const blocxx::SelectableCallbackIFCRef& cb);

	struct selectableFinder
	{
		selectableFinder(const blocxx::SelectableIFCRef& obj) : m_obj(obj) {}
		template <typename T>
		bool operator()(const T& x)
		{
			return x.first == m_obj;
		}
		const blocxx::SelectableIFCRef& m_obj;
	};

	virtual void removeSelectable(const blocxx::SelectableIFCRef& obj);
	virtual blocxx::String getConfigItem(const blocxx::String &name, const blocxx::String& defRetVal) const;
	virtual blocxx::StringArray getMultiConfigItem(const blocxx::String &itemName,
		const blocxx::StringArray& defRetVal, const char* tokenizeSeparator) const;
	virtual void setConfigItem(const blocxx::String& item, const blocxx::String& value, EOverwritePreviousFlag overwritePrevious);

	virtual RequestHandlerIFCRef getRequestHandler(const blocxx::String& ct) const;
	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext&,
		EBypassProvidersFlag bypassProviders) const;

private:
	ConfigFile::ConfigMap m_configItems;
	AuthenticatorIFCRef m_authenticator;
	blocxx::String m_callbackURL;
	blocxx::Array<RequestHandlerIFCRef> m_requestHandlers;
	blocxx::Reference<blocxx::Array<SelectablePair_t> > m_selectables;
	blocxx::Map<blocxx::String, CppProviderBaseIFCRef> m_assocProvs;
	blocxx::Map<blocxx::String, CppProviderBaseIFCRef> m_instProvs;
	blocxx::Map<blocxx::String, CppProviderBaseIFCRef> m_secondaryInstProvs;
	blocxx::Map<blocxx::String, CppProviderBaseIFCRef> m_methodProvs;
	/// @todo Refactor me.  ProviderAgentCIMOMHandles get a reference
	/// (&, not Reference) to m_cimClasses, and modify it.
	mutable Cache<CIMClass> m_cimClasses;
	ProviderAgentLockerIFCRef m_locker;
	EClassRetrievalFlag m_classRetrieval;
	mutable ClientCIMOMHandleConnectionPool m_connectionPool;
	ProviderAgentEnvironment::EConnectionCredentialsUsageFlag m_useConnectionCredentials;

};

} // end namespace OW_NAMESPACE

#endif // #ifndef OW_PROVIDERAGENTENVIRONMENT_HPP_INCLUDE_GUARD_
