/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#ifndef OW_UNIT_TEST_ENVIRONMENT_
#define OW_UNIT_TEST_ENVIRONMENT_

#include "OW_ServiceEnvironmentIFC.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_Map.hpp"
#include <iostream>

class OW_TestLogger : public OW_Logger
{
protected:
	virtual void doLogMessage(const OW_String &message, const OW_LogLevel) const {
		std::cout << message << std::endl;
	}
};


class OW_TestEnvironment : public OW_ServiceEnvironmentIFC
{
public:
	virtual OW_LoggerRef getLogger() const {
		return OW_LoggerRef(new OW_TestLogger);
	}
	virtual OW_String getConfigItem(const OW_String &name, const OW_String& defRetVal) const {
		if (config.find(name) != config.end())
		{
			return config.find(name)->second;
		}
		else
			return defRetVal;
	}
	virtual void setConfigItem(const OW_String &item, const OW_String &value, OW_Bool overwritePrevious=true) {
		OW_Map<OW_String, OW_String>::iterator it = config.find(item);
		if(it == config.end() || overwritePrevious)
		{
			config[item] = value;
		}
	}
	virtual OW_Bool authenticate(OW_String &, const OW_String &, OW_String &) {
		return true;
	}
	virtual void addSelectable(OW_SelectableIFCRef, OW_SelectableCallbackIFCRef) {
	}
	virtual void removeSelectable(OW_SelectableIFCRef, OW_SelectableCallbackIFCRef) {
	}
	virtual OW_RequestHandlerIFCRef getRequestHandler(const OW_String &) {
		return OW_RequestHandlerIFCRef();
	}
	virtual OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_String &, const OW_Bool, const OW_Bool) {
		OW_THROW(OW_Exception, "Cannot call OW_TestEnvironment::getCIMOMHandle()");
		//return OW_CIMOMHandleIFCRef();
	}

	OW_Map<OW_String, OW_String> config;
};

extern OW_ServiceEnvironmentIFCRef g_testEnvironment;

#endif

