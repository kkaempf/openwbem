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
#ifndef OW_DYLD_SHARED_LIBRARY_LOADER_HPP_INCLUDE_GUARD_
#define OW_DYLD_SHARED_LIBRARY_LOADER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#if defined(OW_USE_DYLD)
#include "OW_SharedLibraryLoader.hpp"

namespace OpenWBEM
{

/**
 * dyldSharedLibraryLoader is the class for loading
 * shared libraries on dl.
 */
class dyldSharedLibraryLoader : public SharedLibraryLoader
{
	public:
		virtual ~dyldSharedLibraryLoader();
		/**
		 * Load a shared library specified by filename.  If the operation fails,
		 * the return value will be null ref counted pointer.
		 */
		virtual SharedLibraryRef loadSharedLibrary(const String& filename,
			LoggerRef logger) const;
};

} // end namespace OpenWBEM

#endif // #if defined(OW_USE_DYLD)
#endif