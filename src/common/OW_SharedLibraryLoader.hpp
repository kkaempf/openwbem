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

#ifndef OW_SHARED_LIBRARY_LOADER_HPP_
#define OW_SHARED_LIBRARY_LOADER_HPP_

#include "OW_config.h"
#include "OW_SharedLibrary.hpp"
#include "OW_String.hpp"
#include "OW_Reference.hpp"
#include "OW_Logger.hpp"

class OW_SharedLibraryLoader;
typedef OW_Reference<OW_SharedLibraryLoader> OW_SharedLibraryLoaderRef;

/**
 * OW_SharedLibraryLoader is the base class for a platform class for loading
 * shared libraries.
 */
class OW_SharedLibraryLoader
{
public:
	virtual ~OW_SharedLibraryLoader()
	{
	}

	/**
	 * Load a shared library specified by filename.  If the operation fails,
	 * the return value will be null ref counted pointer.
	 */
	virtual OW_SharedLibraryRef loadSharedLibrary(const OW_String& filename,
		OW_LoggerRef logger) const = 0;

	/**
	 * @return A reference to an OW_SharedLibraryLoader object.
	 *
	 * Note: The implementation of createSharedLibraryLoader is contained
	 * in the platform specific branch of the source tree.
	 */
	static OW_SharedLibraryLoaderRef createSharedLibraryLoader();

};

#endif


