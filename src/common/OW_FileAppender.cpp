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
#include "OW_FileAppender.hpp"
#include "OW_Format.hpp"
#include "OW_Logger.hpp"
#include "OW_LogMessage.hpp"

#include <fstream>

namespace OpenWBEM
{

/////////////////////////////////////////////////////////////////////////////
FileAppender::FileAppender(const StringArray& components,
	const StringArray& categories,
	const char* filename,
	const String& pattern )
	: LogAppender(components, categories, pattern)
	, m_filename(filename)
{
	std::ofstream log(m_filename.c_str(), std::ios::out | std::ios::app);
	if (!log)
	{
		OW_THROW(LoggerException, Format("FileAppender: Unable to open file: %1", m_filename).toString().c_str() );
	}
}

/////////////////////////////////////////////////////////////////////////////
FileAppender::~FileAppender()
{
}

/////////////////////////////////////////////////////////////////////////////
void
FileAppender::doProcessLogMessage(const String& formattedMessage, const LogMessage& message) const
{
	std::ofstream log(m_filename.c_str(), std::ios::out | std::ios::app);
	if (!log)
	{
		// hmm, not much we can do here.  doProcessLogMessage can't throw.
	}
	else
	{
		log << formattedMessage << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////
const String FileAppender::STR_DEFAULT_MESSAGE_PATTERN("%d{%a %b %d %H:%M:%S %Y} [%t]: %m");

} // end namespace OpenWBEM



