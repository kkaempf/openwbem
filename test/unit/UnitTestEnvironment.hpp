/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc. All rights reserved.
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
#ifndef OW_UNIT_TEST_ENVIRONMENT_
#define OW_UNIT_TEST_ENVIRONMENT_

#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_Map.hpp"
#include "OW_Assertion.hpp"
#include <iostream>

using namespace OpenWBEM;

class TestLogger : public Logger
{
protected:
	virtual void doLogMessage(const String &message, const ELogLevel) const {
		std::cout << message << std::endl;
	}
};


class TestEnvironment : public ServiceEnvironmentIFC
{
public:
	virtual LoggerRef getLogger() const {
		return LoggerRef(new TestLogger);
	}
	virtual String getConfigItem(const String &name, const String& defRetVal) const {
		if (config.find(name) != config.end())
		{
			return config.find(name)->second;
		}
		else
			return defRetVal;
	}
	virtual void setConfigItem(const String &item, const String &value, EOverwritePreviousFlag overwritePrevious = E_OVERWRITE_PREVIOUS) {
		Map<String, String>::iterator it = config.find(item);
		if(it == config.end() || overwritePrevious)
		{
			config[item] = value;
		}
	}
	virtual bool authenticate(String &, const String &, String &, OperationContext&) {
		return true;
	}
	virtual void addSelectable(SelectableIFCRef, SelectableCallbackIFCRef) {
	}
	virtual void removeSelectable(SelectableIFCRef, SelectableCallbackIFCRef) {
	}
	virtual RequestHandlerIFCRef getRequestHandler(const String &) {
		return RequestHandlerIFCRef();
	}
	virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext &, ESendIndicationsFlag, EBypassProvidersFlag) {
		OW_ASSERT("Cannot call TestEnvironment::getCIMOMHandle()" == 0);
		return CIMOMHandleIFCRef();
	}

	Map<String, String> config;
};

extern ServiceEnvironmentIFCRef g_testEnvironment;

#endif

