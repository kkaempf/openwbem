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

namespace OpenWBEM
{

/**
 * dlSharedLibrary loads and queries shared libraries. Using dlsym &
 * friends.
 */
class dlSharedLibrary : public SharedLibrary
{
public:
	dlSharedLibrary(void * libhandle, const String& libName)
		: SharedLibrary(), m_libhandle( libhandle ), m_libName(libName)
	{
	}
	virtual ~dlSharedLibrary();

	/** 
	 * on some platforms (e.g. glibc 2.2.x), there are bugs in the dl* functions, 
	 * and the workaround is to not call dlclose.  Setting this variable to 0
	 * will cause dlclose to never be called.  Doing this has some problems:
	 * memory mapped to the shared library will never be freed up. New versions
	 * of the library can't be loaded (i.e. a provider is updated)
	 */
	static int m_call_dlclose;

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
	virtual bool doGetFunctionPointer( const String& functionName,
											  void** fp ) const;
private:
	void* m_libhandle;
	String m_libName;
};

} // end namespace OpenWBEM

#endif
