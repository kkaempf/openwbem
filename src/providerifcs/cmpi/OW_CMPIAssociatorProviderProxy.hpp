/*******************************************************************************
* Copyright (C) 2003-2004 Quest Software, Inc., IBM Corp. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* Author:        Markus Mueller <sedgewick_de@yahoo.de>
*
*******************************************************************************/
#ifndef OW_CMPI_ASSOCIATOR_PROVIDER_PROXY_HPP_
#define OW_CMPI_ASSOCIATOR_PROVIDER_PROXY_HPP_
#include "OW_config.h"
#include "OW_AssociatorProviderIFC.hpp"
#include "OW_FTABLERef.hpp"

namespace OW_NAMESPACE
{

class CMPIAssociatorProviderProxy : public AssociatorProviderIFC
{
public:
	CMPIAssociatorProviderProxy(const CMPIFTABLERef& f)
	: m_ftable(f)
	{
	}
	virtual ~CMPIAssociatorProviderProxy()
	{
	}
	virtual void associatorNames(
		const ProviderEnvironmentIFCRef &env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole);

	virtual void associators(
		const ProviderEnvironmentIFCRef &env,
		CIMInstanceResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList);

	virtual void references(
		const ProviderEnvironmentIFCRef &env,
		CIMInstanceResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList);

	virtual void referenceNames(
		const ProviderEnvironmentIFCRef &env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role);

	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);
private:
	CMPIFTABLERef m_ftable;
};

} // end namespace OW_NAMESPACE

#endif
