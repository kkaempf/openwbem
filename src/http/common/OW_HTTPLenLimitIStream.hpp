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
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 *
 *
 */

#ifndef OW_HTTPLENLIMITSTREAM_HPP_INCLUDE_GUARD_
#define OW_HTTPLENLIMITSTREAM_HPP_INCLUDE_GUARD_

#define HTTPLL_STREAM_BUF_SIZE 256

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_CIMProtocolIStreamIFC.hpp"



class OW_HTTPLengthLimitStreamBuffer : public OW_BaseStreamBuffer
{
public:
	OW_HTTPLengthLimitStreamBuffer(std::istream& istr, OW_Int64 length);
	virtual ~OW_HTTPLengthLimitStreamBuffer();

	/**
	 * sets the Len to a new value,
	 * sets m_pos to zero, and m_isEnd to false
	 */
	void resetLen(OW_Int64 len);

protected:
	virtual int buffer_from_device(char* c, int n);

private:
	std::istream& m_istr;
	
	// holds the content length.
	OW_Int64 m_length;
	// keeps track of how much we've read.
	OW_Int64 m_pos;

	// keeps track if we are at end of length.
	OW_Bool m_isEnd;

	// prohibit copying and assigning
	// NO IMPLEMENTATION
	OW_HTTPLengthLimitStreamBuffer(const OW_HTTPLengthLimitStreamBuffer& arg);
	OW_HTTPLengthLimitStreamBuffer& operator=(
			const OW_HTTPLengthLimitStreamBuffer& arg);

};

//////////////////////////////////////////////////////////////////////////////
class OW_HTTPLenLimitIStreamBase
{
public:
	OW_HTTPLenLimitIStreamBase(std::istream& istr, OW_Int64 length)
		: m_strbuf(istr, length) {}
	OW_HTTPLengthLimitStreamBuffer m_strbuf;
};


//////////////////////////////////////////////////////////////////////////////
class OW_HTTPLenLimitIStream : private OW_HTTPLenLimitIStreamBase,
	public OW_CIMProtocolIStreamIFC
{
public:
	/**
	 * Convert a stream to a LenLimit istream.  This is used for
	 * HTTP entities whose size are determined by a Content-Length
	 * header.  Once len bytes are read, EOF will be set.
	 * @param istr the original istream
	 * @param len the number of bytes to read before setting EOF.
	 */
	OW_HTTPLenLimitIStream(std::istream& istr, OW_Int64 len);

	/**
	 * Get the original istream.
	 * @return the original istream
	 */
	std::istream& getInputStreamOrig() const { return m_istr; }

	/**
	 * Clear the EOF bit, and set the new length to len
	 * @param len the new length to read before (re)setting EOF
	 */
	void resetLen(OW_Int64 len);

private:


	std::istream& m_istr;
	
	// don't allow copying and assigning.
	// NO IMPLEMENTATION
	OW_HTTPLenLimitIStream(const OW_HTTPLenLimitIStream& arg);
	OW_HTTPLenLimitIStream& operator=(const OW_HTTPLenLimitIStream& arg);
};

#endif
