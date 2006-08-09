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

#ifndef OW_SAFE_LIB_CREATE_HPP_INCLUDE_GUARD_
#define OW_SAFE_LIB_CREATE_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_SharedLibraryReference.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_Exception.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_Assertion.hpp"

#include <utility> // for std::pair
#include <setjmp.h> // for sigsetjmp, siglongjmp and jmp_buf

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

// Note that using setjmp/longjmp will fail to reset the blocked signal mask after the first time this fails.
// using sigsetjmp/siglongjmp fixes this problem.

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
template <typename T>
class SafeLibCreate
{
	typedef T* (*createFunc_t)();
	typedef const char* (*versionFunc_t)();
public:
	typedef std::pair<IntrusiveReference<T>, SharedLibraryRef> return_type;
	typedef SharedLibraryReference<IntrusiveReference<T> > return_obj;
	
	static return_type
	loadAndCreate(String const& libname, String const& createFuncName)
	{
		SharedLibraryLoaderRef sll = SharedLibraryLoader::createSharedLibraryLoader();
		SharedLibraryRef sl = sll->loadSharedLibrary(libname);
		T* ptr = 0;
		if ( !sl.isNull() )
		{
			ptr = create(sl, createFuncName);
		}
		else
		{
			Logger lgr("ow.SafeLibCreate");
			OW_LOG_DEBUG(lgr, Format("safeLibCreate::loadAndCreate FAILED loading library %1", libname));
		}
		return std::make_pair(IntrusiveReference<T>(ptr),sl);
	}

	static return_obj
	loadAndCreateObject(String const& libname,
		String const& createFuncName)
	{
		SharedLibraryLoaderRef sll = SharedLibraryLoader::createSharedLibraryLoader();
		SharedLibraryRef sl = sll->loadSharedLibrary(libname);
		T* ptr = 0;
		if ( sl )
		{
			ptr = create(sl, createFuncName);
		}
		else
		{
			Logger lgr("ow.SafeLibCreate");
			OW_LOG_DEBUG(lgr, Format("safeLibCreate::loadAndCreate FAILED loading library %1", libname));
		}
		return return_obj(sl, ptr);
	}

	static T*
	create(SharedLibraryRef sl, String const& createFuncName)
	{
		Logger logger("ow.SafeLibCreate");
		OW_LOG_DEBUG(logger, Format("safeLibCreate::create called.  createFuncName = %1", createFuncName).c_str());
		
		OW_ASSERT(sl);
		try
		{
			int sigtype;
			SignalScope r1( OW_SIGFPE,  theSignalHandler );
			SignalScope r3( OW_SIGSEGV, theSignalHandler );
#ifdef SIGBUS // NetWare doesn't have this signal
			SignalScope r4( OW_SIGBUS,  theSignalHandler );
#endif
			SignalScope r5( OW_SIGABRT, theSignalHandler );
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
					OW_LOG_ERROR(logger, "safeLibCreate::create failed getting function pointer to \"getOWVersion\" from library");
	
					return 0;
				}
	
				const char* strVer = 0;
				strVer = (*versFunc)();
				if (!strVer || strncmp(strVer, OW_VERSION,strlen(OW_VERSION)) != 0)
				{
					OW_LOG_INFO(logger, Format("safeLibCreate::create - Warning: version returned from \"getOWVersion\" (%1) does not match (%2)", 
						strVer ? strVer : "", OW_VERSION));
				}

				createFunc_t createFunc;
				if (!sl->getFunctionPointer( createFuncName, createFunc ))
				{
					OW_LOG_ERROR(logger, Format("safeLibCreate::create failed getting function pointer to \"%1\" from library", createFuncName));

					return 0;
				}

				T* ptr = (*createFunc)();

				return ptr;
			}
			else
			{
				OW_LOG_ERROR(logger, Format("safeLibCreate::create sigsetjmp call returned %1, we caught a segfault.  getOWVersion() or %2() is misbehaving", 
					sigtype, createFuncName));
	
				return 0;
			}
		}
		catch(Exception& e)
		{
			OW_LOG_ERROR(logger, "safeLibCreate::create");
			OW_LOG_ERROR(logger, Format("File: %1", e.getFile()));
			OW_LOG_ERROR(logger, Format("Line: %1", e.getLine()));
			OW_LOG_ERROR(logger, Format("Msg: %1", e.getMessage()));
		}
		catch (...)
		{
			OW_LOG_ERROR(logger, "safeLibCreate::create caught unknown exception");
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
jmp_buf SafeLibCreate<T>::theLoaderBuf;
	
} // end namespace OW_NAMESPACE

#endif

