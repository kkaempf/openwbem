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
#ifndef OW_TMPFILE_HPP_INCLUDE_GUARD_
#define OW_TMPFILE_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Reference.hpp"
#include "OW_String.hpp"
#ifdef OW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef OW_WIN32
#include <io.h>
#endif

namespace OpenWBEM
{

class TmpFileImpl
{
public:
	TmpFileImpl();
	TmpFileImpl(String const& filename);
	~TmpFileImpl();
	size_t read(void* bfr, size_t numberOfBytes, long offset=-1L);
	size_t write(const void* bfr, size_t numberOfBytes, long offset=-1L);
#ifdef OW_WIN32
	int seek(long offset, int whence=SEEK_SET)
		{ return ::_lseek(m_hdl, offset, whence); }
	long tell() { return ::_lseek(m_hdl, 0, SEEK_CUR); }
	void rewind() { ::_lseek(m_hdl, 0, SEEK_SET); }
#else
	int seek(long offset, int whence=SEEK_SET)
		{ return ::lseek(m_hdl, offset, whence); }
	long tell() { return ::lseek(m_hdl, 0, SEEK_CUR); }
	void rewind() { ::lseek(m_hdl, 0, SEEK_SET); }
#endif
	int flush() { return 0; }
	void newFile() { open(); }
	long getSize();
	String releaseFile();
private:
	void open();
	int close();
	TmpFileImpl(const TmpFileImpl& arg);	// Not implemented
	TmpFileImpl& operator= (const TmpFileImpl& arg);	// Not implemented
	char* m_filename;
	int m_hdl;
};
class TmpFile
{
public:
	TmpFile() :
		m_impl(new TmpFileImpl) {  }
	TmpFile(String const& filename)
		: m_impl(new TmpFileImpl(filename))
	{}
	TmpFile(const TmpFile& arg) :
		m_impl(arg.m_impl) {}
	TmpFile& operator= (const TmpFile& arg)
	{
		m_impl = arg.m_impl;
		return *this;
	}
	~TmpFile()  {  }
	size_t read(void* bfr, size_t numberOfBytes, long offset=-1L)
	{
		return m_impl->read(bfr, numberOfBytes, offset);
	}
	size_t write(const void* bfr, size_t numberOfBytes, long offset=-1L)
	{
		return m_impl->write(bfr, numberOfBytes, offset);
	}
	int seek(long offset, int whence=SEEK_SET)
		{ return m_impl->seek(offset, whence); }
	long tell() { return m_impl->tell(); }
	void rewind() { m_impl->rewind(); }
	int flush() { return m_impl->flush(); }
	void newFile() { m_impl->newFile(); }
	long getSize() { return m_impl->getSize(); }
	String releaseFile() { return m_impl->releaseFile(); }
private:
	Reference<TmpFileImpl> m_impl;
};

} // end namespace OpenWBEM

typedef OpenWBEM::TmpFile OW_TmpFile;

#endif
