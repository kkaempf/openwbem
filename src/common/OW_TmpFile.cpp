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
#include "OW_Types.hpp"
#include "OW_TmpFile.hpp"
#include "OW_IOException.hpp"
#include "OW_MutexLock.hpp"
#include "OW_String.hpp"
#include "OW_Format.hpp"

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cstdio>

extern "C"
{
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
}

//////////////////////////////////////////////////////////////////////////////
OW_TmpFileImpl::OW_TmpFileImpl()
	: m_filename(NULL)
	, m_hdl(-1)
{
	open();
}

//////////////////////////////////////////////////////////////////////////////
OW_TmpFileImpl::OW_TmpFileImpl(OW_String const& filename)
	: m_filename(NULL)
	, m_hdl(-1)
{
	size_t len = filename.length();
	m_filename = new char[len + 1];
	strncpy(m_filename, filename.c_str(), len);
	m_filename[len] = '\0';

	m_hdl = ::open(m_filename, O_RDWR);
	if(m_hdl == -1)
	{
		delete[] m_filename;
		m_filename = NULL;
		OW_THROW(OW_IOException, format("Error opening file %1: %2", filename,
			strerror(errno)).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_TmpFileImpl::~OW_TmpFileImpl()
{
	close();
}

//////////////////////////////////////////////////////////////////////////////
long
OW_TmpFileImpl::getSize()
{
	long cv = tell();
	seek(0L, SEEK_END);
	long rv = tell();
	seek(cv, SEEK_SET);
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_TmpFileImpl::open()
{
	close();
#ifdef OW_OPENUNIX
	OW_String sfname("/var/tmp/owtmpfileXXXXXX");
#else
	OW_String sfname("/tmp/owtmpfileXXXXXX");
#endif
	size_t len = sfname.length();
	m_filename = new char[len + 1];
	strncpy(m_filename, sfname.c_str(), len);
	m_filename[len] = '\0';
	static OW_Mutex tmpfileMutex;
	OW_MutexLock tmpfileML(tmpfileMutex);
#ifdef OW_WIN32
	m_hdl = -1;
#else
	m_hdl = mkstemp(m_filename);
#endif
	if(m_hdl == -1)
	{
		delete[] m_filename;
		m_filename = NULL;
		OW_THROW(OW_IOException, format("Error opening file from mkstemp: %1", 
			strerror(errno)).c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_TmpFileImpl::close()
{
	int rv = -1;
	if(m_hdl != -1)
	{
		rv = ::close(m_hdl);
		remove(m_filename);
		delete [] m_filename;
		m_filename = NULL;
		m_hdl = -1;
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
size_t
OW_TmpFileImpl::read(void* bfr, size_t numberOfBytes, long offset)
{
	if(offset == -1L)
	{
		::lseek(m_hdl, 0L, SEEK_CUR);
	}
	else
	{
		::lseek(m_hdl, offset, SEEK_SET);
	}

	return ::read(m_hdl, bfr, numberOfBytes);
}

//////////////////////////////////////////////////////////////////////////////
size_t
OW_TmpFileImpl::write(const void* bfr, size_t numberOfBytes, long offset)
{
	if(offset == -1L)
	{
		::lseek(m_hdl, 0L, SEEK_CUR);
	}
	else
	{
		::lseek(m_hdl, offset, SEEK_SET);
	}

#ifdef OW_USE_GNU_PTH
    int rv = pth_write(m_hdl, bfr, numberOfBytes);
#else
	int rv = ::write(m_hdl, bfr, numberOfBytes);
#endif
	if (rv == -1)
	{
		perror("OW_TmpFile::write()");
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_TmpFileImpl::releaseFile()
{
	OW_String rval(m_filename);
	if(m_hdl != -1)
	{
		if( ::close(m_hdl) == -1)
			OW_THROW(OW_IOException, "Unable to close file");

		// work like close, but don't delete the file, it will be give to the
		// caller
		delete [] m_filename;
		m_filename = NULL;
		m_hdl = -1;
	}
	return rval;
}
