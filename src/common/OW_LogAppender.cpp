/*******************************************************************************
* Copyright (C) 2003-2005 Quest Software, Inc. All rights reserved.
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
#include "OW_MultiProcessFileAppender.hpp"
#include "OW_Format.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_SortedVectorMap.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_ThreadOnce.hpp"
#include "OW_NullLogger.hpp"
#include "OW_GlobalPtr.hpp"


namespace OW_NAMESPACE
{


//////////////////////////////////////////////////////////////////////////////
LogAppender::~LogAppender()
{
}

/////////////////////////////////////////////////////////////////////////////
// we're passing a pointer to this to pthreads, it has to have C linkage.
extern "C" 
{
static void freeThreadLogAppender(void *ptr)
{
	delete static_cast<LogAppenderRef *>(ptr);
}
} // end extern "C"

/////////////////////////////////////////////////////////////////////////////
namespace
{

OnceFlag g_onceGuard  = OW_ONCE_INIT;
NonRecursiveMutex* g_mutexGuard = NULL;
pthread_key_t g_loggerKey; // TODO: port me

struct NullAppenderFactory
{
	static LogAppenderRef* create()
	{
		return new LogAppenderRef(new NullAppender());
	}
};
::OW_NAMESPACE::GlobalPtr<LogAppenderRef,NullAppenderFactory> g_defaultLogAppender = OW_GLOBAL_PTR_INIT;


/////////////////////////////////////////////////////////////////////////////
void initGuardAndKey()
{
	g_mutexGuard = new NonRecursiveMutex();
	int ret = pthread_key_create(&g_loggerKey, freeThreadLogAppender);
	OW_ASSERTMSG(ret == 0, "failed create a thread specific key");
}


} // end unnamed namespace

/////////////////////////////////////////////////////////////////////////////
// STATIC
LogAppenderRef
LogAppender::getCurrentLogAppender()
{
	LogAppenderRef threadLogAppender = getThreadLogAppender();
	if(threadLogAppender)
	{
		return threadLogAppender;
	}
	else
	{
		return getDefaultLogAppender();
	}
}

/////////////////////////////////////////////////////////////////////////////
// STATIC
LogAppenderRef
LogAppender::getDefaultLogAppender()
{
	callOnce(g_onceGuard, initGuardAndKey);
	NonRecursiveMutexLock lock(*g_mutexGuard);

	// This looks unsafe, but the get() method (called indirectly by operator*),
	// if it has never been previously called, will allocate a new
	// LogAppenderRef wich will have a NullAppender inside it.
	return *g_defaultLogAppender;
}


/////////////////////////////////////////////////////////////////////////////
// STATIC
bool
LogAppender::setDefaultLogAppender(const LogAppenderRef &ref)
{
	if (ref)
	{
		callOnce(g_onceGuard, initGuardAndKey);
		NonRecursiveMutexLock lock(*g_mutexGuard);

		LogAppenderRef(ref).swap(*g_defaultLogAppender);
		return true;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// STATIC
LogAppenderRef
LogAppender::getThreadLogAppender()
{
	callOnce(g_onceGuard, initGuardAndKey);
	LogAppenderRef *ptr = static_cast<LogAppenderRef *>(pthread_getspecific(g_loggerKey));
	if(ptr)
	{
		return *ptr;
	}
	else
	{
		return LogAppenderRef();
	}
}

/////////////////////////////////////////////////////////////////////////////
// STATIC
bool
LogAppender::setThreadLogAppender(const LogAppenderRef &ref)
{
	callOnce(g_onceGuard, initGuardAndKey);
	LogAppenderRef *ptr = 0;
	if (ref)
	{
		ptr = new LogAppenderRef(ref);
	}
	freeThreadLogAppender(pthread_getspecific(g_loggerKey));

	int ret = pthread_setspecific(g_loggerKey, ptr);
	if (ret != 0)
	{
		delete ptr;
	}
	OW_ASSERTMSG(ret == 0, "failed to set a thread specific logger");
	return (ref != 0);
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

/////////////////////////////////////////////////////////////////////////////
namespace
{
	String
	getConfigItem(const LogAppender::ConfigMap& configItems, const String &itemName, const String& defRetVal = "")
	{
		LogAppender::ConfigMap::const_iterator i = configItems.find(itemName);
		if (i != configItems.end())
		{
			return i->second;
		}
		else
		{
			return defRetVal;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
LogAppenderRef
LogAppender::createLogAppender(
	const String& name,
	const StringArray& components,
	const StringArray& categories,
	const String& messageFormat,
	const String& type,
	const ConfigMap& configItems)
{
	LogAppenderRef appender;
	if (type.empty() || type.equalsIgnoreCase(TYPE_NULL))
	{
		appender = new NullAppender(components, categories, messageFormat);
	}
#ifndef OW_WIN32
	else if ( type == TYPE_SYSLOG )
	{
		appender = new SyslogAppender(components, categories, messageFormat);
	}
#endif
	else if (type == TYPE_STDERR || type == "cerr")
	{
		appender = new CerrAppender(components, categories, messageFormat);
	}
	else if (type == TYPE_FILE || type == TYPE_MPFILE)
	{
		String configItem = Format(ConfigOpts::LOG_1_LOCATION_opt, name);
		String filename = getConfigItem(configItems, configItem);
		
		UInt64 maxFileSize(0);
		try
		{
			maxFileSize = getConfigItem(configItems, Format(ConfigOpts::LOG_1_MAX_FILE_SIZE_opt, name), 
				OW_DEFAULT_LOG_1_MAX_FILE_SIZE).toUInt64();
		}
		catch (StringConversionException& e)
		{
			OW_THROW_ERR_SUBEX(LoggerException, 
				Format("%1: Invalid config value: %2", ConfigOpts::LOG_1_MAX_FILE_SIZE_opt, e.getMessage()).c_str(), 
				Logger::E_INVALID_MAX_FILE_SIZE, e);
		}
		
		unsigned int maxBackupIndex(0);
		try
		{
			maxBackupIndex = getConfigItem(configItems, Format(ConfigOpts::LOG_1_MAX_BACKUP_INDEX_opt, name), 
				OW_DEFAULT_LOG_1_MAX_BACKUP_INDEX).toUnsignedInt();
		}
		catch (StringConversionException& e)
		{
			OW_THROW_ERR_SUBEX(LoggerException, 
				Format("%1: Invalid config value: %2", ConfigOpts::LOG_1_MAX_BACKUP_INDEX_opt, e.getMessage()).c_str(), 
				Logger::E_INVALID_MAX_BACKUP_INDEX, e);
		}

		if (type == TYPE_FILE)
		{
			bool flushLog =
				getConfigItem(
					configItems,
					Format(ConfigOpts::LOG_1_FLUSH_opt, name),
					OW_DEFAULT_LOG_1_FLUSH
				).equalsIgnoreCase("true");
			appender = new FileAppender(
				components, categories, filename.c_str(), messageFormat,
				maxFileSize, maxBackupIndex, flushLog
			);
		}
		else // type == TYPE_MPFILE
		{
			appender = new MultiProcessFileAppender(
				components, categories, filename, messageFormat,
				maxFileSize, maxBackupIndex
			);
		}
	}
	else
	{
		OW_THROW_ERR(LoggerException, Format("Unknown log type: %1", type).c_str(), Logger::E_UNKNOWN_LOG_APPENDER_TYPE);
	}

	return appender;
}

//////////////////////////////////////////////////////////////////////////////
const StringArray LogAppender::ALL_COMPONENTS(String("*").tokenize());
const StringArray LogAppender::ALL_CATEGORIES(String("*").tokenize());
const String LogAppender::STR_TTCC_MESSAGE_FORMAT("%r [%t] %-5p %c - %m");
const String LogAppender::TYPE_SYSLOG("syslog");
const String LogAppender::TYPE_STDERR("stderr");
const String LogAppender::TYPE_FILE("file");
const String LogAppender::TYPE_MPFILE("mpfile");
const String LogAppender::TYPE_NULL("null");

//////////////////////////////////////////////////////////////////////////////
LogAppender::LogAppender(const StringArray& components, const StringArray& categories, const String& pattern)
	: m_components(components.begin(), components.end())
	, m_categories(categories.begin(), categories.end())
	, m_formatter(pattern)
	, m_logLevel(E_NONE_LEVEL)
{
	m_allComponents = m_components.count("*") > 0;
	m_allCategories = m_categories.count("*") > 0;

	// set up the log level
	size_t numCategories = m_categories.size();
	size_t debugCount = m_categories.count(Logger::STR_DEBUG_CATEGORY);
	size_t infoCount = m_categories.count(Logger::STR_INFO_CATEGORY);
	size_t errorCount = m_categories.count(Logger::STR_ERROR_CATEGORY);
	size_t fatalCount = m_categories.count(Logger::STR_FATAL_CATEGORY);
	int nonLevelCategoryCount = numCategories - debugCount - infoCount - errorCount - fatalCount;

	if (numCategories == 0)
	{
		m_logLevel = E_NONE_LEVEL;
	}
	else if (m_allCategories || nonLevelCategoryCount > 0)
	{
		m_logLevel = E_ALL_LEVEL;
	}
	else if (debugCount > 0)
	{
		m_logLevel = E_DEBUG_LEVEL;
	}
	else if (infoCount > 0)
	{
		m_logLevel = E_INFO_LEVEL;
	}
	else if (errorCount > 0)
	{
		m_logLevel = E_ERROR_LEVEL;
	}
	else if (fatalCount > 0)
	{
		m_logLevel = E_FATAL_ERROR_LEVEL;
	}
	else
	{
		OW_ASSERTMSG(0, "Internal error. LogAppender unable to determine log level!");
	}
}



} // end namespace OW_NAMESPACE


