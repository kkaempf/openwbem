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
#include <utility> // for std::pair
#include <setjmp.h> // for setjmp, longjmp and jmp_buf

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
template <typename T>
class SafeLibCreate
{
	typedef T* (*createFunc_t)();
	typedef const char* (*versionFunc_t)();
public:
	typedef std::pair<Reference<T>, SharedLibraryRef> return_type;
	typedef SharedLibraryReference<Reference<T> > return_obj;
	
	static return_type
	loadAndCreate(String const& libname, String const& createFuncName,
		const LoggerRef& logger)
	{
		SharedLibraryLoaderRef sll =
			SharedLibraryLoader::createSharedLibraryLoader();
		SharedLibraryRef sl = sll->loadSharedLibrary(libname, logger);
		T* ptr = 0;
		if ( !sl.isNull() )
		{
			ptr = create(sl, createFuncName, logger);
		}
		else
		{
			logger->logDebug(Format("safeLibCreate::loadAndCreate"
				" FAILED loading library %1", libname));
		}
		return std::make_pair(Reference<T>(ptr),sl);
	}
	static return_obj
	loadAndCreateObject(String const& libname,
		String const& createFuncName, const LoggerRef& logger)
	{
		SharedLibraryLoaderRef sll =
			SharedLibraryLoader::createSharedLibraryLoader();
		SharedLibraryRef sl = sll->loadSharedLibrary(libname, logger);
		T* ptr = 0;
		if ( !sl.isNull() )
		{
			ptr = create(sl, createFuncName, logger);
		}
		else
		{
			logger->logDebug(Format("safeLibCreate::loadAndCreate"
				" FAILED loading library %1", libname));
		}
		return return_obj(sl, ptr);
	}
	static T*
	create(SharedLibraryRef sl, String const& createFuncName,
		const LoggerRef& logger)
	{
		logger->logDebug(Format("safeLibCreate::create called.  createFuncName = %1",
			createFuncName).c_str());
		
		try
		{
			int sigtype;
			SignalScope r1( OW_SIGFPE,  theSignalHandler );
			SignalScope r3( OW_SIGSEGV, theSignalHandler );
			SignalScope r4( OW_SIGBUS,  theSignalHandler );
			SignalScope r5( OW_SIGABRT, theSignalHandler );
			sigtype = setjmp(theLoaderBuf);
			if ( sigtype == 0 )
			{
				versionFunc_t versFunc;
				if (!sl->getFunctionPointer( "getOWVersion", versFunc))
				{
					logger->logError("safeLibCreate::create failed getting"
						" function pointer to \"getOWVersion\" from library");
	
					return 0;
				}
	
				const char* strVer = 0;
				strVer = (*versFunc)();
				if(!strVer || strcmp(strVer, OW_VERSION) != 0)
				{
					logger->logError("safeLibCreate::create -"
						" Invalid version returned from \"getOWVersion\"");
					return 0;
				}
				else
				{
					createFunc_t createFunc;
					if (!sl->getFunctionPointer( createFuncName
						, createFunc ))
					{
						logger->logError(Format("safeLibCreate::create failed"
							" getting function pointer to \"%1\" from"
							" library", createFuncName));
	
						return 0;
					}
	
					T* ptr = (*createFunc)();
	
					return ptr;
				}
			}
			else
			{
				logger->logError(Format("safeLibCreate::create setjmp call"
					" returned != 0, we caught a segfault.  "
					"getOWVersion() or %1() is misbehaving", createFuncName));
	
				return 0;
			}
		}
		catch(Exception& e)
		{
			logger->logError("safeLibCreate::create");
			logger->logError(Format("File: %1", e.getFile()));
			logger->logError(Format("Line: %1", e.getLine()));
			logger->logError(Format("Msg: %1", e.getMessage()));
		}
		catch (...)
		{
			logger->logError("safeLibCreate::create caught unknown"
				" exception");
		}
	
		return 0;
	}
	
private:
	static jmp_buf theLoaderBuf;
	
	// this is commented out because it won't compile.  As it is, it may
	// invoke undefined behavior if the C calling convention is different
	// from the C++ calling convention.  Fortunately that's not the case
	// with gcc :-)
	//extern "C" {
	static void theSignalHandler(int sig)
	{
		longjmp(theLoaderBuf, sig);
	}
	
	//} // extern "C"
	
};
template <typename T>
jmp_buf SafeLibCreate<T>::theLoaderBuf;

} // end namespace OpenWBEM

#endif

