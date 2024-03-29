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
 * @author Dan Nuffer
 */

#ifndef OW_SafeLibCreate_HPP_INCLUDE_GUARD_
#define OW_SafeLibCreate_HPP_INCLUDE_GUARD_



#include "OW_config.h"
#include "blocxx/SharedLibraryReference.hpp"
#include "blocxx/SharedLibrary.hpp"
#include "blocxx/SharedLibraryLoader.hpp"
#include "blocxx/Logger.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/SignalScope.hpp"
#include "OW_Exception.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/Assertion.hpp"

#include <utility> // for std::pair
#include <setjmp.h> // for sigsetjmp, siglongjmp and jmp_buf

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

// Note that using setjmp/longjmp will fail to reset the blocked signal mask after the first time this fails.
// using sigsetjmp/siglongjmp fixes this problem.
//
namespace OW_NAMESPACE
{
//////////////////////////////////////////////////////////////////////////////
template <typename T>
class SafeLibCreate
{
	typedef T* (*createFunc_t)();
	typedef const char* (*versionFunc_t)();
public:
	typedef std::pair<blocxx::IntrusiveReference<T>, blocxx::SharedLibraryRef> return_type;
	typedef blocxx::SharedLibraryReference<blocxx::IntrusiveReference<T> > return_obj;

	static return_type
	loadAndCreate(blocxx::String const& libname, blocxx::String const& createFuncName, blocxx::String const& version)
	{
		SharedLibraryLoaderRef sll = SharedLibraryLoader::createSharedLibraryLoader();
		SharedLibraryRef sl = sll->loadSharedLibrary(libname);
		T* ptr = 0;
		if ( sl )
		{
			ptr = create(sl, createFuncName, version);
		}
		else
		{
			Logger lgr("ow.SafeLibCreate");
			BLOCXX_LOG_DEBUG(lgr, Format("SafeLibCreate::loadAndCreate FAILED loading library %1", libname));
		}
		return std::make_pair(IntrusiveReference<T>(ptr),sl);
	}

	static return_obj
	loadAndCreateObject(blocxx::String const& libname,
		blocxx::String const& createFuncName, blocxx::String const& version)
	{
		SharedLibraryLoaderRef sll = SharedLibraryLoader::createSharedLibraryLoader();
		SharedLibraryRef sl = sll->loadSharedLibrary(libname);
		T* ptr = 0;
		if ( sl )
		{
			ptr = create(sl, createFuncName, version);
		}
		else
		{
			Logger lgr("ow.SafeLibCreate");
			BLOCXX_LOG_DEBUG(lgr, Format("SafeLibCreate::loadAndCreate FAILED loading library %1", libname));
		}
		return return_obj(sl, ptr);
	}

	static T*
	create(blocxx::SharedLibraryRef sl, blocxx::String const& createFuncName,
			blocxx::String const& version)
	{
		Logger logger("ow.SafeLibCreate");
		BLOCXX_LOG_DEBUG3(logger, Format("SafeLibCreate::create called.  createFuncName = %1", createFuncName).c_str());

		BLOCXX_ASSERT(sl);
		try
		{
			int sigtype;
			SignalScope r1( BLOCXX_SIGFPE,  theSignalHandler );
			SignalScope r3( BLOCXX_SIGSEGV, theSignalHandler );
#ifdef SIGBUS // NetWare doesn't have this signal
			SignalScope r4( BLOCXX_SIGBUS,  theSignalHandler );
#endif
			SignalScope r5( BLOCXX_SIGABRT, theSignalHandler );
#ifdef WIN32
			sigtype = setjmp(theLoaderBuf);
#else
			sigtype = sigsetjmp(theLoaderBuf, 1);
#endif
			if ( sigtype == 0 )
			{
				versionFunc_t versFunc;
				if (!sl->getFunctionPointer( "getOWVersion", versFunc))
				{
					BLOCXX_LOG_ERROR(logger, "SafeLibCreate::create failed getting function pointer to \"getOWVersion\" from library");

					return 0;
				}

				const char* strVer = 0;
				strVer = (*versFunc)();
				if (!strVer || version != strVer)
				{
					BLOCXX_LOG_INFO(logger, Format("SafeLibCreate::create - Warning: version returned from \"getOWVersion\" (%1) does not match (%2)",
						strVer ? strVer : "", version));
				}

				createFunc_t createFunc;
				if (!sl->getFunctionPointer( createFuncName, createFunc ))
				{
					BLOCXX_LOG_ERROR(logger, Format("SafeLibCreate::create failed getting function pointer to \"%1\" from library", createFuncName));

					return 0;
				}

				T* ptr = (*createFunc)();

				return ptr;
			}
			else
			{
				BLOCXX_LOG_ERROR(logger, Format("SafeLibCreate::create sigsetjmp call returned %1, we caught a segfault.  getOWVersion() or %2() is misbehaving",
					sigtype, createFuncName));

				return 0;
			}
		}
		catch(Exception& e)
		{
			BLOCXX_LOG_ERROR(logger, "SafeLibCreate::create");
			BLOCXX_LOG_ERROR(logger, Format("File: %1", e.getFile()));
			BLOCXX_LOG_ERROR(logger, Format("Line: %1", e.getLine()));
			BLOCXX_LOG_ERROR(logger, Format("Msg: %1", e.getMessage()));
		}
		catch (...)
		{
			BLOCXX_LOG_ERROR(logger, "SafeLibCreate::create caught unknown exception");
		}

		return 0;
	}

private:
#ifdef WIN32
	static jmp_buf theLoaderBuf;
#else
	static sigjmp_buf theLoaderBuf;
#endif

	// this is commented out because it won't compile.  As it is, it may
	// invoke undefined behavior if the C calling convention is different
	// from the C++ calling convention.  Fortunately that's not the case
	// with gcc :-)
	//extern "C" {
	static void theSignalHandler(int sig)
	{
#ifdef WIN32
		longjmp(theLoaderBuf, sig);
#else
		siglongjmp(theLoaderBuf, sig);
#endif
	}

	//} // extern "C"

};
template <typename T>
#ifdef WIN32
jmp_buf
#else
sigjmp_buf
#endif
SafeLibCreate<T>::theLoaderBuf;


} // end namespace OW_NAMESPACE

#endif

