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
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_LogMessage.hpp"
#include "OW_Assertion.hpp"
#include "OW_Array.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_LogMessagePatternFormatter.hpp"
#include "OW_AppenderLogger.hpp"
#include "OW_LogAppender.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(Logger);

namespace
{
Mutex LoggerMutex;
}

const String Logger::STR_FATAL_CATEGORY("FATAL");
const String Logger::STR_ERROR_CATEGORY("ERROR");
const String Logger::STR_INFO_CATEGORY("INFO");
const String Logger::STR_DEBUG_CATEGORY("DEBUG");
const String Logger::STR_DEFAULT_COMPONENT("none");

//////////////////////////////////////////////////////////////////////////////
Logger::~Logger()
{
}

//////////////////////////////////////////////////////////////////////////////
Logger::Logger()
	: m_logLevel(E_ERROR_LEVEL)
	, m_defaultComponent(STR_DEFAULT_COMPONENT)
{
}

//////////////////////////////////////////////////////////////////////////////
Logger::Logger(const ELogLevel l)
	: m_logLevel(l)
	, m_defaultComponent(STR_DEFAULT_COMPONENT)
{
}

//////////////////////////////////////////////////////////////////////////////
Logger::Logger(const String& defaultComponent, const ELogLevel l)
	: m_logLevel(l)
	, m_defaultComponent(defaultComponent)
{
	OW_ASSERT(m_defaultComponent != "");
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::processLogMessage(const LogMessage& message) const
{
	OW_ASSERT(!message.component.empty());
	OW_ASSERT(!message.category.empty());
	OW_ASSERT(!message.message.empty());

	// OW_DEPRECATED in 3.1.0 - Remove this stuff once the old doLogMessage() goes away.
	if (useDeprecatedDoLogMessage())
	{
		ELogLevel l = E_INFO_LEVEL;
		if (message.category == STR_FATAL_CATEGORY)
		{
			l = E_FATAL_ERROR_LEVEL;
		}
		else if (message.category == STR_ERROR_CATEGORY)
		{
			l = E_ERROR_LEVEL;
		}
		else if (message.category == STR_INFO_CATEGORY)
		{
			l = E_INFO_LEVEL;
		}
		else if (message.category == STR_DEBUG_CATEGORY)
		{
			l = E_DEBUG_LEVEL;
		}
		MutexLock mtxlck( LoggerMutex );
		doLogMessage(message.message, l);
	}
	else
	{
		doProcessLogMessage(message);
	}
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
	else
	{
		setLogLevel(E_FATAL_ERROR_LEVEL);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logFatalError(const String& message, const char* filename, int fileline) const
{
	if (m_logLevel >= E_FATAL_ERROR_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_FATAL_CATEGORY, message, filename, fileline) );
	}
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logError(const String& message, const char* filename, int fileline) const
{
	if (m_logLevel >= E_ERROR_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_ERROR_CATEGORY, message, filename, fileline) );
	}
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logInfo(const String& message, const char* filename, int fileline) const
{
	if (m_logLevel >= E_INFO_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_INFO_CATEGORY, message, filename, fileline) );
	}
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logDebug(const String& message, const char* filename, int fileline) const
{
	if (m_logLevel >= E_DEBUG_LEVEL)
	{
		processLogMessage( LogMessage(m_defaultComponent, STR_DEBUG_CATEGORY, message, filename, fileline) );
	}
}
	
//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& component, const String& category, const String& message) const
{
	processLogMessage(LogMessage(component, category, message, 0, -1));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& component, const String& category, const String& message, const char* filename, int fileline) const
{
	processLogMessage(LogMessage(component, category, message, filename, fileline));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& category, const String& message) const
{
	processLogMessage(LogMessage(m_defaultComponent, category, message, 0, -1));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::logMessage(const String& category, const String& message, const char* filename, int fileline) const
{
	processLogMessage(LogMessage(m_defaultComponent, category, message, filename, fileline));
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::doLogMessage( const String& message, const ELogLevel level) const
{
	OW_ASSERTMSG(0, "new derived classes which implement doProcessLogMessage() have to implement useOldLogMessage() to return false");
}

//////////////////////////////////////////////////////////////////////////////
bool
Logger::useDeprecatedDoLogMessage() const
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////
void
Logger::doProcessLogMessage(const LogMessage& message) const
{
	OW_ASSERTMSG(0, "new derived classes which implement useOldLogMessage() to return false must also implement doProcessLogMessage()");
}

//////////////////////////////////////////////////////////////////////////////
bool
Logger::categoryIsEnabled(const String& category) const
{
	return doCategoryIsEnabled(category);
}

//////////////////////////////////////////////////////////////////////////////
bool
Logger::componentAndCategoryAreEnabled(const String& component, const String& category) const
{
	return doComponentAndCategoryAreEnabled(component, category);
}

//////////////////////////////////////////////////////////////////////////////
bool
Logger::doComponentAndCategoryAreEnabled(const String& component, const String& category) const
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool
Logger::doCategoryIsEnabled(const String& category) const
{
	return true;
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

/////////////////////////////////////////////////////////////////////////////
LoggerRef
Logger::createLogger( const String& type, bool debug )
{
	StringArray components;
	components.push_back("*");

	StringArray categories;
	categories.push_back("*");

	ConfigFile::ConfigMap configItems;

	Array<LogAppenderRef> appenders;
	// TODO: we need a special case for filenames in the type, since createLogAppender only handles types it knows about
	appenders.push_back(LogAppender::createLogAppender("", components, categories,
		LogMessagePatternFormatter::STR_DEFAULT_MESSAGE_PATTERN, type, configItems));

	if ( debug )
	{
		appenders.push_back(LogAppender::createLogAppender("", components, categories,
			LogMessagePatternFormatter::STR_DEFAULT_MESSAGE_PATTERN, "stderr", configItems));

	}

	return LoggerRef(new AppenderLogger(STR_DEFAULT_COMPONENT, appenders));

}

} // end namespace OpenWBEM

