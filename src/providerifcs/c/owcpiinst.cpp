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
cpiInstAlloc(cpiInstHdl* pInst)
{
	*pInst = (cpiInstHdl*) new CPI_InstanceHandle(new OW_CIMInstance(true));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstFree(cpiInstHdl inst)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
cpiInstGetClsName(cpiInstHdl inst, int* result)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;

		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getClassName().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstSetClsName(cpiInstHdl inst, const char* name)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setClassName(OW_String(name));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiInstToString(cpiInstHdl inst, int* result)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
cpiInstGetKeys(cpiInstHdl inst, cpiVectHdl* keyProperties)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMPropertyArray pra = (*phdl)->getKeyValuePairs();
	return OW_CPI::propsToVect(pra, *keyProperties);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiInstGetAlias(cpiInstHdl inst, int* result)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getAlias().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstSetAlias(cpiInstHdl inst, const char* aliasName)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setAlias(OW_String(aliasName));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstGetQual(cpiInstHdl inst, const char* qualName, cpiQualHdl* pQual)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
cpiInstGetQuals(cpiInstHdl inst, cpiVectHdl* qualifiers)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMQualifierArray qra = (*phdl)->getQualifiers();
	return OW_CPI::qualsToVect(qra, *qualifiers);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstSetQual(cpiInstHdl inst, cpiQualHdl qualifier)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
cpiInstSetQuals(cpiInstHdl inst, cpiVectHdl qualifiers)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
CPI_EXPORT int 
cpiInstRemoveQual(cpiInstHdl inst, const char* qualifierName)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->removeQualifier(OW_String(qualifierName));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstGetProps(cpiInstHdl inst, cpiVectHdl* properties)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMPropertyArray pra = (*phdl)->getProperties();
	return OW_CPI::propsToVect(pra, *properties);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstGetProp(cpiInstHdl inst, const char* propName, cpiPropHdl* pProp)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	OW_CIMProperty prop = (*phdl)->getProperty(OW_String(propName));
	if(!prop)
	{
		return CPI_E_NOTFOUND;
	}

	return OW_CPI::propToObjHdl(prop, *pProp);
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiInstAddProp(cpiInstHdl inst, cpiPropHdl property)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
cpiInstSetProp(cpiInstHdl inst, cpiPropHdl property)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
cpiInstSetProps(cpiInstHdl inst, cpiVectHdl properties)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
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
cpiInstRemoveProp(cpiInstHdl inst, const char* propertyName)
{
	CPI_InstanceHandle* phdl = OW_CPI::castInstancePtr(inst);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->removeProperty(OW_String(propertyName));
	return CPI_SUCCESS;
}

