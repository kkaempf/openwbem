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

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Blob.hpp"

#include <cstring>

using std::streambuf;

//////////////////////////////////////////////////////////////////////////////
OW_BlobOStreamBuf::OW_BlobOStreamBuf(int incSize)
	: streambuf()
	, m_incSize(incSize < MIN_INCSIZE ? MIN_INCSIZE : incSize)
	, m_blob((OW_Blob*) ::malloc(m_incSize + sizeof(OW_Blob)))
	, m_size(m_incSize)
	, m_forgetMemory(false)

{
	m_blob->len = 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_BlobOStreamBuf::~OW_BlobOStreamBuf()
{
	if(!m_forgetMemory)
	{
		::free(m_blob);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Blob*
OW_BlobOStreamBuf::getBlob(OW_Bool forgetMemory)
{
	m_forgetMemory = forgetMemory;
	return m_blob;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_BlobOStreamBuf::overflow(int c)
{
	if(m_blob->len == size_t(m_size))
	{
		m_size += m_incSize;
		OW_Blob* blb = (OW_Blob*) malloc(m_size + 
			sizeof(OW_Blob));
		blb->len = m_blob->len;
		::memcpy(blb->bfr, m_blob->bfr, m_blob->len);
		::free(m_blob);
		m_blob = blb;
	}

	m_blob->bfr[m_blob->len++] = (unsigned char) c;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_BlobOStreamBuf::clear()
{
	if(m_size != m_incSize)
	{
		::free(m_blob);
		m_blob = (OW_Blob*) ::malloc(m_incSize + sizeof(OW_Blob));
		m_size = m_incSize;
	}
	m_blob->len = 0;
}


