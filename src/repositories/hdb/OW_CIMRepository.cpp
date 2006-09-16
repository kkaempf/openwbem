/*******************************************************************************
* Copyright (C) 2002-2004 Vintela, Inc. All rights reserved.
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
#include "OW_CIMRepository.hpp"
#include "OW_FileSystem.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Map.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_OperationContext.hpp"
#include "OW_ServiceIFCNames.hpp"

namespace OW_NAMESPACE
{

namespace
{
	const String COMPONENT_NAME("ow.repository.hdb");
}

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
CIMRepository::CIMRepository()
	: m_logger(COMPONENT_NAME)
	, m_checkReferentialIntegrity(false)
{
}
//////////////////////////////////////////////////////////////////////////////
CIMRepository::~CIMRepository()
{
	try
	{
		close();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::open(const String& path)
{
	if (m_nStore.isOpen())
	{
		close();
	}
	FileSystem::makeDirectory(path);
	if (!FileSystem::exists(path))
	{
		String msg("failed to create directory: " );
		msg += path;
		OW_THROW_ERRNO_MSG(IOException, msg.c_str());
	}
	else
	{
		if (!FileSystem::canWrite(path))
		{
			String msg("don't have write access to directory: ");
			msg += path;
			OW_THROW(IOException, msg.c_str());
		}
	}
	String fname = path;
	if (!fname.endsWith(String(OW_FILENAME_SEPARATOR)))
	{
		fname += OW_FILENAME_SEPARATOR;
	}
	
	m_nStore.open(fname + NS_REPOS_NAME);
	m_iStore.open(fname + INST_REPOS_NAME);
	m_mStore.open(fname + META_REPOS_NAME);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	m_classAssocDb.open(fname + CLASS_ASSOC_REPOS_NAME);
	m_instAssocDb.open(fname + INST_ASSOC_REPOS_NAME);
#endif
}

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::close()
{
	m_nStore.close();
	m_iStore.close();
	m_mStore.close();
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	m_classAssocDb.close();
	m_instAssocDb.close();
#endif
}

//////////////////////////////////////////////////////////////////////////////
String
CIMRepository::getName() const
{
	return ServiceIFCNames::CIMRepository;
}

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::init(const ServiceEnvironmentIFCRef& env)
{
	m_nStore.init(env);
	m_iStore.init(env);
	m_mStore.init(env);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	m_classAssocDb.init(env);
	m_instAssocDb.init(env);
#endif
	m_env = env;
	if (m_env->getConfigItem(ConfigOpts::CHECK_REFERENTIAL_INTEGRITY_opt,
		OW_DEFAULT_CHECK_REFERENTIAL_INTEGRITY).equalsIgnoreCase("true"))
	{
		m_checkReferentialIntegrity = true;
	}

	this->open(m_env->getConfigItem(ConfigOpts::DATADIR_opt, OW_DEFAULT_DATADIR));
}

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::shutdown()
{
	close();
	m_env = 0;
}

#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::createNameSpace(const String& ns,
	OperationContext&)
{
	if (ns.empty())
	{
		OW_THROWCIM(CIMException::INVALID_PARAMETER);
	}

	// namespaces can't contain :
	const char NS_SEPARATOR_C(':');
	if (ns.indexOf(NS_SEPARATOR_C) != String::npos)
	{
		OW_THROWCIMMSG(CIMException::FAILED, Format("Invalid namespace (%1). %2 is not allowed", ns, NS_SEPARATOR_C).c_str());
	}

	if (m_nStore.createNameSpace(ns) == -1)
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS,
			ns.c_str());
	}
	// TODO: Make this exception safe.
	if (m_iStore.createNameSpace(ns) == -1)
	{
		m_nStore.deleteNameSpace(ns);
		OW_THROWCIMMSG(CIMException::FAILED, Format("Failed to create namespace %1", ns).c_str());
	}

	if (m_mStore.createNameSpace(ns) == -1)
	{
		m_nStore.deleteNameSpace(ns);
		m_iStore.deleteNameSpace(ns);
		OW_THROWCIMMSG(CIMException::FAILED, Format("Failed to create namespace %1", ns).c_str());
	}

	OW_LOG_DEBUG(m_logger, Format("CIMRepository created namespace: %1", ns));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::deleteNameSpace(const String& ns,
	OperationContext&)
{
	if (ns.empty())
	{
		OW_THROWCIM(CIMException::INVALID_PARAMETER);
	}
	// TODO: Make this exception safe.
	m_nStore.deleteNameSpace(ns);
	m_iStore.deleteNameSpace(ns);
	m_mStore.deleteNameSpace(ns);
	
	OW_LOG_DEBUG(m_logger, Format("CIMRepository deleted namespace: %1", ns));
}
#endif
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext&)
{
	// TODO: Move this into m_nStore
	HDBHandleLock hdl(&m_nStore, m_nStore.getHandle());
	HDBNode nsNode = hdl->getFirstRoot();
	//HDBNode nsNode = m_nStore.getNameSpaceNode(hdl, nsName);
	//if (!nsNode)
	//{
	//	OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, nsName.c_str());
	//}
	//nsNode = hdl->getFirstChild(nsNode);
	while (nsNode)
	{
		result.handle(nsNode.getKey());
		nsNode = hdl->getNextSibling(nsNode);
	}
	OW_LOG_DEBUG(m_logger, "CIMRepository enumerated namespaces");
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMRepository::getQualifierType(const String& ns,
	const String& qualifierName,
	OperationContext&)
{
	OW_LOG_DEBUG(m_logger, Format("CIMRepository getting qualifier type: %1",
		CIMObjectPath(qualifierName,ns).toString()));
	return m_mStore.getQualifierType(ns, qualifierName);
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result,
	OperationContext&)
{
	m_mStore.enumQualifierTypes(ns, result);
	OW_LOG_DEBUG(m_logger, Format("CIMRepository enumerated qualifiers in namespace: %1", ns));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext&)
{
	// TODO: What happens if the qualifier is being used???
	if (!m_mStore.deleteQualifierType(ns, qualName))
	{
		if (m_nStore.nameSpaceExists(ns))
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND,
				String(ns + "/" + qualName).c_str());
		}
		else
		{
			OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
				String(ns + "/" + qualName).c_str());
		}
	}
	
	OW_LOG_DEBUG(m_logger, Format("CIMRepository deleted qualifier type: %1 in namespace: %2", qualName, ns));
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::setQualifierType(
	const String& ns,
	const CIMQualifierType& qt, OperationContext&)
{
	m_mStore.setQualifierType(ns, qt);
	OW_LOG_DEBUG(m_logger, Format("CIMRepository set qualifier type: %1 in "
		"namespace: %2", qt.toString(), ns));
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository::getClass(
	const String& ns, const String& className,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
	OperationContext&)
{
	try
	{
		CIMClass theClass(CIMNULL);
		CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className,
			localOnly, includeQualifiers, includeClassOrigin, propertyList,
			theClass);
		checkGetClassRvalAndThrow(rval, ns, className);
		OW_LOG_DEBUG(m_logger, Format("CIMRepository got class: %1 from "
			"namespace: %2", theClass.getName(), ns));
		return theClass;
	}
	catch(HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch(IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository::_getClass(const String& ns, const CIMName& className)
{
	CIMClass theClass(CIMNULL);
	CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, theClass);
	checkGetClassRvalAndThrow(rval, ns, className);
	return theClass;
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository::_instGetClass(const String& ns, const CIMName& className)
{
	CIMClass theClass(CIMNULL);
	CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, theClass);
	checkGetClassRvalAndThrowInst(rval, ns, className);
	return theClass;
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassDeleter : public CIMClassResultHandlerIFC
	{
	public:
		CIMClassDeleter(MetaRepository& mr, const String& ns_,
			InstanceRepository& mi
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			, AssocDb& m_assocDb_
#endif
			)
		: m_mStore(mr)
		, ns(ns_)
		, m_iStore(mi)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		, m_assocDb(m_assocDb_)
#endif
		{}
	protected:
		virtual void doHandle(const CIMClass &c)
		{
			CIMName cname = c.getName();
			if (!m_mStore.deleteClass(ns, cname.toString()))
			{
				OW_THROWCIM(CIMException::NOT_FOUND);
			}
			// TODO: this doesn't work quite right.  what about associations to
			// the instances we delete?  If we fix deleteInstance to also delete
			// associations, then we could just call enumInstances and then
			// deleteInstance for all instances.
			// delete any instances of the class
			m_iStore.deleteClass(ns, cname.toString());
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			// remove class from association index
			if (c.isAssociation())
			{
				AssocDbHandle hdl = m_assocDb.getHandle();
				hdl.deleteEntries(ns,c);
			}
#endif
		}
	private:
		MetaRepository& m_mStore;
		const String& ns;
		InstanceRepository& m_iStore;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssocDb& m_assocDb;
#endif
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository::deleteClass(const String& ns, const String& className,
	OperationContext& acl)
{
	try
	{
		CIMClass cc = _getClass(ns, className);
		OW_ASSERT(cc);
		// TODO: this doesn't work quite right.  what about associations to
		// the instances we delete?
		// should this operation be atomic?  If something fails, how can we
		// undo so as to not leave things in a weird state?
		// We need to also delete the associations to the instances we delete.
		// To make this atomic, we need to introduce transactions.  This would
		// mean upgrading to a new version of libdb, or else using an SQL
		// database that supports transactions.  Either way, a lot of work :-(
		// delete the class and any subclasses
		CIMClassDeleter ccd(m_mStore, ns, m_iStore
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			, m_classAssocDb
#endif
			);
		this->enumClasses(ns, className, ccd,
			E_DEEP, E_LOCAL_ONLY,
			E_EXCLUDE_QUALIFIERS,
			E_EXCLUDE_CLASS_ORIGIN,
			acl);
		ccd.handle(cc);
		OW_LOG_DEBUG(m_logger, Format("CIMRepository deleted class: %1 in "
			"namespace: %2", className, ns));
		return cc;
	}
	catch(IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch(HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::createClass(const String& ns, const CIMClass& cimClass_,
	OperationContext&)
{
	try
	{
		// m_mStore.createClass modifies cimClass to be consistent with base
		// classes, etc.
		CIMClass cimClass(cimClass_);

		// validate that any reference properties are valid classes
		CIMPropertyArray props(cimClass.getAllProperties());
		for (size_t i = 0; i < props.size(); ++i)
		{
			CIMProperty& prop(props[i]);
			CIMDataType type(prop.getDataType());
			if (type.isReferenceType())
			{
				CIMName refClassName(type.getRefClassName());
				try
				{
					_getClass(ns, refClassName);
				}
				catch (CIMException& e)
				{
					// if we have a NOT_FOUND, change it into a INVALID_PARAMETER
					if (e.getErrNo() == CIMException::NOT_FOUND)
					{
						OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							Format("Class %1 referenced by reference property %2 doesn't exist in namespace %3",
								refClassName, prop.getName(), ns).c_str());
					}
					throw;
				}
			}
		}

		m_mStore.createClass(ns, cimClass);
		m_iStore.createClass(ns, cimClass);
		// we need to re-get the class, so that it will be consistent.  currently
		// cimClass only contains "unique" items that are added in the child class.
		cimClass = _getClass(ns, cimClass.getName());
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if (cimClass.isAssociation())
		{
			AssocDbHandle hdl = m_classAssocDb.getHandle();
			hdl.addEntries(ns,cimClass);
		}
#endif
		OW_LOG_DEBUG(m_logger, Format("Created class: %1:%2", ns, cimClass.toMOF()));
	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository::modifyClass(
	const String& ns,
	const CIMClass& cc,
	OperationContext&)
{
	OW_ASSERT(cc);
	try
	{
		CIMClass origClass = _getClass(ns, cc.getName());
		// TODO: this needs to update the subclasses of the modified class.
		//			If that's not possible, then we need to throw a
		//			CLASS_HAS_CHILDREN CIMException.
		// TODO: Need to update the instances of the class and any subclasses.
		//			If that's not possible, then we need to throw a
		//			CLASS_HAS_INSTANCES CIMException.
		m_mStore.modifyClass(ns, cc);
		OW_ASSERT(origClass);
		OW_LOG_DEBUG(m_logger, Format("Modified class: %1:%2 from %3 to %4", ns,
			cc.getName(), origClass.toMOF(), cc.toMOF()));
		return origClass;
	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, OperationContext&)
{
	try
	{
		m_mStore.enumClass(ns, className,
			result, deep,
			localOnly, includeQualifiers, includeClassOrigin);
		OW_LOG_DEBUG(m_logger, Format("CIMRepository enumerated classes: %1:%2", ns,
			className));
	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, OperationContext&)
{
	try
	{
		m_mStore.enumClassNames(ns, className, result, deep);
		OW_LOG_DEBUG(m_logger, Format("CIMRepository enumerated class names: %1:%2", ns,
			className));
	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace {
class CIMNameArrayBuilder : public StringResultHandlerIFC
{
public:
	CIMNameArrayBuilder(CIMNameArray& result)
		: m_result(result)
	{}
	void doHandle(const String& name)
	{
		m_result.push_back(name);
	}
private:
	CIMNameArray& m_result;
};
// utility function
CIMNameArray getClassChildren(MetaRepository& rep, const String& ns, const CIMName& clsName)
{
	CIMNameArray result;
	CIMNameArrayBuilder handler(result);
	rep.enumClassNames(ns, clsName.toString(), handler, E_DEEP);
	return result;
}

//////////////////////////////////////////////////////////////////////////////
class InstNameEnumerator : public CIMClassResultHandlerIFC
{
public:
	InstNameEnumerator(
		const String& ns_,
		CIMObjectPathResultHandlerIFC& result_,
		Logger& lgr_,
		InstanceRepository& iStore)
		: ns(ns_)
		, result(result_)
		, m_lgr(lgr_)
		, m_iStore(iStore)
	{}
protected:
	virtual void doHandle(const CIMClass &cc)
	{
		OW_LOG_DEBUG(m_lgr, Format("CIMServer InstNameEnumerator enumerated derived instance names: %1:%2", ns,
			cc.getName()));
		m_iStore.getInstanceNames(ns, cc, result);
	}
private:
	String ns;
	CIMObjectPathResultHandlerIFC& result;
	Logger& m_lgr;
	InstanceRepository& m_iStore;
};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep,
	OperationContext&)
{
	try
	{
		InstNameEnumerator ie(ns, result, m_logger, m_iStore);
		CIMClass theClass = _instGetClass(ns, className);
		ie.handle(theClass);
		// If this is the namespace class then just return now
		if (className.equalsIgnoreCase("__Namespace")
			|| !deep)
		{
			return;
		}
		else
		{
			// TODO: measure whether it would be faster to use
			// enumClassNames + getClass() here.
			m_mStore.enumClass(ns,className,ie,deep,E_NOT_LOCAL_ONLY,
				E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN);
		}

	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace CIMRepositoryImpl
{

class instEnumerator : public StringResultHandlerIFC
{
public:
	instEnumerator(CIMRepository& rep_,
		const String& ns_,
		const CIMClass& theTopClass_,
		CIMInstanceResultHandlerIFC& result_,
		EDeepFlag deep_,
		ELocalOnlyFlag localOnly_,
		EIncludeQualifiersFlag includeQualifiers_,
		EIncludeClassOriginFlag includeClassOrigin_,
		const StringArray* propertyList_)
		: rep(rep_)
		, ns(ns_)
		, theTopClass(theTopClass_)
		, result(result_)
		, deep(deep_)
		, localOnly(localOnly_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
	{}
	void doHandle(const String& className)
	{
		CIMClass theClass = rep._instGetClass(ns, className);
		rep.m_iStore.getCIMInstances(ns, className, theTopClass, theClass, result,
			deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
		OW_LOG_DEBUG(rep.m_logger, Format("CIMRepository Enumerated derived instances: %1:%2", ns, className));
	}
private:
	CIMRepository& rep;
	const String& ns;
	const CIMClass& theTopClass;
	CIMInstanceResultHandlerIFC& result;
	EDeepFlag deep;
	ELocalOnlyFlag localOnly;
	EIncludeQualifiersFlag includeQualifiers;
	EIncludeClassOriginFlag includeClassOrigin;
	const StringArray* propertyList;
};
} // end namespace CIMRepositoryImpl

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::enumInstances(
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result, EDeepFlag deep,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	EEnumSubclassesFlag enumSubclasses, OperationContext&)
{
	// deep means a different thing here than for enumInstanceNames.  See the spec.
	try
	{
		CIMClass theTopClass = _instGetClass(ns, className);
		m_iStore.getCIMInstances(ns, className, theTopClass, theTopClass, result,
			deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
		
		OW_LOG_DEBUG(m_logger, Format("CIMRepository Enumerated instances: %1:%2", ns,
			className));
		if (enumSubclasses)
		{
			CIMRepositoryImpl::instEnumerator ie(*this, ns, theTopClass, result, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
			m_mStore.enumClassNames(ns, className, ie, E_DEEP);
		}
	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMRepository::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	return getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, NULL, context);
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMRepository::getInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMClass* pOutClass,
	OperationContext&)
{
	StringArray lpropList;
	if (propertyList)
	{
		lpropList = *propertyList;
	}
	CIMInstance ci(CIMNULL);
	CIMClass cc(_instGetClass(ns, instanceName.getClassName()));
	try
	{
		ci = m_iStore.getCIMInstance(ns, instanceName, cc, localOnly,
			includeQualifiers, includeClassOrigin, propertyList);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	OW_ASSERT(ci);
	if (pOutClass)
	{
		*pOutClass = cc;
	}
	
	return ci;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMRepository::deleteInstance(const String& ns, const CIMObjectPath& cop_,
	OperationContext& acl)
{
	CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);
	OW_LOG_DEBUG(m_logger, Format("CIMRepository::deleteInstance.  cop = %1",
		cop.toString()));
	try
	{
		CIMClass theClass(CIMNULL);
		CIMInstance oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
			&theClass, acl);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssocDbHandle hdl = m_instAssocDb.getHandle();
		// Ensure no associations exist for this instance
		if (hdl.hasAssocEntries(ns, cop))
		{
			// TODO: Revisit this.  Instead of throwing, it is allowed in the
			// spec to to delete the associations that reference the instance.
			// See http://dmtf.org/standards/documents/WBEM/DSP200.html
			//   2.3.2.4. DeleteInstance
			OW_THROWCIMMSG(CIMException::FAILED,
				Format("Instance %1 has associations", cop.toString()).c_str());
		}
		// TODO: It would be good to check for Min(1) relationships to the
		// instance.
		// If we're deleting an association instance, then remove all
		// traces of it in the association database.
		if (theClass.isAssociation())
		{
			hdl.deleteEntries(ns, oldInst);
		}
#endif
		// Delete the instance from the instance repository
		m_iStore.deleteInstance(ns, cop, theClass);
		OW_ASSERT(oldInst);
		return oldInst;
	}
	catch(IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch(HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
CIMRepository::createInstance(
	const String& ns,
	const CIMInstance& ci,
	OperationContext& context)
{
	CIMObjectPath rval(ns, ci);
	try
	{
		OW_LOG_DEBUG(m_logger, Format("CIMRepository::createInstance.  ns = %1, "
			"instance = %2", ns, ci.toMOF()));
		CIMClass theClass = _instGetClass(ns, ci.getClassName());
		if (m_checkReferentialIntegrity)
		{
			if (theClass.isAssociation())
			{
				CIMPropertyArray pra = ci.getProperties(
					CIMDataType::REFERENCE);
				for (size_t j = 0; j < pra.size(); j++)
				{
					CIMValue cv = pra[j].getValue();
					if (!cv)
					{
						OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							"Association has a NULL reference");
					}
					CIMObjectPath op(CIMNULL);
					cv.get(op);
					if (!op)
					{
						OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							"Association has a NULL reference");
					}
					CIMClass rcc(CIMNULL);
					try
					{
						m_env->getCIMOMHandle(context, ServiceEnvironmentIFC::E_USE_PROVIDERS)->getInstance(ns, op);
					}
					catch (CIMException& e)
					{
						OW_THROWCIMMSG_SUBEX(CIMException::INVALID_PARAMETER,
							Format("Association references an invalid instance:"
								" %1", op.toString()).c_str(), e);
					}
				}
			}
			_validatePropagatedKeys(context, ns, ci, theClass);
		}
		//TODO: _checkRequiredProperties(theClass, ci);
		m_iStore.createInstance(ns, theClass, ci);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if (theClass.isAssociation())
		{
			AssocDbHandle hdl = m_instAssocDb.getHandle();
			hdl.addEntries(ns, ci);
		}
#endif
		OW_ASSERT(rval);
		return rval;
	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMRepository::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	OperationContext& acl)
{
	try
	{
		CIMClass theClass(CIMNULL);
		CIMObjectPath cop(ns, modifiedInstance);
		CIMInstance oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
			&theClass, acl);
		//TODO: _checkRequiredProperties(theClass, modifiedInstance);
		m_iStore.modifyInstance(ns, cop, theClass, modifiedInstance, oldInst, includeQualifiers, propertyList);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		// TODO: Verify that this code is needed.  Aren't all references keys, and thus can't be changed?  So why update the assoc db?
		// just create a test to try and break it.
		if (theClass.isAssociation())
		{
			AssocDbHandle adbHdl = m_instAssocDb.getHandle();
			adbHdl.deleteEntries(ns, oldInst);
			adbHdl.addEntries(ns, modifiedInstance);
		}
#endif
		OW_ASSERT(oldInst);
		return oldInst;
	}
	catch (HDBException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
	catch (IOException& e)
	{
		OW_THROWCIM_SUBEX(CIMException::FAILED, e);
	}
}
#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::setProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, const CIMValue& valueArg,
	OperationContext& context)
{
	CIMClass theClass = _instGetClass(ns, name.getClassName());
	CIMProperty cp = theClass.getProperty(propertyName);
	if (!cp)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	// Ensure value passed in is right data type
	CIMValue cv(valueArg);
	if (cv && (cp.getDataType().getType() != cv.getType()))
	{
		try
		{
			// this throws a FAILED CIMException if it can't convert
			cv = CIMValueCast::castValueToDataType(cv, cp.getDataType());
		}
		catch (CIMException& ce)
		{
			// translate FAILED to TYPE_MISMATCH
			if (ce.getErrNo() == CIMException::FAILED)
			{
				ce.setErrNo(CIMException::TYPE_MISMATCH);
			}
			throw ce;
		}
	}
	CIMInstance ci = getInstance(ns, name, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		NULL, context);
	if (!ci)
	{
		OW_THROWCIMMSG(CIMException::NOT_FOUND, name.toString().c_str());
	}
	CIMProperty tcp = ci.getProperty(propertyName);
	if (cp.isKey() && tcp.getValue() && !tcp.getValue().equal(cv))
	{
		String msg("Cannot modify key property: ");
		msg += cp.getName();
		OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
	}
	cp.setValue(cv);
	ci.setProperty(cp);
	modifyInstance(ns, ci, E_INCLUDE_QUALIFIERS, 0, context);
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

#if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMRepository::getProperty(
	const String& ns,
	const CIMObjectPath& name,
	const String& propertyName, OperationContext& context)
{
	CIMClass theClass = _instGetClass(ns,name.getClassName());
	CIMProperty cp = theClass.getProperty(propertyName);
	if (!cp)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	CIMInstance ci = getInstance(ns, name, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		NULL, context);
	CIMProperty prop = ci.getProperty(propertyName);
	if (!prop)
	{
		OW_THROWCIMMSG(CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}
	return prop.getValue();
}
#endif // #if !defined(OW_DISABLE_PROPERTY_OPERATIONS)
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMRepository::invokeMethod(
	const String&,
	const CIMObjectPath&,
	const String&, const CIMParamValueArray&,
	CIMParamValueArray&, OperationContext&)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED);
}
//////////////////////////////////////////////////////////////////////
void
CIMRepository::execQuery(
	const String&,
	CIMInstanceResultHandlerIFC&,
	const String&,
	const String&, OperationContext&)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED);
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::associators(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &result, 0, 0,
		context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::associatorsClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, 0, 0, &result,
		context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::associatorNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& assocClass, const String& resultClass,
	const String& role, const String& resultRole,
	OperationContext& context)
{
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, 0, &result, 0, context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::references(
	const String& ns,
	const CIMObjectPath& path,
	CIMInstanceResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &result, 0, 0, context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::referencesClasses(
	const String& ns,
	const CIMObjectPath& path,
	CIMClassResultHandlerIFC& result,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, OperationContext& context)
{
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, 0, 0, &result, context);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::referenceNames(
	const String& ns,
	const CIMObjectPath& path,
	CIMObjectPathResultHandlerIFC& result,
	const String& resultClass, const String& role,
	OperationContext& context)
{
	_commonReferences(ns, path, resultClass, role, E_EXCLUDE_QUALIFIERS, E_EXCLUDE_CLASS_ORIGIN, 0, 0, &result, 0,
		context);
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class assocClassBuilder : public CIMClassResultHandlerIFC
	{
	public:
		assocClassBuilder(
			CIMClassArray& Assocs_)
		: Assocs(Assocs_)
		{}
	protected:
		virtual void doHandle(const CIMClass &cc)
		{
			if (!cc.isAssociation())
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					Format("class %1 is not an association", cc.getName()).c_str());
			}
			Assocs.append(cc);
		}
	private:
		CIMClassArray& Assocs;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_commonReferences(
	const String& ns,
	const CIMObjectPath& path_,
	const CIMName& resultClass, const CIMName& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult, OperationContext& context)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	if (!m_nStore.nameSpaceExists(ns))
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.
	CIMClassArray Assocs;
	assocClassBuilder assocClassResult(Assocs);
	_getAssociationClasses(ns, resultClass, path.getClassName(), assocClassResult, role, context);
	StringArray resultClassNames;
	for (size_t i = 0; i < Assocs.size(); i++)
	{
		resultClassNames.append(Assocs[i].getName());
	}
	SortedVectorSet<CIMName> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		_staticReferencesClass(path,
			resultClass == CIMName() ? 0 : &resultClassNamesSet,
			role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, context);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticReferences(path,
				resultClass == CIMName() ? 0 : &resultClassNamesSet, role,
				includeQualifiers, includeClassOrigin, propertyList, *piresult, context);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			_staticReferenceNames(path,
				resultClass == CIMName() ? 0 : &resultClassNamesSet, role,
				*popresult);
		}
		else
		{
			OW_ASSERT(0);
		}
	}
}
namespace
{
//////////////////////////////////////////////////////////////////////////////
	class staticReferencesObjectPathResultHandler : public AssocDbEntryResultHandlerIFC
	{
	public:
		staticReferencesObjectPathResultHandler(
			CIMObjectPathResultHandlerIFC& result_)
		: result(result_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry::entry &e)
		{
			result.handle(e.m_associationPath);
		}
	private:
		CIMObjectPathResultHandlerIFC& result;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticReferencesClassResultHandler : public AssocDbEntryResultHandlerIFC
	{
	public:
		staticReferencesClassResultHandler(
			CIMClassResultHandlerIFC& result_,
			CIMRepository& server_,
			String& ns_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propList_,
			OperationContext& context_)
		: result(result_)
		, server(server_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, context(context_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry::entry &e)
		{
			CIMObjectPath cop = e.m_associationPath;
			if (cop.getNameSpace().empty())
			{
				cop.setNameSpace(ns);
			}
			CIMClass cc = server.getClass(cop.getNameSpace(),
				cop.getClassName(), E_NOT_LOCAL_ONLY, includeQualifiers,
				includeClassOrigin, propList, context);
			result.handle(cc);
		}
	private:
		CIMClassResultHandlerIFC& result;
		CIMRepository& server;
		String& ns;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propList;
		OperationContext& context;
	};
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsInstResultHandler : public AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsInstResultHandler(
			OperationContext& context_,
			const CIMOMHandleIFCRef& hdl_,
			CIMInstanceResultHandlerIFC& result_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propertyList_)
		: context(context_)
		, hdl(hdl_)
		, result(result_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry::entry &e)
		{
			CIMObjectPath op = e.m_associatedObject;
			CIMInstance ci = hdl->getInstance(op.getNameSpace(), op, E_NOT_LOCAL_ONLY, includeQualifiers, includeClassOrigin, propertyList);
			ci.setNameSpace(op.getNameSpace()); 
			result.handle(ci);
		}
	private:
		OperationContext& context;
		CIMOMHandleIFCRef hdl;
		CIMInstanceResultHandlerIFC& result;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propertyList;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticReferencesInstResultHandler : public AssocDbEntryResultHandlerIFC
	{
	public:
		staticReferencesInstResultHandler(OperationContext& intAclInfo_,
			const CIMOMHandleIFCRef& hdl_,
			CIMInstanceResultHandlerIFC& result_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propertyList_)
		: intAclInfo(intAclInfo_)
		, hdl(hdl_)
		, result(result_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry::entry &e)
		{
			CIMObjectPath op = e.m_associationPath;
			CIMInstance ci = hdl->getInstance(op.getNameSpace(), op, E_NOT_LOCAL_ONLY, includeQualifiers, includeClassOrigin, propertyList);
			ci.setNameSpace(op.getNameSpace()); 
			result.handle(ci);
		}
	private:
		OperationContext& intAclInfo;
		CIMOMHandleIFCRef hdl;
		CIMInstanceResultHandlerIFC& result;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propertyList;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_staticReferences(const CIMObjectPath& path,
	const SortedVectorSet<CIMName>* refClasses, const CIMName& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC& result,
	OperationContext& context)
{
	AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticReferencesInstResultHandler handler(context, m_env->getCIMOMHandle(context,
		ServiceEnvironmentIFC::E_USE_PROVIDERS), result,
		includeQualifiers, includeClassOrigin, propertyList);
	dbhdl.getAllEntries(path,
		refClasses, 0, role, CIMName(), handler);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_staticReferenceNames(const CIMObjectPath& path,
	const SortedVectorSet<CIMName>* refClasses, const CIMName& role,
	CIMObjectPathResultHandlerIFC& result)
{
	AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticReferencesObjectPathResultHandler handler(result);
	dbhdl.getAllEntries(path,
		refClasses, 0, role, CIMName(), handler);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_commonAssociators(
	const String& ns,
	const CIMObjectPath& path_,
	const CIMName& assocClassName, const CIMName& resultClass,
	const CIMName& role, const CIMName& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	if (!m_nStore.nameSpaceExists(ns))
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	// Get association classes from the association repository
	CIMClassArray Assocs;
	assocClassBuilder assocClassResult(Assocs);
	_getAssociationClasses(ns, assocClassName, path.getClassName(), assocClassResult, role, context);
	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	CIMNameArray resultClassNames;
	if (resultClass != CIMName())
	{
		resultClassNames = getClassChildren(m_mStore, ns, resultClass);
		resultClassNames.append(resultClass);
	}
	StringArray assocClassNames;
	for (size_t i = 0; i < Assocs.size(); i++)
	{
		assocClassNames.append(Assocs[i].getName());
	}
	SortedVectorSet<CIMName> assocClassNamesSet(assocClassNames.begin(),
			assocClassNames.end());
	SortedVectorSet<CIMName> resultClassNamesSet(resultClassNames.begin(),
			resultClassNames.end());
	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		_staticAssociatorsClass(path, assocClassName == CIMName() ? 0 : &assocClassNamesSet,
			resultClass == CIMName() ? 0 : &resultClassNamesSet,
			role, resultRole, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, context);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticAssociators(path, assocClassName == CIMName() ? 0 : &assocClassNamesSet,
				resultClass == CIMName() ? 0 : &resultClassNamesSet, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList, *piresult, context);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			_staticAssociatorNames(path, assocClassName == CIMName() ? 0 : &assocClassNamesSet,
				resultClass == CIMName() ? 0 : &resultClassNamesSet, role, resultRole,
				*popresult);
		}
		else
		{
			OW_ASSERT(0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_staticAssociators(const CIMObjectPath& path,
	const SortedVectorSet<CIMName>* passocClasses,
	const SortedVectorSet<CIMName>* presultClasses,
	const CIMName& role, const CIMName& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC& result,
	OperationContext& context)
{
	AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticAssociatorsInstResultHandler handler(context, m_env->getCIMOMHandle(context,
		ServiceEnvironmentIFC::E_USE_PROVIDERS), result,
		includeQualifiers, includeClassOrigin, propertyList);
	dbhdl.getAllEntries(path,
		passocClasses, presultClasses, role, resultRole, handler);
		
}
namespace
{
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsObjectPathResultHandler : public AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsObjectPathResultHandler(
			CIMObjectPathResultHandlerIFC& result_)
		: result(result_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry::entry &e)
		{
			result.handle(e.m_associatedObject);
		}
	private:
		CIMObjectPathResultHandlerIFC& result;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsClassResultHandler : public AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsClassResultHandler(
			CIMClassResultHandlerIFC& result_,
			CIMRepository& server_,
			String& ns_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propList_,
			OperationContext& context_)
		: result(result_)
		, server(server_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, context(context_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry::entry &e)
		{
			CIMObjectPath cop = e.m_associatedObject;
			if (cop.getNameSpace().empty())
			{
				cop.setNameSpace(ns);
			}
			CIMClass cc = server.getClass(cop.getNameSpace(),
				cop.getClassName(), E_NOT_LOCAL_ONLY, includeQualifiers,
				includeClassOrigin, propList, context);
			result.handle(cc);
		}
	private:
		CIMClassResultHandlerIFC& result;
		CIMRepository& server;
		String& ns;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propList;
		OperationContext& context;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_staticAssociatorNames(const CIMObjectPath& path,
	const SortedVectorSet<CIMName>* passocClasses,
	const SortedVectorSet<CIMName>* presultClasses,
	const CIMName& role, const CIMName& resultRole,
	CIMObjectPathResultHandlerIFC& result)
{
	AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticAssociatorsObjectPathResultHandler handler(result);
	dbhdl.getAllEntries(path,
		passocClasses, presultClasses, role, resultRole, handler);
		
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_staticAssociatorsClass(
	const CIMObjectPath& path,
	const SortedVectorSet<CIMName>* assocClassNames,
	const SortedVectorSet<CIMName>* resultClasses,
	const CIMName& role, const CIMName& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	AssocDbHandle dbhdl = m_classAssocDb.getHandle();
	// need to run the query for every superclass of the class arg.
	CIMName curClsName = path.getClassName();
	CIMObjectPath curPath = path;
	while (curClsName != CIMName())
	{
		if (popresult != 0)
		{
			staticAssociatorsObjectPathResultHandler handler(*popresult);
			dbhdl.getAllEntries(curPath, assocClassNames, resultClasses, role, resultRole,
				handler);
		}
		else if (pcresult != 0)
		{
			String ns = path.getNameSpace();
			staticAssociatorsClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, context);
			dbhdl.getAllEntries(curPath, assocClassNames, resultClasses, role, resultRole,
				handler);
		}
		else
		{
			OW_ASSERT(0);
		}
		// get the current class so we can get the name of the superclass
		CIMClass theClass = _getClass(curPath.getNameSpace(), curPath.getClassName());
		curClsName = theClass.getSuperClass();
		curPath.setClassName(curClsName);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_staticReferencesClass(const CIMObjectPath& path,
	const SortedVectorSet<CIMName>* resultClasses,
	const CIMName& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	AssocDbHandle dbhdl = m_classAssocDb.getHandle();
	// need to run the query for every superclass of the class arg.
	CIMName curClsName = path.getClassName();
	CIMObjectPath curPath = path;
	while (curClsName != CIMName())
	{
		OW_LOG_DEBUG(m_logger, Format("curPath = %1", curPath.toString()));
		if (popresult != 0)
		{
			staticReferencesObjectPathResultHandler handler(*popresult);
			dbhdl.getAllEntries(curPath, resultClasses, 0, role, CIMName(),
				handler);
		}
		else if (pcresult != 0)
		{
			String ns = path.getNameSpace();
			staticReferencesClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, context);
			dbhdl.getAllEntries(curPath, resultClasses, 0, role, CIMName(),
				handler);
		}
		else
		{
			OW_ASSERT(0);
		}
		// get the current class so we can get the name of the superclass
		CIMClass theClass = _getClass(curPath.getNameSpace(), curPath.getClassName());
		curClsName = theClass.getSuperClass();
		curPath.setClassName(curClsName);
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class assocHelper : public CIMClassResultHandlerIFC
	{
	public:
		assocHelper(
			CIMClassResultHandlerIFC& handler_,
			MetaRepository& m_mStore_,
			const String& ns_)
		: handler(handler_)
		, m_mStore(m_mStore_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const CIMClass &cc)
		{
			handler.handle(cc);
			m_mStore.enumClass(ns, cc.getName(), handler, E_DEEP, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN);
		}
	private:
		CIMClassResultHandlerIFC& handler;
		MetaRepository& m_mStore;
		const String& ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_getAssociationClasses(const String& ns,
		const CIMName& assocClassName, const CIMName& className,
		CIMClassResultHandlerIFC& result, const CIMName& role,
		OperationContext& context)
{
	if (assocClassName != CIMName())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		m_mStore.enumClass(ns, assocClassName.toString(), result, E_DEEP, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN);
		CIMClass cc(CIMNULL);
		CIMException::ErrNoType rc = m_mStore.getCIMClass(ns, assocClassName.toString(), E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, cc);
		if (rc != CIMException::SUCCESS)
		{
			OW_THROWCIM(CIMException::FAILED);
		}
		result.handle(cc);
	}
	else
	{
		// need to get all the assoc classes with dynamic providers
		CIMObjectPath cop(className, ns);
		_staticReferencesClass(cop,0,role,E_INCLUDE_QUALIFIERS,E_EXCLUDE_CLASS_ORIGIN,0,0,&result, context);
		// TODO: test if this is faster
		//assocHelper helper(result, m_mStore, ns);
		//m_mStore.getTopLevelAssociations(ns, helper);
	}
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::checkGetClassRvalAndThrow(CIMException::ErrNoType rval,
	const String& ns, const CIMName& className)
{
	if (rval != CIMException::SUCCESS)
	{
		// check whether the namespace was invalid or not
		if (rval == CIMException::NOT_FOUND)
		{
			if (!m_nStore.nameSpaceExists(ns))
			{
				OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
			}
		}
		OW_THROWCIMMSG(rval, CIMObjectPath(className, ns).toString().c_str());
	}
}
void
CIMRepository::checkGetClassRvalAndThrowInst(CIMException::ErrNoType rval,
	const String& ns, const CIMName& className)
{
	if (rval != CIMException::SUCCESS)
	{
		// check whether the namespace was invalid or not
		if (rval == CIMException::NOT_FOUND)
		{
			if (!m_nStore.nameSpaceExists(ns))
			{
				OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
			}
			else
			{
				rval = CIMException::INVALID_CLASS;
			}
		}
		OW_THROWCIMMSG(rval, CIMObjectPath(className, ns).toString().c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class ClassNameArrayBuilder : public CIMObjectPathResultHandlerIFC
	{
	public:
		ClassNameArrayBuilder(StringArray& names_)
		: names(names_)
		{}
		void doHandle(const CIMObjectPath& op)
		{
			names.push_back(op.getClassName());
		}
	private:
		StringArray& names;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::_validatePropagatedKeys(OperationContext& context, const String& ns,
	const CIMInstance& ci, const CIMClass& theClass)
{
	CIMObjectPathArray rv;
	CIMPropertyArray kprops = theClass.getKeys();
	if (kprops.size() == 0)
	{
		return;
	}
	Map<CIMName, CIMPropertyArray> theMap;
	Bool hasPropagatedKeys = false;
	// Look at all propagated key properties
	for (size_t i = 0; i < kprops.size(); i++)
	{
		CIMQualifier cq = kprops[i].getQualifier(
			CIMQualifier::CIM_QUAL_PROPAGATED);
		if (!cq)
		{
			continue;
		}
		hasPropagatedKeys = true;
		CIMValue cv = cq.getValue();
		if (!cv)
		{
			continue;
		}
		String cls;
		cv.get(cls);
		if (cls.empty())
		{
			continue;
		}
		size_t idx = cls.indexOf('.');
		CIMName ppropName;
		if (idx != String::npos)
		{
			ppropName = cls.substring(idx+1);
			cls = cls.substring(0,idx);
		}
		CIMProperty cp = ci.getProperty(kprops[i].getName());
		if (!cp || !cp.getValue())
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Cannot create instance. Propagated key field missing:"
					" %1", kprops[i].getName()).c_str());
		}
		if (ppropName != CIMName())
		{
			// We need to use the propagated property name, not the property
			// name on ci.  e.g. Given
			// [Propagated("fooClass.fooPropName")] string myPropName;
			// we need to check for fooPropName as the key to the propagated
			// instance, not myPropName.
			cp.setName(ppropName);
		}
		theMap[cls].append(cp);
	}
	if (!hasPropagatedKeys)
	{
		return;
	}
	if (theMap.size() == 0)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"Cannot create instance. Propagated key properties missing");
	}
	CIMObjectPath op(ns, ci);
	Map<CIMName, CIMPropertyArray>::iterator it = theMap.begin();
	while (it != theMap.end())
	{
		CIMName clsname = it->first;
		
		// since we don't know what class the keys refer to, we get all subclasses
		// and try calling getInstance for each to see if we can find one with
		// the matching keys.
		OW_LOG_DEBUG(m_logger, Format("Getting class children of: %1", clsname));
		CIMNameArray classes = getClassChildren(m_mStore, ns,
			clsname);
		classes.push_back(clsname);
		op.setKeys(it->second);
		bool found = false;
		for (size_t i = 0; i < classes.size(); ++i)
		{
			op.setClassName(classes[i]);
			OW_LOG_DEBUG(m_logger, Format("Trying getInstance of: %1", op.toString()));
			try
			{
				m_env->getCIMOMHandle(context, ServiceEnvironmentIFC::E_USE_PROVIDERS)->getInstance(ns, op);
				// if the previous line didn't throw, then we found it.
				found = true;
				break;
			}
			catch (const CIMException&)
			{
			}
		}
		if (!found)
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Propagated keys refer to non-existent object: %1",
					op.toString()).c_str());
		}
		++it;
	}
}

namespace
{
// TODO: Make this configurable?  Maybe even a parameter that can be specifed by the client on each request?
const Timeout LockTimeout = Timeout::relative(300); // 5 mins.
}

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context)
{
	switch (op)
	{
	case E_CREATE_NAMESPACE:
	case E_DELETE_NAMESPACE:
	case E_DELETE_INSTANCE:
	case E_CREATE_INSTANCE:
	case E_MODIFY_INSTANCE:
	case E_SET_PROPERTY:
	case E_INVOKE_METHOD:
	case E_EXEC_QUERY:
		m_schemaLock.getWriteLock(LockTimeout);
		m_instanceLock.getWriteLock(LockTimeout);
		break;
	case E_ENUM_NAMESPACE:
	case E_GET_QUALIFIER_TYPE:
	case E_ENUM_QUALIFIER_TYPES:
	case E_GET_CLASS:
	case E_ENUM_CLASSES:
	case E_ENUM_CLASS_NAMES:
	case E_ASSOCIATORS_CLASSES:
	case E_REFERENCES_CLASSES:
		m_schemaLock.getReadLock(LockTimeout);
		break;
	case E_DELETE_QUALIFIER_TYPE:
	case E_SET_QUALIFIER_TYPE:
	case E_DELETE_CLASS:
	case E_CREATE_CLASS:
	case E_MODIFY_CLASS:
		m_schemaLock.getWriteLock(LockTimeout);
		break;
	case E_ENUM_INSTANCES:
	case E_ENUM_INSTANCE_NAMES:
	case E_GET_INSTANCE:
	case E_GET_PROPERTY:
	case E_ASSOCIATOR_NAMES:
	case E_ASSOCIATORS:
	case E_REFERENCE_NAMES:
	case E_REFERENCES:
		m_schemaLock.getReadLock(LockTimeout);
		m_instanceLock.getReadLock(LockTimeout);
		break;
	case E_EXPORT_INDICATION:
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository::endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result)
{
	switch (op)
	{
	case E_CREATE_NAMESPACE:
	case E_DELETE_NAMESPACE:
	case E_DELETE_INSTANCE:
	case E_CREATE_INSTANCE:
	case E_MODIFY_INSTANCE:
	case E_SET_PROPERTY:
	case E_INVOKE_METHOD:
	case E_EXEC_QUERY:
		m_instanceLock.releaseWriteLock();
		m_schemaLock.releaseWriteLock();
		break;
	case E_ENUM_NAMESPACE:
	case E_GET_QUALIFIER_TYPE:
	case E_ENUM_QUALIFIER_TYPES:
	case E_GET_CLASS:
	case E_ENUM_CLASSES:
	case E_ENUM_CLASS_NAMES:
	case E_ASSOCIATORS_CLASSES:
	case E_REFERENCES_CLASSES:
		m_schemaLock.releaseReadLock();
		break;
	case E_DELETE_QUALIFIER_TYPE:
	case E_SET_QUALIFIER_TYPE:
	case E_DELETE_CLASS:
	case E_CREATE_CLASS:
	case E_MODIFY_CLASS:
		m_schemaLock.releaseWriteLock();
		break;
	case E_ENUM_INSTANCES:
	case E_ENUM_INSTANCE_NAMES:
	case E_GET_INSTANCE:
	case E_GET_PROPERTY:
	case E_ASSOCIATOR_NAMES:
	case E_ASSOCIATORS:
	case E_REFERENCE_NAMES:
	case E_REFERENCES:
		m_instanceLock.releaseReadLock();
		m_schemaLock.releaseReadLock();
		break;
	case E_EXPORT_INDICATION:
	default:
		break;
	}
}


const char* const CIMRepository::INST_REPOS_NAME = "instances";
const char* const CIMRepository::META_REPOS_NAME = "schema";
const char* const CIMRepository::NS_REPOS_NAME = "namespaces";
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const char* const CIMRepository::CLASS_ASSOC_REPOS_NAME = "classassociation";
const char* const CIMRepository::INST_ASSOC_REPOS_NAME = "instassociation";
#endif

} // end namespace OW_NAMESPACE

