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
#include "OW_NPIAssociatorProviderProxy.hpp"

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_NPIAssociatorProviderProxy::associatorNames(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String resultClass, OW_String role,
    OW_String resultRole)
{
	(void)env; (void)assocName; (void)objectName; (void)resultClass; (void)role; (void)resultRole;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_NPIAssociatorProviderProxy::associators(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String resultClass, OW_String role,
    OW_String resultRole, OW_Bool includeQualifiers,
    OW_Bool includeClassOrigin, const OW_StringArray *propertyList)
{
(void)env; (void)assocName; (void)objectName; (void)resultClass; (void)role; (void)resultRole; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_NPIAssociatorProviderProxy::references(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String role, OW_Bool includeQualifiers,
    OW_Bool includeClassOrigin, const OW_StringArray *propertyList)
{
	(void)env; (void)assocName; (void)objectName; (void)role; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_NPIAssociatorProviderProxy::referenceNames(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String role)
{
	(void)env; (void)assocName; (void)objectName; (void)role;
}

