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

#ifndef OW_LOG_APPENDER_IFC_HPP_INCLUDE_GUARD_
#define OW_LOG_APPENDER_IFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_SortedVectorSet.hpp"
#include "OW_LogLevel.hpp"
#include "OW_LogMessagePatternFormatter.hpp"


namespace OpenWBEM
{

#ifdef OW_WIN32
template class OW_COMMON_API Array<String>;
template class OW_COMMON_API SortedVectorSet<String>;
#endif

class OW_COMMON_API LogAppender : public IntrusiveCountableBase
{
public:

	virtual ~LogAppender();

	/**
	 * Log a message using the specified component and category
	 * @param message The message to log
	 */
	void logMessage(const LogMessage& message) const;

	bool categoryIsEnabled(const String& category) const;
	bool componentAndCategoryAreEnabled(const String& component, const String& category) const;

	ELogLevel getLogLevel() const;

	/**
	 * Create a concrete log appender depending on the type string passed in.
	 * If type == "syslog" a logger the writes to the syslog
	 * will be returned.
	 * If type == "" || type == "null" a logger that doesn't do anything
	 * will be returned.
	 * If type == "stderr" a logger that writes to stderr will be returned.
	 * Otherwise type is treated as a filename and a logger that writes
	 * to that file will be returned.
	 *
	 * @param name The name of the logger to create
	 * @param components The message components the logger will log.
	 *  "*" means all components.
	 * @param categories The message categories the logger will log.
	 *  "*" means all categories.
	 * @param type The type of logger to create
	 * @param configItems Additional config items the logger may use for
	 *  configuration.
	 * @return a class that implements the Logger interface.
	 * @throws LoggerException - E_UNKNOWN_LOG_APPENDER_TYPE - if type is unkonwn.
	 *                         - E_INVALID_MAX_FILE_SIZE     - if the max_file_size option is invalid
	 *                         - E_INVALID_MAX_BACKUP_INDEX  - if the max_backup_index option is invalid
	 */
	static LogAppenderRef createLogAppender(
		const String& name,
		const StringArray& components,
		const StringArray& categories,
		const String& messageFormat,
		const String& type,
		const ConfigFile::ConfigMap& configItems);

	/// Pass to createLogAppender to indicate all components.
	static const StringArray ALL_COMPONENTS;
	/// Pass to createLogAppender to indicate all categories.
	static const StringArray ALL_CATEGORIES;
	/// The Log4j TTCC message format - TTCC is acronym for Time Thread Category Component.
	/// "%r [%t] %-5p %c - %m"
	static const String STR_TTCC_MESSAGE_FORMAT;
	/// String of the type of the syslog log appender
	static const String TYPE_SYSLOG;
	/// String of the type of the stderr log appender
	static const String TYPE_STDERR;
	/// String of the type of the file log appender
	static const String TYPE_FILE;
	/// String of the type of the null log appender
	static const String TYPE_NULL;

protected:

	LogAppender(const StringArray& components, const StringArray& categories, const String& pattern);

private:
	virtual void doProcessLogMessage(const String& formattedMessage, const LogMessage& message) const = 0;

private: // data
	SortedVectorSet<String> m_components;
	bool m_allComponents;
	SortedVectorSet<String> m_categories;
	bool m_allCategories;

	LogMessagePatternFormatter m_formatter;

};

} // end namespace OpenWBEM

#endif


