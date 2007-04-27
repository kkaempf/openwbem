/*******************************************************************************
* Copyright (C) 2005, Quest Software, Inc. All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates, nor Quest Software, Inc., nor the
*       names of its contributors or employees may be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_Assertion.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_MonitoredPerl.hpp"
#include "OW_PrivilegeManager.hpp"
#include "OW_Logger.hpp"
#include "OW_Format.hpp"

#include <vector>

using std::vector;

namespace OW_NAMESPACE
{

namespace
{
	String perl_executable_path()
	{
		return ConfigOpts::installed_owlibexec_dir + "/perl";
	}

	String perl_module_path()
	{
		return ConfigOpts::installed_owlib_dir + "/monitoredPerl/lib";
	}

	String perl_shared_lib_path()
	{
		return ConfigOpts::installed_owlib_dir + "/monitoredPerl/arch";
	}

#ifdef OW_HPUX
	String ld_preload_setting()
	{
		// This will have to be made configurable if we want to run
		// all of the monitor tests on HPUX.
		Logger logger("ld_preload");

#if defined(OW_ARCH_IA64)
		String preloads("LD_PRELOAD=/usr/lib/hpux64/libpthread" OW_SHAREDLIB_EXTENSION
			":" OW_DEFAULT_LIB_DIR "/libopenwbem" OW_SHAREDLIB_EXTENSION);
		OW_LOG_DEBUG(logger, Format("returning itanium preloads: %1", preloads));
#else

		String preloads("LD_PRELOAD=/usr/lib/libCsup_v2" OW_SHAREDLIB_EXTENSION
			":/usr/lib/libstd_v2" OW_SHAREDLIB_EXTENSION
			":/usr/lib/libpthread" OW_SHAREDLIB_EXTENSION
			":" OW_DEFAULT_LIB_DIR "/libopenwbem" OW_SHAREDLIB_EXTENSION);
		OW_LOG_DEBUG(logger, Format("returning parisc preloads.", preloads));
#endif
		return preloads;
	}
#endif


	class CstrArrStorage
	{
	public:
		void push_back(String const & s)
		{
			OW_ASSERT(m_cstr_arr.empty());
			m_str_arr.push_back(s);
		}

		void append_final(char const * const * src);

		char const * const * arr() const
		{
			OW_ASSERT(!m_cstr_arr.empty() && m_cstr_arr.back() == 0);
			return &m_cstr_arr[0];
		}

	private:
		vector<char const *> m_cstr_arr;
		vector<String> m_str_arr;
	};

	void CstrArrStorage::append_final(char const * const * src)
	{
		OW_ASSERT(m_cstr_arr.empty());
		vector<char const *> & dst = m_cstr_arr;
		size_t n = m_str_arr.size();
		for (size_t i = 0; i < n; ++i)
		{
			dst.push_back(m_str_arr[i].c_str());
		}
		for ( ; src && *src; ++src)
		{
			dst.push_back(*src);
		}
		dst.push_back(0);
	}

	class PerlArgs
	{
	public:
		PerlArgs(
			char const * script_path, char const * const * script_args,
			char const * const * envp)
		{
			m_argv.push_back(perl_executable_path());
			m_argv.push_back("-I" + perl_module_path());
			m_argv.push_back("-I" + perl_shared_lib_path());
			m_argv.push_back(script_path);
			m_argv.append_final(script_args);

#ifdef OW_HPUX
			// Needed to avoid the problem that a dynamically-loaded shared
			// library cannot depend on another shared library that contains
			// thread-local storage, unless the latter is loaded at startup.
			// See documentation for shl_load, which is called from the Perl
			// dl_load_file function, which is called in the DynaLoader
			// Perl module.
			m_env.push_back(ld_preload_setting());
#endif
			m_env.append_final(envp);
		}

		char const * const * argv() const
		{
			return m_argv.arr();
		}

		char const * const * env() const
		{
			return m_env.arr();
		}

	private:
		CstrArrStorage m_argv;
		CstrArrStorage m_env;
	};
} // anonymous namespace

namespace MonitoredPerl
{

	ProcessRef monitoredPerl(
		char const * script_path, char const * app_name,
		char const * const script_args[],
		char const * const envp[]
	)
	{
		PerlArgs perl_args(script_path, script_args, envp);
		char const * const * argv = perl_args.argv();
		return PrivilegeManager::getPrivilegeManager().monitoredSpawn(
			argv[0], app_name, argv, perl_args.env()
		);
	}
	
} // namespace MonitoredPerl

} // namespace OW_NAMESPACE
