/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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
#include "OW_AppenderLogger.hpp"
#include "OW_LogMessage.hpp"
#include "OW_LogAppender.hpp"

namespace OpenWBEM
{

/////////////////////////////////////////////////////////////////////////////
AppenderLogger::AppenderLogger(const String& defaultComponent, ELogLevel level, const LogAppenderRef& appender)
	: Logger(defaultComponent, level)
	, m_appenders(1, appender)
{
}

/////////////////////////////////////////////////////////////////////////////
AppenderLogger::AppenderLogger(const String& defaultComponent, const Array<LogAppenderRef>& appenders)
	: Logger(defaultComponent, getLevel(appenders))
	, m_appenders(appenders)
{
}

/////////////////////////////////////////////////////////////////////////////
AppenderLogger::~AppenderLogger()
{
}

/////////////////////////////////////////////////////////////////////////////
bool
AppenderLogger::useDeprecatedDoLogMessage() const
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////
void
AppenderLogger::doProcessLogMessage(const LogMessage& message) const
{
	for (size_t i = 0; i < m_appenders.size(); ++i)
	{
		m_appenders[i]->logMessage(message);
	}
}

/////////////////////////////////////////////////////////////////////////////
bool
AppenderLogger::doComponentAndCategoryAreEnabled(const String& component, const String& category) const
{
	for (size_t i = 0; i < m_appenders.size(); ++i)
	{
		if (m_appenders[i]->componentAndCategoryAreEnabled(component, category))
		{
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool
AppenderLogger::doCategoryIsEnabled(const String& category) const
{
	for (size_t i = 0; i < m_appenders.size(); ++i)
	{
		if (m_appenders[i]->categoryIsEnabled(category))
		{
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
ELogLevel
AppenderLogger::getLevel(const Array<LogAppenderRef>& appenders)
{
	ELogLevel rv = E_FATAL_ERROR_LEVEL;
	for (size_t i = 0; i < appenders.size(); ++i)
	{
		rv = std::max(rv, appenders[i]->getLogLevel());
	}
	return rv;
}

} // end namespace OpenWBEM






