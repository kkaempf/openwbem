/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#ifndef OWBI1_PROXY_PROVIDER_DETAIL_HPP_INCLUDE_GUARD_
#define OWBI1_PROXY_PROVIDER_DETAIL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OWBI1_config.h"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_AssociatorProviderIFC.hpp"
#endif
#include "OW_InstanceProviderIFC.hpp"
#include "OW_SecondaryInstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"

#include "OWBI1_AssociatorProviderIFC.hpp"
#include "OWBI1_InstanceProviderIFC.hpp"
#include "OWBI1_SecondaryInstanceProviderIFC.hpp"
#include "OWBI1_MethodProviderIFC.hpp"
#include "OWBI1_PolledProviderIFC.hpp"
#include "OWBI1_IndicationExportProviderIFC.hpp"
#include "OWBI1_IndicationProviderIFC.hpp"

#include "OW_SharedLibraryReference.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_BinarySerialization.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_WQLSelectStatement.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OWBI1_ResultHandlerIFC.hpp"
#include "OWBI1_CIMPropertyList.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OWBI1
{

namespace detail
{
	OWBI1::CIMInstance convertCIMInstance(const OpenWBEM::CIMInstance& inst);
	OpenWBEM::CIMInstance convertCIMInstance(const OWBI1::CIMInstance& inst);
	OWBI1::CIMInstanceArray convertCIMInstances(const OpenWBEM::CIMInstanceArray& inst);
	OpenWBEM::CIMInstanceArray convertCIMInstances(const OWBI1::CIMInstanceArray& inst);
	OWBI1::CIMObjectPath convertCIMObjectPath(const OpenWBEM::CIMObjectPath& inst);
	OpenWBEM::CIMObjectPath convertCIMObjectPath(const OWBI1::CIMObjectPath& inst);
	OWBI1::CIMClass convertCIMClass(const OpenWBEM::CIMClass& inst);
	OpenWBEM::CIMClass convertCIMClass(const OWBI1::CIMClass& inst);
	OWBI1::CIMParamValueArray convertCIMParamValues(const OpenWBEM::CIMParamValueArray& inst);
	OpenWBEM::CIMParamValueArray convertCIMParamValues(const OWBI1::CIMParamValueArray& inst);
	OWBI1::CIMValue convertCIMValue(const OpenWBEM::CIMValue& inst);
	OpenWBEM::CIMValue convertCIMValue(const OWBI1::CIMValue& inst);

	OWBI1::ProviderEnvironmentIFCRef createProvEnvWrapper(const OpenWBEM::ProviderEnvironmentIFCRef& env);
	OWBI1::WQLSelectStatement convertWQLSelectStatement(const OpenWBEM::WQLSelectStatement& stmt);
	OWBI1::StringArray convertStringArray(const OpenWBEM::StringArray& in);
	OpenWBEM::StringArray convertStringArray(const OWBI1::StringArray& in);

	OWBI1::WBEMFlags::EIncludeQualifiersFlag convertWBEMFlag(OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers);
	OpenWBEM::WBEMFlags::EIncludeQualifiersFlag convertWBEMFlag(OWBI1::WBEMFlags::EIncludeQualifiersFlag includeQualifiers);
	OWBI1::WBEMFlags::EIncludeClassOriginFlag convertWBEMFlag(OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin);
	OWBI1::WBEMFlags::ELocalOnlyFlag convertWBEMFlag(OpenWBEM::WBEMFlags::ELocalOnlyFlag includeClassOrigin);
	OWBI1::WBEMFlags::EDeepFlag convertWBEMFlag(OpenWBEM::WBEMFlags::EDeepFlag includeClassOrigin);

	OWBI1::CIMPropertyList convertPropertyList(const OpenWBEM::StringArray* propertyList);

	class CIMInstanceResultHandlerWrapper : public CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceResultHandlerWrapper(OpenWBEM::CIMInstanceResultHandlerIFC& wrapped);
	
		void doHandle(const CIMInstance& i);
	private:
		OpenWBEM::CIMInstanceResultHandlerIFC& m_wrapped;
	};
	
	class CIMObjectPathResultHandlerWrapper : public CIMObjectPathResultHandlerIFC
	{
	public:
		CIMObjectPathResultHandlerWrapper(OpenWBEM::CIMObjectPathResultHandlerIFC& wrapped);
	
		void doHandle(const CIMObjectPath& i);
	private:
		OpenWBEM::CIMObjectPathResultHandlerIFC& m_wrapped;
	};
	

} // end namespace ProxyProviderDetail
} // end namespace OWBI1

#endif

