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
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_FileSystem.hpp"

#include <fstream>

namespace OpenWBEM
{

/////////////////////////////////////////////////////////////////////////////
FileAppender::FileAppender(const StringArray& components,
	const StringArray& categories,
	const char* filename,
	const String& pattern,
	UInt64 maxFileSize,
	unsigned int maxBackupIndex)
	: LogAppender(components, categories, pattern)
	, m_filename(filename)
	, m_maxFileSize(maxFileSize)
	, m_maxBackupIndex(maxBackupIndex)
{
	m_log.open(m_filename.c_str(), std::ios::out | std::ios::app);
	if (!m_log)
	{
		OW_THROW(LoggerException, Format("FileAppender: Unable to open file: %1", m_filename).toString().c_str() );
	}
}

/////////////////////////////////////////////////////////////////////////////
FileAppender::~FileAppender()
{
}

/////////////////////////////////////////////////////////////////////////////
namespace
{
	Mutex fileGuard;
}
void
FileAppender::doProcessLogMessage(const String& formattedMessage, const LogMessage& message) const
{
	MutexLock lock(fileGuard);

	// take into account external log rotators, if the file we have open no longer exists, then reopen it.
	if (!FileSystem::exists(m_filename.c_str()))
	{
		m_log.close();
		m_log.open(m_filename.c_str(), std::ios::out | std::ios::app);
	}

	if (!m_log)
	{
		// hmm, not much we can do here.  doProcessLogMessage can't throw.
		return;
	}

	m_log.write(formattedMessage.c_str(), formattedMessage.length());
	m_log << '\n';

	// handle log rotation
	if (m_maxFileSize != NO_MAX_LOG_SIZE && m_log.tellp() >= static_cast<std::streampos>(m_maxFileSize * 1024))
	{
		// since we can't throw an exception, or log any errors, it something fails here, we'll just return silently :-(

		// do the roll over
		m_log.close();

		if (m_maxBackupIndex > 0)
		{
			// delete the oldest file first - this may or may not exist, we try anyway.
			FileSystem::removeFile(m_filename + '.' + String(m_maxBackupIndex));

			// increment the numbers on all the files - some may exist or not, but try anyway.
			for (unsigned int i = m_maxBackupIndex - 1; i >= 1; --i)
			{
				FileSystem::renameFile(m_filename + '.' + String(i), m_filename + '.' + String(i + 1));
			}

			if (!FileSystem::renameFile(m_filename, m_filename + ".1"))
			{
				// if we can't rename it, avoid truncating it.
				return;
			}
		}

		// truncate the existing one
		m_log.open(m_filename.c_str(), std::ios_base::out | std::ios_base::trunc);
	}
}

/////////////////////////////////////////////////////////////////////////////
const String FileAppender::STR_DEFAULT_MESSAGE_PATTERN("%d{%a %b %d %H:%M:%S %Y} [%t]: %m");

} // end namespace OpenWBEM




