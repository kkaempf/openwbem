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
 */

#include "OW_config.h"
#include "OW_SharedLibraryException.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "OW_Format.hpp"
#include "OW_SignalScope.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_FileSystem.hpp"
#include "OW_SafeLibCreate.hpp"
#include "OW_Exception.hpp"

namespace OW_NAMESPACE
{

namespace
{
const String COMPONENT_NAME("ow.owcimomd.ProviderIFCLoader");

OW_DECLARE_EXCEPTION(ProviderIFCLoader);
OW_DEFINE_EXCEPTION(ProviderIFCLoader);

}
///////////////////////////////////////////////////////////////////////////////
ProviderIFCBaseIFCRef
ProviderIFCLoaderBase::createProviderIFCFromLib(const String& libname) const
{
	LoggerRef lgr(m_env->getLogger(COMPONENT_NAME));
	OW_LOG_DEBUG(lgr, Format("ProviderIFCBaseIFCLoaderBase::createProviderIFCFromLib loading library %1", libname));
	SharedLibraryRef sl = m_sll->loadSharedLibrary(libname,
		lgr);
	ProviderIFCBaseIFC* ptr = 0;
	if ( sl )
	{
		ptr = SafeLibCreate<ProviderIFCBaseIFC>::create(sl, "createProviderIFC", lgr);
	}
	else
	{
		OW_THROW(ProviderIFCLoaderException, Format("ProviderIFCBaseIFCLoaderBase::createProviderIFCFromLib FAILED loading library %1", libname).c_str());
	}
	ProviderIFCBaseIFCRef retval(sl, ptr);
	return retval;
}

///////////////////////////////////////////////////////////////////////////////
void
ProviderIFCLoader::loadIFCs(Array<ProviderIFCBaseIFCRef>& ifcs) const
{
	ServiceEnvironmentIFCRef env = getEnvironment();
	String libdir = env->getConfigItem(ConfigOpts::PROVIDER_IFC_LIBS_opt, OW_DEFAULT_IFC_LIBS);
	LoggerRef lgr(env->getLogger(COMPONENT_NAME));
	OW_LOG_DEBUG(lgr, Format("ProviderIFCBaseIFCLoaderBase::loadIFC getting provider interfaces from: %1", libdir));
	StringArray libs;
	FileSystem::getDirectoryContents(libdir, libs);
	if (libs.size() == 0)
	{
		OW_THROW(ProviderIFCLoaderException, "ProviderIFCBaseIFCLoaderBase::loadIFCs did not find any provider interfaces");
		return;
	}
	int ifcCount = 0;
	for (StringArray::size_type i = 0; i < libs.size(); ++i)
	{
		if (!libs[i].endsWith(OW_SHAREDLIB_EXTENSION))
		{
			continue;
		}
#ifdef OW_DARWIN
		if (libs[i].indexOf(OW_VERSION) != String::npos)
		{
				continue;
		}
#endif // OW_DARWIN
		ProviderIFCBaseIFCRef rval;
		ProviderIFCBaseIFCRef pmr;
		rval = createProviderIFCFromLib(libdir + OW_FILENAME_SEPARATOR + libs[i]);
		if (rval)
		{
			ifcCount++;
			ifcs.push_back(rval);
		}
		else
		{
			OW_LOG_ERROR(lgr, Format("Unable to load ProviderIFC library %1", libs[i]));
		}
	}
	OW_LOG_DEBUG(lgr, Format("Number of provider interfaces loaded: %1",
		ifcCount));
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
ProviderIFCLoaderRef
ProviderIFCLoader::createProviderIFCLoader(ServiceEnvironmentIFCRef env)
{
	return ProviderIFCLoaderRef(new ProviderIFCLoader(
		SharedLibraryLoader::createSharedLibraryLoader(), env));
}
//////////////////////////////////////////////////////////////////////////////
ProviderIFCLoaderBase::~ProviderIFCLoaderBase()
{
}
//////////////////////////////////////////////////////////////////////////////
ProviderIFCLoader::~ProviderIFCLoader()
{
}

} // end namespace OW_NAMESPACE

