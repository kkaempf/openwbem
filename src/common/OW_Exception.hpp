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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#ifndef OW_EXCEPTION_HPP_INCLUDE_GUARD_
#define OW_EXCEPTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_AutoPtr.hpp"
#if defined(OW_NON_THREAD_SAFE_EXCEPTION_HANDLING)
#include "OW_CommonFwd.hpp" // for Mutex
#endif
#include <iosfwd>
#include <exception>
#include <new>

namespace OpenWBEM
{

/**
 * This class is the base of all exceptions thrown by OpenWBEM code.
 * It cannot be constructed directly, only subclasses can be instantiated.
 *
 * Creating a correct subclass is facilitated by the macros defined in
 * this file.
 *
 * Exceptions should be thrown by using one of the OW_THROW_* macros which
 * inserts the file and line number among other helpful things.
 */
class OW_COMMON_API Exception : public std::exception
{
protected:
	/**
	 * This constructor is deprecated
	 */
	Exception(const char* file, int line, const char* msg) OW_DEPRECATED; // in 3.1.0
	/**
	 * @param file The filename of the code throwing the exception.
	 * @param line The line of the code throwing the exception.
	 * @param msg A human readable message associated with the exception.
	 * @param errorCode A number identifying the specific error that caused
	 *   this exception.
	 * @param otherException A pointer to another exception.  This allows for
	 *   exception chaining, presumably otherException is the underlying cause
	 *   that the current exception is being thrown.  If otherException != 0,
	 *   The new Exception instance will store and delete the result of
	 *   calling otherException->clone(). otherException will not be saved.
	 * @param subClassId Each subclass of Exception may create an id.
	 *   getSubClassId() will return this value.  You may pass
	 *   UNKNOWN_SUBCLASS_ID if no id is required.  OW Ids are be registered
	 *   in OW_ExceptionIds.hpp.
	 */
	Exception(const char* file, int line, const char* msg, int errorCode, const Exception* otherException = 0, int subClassId = UNKNOWN_SUBCLASS_ID);

	Exception(int subClassId, const char* file, int line, const char* msg, int errorCode,
		const Exception* otherException = 0) OW_DEPRECATED; // in 3.1.0 -
		// Had to change the order of the parameters so the subClassId could be
		// defaulted and the subclasses' signature could match the base class signature.

#ifdef OW_WIN32
	// Can't seem to catch exceptions by reference if copy CTOR is
	// protected on Windoz
public:
#endif
	Exception(const Exception& e);
	Exception& operator= (const Exception& rhs);
#ifdef OW_WIN32
protected:
#endif
	void swap(Exception& x);
	virtual ~Exception() throw();
	void setSubClassId(int subClassId);
	void setErrorCode(int errorCode);

public:

	static const int UNKNOWN_SUBCLASS_ID = -1;
	static const int UNKNOWN_ERROR_CODE = -1;

	/**
	 * Returns a string representing the concrete type.  e.g. "SocketException".  Will not return 0.
	 * This function will not throw.  Derived class implementations must not throw.
	 */
	virtual const char* type() const;
	/**
	 * Returns the message.  May return 0.
	 * This function will not throw.  Derived class implementations must not throw.
	 */
	virtual const char* getMessage() const;
	/**
	 * Returns the file.  May return 0.
	 */
	const char* getFile() const;
	int getLine() const;
	int getSubClassId() const;
	/**
	 * Returns the sub exception if available, otherwise 0.
	 */
	const Exception* getSubException() const;
	/**
	 * Returns the error code representing the error which occurred.
	 * Code are unique only in the scope of the derived exception class.
	 * May return UNKNONWN_ERROR_CODE if the error is unavailable.
	 */
	int getErrorCode() const;

	/**
	 * Returns getMessage()
	 */
	virtual const char* what() const throw();

	/**
	 * Make a copy of this exception object. If allocation fails, return 0.
	 * Subclasses need to override this function.  This function must not
	 * throw.  Here is an example of how to implement this in a derived class:
	 *
	 * virtual DerivedException* clone() const
	 * {
	 *     return new(std::nothrow) DerivedException(*this);
	 * }
	 *
	 */
	virtual Exception* clone() const;

private:
	char* m_file;
	int m_line;
	char* m_msg;
	int m_subClassId;
	const Exception* m_subException;
	int m_errorCode;

#if defined(OW_NON_THREAD_SAFE_EXCEPTION_HANDLING)
	static Mutex* m_mutex;
#endif

};

namespace ExceptionDetail
{

	// Reconciles GNU strerror_r and POSIX strerror_r.
	//
	void portable_strerror_r(int errnum, char * buf, unsigned n);

	class FormatMsgImpl;

	class FormatMsg
	{
		AutoPtr<FormatMsgImpl> pImpl;
	public:
		FormatMsg(char const * msg, int errnum);
		~FormatMsg();
		char const * get() const;
	private:
		FormatMsg(const FormatMsg&);
		FormatMsg& operator=(const FormatMsg&);
	};

	unsigned const BUFSZ = 1024;

	template <typename exType>
	struct Errno
	{
		static exType simple(char const * file, int line, int errnum)
		{
			char buf[BUFSZ];
			portable_strerror_r(errnum, buf, BUFSZ);
			return exType(file, line, buf, errnum);
		}

		template <typename Mtype>
		static exType format(char const * file, int line,
		                     Mtype const & msg, int errnum)
		{
			return format(file, line, msg.c_str(), errnum);
		}

		static exType format(char const * file, int line,
		                     char const * msg, int errnum)
		{
			FormatMsg fm(msg, errnum);
			return exType(file, line, fm.get());
		}
	}; // struct Errno

} // namespace ExceptionDetail

/**
 * Writes the exception object to the stream in the form:
 *  <file>: <line> <type>: <message>
 */
OW_COMMON_API std::ostream& operator<< (std::ostream& os, const Exception& e);

/**
 * Throw an exception using __FILE__ and __LINE__.  If applicable,
 * OW_THROW_ERR should be used instead of this macro.
 *
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 */
#define OW_THROW(exType, msg) throw exType(__FILE__, __LINE__, (msg))

/**
 * This macro is deprecated in 3.1.0.
 */
#define OW_THROWL(exType, line, msg) throw exType(__FILE__, (line), (msg))

/**
 * Throw an exception using __FILE__ and __LINE__.
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 * @param subex A sub-exception. A pointer to it will be passed to the
 *   exception constructor, which should clone() it.
 */
#define OW_THROW_SUBEX(exType, msg, subex) \
throw exType(__FILE__, __LINE__, (msg), \
             ::OpenWBEM::Exception::UNKNOWN_ERROR_CODE, &(subex))

/**
 * Throw an exception using __FILE__ and __LINE__.
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 * @param err The error code.
 */
#define OW_THROW_ERR(exType, msg, err) \
throw exType(__FILE__, __LINE__, (msg), (err))

/**
 * Throw an exception using __FILE__, __LINE__, errno and strerror(errno)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO(exType) OW_THROW_ERRNO1(exType, errno)

/**
 * Throw an exception using __FILE__, __LINE__, errnum and strerror(errnum)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO1(exType, errnum) \
throw ExceptionDetail::Errno< exType >::simple(__FILE__, __LINE__, (errnum))

/**
 * Throw an exception using __FILE__, __LINE__, errno and strerror(errno)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO_MSG(exType, msg) \
OW_THROW_ERRNO_MSG1(exType, (msg), errno)

/**
 * Throw an exception using __FILE__, __LINE__, errnum and strerror(errnum)
 * @param exType The type of the exception; ctor must take file, line,
 *               message, and error code.
 */
#define OW_THROW_ERRNO_MSG1(exType, msg, errnum) \
throw ExceptionDetail::Errno< exType >:: \
      format(__FILE__, __LINE__, (msg), (errnum))

/**
 * Throw an exception using __FILE__ and __LINE__.
 * @param exType The type of the exception
 * @param msg The exception message.  A string that will be copied.
 * @param err The error code.
 * @param subex A sub-exception. A point to it will be passed to the
 *   exception constructor, which should clone() it.
 */
#define OW_THROW_ERR_SUBEX(exType, msg, err, subex) \
throw exType(__FILE__, __LINE__, (msg), (err), &(subex))

/**
 * Declare a new exception class named <NAME>Exception that derives from <BASE>.
 * This macro is typically used in a header file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 */
#define OW_DECLARE_EXCEPTION2(NAME, BASE) \
class NAME##Exception : public BASE \
{ \
public: \
	NAME##Exception(const char* file, int line, const char* msg, int errorCode = ::OpenWBEM::Exception::UNKNOWN_ERROR_CODE, const Exception* otherException = 0, int subClassId = ::OpenWBEM::Exception::UNKNOWN_SUBCLASS_ID); \
	virtual ~NAME##Exception() throw(); \
	virtual const char* type() const; \
	virtual NAME##Exception* clone() const; \
};

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
class LINKAGE_SPEC NAME##Exception : public BASE \
{ \
public: \
	NAME##Exception(const char* file, int line, const char* msg, int errorCode = ::OpenWBEM::Exception::UNKNOWN_ERROR_CODE, const Exception* otherException = 0, int subClassId = ::OpenWBEM::Exception::UNKNOWN_SUBCLASS_ID); \
	virtual ~NAME##Exception() throw(); \
	virtual const char* type() const; \
	virtual NAME##Exception* clone() const; \
};




/**
 * Declare a new exception class named <NAME>Exception that derives from Exception
 * This macro is typically used in a header file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 */
#define OW_DECLARE_EXCEPTION(NAME) OW_DECLARE_EXCEPTION2(NAME, ::OpenWBEM::Exception)

/**
 * Declare a new exception class named <NAME>Exception that derives from Exception
 * This macro is typically used in a header file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param LINKAGE_SPEC the linkage specifier. If the OW_DEFINE_EXCEPTION is part
 *		of libopenwbem this would OW_COMMON_API...
 */
#define OW_DECLARE_APIEXCEPTION(NAME, LINKAGE_SPEC) OW_DECLARE_APIEXCEPTION2(NAME, ::OpenWBEM::Exception, LINKAGE_SPEC)

/**
 * Define a new exception class named <NAME>Exception that derives from <BASE>.
 * The new class will use UNKNOWN_SUBCLASS_ID for the subclass id.
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 */
#define OW_DEFINE_EXCEPTION2(NAME, BASE) \
NAME##Exception::NAME##Exception(const char* file, int line, const char* msg, int errorCode, const ::OpenWBEM::Exception* otherException, int subClassId) \
	: BASE(file, line, msg, errorCode, otherException, subClassId) {} \
NAME##Exception::~NAME##Exception() throw() { } \
NAME##Exception* NAME##Exception::clone() const { return new(std::nothrow) NAME##Exception(*this); } \
const char* NAME##Exception::type() const { return #NAME "Exception"; }

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
NAME##Exception::NAME##Exception(const char* file, int line, const char* msg, int errorCode, const ::OpenWBEM::Exception* otherException, int subClassId) \
	: BASE(file, line, msg, errorCode, otherException, subClassId == ::OpenWBEM::Exception::UNKNOWN_SUBCLASS_ID ? (SUB_CLASS_ID) : subClassId) {} \
NAME##Exception::~NAME##Exception() throw() { } \
NAME##Exception* NAME##Exception::clone() const { return new(std::nothrow) NAME##Exception(*this); } \
const char* NAME##Exception::type() const { return #NAME "Exception"; }

/**
 * Define a new exception class named <NAME>Exception that derives from Exception.
 * The new class will use UNKNOWN_SUBCLASS_ID for the subclass id.
 * Use this macro for internal implementation exceptions that don't have an id.
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 */
#define OW_DEFINE_EXCEPTION(NAME) OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID_AUX(NAME, ::OpenWBEM::Exception, ::OpenWBEM::Exception::UNKNOWN_SUBCLASS_ID)

/**
 * Define a new exception class named <NAME>Exception that derives from Exception.
 * The new class will use ExceptionIds::<NAME>ExceptionId for the subclass id.
 * Use this macro to create public exceptions that have an id in the OpenWBEM::ExceptionIds namespace that derive from Exception
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 */
#define OW_DEFINE_EXCEPTION_WITH_ID(NAME) OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID_AUX(NAME, ::OpenWBEM::Exception, ::OpenWBEM::ExceptionIds::NAME##ExceptionId)

/**
 * Define a new exception class named <NAME>Exception that derives from <BASE>.
 * The new class will use ExceptionIds::<NAME>ExceptionId for the subclass id.
 * Use this macro to create public exceptions that have an id in the OpenWBEM::ExceptionIds namespace that will derive from BASE
 * This macro is typically used in a cpp file.
 *
 * @param NAME The name of the new class (Exception will be postfixed)
 * @param BASE The base class.
 */
#define OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID(NAME, BASE) OW_DEFINE_EXCEPTION_WITH_BASE_AND_ID_AUX(NAME, BASE, ::OpenWBEM::ExceptionIds::NAME##ExceptionId)

} // end namespace OpenWBEM

#endif
