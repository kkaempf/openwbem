#ifndef OW_SECURE_HPP_INCLUDE_GUARD_
#define OW_SECURE_HPP_INCLUDE_GUARD_

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
*     * Neither the name of Quest Software, Inc., nor the
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

/// @author Kevin S. Van Horn

#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CommonFwd.hpp"
#include <sys/types.h>
#include <utility>
#include <vector>

namespace OW_NAMESPACE
{
namespace Secure
{
	/**
	* Secure::ProcessAbortException should be caught only at the top level of
	* the program.  The process should then exit after optionally writing an
	* error message somewhere.
	*/
	OW_DECLARE_EXCEPTION(ProcessAbort);

	/**
	* Changes both the effective and actual user ID to @a newuid, and
	* the effective and actual group IDs to @a newgid, clearing out all auxiliary
	* groups.  If @a newuid == -1, defaults to the actual user ID.
	* If @a newgid == -1, defaults to the actual group ID.
	*
	* @pre Currently running as root (both uid and euid).
	*/
	void dropPrivilegesPermanently(::uid_t newuid, ::gid_t newgid);

	/**
	* Look up user ID and group ID for username in password file, chdir
	* to "/", then drop privileges and run with that user ID and group ID.
	*
	* @pre Currently running as root (both uid and euid).
	*/
	void runAs(char const * username);

	/**
	* @return A minimal environment appropriate for the platform.
	*/
	StringArray minimalEnvironment();

} // namespace Secure
} // namespace OW_NAMESPACE

#endif

