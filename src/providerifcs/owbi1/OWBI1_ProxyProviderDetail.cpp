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

#include "OWBI1_config.h"
#include "OWBI1_ProxyProviderDetail.hpp"

#include "OW_SharedLibraryReference.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_BinarySerialization.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_WQLSelectStatement.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OWBI1_ResultHandlerIFC.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OWBI1_CIMObjectPath.hpp"
#include "OW_CIMClass.hpp"
#include "OWBI1_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMPropertyList.hpp"
#include "OWBI1_ProviderEnvironmentIFC.hpp"
#include "OW_CIMParamValue.hpp"
#include "OWBI1_CIMParamValue.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OWBI1
{

namespace detail
{

OWBI1::CIMInstance convertCIMInstance(const OpenWBEM::CIMInstance& inst)
{
	return OWBI1::CIMInstance(OWBI1::CIMInstanceRep(inst));
}

OpenWBEM::CIMInstance convertCIMInstance(const OWBI1::CIMInstance& inst)
{
	return inst.getRep()->inst;
}

OWBI1::CIMInstanceArray convertCIMInstances(const OpenWBEM::CIMInstanceArray& inst)
{
	return OWBI1::CIMInstanceArray();
}

OpenWBEM::CIMInstanceArray convertCIMInstances(const OWBI1::CIMInstanceArray& inst)
{
	return OpenWBEM::CIMInstanceArray();
}

OWBI1::CIMObjectPath convertCIMObjectPath(const OpenWBEM::CIMObjectPath& inst)
{
	return OWBI1::CIMObjectPath();
}

OpenWBEM::CIMObjectPath convertCIMObjectPath(const OWBI1::CIMObjectPath& inst)
{
	return OpenWBEM::CIMObjectPath();
}

OWBI1::CIMClass convertCIMClass(const OpenWBEM::CIMClass& inst)
{
	return OWBI1::CIMClass();
}

OpenWBEM::CIMClass convertCIMClass(const OWBI1::CIMClass& inst)
{
	return OpenWBEM::CIMClass();
}

OWBI1::CIMParamValueArray convertCIMParamValues(const OpenWBEM::CIMParamValueArray& inst)
{
	return OWBI1::CIMParamValueArray();
}

OpenWBEM::CIMParamValueArray convertCIMParamValues(const OWBI1::CIMParamValueArray& inst)
{
	return OpenWBEM::CIMParamValueArray();
}

OWBI1::CIMValue convertCIMValue(const OpenWBEM::CIMValue& inst)
{
	return OWBI1::CIMValue(CIMNULL);
}

OpenWBEM::CIMValue convertCIMValue(const OWBI1::CIMValue& inst)
{
	return OpenWBEM::CIMValue(OpenWBEM::CIMNULL);
}

OWBI1::ProviderEnvironmentIFCRef createProvEnvWrapper(const OpenWBEM::ProviderEnvironmentIFCRef& env)
{
	return OWBI1::ProviderEnvironmentIFCRef();
}

OWBI1::WQLSelectStatement convertWQLSelectStatement(const OpenWBEM::WQLSelectStatement& stmt)
{
	return OWBI1::WQLSelectStatement();
}

OWBI1::StringArray convertStringArray(const OpenWBEM::StringArray& in)
{
	OpenWBEM::TempFileStream tfs;
	OpenWBEM::BinarySerialization::writeArray(tfs, in);
	OWBI1::StringArray out;
	OpenWBEM::BinarySerialization::readArray(tfs, out);
	return out;
}

OpenWBEM::StringArray convertStringArray(const OWBI1::StringArray& in)
{
	OpenWBEM::TempFileStream tfs;
	OpenWBEM::BinarySerialization::writeArray(tfs, in);
	OpenWBEM::StringArray out;
	OpenWBEM::BinarySerialization::readArray(tfs, out);
	return out;
}

OWBI1::WBEMFlags::EIncludeQualifiersFlag convertWBEMFlag(OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers)
{
	return OWBI1::WBEMFlags::EIncludeQualifiersFlag();
}
OWBI1::WBEMFlags::EIncludeClassOriginFlag convertWBEMFlag(OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin)
{
	return OWBI1::WBEMFlags::EIncludeClassOriginFlag();
}

OWBI1::WBEMFlags::ELocalOnlyFlag convertWBEMFlag(OpenWBEM::WBEMFlags::ELocalOnlyFlag includeClassOrigin)
{
	return OWBI1::WBEMFlags::ELocalOnlyFlag();
}
OWBI1::WBEMFlags::EDeepFlag convertWBEMFlag(OpenWBEM::WBEMFlags::EDeepFlag includeClassOrigin)
{
	return OWBI1::WBEMFlags::EDeepFlag();
}

OWBI1::CIMPropertyList convertPropertyList(const OpenWBEM::StringArray* propertyList)
{
	return OWBI1::CIMPropertyList();
}

CIMInstanceResultHandlerWrapper::CIMInstanceResultHandlerWrapper(OpenWBEM::CIMInstanceResultHandlerIFC& wrapped)
: m_wrapped(wrapped)
{
}

void CIMInstanceResultHandlerWrapper::doHandle(const CIMInstance& i)
{
	// TODO: handle includeQualifiers & includeClassOrigin
	m_wrapped.handle(convertCIMInstance(i));
}

CIMObjectPathResultHandlerWrapper::CIMObjectPathResultHandlerWrapper(OpenWBEM::CIMObjectPathResultHandlerIFC& wrapped)
: m_wrapped(wrapped)
{
}

void CIMObjectPathResultHandlerWrapper::doHandle(const CIMObjectPath& i)
{
	m_wrapped.handle(convertCIMObjectPath(i));
}
	

} // end namespace ProxyProviderDetail
} // end namespace OWBI1



