/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_PROVIDERAGENTENVIRONMENT_HPP_INCLUDE_GUARD_
#define OW_PROVIDERAGENTENVIRONMENT_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_AuthenticatorIFC.hpp"

#include "OW_CppProviderBaseIFC.hpp"

namespace OpenWBEM
{

typedef std::pair<SelectableIFCRef, SelectableCallbackIFCRef> SelectablePair_t;

class ProviderAgentEnvironment : public ServiceEnvironmentIFC
{
public:
	ProviderAgentEnvironment(ConfigFile::ConfigMap configMap,
		Reference<CppProviderBaseIFC> provider, 
		Reference<AuthenticatorIFC> authenticator,
		Array<RequestHandlerIFCRef> requestHandlers, 
		LoggerRef logger,
		Reference<Array<SelectablePair_t> > selectables); 
	virtual ~ProviderAgentEnvironment(); 
	virtual bool authenticate(String &userName,
		const String &info, String &details, OperationContext& context); 
	virtual void addSelectable(const SelectableIFCRef& obj,
		const SelectableCallbackIFCRef& cb); 

	struct selectableFinder
	{
		selectableFinder(const SelectableIFCRef& obj) : m_obj(obj) {}
		template <typename T>
		bool operator()(const T& x)
		{
			return x.first == m_obj;
		}
		const SelectableIFCRef& m_obj;
	};

	virtual void removeSelectable(const SelectableIFCRef& obj); 
	virtual String getConfigItem(const String &name, const String& defRetVal="") const; 
	virtual void setConfigItem(const String& item, const String& value, EOverwritePreviousFlag overwritePrevious); 

	virtual CIMInstanceArray getInteropInstances(const String& className)const ; 
	virtual void setInteropInstance(const CIMInstance& inst); 
	
	virtual RequestHandlerIFCRef getRequestHandler(const String& ct); 
	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext&,
		ESendIndicationsFlag /*doIndications*/,
		EBypassProvidersFlag /*bypassProviders*/); 
	virtual LoggerRef getLogger() const; 
private:
	Map<String, String> m_configItems;
	Reference<AuthenticatorIFC> m_authenticator;
	LoggerRef m_logger;
	Array<RequestHandlerIFCRef> m_requestHandlers; 
	Reference<Array<SelectablePair_t> > m_selectables;
	Reference<CppProviderBaseIFC> m_prov; 
	class DummyLogger : public Logger
	{
	protected:
		virtual void doLogMessage(const String &, const ELogLevel) const
		{
			return;
		}
	};
};

} // end namespace OpenWBEM

#endif // #ifndef OW_PROVIDERAGENTENVIRONMENT_HPP_INCLUDE_GUARD_
