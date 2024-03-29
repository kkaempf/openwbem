/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "blocxx/SharedLibraryException.hpp"
#include "OW_ProviderIFCLoader.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/SignalScope.hpp"
#include "blocxx/Logger.hpp"
#include "OW_ConfigOpts.hpp"
#include "blocxx/FileSystem.hpp"
#include "OW_SafeLibCreate.hpp"
#include "OW_Exception.hpp"

namespace OW_NAMESPACE
{
using namespace blocxx;

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
	Logger lgr(COMPONENT_NAME);
	BLOCXX_LOG_DEBUG3(lgr, Format("ProviderIFCBaseIFCLoaderBase::createProviderIFCFromLib loading library %1", libname));
	SharedLibraryRef sl = m_sll->loadSharedLibrary(libname);
	ProviderIFCBaseIFC* ptr = 0;
	if ( sl )
	{
		ptr = SafeLibCreate<ProviderIFCBaseIFC>::create(sl, "createProviderIFC", OW_VERSION);
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
	Logger lgr(COMPONENT_NAME);
	int ifcCount = 0;

	StringArray libdirs = env->getMultiConfigItem(ConfigOpts::PROVIDER_IFC_LIBS_opt, String(OW_DEFAULT_PROVIDER_IFC_LIBS).tokenize(), OW_PATHNAME_SEPARATOR);
	for (size_t i = 0; i < libdirs.size(); ++i)
	{
		const String libdir(libdirs[i]);
		BLOCXX_LOG_DEBUG(lgr, Format("ProviderIFCBaseIFCLoaderBase::loadIFC getting provider interfaces from: %1", libdir));
		StringArray libs;
		if (!FileSystem::getDirectoryContents(libdir, libs))
		{
			OW_THROW_ERRNO_MSG(ProviderIFCLoaderException, "ProviderIFCBaseIFCLoaderBase::loadIFCs failed to getDirectoryContents");
		}
		if (libs.size() == 0)
		{
			OW_THROW(ProviderIFCLoaderException, "ProviderIFCBaseIFCLoaderBase::loadIFCs did not find any provider interfaces");
		}
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
				BLOCXX_LOG_ERROR(lgr, Format("Unable to load ProviderIFC library %1", libs[i]));
			}
		}
	}
	BLOCXX_LOG_DEBUG(lgr, Format("Number of provider interfaces loaded: %1", ifcCount));
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

