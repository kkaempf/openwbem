/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_Logger.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_LogMessage.hpp"
#include "OW_Assertion.hpp"
#include "OW_Array.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_LogMessagePatternFormatter.hpp"
#include "OW_AppenderLogger.hpp"
#include "OW_LogAppender.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(Logger);

const String Logger::STR_NONE_CATEGORY("NONE");
const String Logger::STR_FATAL_CATEGORY("FATAL");
const String Logger::STR_ERROR_CATEGORY("ERROR");
const String Logger::STR_INFO_CATEGORY("INFO");
const String Logger::STR_DEBUG_CATEGORY("DEBUG");
const String Logger::STR_ALL_CATEGORY("ALL");
const String Logger::STR_DEFAULT_COMPONENT("none");

//////////////////////////////////////////////////////////////////////////////
Logger::Logger(const String& defaultComponent, const LogAppenderRef& appender)
	: m_defaultComponent(defaultComponent)
	, m_appender(appender ? appender : LogAppender::getCurrentLogAppender())
	, m_logLevel(m_appender->getLogLevel())
{
	OW_ASSERT(m_defaultComponent != "");
}

/////////////////////////////////////////////////////////////////////////////
Logger::Logger(const Logger& x)
	: IntrusiveCountableBase(x)
	, m_defaultComponent(x.m_defaultComponent)
	, m_appender(x.m_appender)
	, m_logLevel(x.m_logLevel)
{
}

/////////////////////////////////////////////////////////////////////////////
Logger&
Logger::operator=(const Logger& x)
{
	m_defaultComponent = x.m_defaultComponent;
	m_appender = x.m_appender;
	m_logLevel = x.m_logLevel;

	return *this;
}

/////////////////////////////////////////////////////////////////////////////
void
Logger::swap(Logger& x)
{
	m_defaultComponent.swap(x.m_defaultComponent);
	m_appender.swap(x.m_appender);
	std::swap(m_logLevel, x.m_logLevel);
}

//////////////////////////////////////////////////////////////////////////////
Logger::~Logger()
{
}

//////////////////////////////////////////////////////////////////////////////
LoggerRef
Logger::clone() const
{
	return LoggerRef(new Logger(*this));
}
	
//////////////////////////////////////////////////////////////////////////////
void
Logger::logFatalError(const String& message, const char* filename, int fileline, const char* methodname) const
{
	if (m_logLevel >= E_FATAL_ERROR_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_FATAL_CATEGORY, message, filename, fileline, methodname) );
	}
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logError(const String& message, const char* filename, int fileline, const char* methodname) const
{
	if (m_logLevel >= E_ERROR_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_ERROR_CATEGORY, message, filename, fileline, methodname) );
	}
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logInfo(const String& message, const char* filename, int fileline, const char* methodname) const
{
	if (m_logLevel >= E_INFO_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_INFO_CATEGORY, message, filename, fileline, methodname) );
	}
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logDebug(const String& message, const char* filename, int fileline, const char* methodname) const
{
	if (m_logLevel >= E_DEBUG_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_DEBUG_CATEGORY, message, filename, fileline, methodname) );
	}
}
	
//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& component, const String& category, const String& message) const
{
	processLogMessage(LogMessage(component, category, message, 0, -1, 0));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& component, const String& category, const String& message, const char* filename, int fileline, const char* methodname) const
{
	processLogMessage(LogMessage(component, category, message, filename, fileline, methodname));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& category, const String& message) const
{
	processLogMessage(LogMessage(m_defaultComponent, category, message, 0, -1, 0));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& category, const String& message, const char* filename, int fileline, const char* methodname) const
{
	processLogMessage(LogMessage(m_defaultComponent, category, message, filename, fileline, methodname));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const LogMessage& message) const
{
	processLogMessage(message);
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::setDefaultComponent(const String& component)
{
	OW_ASSERT(component != "");
	m_defaultComponent = component;
}

//////////////////////////////////////////////////////////////////////////////
String
Logger::getDefaultComponent() const
{
	return m_defaultComponent;
}
	
//////////////////////////////////////////////////////////////////////////////
void
Logger::setLogLevel(ELogLevel logLevel)
{
	m_logLevel = logLevel;
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::setLogLevel(const String& l)
{
	if (l.equalsIgnoreCase(STR_INFO_CATEGORY))
	{
		setLogLevel(E_INFO_LEVEL);
	}
	else if (l.equalsIgnoreCase(STR_DEBUG_CATEGORY))
	{
		setLogLevel(E_DEBUG_LEVEL);
	}
	else if (l.equalsIgnoreCase(STR_ERROR_CATEGORY))
	{
		setLogLevel(E_ERROR_LEVEL);
	}
	else if (l.equalsIgnoreCase(STR_ALL_CATEGORY))
	{
		setLogLevel(E_ALL_LEVEL);
	}
	else if (l.equalsIgnoreCase(STR_NONE_CATEGORY))
	{
		setLogLevel(E_NONE_LEVEL);
	}
	else
	{
		setLogLevel(E_FATAL_ERROR_LEVEL);
	}
}

//////////////////////////////////////////////////////////////////////////////
bool
Logger::categoryIsEnabled(const String& category) const
{
	return m_appender->categoryIsEnabled(category);
}

/////////////////////////////////////////////////////////////////////////////
bool
Logger::levelIsEnabled(const ELogLevel level)
{
	return (getLogLevel() >= level);
}

//////////////////////////////////////////////////////////////////////////////
bool
Logger::componentAndCategoryAreEnabled(const String& component, const String& category) const
{
	return m_appender->componentAndCategoryAreEnabled(component, category);
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::processLogMessage(const LogMessage& message) const
{
	OW_ASSERT(!message.component.empty());
	OW_ASSERT(!message.category.empty());
	OW_ASSERT(!message.message.empty());

	m_appender->logMessage(message);
}

} // end namespace OW_NAMESPACE

