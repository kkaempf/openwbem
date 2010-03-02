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

#include "OW_PrivilegeManagerMockObject.hpp"

namespace OW_NAMESPACE
{
	using namespace blocxx;

	OW_DECLARE_EXCEPTION2(PrivilegeManagerMockObjectUnimplemented, PrivilegeManagerException);
	OW_DEFINE_EXCEPTION2(PrivilegeManagerMockObjectUnimplemented, PrivilegeManagerException);

	PrivilegeManagerMockObject::PrivilegeManagerMockObject()
	{
	}

	PrivilegeManagerMockObject::~PrivilegeManagerMockObject()
	{
	}

	AutoDescriptor PrivilegeManagerMockObject::open(const char* pathname, PrivilegeManager::OpenFlags flags, PrivilegeManager::OpenPerms perms)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "open");
	}

	FileSystem::FileInformation PrivilegeManagerMockObject::stat(const char* pathname)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "stat");
	}

	FileSystem::FileInformation PrivilegeManagerMockObject::lstat(const char* pathname)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "lstat");
	}

	StringArray PrivilegeManagerMockObject::readDirectory(const char* pathname, PrivilegeManager::ReadDirOptions opt)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "readDirectory");
	}

	String PrivilegeManagerMockObject::readLink(const char* pathname)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "readLink");
	}

	void PrivilegeManagerMockObject::rename(const char* oldpath, const char* newpath)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "rename");
	}

	bool PrivilegeManagerMockObject::removeFile(const char* path)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "removeFile");
	}

	bool PrivilegeManagerMockObject::removeDirectory(const char* path)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "removeDirectory");
	}

	ProcessRef PrivilegeManagerMockObject::monitoredSpawn(
		const char* exec_path, const char* app_name,
		const char* const argv[], const char* const envp[])
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "monitoredSpawn");
	}

	ProcessRef PrivilegeManagerMockObject::monitoredUserSpawn(
		const char* exec_path, const char* app_name,
		const char* const argv[], const char* const envp[],
		const char* user)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "monitoredUserSpawn");
	}

	ProcessRef PrivilegeManagerMockObject::userSpawn(
		const char* exec_path,
		const char* const argv[], const char* const envp[],
		const char* user,
		const char* working_dir)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "userSpawn");
	}

	void PrivilegeManagerMockObject::userSpawnDaemon(
		const char* exec_path,
		const char* const argv[], const char* const envp[],
		const char* user,
		const char* working_dir)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "userSpawnDaemon");
	}

	int PrivilegeManagerMockObject::kill(ProcId pid, int sig)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "kill");
	}

	Process::Status PrivilegeManagerMockObject::pollStatus(ProcId pid)
	{
		OW_THROW(PrivilegeManagerMockObjectUnimplementedException, "pollStatus");
	}

} // end namespace OW_NAMESPACE
