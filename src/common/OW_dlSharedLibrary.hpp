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

#ifndef OW_LINUXSHAREDLIBRARY_HPP_
#define OW_LINUXSHAREDLIBRARY_HPP_

#include "OW_config.h"
#include "OW_SharedLibrary.hpp"

/**
 * OW_dlSharedLibrary loads and queries shared libraries. Using dlsym &
 * friends.
 */
class OW_dlSharedLibrary : public OW_SharedLibrary
{
	public:
		OW_dlSharedLibrary(void * libhandle, const OW_String& libName)
			: OW_SharedLibrary(), m_libhandle( libhandle ), m_libName(libName)
		{
		}

		virtual ~OW_dlSharedLibrary();


	protected:
		/**
		 * Derived classes have to override this function to implement
		 * the symbol loading.  The symbol to be looked up is contained in
		 * functionName, and the pointer to the function should be written
		 * into *fp.  Return true if the function succeeded, false otherwise.
		 * @param functionName	The name of the function to resolve.
		 * @param fp				Where to store the function pointer.
		 * @return true if function succeeded, false otherwise.
		 */
		virtual bool doGetFunctionPointer( const OW_String& functionName,
												  void** fp ) const;
			// throw ()

	private:
		void* m_libhandle;
		OW_String m_libName;
};

#endif

