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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_FileSystem.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_File.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_Format.hpp"
#include "OW_ExceptionIds.hpp"

extern "C"
{
#ifdef OW_WIN32

#include <direct.h>
#include <io.h>
#include <share.h>

#define _ACCESS ::_access
#define R_OK 4
#define F_OK 0
#define W_OK 2
#define _CHDIR _chdir
#define _MKDIR(a,b)	_mkdir((a))
#define _RMDIR _rmdir
#define _UNLINK _unlink
#define _LSEEK ::_lseek
#define _READ ::_read
#define _WRITE ::_write
#define _CLOSEFILE ::_close

#else

#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef OW_HAVE_DIRENT_H
#include <dirent.h>
#endif

#define _ACCESS ::access
#define _CHDIR chdir
#define _MKDIR(a,b) mkdir((a),(b))
#define _RMDIR rmdir
#define _UNLINK unlink
#define _LSEEK ::lseek
#define _READ ::read
#define _WRITE ::write
#define _CLOSEFILE ::close

#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
}

#include <cstdio> // for rename
#include <fstream>

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION_WITH_ID(FileSystem);

namespace FileSystem
{

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
changeFileOwner(const String& filename,
	const UserId& userId)
{
#ifdef OW_WIN32
	return 0;	// File ownership on Win32?
#else
	return ::chown(filename.c_str(), userId, gid_t(-1));
#endif
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
openFile(const String& path)
{
#ifdef OW_WIN32
	return File(::_sopen( path.c_str(), _O_RDWR, _SH_DENYNO, 
        _S_IREAD | _S_IWRITE ));
#else
	return File(::open(path.c_str(), O_RDWR));
#endif
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
createFile(const String& path)
{
#ifdef OW_WIN32
	int fd = ::_sopen(path.c_str(), _O_CREAT | _O_EXCL | _O_TRUNC | _O_RDWR,
		_SH_DENYNO, _S_IREAD | _S_IWRITE);
#else
	int fd = ::open(path.c_str(), O_CREAT | O_EXCL | O_TRUNC | O_RDWR, 0660);
#endif

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
#ifdef OW_WIN32
	return File(::_sopen(path.c_str(), O_RDWR | O_CREAT, _SH_DENYNO,
		_S_IREAD | _S_IWRITE));
#else
	return File(::open(path.c_str(), O_RDWR | O_CREAT, 0660));
#endif
}

//////////////////////////////////////////////////////////////////////////////
bool
exists(const String& path)
{
	return _ACCESS(path.c_str(), F_OK) == 0;
}

//////////////////////////////////////////////////////////////////////////////
bool
canRead(const String& path)
{
	return _ACCESS(path.c_str(), R_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
canWrite(const String& path)
{
	return _ACCESS(path.c_str(), W_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
isDirectory(const String& path)
{
#ifdef OW_WIN32
	struct _stat st;
	if(_stat(path.c_str(), &st) != 0)
		return false;
	return ((st.st_mode & _S_IFDIR) != 0);
#else
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
#endif
}
//////////////////////////////////////////////////////////////////////////////
bool
changeDirectory(const String& path)
{
	return _CHDIR(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
makeDirectory(const String& path, int mode)
{
	return _MKDIR(path.c_str(), mode) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
getFileSize(const String& path, off_t& size)
{
#ifdef OW_WIN32
	struct _stat st;
	if(_stat(path.c_str(), &st) != 0)
		return false;
#else
	struct stat st;
	if(stat(path.c_str(), &st) != 0)
		return false;
#endif
	size = st.st_size;
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
removeDirectory(const String& path)
{
	return _RMDIR(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
removeFile(const String& path)
{
	return _UNLINK(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
getDirectoryContents(const String& path,
	StringArray& dirEntries)
{
	static Mutex readdirGuard;
	MutexLock lock(readdirGuard);

#ifdef OW_WIN32
    struct _finddata_t dentry;
    long hFile;
    // Find first directory entry
    if((hFile = _findfirst( "*", &dentry)) == -1L)
	{
		return false;
	}
	dirEntries.clear();
	while(_findnext(hFile, &dentry) == 0)
	{
		dirEntries.append(String(dentry.name));
	}
	_findclose(hFile);
#else
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
#endif
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
	off_t offset)
{
	if(offset != -1L)
	{
		_LSEEK(hdl, offset, SEEK_SET);
	}
	return _READ(hdl, bfr, numberOfBytes);
}
//////////////////////////////////////////////////////////////////////////////
size_t
write(FileHandle& hdl, const void* bfr, size_t numberOfBytes,
	off_t offset)
{
	if(offset != -1L)
	{
		_LSEEK(hdl, offset, SEEK_SET);
	}
	return _WRITE(hdl, bfr, numberOfBytes);
}
//////////////////////////////////////////////////////////////////////////////
off_t
seek(FileHandle& hdl, off_t offset, int whence)
{
	return _LSEEK(hdl, offset, whence);
}
//////////////////////////////////////////////////////////////////////////////
off_t
tell(FileHandle& hdl)
{
	return _LSEEK(hdl, 0, SEEK_CUR);
}
//////////////////////////////////////////////////////////////////////////////
void
rewind(FileHandle& hdl)
{
	_LSEEK(hdl, 0, SEEK_SET);
}
//////////////////////////////////////////////////////////////////////////////
int
close(FileHandle& hdl)
{
	return _CLOSEFILE(hdl);
}
//////////////////////////////////////////////////////////////////////////////
int
flush(FileHandle& hdl)
{
#ifdef OW_WIN32
	return _commit(hdl);
#else
	#ifdef OW_DARWIN
		return ::fsync(hdl);
	#else
		return 0;
	#endif
#endif
}
//////////////////////////////////////////////////////////////////////////////
void
initRandomFile(const String& filename)
{
#ifdef OW_WIN32
	int hdl = ::_sopen(filename.c_str(), _O_CREAT | _O_TRUNC | _O_WRONLY,
		_SH_DENYNO, _S_IREAD | _S_IWRITE );
#else
	int hdl = ::open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
#endif

	if (hdl == -1)
	{
		OW_THROW(FileSystemException, Format("Can't open random file %1 for writing", filename).c_str());
	}
	RandomNumber rnum(0, 0xFF);
	for (size_t i = 0; i < 1024; ++i)
	{
		char c = rnum.getNextNumber();
		_WRITE(hdl, &c, 1);
	}
	_CLOSEFILE(hdl);
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

