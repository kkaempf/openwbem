#include "OW_WQLLogger.hpp"
#include <vector>
#include <string.h>

#ifdef __GNUC__
#include <cxxabi.h>
#endif //__GNUC__

namespace OpenWBEM
{
#ifdef __GNUC__
	String demangle(char const* name)
	{
		std::vector<char> outputBuffer(strlen(name)*8);
		size_t length(outputBuffer.size());
		int error(0);
		String ret(abi::__cxa_demangle(name, &outputBuffer[0], &length, &error));
		if (error)
		{
			return String(name) + " " + String(error) + " ";
		}
		else
		{
			return ret;
		}
	}
#endif //__GNUC__
	
	namespace WQLLogger
	{
		namespace
		{
			LoggerRef createWqlProcessorLogger()
			{
				Array<LogAppenderRef> appenders;
				StringArray components; components.push_back("wql_processor");
				StringArray categories; categories.push_back("*");
				String messageFormat("%r [%t] %p %c - %m");
				appenders.push_back
					( LogAppenderRef
						( new FileAppender
							(components, categories, components[0].c_str(), messageFormat)
						)
					);
				return LoggerRef(new AppenderLogger(components[0], appenders));
			}
		
		}
		LoggerRef getLogger()
		{
			//****************
			//FIXME: Not thread-safe.
			static LoggerRef logger(createWqlProcessorLogger());
			return logger;
		}
	}
}

