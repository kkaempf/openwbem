#ifndef OW_MULTI_PROCESS_FILE_APPENDER_HPP_INCLUDE_GUARD_
#define OW_MULTI_PROCESS_FILE_APPENDER_HPP_INCLUDE_GUARD_
/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_LogAppender.hpp"
#include "OW_String.hpp"

#include "OW_File.hpp"

namespace OW_NAMESPACE
{

/**
 * This class sends log messges to a file, for use when there may be
 * multiple processes logging to the same file.
 */
class OW_COMMON_API MultiProcessFileAppender : public LogAppender
{
public:
	static const unsigned int NO_MAX_LOG_SIZE = 0;

	/**
	* All processes that log to @a filename must use
	* the same values for @a maxFileSize and @a maxBackupIndex.
	*/
	MultiProcessFileAppender(
		const StringArray& components,
		const StringArray& categories,
		const String & filename,
		const String& pattern,
		UInt64 maxFileSize,
		UInt32 maxBackupIndex);

	virtual ~MultiProcessFileAppender();

protected:
	virtual void doProcessLogMessage(const String& formattedMessage, const LogMessage& message) const;

private:
	String m_filename;
	UInt64 m_maxFileSize;
	unsigned m_maxBackupIndex;
	mutable File m_log;
	mutable File m_lock;
};

} // end namespace OW_NAMESPACE

#endif


