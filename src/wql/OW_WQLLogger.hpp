#ifndef OW_WQLLOGGER_HPP

#include "OW_config.h"
#include "OW_AppenderLogger.hpp"
#include "OW_Array.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_FileAppender.hpp"
#include "OW_Logger.hpp"
#include "OW_LogAppender.hpp"
#include "OW_String.hpp"
#include <sstream>
	
#ifdef __GNUC__
#define OW_FUNCTION_NAME	__PRETTY_FUNCTION__
#else
#define OW_FUNCTION_NAME ""
#endif

#ifdef __GNUC__
#define OW_TYPE_NAME(type_or_object)	::OpenWBEM::demangle(typeid(type_or_object).name()).c_str()
#else
#define OW_TYPE_NAME typeid(type_or_object).name()
#endif

#define WQL_LOG_DEBUG(message)												\
do																								\
{																									\
	String messageWithFunction(message);						\
	messageWithFunction+= String(" ") + String(OW_FUNCTION_NAME) + String(" "); \
	OW_LOG_DEBUG(::OpenWBEM::WQLLogger::getLogger(), messageWithFunction); \
}while(0)

namespace OpenWBEM
{
#ifdef __GNUC__
	String demangle(char const* name);
#endif
	// endif __GNUC__	
	namespace WQLLogger
	{
		LoggerRef getLogger();
	}
}

#endif
/* endif OW_WQLLOGGER_HPP */
