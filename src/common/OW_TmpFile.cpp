/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
}

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
TmpFileImpl::TmpFileImpl()
	: m_filename(NULL)
	, m_hdl(-1)
{
	open();
}
//////////////////////////////////////////////////////////////////////////////
TmpFileImpl::TmpFileImpl(String const& filename)
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
		OW_THROW(IOException, format("Error opening file %1: %2", filename,
			strerror(errno)).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
TmpFileImpl::~TmpFileImpl()
{
	close();
}
//////////////////////////////////////////////////////////////////////////////
long
TmpFileImpl::getSize()
{
	long cv = tell();
	seek(0L, SEEK_END);
	long rv = tell();
	seek(cv, SEEK_SET);
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
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
#ifdef OW_WIN32
	m_hdl = -1;
#else
	m_hdl = mkstemp(m_filename);
#endif
	if(m_hdl == -1)
	{
		delete[] m_filename;
		m_filename = NULL;
		OW_THROW(IOException, format("Error opening file from mkstemp: %1", 
			strerror(errno)).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
int
TmpFileImpl::close()
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
TmpFileImpl::read(void* bfr, size_t numberOfBytes, long offset)
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
TmpFileImpl::write(const void* bfr, size_t numberOfBytes, long offset)
{
	if(offset == -1L)
	{
		::lseek(m_hdl, 0L, SEEK_CUR);
	}
	else
	{
		::lseek(m_hdl, offset, SEEK_SET);
	}
	int rv = ::write(m_hdl, bfr, numberOfBytes);
	if (rv == -1)
	{
		perror("TmpFile::write()");
	}
	return rv;
}
//////////////////////////////////////////////////////////////////////////////
String
TmpFileImpl::releaseFile()
{
	String rval(m_filename);
	if(m_hdl != -1)
	{
		if( ::close(m_hdl) == -1)
			OW_THROW(IOException, "Unable to close file");
		// work like close, but don't delete the file, it will be give to the
		// caller
		delete [] m_filename;
		m_filename = NULL;
		m_hdl = -1;
	}
	return rval;
}

} // end namespace OpenWBEM

