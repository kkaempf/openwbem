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

#ifndef __OW_TMPFILE_HPP__
#define __OW_TMPFILE_HPP__

#include "OW_config.h"
#include "OW_Reference.hpp"
#include "OW_String.hpp"

#include <unistd.h>

class OW_TmpFileImpl
{
public:
	OW_TmpFileImpl();
	OW_TmpFileImpl(OW_String const& filename);
	~OW_TmpFileImpl();

	size_t read(void* bfr, size_t numberOfBytes, long offset=-1L);
	size_t write(const void* bfr, size_t numberOfBytes, long offset=-1L);

	int seek(long offset, int whence=SEEK_SET)
		{ return ::lseek(m_hdl, offset, whence); }

	long tell() { return ::lseek(m_hdl, 0, SEEK_CUR); }
	void rewind() { ::lseek(m_hdl, 0, SEEK_SET); }
	int flush() { return 0; }
	void newFile() { open(); }
	long getSize();

	OW_String releaseFile();

private:
	void open();
	int close();

	OW_TmpFileImpl(const OW_TmpFileImpl& arg);	// Not implemented
	OW_TmpFileImpl& operator= (const OW_TmpFileImpl& arg);	// Not implemented

	char* m_filename;
	int m_hdl;
};


class OW_TmpFile
{
public:
	OW_TmpFile() :
		m_impl(new OW_TmpFileImpl) {  }

	OW_TmpFile(OW_String const& filename)
		: m_impl(new OW_TmpFileImpl(filename))
	{}

	OW_TmpFile(const OW_TmpFile& arg) :
		m_impl(arg.m_impl) {}

	OW_TmpFile& operator= (const OW_TmpFile& arg)
	{
		m_impl = arg.m_impl;
		return *this;
	}

	~OW_TmpFile()  {  }

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

	OW_String releaseFile() { return m_impl->releaseFile(); }
private:

	OW_Reference<OW_TmpFileImpl> m_impl;
};

#endif	// __OW_TMPFILE_HPP__
