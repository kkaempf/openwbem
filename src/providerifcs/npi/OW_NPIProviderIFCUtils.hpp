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
#ifndef OW_NPI_PROFIDER_IFC_UTILS_HPP_
#define OW_NPI_PROFIDER_IFC_UTILS_HPP_
#include "OW_config.h"
#include "OW_CIMInstance.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "NPIExternal.hpp"

namespace OpenWBEM
{

class NPIVectorFreer
{
public:
	NPIVectorFreer(::Vector v) : m_vector(v)
	{
	}
	~NPIVectorFreer()
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
class NPIHandleFreer
{
public:
	NPIHandleFreer(::NPIHandle& h) : m_handle(h)
	{
//printf("Garbage array created\n");
	}
	~NPIHandleFreer()
	{
	   // delete perlcontext garbage first
	   UInt32 sz = ((NPIContext *)(m_handle.context))->garbage.size();
//printf("Garbage array size is %x\n", sz);
	   for (int i= sz-1; i>=0; i--)
	   {
			  NPIGarbageType pgt =
			 ((NPIContext *)(m_handle.context))->garbageType[i];
//printf("Delete Entry %x, Type %x\n", i, pgt);
		  void * obj_ptr = ((NPIContext *)(m_handle.context))->garbage[i];
		  switch(pgt)
		  {
			 case VECTOR:
				delete(static_cast<charVect *>(obj_ptr));
				break;
			 case CIM_VALUE:
				delete(static_cast<CIMValue *>(obj_ptr));
				break;
			 case CIM_QUALIFIER:
			  //delete(static_cast<CIMQualifier *>(
			  //       ((NPIContext *)(m_handle.context))->garbage[i]) );
				break;
			 case CIM_PARAMVALUE:
				delete(static_cast<CIMParamValue *>(obj_ptr));
				break;
			 case CIM_PROPERTY:
				delete(static_cast<CIMProperty *>(obj_ptr));
				break;
			 case CIM_INSTANCE:
				delete(static_cast<CIMInstance *>(obj_ptr));
				break;
			 case CIM_OBJECTPATH:
				delete(static_cast<CIMObjectPath *>(obj_ptr));
				break;
			 case CIM_CLASS:
				delete(static_cast<CIMClass *>(obj_ptr));
				break;
			 default:
				break;
		  }
	   }
	   ((NPIContext *)(m_handle.context))->garbage.clear();
	   ((NPIContext *)(m_handle.context))->garbageType.clear();
		if (m_handle.providerError != NULL)
		{
			free(const_cast<void*>(static_cast<const void*>(m_handle.providerError)));
		}
	}
private:
	::NPIHandle& m_handle;
};

} // end namespace OpenWBEM

#endif
