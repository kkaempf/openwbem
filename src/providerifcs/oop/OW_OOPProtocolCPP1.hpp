/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#ifndef OW_OOP_PROTOCOL_CPP1_HPP_INCLUDE_GUARD_
#define OW_OOP_PROTOCOL_CPP1_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_OOPProtocolIFC.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OOPProtocolCPP1 : public OOPProtocolIFC
{
public:
	OOPProtocolCPP1(OOPProviderBase* pprov);
	virtual ~OOPProtocolCPP1();

	virtual void enumInstanceNames(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass);

	virtual void enumInstances(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const blocxx::String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EDeepFlag deep,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass);

	virtual CIMInstance getInstance(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList,
		const CIMClass& cimClass);

	virtual CIMObjectPath createInstance(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMInstance& cimInstance);

	virtual void modifyInstance(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const blocxx::StringArray* propertyList,
		const CIMClass& theClass);

	virtual void deleteInstance(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMObjectPath& cop);

	virtual void associators(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String& ns,
		const CIMObjectPath& objectName,
		const blocxx::String& assocClass,
		const blocxx::String& resultClass,
		const blocxx::String& role,
		const blocxx::String& resultRole,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);

	virtual void associatorNames(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String& ns,
		const CIMObjectPath& objectName,
		const blocxx::String& assocClass,
		const blocxx::String& resultClass,
		const blocxx::String& role,
		const blocxx::String& resultRole);

	virtual void references(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const blocxx::String& ns,
		const CIMObjectPath& objectName,
		const blocxx::String& resultClass,
		const blocxx::String& role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const blocxx::StringArray* propertyList);

	virtual void referenceNames(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const blocxx::String& ns,
		const CIMObjectPath& objectName,
		const blocxx::String& resultClass,
		const blocxx::String& role);

	virtual CIMValue invokeMethod(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMObjectPath& path,
		const blocxx::String& methodName,
		const CIMParamValueArray& inparams,
		CIMParamValueArray& outparams);

	virtual blocxx::Int32 poll(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env);

	virtual blocxx::Int32 getInitialPollingInterval(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env);

	virtual void setPersistent(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		bool persistent);

	virtual void setLogLevel(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		blocxx::ELogLevel logLevel);

	virtual void activateFilter(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const blocxx::String& eventType,
		const blocxx::String& nameSpace,
		const blocxx::StringArray& classes,
		bool firstActivation);

	virtual void authorizeFilter(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const blocxx::String& eventType,
		const blocxx::String& nameSpace,
		const blocxx::StringArray& classes,
		const blocxx::String& owner);

	virtual void deActivateFilter(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const blocxx::String& eventType,
		const blocxx::String& nameSpace,
		const blocxx::StringArray& classes,
		bool lastActivation);

	virtual int mustPoll(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const blocxx::String& eventType,
		const blocxx::String& nameSpace,
		const blocxx::StringArray& classes);

	virtual void exportIndication(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const CIMInstance& indHandlerInst,
		const CIMInstance& indicationInst);

	virtual void shuttingDown(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env);

	virtual void queryInstances(
		const blocxx::UnnamedPipeRef& in,
		const blocxx::UnnamedPipeRef& out,
		const blocxx::Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const blocxx::String& ns,
		const WQLSelectStatement& query,
		CIMInstanceResultHandlerIFC& result,
		const CIMClass& cimClass);

};

} // end namespace OW_NAMESPACE


#endif





