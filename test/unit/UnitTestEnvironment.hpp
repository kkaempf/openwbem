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
	/**
	 * To be overridden by derived classes with a function that does the
	 * actual logging.
	 */
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
	virtual OW_String getConfigItem(const OW_String &name) const {
		if (config.find(name) != config.end())
		{
			return config.find(name)->second;
		}
		else
			return "";
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
	virtual OW_RequestHandlerIFCRef getRequestHandler(const OW_String &) const {
		return OW_RequestHandlerIFCRef();
	}
	virtual OW_CIMOMHandleIFCRef getCIMOMHandle(const OW_String &, const OW_Bool) {
		return OW_CIMOMHandleIFCRef();
	}

	OW_Map<OW_String, OW_String> config;
};

extern OW_ServiceEnvironmentIFCRef g_testEnvironment;

#endif

