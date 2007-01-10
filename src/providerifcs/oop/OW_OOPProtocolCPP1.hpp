/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass);
	
	virtual void enumInstances(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		WBEMFlags::ELocalOnlyFlag localOnly, 
		WBEMFlags::EDeepFlag deep, 
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass);
	
	virtual CIMInstance getInstance(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		WBEMFlags::ELocalOnlyFlag localOnly,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, 
		const CIMClass& cimClass);

	virtual CIMObjectPath createInstance(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance);
	
	virtual void modifyInstance(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass);
	
	virtual void deleteInstance(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop);

	virtual void associators(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
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
		const StringArray* propertyList);

	virtual void associatorNames(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole);

	virtual void references(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMInstanceResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role,
		WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList);

	virtual void referenceNames(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role);

	virtual CIMValue invokeMethod(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& inparams,
		CIMParamValueArray& outparams);

	virtual Int32 poll(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env);

	virtual Int32 getInitialPollingInterval(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env);

	virtual void setPersistent(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		bool persistent);

	virtual void activateFilter(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		bool firstActivation);
	
	virtual void authorizeFilter(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		const String& owner);
	
	virtual void deActivateFilter(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		bool lastActivation);
	
	virtual int mustPoll(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes);

	virtual void exportIndication(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& indHandlerInst, 
		const CIMInstance& indicationInst);

	virtual void shuttingDown(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env);

	virtual void queryInstances(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const WQLSelectStatement& query,
		CIMInstanceResultHandlerIFC& result,
		const CIMClass& cimClass);

};

} // end namespace OW_NAMESPACE


#endif





