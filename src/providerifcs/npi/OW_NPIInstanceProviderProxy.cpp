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
#include "OW_NPIInstanceProviderProxy.hpp"
#include "OW_CIMClass.hpp"

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_NPIInstanceProviderProxy::enumInstances(
        const OW_ProviderEnvironmentIFCRef& env,
        OW_CIMObjectPath cop,
        OW_Bool deep,
        OW_CIMClass cimClass )
{
	(void)env; (void)cop; (void)deep; (void)cimClass;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_NPIInstanceProviderProxy::enumInstances(
        const OW_ProviderEnvironmentIFCRef& env,
        OW_CIMObjectPath cop,
        OW_Bool deep,
        OW_CIMClass cimClass,
        OW_Bool localOnly )
{
	(void)env; (void)cop; (void)deep; (void)cimClass; (void)localOnly;
}
	
/////////////////////////////////////////////////////////////////////////////
void
OW_NPIInstanceProviderProxy::deleteInstance(const OW_ProviderEnvironmentIFCRef &env,
    OW_CIMObjectPath cop)
{
	(void)env; (void)cop;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_NPIInstanceProviderProxy::getInstance(const OW_ProviderEnvironmentIFCRef &env,
    OW_CIMObjectPath cop, OW_CIMClass cimClass, OW_Bool localOnly)
{
	(void)env; (void)cop; (void)cimClass; (void)localOnly;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_NPIInstanceProviderProxy::createInstance(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath cop,
    OW_CIMInstance cimInstance)
{
	(void)env; (void)cop; (void)cimInstance;
}

/////////////////////////////////////////////////////////////////////////////
void
OW_NPIInstanceProviderProxy::setInstance(const OW_ProviderEnvironmentIFCRef &env,
    OW_CIMObjectPath cop, OW_CIMInstance cimInstance)
{
	(void)env; (void)cop; (void)cimInstance;
}


