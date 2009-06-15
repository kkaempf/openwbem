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
#ifndef OW_MONITORED_PERL_HPP_INCLUDE_GUARD_
#define OW_MONITORED_PERL_HPP_INCLUDE_GUARD_

/**
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "blocxx/Cstr.hpp"

namespace OW_NAMESPACE
{

class PrivilegeManager;

namespace MonitoredPerl
{
	/**
	* @param script_path Perl script to run
	* @param app_name Name of privilege configuration file to use
	* @param script_args The null-terminated argument list for the
	*   Perl script, NOT including @a script_path itself.  A null value
	*   indicates no arguments.
	* @param envp The null-terminated environment for the Perl script
	*   (@see Exec::Spawn for details).
	*
	* Runs the Perl interpreter at <OWLIBEXEC>/perl, telling it to look in
	* <OWLIB>/monitoredPerl/lib and <OWLIB>/monitoredPerl/arch for the
	* monitored Perl extensions.  <OWLIBEXEC> is
	* ConfigOpts::installed_owlibexec_dir and <OWLIB> is
	* ConfigOpts::installed_owlib_dir.  
	*/
	ProcessRef monitoredPerl(
		char const * script_path, char const * app_name,
		char const * const script_args[],
		char const * const envp[]
	);

	template <typename S1, typename S2, typename SA1, typename SA2>
	ProcessRef monitoredPerl(
		S1 const & script_path, S2 const & app_name,
		SA1 const & script_args, SA2 const & envp
	)
	{
		Cstr::CstrArr<SA1> sa_argv(script_args);
		Cstr::CstrArr<SA2> sa_envp(envp);
		return monitoredPerl(
			Cstr::to_char_ptr(script_path), Cstr::to_char_ptr(app_name),
			sa_argv.sarr, sa_envp.sarr
		);
	}
	
} // namespace MonitoredPerl

} // namespace OW_NAMESPACE

#endif
