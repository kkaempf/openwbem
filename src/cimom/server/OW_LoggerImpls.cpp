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
#include "OW_config.h"
#include "OW_Logger.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_DateTime.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"

#include <fstream>
#include <iostream> // for cerr
#include <syslog.h>
#include <unistd.h> // for getpid

namespace OpenWBEM
{

using std::ofstream;
using std::endl;
class FileLogger : public Logger
{
	public:
		FileLogger( const char* filename ) :
			log( filename, std::ios::out|std::ios::app )
		{
			if (!log)
				OW_THROW(LoggerException, Format("FileLogger: Unable to open file: %1", filename).toString().c_str() );
		}
		virtual ~FileLogger() {}
	protected:
		virtual void doLogMessage( const String& s,
			const ELogLevel /*level*/ ) const
		{
			DateTime DateTime;
			DateTime.setToCurrent();
			// FIXME: Need to remove newline after DateTime.
			log << DateTime.toString() << s << endl;
		}
	private:
		mutable ofstream log;
};
class SyslogLogger : public Logger
{
	public:
		SyslogLogger()
		{
			if (!calledOpenLog)
			{
				openlog( OW_PACKAGE_PREFIX"openwbem", LOG_CONS, LOG_DAEMON );
				calledOpenLog = true;
			}
		}
		virtual ~SyslogLogger() {}
	protected:
		virtual void doLogMessage( const String& s, const ELogLevel level ) const
		{
			int syslogPriority;
			switch( level )
			{
				case E_FATAL_ERROR_LEVEL:
					syslogPriority = LOG_CRIT;
					break;
				case E_ERROR_LEVEL:
					syslogPriority = LOG_ERR;
					break;
				case E_INFO_LEVEL:
					syslogPriority = LOG_INFO;
					break;
				case E_DEBUG_LEVEL:
				default:
					syslogPriority = LOG_DEBUG;
					break;
			}
			StringArray a = s.tokenize("\n");
			for (size_t i = 0; i < a.size(); ++i)
				syslog( syslogPriority, "%s", a[i].c_str() );
		}
		static bool calledOpenLog;
};
bool SyslogLogger::calledOpenLog = false;

class TeeLogger : public Logger
{
	public:
		TeeLogger( LoggerRef const& first, LoggerRef const& second ):
			m_first(first), m_second(second)
		{
			m_first->setLogLevel(E_DEBUG_LEVEL);
			m_second->setLogLevel(E_DEBUG_LEVEL);
		}
		virtual ~TeeLogger()
		{
		}
	protected:
		virtual void doLogMessage( const String& s,
			const ELogLevel level) const
		{
			switch (level)
			{
				case E_FATAL_ERROR_LEVEL:
					m_first->logFatalError(s);
					m_second->logFatalError(s);
					break;
				case E_ERROR_LEVEL:
					m_first->logError(s);
					m_second->logError(s);
					break;
				case E_DEBUG_LEVEL:
					m_first->logDebug(s);
					m_second->logDebug(s);
					break;
				case E_INFO_LEVEL:
					m_first->logInfo(s);
					m_second->logInfo(s);
					break;
				default:
					break;
			}
		}
	private:
		LoggerRef m_first;
		LoggerRef m_second;
};
class CerrLogger : public Logger
{
	public:
		virtual ~CerrLogger(){}
	protected:
		virtual void doLogMessage( const String& s,
			const ELogLevel /*level*/ ) const
		{
			std::cerr << '[' << getpid() << "] " << s << std::endl;
		}
};
class NullLogger : public Logger
{
public:
	virtual ~NullLogger() {}
protected:
	virtual void doLogMessage(const String &,
		const ELogLevel) const
	{
	}
};
LoggerRef Logger::createLogger( const String& type, bool debug )
{
	LoggerRef retval;
	if (type.empty() || type.equalsIgnoreCase("null"))
		retval = new NullLogger;
	else if ( type == "syslog" )
		retval = new SyslogLogger;
	else
		retval = new FileLogger( type.c_str() );
	if ( debug )
	{
		retval = new TeeLogger( retval, LoggerRef(new CerrLogger) );
	}
	return retval;
}

} // end namespace OpenWBEM

