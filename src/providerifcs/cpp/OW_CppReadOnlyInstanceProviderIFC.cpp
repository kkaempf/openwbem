/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_config.h"
#include "OW_CppReadOnlyInstanceProviderIFC.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMClass.hpp"

using namespace OW_WBEMFlags;

//////////////////////////////////////////////////////////////////////////////
void 
OW_CppReadOnlyInstanceProviderIFC::deleteInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &)
{
	OW_THROWCIMMSG(OW_CIMException::FAILED, "DeleteInstance not supported by provider");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath 
OW_CppReadOnlyInstanceProviderIFC::createInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMInstance &)
{
	OW_THROWCIMMSG(OW_CIMException::FAILED, "DeleteInstance not supported by provider");
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_CppReadOnlyInstanceProviderIFC::modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		const OW_CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_CIMClass& theClass)
{
	(void)env; (void)ns; (void)modifiedInstance; (void)previousInstance; (void)includeQualifiers; (void)propertyList; (void)theClass;
	OW_THROWCIMMSG(OW_CIMException::FAILED, "DeleteInstance not supported by provider");
}


