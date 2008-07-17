#ifndef OW_PRIVILEGE_MANAGER_MOCK_OBJECT_HPP_INCLUDE_GUARD_
#define OW_PRIVILEGE_MANAGER_MOCK_OBJECT_HPP_INCLUDE_GUARD_

/*******************************************************************************
* Copyright (C) 2008, Quest Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Network Associates,
*       nor Quest Software, Inc., nor Novell, Inc., nor the
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
 * @author Kevin Harris
 */

#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_PrivManOpenFlags.h"
#include "OW_Process.hpp"
#include "OW_Types.hpp"
#include "OW_AutoDescriptor.hpp"
#include "OW_FileSystem.hpp"
#include "OW_PrivilegeManager.hpp"


namespace OW_NAMESPACE
{
	/**
	 * This class can be used to modify/replace the behavior of the PrivilegeManager functions.
	 */
	class PrivilegeManagerMockObject
	{
	public:
		PrivilegeManagerMockObject();
		virtual ~PrivilegeManagerMockObject();

		virtual AutoDescriptor open(const char* pathname, PrivilegeManager::OpenFlags flags, PrivilegeManager::OpenPerms perms);
		virtual FileSystem::FileInformation stat(const char* pathname);
		virtual FileSystem::FileInformation lstat(const char* pathname);
		virtual StringArray readDirectory(const char* pathname, PrivilegeManager::ReadDirOptions opt);
		virtual String readLink(const char* pathname);
		virtual void rename(const char* oldpath, const char* newpath);
		virtual bool removeFile(const char* path);
		virtual bool removeDirectory(const char* path);
		virtual ProcessRef monitoredSpawn(
			const char* exec_path, const char* app_name,
			const char* const argv[], const char* const envp[]);
		virtual ProcessRef monitoredUserSpawn(
			const char* exec_path, const char* app_name,
			const char* const argv[], const char* const envp[],
			const char* user);
		virtual ProcessRef userSpawn(
			const char* execpath,
			const char* const argv[], const char* const envp[],
			const char* user,
			const char* working_dir);

		virtual int kill(ProcId pid, int sig);
		virtual Process::Status pollStatus(ProcId pid);
	};

} // namespace OW_NAMESPACE
#endif
