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
#include "OW_DLLSharedLibrary.hpp"
#include "OW_Format.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"

#include "OW_Types.h"

#include <iostream>

namespace OpenWBEM
{

Mutex DLLSharedLibrary_guard;

//////////////////////////////////////////////////////////////////////////////
DLLSharedLibrary::DLLSharedLibrary(HINSTANCE libhandle, const String& libName)
	: SharedLibrary(), m_libhandle(libhandle), m_libName(libName)
{
}
//////////////////////////////////////////////////////////////////////////////
DLLSharedLibrary::~DLLSharedLibrary()
{
	::FreeLibrary(m_libhandle);
}
//////////////////////////////////////////////////////////////////////////////
bool
DLLSharedLibrary::doGetFunctionPointer(const String& functionName,
	void** fp) const
{
	MutexLock l(DLLSharedLibrary_guard);

	*fp = (void*) ::GetProcAddress(m_libhandle, functionName.c_str());
	  
	if (!*fp)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool DLLSharedLibrary::isFakeLibrary(const String& library_path)
{
  return false;
}

} // end namespace OpenWBEM

#endif

