/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc., IBM Corp. All rights reserved.
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
*
* Author:        Markus Mueller <sedgewick_de@yahoo.de>
*
*******************************************************************************/
#ifndef OW_CMPI_PROFIDER_IFC_UTILS_HPP_
#define OW_CMPI_PROFIDER_IFC_UTILS_HPP_
#include "OW_config.h"

namespace OpenWBEM
{

//#include "CMPIExternal.hpp"
#if 0
class CMPIVectorFreer
{
public:
	CMPIVectorFreer(::Vector v) : m_vector(v)
	{
	}
	~CMPIVectorFreer()
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
class CMPIHandleFreer
{
public:
	CMPIHandleFreer(::NPIHandle& h) : m_handle(h)
	{
	}
	~CMPIHandleFreer()
	{
		if (m_handle.providerError != NULL)
		{
			free((void*)m_handle.providerError);
		}
	}
private:
	::NPIHandle& m_handle;
};
#endif

} // end namespace OpenWBEM

#endif
