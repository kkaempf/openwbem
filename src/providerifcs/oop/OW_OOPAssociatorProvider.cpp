/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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
 * @author Jon Carey
 */

#include "OW_config.h"
#include "OW_OOPAssociatorProvider.hpp"
#include "OW_OOPShuttingDownCallback.hpp"
#include "OW_OOPProtocolIFC.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.provider.OOP.ifc");
}


OOPAssociatorProvider::OOPAssociatorProvider(const OOPProviderInterface::ProvRegInfo& info,
	const OOPProcessState& processState)
	: OOPProviderBase(info, processState)
{
}
OOPAssociatorProvider::~OOPAssociatorProvider()
{
}

namespace
{
	class AssociatorsCallback : public OOPProviderBase::MethodCallback
	{
	public:
		AssociatorsCallback(
			CIMInstanceResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& assocClass,
			const String& resultClass,
			const String& role,
			const String& resultRole,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
			: m_result(result)
			, m_ns(ns)
			, m_objectName(objectName)
			, m_assocClass(assocClass)
			, m_resultClass(resultClass)
			, m_role(role)
			, m_resultRole(resultRole)
			, m_includeQualifiers(includeQualifiers)
			, m_includeClassOrigin(includeClassOrigin)
			, m_propertyList(propertyList)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, 
			const UnnamedPipeRef& in, const Timeout& timeout, 
			const ProviderEnvironmentIFCRef& env) const
		{
			protocol->associators(out, in, timeout, env, m_result, m_ns, m_objectName, m_assocClass,
				m_resultClass, m_role, m_resultRole, m_includeQualifiers, m_includeClassOrigin,
				m_propertyList);
		}

	private:

		CIMInstanceResultHandlerIFC& m_result;
		const String& m_ns;
		const CIMObjectPath& m_objectName;
		const String& m_assocClass;
		const String& m_resultClass;
		const String& m_role;
		const String& m_resultRole;
		WBEMFlags::EIncludeQualifiersFlag m_includeQualifiers;
		WBEMFlags::EIncludeClassOriginFlag m_includeClassOrigin;
		const StringArray* m_propertyList;
	};
}
void
OOPAssociatorProvider::associators(
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	AssociatorsCallback associatorsCallback(result, ns, objectName, assocClass, resultClass,
		role, resultRole, includeQualifiers, includeClassOrigin, propertyList);

	startProcessAndCallFunction(env, associatorsCallback, "OOPInstanceProvider::associators");
}

namespace
{
	class AssociatorNamesCallback : public OOPProviderBase::MethodCallback
	{
	public:
		AssociatorNamesCallback(
			CIMObjectPathResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& assocClass,
			const String& resultClass,
			const String& role,
			const String& resultRole )
			: m_result(result)
			, m_ns(ns)
			, m_objectName(objectName)
			, m_assocClass(assocClass)
			, m_resultClass(resultClass)
			, m_role(role)
			, m_resultRole(resultRole)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, 
			const UnnamedPipeRef& in, const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->associatorNames(out, in, timeout, env, m_result, m_ns, m_objectName, m_assocClass,
				m_resultClass, m_role, m_resultRole);
		}

	private:
		CIMObjectPathResultHandlerIFC& m_result;
		const String& m_ns;
		const CIMObjectPath& m_objectName;
		const String& m_assocClass;
		const String& m_resultClass;
		const String& m_role;
		const String& m_resultRole;
	};
}

void 
OOPAssociatorProvider::associatorNames(
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole )
{
	AssociatorNamesCallback associatorNamesCallback(result, ns, objectName, assocClass,
		resultClass, role, resultRole);
	startProcessAndCallFunction(env, associatorNamesCallback, "OOPInstanceProvider::associatorNames");
}

namespace
{
	class ReferencesCallback : public OOPProviderBase::MethodCallback
	{
	public:
		ReferencesCallback(
			CIMInstanceResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& resultClass,
			const String& role,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
			: m_result(result)
			, m_ns(ns)
			, m_objectName(objectName)
			, m_resultClass(resultClass)
			, m_role(role)
			, m_includeQualifiers(includeQualifiers)
			, m_includeClassOrigin(includeClassOrigin)
			, m_propertyList(propertyList)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, 
			const UnnamedPipeRef& in, const Timeout& timeout, 
			const ProviderEnvironmentIFCRef& env) const
		{
			protocol->references(out, in, timeout, env, m_result, m_ns, m_objectName,
				m_resultClass, m_role, m_includeQualifiers, m_includeClassOrigin,
				m_propertyList);
		}

	private:

		CIMInstanceResultHandlerIFC& m_result;
		const String& m_ns;
		const CIMObjectPath& m_objectName;
		const String& m_resultClass;
		const String& m_role;
		WBEMFlags::EIncludeQualifiersFlag m_includeQualifiers;
		WBEMFlags::EIncludeClassOriginFlag m_includeClassOrigin;
		const StringArray* m_propertyList;
	};
}

void
OOPAssociatorProvider::references(
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	ReferencesCallback referencesCallback(result, ns, objectName, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList);
	startProcessAndCallFunction(env, referencesCallback, "OOPInstanceProvider::references");
}

namespace
{
	class ReferenceNamesCallback : public OOPProviderBase::MethodCallback
	{
	public:
		ReferenceNamesCallback(
			CIMObjectPathResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& resultClass,
			const String& role )
			: m_result(result)
			, m_ns(ns)
			, m_objectName(objectName)
			, m_resultClass(resultClass)
			, m_role(role)
		{
		}

		virtual void call(const OOPProtocolIFCRef& protocol, const UnnamedPipeRef& out, 
			const UnnamedPipeRef& in, const Timeout& timeout, const ProviderEnvironmentIFCRef& env) const
		{
			protocol->referenceNames(out, in, timeout, env, m_result, m_ns, m_objectName,
				m_resultClass, m_role);
		}

	private:

		CIMObjectPathResultHandlerIFC& m_result;
		const String& m_ns;
		const CIMObjectPath& m_objectName;
		const String& m_resultClass;
		const String& m_role;
	};
}

void
OOPAssociatorProvider::referenceNames(
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role )
{
	ReferenceNamesCallback referenceNamesCallback(result, ns, objectName, resultClass, role);
	startProcessAndCallFunction(env, referenceNamesCallback, "OOPInstanceProvider::referenceNames");
}

//////////////////////////////////////////////////////////////////////////////
void
OOPAssociatorProvider::shuttingDown(const ProviderEnvironmentIFCRef& env)
{
	OOPShuttingDownCallback shuttingDownCallback;
	startProcessAndCallFunction(env, shuttingDownCallback, "OOPAssociatorProvider::shuttingDown");
}


} // end namespace OW_NAMESPACE








