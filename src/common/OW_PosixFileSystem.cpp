/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "OW_Format.hpp"
extern "C"
{
#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef OW_HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}
#include <cstdio> // for rename

namespace OpenWBEM
{

OW_DECLARE_EXCEPTION(FileSystem)
OW_DEFINE_EXCEPTION(FileSystem)

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
FileSystem::changeFileOwner(const String& filename,
	const UserId& userId)
{
	return ::chown(filename.c_str(), userId, gid_t(-1));
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
FileSystem::openFile(const String& path)
{
	return File(::open(path.c_str(), O_RDWR));
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
FileSystem::createFile(const String& path)
{
	int fd = ::open(path.c_str(), O_CREAT | O_EXCL | O_TRUNC | O_RDWR, 0660);
	if(fd != -1)
	{
		return File(fd);
	}
	return File();
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
FileSystem::openOrCreateFile(const String& path)
{
	return File(::open(path.c_str(), O_RDWR | O_CREAT, 0660));
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::exists(const String& path)
{
	return access(path.c_str(), F_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::canRead(const String& path)
{
	return access(path.c_str(), R_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::canWrite(const String& path)
{
	return access(path.c_str(), W_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::isDirectory(const String& path)
{
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::changeDirectory(const String& path)
{
	return chdir(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::makeDirectory(const String& path, int mode)
{
	return mkdir(path.c_str(), mode) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::getFileSize(const String& path, UInt32& size)
{
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;
	size = st.st_size;
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::removeDirectory(const String& path)
{
	return rmdir(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::removeFile(const String& path)
{
	return unlink(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::getDirectoryContents(const String& path,
	StringArray& dirEntries)
{
	static Mutex readdirGuard;
	MutexLock lock(readdirGuard);
	DIR* dp;
	struct dirent* dentry;
	if((dp = opendir(path.c_str())) == NULL)
	{
		return false;
	}
	dirEntries.clear();
	while((dentry = readdir(dp)) != NULL)
	{
		dirEntries.append(String(dentry->d_name));
	}
	closedir(dp);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
FileSystem::renameFile(const String& oldFileName,
	const String& newFileName)
{
	return ::rename(oldFileName.c_str(), newFileName.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
size_t
FileSystem::read(FileHandle& hdl, void* bfr, size_t numberOfBytes,
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
FileSystem::write(FileHandle& hdl, const void* bfr, size_t numberOfBytes,
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
FileSystem::seek(FileHandle& hdl, off_t offset, int whence)
{
	return ::lseek(hdl, offset, whence);
}
//////////////////////////////////////////////////////////////////////////////
off_t
FileSystem::tell(FileHandle& hdl)
{
	return ::lseek(hdl, 0, SEEK_CUR);
}
//////////////////////////////////////////////////////////////////////////////
void
FileSystem::rewind(FileHandle& hdl)
{
	::lseek(hdl, 0, SEEK_SET);
}
//////////////////////////////////////////////////////////////////////////////
int
FileSystem::close(FileHandle& hdl)
{
	return ::close(hdl);
}
//////////////////////////////////////////////////////////////////////////////
int
FileSystem::flush(FileHandle&)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void
FileSystem::initRandomFile(const String& filename)
{
	int hdl = ::open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
	if (hdl == -1)
	{
		OW_THROW(FileSystemException, format("Can't open random file %1 for writing", filename).c_str());
	}
	RandomNumber rnum(0, 0xFF);
	for (size_t i = 0; i < 1024; ++i)
	{
		char c = rnum.getNextNumber();
		::write(hdl, &c, 1);
	}
	::close(hdl);
}

} // end namespace OpenWBEM

