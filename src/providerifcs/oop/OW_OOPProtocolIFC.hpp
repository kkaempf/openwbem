/*******************************************************************************
* Copyright (C) 2004 Quest Software, Inc. All rights reserved.
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

#ifndef OW_OOP_PROTOCOL_IFC_HPP_INCLUDE_GUARD_
#define OW_OOP_PROTOCOL_IFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/IntrusiveCountableBase.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_IfcsFwd.hpp"
#include "OW_WBEMFlags.hpp"
#include "blocxx/LogLevel.hpp"

#include <iosfwd>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OOPProviderBase;

class OOPProtocolIFC : public IntrusiveCountableBase
{
public:
	OOPProtocolIFC(OOPProviderBase* pprov);
	virtual ~OOPProtocolIFC();

	virtual void enumInstanceNames(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass) = 0;


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
		const CIMClass& cimClass) = 0;


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
		const CIMClass& cimClass) = 0;

	virtual CIMObjectPath createInstance(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance) = 0;

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
		const CIMClass& theClass) = 0;

	virtual void deleteInstance(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop) = 0;

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
		const StringArray* propertyList) = 0;

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
		const String& resultRole) = 0;

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
		const StringArray* propertyList) = 0;

	virtual void referenceNames(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role) = 0;

	virtual CIMValue invokeMethod(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& inparams,
		CIMParamValueArray& outparams) = 0;

	virtual Int32 poll(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env) = 0;

	virtual Int32 getInitialPollingInterval(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env) = 0;

	virtual void setPersistent(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		bool persistent) = 0;

	virtual void setLogLevel(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		ELogLevel logLevel) = 0;

	virtual void activateFilter(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		bool firstActivation) = 0;

	virtual void authorizeFilter(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		const String& owner) = 0;

	virtual void deActivateFilter(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes,
		bool lastActivation) = 0;

	virtual int mustPoll(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter,
		const String& eventType,
		const String& nameSpace,
		const StringArray& classes) = 0;

	virtual void exportIndication(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& indHandlerInst,
		const CIMInstance& indicationInst) = 0;

	virtual void shuttingDown(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env) = 0;

	virtual void queryInstances(
		const UnnamedPipeRef& in,
		const UnnamedPipeRef& out,
		const Timeout& timeout,
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const WQLSelectStatement& query,
		CIMInstanceResultHandlerIFC& result,
		const CIMClass& cimClass) = 0;

protected:
	OOPProviderBase* m_pprov;		// Pointer to avoid circular reference
};


} // end namespace OW_NAMESPACE


#endif




