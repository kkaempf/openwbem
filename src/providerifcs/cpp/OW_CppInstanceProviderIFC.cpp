/*******************************************************************************
* Copyright (C) 2003-2006 Novell Inc All rights reserved.
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
*  - Neither the name of Novell nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 */

#include "OW_config.h"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_ResultHandlerIFC.hpp"

namespace OW_NAMESPACE
{

using namespace blocxx;
    class _RHEnumInstances : public CIMObjectPathResultHandlerIFC
    {
    public:
        _RHEnumInstances(
          CppInstanceProviderIFC& ip,
          const ProviderEnvironmentIFCRef& env,
          const String& ns,
          CIMInstanceResultHandlerIFC& result,
          WBEMFlags::ELocalOnlyFlag localOnly,
          WBEMFlags::EDeepFlag deep,
          WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
          WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
          const StringArray* propertyList,
          const CIMClass& cimClass )
            : _ip(ip)
            , _env(env)
            , _ns(ns)
            , _realHandler(result)
            , _localOnly(localOnly)
            , _includeQualifiers(includeQualifiers)
            , _includeClassOrigin(includeClassOrigin)
            , _propertyList(propertyList)
            , _cimClass(cimClass)
        {}

        void doHandle(const CIMObjectPath& cop)
        {
            CIMInstance inst = _ip.getInstance(_env,_ns, cop,_localOnly,
                                                _includeQualifiers,
                                                _includeClassOrigin,
                                                _propertyList, _cimClass);
            _realHandler.handle(inst);
        }
    private:
        CppInstanceProviderIFC& _ip;
        const ProviderEnvironmentIFCRef& _env;
        const String& _ns;
        CIMInstanceResultHandlerIFC& _realHandler;
        WBEMFlags::ELocalOnlyFlag _localOnly;
        WBEMFlags::EIncludeQualifiersFlag _includeQualifiers;
        WBEMFlags::EIncludeClassOriginFlag _includeClassOrigin;
        const StringArray* _propertyList;
        const CIMClass& _cimClass;
    };
///////////////////////////////////////////////////////////////////////////////

    void
    CppInstanceProviderIFC::enumInstances(
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
                                      const CIMClass& cimClass )
    {
        _RHEnumInstances rh(*this,env , ns,result ,localOnly ,deep ,includeQualifiers ,
              includeClassOrigin ,propertyList , cimClass);
        enumInstanceNames(env,ns ,className , rh,cimClass);
    }

///////////////////////////////////////////////////////////////////////////////
	void
	CppInstanceProviderIFC::getInstanceProviderInfoWithEnv(
		const ProviderRegistrationEnvironmentIFCRef& env,
		InstanceProviderInfo& info)
	{
		return getInstanceProviderInfo(info);
	}

///////////////////////////////////////////////////////////////////////////////
	void
	CppInstanceProviderIFC::getInstanceProviderInfo(InstanceProviderInfo& info)
	{
	}

///////////////////////////////////////////////////////////////////////////////
	CppInstanceProviderIFC*
	CppInstanceProviderIFC::getInstanceProvider()
	{
		return this;
	}

///////////////////////////////////////////////////////////////////////////////
	CppInstanceProviderIFC::CppInstanceProviderIFC()
	{
	}

///////////////////////////////////////////////////////////////////////////////
	CppInstanceProviderIFC::~CppInstanceProviderIFC()
	{
	}

} // end namespace OW_NAMESPACE

