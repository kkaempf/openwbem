/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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
#include "OW_Types.h"
#include "OW_LogLevel.hpp"
#include "OW_Reference.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_SortedVector.hpp"
#include "OW_ServiceIFC.hpp"
#include "OW_AuthManager.hpp"
#include "OW_RequestHandlerIFC.hpp"

static const char* const OW_DAEMON_NAME = "newdaemon";

class OW_DaemonEnv : public OW_ServiceEnvironmentIFC
{
	public:
		virtual OW_CIMOMHandleRef getCIMOMHandle(const OW_String &username, const OW_Bool doIndications);
		virtual OW_LoggerRef getLogger() const;
		virtual OW_Bool authenticate(OW_String &userName, const OW_String &info, OW_String &details);
		virtual OW_RequestHandlerIFCRef getRequestHandler(const OW_String& id) const;
		virtual void addSelectable(OW_SelectableIFCRef obj, OW_SelectableCallbackIFCRef cb);
		virtual void removeSelectable(OW_SelectableIFCRef obj, OW_SelectableCallbackIFCRef cb);

		void init();
		void startServices();
		void shutdown();

		virtual void setConfigItem( const OW_String& item,
				const OW_String& value, OW_Bool overwritePrevious = true );
		virtual OW_String getConfigItem( const OW_String& item ) const;

		void logDebug(const OW_String& message);
		void logCustInfo(const OW_String& message);
		void logError(const OW_String& message);

		void runSelectEngine();

	private:
		void loadConfigItemsFromFile( const OW_String& filename );
		void clearConfigItems();
		void deleteConfigMap();
		void createService();
		void startService();
		void deleteService();
		void createLogger();
		void setLogLevel(const OW_LogLevel level);
		void setLogLevel(const OW_String& level);
		void deleteLogger();
		void createAuthManager();
		void deleteAuthManager();
		void createRequestHandler();
		void deleteRequestHandler();

		OW_LoggerRef m_Logger;
		
		typedef OW_SortedVector<OW_String, OW_String> config_t;
		config_t m_configItems;
		
		typedef std::pair<OW_SelectableIFCRef, OW_SelectableCallbackIFCRef> SelectablePair_t;
		OW_Array<SelectablePair_t> m_selectables;

		OW_ServiceIFCRef m_service;
		OW_AuthManagerRef m_authManager;
		OW_RequestHandlerIFCRef m_requestHandler;
		OW_SharedLibraryRef m_serviceLib, m_requestHandlerLib;

};

#endif // OW_ENVIRONMENT_HPP_
