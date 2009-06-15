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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "blocxx/Enumeration.hpp" // aCC needs this.

namespace OW_NAMESPACE
{

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
LocalCIMOMHandle::LocalCIMOMHandle(CIMOMEnvironmentRef env,
	RepositoryIFCRef pRepos, OperationContext& context, EInitialLockFlag initialLock)
	: RepositoryCIMOMHandle(pRepos, context)
	, m_env(env)
	, m_initialLock(initialLock)
{
	switch (m_initialLock)
	{
		case E_NONE:
			// do nothing
		break;
		case E_READ:
			beginOperation(WBEMFlags::E_INVOKE_METHOD_READ_LOCK, context);
		break;
		case E_WRITE:
			beginOperation(WBEMFlags::E_INVOKE_METHOD_WRITE_LOCK, context);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
LocalCIMOMHandle::~LocalCIMOMHandle()
{
	switch (m_initialLock)
	{
		case E_NONE:
			// do nothing
		break;
		case E_READ:
			endOperation(WBEMFlags::E_INVOKE_METHOD_READ_LOCK, m_context, WBEMFlags::E_SUCCESS);
		break;
		case E_WRITE:
			endOperation(WBEMFlags::E_INVOKE_METHOD_WRITE_LOCK, m_context, WBEMFlags::E_SUCCESS);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
CIMFeatures
LocalCIMOMHandle::getServerFeatures()
{
	CIMFeatures cf;
	cf.protocolVersion = "1.0";
	cf.cimom = "/cimom";
	cf.cimProduct = CIMFeatures::SERVER;
	cf.extURL = "http://www.dmtf.org/cim/mapping/http/v1.0";
	cf.supportedGroups.push_back("basic-read");
	cf.supportedGroups.push_back("basic-write");
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
	cf.supportedGroups.push_back("schema-manipulation");
#endif
	cf.supportedGroups.push_back("instance-manipulation");
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
	cf.supportedGroups.push_back("qualifier-declaration");
#endif
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	cf.supportedGroups.push_back("association-traversal");
#endif
	cf.supportedQueryLanguages.clear();
	cf.supportsBatch = true;
	cf.validation = String();
	return cf;
}
//////////////////////////////////////////////////////////////////////////////
void
LocalCIMOMHandle::exportIndication(const CIMInstance& instance,
	const String& instNS)
{
	m_env->exportIndication(instance, instNS);
}


} // end namespace OW_NAMESPACE

