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

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.__Namespace");
}

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class namespaceFilterer : public StringResultHandlerIFC
	{
	public:
		namespaceFilterer(const String& ns_, bool deep_, StringResultHandlerIFC& result_)
			: ns(ns_.tokenize("/"))
			, deep(deep_)
			, result(result_)
		{}
		void doHandle(const String& s)
		{
			StringArray split(s.tokenize("/"));
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
		StringArray ns;
		bool deep;
		StringResultHandlerIFC& result;
	};
}
//////////////////////////////////////////////////////////////////////////////
NameSpaceProvider::~NameSpaceProvider()
{
}
namespace
{
	class StringArrayBuilder : public StringResultHandlerIFC
	{
	public:
		StringArrayBuilder(StringArray& a) : m_a(a) {}
	protected:
		virtual void doHandle(const String &s)
		{
			m_a.push_back(s);
		}
	private:
		StringArray& m_a;
	};
	
	StringArray enumNameSpaceE(const ProviderEnvironmentIFCRef& env, const String& ns)
	{
		RepositoryIFCRef rep = env->getRepository();
		StringArray rval;
		StringArrayBuilder arrayBuilder(rval);
		namespaceFilterer handler(ns, true, arrayBuilder);
		rep->enumNameSpace(handler, env->getOperationContext());
		return rval;
	}
	void enumNameSpace(const ProviderEnvironmentIFCRef& env, const String& ns, StringResultHandlerIFC& result, bool deep)
	{
		RepositoryIFCRef rep = env->getRepository();
		namespaceFilterer handler(ns, deep, result);
		rep->enumNameSpace(handler, env->getOperationContext());
	}
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
void
NameSpaceProvider::deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
{
	CIMPropertyArray pra = cop.getKeys();
	if (pra.size() == 0)
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
			"root namespace cannot be deleted");
	}
	CIMProperty nameProp = cop.getKey(CIMProperty::NAME_PROPERTY);
	if (!nameProp)
	{
		OW_THROWCIMMSG(CIMException::FAILED,
			"Name property not found");
	}
	
	CIMValue cv = nameProp.getValue();
	if (!cv)
	{
		OW_THROWCIMMSG(CIMException::FAILED,
			"Name property doesn't have a value");
	}
	String nsName;
	cv.get(nsName);
	if (nsName.empty())
	{
		OW_THROWCIMMSG(CIMException::FAILED,
			"Name property contains an empty value");
	}
	String newns = ns + "/" + nsName;
	// deleteNameSpace doesn't automatically delete subnamespaces, so we need to do it.
	StringArray nstodel = enumNameSpaceE(env, newns);
	for (size_t i = 0; i < nstodel.size(); ++i)
	{
		env->getRepository()->deleteNameSpace(nstodel[i], env->getOperationContext());
	}
	env->getRepository()->deleteNameSpace(newns, env->getOperationContext());
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
namespace
{
	class CIMInstanceToObjectPath : public CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceToObjectPath(CIMObjectPathResultHandlerIFC& h,
			const String& ns_,
			const String& className_) : m_h(h), cop(className_, ns_) {}
	protected:
		virtual void doHandle(const CIMInstance &ci)
		{
			cop.setKeys(ci.getKeyValuePairs());
			m_h.handle(cop);
		}
	private:
		CIMObjectPathResultHandlerIFC& m_h;
		CIMObjectPath cop;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
NameSpaceProvider::enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass)
{
	CIMInstanceToObjectPath handler(result, ns, className);
	enumInstances(env, ns, className, handler, E_NOT_LOCAL_ONLY, E_SHALLOW, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, cimClass, cimClass);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class NameSpaceEnumBuilder : public StringResultHandlerIFC
	{
	public:
		NameSpaceEnumBuilder(CIMInstanceResultHandlerIFC& handler_,
			const CIMClass& cimClass_)
		: handler(handler_)
		, cimClass(cimClass_)
		{}
	protected:
		virtual void doHandle(const String &s)
		{
			String nameSpaceName = s;
			size_t ndx = nameSpaceName.lastIndexOf('/');
			if (ndx != String::npos)
			{
				nameSpaceName = nameSpaceName.substring(ndx+1);
			}
			CIMInstance ci = cimClass.newInstance();
			ci.setProperty("Name", CIMValue(nameSpaceName));
			handler.handle(ci);
		}
	private:
		CIMInstanceResultHandlerIFC& handler;
		const CIMClass& cimClass;
	};
	class CIMInstanceEnumBuilder : public CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceEnumBuilder(CIMInstanceEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandle(const CIMInstance &ci)
		{
			m_e.addElement(ci);
		}
	private:
		CIMInstanceEnumeration& m_e;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
NameSpaceProvider::enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly,
		EDeepFlag deep,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass)
{
	NameSpaceEnumBuilder handler(result, cimClass);
	enumNameSpace(env, ns, handler, false);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
NameSpaceProvider::getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass)
{
	CIMProperty cp = instanceName.getKey(CIMProperty::NAME_PROPERTY);
	CIMValue nsVal(CIMNULL);
	if (cp)
	{
		nsVal = cp.getValue();
	}
	if (nsVal && nsVal.getType() == CIMDataType::STRING)
	{
		CIMInstanceEnumeration cie;
		CIMInstanceEnumBuilder handler(cie);
		enumInstances(env, ns, instanceName.getClassName(), handler,
			E_NOT_LOCAL_ONLY, E_SHALLOW, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, cimClass,
			cimClass);
		
		while (cie.hasMoreElements())
		{
			CIMInstance ci = cie.nextElement();
			if (ci)
			{
				CIMProperty cp = ci.getProperty(CIMProperty::NAME_PROPERTY);
				if (cp)
				{
					CIMValue v = cp.getValue();
					if (v && v.getType() == CIMDataType::STRING)
					{
						String vval;
						v.get(vval);
						String nsValStr;
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
	OW_THROWCIM(CIMException::NOT_FOUND);
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
NameSpaceProvider::createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance)
{
	CIMProperty cp = cimInstance.getProperty(CIMProperty::NAME_PROPERTY);
	if (!cp)
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
			"Instance \"Name\" property is not set");
	}
	CIMValue cv = cp.getValue();
	String newName;
	cv.get(newName);
	newName = newName.substring(newName.indexOf('=') + 1);
	String newNameSpace = ns;
	newNameSpace += "/";
	newNameSpace += newName;
	env->getLogger(COMPONENT_NAME)->logDebug(Format("NameSpaceProvider::createInstance calling"
			" createNameSpace with %1", newNameSpace));
	env->getRepository()->createNameSpace(newNameSpace, env->getOperationContext());
	return CIMObjectPath(ns, cimInstance);
}
//////////////////////////////////////////////////////////////////////////////
void
NameSpaceProvider::modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
{
	OW_THROWCIMMSG(CIMException::FAILED, "Modifying a __Namespace instance is not allowed");
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
NameSpaceProvider::initialize(const ProviderEnvironmentIFCRef& env)
{
	env->getLogger(COMPONENT_NAME)->logDebug("NameSpaceProvider initialize called");
}
//////////////////////////////////////////////////////////////////////////////
void
NameSpaceProvider::getInstanceProviderInfo(InstanceProviderInfo& info)
{
	info.addInstrumentedClass("__Namespace");
}

} // end namespace OpenWBEM

OW_PROVIDERFACTORY(OpenWBEM::NameSpaceProvider, owprovinstOW_NameSpace);

