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
#include "OW_LogAppender.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_LogMessage.hpp"
#include "OW_Logger.hpp"
#include "OW_Assertion.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_NullAppender.hpp"
#ifndef OW_WIN32
#include "OW_SyslogAppender.hpp"
#endif
#include "OW_CerrAppender.hpp"
#include "OW_FileAppender.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_ConfigOpts.hpp"

namespace OpenWBEM
{


//////////////////////////////////////////////////////////////////////////////
LogAppender::LogAppender(const StringArray& components, const StringArray& categories, const String& pattern)
	: m_components(components.begin(), components.end())
	, m_categories(categories.begin(), categories.end())
	, m_formatter(pattern)
{
	m_allComponents = m_components.count("*") > 0;
	m_allCategories = m_categories.count("*") > 0;
}

//////////////////////////////////////////////////////////////////////////////
void
LogAppender::logMessage(const LogMessage& message) const
{
	if (componentAndCategoryAreEnabled(message.component, message.category))
	{
		StringBuffer buf;
		m_formatter.formatMessage(message, buf);
		doProcessLogMessage(buf.releaseString(), message);
	}
}

//////////////////////////////////////////////////////////////////////////////
LogAppender::~LogAppender()
{
}

//////////////////////////////////////////////////////////////////////////////
bool
LogAppender::categoryIsEnabled(const String& category) const
{
	return m_allCategories || m_categories.count(category) > 0;
}

//////////////////////////////////////////////////////////////////////////////
bool
LogAppender::componentAndCategoryAreEnabled(const String& component, const String& category) const
{
	return (m_allComponents || m_components.count(component) > 0) &&
		categoryIsEnabled(category);
}

//////////////////////////////////////////////////////////////////////////////
ELogLevel
LogAppender::getLogLevel() const
{
	int nonLevelCategoryCount = m_categories.size() -
		m_categories.count(Logger::STR_DEBUG_CATEGORY) -
		m_categories.count(Logger::STR_INFO_CATEGORY) -
		m_categories.count(Logger::STR_ERROR_CATEGORY) -
		m_categories.count(Logger::STR_FATAL_CATEGORY);

	if (m_allCategories || nonLevelCategoryCount > 0 || categoryIsEnabled(Logger::STR_DEBUG_CATEGORY))
	{
		return E_DEBUG_LEVEL;
	}
	else if (categoryIsEnabled(Logger::STR_INFO_CATEGORY))
	{
		return E_INFO_LEVEL;
	}
	else if (categoryIsEnabled(Logger::STR_ERROR_CATEGORY))
	{
		return E_ERROR_LEVEL;
	}
	else if (categoryIsEnabled(Logger::STR_FATAL_CATEGORY))
	{
		return E_FATAL_ERROR_LEVEL;
	}
	OW_ASSERTMSG(0, "Internal error. LogAppender unable to determine log level!");
	return E_DEBUG_LEVEL;
}

/////////////////////////////////////////////////////////////////////////////
LogAppenderRef
LogAppender::createLogAppender(
	const String& name,
	const StringArray& components,
	const StringArray& categories,
	const String& messageFormat,
	const String& type,
	const ConfigFile::ConfigMap& configItems)
{
	// name and configItems are unused for now, but are provided so if a logger needs to look up configuration, it
	// can use name to find the appropriate config items.

	LogAppenderRef appender;
	if (type.empty() || type.equalsIgnoreCase("null"))
	{
		appender = new NullAppender(components, categories, messageFormat);
	}
#ifndef OW_WIN32
	else if ( type == "syslog" )
	{
		appender = new SyslogAppender(components, categories, messageFormat);
	}
#endif
	else if (type == "stderr" || type == "cerr")
	{
		appender = new CerrAppender(components, categories, messageFormat);
	}
	else if (type == "file")
	{
		String configItem = Format(ConfigOpts::LOG_1_LOCATION_opt, name);
		String filename = ConfigFile::getConfigItem(configItems, configItem);
		appender = new FileAppender(components, categories, filename.c_str(), messageFormat);
	}
	else
	{
		OW_THROW_ERR(LoggerException, Format("Unknown log type: %1", type).c_str(), Logger::E_UNKNOWN_LOG_APPENDER_TYPE);
	}

	return appender;
}


} // end namespace OpenWBEM


