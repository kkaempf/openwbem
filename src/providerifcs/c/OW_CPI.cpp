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
#include "OW_CPI.hpp"

//////////////////////////////////////////////////////////////////////////////
int 
OW_CPI::objHdlToParm(cpiObjHdl ohdl, OW_CIMParameter& parm)
{
	CPI_ParameterHandle* phdl = (CPI_ParameterHandle*)ohdl;
	if(phdl->m_sig != CPI_PARM_SIG)
	{
		return CPI_E_INVALID_PARMHANDLE;
	}

	parm = *(*phdl);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_CPI::parmToObjHdl(const OW_CIMParameter& parm, cpiObjHdl& ohdl)
{
	CPI_ParameterHandle* phdl = new CPI_ParameterHandle(
		new OW_CIMParameter(parm));

	ohdl = (cpiObjHdl) phdl;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_CPI::vectToParms(cpiVectHdl vect, OW_CIMParameterArray& pra)
{
	pra.clear();
	unsigned int vt = cpiVectorType(vect);
	if(vt != CPI_PARM_SIG)
	{
		return CPI_E_INVALID_PARMVECTOR;
	}

	int size = cpiVectSize(vect);
	if(size < 0)
	{
		return size;
	}

	cpiObjHdl ohdl;
	for(int i = 0; i < size; i++)
	{
		int cc = cpiVectorGet(vect, &ohdl, size_t(i));
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		OW_CIMParameter parm;
		cc = objHdlToParm(ohdl, parm);
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		pra.append(parm);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_CPI::parmsToVect(const OW_CIMParameterArray& pra, cpiVectHdl& vhdl)
{
	cpiVectorAlloc(&vhdl, CPI_PARM_SIG);
	for(size_t i = 0; i < pra.size(); i++)
	{
		CPI_ParameterHandle* phdl = new CPI_ParameterHandle(
			new OW_CIMParameter(pra[i]));
		cpiVectorAppend(vhdl, (cpiObjHdl)phdl);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::objHdlToMeth(cpiObjHdl ohdl, OW_CIMMethod& meth)
{
	CPI_MethodHandle* phdl = (CPI_MethodHandle*)ohdl;
	if(phdl->m_sig != CPI_METHOD_SIG)
	{
		return CPI_E_INVALID_METHHANDLE;
	}

	meth = *(*phdl);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_CPI::methToObjHdl(const OW_CIMMethod& meth, cpiObjHdl& ohdl)
{
	CPI_MethodHandle* phdl = new CPI_MethodHandle(new OW_CIMMethod(meth));
	ohdl = (cpiObjHdl) phdl;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_CPI::vectToMeths(cpiVectHdl vect, OW_CIMMethodArray& mra)
{
	mra.clear();
	unsigned int vt = cpiVectorType(vect);
	if(vt != CPI_METHOD_SIG)
	{
		return CPI_E_INVALID_METHVECTOR;
	}

	int size = cpiVectSize(vect);
	if(size < 0)
	{
		return size;
	}

	cpiObjHdl ohdl;
	for(int i = 0; i < size; i++)
	{
		int cc = cpiVectorGet(vect, &ohdl, size_t(i));
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		OW_CIMMethod meth;
		cc = objHdlToMeth(ohdl, meth);
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		mra.append(meth);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_CPI::methsToVect(const OW_CIMMethodArray& mra, cpiVectHdl& vhdl)
{
	cpiVectorAlloc(&vhdl, CPI_METHOD_SIG);
	for(size_t i = 0; i < mra.size(); i++)
	{
		CPI_MethodHandle* phdl = new CPI_MethodHandle(
			new OW_CIMMethod(mra[i]));
		cpiVectorAppend(vhdl, (cpiObjHdl)phdl);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::vectToProps(cpiVectHdl vect, OW_CIMPropertyArray& pra)
{
	pra.clear();
	unsigned int vt = cpiVectorType(vect);
	if(vt != CPI_PROP_SIG)
	{
		return CPI_E_INVALID_PROPVECTOR;
	}

	int size = cpiVectSize(vect);
	if(size < 0)
	{
		return size;
	}

	cpiObjHdl ohdl;
	for(int i = 0; i < size; i++)
	{
		int cc = cpiVectorGet(vect, &ohdl, size_t(i));
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		OW_CIMProperty prop;
		cc = objHdlToProp(ohdl, prop);
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		pra.append(prop);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::propsToVect(const OW_CIMPropertyArray& pra, cpiVectHdl& vhdl)
{
	cpiVectorAlloc(&vhdl, CPI_PROP_SIG);
	for(size_t i = 0; i < pra.size(); i++)
	{
		CPI_PropertyHandle* phdl = new CPI_PropertyHandle(
			new OW_CIMProperty(pra[i]));
		cpiVectorAppend(vhdl, (cpiObjHdl)phdl);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::propToObjHdl(const OW_CIMProperty& prop, cpiObjHdl& ohdl)
{
	CPI_PropertyHandle* phdl = new CPI_PropertyHandle(new OW_CIMProperty(prop));
	ohdl = (cpiObjHdl) phdl;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::qualsToVect(const OW_CIMQualifierArray& qra, cpiVectHdl& vhdl)
{
	cpiVectorAlloc(&vhdl, CPI_QUAL_SIG);
	for(size_t i = 0; i < qra.size(); i++)
	{
		CPI_QualifierHandle* phdl = new CPI_QualifierHandle(
			new OW_CIMQualifier(qra[i]));
		cpiVectorAppend(vhdl, (cpiObjHdl)phdl);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::qualToObjHdl(const OW_CIMQualifier& qual, cpiObjHdl& ohdl)
{
	CPI_QualifierHandle* phdl = new CPI_QualifierHandle(
		new OW_CIMQualifier(qual));

	ohdl = (cpiObjHdl) phdl;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::objHdlToQual(cpiObjHdl ohdl, OW_CIMQualifier& qual)
{
	CPI_QualifierHandle* phdl = (CPI_QualifierHandle*)ohdl;
	if(phdl->m_sig != CPI_QUAL_SIG)
	{
		return CPI_E_INVALID_QUALHANDLE;
	}

	qual = *(*phdl);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::objHdlToProp(cpiObjHdl ohdl, OW_CIMProperty& prop)
{
	CPI_PropertyHandle* phdl = (CPI_PropertyHandle*)ohdl;
	if(phdl->m_sig != CPI_PROP_SIG)
	{
		return CPI_E_INVALID_PROPHANDLE;
	}

	prop = *(*phdl);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_CPI::vectToQuals(cpiVectHdl vect, OW_CIMQualifierArray& qra)
{
	qra.clear();
	unsigned int vt = cpiVectorType(vect);
	if(vt != CPI_QUAL_SIG)
	{
		return CPI_E_INVALID_QUALVECTOR;
	}

	int size = cpiVectSize(vect);
	if(size < 0)
	{
		return size;
	}

	cpiObjHdl ohdl;
	for(int i = 0; i < size; i++)
	{
		int cc = cpiVectorGet(vect, &ohdl, size_t(i));
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		OW_CIMQualifier qual;
		cc = objHdlToQual(ohdl, qual);
		if(cc != CPI_SUCCESS)
		{
			return cc;
		}

		qra.append(qual);
	}

	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_InstanceHandle*
OW_CPI::castInstancePtr(void* p)
{
	CPI_InstanceHandle* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_InstanceHandle*) p;
		if(objPtr->m_sig != CPI_INSTANCE_SIG)
		{
			objPtr =0;
		}
	}

	return objPtr;
}

//////////////////////////////////////////////////////////////////////////////
CPI_MethodHandle*
OW_CPI::castMethodPtr(void* p)
{
	CPI_MethodHandle* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_MethodHandle*) p;
		if(objPtr->m_sig != CPI_METHOD_SIG)
		{
			objPtr =0;
		}
	}

	return objPtr;
}

//////////////////////////////////////////////////////////////////////////////
CPI_ParameterHandle*
OW_CPI::castParmPtr(void* p)
{
	CPI_ParameterHandle* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_ParameterHandle*) p;
		if(objPtr->m_sig != CPI_PARM_SIG)
		{
			objPtr =0;
		}
	}

	return objPtr;
}

//////////////////////////////////////////////////////////////////////////////
CPI_NameSpaceHandle*
OW_CPI::castNSPtr(void* p)
{
	CPI_NameSpaceHandle* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_NameSpaceHandle*) p;
		if(objPtr->m_sig != CPI_NS_SIG)
		{
			objPtr =0;
		}
	}

	return objPtr;
}

//////////////////////////////////////////////////////////////////////////////
CPI_ClassHandle*
OW_CPI::castClassPtr(void* p)
{
	CPI_ClassHandle* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_ClassHandle*) p;
		if(objPtr->m_sig != CPI_CLASS_SIG)
		{
			objPtr =0;
		}
	}

	return objPtr;
}

//////////////////////////////////////////////////////////////////////////////			   
CPI_QualifierHandle*
OW_CPI::castQualifierPtr(void* p)
{
	CPI_QualifierHandle* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_QualifierHandle*) p;
		if(objPtr->m_sig != CPI_QUAL_SIG)
		{
			objPtr =0;
		}
	}

	return objPtr;
}

//////////////////////////////////////////////////////////////////////////////			   
CPI_PropertyHandle*
OW_CPI::castPropertyPtr(void* p)
{
	CPI_PropertyHandle* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_PropertyHandle*) p;
		if(objPtr->m_sig != CPI_PROP_SIG)
		{
			objPtr =0;
		}
	}

	return objPtr;
}

//////////////////////////////////////////////////////////////////////////////			   
bool
OW_CPI::validObjectSig(unsigned int sig)
{
	switch(sig)
	{
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
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////			   
CPI_VectorHandle*
OW_CPI::castVectPtr(void* p)
{
	CPI_VectorHandle* hdl = 0;
	if(p)
	{
		hdl = (CPI_VectorHandle*) p;
		if(hdl->m_sig != CPI_VECT_SIG)
		{
			hdl = 0;
		}
	}
	return hdl;
}

//////////////////////////////////////////////////////////////////////////////
CPI_ObjHandleBase*
OW_CPI::castObjPtr(void* p)
{
	CPI_ObjHandleBase* objPtr = 0;
	if(p)
	{
		objPtr = (CPI_ObjHandleBase*) p;
		if(!validObjectSig(objPtr->m_sig))
		{
			objPtr =0;
		}
	}

	return objPtr;
}

