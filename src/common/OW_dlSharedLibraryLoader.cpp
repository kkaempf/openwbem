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

#include "OW_config.h"
#if defined(OW_USE_DL)
#include "OW_dlSharedLibraryLoader.hpp"
#include "OW_dlSharedLibrary.hpp"
#include "OW_Format.hpp"
#include <dlfcn.h>

namespace OW_NAMESPACE
{

///////////////////////////////////////////////////////////////////////////////
SharedLibraryRef
dlSharedLibraryLoader::loadSharedLibrary(const String& filename,
	const LoggerRef& logger) const
{
	// There is a reason to use RTLD_NOW.  If some symbols can't be resolved because
	// the shared library is built incorrectly or missing some symbols or something,
	// then using RTLD_NOW will cause dlopen() to fail, and then the error can be detected
	// and handled.  If RTLD_LAZY is specified, dlopen() won't fail, but when code is
	// executed that tries to use an unresolvable symbol, that will cause a segfault.
	// RTLD_GLOBAL is necessary for proper exception and rtti support with gcc.
	// See http://gcc.gnu.org/faq.html#dso, so even though it may cause symbol conflicts,
	// we have to live with it.
	void* libhandle = dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
	
	String first_error = dlerror();
	String second_error;

#if defined(OW_USE_FAKE_LIBS)
	// This section, if it determines that a library is fake, will attempt
	// to load NULL (which, on most dlopen platforms should return a handle
	// to the main executable).  This handle can be used just like any
	// other, assuming that the main executable was linked with the proper
	// export flags (--export-dynamic on linux, -bexpall on AIX, etc.).
	if ( !libhandle )
	{
	  if ( dlSharedLibrary::isFakeLibrary(filename) )
	  {
	    libhandle = dlopen(NULL, RTLD_NOW | RTLD_GLOBAL);
	
	    if ( !libhandle )
	    {
	      second_error = dlerror();
	    }
	  }
	}
#endif /* defined(OW_USE_FAKE_LIBS) */	
	if (libhandle)
	{
		return SharedLibraryRef( new dlSharedLibrary(libhandle,
			filename));
	}
	else
	{
		OW_LOG_ERROR(logger, Format("dlSharedLibraryLoader::loadSharedLibrary "
			"dlopen returned NULL.  Error is: %1", first_error));
		if ( !second_error.empty() )
		{
			OW_LOG_ERROR(logger, Format("dlSharedLibraryLoader::loadSharedLibrary (fakelib) "
				"dlopen returned NULL.  Error is: %1", second_error));		
		}
		return SharedLibraryRef( 0 );
	}
}

///////////////////////////////////////////////////////////////////////////////
SharedLibraryLoaderRef
SharedLibraryLoader::createSharedLibraryLoader()
{
	return SharedLibraryLoaderRef(new dlSharedLibraryLoader);
}
///////////////////////////////////////////////////////////////////////////////
dlSharedLibraryLoader::~dlSharedLibraryLoader()
{
}

} // end namespace OW_NAMESPACE

#endif // OW_USE_DL
