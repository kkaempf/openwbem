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

#ifndef OW_UNNAMEDPIPE_HPP_
#define OW_UNNAMEDPIPE_HPP_

#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_Exception.hpp"
#include "OW_Bool.hpp"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_Reference.hpp"
#include <iostream>
class OW_String;


DEFINE_EXCEPTION(UnnamedPipe);

class OW_UnnamedPipe;
typedef OW_Reference<OW_UnnamedPipe> OW_UnnamedPipeRef;

// TODO: Make this class derive from OW_IOIFC
class OW_UnnamedPipe : public OW_SelectableIFC
{
public:
	virtual ~OW_UnnamedPipe() {}

	int write(int value);
	int write(const OW_String& strData);
	int read(int* value);
	int read(OW_String& strData);
	OW_String readAll();

//     std::istream& getInputStream() { return m_in; }
//     std::ostream& getOutputStream() { return m_out; }
//     std::iostream& getIOStream() { return m_inout; }

	virtual int write(const void* data, int dataLen) = 0;
	virtual int read(void* buffer, int bufferLen) = 0;

	virtual void open() = 0;
	virtual int close() = 0;
	virtual void setOutputBlocking(OW_Bool outputIsBlocking=true) = 0;

	static OW_UnnamedPipeRef createUnnamedPipe(OW_Bool doOpen=true);

protected:
	OW_UnnamedPipe()
		: OW_SelectableIFC()
//         , m_strbuf(this)
//         , m_in(&m_strbuf)
//         , m_out(&m_strbuf)
//         , m_inout(&m_strbuf)
	{}


private:
//     class UnnamedPipeBuffer : public OW_BaseStreamBuffer
//     {
//     public:
//         UnnamedPipeBuffer(OW_UnnamedPipe* upipe) : m_upipe(upipe) {}
//
//     protected:
//         /**
//          * Writes the buffer to the "device"
//          * @param c A pointer to the start of the buffer
//          * @param n the number of bytes to write
//          *
//          * @return -1 if error, number of bytes written otherwise.
//          */
//         virtual int buffer_to_device(const char *c, int n)
//         {
//             if (m_upipe->write(c, n) != n)
//                 return -1;
//             else
//                 return 0;
//         }
//         /**
//          * Fill the buffer from the "device"
//          * @param c A pointer to the beginning of the buffer
//          * @param n The number of bytes to be read into the buffer.
//          *
//          * @return -1 if no bytes are able to be read from the "device"
//          *             (for instance, end of input stream).  Otherwise,
//          *             return the number of bytes read into the buffer.
//          */
//         virtual int buffer_from_device(char *c, int n)
//         {
//             return m_upipe->read(c, n);
//         }
//
//     private: OW_UnnamedPipe* m_upipe;
//     };
//
//     UnnamedPipeBuffer m_strbuf;
//     std::istream m_in;
//     std::ostream m_out;
//     std::iostream m_inout;

};

#endif	// OW_UNNAMEDPIPE_HPP_



