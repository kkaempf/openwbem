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

#ifndef OW_LOGGER_HPP_INCLUDE_GUARD_
#define OW_LOGGER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_LogLevel.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(Logger)

class Logger;
typedef IntrusiveReference<Logger> LoggerRef;
/**
 * Logging interface.
 */
class Logger : public IntrusiveCountableBase
{
	public:
		/**
		 * Log a fatal error.
		 * @param s The string to log.
		 */
		void logFatalError( const String& s ) const
			{  logMessage( E_FATAL_ERROR_LEVEL, s ); }
		/**
		 * Log an error.
		 * @param s The string to log.
		 */
		void logError( const String& s ) const
			{  logMessage( E_ERROR_LEVEL, s ); }
		/**
		 * Log customer info.
		 * @param s The string to log.
		 */
		void logInfo( const String& s ) const
			{  logMessage( E_INFO_LEVEL, s ); }
		/**
		 * Log debug info.
		 * @param s The string to log.
		 */
		void logDebug( const String& s ) const
			{  logMessage( E_DEBUG_LEVEL, s ); }
		/**
		 * Set the logging level.  All messages with priority > l will be logged.
		 * Valid values are: debug, info, error, fatalerror
		 * @param l The level of messages to log
		 */
		void setLogLevel( const ELogLevel l ) {  m_level = l; }
		void setLogLevel( const String& l );
		/**
		 * @return The current logging level
		 */
		ELogLevel getLogLevel() const {  return m_level; }
		/**
		 * Create a concrete logger depending on the type string passed in.
		 * On Linux, if type == "syslog" a logger the writes to the syslog
		 * will be returned.
		 * Otherwise type is treated as a filename and a logger that writes
		 * to that file will be returned.
		 * @param type The type of logger to create
		 * @param debug Indicates whether to create a debug logger (duplicates
		 *   			all messages to stdout)
		 * @return a class that implements the Logger interface.
		 */
		static LoggerRef createLogger( const String& type,
			bool debug );
		virtual ~Logger();
	protected:
		Logger() : m_level(E_ERROR_LEVEL) { }
		Logger( const ELogLevel l ) : m_level(l) {  }
		/**
		 * To be overridden by derived classes with a function that does the
		 * actual logging.
		 */
		virtual void doLogMessage( const String& message,
			const ELogLevel level) const = 0;
	private:
		void logMessage( const ELogLevel l, const String& s ) const;
	private: // data
		ELogLevel m_level;
};

} // end namespace OpenWBEM

typedef OpenWBEM::Logger OW_Logger OW_DEPRECATED; // in 3.0.0
typedef OpenWBEM::LoggerRef OW_LoggerRef OW_DEPRECATED; // in 3.0.0

#endif
