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
#include "OW_FileSystem.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_Exception.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_File.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"

extern "C"
{
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

//#include <cstring>

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_FileSystem::changeFileOwner(const OW_String& filename,
	const OW_UserId& userId)
{
	return ::chown(filename.c_str(), userId, gid_t(-1));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_File
OW_FileSystem::openFile(const OW_String& path)
{
	return OW_File(::open(path.c_str(), O_RDWR));
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_File
OW_FileSystem::createFile(const OW_String& path)
{
	// TODO: Fix up this function, it's subject to race conditions.
	if(!OW_FileSystem::exists(path))
	{
		return OW_File( ::open(path.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0660));
	}

	return OW_File();
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_File
OW_FileSystem::openOrCreateFile(const OW_String& path)
{
	return OW_File(::open(path.c_str(), O_RDWR | O_CREAT, 0660));
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::exists(const OW_String& path)
{
	return access(path.c_str(), F_OK) == 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::canRead(const OW_String& path)
{
	return access(path.c_str(), R_OK) == 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::canWrite(const OW_String& path)
{
	return access(path.c_str(), W_OK) == 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::isDirectory(const OW_String& path)
{
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;

	return S_ISDIR(st.st_mode);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::changeDirectory(const OW_String& path)
{
	return chdir(path.c_str()) == 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::makeDirectory(const OW_String& path)
{
	return mkdir(path.c_str(), 0777) == 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::getFileSize(const OW_String& path, OW_UInt32& size)
{
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;

	size = st.st_size;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::removeDirectory(const OW_String& path)
{
	return rmdir(path.c_str()) == 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::removeFile(const OW_String& path)
{
	return unlink(path.c_str()) == 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::getDirectoryContents(const OW_String& path,
	OW_StringArray& dirEntries)
{
	static OW_Mutex readdirGuard;
	OW_MutexLock lock(readdirGuard);
	DIR* dp;
	struct dirent* dentry;
	if((dp = opendir(path.c_str())) == NULL)
	{
		return false;
	}

	dirEntries.clear();
	while((dentry = readdir(dp)) != NULL)
	{
		dirEntries.append(OW_String(dentry->d_name));
	}
	closedir(dp);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_FileSystem::renameFile(const OW_String& oldFileName,
	const OW_String& newFileName)
{
	return ::rename(oldFileName.c_str(), newFileName.c_str()) == 0;
}

//////////////////////////////////////////////////////////////////////////////
size_t 
OW_FileSystem::read(OW_FileHandle& hdl, void* bfr, size_t numberOfBytes,
	long offset)
{
	if(offset != -1L)
	{
		::lseek(hdl, offset, SEEK_SET);
	}

#ifdef OW_USE_GNU_PTH
    return pth_read(hdl, bfr, numberOfBytes);
#else
	return ::read(hdl, bfr, numberOfBytes);
#endif
}

//////////////////////////////////////////////////////////////////////////////
size_t 
OW_FileSystem::write(OW_FileHandle& hdl, const void* bfr, size_t numberOfBytes,
	long offset)
{
	if(offset != -1L)
	{
		::lseek(hdl, offset, SEEK_SET);
	}

#ifdef OW_USE_GNU_PTH
    return pth_write(hdl, bfr, numberOfBytes);
#else
	return ::write(hdl, bfr, numberOfBytes);
#endif
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_FileSystem::seek(OW_FileHandle& hdl, OW_off_t offset, int whence)
{
	return ::lseek(hdl, offset, whence);
}

//////////////////////////////////////////////////////////////////////////////
OW_off_t 
OW_FileSystem::tell(OW_FileHandle& hdl)
{
	return ::lseek(hdl, 0, SEEK_CUR);
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_FileSystem::rewind(OW_FileHandle& hdl)
{
	::lseek(hdl, 0, SEEK_SET);
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_FileSystem::close(OW_FileHandle& hdl)
{
	return ::close(hdl);
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_FileSystem::flush(OW_FileHandle&)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_FileSystem::initRandomFile(const OW_String& filename)
{
	int hdl = ::open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_EXCL, 0600);
	if (hdl == -1)
	{
		OW_THROW(OW_Exception, "Can't open random file for writing");
	}
	OW_RandomNumber rnum(0, 0xFF);
	for (size_t i = 0; i < 1024; ++i)
	{
		char c = (char)rnum.getNextNumber();
		::write(hdl, &c, 1);
	}
	::close(hdl);
}


