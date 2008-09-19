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

#include "OW_config.h"
#include "OW_SharedLibraryException.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_SafeLibCreate.hpp"


///////////////////////////////////////////////////////////////////////////////
OW_ProviderIFCBaseIFCRef
OW_ProviderIFCLoaderBase::createProviderIFCFromLib(
	const OW_String& libname) const
{
	m_env->getLogger()->logDebug(format(
		"OW_ProviderIFCBaseIFCLoaderBase::createProviderIFCFromLib"
		" loading library %1", libname));

	OW_SharedLibraryRef sl = m_sll->loadSharedLibrary(libname,
		m_env->getLogger());

	OW_ProviderIFCBaseIFC* ptr = 0;
	if ( !sl.isNull() )
	{
		ptr = OW_SafeLibCreate<OW_ProviderIFCBaseIFC>::create(sl,
                "createProviderIFC", m_env->getLogger());
	}
	else
	{
		m_env->getLogger()->logDebug(format("OW_ProviderIFCBaseIFCLoaderBase::"
			"createProviderIFCFromLib FAILED loading library %1", libname));
	}

	OW_ProviderIFCBaseIFCRef retval(sl, ptr);
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ProviderIFCLoader::loadIFCs(OW_Array<OW_ProviderIFCBaseIFCRef>& ifcs) const
{
	OW_ServiceEnvironmentIFCRef env = getEnvironment();
	OW_String libdir = env->getConfigItem(
		OW_ConfigOpts::PROVIDER_IFC_LIBS_opt);

	env->getLogger()->logDebug(format("OW_ProviderIFCBaseIFCLoaderBase::loadIFC getting provider"
		" interfaces from: %1", libdir));

	OW_StringArray libs;
	OW_FileSystem::getDirectoryContents(libdir, libs);

	if(libs.size() == 0)
	{
		env->getLogger()->logDebug("OW_ProviderIFCBaseIFCLoaderBase::loadIFCs did not find any"
			" provider interfacess");
		return;
	}

	int ifcCount = 0;
	for (OW_StringArray::size_type i = 0; i < libs.size(); ++i)
	{
		if(!libs[i].endsWith(OW_SHAREDLIB_EXTENSION))
		{
			continue;
		}

		OW_ProviderIFCBaseIFCRef rval;
		OW_ProviderIFCBaseIFCRef pmr;
		rval = createProviderIFCFromLib(libdir + OW_FILENAME_SEPARATOR + libs[i]);
		if(rval)
		{
			ifcCount++;
			ifcs.push_back(rval);
		}
		else
		{
			env->getLogger()->logError(format("Unable to load ProviderIFC library %1",
				libs[i]));
		}
	}

	env->getLogger()->logDebug(format("Number of provider interfaces loaded: %1",
		ifcCount));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_ProviderIFCLoaderRef
OW_ProviderIFCLoader::createProviderIFCLoader(OW_ServiceEnvironmentIFCRef env)
{
	return OW_ProviderIFCLoaderRef(new OW_ProviderIFCLoader(
		OW_SharedLibraryLoader::createSharedLibraryLoader(), env));
}



