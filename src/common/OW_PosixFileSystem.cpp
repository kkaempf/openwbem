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
#include "OW_config.h"
#include "OW_FileSystem.hpp"
#include "OW_RandomNumber.hpp"
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
}
#include <cstdio> // for rename
#include <fstream>

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION(FileSystem)

namespace FileSystem
{

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
changeFileOwner(const String& filename,
	const UserId& userId)
{
	return ::chown(filename.c_str(), userId, gid_t(-1));
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
openFile(const String& path)
{
	return File(::open(path.c_str(), O_RDWR));
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
createFile(const String& path)
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
openOrCreateFile(const String& path)
{
	return File(::open(path.c_str(), O_RDWR | O_CREAT, 0660));
}
//////////////////////////////////////////////////////////////////////////////
bool
exists(const String& path)
{
	return access(path.c_str(), F_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
canRead(const String& path)
{
	return access(path.c_str(), R_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
canWrite(const String& path)
{
	return access(path.c_str(), W_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
isDirectory(const String& path)
{
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}
//////////////////////////////////////////////////////////////////////////////
bool
changeDirectory(const String& path)
{
	return chdir(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
makeDirectory(const String& path, int mode)
{
	return mkdir(path.c_str(), mode) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
getFileSize(const String& path, UInt32& size)
{
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;
	size = st.st_size;
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
removeDirectory(const String& path)
{
	return rmdir(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
removeFile(const String& path)
{
	return unlink(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
getDirectoryContents(const String& path,
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
renameFile(const String& oldFileName,
	const String& newFileName)
{
	return ::rename(oldFileName.c_str(), newFileName.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
size_t
read(FileHandle& hdl, void* bfr, size_t numberOfBytes,
	long offset)
{
	if(offset != -1L)
	{
		::lseek(hdl, offset, SEEK_SET);
	}
	return ::read(hdl, bfr, numberOfBytes);
}
//////////////////////////////////////////////////////////////////////////////
size_t
write(FileHandle& hdl, const void* bfr, size_t numberOfBytes,
	long offset)
{
	if(offset != -1L)
	{
		::lseek(hdl, offset, SEEK_SET);
	}
	return ::write(hdl, bfr, numberOfBytes);
}
//////////////////////////////////////////////////////////////////////////////
int
seek(FileHandle& hdl, off_t offset, int whence)
{
	return ::lseek(hdl, offset, whence);
}
//////////////////////////////////////////////////////////////////////////////
off_t
tell(FileHandle& hdl)
{
	return ::lseek(hdl, 0, SEEK_CUR);
}
//////////////////////////////////////////////////////////////////////////////
void
rewind(FileHandle& hdl)
{
	::lseek(hdl, 0, SEEK_SET);
}
//////////////////////////////////////////////////////////////////////////////
int
close(FileHandle& hdl)
{
	return ::close(hdl);
}
//////////////////////////////////////////////////////////////////////////////
int
flush(FileHandle&)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void
initRandomFile(const String& filename)
{
	int hdl = ::open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
	if (hdl == -1)
	{
		OW_THROW(FileSystemException, Format("Can't open random file %1 for writing", filename).c_str());
	}
	RandomNumber rnum(0, 0xFF);
	for (size_t i = 0; i < 1024; ++i)
	{
		char c = rnum.getNextNumber();
		::write(hdl, &c, 1);
	}
	::close(hdl);
}

//////////////////////////////////////////////////////////////////////////////
String getFileContents(const String& filename)
{
	std::ifstream in(filename.c_str());
	if (!in)
	{
		OW_THROW(FileSystemException, Format("Failed to open file %1", filename).c_str());
	}
	OStringStream ss;
	ss << in.rdbuf();
	return ss.toString();
}

//////////////////////////////////////////////////////////////////////////////
StringArray getFileLines(const String& filename)
{
	return getFileContents(filename).tokenize("\r\n");
}

} // end namespace FileSystem
} // end namespace OpenWBEM

