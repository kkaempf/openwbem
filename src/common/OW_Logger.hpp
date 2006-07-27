/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
 * @author Dan Nuffer
 */

#ifndef OW_LOGGER_HPP_INCLUDE_GUARD_
#define OW_LOGGER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_String.hpp"
#include "OW_LogLevel.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_Exception.hpp"
#include "OW_LogAppender.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(Logger, OW_COMMON_API)


/**
 * Logging interface. Used to output log messages.
 * A Logger has a component, a log level and a LogAppenderRef.
 * The component will be used for all log messages generated, unless another component is explicitly
 * specified in a call to logMessage().
 * The AppenderRef is the target for log messages. The global LogAppenderRef will be retrieved via 
 * LogAppender::getCurrentLogAppender() if one is not passed to the constructor.
 * An application may call LogAppender::setDefaultLogAppender() to set the global LogAppenderRef.
 * The log level will be obtained from the appender by calling m_appender->getLogLevel().
 *
 * Invariants:
 * - m_defaultComponent != ""
 * - m_appender is not NULL.
 *
 * Responsibilities:
 * - Report log level.
 * - Provide interface to log messages.
 * - Filter messages based on log level.
 *
 * Collaborators:
 * - Any class which needs to log messages.
 *
 * Thread safety: read/write, except for setDefaultComponent() and setLogLevel() which should
 * only be called during initialization phase.
 *
 * Copy semantics: Value
 *
 * Exception safety: Strong
 */
class OW_COMMON_API Logger : public IntrusiveCountableBase
{
public:

	static const String STR_NONE_CATEGORY;
	static const String STR_FATAL_CATEGORY;
	static const String STR_ERROR_CATEGORY;
	static const String STR_INFO_CATEGORY;
	static const String STR_DEBUG_CATEGORY;
	static const String STR_ALL_CATEGORY;
	static const String STR_DEFAULT_COMPONENT; // "none"

	enum ELoggerErrorCodes
	{
		E_UNKNOWN_LOG_APPENDER_TYPE,
		E_INVALID_MAX_FILE_SIZE,
		E_INVALID_MAX_BACKUP_INDEX
	};

	/**
	 * @param defaultComponent The component used for log messages (can be overridden by logMessage())
	 * @param appender The Appender which will receive log messages. If NULL, the result of LogAppender::getCurrentLogAppender() will be used.
	 */
	Logger(const String& defaultComponent = STR_DEFAULT_COMPONENT, const LogAppenderRef& appender = LogAppenderRef());

	Logger(const Logger&);
	Logger& operator=(const Logger&);
	void swap(Logger& x);
	virtual ~Logger();

	virtual LoggerRef clone() const OW_DEPRECATED; // in 4.0.0
	
	/**
	 * Log message with a fatal error category and the default component.
	 * @param message The string to log.
	 * @param filename The file where the log statement was written.
	 * @param fileline The line number of the file where the log statement was written.
	 * @param methodname The method name where the log statement was written.
	 */
	void logFatalError(const String& message, const char* filename = 0, int fileline = -1, const char* methodname = 0) const;
	
	/**
	 * If getLogLevel() >= E_ERROR_LEVEL, Log message with an error category and the default component.
	 * @param message The string to log.
	 * @param filename The file where the log statement was written.
	 * @param fileline The line number of the file where the log statement was written.
	 * @param methodname The method name where the log statement was written.
	 */
	void logError(const String& message, const char* filename = 0, int fileline = -1, const char* methodname = 0) const;
	
	/**
	 * If getLogLevel() >= E_INFO_LEVEL, Log info.
	 * @param message The string to log.
	 * @param filename The file where the log statement was written.
	 * @param fileline The line number of the file where the log statement was written.
	 * @param methodname The method name where the log statement was written.
	 */
	void logInfo(const String& message, const char* filename = 0, int fileline = -1, const char* methodname = 0) const;
	
	/**
	 * If getLogLevel() >= E_DEBUG_LEVEL, Log debug info.
	 * @param message The string to log.
	 * @param filename The file where the log statement was written.
	 * @param fileline The line number of the file where the log statement was written.
	 * @param methodname The method name where the log statement was written.
	 */
	void logDebug(const String& message, const char* filename = 0, int fileline = -1, const char* methodname = 0) const;

	// Note that we don't use defaults on logMessage so the correct overload will be chosen.
	/**
	 * Log a message using the specified component and category
	 * The current log level is ignored.
	 * @param component The component generating the log message.
	 * @param category The category of the log message.
	 * @param message The message to log
	 */
	void logMessage(const String& component, const String& category, const String& message) const;
	/**
	 * Log a message using the specified component and category
	 * The current log level is ignored.
	 * @param component The component generating the log message.
	 * @param category The category of the log message.
	 * @param message The message to log
	 * @param filename The file where the log statement was written.
	 * @param fileline The line number of the file where the log statement was written.
	 * @param methodname The method name where the log statement was written.
	 */
	void logMessage(const String& component, const String& category, const String& message, const char* filename, int fileline, const char* methodname) const;

	/**
	 * Log a message using the default component and specified category.
	 * The current log level is ignored.
	 * @param category The category of the log message.
	 * @param message The message to log
	 */
	void logMessage(const String& category, const String& message) const;
	
	/**
	 * Log a message using the default component and specified category.
	 * The current log level is ignored.
	 * @param category The category of the log message.
	 * @param message The message to log
	 * @param filename The file where the log statement was written.
	 * @param fileline The line number of the file where the log statement was written.
	 * @param methodname The method name where the log statement was written.
	 */
	void logMessage(const String& category, const String& message, const char* filename, int fileline, const char* methodname) const;

	/**
	 * Log a message.
	 * The current log level is ignored.
	 * @param message The message to log
	 */
	void logMessage(const LogMessage& message) const;

	/**
	 * Sets the default component.
	 * This function is not thread safe.
	 * @param component The new default component
	 */
	void setDefaultComponent(const String& component);

	/**
	 * Gets the default component.
	 * @return The default component
	 */
	String getDefaultComponent() const;

	/**
	 * @return The current log level
	 */
	ELogLevel getLogLevel() const
	{
		return m_logLevel;
	}

	/**
	 * Set the log level. All lower level log messages will be ignored.
	 * This function is not thread safe.
	 * @param logLevel the level as an enumeration value.
	 */
	void setLogLevel(ELogLevel logLevel);

	/**
	 * Set the log level. All lower level log messages will be ignored.
	 * This function is not thread safe.
	 * @param logLevel The log level, valid values: { STR_FATAL_ERROR_CATEGORY, STR_ERROR_CATEGORY,
	 *   STR_INFO_CATEGORY, STR_DEBUG_CATEGORY }.  Case-insensitive.
	 *   If logLevel is unknown, the level will be set to E_FATAL_ERROR_LEVEL
	 */
	void setLogLevel(const String& logLevel);

	/**
	 * Determine if the log category is enabled.
	 */
	bool categoryIsEnabled(const String& category) const;

	/**
	 * Check if the logger is enabled for given level.
	 */
	bool levelIsEnabled(const ELogLevel level);

	/**
	 * Determine if the component and category are both enabled.
	 */
	bool componentAndCategoryAreEnabled(const String& component, const String& category) const;

	/**
	 * Utility functions for backward compatibility with LoggerRef and the OW_LOG macros
	 */
	static inline const Logger& asLogger(const Logger& lgr)
	{
		return lgr;
	}
	static inline const Logger& asLogger(const LoggerRef& lgr)
	{
		return *lgr;
	}

private:
	void processLogMessage(const LogMessage& message) const;

private: // data
	String m_defaultComponent;
	LogAppenderRef m_appender;
	ELogLevel m_logLevel;
};
OW_EXPORT_TEMPLATE(OW_COMMON_API, IntrusiveReference, Logger);

} // end namespace OW_NAMESPACE


#if defined(OW_HAVE_UUPRETTY_FUNCTIONUU)
#define OW_LOGGER_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(OW_HAVE_C99_UUFUNCUU)
#define OW_LOGGER_PRETTY_FUNCTION __func__
#else
#define OW_LOGGER_PRETTY_FUNCTION ""
#endif

/**
 * Log message to logger with the Debug level.  message is only evaluated if logger->getLogLevel() >= E_DEBUG_LEVEL
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define OW_LOG_DEBUG(logger, message) \
do \
{ \
	if (::OW_NAMESPACE::Logger::asLogger((logger)).getLogLevel() >= ::OW_NAMESPACE::E_DEBUG_LEVEL) \
	{ \
		::OW_NAMESPACE::Logger::asLogger((logger)).logMessage(::OW_NAMESPACE::Logger::STR_DEBUG_CATEGORY, (message), __FILE__, __LINE__, OW_LOGGER_PRETTY_FUNCTION); \
	} \
} while (0)

/**
 * Log message to logger with the Info level.  message is only evaluated if logger->getLogLevel() >= E_INFO_LEVEL
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define OW_LOG_INFO(logger, message) \
do \
{ \
	if (::OW_NAMESPACE::Logger::asLogger((logger)).getLogLevel() >= ::OW_NAMESPACE::E_INFO_LEVEL) \
	{ \
		::OW_NAMESPACE::Logger::asLogger((logger)).logMessage(::OW_NAMESPACE::Logger::STR_INFO_CATEGORY, (message), __FILE__, __LINE__, OW_LOGGER_PRETTY_FUNCTION); \
	} \
} while (0)

/**
 * Log message to logger with the Error level.  message is only evaluated if logger->getLogLevel() >= E_ERROR_LEVEL
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define OW_LOG_ERROR(logger, message) \
do \
{ \
	if (::OW_NAMESPACE::Logger::asLogger((logger)).getLogLevel() >= ::OW_NAMESPACE::E_ERROR_LEVEL) \
	{ \
		::OW_NAMESPACE::Logger::asLogger((logger)).logMessage(::OW_NAMESPACE::Logger::STR_ERROR_CATEGORY, (message), __FILE__, __LINE__, OW_LOGGER_PRETTY_FUNCTION); \
	} \
} while (0)

/**
 * Log message to logger with the FatalError level.  message is always evaluated.
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define OW_LOG_FATAL_ERROR(logger, message) \
do \
{ \
	if (::OW_NAMESPACE::Logger::asLogger((logger)).getLogLevel() >= ::OW_NAMESPACE::E_FATAL_ERROR_LEVEL) \
	{ \
		::OW_NAMESPACE::Logger::asLogger((logger)).logMessage(::OW_NAMESPACE::Logger::STR_FATAL_CATEGORY, (message), __FILE__, __LINE__, OW_LOGGER_PRETTY_FUNCTION); \
	} \
} while (0)

/**
 * Log message to logger with the specified category.  message is only evaluated if logger->categoryIsEnabled(category) == true
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param category The message category
 * @param message An expression that evaluates to a String which will be logged.
 */
#define OW_LOG(logger, category, message) \
do \
{ \
	if (::OW_NAMESPACE::Logger::asLogger((logger)).categoryIsEnabled((category))) \
	{ \
		::OW_NAMESPACE::Logger::asLogger((logger)).logMessage((category), (message), __FILE__, __LINE__, OW_LOGGER_PRETTY_FUNCTION); \
	} \
} while (0)



#endif
