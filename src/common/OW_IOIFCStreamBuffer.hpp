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
#ifndef __OW_IOIFCSTREAMBUFFER_HPP__
#define __OW_IOIFCSTREAMBUFFER_HPP__

#include "OW_config.h"
#include "OW_IOIFC.hpp"
#include "OW_BaseStreamBuffer.hpp"

class OW_IOIFCStreamBuffer : public OW_BaseStreamBuffer
{
public:
	OW_IOIFCStreamBuffer(OW_IOIFC* dev, int bufSize = BASE_BUF_SIZE,
		const OW_String& direction = "io");

	OW_IOIFCStreamBuffer(const OW_IOIFCStreamBuffer& arg)
		: OW_BaseStreamBuffer(), m_dev(arg.m_dev) {}

	virtual ~OW_IOIFCStreamBuffer();

	virtual void reset() { initBuffers(); }

	OW_IOIFCStreamBuffer& operator= (const OW_IOIFCStreamBuffer& arg)
	{
		m_dev = arg.m_dev;
		return *this;
	}

	OW_IOIFC* getDevice() const { return m_dev; }

protected:
	virtual int buffer_to_device(const char* c, int n);
	virtual int buffer_from_device(char* c, int n);

	OW_IOIFC* m_dev;
};





#endif	// __OW_IOIFCSTREAMBUFFER_HPP__



