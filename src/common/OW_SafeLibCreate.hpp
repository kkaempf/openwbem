/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_SharedLibraryReference.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_Exception.hpp"

#include <utility> // for std::pair
#include <setjmp.h> // for setjmp, longjmp and jmp_buf

//////////////////////////////////////////////////////////////////////////////
template <typename T>
class OW_SafeLibCreate
{
	typedef T* (*createFunc_t)();
	typedef const char* (*versionFunc_t)();

public:

	typedef std::pair<OW_Reference<T>, OW_SharedLibraryRef> return_type;
	typedef OW_SharedLibraryReference<T> return_obj;
	
	static return_type
	loadAndCreate(OW_String const& libname, OW_String const& createFuncName,
		const OW_LoggerRef& logger)
	{
		OW_SharedLibraryLoaderRef sll =
			OW_SharedLibraryLoader::createSharedLibraryLoader();
		OW_SharedLibraryRef sl = sll->loadSharedLibrary(libname, logger);
		T* ptr = 0;
		if ( !sl.isNull() )
		{
			ptr = create(sl, createFuncName, logger);
		}
		else
		{
			logger->logDebug(format("safeLibCreate::loadAndCreate"
				" FAILED loading library %1", libname));
		}
		return std::make_pair(OW_Reference<T>(ptr),sl);
	}

	static return_obj
	loadAndCreateObject(OW_String const& libname,
		OW_String const& createFuncName, const OW_LoggerRef& logger)
	{
		OW_SharedLibraryLoaderRef sll =
			OW_SharedLibraryLoader::createSharedLibraryLoader();
		OW_SharedLibraryRef sl = sll->loadSharedLibrary(libname, logger);
		T* ptr = 0;
		if ( !sl.isNull() )
		{
			ptr = createObj(sl, createFuncName, logger);
		}
		else
		{
			logger->logDebug(format("safeLibCreate::loadAndCreate"
				" FAILED loading library %1", libname));
		}

		return return_obj(sl, ptr);
	}

	static T*
	createObj(OW_SharedLibraryRef sl, OW_String const& createFuncName,
		const OW_LoggerRef& logger)
	{
		logger->logDebug(format("safeLibCreate::create called.  createFuncName = %1",
			createFuncName).c_str());
		
		try
		{
			int sigtype;
			OW_SignalScope r1( SIGFPE,  theSignalHandler );
			OW_SignalScope r3( SIGSEGV, theSignalHandler );
			OW_SignalScope r4( SIGBUS,  theSignalHandler );
			OW_SignalScope r5( SIGABRT, theSignalHandler );
			sigtype = setjmp(theLoaderBuf);
			if ( sigtype == 0 )
			{
				versionFunc_t versFunc;
				if (!OW_SharedLibrary::getFunctionPointer( sl, "getOWVersion", versFunc))
				{
					logger->logError("safeLibCreate::create failed getting"
						" function pointer to \"getOWVersion\" from library");
	
					return 0;
				}
	
				const char* strVer = (*versFunc)();
				if(strcmp(strVer, OW_VERSION))
				{
					logger->logError("safeLibCreate::create -"
						" Invalid version returned from \"getOWVersion\"");
					return 0;
				}
				else
				{
					createFunc_t createFunc;
					if (!OW_SharedLibrary::getFunctionPointer( sl, createFuncName
						, createFunc ))
					{
						logger->logError("safeLibCreate::create failed"
							" getting function pointer to \"createWQL\" from"
							" library");
	
						return 0;
					}
	
					T* ptr = (*createFunc)();
					return ptr;
				}
			}
			else
			{
				logger->logError("safeLibCreate::create setjmp call"
					" failed");
	
				return 0;
			}
		}
		catch(OW_Exception& e)
		{
			logger->logError("safeLibCreate::create");
			logger->logError(format("File: %1", e.getFile()));
			logger->logError(format("Line: %1", e.getLine()));
			logger->logError(format("Msg: %1", e.getMessage()));
		}
		catch (...)
		{
			logger->logError("safeLibCreate::create caught unknown"
				" exception");
		}
	
		return 0;
	}

	// OLD
	static T*
	create(OW_SharedLibraryRef sl, OW_String const& createFuncName,
		const OW_LoggerRef& logger)
	{
		logger->logDebug(format("safeLibCreate::create called.  createFuncName = %1",
			createFuncName).c_str());
		
		try
		{
			int sigtype;
			OW_SignalScope r1( SIGFPE,  theSignalHandler );
			OW_SignalScope r3( SIGSEGV, theSignalHandler );
			OW_SignalScope r4( SIGBUS,  theSignalHandler );
			OW_SignalScope r5( SIGABRT, theSignalHandler );
			sigtype = setjmp(theLoaderBuf);
			if ( sigtype == 0 )
			{
				versionFunc_t versFunc;
				if (!OW_SharedLibrary::getFunctionPointer( sl, "getOWVersion", versFunc))
				{
					logger->logError("safeLibCreate::create failed getting"
						" function pointer to \"getOWVersion\" from library");
	
					return 0;
				}
	
				const char* strVer = (*versFunc)();
				if(strcmp(strVer, OW_VERSION))
				{
					logger->logError("safeLibCreate::create -"
						" Invalid version returned from \"getOWVersion\"");
					return 0;
				}
				else
				{
					createFunc_t createFunc;
					if (!OW_SharedLibrary::getFunctionPointer( sl, createFuncName
						, createFunc ))
					{
						logger->logError("safeLibCreate::create failed"
							" getting function pointer to \"createWQL\" from"
							" library");
	
						return 0;
					}
	
					T* ptr = (*createFunc)();
	
					return ptr;
				}
			}
			else
			{
				logger->logError("safeLibCreate::create setjmp call"
					" failed");
	
				return 0;
			}
		}
		catch(OW_Exception& e)
		{
			logger->logError("safeLibCreate::create");
			logger->logError(format("File: %1", e.getFile()));
			logger->logError(format("Line: %1", e.getLine()));
			logger->logError(format("Msg: %1", e.getMessage()));
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
jmp_buf OW_SafeLibCreate<T>::theLoaderBuf;




