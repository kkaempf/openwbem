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

#ifndef OW_NPI_PROFIDER_IFC_UTILS_HPP_
#define OW_NPI_PROFIDER_IFC_UTILS_HPP_

#include "OW_config.h"
#include "OW_Blob.hpp"
#include "NPIExternal.hpp"

class OW_BlobFreer
{
public:
	OW_BlobFreer(OW_Blob* b) : m_blob(b)
	{
	}
	~OW_BlobFreer()
	{
		free(static_cast<void*>(m_blob));
	}
private:
	OW_Blob* m_blob;
};


class OW_NPIVectorFreer
{
public:
	OW_NPIVectorFreer(::Vector v) : m_vector(v)
	{
	}
	~OW_NPIVectorFreer()
	{
		int n = ::VectorSize(0,m_vector);
		for (int i=0; i < n; i++)
		{
			void * p = ::_VectorGet(0, m_vector, i);
			free (p);
		}
	}
private:
	::Vector m_vector;
};


#endif
