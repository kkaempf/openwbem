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
#include "OW_Array.hpp"
#include "OW_String.hpp"
#include "OW_CIM.hpp"
#include "owcpiprov.h"
#include "OW_CPI.hpp"

//////////////////////////////////////////////////////////////////////////////
static CPI_VectorHandle*
vectAppendCheck(cpiVectHdl vect, unsigned int type, int& resCode)
{
	resCode = CPI_SUCCESS;
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		resCode = CPI_E_INVALID_VECTHANDLE;
		return 0;
	}

	if(!phdl->m_obj)
	{
		switch(type)
		{
			case CPI_BOOL_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiBool>; break;
			case CPI_UINT8_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiUint8>; break;
			case CPI_SINT8_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiSint8>; break;
			case CPI_UINT16_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiUint16>; break;
			case CPI_SINT16_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiSint16>; break;
			case CPI_UINT32_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiUint32>; break;
			case CPI_SINT32_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiSint32>; break;
			case CPI_UINT64_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiUint64>; break;
			case CPI_SINT64_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiSint64>; break;
			case CPI_REAL32_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiReal32>; break;
			case CPI_REAL64_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiReal64>; break;
			case CPI_CHAR16_SIG:
				phdl->m_obj = (void*) new OW_Array<cpiChar16>; break;
			case CPI_STRING_SIG:
				phdl->m_obj = (void*) new OW_Array<OW_String>; break;
			default:
				if(OW_CPI::validObjectSig(type))
				{
					phdl->m_obj = (void*) new CPI_ObjVector;
				}
				else
				{
					resCode = CPI_E_INVALID_DATATYPE;
					return 0;
				}
				break;
		}
		phdl->m_type = type;
	}

	if(phdl->m_type != type)
	{
		resCode = CPI_E_INCOMPAT_OBJHANDLE;
		return 0;
	}

	return phdl;
}

//////////////////////////////////////////////////////////////////////////////
static CPI_VectorHandle*
vectNdxCheck(cpiVectHdl vect, unsigned int type, int ndx, int& resCode)
{
	if(ndx < 0)
	{
		resCode = CPI_E_BAD_INDEX;
		return 0;
	}

	resCode = CPI_SUCCESS;
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		resCode = CPI_E_INVALID_VECTHANDLE;
		return 0;
	}

	if(!phdl->m_obj)
	{
		resCode = CPI_E_NULL_VECTOR;
		return 0;
	}

	if(phdl->m_type != type)
	{
		resCode = CPI_E_INCOMPAT_OBJHANDLE;
		return 0;
	}

	size_t size = 0;
	switch(type)
	{
		case CPI_BOOL_SIG:
			size = ((OW_Array<cpiBool>*)phdl->m_obj)->size(); break;
		case CPI_UINT8_SIG:
			size = ((OW_Array<cpiUint8>*)phdl->m_obj)->size(); break;
		case CPI_SINT8_SIG:
			size = ((OW_Array<cpiSint8>*)phdl->m_obj)->size(); break;
		case CPI_UINT16_SIG:
			size = ((OW_Array<cpiUint16>*)phdl->m_obj)->size(); break;
		case CPI_SINT16_SIG:
			size = ((OW_Array<cpiSint16>*)phdl->m_obj)->size(); break;
		case CPI_UINT32_SIG:
			size = ((OW_Array<cpiUint32>*)phdl->m_obj)->size(); break;
		case CPI_SINT32_SIG:
			size = ((OW_Array<cpiSint32>*)phdl->m_obj)->size(); break;
		case CPI_UINT64_SIG:
			size = ((OW_Array<cpiUint64>*)phdl->m_obj)->size(); break;
		case CPI_SINT64_SIG:
			size = ((OW_Array<cpiSint64>*)phdl->m_obj)->size(); break;
		case CPI_REAL32_SIG:
			size = ((OW_Array<cpiReal32>*)phdl->m_obj)->size(); break;
		case CPI_REAL64_SIG:
			size = ((OW_Array<cpiReal64>*)phdl->m_obj)->size(); break;
		case CPI_CHAR16_SIG:
			size = ((OW_Array<cpiChar16>*)phdl->m_obj)->size(); break;
		case CPI_STRING_SIG:
			size = ((OW_Array<OW_String>*)phdl->m_obj)->size(); break;
		default:
			if(OW_CPI::validObjectSig(type))
			{
				size = ((CPI_ObjVector*)phdl->m_obj)->size();
			}
			else
			{
				resCode = CPI_E_INVALID_DATATYPE;
				return 0;
			}
	}

	if(int(size) <= ndx)
	{
		resCode = CPI_E_OUTOFBOUNDS;
		return 0;
	}

	return phdl;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int
cpiVectorAlloc(cpiVectHdl* pVect)
{
	CPI_VectorHandle* phdl = new CPI_VectorHandle(0);
	*pVect = (void*) phdl;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
cpiVectorAlloc(cpiVectHdl* pVect, unsigned int type)
{
	CPI_VectorHandle* phdl = new CPI_VectorHandle(0);
	phdl->m_type = type;
	*pVect = (void*) phdl;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
unsigned int
cpiVectorType(cpiVectHdl vect)
{
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return (unsigned int)CPI_E_INVALID_VECTHANDLE;
	}

	return phdl->m_type;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int
cpiVectorFree(cpiVectHdl vect)
{
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return CPI_E_INVALID_VECTHANDLE;
	}
	
	switch(phdl->m_type)
	{
		case CPI_BOOL_SIG:
			delete (OW_Array<cpiBool>*) phdl->m_obj; break;
		case CPI_UINT8_SIG:
			delete (OW_Array<cpiUint8>*) phdl->m_obj; break;
		case CPI_SINT8_SIG:
			delete (OW_Array<cpiSint8>*) phdl->m_obj; break;
		case CPI_UINT16_SIG:
			delete (OW_Array<cpiUint16>*) phdl->m_obj; break;
		case CPI_SINT16_SIG:
			delete (OW_Array<cpiSint16>*) phdl->m_obj; break;
		case CPI_UINT32_SIG:
			delete (OW_Array<cpiUint32>*) phdl->m_obj; break;
		case CPI_SINT32_SIG:
			delete (OW_Array<cpiSint32>*) phdl->m_obj; break;
		case CPI_UINT64_SIG:
			delete (OW_Array<cpiUint64>*) phdl->m_obj; break;
		case CPI_SINT64_SIG:
			delete (OW_Array<cpiSint64>*) phdl->m_obj; break;
		case CPI_REAL32_SIG:
			delete (OW_Array<cpiReal32>*) phdl->m_obj; break;
		case CPI_REAL64_SIG:
			delete (OW_Array<cpiReal64>*) phdl->m_obj; break;
		case CPI_CHAR16_SIG:
			delete (OW_Array<cpiChar16>*) phdl->m_obj; break;
		case CPI_STRING_SIG:
			delete (OW_Array<OW_String>*) phdl->m_obj; break;

		case CPI_CLASS_SIG:
		case CPI_INSTANCE_SIG:
		case CPI_METHOD_SIG:
		case CPI_NS_SIG:
		case CPI_OBJPATH_SIG:
		case CPI_PARM_SIG:
		case CPI_PROP_SIG:
		case CPI_QUAL_SIG:
		case CPI_QUALTYPE_SIG:
		case CPI_VALUE_SIG:
		case CPI_DATETIME_SIG:
			{
				CPI_ObjVector* vp = (CPI_ObjVector*) phdl->m_obj;
				for(size_t i = 0; i < vp->size(); i++)
				{
					(*vp)[i]->deleteObj();
					delete (*vp)[i];
				}
				delete vp;
			}
			break;;

		default:
			return CPI_E_BAD_VECTOR_CONTENTS;
	}

	phdl->m_sig = 0;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int
cpiVectorAppend(cpiVectHdl vect, cpiObjHdl obj)
{
	CPI_ObjHandleBase* pObj = OW_CPI::castObjPtr(obj);
	if(!pObj)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return CPI_E_INVALID_VECTHANDLE;
	}

	if(!phdl->m_obj)
	{
		phdl->m_obj = (void*) new CPI_ObjVector;
		phdl->m_type = pObj->m_sig;
	}

	if(phdl->m_type != pObj->m_sig)
	{
		return CPI_E_INCOMPAT_OBJHANDLE;
	}

	((CPI_ObjVector*)phdl->m_obj)->append(pObj);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendUint8(cpiVectHdl vect, cpiUint8 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_UINT8_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint8>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendSint8(cpiVectHdl vect, cpiSint8 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_SINT8_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint8>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendUint16(cpiVectHdl vect, cpiUint16 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_UINT16_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint16>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendSint16(cpiVectHdl vect, cpiSint16 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_SINT16_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint16>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendUint32(cpiVectHdl vect, cpiUint32 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_UINT32_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint32>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendSint32(cpiVectHdl vect, cpiSint32 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_SINT32_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint32>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendUint64(cpiVectHdl vect, cpiUint64 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_UINT64_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint64>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendSint64(cpiVectHdl vect, cpiSint64 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_SINT64_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint64>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendReal32(cpiVectHdl vect, cpiReal32 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_REAL32_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiReal32>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendReal64(cpiVectHdl vect, cpiReal64 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_REAL64_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiReal64>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendString(cpiVectHdl vect, const char* v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_STRING_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<OW_String>*)phdl->m_obj)->append(OW_String(v));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendBool(cpiVectHdl vect, cpiBool v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_BOOL_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiBool>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorAppendChar16(cpiVectHdl vect, cpiChar16 v)
{
	int resCode;
	CPI_VectorHandle* phdl = vectAppendCheck(vect, CPI_CHAR16_SIG, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiChar16>*)phdl->m_obj)->append(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetUint8(cpiVectHdl vect, cpiUint8 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT8_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint8>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetSint8(cpiVectHdl vect, cpiSint8 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT8_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint8>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetUint16(cpiVectHdl vect, cpiUint16 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT16_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint16>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetSint16(cpiVectHdl vect, cpiSint16 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT16_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint16>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetUint32(cpiVectHdl vect, cpiUint32 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT32_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint32>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetSint32(cpiVectHdl vect, cpiSint32 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT32_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint32>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetUint64(cpiVectHdl vect, cpiUint64 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT64_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiUint64>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetSint64(cpiVectHdl vect, cpiSint64 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT64_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiSint64>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetReal32(cpiVectHdl vect, cpiReal32 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_REAL32_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiReal32>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetReal64(cpiVectHdl vect, cpiReal64 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_REAL64_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiReal64>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetString(cpiVectHdl vect, const char* v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_STRING_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<OW_String>*)phdl->m_obj)->operator [](ndx) = OW_String(v);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetBool(cpiVectHdl vect, cpiBool v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_BOOL_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiBool>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSetChar16(cpiVectHdl vect, cpiChar16 v, int ndx)
{
	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_CHAR16_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	((OW_Array<cpiChar16>*)phdl->m_obj)->operator [](ndx) = v;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetUint8(cpiVectHdl vect, cpiUint8* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT8_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiUint8>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetSint8(cpiVectHdl vect, cpiSint8* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT8_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiSint8>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetUint16(cpiVectHdl vect, cpiUint16* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT16_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiUint16>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetSint16(cpiVectHdl vect, cpiSint16* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT16_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiSint16>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetUint32(cpiVectHdl vect, cpiUint32* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT32_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiUint32>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetSint32(cpiVectHdl vect, cpiSint32* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT32_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiSint32>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetUint64(cpiVectHdl vect, cpiUint64* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_UINT64_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiUint64>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetSint64(cpiVectHdl vect, cpiSint64* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_SINT64_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiSint64>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetReal32(cpiVectHdl vect, cpiReal32* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_REAL32_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiReal32>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetReal64(cpiVectHdl vect, cpiReal64* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_REAL64_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiReal64>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char*
cpiVectorGetString(cpiVectHdl vect, int ndx, int* errCode)
{
	if(errCode)
		*errCode = CPI_SUCCESS;

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_STRING_SIG, ndx, resCode);
	if(!phdl)
	{
		if(errCode)
			*errCode = resCode;
		return 0;
	}
	OW_String v = ((OW_Array<OW_String>*)phdl->m_obj)->operator [](ndx);
	return v.c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetBool(cpiVectHdl vect, cpiBool* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_BOOL_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiBool>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGetChar16(cpiVectHdl vect, cpiChar16* v, int ndx)
{
	if(!v)
	{
		return CPI_E_INVALID_PARM;
	}

	int resCode;
	CPI_VectorHandle* phdl = vectNdxCheck(vect, CPI_CHAR16_SIG, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}
	*v = ((OW_Array<cpiChar16>*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorRemove(cpiVectHdl vect, size_t ndx)
{
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return CPI_E_INVALID_VECTHANDLE;
	}

	if(!phdl->m_obj)
	{
		return CPI_SUCCESS;
	}

	int resCode;
	phdl = vectNdxCheck(vect, phdl->m_type, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}

	((CPI_ObjVector*)phdl->m_obj)->remove(ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorGet(cpiVectHdl vect, cpiObjHdl* pObj, size_t ndx)
{
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return CPI_E_INVALID_VECTHANDLE;
	}

	if(!phdl->m_obj)
	{
		return CPI_E_OUTOFBOUNDS;
	}

	int resCode;
	phdl = vectNdxCheck(vect, phdl->m_type, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}

	*pObj = (cpiObjHdl) ((CPI_ObjVector*)phdl->m_obj)->operator [](ndx);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectorSet(cpiVectHdl vect, cpiObjHdl obj, size_t ndx)
{
	CPI_ObjHandleBase* pObj = OW_CPI::castObjPtr(obj);
	if(!pObj)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return CPI_E_INVALID_VECTHANDLE;
	}

	if(!phdl->m_obj)
	{
		return CPI_E_OUTOFBOUNDS;
	}

	int resCode;
	phdl = vectNdxCheck(vect, phdl->m_type, ndx, resCode);
	if(!phdl)
	{
		return resCode;
	}

	((CPI_ObjVector*)phdl->m_obj)->operator [](ndx) = pObj;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectSize(cpiVectHdl vect)
{
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return CPI_E_INVALID_VECTHANDLE;
	}

	if(!phdl->m_obj)
	{
		return 0;
	}

	size_t size = 0;
	switch(phdl->m_type)
	{
		case CPI_BOOL_SIG:
			size = ((OW_Array<cpiBool>*)phdl->m_obj)->size(); break;
		case CPI_UINT8_SIG:
			size = ((OW_Array<cpiUint8>*)phdl->m_obj)->size(); break;
		case CPI_SINT8_SIG:
			size = ((OW_Array<cpiSint8>*)phdl->m_obj)->size(); break;
		case CPI_UINT16_SIG:
			size = ((OW_Array<cpiUint16>*)phdl->m_obj)->size(); break;
		case CPI_SINT16_SIG:
			size = ((OW_Array<cpiSint16>*)phdl->m_obj)->size(); break;
		case CPI_UINT32_SIG:
			size = ((OW_Array<cpiUint32>*)phdl->m_obj)->size(); break;
		case CPI_SINT32_SIG:
			size = ((OW_Array<cpiSint32>*)phdl->m_obj)->size(); break;
		case CPI_UINT64_SIG:
			size = ((OW_Array<cpiUint64>*)phdl->m_obj)->size(); break;
		case CPI_SINT64_SIG:
			size = ((OW_Array<cpiSint64>*)phdl->m_obj)->size(); break;
		case CPI_REAL32_SIG:
			size = ((OW_Array<cpiReal32>*)phdl->m_obj)->size(); break;
		case CPI_REAL64_SIG:
			size = ((OW_Array<cpiReal64>*)phdl->m_obj)->size(); break;
		case CPI_CHAR16_SIG:
			size = ((OW_Array<cpiChar16>*)phdl->m_obj)->size(); break;
		case CPI_STRING_SIG:
			size = ((OW_Array<OW_String>*)phdl->m_obj)->size(); break;
		default:
			if(OW_CPI::validObjectSig(phdl->m_type))
			{
				size = ((CPI_ObjVector*)phdl->m_obj)->size();
			}
			else
			{
				return CPI_E_INVALID_DATATYPE;
			}
	}

	return int(size);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiVectClear(cpiVectHdl vect)
{
	CPI_VectorHandle* phdl = OW_CPI::castVectPtr(vect);
	if(!phdl)
	{
		return CPI_E_INVALID_VECTHANDLE;
	}

	if(!phdl->m_obj)
	{
		return CPI_SUCCESS;
	}

	switch(phdl->m_type)
	{
		case CPI_BOOL_SIG:
			((OW_Array<cpiBool>*)phdl->m_obj)->clear(); break;
		case CPI_UINT8_SIG:
			((OW_Array<cpiUint8>*)phdl->m_obj)->clear(); break;
		case CPI_SINT8_SIG:
			((OW_Array<cpiSint8>*)phdl->m_obj)->clear(); break;
		case CPI_UINT16_SIG:
			((OW_Array<cpiUint16>*)phdl->m_obj)->clear(); break;
		case CPI_SINT16_SIG:
			((OW_Array<cpiSint16>*)phdl->m_obj)->clear(); break;
		case CPI_UINT32_SIG:
			((OW_Array<cpiUint32>*)phdl->m_obj)->clear(); break;
		case CPI_SINT32_SIG:
			((OW_Array<cpiSint32>*)phdl->m_obj)->clear(); break;
		case CPI_UINT64_SIG:
			((OW_Array<cpiUint64>*)phdl->m_obj)->clear(); break;
		case CPI_SINT64_SIG:
			((OW_Array<cpiSint64>*)phdl->m_obj)->clear(); break;
		case CPI_REAL32_SIG:
			((OW_Array<cpiReal32>*)phdl->m_obj)->clear(); break;
		case CPI_REAL64_SIG:
			((OW_Array<cpiReal64>*)phdl->m_obj)->clear(); break;
		case CPI_CHAR16_SIG:
			((OW_Array<cpiChar16>*)phdl->m_obj)->clear(); break;
		case CPI_STRING_SIG:
			((OW_Array<OW_String>*)phdl->m_obj)->clear(); break;
		default:
			if(OW_CPI::validObjectSig(phdl->m_type))
			{
				((CPI_ObjVector*)phdl->m_obj)->clear();
			}
			else
			{
				return CPI_E_INVALID_DATATYPE;
			}
	}

	return CPI_SUCCESS;
}

