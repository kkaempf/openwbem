/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_NameSpaceProvider.hpp"
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMUrl.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_UserInfo.hpp"

using namespace OW_WBEMFlags;

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class namespaceFilterer : public OW_StringResultHandlerIFC
	{
	public:
		namespaceFilterer(const OW_String& ns_, bool deep_, OW_StringResultHandlerIFC& result_)
			: ns(ns_.tokenize("/"))
			, deep(deep_)
			, result(result_)
		{}

		void doHandle(const OW_String& s)
		{
			OW_StringArray split(s.tokenize("/"));
			if (split.size() <= ns.size())
			{
				// it's a parent or the same namespace, so ignore it.
				return;
			}
			if (!deep && split.size() > ns.size() + 1)
			{
				// it's more than one deep, so ignore it.
				return;
			}
			for (size_t i = 0; i < ns.size(); ++i)
			{
				if (ns[i] != split[i])
				{
					// it's not under the requested namespace so return.
					return;
				}
			}
			// match, pass it on.
			result.handle(s);
		}

	private:
		OW_StringArray ns;
		bool deep;
		OW_StringResultHandlerIFC& result;
	};
}

//////////////////////////////////////////////////////////////////////////////
OW_NameSpaceProvider::~OW_NameSpaceProvider()
{
}

namespace
{
	class StringArrayBuilder : public OW_StringResultHandlerIFC
	{
	public:
		StringArrayBuilder(OW_StringArray& a) : m_a(a) {}
	protected:
		virtual void doHandle(const OW_String &s)
		{
			m_a.push_back(s);
		}
	private:
		OW_StringArray& m_a;
	};
	
	OW_StringArray enumNameSpaceE(const OW_ProviderEnvironmentIFCRef& env, const OW_String& ns)
	{
		OW_RepositoryIFCRef rep = env->getRepository();
		OW_StringArray rval;
		StringArrayBuilder arrayBuilder(rval);
		namespaceFilterer handler(ns, true, arrayBuilder);
		rep->enumNameSpace(handler, OW_UserInfo(env->getUserName()));
		return rval;
	}

	void enumNameSpace(const OW_ProviderEnvironmentIFCRef& env, const OW_String& ns, OW_StringResultHandlerIFC& result, bool deep)
	{
		OW_RepositoryIFCRef rep = env->getRepository();
		namespaceFilterer handler(ns, deep, result);
		rep->enumNameSpace(handler, OW_UserInfo(env->getUserName()));
	}

}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
{
	OW_CIMPropertyArray pra = cop.getKeys();
	if(pra.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
			"root namespace cannot be deleted");
	}

	OW_CIMProperty nameProp = cop.getKey(OW_CIMProperty::NAME_PROPERTY);
	if (!nameProp)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Name property not found");
	}
	
	OW_CIMValue cv = nameProp.getValue();
	if(!cv)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Name property doesn't have a value");
	}
	OW_String nsName;
	cv.get(nsName);

	if(nsName.empty())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Name property contains an empty value");
	}

	OW_String newns = ns + "/" + nsName;

	// deleteNameSpace doesn't automatically delete subnamespaces, so we need to do it.
	OW_StringArray nstodel = enumNameSpaceE(env, newns);
	OW_UserInfo acl(env->getUserName());
	for (size_t i = 0; i < nstodel.size(); ++i)
	{
		env->getRepository()->deleteNameSpace(nstodel[i], acl);
	}

	env->getRepository()->deleteNameSpace(newns, acl);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

namespace
{
	class CIMInstanceToObjectPath : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceToObjectPath(OW_CIMObjectPathResultHandlerIFC& h,
			const OW_String& ns_,
			const OW_String& className_) : m_h(h), cop(className_, ns_) {}
	protected:
		virtual void doHandle(const OW_CIMInstance &ci)
		{
			cop.setKeys(ci.getKeyValuePairs());
			m_h.handle(cop);
		}
	private:
		OW_CIMObjectPathResultHandlerIFC& m_h;
		OW_CIMObjectPath cop;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_CIMClass& cimClass)
{
	CIMInstanceToObjectPath handler(result, ns, className);
	enumInstances(env, ns, className, handler, E_NOT_LOCAL_ONLY, E_SHALLOW, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, cimClass, cimClass);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class NameSpaceEnumBuilder : public OW_StringResultHandlerIFC
	{
	public:
		NameSpaceEnumBuilder(OW_CIMInstanceResultHandlerIFC& handler_,
			const OW_CIMClass& cimClass_)
		: handler(handler_)
		, cimClass(cimClass_)
		{}
	protected:
		virtual void doHandle(const OW_String &s)
		{
			OW_String nameSpaceName = s;
			size_t ndx = nameSpaceName.lastIndexOf('/');
			if(ndx != OW_String::npos)
			{
				nameSpaceName = nameSpaceName.substring(ndx+1);
			}

			OW_CIMInstance ci = cimClass.newInstance();
			ci.setProperty("Name", OW_CIMValue(nameSpaceName));
			handler.handle(ci);
		}
	private:
		OW_CIMInstanceResultHandlerIFC& handler;
		const OW_CIMClass& cimClass;
	};

	class CIMInstanceEnumBuilder : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceEnumBuilder(OW_CIMInstanceEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandle(const OW_CIMInstance &ci)
		{
			m_e.addElement(ci);
		}
	private:
		OW_CIMInstanceEnumeration& m_e;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass,
		const OW_CIMClass& cimClass)
{
	(void)className; (void)localOnly; (void)deep; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList; (void)requestedClass;
	NameSpaceEnumBuilder handler(result, cimClass);
	enumNameSpace(env, ns, handler, false);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_NameSpaceProvider::getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
		OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OW_StringArray* propertyList, 
		const OW_CIMClass& cimClass)
{
	(void)localOnly; (void)includeQualifiers; (void)includeClassOrigin; (void)propertyList;
	OW_CIMProperty cp = instanceName.getKey(OW_CIMProperty::NAME_PROPERTY);
	OW_CIMValue nsVal(OW_CIMNULL);
	if (cp)
	{
		nsVal = cp.getValue();
	}

	if (nsVal && nsVal.getType() == OW_CIMDataType::STRING)
	{
		OW_CIMInstanceEnumeration cie;
		CIMInstanceEnumBuilder handler(cie);
		enumInstances(env, ns, instanceName.getObjectName(), handler,
			E_NOT_LOCAL_ONLY, E_SHALLOW, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, cimClass,
			cimClass);
		
		while (cie.hasMoreElements())
		{
			OW_CIMInstance ci = cie.nextElement();
			if (ci)
			{
				OW_CIMProperty cp = ci.getProperty(OW_CIMProperty::NAME_PROPERTY);
				if (cp)
				{
					OW_CIMValue v = cp.getValue();
					if (v && v.getType() == OW_CIMDataType::STRING)
					{
						OW_String vval;
						v.get(vval);
						OW_String nsValStr;
						nsVal.get(nsValStr);
						if (vval == nsValStr)
						{
							return ci;
						}
					}
				}
			}
		}
	}

	OW_THROWCIM(OW_CIMException::NOT_FOUND);
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_NameSpaceProvider::createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance)
{
	OW_CIMProperty cp = cimInstance.getProperty(OW_CIMProperty::NAME_PROPERTY);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
			"Instance \"Name\" property is not set");
	}

	OW_CIMValue cv = cp.getValue();
	OW_String newName;
	cv.get(newName);

	newName = newName.substring(newName.indexOf('=') + 1);
	OW_String newNameSpace = ns;
	newNameSpace += "/";
	newNameSpace += newName;

	env->getLogger()->logDebug(format("OW_NameSpaceProvider::createInstance calling"
			" createNameSpace with %1", newNameSpace));

	env->getRepository()->createNameSpace(newNameSpace, OW_UserInfo(env->getUserName()));

	return OW_CIMObjectPath(ns, cimInstance);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		const OW_CIMInstance& previousInstance,
		OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_CIMClass& theClass)
{
	(void)env; (void)ns; (void)modifiedInstance; (void)previousInstance; (void)includeQualifiers; (void)propertyList; (void)theClass;
	OW_THROWCIMMSG(OW_CIMException::FAILED, "Modifying a __Namespace instance is not allowed");
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::initialize(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug("OW_NameSpaceProvider initialize called");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NameSpaceProvider::getInstanceProviderInfo(OW_InstanceProviderInfo& info)
{
	info.addInstrumentedClass("__Namespace");
}

OW_PROVIDERFACTORY(OW_NameSpaceProvider, owprovinstOW_NameSpace);


