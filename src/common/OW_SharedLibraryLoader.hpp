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
#ifndef OW_SHARED_LIBRARY_LOADER_HPP_
#define OW_SHARED_LIBRARY_LOADER_HPP_
#include "OW_config.h"
#include "OW_SharedLibrary.hpp"
#include "OW_String.hpp"
#include "OW_Reference.hpp"
#include "OW_Logger.hpp"

namespace OpenWBEM
{

class SharedLibraryLoader;
typedef Reference<SharedLibraryLoader> SharedLibraryLoaderRef;
/**
 * SharedLibraryLoader is the base class for a platform class for loading
 * shared libraries.
 */
class SharedLibraryLoader
{
public:
	virtual ~SharedLibraryLoader()
	{
	}
	/**
	 * Load a shared library specified by filename.  If the operation fails,
	 * the return value will be null ref counted pointer, and 
	 * logger->logError() will be called to report the details of the error.
	 * Exception safety: Strong
	 * @param filename The name of the shared library to load.
	 * @param logger If an error occurs, logger->logError() will be passed
	 *  a description.
	 * @return SharedLibraryRef owning representing the shared library 
	 *  identified by filename.  NULL on failure.
	 */
	virtual SharedLibraryRef loadSharedLibrary(const String& filename,
		LoggerRef logger) const = 0;

	/**
	 * @return A reference to an SharedLibraryLoader object.
	 *
	 * Note: The implementation of createSharedLibraryLoader is contained
	 * in the platforms specific source file.  Only one type of 
	 * SharedLibraryLoader exists for a given system.  The build system selects
	 * the correct one to build.
	 */
	static SharedLibraryLoaderRef createSharedLibraryLoader();
};

} // end namespace OpenWBEM

typedef OpenWBEM::SharedLibraryLoaderRef OW_SharedLibraryLoaderRef OW_DEPRECATED;
typedef OpenWBEM::SharedLibraryLoader OW_SharedLibraryLoader OW_DEPRECATED;

#endif
