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
#include "OW_Assertion.hpp"

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
#ifdef OW_WIN32
	HANDLE fh = ::CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
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
isLink(const String& path)
{
#ifdef OW_WIN32
	return false;
/* This stuff does not compile (_S_IFLNK?)
	struct _stat st;
	if (_stat(path.c_str(), &st) !=0)
	{
		return false;
	}
	return ((st.st_mode & _S_IFLNK) != 0);
*/
#else
	struct stat st;
	if (lstat(path.c_str(), &st) != 0)
	{
		return false;
	}
	return S_ISLNK(st.st_mode);
#endif
}
//////////////////////////////////////////////////////////////////////////////
bool
isDirectory(const String& path)
{
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
	if ((hFile = _findfirst( "*", &dentry)) == -1L)
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
	return ::rename(oldFileName.c_str(), newFileName.c_str()) == 0;
}
//////////////////////////////////////////////////////////////////////////////
size_t
read(const FileHandle& hdl, void* bfr, size_t numberOfBytes,
	off_t offset)
{
#ifdef OW_WIN32
	OVERLAPPED ov = { 0, 0, 0, 0, NULL };
	OVERLAPPED *pov = NULL;
	if(offset != -1L)
	{
		ov.Offset = (DWORD) offset;
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
		::lseek(hdl, offset, SEEK_SET);
	}
	return ::read(hdl, bfr, numberOfBytes);
#endif
}
//////////////////////////////////////////////////////////////////////////////
size_t
write(FileHandle& hdl, const void* bfr, size_t numberOfBytes,
	off_t offset)
{
#ifdef OW_WIN32
	OVERLAPPED ov = { 0, 0, 0, 0, NULL };
	OVERLAPPED *pov = NULL;
	if(offset != -1L)
	{
		ov.Offset = (DWORD) offset;
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
		::lseek(hdl, offset, SEEK_SET);
	}
	return ::write(hdl, bfr, numberOfBytes);
#endif
}
//////////////////////////////////////////////////////////////////////////////
off_t
seek(const FileHandle& hdl, off_t offset, int whence)
{
#ifdef OW_WIN32
	DWORD moveMethod;
	switch(whence)
	{
		case SEEK_END: moveMethod = FILE_END; break;
		case SEEK_CUR: moveMethod = FILE_CURRENT; break;
		default: moveMethod = FILE_BEGIN; break;
	}
	return (off_t) ::SetFilePointer(hdl, (LONG)offset, NULL, moveMethod);
#else
	return ::lseek(hdl, offset, whence);
#endif
}
//////////////////////////////////////////////////////////////////////////////
off_t
tell(const FileHandle& hdl)
{
#ifdef OW_WIN32
	return (off_t) ::SetFilePointer(hdl, 0L, NULL, FILE_CURRENT);
#else
	return ::lseek(hdl, 0, SEEK_CUR);
#endif
}
//////////////////////////////////////////////////////////////////////////////
void
rewind(const FileHandle& hdl)
{
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
#ifdef OW_WIN32
	return (::FlushFileBuffers(hdl)) ? 0 : -1;
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
	char bfr[1024];
	RandomNumber rnum(0, 0xFF);
	for (size_t i = 0; i < 1024; ++i)
	{
		bfr[i] = (char)rnum.getNextNumber();
	}
	HANDLE fh = ::CreateFile(filename.c_str(), GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(fh == INVALID_HANDLE_VALUE)
	{
		OW_THROW(FileSystemException,
			Format("Can't open random file %1 for writing",
			filename).c_str());
	}
	DWORD bytesWritten;
	size_t cc = (size_t)-1;
	bool success = (::WriteFile(fh, bfr, (DWORD)1024, &bytesWritten, NULL) != 0);
	::CloseHandle(fh);
	if(!success || bytesWritten < 1024)
	{
		OW_THROW(FileSystemException,
			Format("Failed writing data to random file %1", filename).c_str());
	}
#else
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
#endif
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

//////////////////////////////////////////////////////////////////////////////
String readSymbolicLink(const String& path)
{
	std::vector<char> buf(MAXPATHLEN);
	int rc;
	do
	{
		rc = ::readlink(path.c_str(), &buf[0], buf.size());
		if (rc >= 0)
		{
			buf.resize(rc);
			buf.push_back('\0');
			return String(&buf[0]);
		}
		buf.resize(buf.size() * 2);
	} while (rc < 0 && errno == ENAMETOOLONG);
	OW_THROW_ERRNO(FileSystemException);
}

//////////////////////////////////////////////////////////////////////////////
namespace Path
{

//////////////////////////////////////////////////////////////////////////////
String realPath(const String& path)
{
#ifdef OW_WIN32
#error "TODO: port realPath"
#else
	String workingPath(path);
	String resolvedPath;
	int numLinks = 0;

	// handle relative paths.
	if (workingPath.length() > 0 && workingPath[0] != '/')
	{
		// result of getCurrentWorkingDirectory is already resolved.
		resolvedPath = getCurrentWorkingDirectory();
	}

	const char* pathCompBegin(workingPath.c_str());
	const char* pathCompEnd(pathCompBegin);
	while (*pathCompBegin != '\0')
	{
		// skip bunches of ////
		while (*pathCompBegin == '/')
		{
			++pathCompBegin;
		}

		// find end of the path component
		pathCompEnd = pathCompBegin;
		while (*pathCompEnd != '\0' && *pathCompEnd != '/')
		{
			++pathCompEnd;
		}

		if (pathCompEnd - pathCompBegin == 0)
		{
			break;
		}
		else if (pathCompEnd - pathCompBegin == 1 && pathCompBegin[0] == '.')
		{
			;// don't add . to the result
		}
		else if (pathCompEnd - pathCompBegin == 2 && pathCompBegin[0] == '.' && pathCompBegin[1] == '.')
		{
			// hit .. so remove the last directory from the result
			size_t lastSlash = resolvedPath.lastIndexOf('/');
			if (lastSlash != String::npos)
			{
				resolvedPath.erase(lastSlash);
			}
		}
		else
		{
			resolvedPath += '/';
			resolvedPath += String(pathCompBegin, pathCompEnd - pathCompBegin);

			// now check the path actually exists
			struct stat pathStats;
			if (::lstat(resolvedPath.c_str(), &pathStats) < 0)
			{
				OW_THROW_ERRNO_MSG(FileSystemException, resolvedPath);
			}

			if (S_ISLNK(pathStats.st_mode))
			{
				++numLinks;
				if (numLinks > MAXSYMLINKS)
				{
					errno = ELOOP;
					OW_THROW_ERRNO_MSG(FileSystemException, resolvedPath);
				}
				String linkTarget(readSymbolicLink(resolvedPath));

				if (linkTarget.length() > 0 && linkTarget[0] != '/')
				{
					// relative link. Remove the link from the resolvedPath and add the linkTarget
					OW_ASSERT(resolvedPath.lastIndexOf('/') != String::npos); // should always happen, we just added a / to the string
					resolvedPath.erase(resolvedPath.lastIndexOf('/'));

					resolvedPath += '/';
					resolvedPath += linkTarget;
				}
				else
				{
					// absolute link
					resolvedPath = linkTarget;
				}

				// now reset and start over on the new path
				resolvedPath += pathCompEnd;
				workingPath = resolvedPath;
				pathCompBegin = pathCompEnd = workingPath.c_str();
				resolvedPath.erase();
			}
		}

		// keep the loop flowing
		pathCompBegin = pathCompEnd;
	}

	if (resolvedPath.empty())
	{
		resolvedPath = "/";
	}

	return resolvedPath;
#endif
}

//////////////////////////////////////////////////////////////////////////////
String dirname(const String& filename)
{
#ifdef OW_WIN32
#error "TODO: port dirname"
#else
	// skip over trailing slashes
	size_t lastSlash = filename.length() - 1;
	while (lastSlash > 0 && filename[lastSlash] == '/')
	{
		--lastSlash;
	}
	
	lastSlash = filename.lastIndexOf('/', lastSlash);

	if (lastSlash == String::npos)
	{
		return ".";
	}

	while (lastSlash > 0 && filename[lastSlash - 1] == '/')
	{
		--lastSlash;
	}

	if (lastSlash == 0)
	{
		return "/";
	}
	return filename.substring(0, lastSlash);
#endif
}

//////////////////////////////////////////////////////////////////////////////
String getCurrentWorkingDirectory()
{
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
} // end namespace OpenWBEM

