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

#ifndef OW_PROVIDER_IFC_LOADER_HPP_
#define OW_PROVIDER_IFC_LOADER_HPP_

#include "OW_config.h"
#include "OW_SharedLibraryLoader.hpp"
#include "OW_SharedLibrary.hpp"
#include "OW_ProviderIFCBaseIFC.hpp"
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"


/**
 * This class is a base class for different provider interface loading
 * strategies. Each derived class should implement a method of locating the
 * provider interface shared libraries.  The loadIFCs function needs to be
 * overridden, and then the createProviderIFCFromLib function can be used to
 * actually load the shared library.
 */
class OW_ProviderIFCLoaderBase
{
public:


	OW_ProviderIFCLoaderBase(OW_SharedLibraryLoaderRef sll,
		OW_ServiceEnvironmentIFCRef env)
		: m_sll( sll )
		, m_env(env)
	{
	}

	virtual ~OW_ProviderIFCLoaderBase() {}

	/**
	 * This function needs to be overridden by derived classes and implement a
	 * strategy to obtain the shared library names of the provider interfaces.
	 * Once the file names are obtained it should call createProviderIFCFromLib
	 * and add the returned provider interface into interfaces.
	 *
	 * @param interfaces This is an out parameter.  The ProviderIFC refs will be
	 * 	added to the array.
	 * @param shlibs This is an our parameter.  The shared libraries of the
	 * 	ProviderIFC refs will be added to this array.  NOTE: The provider
	 * 	IFC refs MUST go away and delete the underlying object before the
	 * 	libraries are deleted, otherwise the program will segfault.
	 *
	 * The derived classes code will probably be similar to this:
	 * 	virtual void loadIFCs(
	 * 		OW_Array<OW_ProviderIFCBaseIFCRef>& ifcs,
	 * 		OW_Array<OW_SharedLibraryRef>& shlibs) const
	 * 	{
	 * 		ifc_lib_pair rval;
	 * 		rval = createProviderIFCFromLib( "libname" );
	 * 		if ( !rval.first.isNull() && !rval.second.isNull() )
	 * 		{
	 * 			out.push_back( rval.first );
	 * 			shlibs.push_back( rval.second );
	 * 		}
	 *
	 * 		rval = createProviderIFCFromLib( "libname2" );
	 * 		if ( !rval.first.isNull() && !rval.second.isNull() )
	 * 		{
	 * 			out.push_back( rval.first );
	 * 			shlibs.push_back( rval.second );
	 * 		}
	 *		}
	 *
	 */
	virtual void loadIFCs(
		OW_Array<OW_ProviderIFCBaseIFCRef>& interfaces) const = 0;

	OW_ServiceEnvironmentIFCRef getEnvironment() const
	{
		return m_env;
	}

protected:
	/**
	 * Function uses the OW_SharedLibraryLoader to load the library designated
	 * by libname (probably a filename) and creates an OW_ProviderIFCBaseIFCRef.
	 *
	 * @param libname The name of the library to load.
	 *
	 * @returns A pair containing a ref counted pointer to the OW_ProviderIFCBaseIFC
	 * 	corresponding to libname and a ref counted point to the corresponding
	 * 	OW_SharedLibrary.  If loading the library fails, null is returned.
	 * 	e.g. retval.first.isNull() == true and retval.second.isNull() == true.
	 */
	OW_ProviderIFCBaseIFCRef createProviderIFCFromLib(const OW_String& libname) const;

private:

	//OW_ProviderIFCBaseIFC* safeCreateIFC(OW_SharedLibraryRef sl) const;
	
	const OW_SharedLibraryLoaderRef m_sll;
	OW_ServiceEnvironmentIFCRef m_env;
};

typedef OW_Reference<OW_ProviderIFCLoaderBase> OW_ProviderIFCLoaderRef;

class OW_ProviderIFCLoader : public OW_ProviderIFCLoaderBase
{
public:
	OW_ProviderIFCLoader(OW_SharedLibraryLoaderRef sll,
		OW_ServiceEnvironmentIFCRef env)
		: OW_ProviderIFCLoaderBase(sll, env) {}

	virtual ~OW_ProviderIFCLoader(){}

	virtual void loadIFCs(OW_Array<OW_ProviderIFCBaseIFCRef>& ifcs) const;

	static OW_ProviderIFCLoaderRef createProviderIFCLoader(
		OW_ServiceEnvironmentIFCRef env);
};


#endif

