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
#include "OW_Types.h"
#include "OW_RepositoryStreams.hpp"

#include <cstring>

using std::streambuf;

//////////////////////////////////////////////////////////////////////////////
OW_RepositoryOStreamBuf::OW_RepositoryOStreamBuf(int incSize) : 
	streambuf(), 
	m_incSize(incSize < MIN_INCSIZE ? MIN_INCSIZE : incSize),
	m_bfr(new unsigned char[m_incSize]), m_size(m_incSize), m_count(0)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_RepositoryOStreamBuf::~OW_RepositoryOStreamBuf()
{
	delete [] m_bfr;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_RepositoryOStreamBuf::overflow(int c)
{
	if(m_count == m_size)
	{
		m_size += m_incSize;
		unsigned char* bfr = new unsigned char[m_size];
		::memcpy(bfr, m_bfr, m_count);
		delete [] m_bfr;
		m_bfr = bfr;
	}

	m_bfr[m_count++] = c;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_RepositoryOStreamBuf::clear()
{
	if(m_size != m_incSize)
	{
		delete [] m_bfr;
		m_bfr = new unsigned char[m_incSize];
		m_size = m_incSize;
	}
	m_count = 0;
}

