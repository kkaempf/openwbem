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
cpiClsAlloc(cpiClsHdl* pCls)
{
	*pCls = (cpiClsHdl*) new CPI_ClassHandle(new OW_CIMClass(true));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsFree(cpiClsHdl cls)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
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
CPI_EXPORT int 
cpiClsNewInst(cpiClsHdl cls, cpiInstHdl* instance)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMInstance* pi = new OW_CIMInstance((*phdl)->newInstance());
	*instance = new CPI_InstanceHandle(pi);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiClsGetName(cpiClsHdl cls, int* result)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
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
cpiClsSetName(cpiClsHdl cls, const char* name)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setName(OW_String(name));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiClsToString(cpiClsHdl cls, int* result)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
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
CPI_EXPORT const char* 
cpiClsGetSuperCls(cpiClsHdl cls, int* result)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getSuperClass().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsSetSuperCls(cpiClsHdl cls, const char* superClassName)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setSuperClass(superClassName);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsGetQual(cpiClsHdl cls, const char* qualName, cpiQualHdl* pQualifier)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifier cq = (*phdl)->getQualifier(qualName);
	if(!cq)
	{
		return CPI_E_NOTFOUND;
	}

	return OW_CPI::qualToObjHdl(cq, *pQualifier);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsGetQuals(cpiClsHdl cls, cpiVectHdl* qualifiers)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifierArray qra = (*phdl)->getQualifiers();
	return OW_CPI::qualsToVect(qra, *qualifiers);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsAddQual(cpiClsHdl cls, cpiQualHdl qualifier)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
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
cpiClsSetQual(cpiClsHdl cls, cpiQualHdl qualifier)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifier qual;
	int cc = OW_CPI::objHdlToQual(qualifier, qual);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->setQualifier(qual);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsSetQuals(cpiClsHdl cls, cpiVectHdl qualifiers)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifierArray qra;
	int cc =  OW_CPI::vectToQuals(qualifiers, qra);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->setQualifiers(qra);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsRemoveQual(cpiClsHdl cls, const char* qualifierName)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->removeQualifier(OW_String(qualifierName));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsGetProps(cpiClsHdl cls, cpiVectHdl* properties)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMPropertyArray pra = (*phdl)->getAllProperties();
	return OW_CPI::propsToVect(pra, *properties);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsGetProp(cpiClsHdl cls, const char* propName, cpiPropHdl* pProperty)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}
	OW_CIMProperty prop = (*phdl)->getProperty(propName);
	if(!prop)
	{
		return CPI_E_NOTFOUND;
	}

	return OW_CPI::propToObjHdl(prop, *pProperty);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsAddProp(cpiClsHdl cls, cpiPropHdl property)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMProperty prop;
	int cc = OW_CPI::objHdlToProp(property, prop);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->addProperty(prop);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsSetProp(cpiClsHdl cls, cpiPropHdl property)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMProperty prop;
	int cc = OW_CPI::objHdlToProp(property, prop);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->setProperty(prop);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsSetProps(cpiClsHdl cls, cpiVectHdl properties)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMPropertyArray pra;
	int cc = OW_CPI::vectToProps(properties, pra);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->setProperties(pra);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsRemoveProp(cpiClsHdl cls, const char* propertyName)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->removeProperty(propertyName);
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsGetMeth(cpiClsHdl cls, const char* methName, cpiMethHdl* pMethod)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMMethod meth = (*phdl)->getMethod(methName);
	if(!meth)
	{
		return CPI_E_NOTFOUND;
	}

	return OW_CPI::methToObjHdl(meth, *pMethod);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsGetMeths(cpiClsHdl cls, cpiVectHdl* methods)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMMethodArray mra = (*phdl)->getAllMethods();
	return OW_CPI::methsToVect(mra, *methods);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsAddMeth(cpiClsHdl cls, cpiMethHdl method)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMMethod meth;
	int cc = OW_CPI::objHdlToMeth(method, meth);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->addMethod(meth);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsSetMeth(cpiClsHdl cls, cpiMethHdl method)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMMethod meth;
	int cc = OW_CPI::objHdlToMeth(method, meth);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->setMethod(meth);
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiClsSetMeths(cpiClsHdl cls, cpiVectHdl methods)
{
	CPI_ClassHandle* phdl = OW_CPI::castClassPtr(cls);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMMethodArray meths;
	int cc = OW_CPI::vectToMeths(methods, meths);
	if(cc == CPI_SUCCESS)
	{
		(*phdl)->setMethods(meths);
	}
	return cc;
}
