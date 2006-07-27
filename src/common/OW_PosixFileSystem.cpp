/*******************************************************************************
* Copyright (C) 2001-2005 Quest Software, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 * @author Kevin S. Van Horn
 */

#include "OW_config.h"
#include "OW_FileSystem.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_File.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_Format.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_Assertion.hpp"
#include "OW_GlobalPtr.hpp"
#include "OW_FileSystemMockObject.hpp"

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

#ifdef OW_NETWARE
#define MAXSYMLINKS 20
#endif

#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
}

#include <cstdio> // for rename
#include <fstream>
#include <cerrno>

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(FileSystem);

namespace FileSystem
{

typedef GlobalPtr<FileSystemMockObject, NullFactory> FileSystemMockObject_t;
FileSystemMockObject_t g_fileSystemMockObject = OW_GLOBAL_PTR_INIT;

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
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->openFile(path);
	}
#ifdef OW_WIN32
	HANDLE fh = ::CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	return (fh  != INVALID_HANDLE_VALUE) ? File(fh) : File();
#else
	return File(::open(path.c_str(), O_RDWR));
#endif
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
createFile(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->createFile(path);
	}
#ifdef OW_WIN32
	HANDLE fh = ::CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, NULL);
	return (fh  != INVALID_HANDLE_VALUE) ? File(fh) : File();
#else
	int fd = ::open(path.c_str(), O_CREAT | O_EXCL | O_TRUNC | O_RDWR, 0660);
	if (fd != -1)
	{
		return File(fd);
	}
	return File();
#endif

}
//////////////////////////////////////////////////////////////////////////////
// STATIC
File
openOrCreateFile(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->openOrCreateFile(path);
	}
#ifdef OW_WIN32
	HANDLE fh = ::CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	return (fh  != INVALID_HANDLE_VALUE) ? File(fh) : File();
#else
	return File(::open(path.c_str(), O_RDWR | O_CREAT, 0660));
#endif
}

//////////////////////////////////////////////////////////////////////////////
bool
exists(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->exists(path);
	}
	return _ACCESS(path.c_str(), F_OK) == 0;
}

//////////////////////////////////////////////////////////////////////////////
#ifndef OW_WIN32
bool
isExecutable(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->isExecutable(path);
	}
	return _ACCESS(path.c_str(), X_OK) == 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////
bool
canRead(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->canRead(path);
	}
	return _ACCESS(path.c_str(), R_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
canWrite(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->canWrite(path);
	}
	return _ACCESS(path.c_str(), W_OK) == 0;
}
//////////////////////////////////////////////////////////////////////////////
#ifndef OW_WIN32
bool
isLink(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->isLink(path);
	}
	struct stat st;
	if (lstat(path.c_str(), &st) != 0)
	{
		return false;
	}
	return S_ISLNK(st.st_mode);
}
#endif
//////////////////////////////////////////////////////////////////////////////
bool
isDirectory(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->isDirectory(path);
	}
#ifdef OW_WIN32
	struct _stat st;
	if (_stat(path.c_str(), &st) != 0)
	{
		return false;
	}
	return ((st.st_mode & _S_IFDIR) != 0);
#else
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
	{
		return false;
	}
	return S_ISDIR(st.st_mode);
#endif
}
//////////////////////////////////////////////////////////////////////////////
bool
changeDirectory(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->changeDirectory(path);
	}
	return _CHDIR(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
makeDirectory(const String& path, int mode)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->makeDirectory(path, mode);
	}
	return _MKDIR(path.c_str(), mode) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
getFileSize(const String& path, Int64& size)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->getFileSize(path, size);
	}
#ifdef OW_WIN32
	struct _stat st;
	if (_stat(path.c_str(), &st) != 0)
	{
		return false;
	}
#else
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
	{
		return false;
	}
#endif
	size = st.st_size;
	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool
removeDirectory(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->removeDirectory(path);
	}
	return _RMDIR(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
removeFile(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->removeFile(path);
	}
	return _UNLINK(path.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
bool
getDirectoryContents(const String& path,
	StringArray& dirEntries)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->getDirectoryContents(path, dirEntries);
	}
	static Mutex readdirGuard;
	MutexLock lock(readdirGuard);

#ifdef OW_WIN32
	struct _finddata_t dentry;
	long hFile;
	String _path = path;

	// Find first directory entry
	if (!_path.endsWith(OW_FILENAME_SEPARATOR))
	{
		_path += OW_FILENAME_SEPARATOR;
	}
	_path += "*";
    if ((hFile = _findfirst( _path.c_str(), &dentry)) == -1L)
	{
		return false;
	}
	dirEntries.clear();
	while (_findnext(hFile, &dentry) == 0)
	{
		dirEntries.append(String(dentry.name));
	}
	_findclose(hFile);
#else
	DIR* dp(0);
	struct dirent* dentry(0);
	if ((dp = opendir(path.c_str())) == NULL)
	{
		return false;
	}
	dirEntries.clear();
	while ((dentry = readdir(dp)) != NULL)
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
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->renameFile(oldFileName, newFileName);
	}
	return ::rename(oldFileName.c_str(), newFileName.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
size_t
read(const FileHandle& hdl, void* bfr, size_t numberOfBytes,
	Int64 offset)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->read(hdl, bfr, numberOfBytes, offset);
	}
#ifdef OW_WIN32
	OVERLAPPED ov = { 0, 0, 0, 0, NULL };
	OVERLAPPED *pov = NULL;
	if(offset != -1L)
	{
		ov.Offset = (DWORD) offset;
		// check for truncation
		if (ov.Offset != offset) 
		{
			OW_THROW(FileSystemException, "offset out of range");
		}
		pov = &ov;
	}

	DWORD bytesRead;
	size_t cc = (size_t)-1;
	if(::ReadFile(hdl, bfr, (DWORD)numberOfBytes, &bytesRead, pov))
	{
		cc = (size_t)bytesRead;
	}
		
	return cc;
#else
	if (offset != -1L)
	{
		::off_t offset2 = static_cast< ::off_t>(offset);
		// check for truncation
		if (offset2 != offset) 
		{
			OW_THROW(FileSystemException, "offset out of range");
		}

		::lseek(hdl, offset2, SEEK_SET);
	}
	return ::read(hdl, bfr, numberOfBytes);
#endif
}
//////////////////////////////////////////////////////////////////////////////
size_t
write(FileHandle hdl, const void* bfr, size_t numberOfBytes,
	Int64 offset)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->write(hdl, bfr, numberOfBytes, offset);
	}
#ifdef OW_WIN32
	OVERLAPPED ov = { 0, 0, 0, 0, NULL };
	OVERLAPPED *pov = NULL;
	if(offset != -1L)
	{
		ov.Offset = (DWORD) offset;
		// check for truncation
		if (ov.Offset != offset) 
		{
			OW_THROW(FileSystemException, "offset out of range");
		}
		pov = &ov;
	}

	DWORD bytesWritten;
	size_t cc = (size_t)-1;
	if(::WriteFile(hdl, bfr, (DWORD)numberOfBytes, &bytesWritten, pov))
	{
		cc = (size_t)bytesWritten;
	}
	return cc;
#else

	if (offset != -1L)
	{
		::off_t offset2 = static_cast< ::off_t>(offset);
		// check for truncation
		if (offset2 != offset) 
		{
			OW_THROW(FileSystemException, "offset out of range");
		}
		::lseek(hdl, offset2, SEEK_SET);
	}
	return ::write(hdl, bfr, numberOfBytes);
#endif
}

//////////////////////////////////////////////////////////////////////////////
Int64
seek(const FileHandle& hdl, Int64 offset, int whence)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->seek(hdl, offset, whence);
	}
#ifdef OW_WIN32
	DWORD moveMethod;
	switch(whence)
	{
		case SEEK_END: moveMethod = FILE_END; break;
		case SEEK_CUR: moveMethod = FILE_CURRENT; break;
		default: moveMethod = FILE_BEGIN; break;
	}

	LARGE_INTEGER li;
	li.QuadPart = offset;
	li.LowPart = SetFilePointer(hdl, li.LowPart, &li.HighPart, moveMethod);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
	   li.QuadPart = -1;
	}

	return li.QuadPart;
#else
	::off_t offset2 = static_cast< ::off_t>(offset);
	// check for truncation
	if (offset2 != offset) 
	{
		OW_THROW(FileSystemException, "offset out of range");
	}
	return ::lseek(hdl, offset2, whence);
#endif
}
//////////////////////////////////////////////////////////////////////////////
Int64
tell(const FileHandle& hdl)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->tell(hdl);
	}
#ifdef OW_WIN32
	LARGE_INTEGER li;
	li.QuadPart = 0;
	li.LowPart = SetFilePointer(hdl, li.LowPart, &li.HighPart, FILE_CURRENT);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
	   li.QuadPart = -1;
	}

	return li.QuadPart;
#else
	return ::lseek(hdl, 0, SEEK_CUR);
#endif
}
//////////////////////////////////////////////////////////////////////////////
#ifndef OW_WIN32
UInt64 fileSize(FileHandle fh)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->fileSize(fh);
	}
	struct stat st;
	int rc = ::fstat(fh, &st);
	if (rc != 0)
	{
		OW_THROW_ERRNO_MSG(FileSystemException, "Could not stat file handle: ");
	}
	return st.st_size;
}
#endif
//////////////////////////////////////////////////////////////////////////////
void
rewind(const FileHandle& hdl)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->rewind(hdl);
	}
#ifdef OW_WIN32
	::SetFilePointer(hdl, 0L, NULL, FILE_BEGIN);
#else
	::lseek(hdl, 0, SEEK_SET);
#endif
}
//////////////////////////////////////////////////////////////////////////////
int
close(const FileHandle& hdl)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->close(hdl);
	}
#ifdef OW_WIN32
	return (::CloseHandle(hdl)) ? 0 : -1;
#else
	return ::close(hdl);
#endif
}
//////////////////////////////////////////////////////////////////////////////
int
flush(FileHandle& hdl)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->flush(hdl);
	}
#ifdef OW_WIN32
	return (::FlushFileBuffers(hdl)) ? 0 : -1;
#else
	return ::fsync(hdl);
#endif
}
//////////////////////////////////////////////////////////////////////////////
String getFileContents(const String& filename)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->getFileContents(filename);
	}
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
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->getFileLines(filename);
	}
	return getFileContents(filename).tokenize("\r\n");
}

//////////////////////////////////////////////////////////////////////////////
String readSymbolicLink(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->readSymbolicLink(path);
	}
#ifdef OW_WIN32
	return Path::realPath(path);
#else
	std::vector<char> buf(MAXPATHLEN + 1);
	int rc;
	while (true)
	{
		rc = ::readlink(path.c_str(), &buf[0], buf.size());
		// If the link value is too big to fit into buf, but
		// there is no other error, then rc == buf.size(); in particular,
		// we do NOT get rc < 0 with errno == ENAMETOOLONG (this indicates
		// a problem with the input path, not the link value returned).
		if (rc < 0)
		{
			OW_THROW_ERRNO_MSG(FileSystemException, path);
		}
		else if (static_cast<unsigned>(rc) == buf.size())
		{
			buf.resize(buf.size() * 2);
		}
		else
		{
			buf.resize(rc);
			buf.push_back('\0');
			return String(&buf[0]);
		}
	}
#endif
	// Not reachable.
	return String();
}

//////////////////////////////////////////////////////////////////////////////
namespace Path
{

//////////////////////////////////////////////////////////////////////////////
String realPath(const String& path)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->realPath(path);
	}
#ifdef OW_WIN32
	char c, *bfr, *pname;
	const char *pathcstr;
	DWORD cc;

	pathcstr = path.c_str();
	while (*pathcstr == '/' || *pathcstr == '\\')
	{
		++pathcstr;
	}

	// if we ate some '\' or '/' chars, the back up to
	// allow for 1
	if(pathcstr != path.c_str())
	{
		--pathcstr;
	}
		
	cc = GetFullPathName(path.c_str(), 1, &c, &pname);
	if(!cc)
	{
		OW_THROW(FileSystemException, Format("Can't get full path name for path %s", path).c_str());
	}
	bfr = new char[cc];
	cc = GetFullPathName(path.c_str(), cc, bfr, &pname);
	if(!cc)
	{
		delete [] bfr;
		OW_THROW(FileSystemException, Format("Can't get full path name for path %s", path).c_str());
	}
	String rstr(bfr);
	delete [] bfr;
	return rstr;
#else
	if (path.startsWith("/"))
	{
		return security(path, 0).second;
	}
	else
	{
		return security(getCurrentWorkingDirectory(), path, 0).second;
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////
String dirname(const String& filename)
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->dirname(filename);
	}
	// skip over trailing slashes
	size_t lastSlash = filename.length() - 1;
	while (lastSlash > 0 
		&& filename[lastSlash] == OW_FILENAME_SEPARATOR_C)
	{
		--lastSlash;
	}
	
	lastSlash = filename.lastIndexOf(OW_FILENAME_SEPARATOR_C, lastSlash);

	if (lastSlash == String::npos)
	{
		return ".";
	}

	while (lastSlash > 0 && filename[lastSlash - 1] == OW_FILENAME_SEPARATOR_C)
	{
		--lastSlash;
	}

	if (lastSlash == 0)
	{
		return OW_FILENAME_SEPARATOR;
	}

	return filename.substring(0, lastSlash);
}

//////////////////////////////////////////////////////////////////////////////
String getCurrentWorkingDirectory()
{
	if (g_fileSystemMockObject)
	{
		return g_fileSystemMockObject->getCurrentWorkingDirectory();
	}
	std::vector<char> buf(MAXPATHLEN);
	char* p;
	do
	{
		p = ::getcwd(&buf[0], buf.size());
		if (p != 0)
		{
			return p;
		}
		buf.resize(buf.size() * 2);
	} while (p == 0 && errno == ERANGE);

	OW_THROW_ERRNO(FileSystemException);
}

} // end namespace Path
} // end namespace FileSystem
} // end namespace OW_NAMESPACE

