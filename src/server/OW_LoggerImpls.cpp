/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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

extern "C"
{
#include <syslog.h>
#include <unistd.h> // for getpid
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

using std::ofstream;
using std::endl;


class OW_FileLogger : public OW_Logger
{
	public:
		OW_FileLogger( const char* filename ) :
			log( filename, std::ios::out|std::ios::app )
		{
			if (!log)
				OW_THROW(OW_Exception, format("OW_FileLogger: Unable to open file: %1", filename).toString().c_str() );
		}
		virtual ~OW_FileLogger() {}

	protected:
		virtual void doLogMessage( const OW_String& s,
			const OW_LogLevel /*level*/ ) const
		{
#ifdef OW_USE_GNU_PTH
            pth_yield(NULL);
#endif
			OW_DateTime DateTime;
			DateTime.setToCurrent();
			// FIXME: Need to remove newline after DateTime.
			log << DateTime.toString() << s << endl;
		}

	private:
		mutable ofstream log;
};

class OW_SyslogLogger : public OW_Logger
{
	public:
		OW_SyslogLogger()
		{
			static OW_Bool calledOpenLog = false;
			if (!calledOpenLog)
			{
				openlog( "openwbem", LOG_CONS, LOG_DAEMON );
				calledOpenLog = true;
			}
		}
		virtual ~OW_SyslogLogger() {}

	protected:
		virtual void doLogMessage( const OW_String& s, const OW_LogLevel level ) const
		{
#ifdef OW_USE_GNU_PTH
            pth_yield(NULL);
#endif
			int syslogPriority;
			switch( level )
			{
				case ErrorLevel:
					syslogPriority = LOG_ERR;
					break;
				case CustInfoLevel:
					syslogPriority = LOG_INFO;
					break;
				case DebugLevel:
				default:
					syslogPriority = LOG_DEBUG;
					break;
			}
			OW_StringArray a = s.tokenize("\n");
			for (size_t i = 0; i < a.size(); ++i)
				syslog( syslogPriority, "%s", a[i].c_str() );
		}
};

class OW_TeeLogger : public OW_Logger
{
	public:
		OW_TeeLogger( OW_LoggerRef const& first, OW_LoggerRef const& second ):
			m_first(first), m_second(second)
		{
			m_first->setLogLevel(DebugLevel);
			m_second->setLogLevel(DebugLevel);
		}
		virtual ~OW_TeeLogger()
		{
		}

	protected:
		virtual void doLogMessage( const OW_String& s,
			const OW_LogLevel level) const
		{
			switch (level)
			{
				case ErrorLevel:
					m_first->logError(s);
					m_second->logError(s);
					break;
				case DebugLevel:
					m_first->logDebug(s);
					m_second->logDebug(s);
					break;
				case CustInfoLevel:
					m_first->logCustInfo(s);
					m_second->logCustInfo(s);
					break;
				default:
					break;
			}
		}

	private:
		OW_LoggerRef m_first;
		OW_LoggerRef m_second;
};

class OW_CerrLogger : public OW_Logger
{
	public:
		virtual ~OW_CerrLogger(){}

	protected:
		virtual void doLogMessage( const OW_String& s,
			const OW_LogLevel /*level*/ ) const
		{
#ifdef OW_USE_GNU_PTH
            pth_yield(NULL);
#endif
			std::cerr << '[' << getpid() << "] " << s << std::endl;
		}
};

class OW_NullLogger : public OW_Logger
{
public:
	virtual ~OW_NullLogger() {}

protected:
	virtual void doLogMessage(const OW_String &, 
		const OW_LogLevel) const 
	{
	}
};

OW_LoggerRef OW_Logger::createLogger( const OW_String& type, const OW_Bool& debug )
{
	OW_LoggerRef retval;
	if (type.empty())
		retval = new OW_NullLogger;
	else if ( type == "syslog" )
		retval = new OW_SyslogLogger;
	else
		retval = new OW_FileLogger( type.c_str() );

	if ( debug )
	{
		retval = new OW_TeeLogger( retval, OW_LoggerRef(new OW_CerrLogger) );
	}

	return retval;
}


