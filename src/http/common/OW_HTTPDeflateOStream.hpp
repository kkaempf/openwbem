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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_HTTPDEFLATEOSTREAM_HPP_INCLUDE_GUARD_
#define OW_HTTPDEFLATEOSTREAM_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#ifdef OW_HAVE_ZLIB_H
#include "OW_BaseStreamBuffer.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Types.hpp"

#ifdef OW_HAVE_OSTREAM
 #include <ostream>
#elif defined(OW_HAVE_OSTREAM_H)
 #include <ostream.h>
#else
 #include <iostream>
#endif

#include <zlib.h>

namespace OpenWBEM
{

class OW_HTTP_API HTTPDeflateOStreamBuffer : public BaseStreamBuffer
{
public:
	HTTPDeflateOStreamBuffer(std::ostream& ostr);
	virtual ~HTTPDeflateOStreamBuffer();
	void termOutput(); 
protected:
	virtual int sync();
	virtual int buffer_to_device(const char *, int) ;
private:
	std::ostream& m_ostr;
	z_stream m_zstr;
	enum { m_outBufSize = HTTP_BUF_SIZE };
	Bytef m_outBuf[m_outBufSize];
	int flushOutBuf(int flush = 0);
	int writeToStream();
	// disallow copying and assigning
	HTTPDeflateOStreamBuffer(const HTTPDeflateOStreamBuffer&);
	HTTPDeflateOStreamBuffer& operator=(const HTTPDeflateOStreamBuffer&);
};
//////////////////////////////////////////////////////////////////////////////
class OW_HTTP_API HTTPDeflateOStreamBase
{
public:
	HTTPDeflateOStreamBase(std::ostream& ostr)
		: m_strbuf(ostr) {}
	HTTPDeflateOStreamBuffer m_strbuf;
};
//////////////////////////////////////////////////////////////////////////////
class OW_HTTP_API HTTPDeflateOStream : private HTTPDeflateOStreamBase, public std::ostream
{
public:
	/**
	 * Converts an ostream to a HTTPDeflateOStream.  The original 
	 * ostream is wrapped.  Anything written to the new ostream, is 
	 * first deflated, and then passed to the original ostream.
	 * @param ostr the original ostream
	 */
	HTTPDeflateOStream(std::ostream& ostr);
	/**
	 * Get the original ostream
	 * @return the original ostream.
	 */
	std::ostream& getOutputStreamOrig() { return m_ostr; };
	/**
	 * Call this when the entire entity has been written to the
	 * ostream.  This preforms some things necessary to terminate
	 * the compressed output stream.
	 */
	void termOutput() { m_strbuf.termOutput(); }
private:
	std::ostream& m_ostr;
	// disallow copying and assigning
	HTTPDeflateOStream(const HTTPDeflateOStream&);
	HTTPDeflateOStream& operator=(const HTTPDeflateOStream&);
};

} // end namespace OpenWBEM

#endif

#endif // #ifdef OW_HAVE_ZLIB_H
