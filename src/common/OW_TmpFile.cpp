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
#include "OW_Types.hpp"
#include "OW_TmpFile.hpp"
#include "OW_IOException.hpp"
#include "OW_MutexLock.hpp"
#include "OW_String.hpp"
#include "OW_Format.hpp"
#include "OW_System.hpp"
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cstdio>
extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined(OW_WIN32)
#include <io.h>
#endif
}

namespace OW_NAMESPACE
{

namespace {

#ifndef OW_WIN32
inline int
closeFile(int fd)
{
	return ::close(fd);
}

#else
inline int
closeFile(HANDLE fh)
{
	return CloseHandle(fh) ? 0 : -1;
}
#endif

}	// end of unnamed namespace

//////////////////////////////////////////////////////////////////////////////
TmpFileImpl::TmpFileImpl()
	: m_filename(NULL)
	, m_hdl(OW_INVALID_FILEHANDLE)
{
	open();
}
//////////////////////////////////////////////////////////////////////////////
TmpFileImpl::TmpFileImpl(String const& filename)
	: m_filename(NULL)
	, m_hdl(OW_INVALID_FILEHANDLE)
{
	size_t len = filename.length();
	m_filename = new char[len + 1];
	::strncpy(m_filename, filename.c_str(), len);
	m_filename[len] = '\0';
#if defined(OW_WIN32)
	m_hdl = CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
#else
	m_hdl = ::open(m_filename, O_RDWR);
#endif
	if (m_hdl == OW_INVALID_FILEHANDLE)
	{
		delete[] m_filename;
		m_filename = NULL;
		OW_THROW(IOException, Format("Error opening file %1: %2", filename,
			System::lastErrorMsg()).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
TmpFileImpl::~TmpFileImpl()
{
	close();
}

#ifdef OW_WIN32
//////////////////////////////////////////////////////////////////////////////
int 
TmpFileImpl::seek(long offset, int whence)
{ 
	DWORD moveMethod;
	switch(whence)
	{
		case SEEK_END: moveMethod = FILE_END; break;
		case SEEK_CUR: moveMethod = FILE_CURRENT; break;
		default: moveMethod = FILE_BEGIN; break;
	}
	return (int) SetFilePointer(m_hdl, (LONG)offset, NULL, moveMethod);
}

//////////////////////////////////////////////////////////////////////////////
long 
TmpFileImpl::tell()
{ 
	return (long) SetFilePointer(m_hdl, 0L, NULL, FILE_CURRENT);
}

//////////////////////////////////////////////////////////////////////////////
void 
TmpFileImpl::rewind()
{ 
	SetFilePointer(m_hdl, 0L, NULL, FILE_BEGIN);
}

//////////////////////////////////////////////////////////////////////////////
int 
TmpFileImpl::flush()
{
	if(m_hdl != OW_INVALID_FILEHANDLE)
	{
		FlushFileBuffers(m_hdl);
	}
	return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////
long
TmpFileImpl::getSize()
{
#ifdef OW_WIN32
	return (long) GetFileSize(m_hdl, NULL);
#else
	long cv = tell();
	seek(0L, SEEK_END);
	long rv = tell();
	seek(cv, SEEK_SET);
	return rv;
#endif
}
//////////////////////////////////////////////////////////////////////////////
#if defined(OW_WIN32)
void
TmpFileImpl::open()
{
	close();
	
	char bfr[MAX_PATH];
	if(!GetTempFileName(".", "owtempfile", 0, bfr))
	{
		OW_THROW(IOException,
			Format("Error generating temp file name: %1",
				System::lastErrorMsg()).c_str());
	}

	size_t len = ::strlen(bfr);
	m_filename = new char[len + 1];
	::strncpy(m_filename, bfr, len);
	m_filename[len] = '\0';
	
	static Mutex tmpfileMutex;
	MutexLock tmpfileML(tmpfileMutex);
	m_hdl = CreateFile(m_filename, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hdl == INVALID_HANDLE_VALUE)
	{
		delete[] m_filename;
		m_filename = NULL;
		OW_THROW(IOException, Format("Error opening temp file %1: %2", 
			bfr, System::lastErrorMsg()).c_str());
	}
}
#else
void
TmpFileImpl::open()
{
	close();
#ifdef OW_OPENUNIX
	String sfname("/var/tmp/owtmpfileXXXXXX");
#else
	String sfname("/tmp/owtmpfileXXXXXX");
#endif
	size_t len = sfname.length();
	m_filename = new char[len + 1];
	strncpy(m_filename, sfname.c_str(), len);
	m_filename[len] = '\0';
	static Mutex tmpfileMutex;
	MutexLock tmpfileML(tmpfileMutex);
	m_hdl = mkstemp(m_filename);
	if (m_hdl == -1)
	{
		delete[] m_filename;
		m_filename = NULL;
		OW_THROW(IOException, Format("Error opening file from mkstemp: %1", 
			strerror(errno)).c_str());
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
int
TmpFileImpl::close()
{
	int rv = -1;
	if (m_hdl != OW_INVALID_FILEHANDLE)
	{
		rv = closeFile(m_hdl);
#if defined(OW_WIN32)
		DeleteFile(m_filename);
#else
		remove(m_filename);
#endif
		delete [] m_filename;
		m_filename = NULL;
		m_hdl = OW_INVALID_FILEHANDLE;
	}
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
size_t
TmpFileImpl::read(void* bfr, size_t numberOfBytes, long offset)
{
	if (offset == -1L)
	{
		seek(0L, SEEK_CUR);
	}
	else
	{
		seek(offset, SEEK_SET);
	}
#if defined(OW_WIN32)
	DWORD bytesRead;
	size_t cc = (size_t)-1;
	if(ReadFile(m_hdl, bfr, (DWORD)numberOfBytes, &bytesRead, NULL))
	{
		cc = (size_t)bytesRead;
	}
	return cc;
#else
	return ::read(m_hdl, bfr, numberOfBytes);
#endif
}
//////////////////////////////////////////////////////////////////////////////
size_t
TmpFileImpl::write(const void* bfr, size_t numberOfBytes, long offset)
{
	if (offset == -1L)
	{
		seek(0L, SEEK_CUR);
	}
	else
	{
		seek(offset, SEEK_SET);
	}
#if defined(OW_WIN32)
	DWORD bytesWritten;
	int rv = -1;
	if(WriteFile(m_hdl, bfr, (DWORD)numberOfBytes, &bytesWritten, NULL))
	{
		rv = (int)bytesWritten;
	}
	return rv;
#else
	int rv = ::write(m_hdl, bfr, numberOfBytes);
	if (rv == -1)
	{
		perror("TmpFile::write()");
	}
	return rv;
#endif
}
//////////////////////////////////////////////////////////////////////////////
String
TmpFileImpl::releaseFile()
{
	String rval(m_filename);
	if (m_hdl != OW_INVALID_FILEHANDLE) 
	{
		if (closeFile(m_hdl) == -1)
		{
			OW_THROW(IOException, "Unable to close file");
		}
		// work like close, but don't delete the file, it will be given to the
		// caller
		delete [] m_filename;
		m_filename = NULL;
		m_hdl = OW_INVALID_FILEHANDLE;
	}
	return rval;
}

} // end namespace OW_NAMESPACE

