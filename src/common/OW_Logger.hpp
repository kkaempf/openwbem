/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* Copyright (C) 2006, Novell, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of
*       Quest Software, Inc.,
*       nor Novell, Inc.,
*       nor the names of its contributors or employees may be used to
*       endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author <see corresponding BloCxx header>
 */




#ifndef OW_Logger_HPP_INCLUDE_GUARD_
#define OW_Logger_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "blocxx/Logger.hpp"

namespace OW_NAMESPACE
{
   using namespace blocxx;
}

#define BLOCXX_LOGGER_PRETTY_FUNCTION BLOCXX_LOGGER_PRETTY_FUNCTION


/**
 * Log message to logger with the Debug3 level.  message is only evaluated if logger->getLogLevel() >=
 * E_DEBUG3_LEVEL __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define BLOCXX_LOG_DEBUG3(logger, message) BLOCXX_LOG_DEBUG3(logger, message)

/**
 * Log message to logger with the Debug2 level.  message is only evaluated if logger->getLogLevel() >=
 * E_DEBUG2_LEVEL __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define BLOCXX_LOG_DEBUG2(logger, message) BLOCXX_LOG_DEBUG2(logger, message)

/**
 * Log message to logger with the Debug level.  message is only evaluated if logger->getLogLevel() >= E_DEBUG_LEVEL
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define BLOCXX_LOG_DEBUG(logger, message) BLOCXX_LOG_DEBUG(logger, message)

/**
 * Log message to logger with the Info level.  message is only evaluated if logger->getLogLevel() >= E_INFO_LEVEL
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define BLOCXX_LOG_INFO(logger, message) \
			BLOCXX_LOG_INFO(logger, message)

/**
 * Log message to logger with the Error level.  message is only evaluated if logger->getLogLevel() >= E_ERROR_LEVEL
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define BLOCXX_LOG_ERROR(logger, message) \
			BLOCXX_LOG_ERROR(logger, message)

/**
 * Log message to logger with the FatalError level.  message is always evaluated.
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param message An expression that evaluates to a String which will be logged.
 */
#define BLOCXX_LOG_FATAL_ERROR(logger, message) \
			BLOCXX_LOG_FATAL_ERROR(logger, message)

/**
 * Log message to logger with the specified category.  message is only evaluated if logger->categoryIsEnabled(category) == true
 * __FILE__ and __LINE__ are logged.
 * @param logger The logger to use.
 * @param category The message category
 * @param message An expression that evaluates to a String which will be logged.
 */
#define BLOCXX_LOG(logger, category, message) \
			BLOCXX_LOG(logger, category, message)


#warning OW_Logger.hpp is deprecated, use "blocxx/Logger.hpp" instead. Replace all usages of \
BLOCXX_LOG* with BLOCXX_LOG*. \
The following bash command should do it for you. \
for file in `grep -IHrl "OW_Logger.hpp" *`; do sed -i -e 's/BLOCXX_LOG/BLOCXX_LOG/g' -e 's!OW_Logger.hpp!blocxx/Logger.hpp!g' $file; done

#endif
