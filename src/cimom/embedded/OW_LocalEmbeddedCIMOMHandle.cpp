/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2005 Novell, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*	this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*	this list of conditions and the following disclaimer in the documentation
*	and/or other materials provided with the distribution.
*
*  - Neither the name of Vintela, Inc., Novell, Inc., nor the names of its
*	contributors may be used to endorse or promote products derived from this
*	software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc., Novell, Inc., OR THE CONTRIBUTORS
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_LocalEmbeddedCIMOMHandle.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMException.hpp"
#include "OW_Enumeration.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
LocalEmbeddedCIMOMHandle::LocalEmbeddedCIMOMHandle(EmbeddedCIMOMEnvironmentRef env,
	RepositoryIFCRef pRepos, OperationContext& context, ELockingFlag lock)
	: RepositoryCIMOMHandle(pRepos, context, lock)
	, m_env(env)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMFeatures
LocalEmbeddedCIMOMHandle::getServerFeatures()
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
LocalEmbeddedCIMOMHandle::exportIndication(const CIMInstance& instance,
	const String& instNS)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}


} // end namespace OW_NAMESPACE

