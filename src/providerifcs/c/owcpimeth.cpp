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
#include "OW_String.hpp"
#include "OW_CIM.hpp"
#include "owcpiprov.h"
#include "OW_CPI.hpp"

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethAlloc(cpiMethHdl* pMeth)
{
	*pMeth = (cpiMethHdl*) new CPI_MethodHandle(new OW_CIMMethod(true));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethFree(cpiMethHdl meth)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	phdl->deleteObj();
	phdl->m_sig = 0;
	delete phdl;
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiMethGetName(cpiMethHdl meth, int* result)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getName().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethSetName(cpiMethHdl meth, const char* name)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setName(OW_String(name));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiMethToString(cpiMethHdl meth, int* result)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->toString().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethGetQual(cpiMethHdl meth, const char* qualName, cpiQualHdl* pQual)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifier qual = (*phdl)->getQualifier(OW_String(qualName));
	if(!qual)
	{
		return CPI_E_NOTFOUND;
	}

	return OW_CPI::qualToObjHdl(qual, *pQual);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethGetQuals(cpiMethHdl meth, cpiVectHdl* qualifiers)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifierArray qra = (*phdl)->getQualifiers();
	return OW_CPI::qualsToVect(qra, *qualifiers);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethAddQual(cpiMethHdl meth, cpiQualHdl qualifier)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifier qual;
	int cc = OW_CPI::objHdlToQual(qualifier, qual);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->addQualifier(qual);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethSetQuals(cpiMethHdl meth, cpiVectHdl qualifiers)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifierArray qra;
	int cc = OW_CPI::vectToQuals(qualifiers, qra);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->setQualifiers(qra);
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiMethGetOriginCls(cpiMethHdl meth, int *result)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getOriginClass().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethSetOriginCls(cpiMethHdl meth, const char* originClassName)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setOriginClass(OW_String(originClassName));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethAddParm(cpiMethHdl meth, cpiParmHdl parmArg)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMParameter parm;
	int cc = OW_CPI::objHdlToParm(parmArg, parm);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->addParameter(parm);
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethGetParms(cpiMethHdl meth, cpiVectHdl* parms)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMParameterArray pra = (*phdl)->getParameters();
	return OW_CPI::parmsToVect(pra, *parms);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethSetReturnType(cpiMethHdl meth, int cimDataType)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setReturnType(OW_CIMDataType(cimDataType));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethGetReturnType(cpiMethHdl meth)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	return (*phdl)->getReturnType().getType();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethSetOverridingMeth(cpiMethHdl meth, const char* orMethName)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setOverridingMethod(OW_String(orMethName));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiMethGetOverridingMeth(cpiMethHdl meth, int *result)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getOverridingMethod().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethSetPropagated(cpiMethHdl meth, cpiBool isPropagated)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setPropagated(OW_Bool(isPropagated));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiMethGetPropagated(cpiMethHdl meth, cpiBool* isPropagated)
{
	CPI_MethodHandle* phdl = OW_CPI::castMethodPtr(meth);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	*isPropagated = ((*phdl)->getPropagated()) ? cpiTRUE : cpiFALSE;
	return CPI_SUCCESS;
}

