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
 * @author Jon Carey
 */

#include "OW_config.h"

#if defined(OW_USE_DLL)
#include "OW_DLLSharedLibraryLoader.hpp"
#include "OW_DLLSharedLibrary.hpp"
#include "OW_Format.hpp"

namespace {

OpenWBEM::String getLastErrorMsg()
{
	OpenWBEM::String errMsg;
	LPVOID lpMsgBuf;
	if (!::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf, 0, NULL ))
	{
		return errMsg;
	}

	errMsg = (char*) lpMsgBuf;
	::LocalFree(lpMsgBuf);
	return errMsg;
}

}	// End of unnamed namespace

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
SharedLibraryRef 
DLLSharedLibraryLoader::loadSharedLibrary(const String& filename,
	LoggerRef logger) const
{
	HINSTANCE libhandle = ::LoadLibrary(filename.c_str());
	if (libhandle)
	{
		return SharedLibraryRef(new DLLSharedLibrary(libhandle,
			filename));
	}

	String errMsg = getLastErrorMsg();

	logger->logError(Format("DLLSharedLibraryLoader::loadSharedLibrary "
		"LoadLibrary returned NULL.  Error is: %1", errMsg));

	return SharedLibraryRef(0);
}

///////////////////////////////////////////////////////////////////////////////
SharedLibraryLoaderRef 
SharedLibraryLoader::createSharedLibraryLoader()
{
	return SharedLibraryLoaderRef(new DLLSharedLibraryLoader);
}
///////////////////////////////////////////////////////////////////////////////
DLLSharedLibraryLoader::~DLLSharedLibraryLoader()
{
}

} // end namespace OpenWBEM

#endif // OW_USE_DLL
