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

#ifndef OW_APPENDER_LOGGER_HPP_INCLUDE_GUARD_
#define OW_APPENDER_LOGGER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_LogLevel.hpp"
#include "OW_Logger.hpp"
#include "OW_Array.hpp"

namespace OW_NAMESPACE
{

/**
 * This implementation of Logger is used to send a Log message to multiple LogAppenders
 */
class OW_COMMON_API AppenderLogger : public Logger
{
public:
	AppenderLogger(const String& defaultComponent, ELogLevel level, const LogAppenderRef& appender);
	AppenderLogger(const String& defaultComponent, const Array<LogAppenderRef>& appenders);
	virtual ~AppenderLogger();

private:
	virtual void doProcessLogMessage(const LogMessage& message) const;
	bool doComponentAndCategoryAreEnabled(const String& component, const String& category) const;
	bool doCategoryIsEnabled(const String& category) const;
	LoggerRef doClone() const;

	static ELogLevel getLevel(const Array<LogAppenderRef>& appenders);

private:
#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	Array<LogAppenderRef> m_appenders;

#ifdef OW_WIN32
#pragma warning (pop)
#endif
};

} // end namespace OW_NAMESPACE

#endif



