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
#include "OW_dlSharedLibraryLoader.hpp"
#include "OW_dlSharedLibrary.hpp"
#include "OW_Format.hpp"
#include <dlfcn.h>

OW_SharedLibraryRef 
OW_dlSharedLibraryLoader::loadSharedLibrary(const OW_String& filename,
	OW_LoggerRef logger) const
{
	void* libhandle = dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
	if (libhandle)
	{
		return OW_SharedLibraryRef( new OW_dlSharedLibrary(libhandle,
			filename));
	}
	else
	{
		logger->logError(format("OW_dlSharedLibraryLoader::loadSharedLibrary "
			"dlopen returned NULL.  Error is: %1", dlerror()));
		return OW_SharedLibraryRef( 0 );
	}
}

OW_SharedLibraryLoaderRef 
OW_SharedLibraryLoader::createSharedLibraryLoader()
{
	return OW_SharedLibraryLoaderRef(new OW_dlSharedLibraryLoader);
}

