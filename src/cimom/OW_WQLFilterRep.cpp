/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_WQLFilterRep.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_Array.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMServer.hpp"
#include "OW_CIMValue.hpp"

///////////////////////////////////////////////////////////////////////////////
OW_WQLFilterRep::OW_WQLFilterRep(const OW_CIMInstance& inst,
	OW_CIMServer* cimServer)
	: OW_RepositoryIFC()
	, m_inst(inst)
	, m_pCIMServer(cimServer)
{
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMOMEnvironmentRef
OW_WQLFilterRep::getEnvironment() const
{
	return m_pCIMServer->getEnvironment();
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_WQLFilterRep::getQualifierType(const OW_CIMObjectPath &/*objPath*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_WQLFilterRep::enumNameSpace(const OW_CIMNameSpace& /*path*/,
	OW_Bool /*deep*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::createNameSpace(const OW_CIMNameSpace &/*ns*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::deleteNameSpace(const OW_CIMNameSpace &/*ns*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::setQualifierType(const OW_CIMObjectPath &/*name*/,
	const OW_CIMQualifierType &/*qt*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_WQLFilterRep::enumQualifierTypes(
	const OW_CIMObjectPath &/*path*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::deleteQualifierType(const OW_CIMObjectPath &/*objPath*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

///////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::createClass(const OW_CIMObjectPath &/*path*/,
	OW_CIMClass &/*cimClass*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_WQLFilterRep::modifyClass(const OW_CIMObjectPath &/*name*/,
	OW_CIMClass &/*cc*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_WQLFilterRep::getClass(const OW_CIMObjectPath& path, OW_Bool b1, OW_Bool b2,
	OW_Bool b3, const OW_StringArray* sap, const OW_ACLInfo& ai)
{
	return m_pCIMServer->getClass(path, b1, b2, b3, sap,
		ai);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_WQLFilterRep::deleteClass(const OW_CIMObjectPath &/*path*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_WQLFilterRep::getInstance(const OW_CIMObjectPath&, OW_Bool, OW_Bool,
	OW_Bool, const OW_StringArray*, const OW_ACLInfo&)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_WQLFilterRep::deleteInstance(const OW_CIMObjectPath &/*cop*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_WQLFilterRep::enumClasses(const OW_CIMObjectPath&, OW_Bool, OW_Bool, OW_Bool,
	OW_Bool, const OW_ACLInfo&)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_WQLFilterRep::enumClassNames(const OW_CIMObjectPath &/*path*/,
	OW_Bool /*deep*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_WQLFilterRep::enumInstances(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	OW_CIMInstanceEnumeration rval;

	if (path.getObjectName().equalsIgnoreCase(m_inst.getClassName()))
	{
		rval.addElement(m_inst.clone(localOnly, includeQualifiers,
			includeClassOrigin, propertyList));
	}
	else if (deep)
	{
		OW_String superClassName = m_inst.getClassName();
		OW_CIMClass cc = m_pCIMServer->getClass(OW_CIMObjectPath(
            superClassName, path.getNameSpace()), false, true, true, NULL,
			aclInfo);

		if (cc)
		{
			superClassName = cc.getSuperClass();
			while (superClassName.length() > 0)
			{
				if (superClassName.equalsIgnoreCase(path.getObjectName()))
				{
					rval.addElement(m_inst.clone(localOnly, includeQualifiers,
						includeClassOrigin, propertyList));
					break;
				}
				
				cc = m_pCIMServer->getClass(OW_CIMObjectPath(
					superClassName, path.getNameSpace()), false, true, true,
					NULL, aclInfo);

				if (cc)
				{
					superClassName = cc.getSuperClass();
				}
				else
				{
					break;
				}
			}
		}
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_WQLFilterRep::enumInstanceNames(
	const OW_CIMObjectPath &/*path*/, OW_Bool /*deep*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_WQLFilterRep::createInstance(const OW_CIMObjectPath &/*cop*/,
	OW_CIMInstance &/*ci*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_WQLFilterRep::modifyInstance(const OW_CIMObjectPath &/*cop*/,
	OW_CIMInstance &/*ci*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_WQLFilterRep::associators(const OW_CIMObjectPath &/*path*/,
	const OW_String &/*assocClass*/, const OW_String &/*resultClass*/,
	const OW_String &/*role*/, const OW_String &/*resultRole*/,
	OW_Bool /*includeQualifiers*/, OW_Bool /*includeClassOrigin*/,
	const OW_StringArray* /*propertyList*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_WQLFilterRep::setProperty(const OW_CIMObjectPath &/*name*/,
	const OW_String& /*propertyName*/, const OW_CIMValue &/*cv*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_WQLFilterRep::referenceNames(
	const OW_CIMObjectPath &/*path*/, const OW_String &/*resultClass*/,
	const OW_String &/*role*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_WQLFilterRep::getProperty(const OW_CIMObjectPath &/*name*/,
	const OW_String &/*propertyName*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_WQLFilterRep::invokeMethod(const OW_CIMObjectPath &/*name*/,
	const OW_String &/*methodName*/, const OW_CIMValueArray &/*inParams*/,
	OW_CIMValueArray &/*outParams*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_WQLFilterRep::associatorNames(
	const OW_CIMObjectPath &/*path*/, const OW_String &/*assocClass*/,
	const OW_String &/*resultClass*/, const OW_String &/*role*/,
	const OW_String &/*resultRole*/, const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration OW_WQLFilterRep::references(
	const OW_CIMObjectPath &/*path*/, const OW_String &/*resultClass*/,
	const OW_String &/*role*/, OW_Bool /*includeQualifiers*/,
	OW_Bool /*includeClassOrigin*/, const OW_StringArray*  /*propertyList*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_WQLFilterRep::execQuery(const OW_CIMNameSpace& /*ns*/,
	const OW_String &/*query*/, const OW_String &/*queryLanguage*/,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_THROWCIMMSG(OW_CIMException::INVALID_QUERY, "Only EnumInstances() "
		"is supported in the WQLCIMOMHandle.");
}

//////////////////////////////////////////////////////////////////////////////
OW_ReadLock
OW_WQLFilterRep::getReadLock()
{
	return m_pCIMServer->getReadLock();
}

//////////////////////////////////////////////////////////////////////////////
OW_WriteLock
OW_WQLFilterRep::getWriteLock()
{
	return m_pCIMServer->getWriteLock();
}


