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
#include "OW_CIMNameSpace.hpp"
#include "owcpiprov.h"
#include "OW_CPI.hpp"

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiNSAlloc(cpiNSHdl* pns)
{
	*pns = (cpiNSHdl*) new CPI_NameSpaceHandle(new OW_CIMNameSpace(true));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiNSFree(cpiNSHdl ns)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
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
cpiNSToString(cpiNSHdl ns, int* result)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
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
cpiNSGetNameSpacePart(cpiNSHdl ns, int* result)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getNameSpace().c_str();

}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiNSSetNameSpacePart(cpiNSHdl ns, const char* nsPart)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setNameSpace(OW_String(nsPart));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiNSGetHostPart(cpiNSHdl ns, int* result)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getHost().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiNSSetHostPart(cpiNSHdl ns, const char* hostPart)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setHost(OW_String(hostPart));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiNSGetProtocolPart(cpiNSHdl ns, int* result)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getProtocol().c_str();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiNSSetProtocolPart(cpiNSHdl ns, const char* protocolPart)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	(*phdl)->setProtocol(OW_String(protocolPart));
	return CPI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT int 
cpiNSGetPort(cpiNSHdl ns)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		return CPI_E_INVALID_OBJHANDLE;
	}

	return (*phdl)->getPortNumber();
}

//////////////////////////////////////////////////////////////////////////////
CPI_EXPORT const char* 
cpiNSGetFileNamePart(cpiNSHdl ns, int* result)
{
	CPI_NameSpaceHandle* phdl = OW_CPI::castNSPtr(ns);
	if(!phdl)
	{
		if(result)
			*result = CPI_E_INVALID_OBJHANDLE;
		return 0;
	}

	if(result)
		*result = CPI_SUCCESS;

	return (*phdl)->getFileName().c_str();
}


