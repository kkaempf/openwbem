/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_config.h"
#if defined(OW_USE_DYLD)
#include "OW_dyldSharedLibraryLoader.hpp"
#include "OW_dyldSharedLibrary.hpp"
#include "OW_Format.hpp"

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
SharedLibraryRef 
dyldSharedLibraryLoader::loadSharedLibrary(const String& filename,
	LoggerRef logger) const
{
    NSObjectFileImage image;
    NSObjectFileImageReturnCode dsoerr = NSCreateObjectFileImageFromFile(filename.c_str(), &image);
	const char* err_msg = NULL;
	void* libhandle = dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);

    if (dsoerr == NSObjectFileImageSuccess) 
	{
        libhandle = NSLinkModule(image, path, NSLINKMODULE_OPTION_RETURN_ON_ERROR);
        if (!m_libhandle) 
		{
            NSLinkEditErrors errors;
            int errorNumber;
            const char *fileName;
            NSLinkEditError(&errors, &errorNumber, &fileName, &err_msg);
        }
        NSDestroyObjectFileImage(image);
    }
    else if ((dsoerr == NSObjectFileImageFormat ||
             dsoerr == NSObjectFileImageInappropriateFile) &&
             NSAddLibrary(filename.c_str()) == TRUE) 
	{
        libhandle = (NSModule)DYLD_LIBRARY_HANDLE;
    }
    else 
	{
		err_msg = "cannot create object file image or add library";
    }



	if (libhandle)
	{
		return SharedLibraryRef( new dyldSharedLibrary(libhandle,
			filename));
	}
	else
	{
		logger->logError(format("dyldSharedLibraryLoader::loadSharedLibrary:"
			" %1", err_msg));
		return SharedLibraryRef( 0 );
	}
}
///////////////////////////////////////////////////////////////////////////////
SharedLibraryLoaderRef 
SharedLibraryLoader::createSharedLibraryLoader()
{
	return SharedLibraryLoaderRef(new dyldSharedLibraryLoader);
}
///////////////////////////////////////////////////////////////////////////////
dyldSharedLibraryLoader::~dyldSharedLibraryLoader()
{
}

} // end namespace OpenWBEM

#endif // #if defined(OW_USE_DYLD)