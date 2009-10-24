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




#ifndef OW_Exception_HPP_INCLUDE_GUARD_
#define OW_Exception_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "blocxx/Exception.hpp"

/**
 * Throw an exception using __FILE__ and __LINE__.  If applicable,
 * OW_THROW_ERR should be used instead of this macro.
 *
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 */
#define OW_THROW(exType, msg) \
			BLOCXX_THROW(exType, msg)

/**
 * This macro is deprecated in 3.1.0.
 */
#define OW_THROWL(exType, line, msg) \
			BLOCXX_THROWL(exType, line, msg)

/**
 * Throw an exception using __FILE__ and __LINE__.
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 * @param subex A sub-exception. A pointer to it will be passed to the
 *   exception constructor, which should clone() it.
 */
#define OW_THROW_SUBEX(exType, msg, subex) \
		BLOCXX_THROW_SUBEX(exType, msg, subex)

/**
 * Throw an exception using __FILE__ and __LINE__.
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 * @param err The error code.
 */
#define OW_THROW_ERR(exType, msg, err) \
		BLOCXX_THROW_ERR(exType, msg, err)

/**
 * Throw an exception using __FILE__, __LINE__, errno and strerror(errno)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO(exType) \
			BLOCXX_THROW_ERRNO(exType)

/**
 * Throw an exception using __FILE__, __LINE__, errnum and strerror(errnum)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO1(exType, errnum) \
			BLOCXX_THROW_ERRNO1(exType, errnum)

/**
 * Throw an exception using __FILE__, __LINE__, errno and strerror(errno)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO_MSG(exType, msg) \
			BLOCXX_THROW_ERRNO_MSG(exType, msg)

/**
 * Throw an exception using __FILE__, __LINE__, errnum and strerror(errnum)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO_MSG1(exType, msg, errnum) \
			BLOCXX_THROW_ERRNO_MSG1(exType, msg, errnum)

/**
 * Throw an exception using __FILE__ and __LINE__.
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 * @param err The error code.
 * @param subex A sub-exception. A point to it will be passed to the
 *   exception constructor, which should clone() it.
 */
#define OW_THROW_ERR_SUBEX(exType, msg, err, subex) \
			BLOCXX_THROW_ERR_SUBEX(exType, msg, err, subex)

/**
 * Declare a new exception class named <NAME>Exception that derives from <BASE>.
 * This macro is typically used in a header file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 */
#define OW_DECLARE_EXCEPTION2(NAME, BASE) \
			BLOCXX_DECLARE_EXCEPTION2(NAME, BASE)

/**
 * Declare a new exception class named <NAME>Exception that derives from <BASE>.
 * This macro is typically used in a header file. The exception class
 * declaration will be prefaced with the linkage_spec parm. This allows
 * the use of OW_COMMON_API when declaring exceptions. Example:
 * 		OW_DECLARE_APIEXCEPTION(Bogus, CIMException, OW_COMMON_API)
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 * @param LINKAGE_SPEC The linkage specifier for the exception class.
 */
#define OW_DECLARE_APIEXCEPTION2(NAME, BASE, LINKAGE_SPEC) \
		BLOCXX_DECLARE_APIEXCEPTION2(NAME, BASE, LINKAGE_SPEC)



/**
 * Declare a new exception class named <NAME>Exception that derives from Exception
 * This macro is typically used in a header file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 */
#define OW_DECLARE_EXCEPTION(NAME) \
			BLOCXX_DECLARE_EXCEPTION(NAME)

/**
 * Declare a new exception class named <NAME>Exception that derives from Exception
 * This macro is typically used in a header file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param LINKAGE_SPEC the linkage specifier. If the OW_DEFINE_EXCEPTION is part
 *		of libopenwbem this would OW_COMMON_API...
 */
#define OW_DECLARE_APIEXCEPTION(NAME, LINKAGE_SPEC) \
			BLOCXX_DECLARE_APIEXCEPTION(NAME, LINKAGE_SPEC)

/**
 * Define a new exception class named <NAME>Exception that derives from <BASE>.
 * The new class will use UNKNOWN_SUBCLASS_ID for the subclass id.
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 */
#define OW_DEFINE_EXCEPTION2(NAME, BASE) \
			BLOCXX_DEFINE_EXCEPTION2(NAME, BASE)

/**
 * Define a new exception class named <NAME>Exception that derives from <BASE>.
 * The new class will use SUB_CLASS_ID for the subclass id.
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 * @param SUB_CLASS_ID The subclass id.
 */
#define OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID_AUX(NAME, BASE, SUB_CLASS_ID) \
		BLOCXX_DEFINE_EXCEPTION_WITH_BASE_AND_ID_AUX(NAME, BASE, SUB_CLASS_ID)

/**
 * Define a new exception class named <NAME>Exception that derives from Exception.
 * The new class will use UNKNOWN_SUBCLASS_ID for the subclass id.
 * Use this macro for internal implementation exceptions that don't have an id.
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 */
#define OW_DEFINE_EXCEPTION(NAME) \
			BLOCXX_DEFINE_EXCEPTION(NAME)

/**
 * Define a new exception class named <NAME>Exception that derives from Exception.
 * The new class will use ExceptionIds::<NAME>ExceptionId for the subclass id.
 * Use this macro to create public exceptions that have an id in the OW_NAMESPACE::ExceptionIds namespace that derive from Exception
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 */
#define OW_DEFINE_EXCEPTION_WITH_ID(NAME) \
			BLOCXX_DEFINE_EXCEPTION_WITH_ID(NAME)

/**
 * Define a new exception class named <NAME>Exception that derives from <BASE>.
 * The new class will use ExceptionIds::<NAME>ExceptionId for the subclass id.
 * Use this macro to create public exceptions that have an id in the OW_NAMESPACE::ExceptionIds namespace that will derive from BASE
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 */
#define OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID(NAME, BASE) \
			BLOCXX_DEFINE_EXCEPTION_WITH_BASE_AND_ID(NAME, BASE)



#endif
