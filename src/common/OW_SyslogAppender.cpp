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
#include "OW_SyslogAppender.hpp"
#include "OW_Logger.hpp"
#include "OW_LogMessage.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include <syslog.h>

namespace OW_NAMESPACE
{

/////////////////////////////////////////////////////////////////////////////
SyslogAppender::SyslogAppender(const StringArray& components,
	const StringArray& categories,
	const String& pattern)
	: LogAppender(components, categories, pattern)
{
	if (!calledOpenLog)
	{
		openlog( OW_PACKAGE_PREFIX"openwbem", LOG_CONS, LOG_DAEMON );
		calledOpenLog = true;
	}
}

/////////////////////////////////////////////////////////////////////////////
SyslogAppender::~SyslogAppender() {}

/////////////////////////////////////////////////////////////////////////////
namespace
{
	Mutex syslogGuard;
}
void
SyslogAppender::doProcessLogMessage(const String& formattedMessage, const LogMessage& message) const
{
	int syslogPriority;
	if (message.category == Logger::STR_FATAL_CATEGORY)
	{
		syslogPriority = LOG_CRIT;
	}
	else if (message.category == Logger::STR_ERROR_CATEGORY)
	{
		syslogPriority = LOG_ERR;
	}
	else if (message.category == Logger::STR_INFO_CATEGORY)
	{
		syslogPriority = LOG_INFO;
	}
	else if (message.category == Logger::STR_DEBUG_CATEGORY)
	{
		syslogPriority = LOG_DEBUG;
	}
	else
	{
		syslogPriority = LOG_INFO;
	}

	StringArray a = formattedMessage.tokenize("\n");
	MutexLock lock(syslogGuard);
	for (size_t i = 0; i < a.size(); ++i)
	{
		syslog( syslogPriority, "%s", a[i].c_str() );
	}
}

/////////////////////////////////////////////////////////////////////////////
bool SyslogAppender::calledOpenLog = false;
const String SyslogAppender::STR_DEFAULT_MESSAGE_PATTERN("[%t]%m");


} // end namespace OW_NAMESPACE




