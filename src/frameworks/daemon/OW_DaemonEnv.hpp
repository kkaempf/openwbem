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
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_DAEMON_ENV_HPP_
#define OW_DAEMON_ENV_HPP_
#include "OW_config.h"
#include "OW_Types.hpp"
#include "OW_LogLevel.hpp"
#include "OW_Reference.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_AuthManager.hpp"
#include "OW_RequestHandlerIFC.hpp"

namespace OpenWBEM
{

static const char* const DAEMON_NAME = "newdaemon";
class DaemonEnv : public ServiceEnvironmentIFC
{
	public:
		virtual CIMOMHandleIFCRef getCIMOMHandle(const String &username, ESendIndicationsFlag doIndications, EBypassProvidersFlag bypassProviders);
		virtual LoggerRef getLogger() const;
		virtual bool authenticate(String &userName, const String &info, String &details);
		virtual RequestHandlerIFCRef getRequestHandler(const String& id) ;
		virtual void addSelectable(SelectableIFCRef obj, SelectableCallbackIFCRef cb);
		virtual void removeSelectable(SelectableIFCRef obj, SelectableCallbackIFCRef cb);
		void init();
		void startServices();
		void shutdown();
		virtual void setConfigItem( const String& item,
				const String& value, EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS );
		virtual String getConfigItem( const String& item ) const;
		void logDebug(const String& message);
		void logCustInfo(const String& message);
		void logError(const String& message);
		void runSelectEngine();
	private:
		void loadConfigItemsFromFile( const String& filename );
		void clearConfigItems();
		void deleteConfigMap();
		void createService();
		void startService();
		void deleteService();
		void createLogger();
		void setLogLevel(const LogLevel level);
		void setLogLevel(const String& level);
		void deleteLogger();
		void createAuthManager();
		void deleteAuthManager();
		void createRequestHandler();
		void deleteRequestHandler();
		LoggerRef m_Logger;
		
		typedef SortedVectorMap<String, String> config_t;
		config_t m_configItems;
		
		typedef std::pair<SelectableIFCRef, SelectableCallbackIFCRef> SelectablePair_t;
		Array<SelectablePair_t> m_selectables;
		ServiceIFCRef m_service;
		AuthManagerRef m_authManager;
		RequestHandlerIFCRef m_requestHandler;
};

} // end namespace OpenWBEM

#endif // OW_ENVIRONMENT_HPP_
