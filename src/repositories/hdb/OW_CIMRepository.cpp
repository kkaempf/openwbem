/*******************************************************************************
* Copyright (C) 2002 Center 7, Inc All rights reserved.
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
#include "OW_CIMRepository.hpp"
#include "OW_FileSystem.hpp"
#include "OW_RepositoryStreams.hpp"
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

//////////////////////////////////////////////////////////////////////////////
OW_CIMRepository::OW_CIMRepository(OW_ServiceEnvironmentIFCRef env)
	: OW_RepositoryIFC()
	, m_nStore(env)
	, m_iStore(env)
	, m_mStore(env)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	, m_classAssocDb(env)
	, m_instAssocDb(env)
#endif
	, m_env(env)
    , m_checkReferentialIntegrity(false)
{
    if (m_env->getConfigItem(OW_ConfigOpts::CHECK_REFERENTIAL_INTEGRITY_opt, 
		OW_DEFAULT_CHECK_REFERENTIAL_INTEGRITY).equalsIgnoreCase("true"))
    {
        m_checkReferentialIntegrity = true;
    }
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMRepository::~OW_CIMRepository()
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
OW_CIMRepository::open(const OW_String& path)
{
	if(m_nStore.isOpen())
	{
		close();
	}

	OW_FileSystem::makeDirectory(path);
	if(!OW_FileSystem::exists(path))
	{
		OW_String msg("failed to create directory: " );
		msg += path;
		OW_THROW(OW_IOException, msg.c_str());
	}
	else
	{
		if(!OW_FileSystem::canWrite(path))
		{
			OW_String msg("don't have write access to directory: ");
			msg += path;
			OW_THROW(OW_IOException, msg.c_str());
		}
	}

	OW_String fname = path;
	if(!fname.endsWith(OW_String(OW_FILENAME_SEPARATOR)))
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
OW_CIMRepository::close()
{
	m_nStore.close();
	m_iStore.close();
	m_mStore.close();
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	m_classAssocDb.close();
	m_instAssocDb.close();
#endif
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::createNameSpace(const OW_String& ns,
	const OW_UserInfo&)
{
	if(ns.empty())
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	if(m_nStore.createNameSpace(ns) == -1)
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS,
			ns.c_str());
	}

	// TODO: Make this exception safe.
	m_iStore.createNameSpace(ns);
	m_mStore.createNameSpace(ns);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug(format("OW_CIMRepository created namespace: %1", ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::deleteNameSpace(const OW_String& ns,
	const OW_UserInfo&)
{
	if(ns.empty())
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	// TODO: Make this exception safe.
	m_nStore.deleteNameSpace(ns);
	m_iStore.deleteNameSpace(ns);
	m_mStore.deleteNameSpace(ns);
	
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug(format("OW_CIMRepository deleted namespace: %1", ns));
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumNameSpace(OW_StringResultHandlerIFC& result,
	const OW_UserInfo&)
{
	// TODO: Move this into m_nStore
	OW_HDBHandleLock hdl(&m_nStore, m_nStore.getHandle());

	OW_HDBNode nsNode = hdl->getFirstRoot();
	//OW_HDBNode nsNode = m_nStore.getNameSpaceNode(hdl, nsName);
	//if(!nsNode)
	//{
	//	OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, nsName.c_str());
	//}

	//nsNode = hdl->getFirstChild(nsNode);

	while(nsNode)
	{
		result.handle(nsNode.getKey());
		nsNode = hdl->getNextSibling(nsNode);
	}

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug("OW_CIMRepository enumerated namespaces");
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMRepository::getQualifierType(const OW_String& ns,
	const OW_String& qualifierName,
	const OW_UserInfo&)
{
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug(format("OW_CIMRepository getting qualifier type: %1",
			OW_CIMObjectPath(qualifierName,ns).toString()));
	}

	return m_mStore.getQualifierType(ns, qualifierName);
}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result,
	const OW_UserInfo&)
{
	m_mStore.enumQualifierTypes(ns, result);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug(format("OW_CIMRepository enumerated qualifiers in namespace: %1", ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::deleteQualifierType(const OW_String& ns, const OW_String& qualName,
	const OW_UserInfo&)
{
	// TODO: What happens if the qualifier is being used???
	if(!m_mStore.deleteQualifierType(ns, qualName))
	{
		if (m_nStore.nameSpaceExists(ns))
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				OW_String(ns + "/" + qualName).c_str());
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
				OW_String(ns + "/" + qualName).c_str());
		}
	}
	
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug(format("OW_CIMRepository deleted qualifier type: %1 in namespace: %2", qualName, ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::setQualifierType(
	const OW_String& ns,
	const OW_CIMQualifierType& qt, const OW_UserInfo&)
{
	m_mStore.setQualifierType(ns, qt);
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug(format("OW_CIMRepository set qualifier type: %1 in "
			"namespace: %2", qt.toString(), ns));
	}
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMRepository::getClass(
	const OW_String& ns, const OW_String& className, 
	OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
	const OW_UserInfo&)
{
	try
	{
		OW_CIMClass theClass(OW_CIMNULL);
		OW_CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, 
			localOnly, includeQualifiers, includeClassOrigin, propertyList, 
			theClass);
		checkGetClassRvalAndThrow(rval, ns, className);

		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("OW_CIMRepository got class: %1 from "
				"namespace: %2", theClass.getName(), ns));
		}

		return theClass;
	}
	catch(OW_HDBException& e)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, e.getMessage());
	}
	catch(OW_IOException& e)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, e.getMessage());
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMRepository::_getClass(const OW_String& ns, const OW_String& className)
{
	OW_CIMClass theClass(OW_CIMNULL);
	OW_CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, false, true, true, 0, theClass);
	checkGetClassRvalAndThrow(rval, ns, className);

	return theClass;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMRepository::_instGetClass(const OW_String& ns, const OW_String& className)
{
	OW_CIMClass theClass(OW_CIMNULL);
	OW_CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, false, true, true, 0, theClass);
	checkGetClassRvalAndThrowInst(rval, ns, className);

	return theClass;
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassDeleter : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassDeleter(OW_MetaRepository& mr, const OW_String& ns_,
			OW_InstanceRepository& mi
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			, OW_AssocDb& m_assocDb_
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
		virtual void doHandle(const OW_CIMClass &c)
		{
			OW_String cname = c.getName();
			if(!m_mStore.deleteClass(ns, cname))
			{
				OW_THROWCIM(OW_CIMException::NOT_FOUND);
			}

			// TODO: this doesn't work quite right.  what about associations to
			// the instances we delete?  If we fix deleteInstance to also delete 
			// associations, then we could just call enumInstances and then
			// deleteInstance for all instances.

			// delete any instances of the class
			m_iStore.deleteClass(ns, cname);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			// remove class from association index
			if (c.isAssociation())
			{
				OW_AssocDbHandle hdl = m_assocDb.getHandle();
				hdl.deleteEntries(ns,c);
			}
#endif

		}
	private:
		OW_MetaRepository& m_mStore;
		const OW_String& ns;
		OW_InstanceRepository& m_iStore;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		OW_AssocDb& m_assocDb;
#endif
	};
}
//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMRepository::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_UserInfo& acl)
{
	try
	{
		OW_CIMClass cc = _getClass(ns, className);
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
			OW_CIMOMHandleIFC::DEEP, OW_CIMOMHandleIFC::LOCAL_ONLY,
			OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS,
			OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN,
            acl);
		ccd.handle(cc);

		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("OW_CIMRepository deleted class: %1 in "
				"namespace: %2", className, ns));
		}

		return cc;
	}
	catch(OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch(OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::createClass(const OW_String& ns, const OW_CIMClass& cimClass_,
	const OW_UserInfo&)
{
	try
	{
		// m_mStore.createClass modifies cimClass to be consistent with base 
		// classes, etc.
		OW_CIMClass cimClass(cimClass_);
		m_mStore.createClass(ns, cimClass);
		m_iStore.createClass(ns, cimClass);

		// we need to re-get the class, so that it will be consistent.  currently
		// cimClass only contains "unique" items that are added in the child class.
		cimClass = _getClass(ns, cimClass.getName());

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if (cimClass.isAssociation())
		{
			OW_AssocDbHandle hdl = m_classAssocDb.getHandle();
			hdl.addEntries(ns,cimClass);
		}
#endif

		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("Created class: %1:%2", ns, cimClass.toMOF()));
		}
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMRepository::modifyClass(
	const OW_String& ns,
	const OW_CIMClass& cc,
	const OW_UserInfo&)
{
	OW_ASSERT(cc);
	try
	{
		OW_CIMClass origClass = _getClass(ns, cc.getName());

		// TODO: this needs to update the subclasses of the modified class.
		//			If that's not possible, then we need to throw a
		//			CLASS_HAS_CHILDREN CIMException.

		// TODO: Need to update the instances of the class and any subclasses.
		//			If that's not possible, then we need to throw a
		//			CLASS_HAS_INSTANCES CIMException.

		m_mStore.modifyClass(ns, cc);
		OW_ASSERT(origClass);
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("Modified class: %1:%2 from %3 to %4", ns,
				cc.getName(), origClass.toMOF(), cc.toMOF()));
		}
		return origClass;
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_UserInfo&)
{
	try
	{
		m_mStore.enumClass(ns, className,
			result, deep,
			localOnly, includeQualifiers, includeClassOrigin);
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("OW_CIMRepository enumerated classes: %1:%2", ns,
				className));
		}
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumClassNames(
	const OW_String& ns,
	const OW_String& className,
	OW_StringResultHandlerIFC& result,
	OW_Bool deep, const OW_UserInfo&)
{
	try
	{
		m_mStore.enumClassNames(ns, className, result, deep);
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("OW_CIMRepository enumerated class names: %1:%2", ns,
				className));
		}
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
namespace {

class stringArrayBuilder : public OW_StringResultHandlerIFC
{
public:
	stringArrayBuilder(OW_StringArray& result)
		: m_result(result)
	{}

	void doHandle(const OW_String& name)
	{
		m_result.push_back(name);
	}

private:
	OW_StringArray& m_result;
};

// utility function
OW_StringArray getClassChildren(OW_MetaRepository& rep, const OW_String& ns, const OW_String& clsName)
{
	OW_StringArray result;
	stringArrayBuilder handler(result);
	rep.enumClassNames(ns, clsName, handler, OW_CIMOMHandleIFC::DEEP);

	return result;
}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep,
	const OW_UserInfo&)
{
	try
	{
		OW_CIMClass theClass = _instGetClass(ns, className);

		m_iStore.getInstanceNames(ns, theClass, result);

		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("OW_CIMRepository enumerated instance names: %1:%2", ns,
				className));
		}
		if(!deep)
		{
			return;
		}

		// This code probably won't ever be executed, because CIMServer
		// has to do each class at a time because of providers, and will
		// thus only call us with deep=false.
		// If the situation ever changes, fix and enable the code below.

		OW_THROWCIMMSG(OW_CIMException::FAILED, "Internal server error");
		// TODO: Switch this to use a callback interface.
		/*
		OW_StringArray classNames = m_mStore.getClassChildren(ns,
			theClass.getName());

		for(size_t i = 0; i < classNames.size(); i++)
		{
			theClass = _instGetClass(ns, classNames[i]);
			m_iStore.getInstanceNames(ns, theClass, result);
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
			{
				m_env->getLogger()->logDebug(format("OW_CIMRepository enumerated derived instance names: %1:%2", ns,
					classNames[i]));
			}
		}
		*/
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
namespace {

class instEnumerator : public OW_StringResultHandlerIFC
{
public:
	instEnumerator(OW_CIMRepository& rep_,
		const OW_String& ns_,
		const OW_CIMClass& theTopClass_,
		OW_CIMInstanceResultHandlerIFC& result_,
		OW_Bool deep_,
		OW_Bool localOnly_,
		OW_Bool includeQualifiers_,
		OW_Bool includeClassOrigin_,
		const OW_StringArray* propertyList_)
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

	void doHandle(const OW_String& className)
	{
		OW_CIMClass theClass = rep._instGetClass(ns, className);

		rep.m_iStore.getCIMInstances(ns, className, theTopClass, theClass, result,
			deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
		if (rep.m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			rep.m_env->getLogger()->logDebug(format("OW_CIMRepository Enumerated derived instances: %1:%2", ns, className));
		}
	}
private:
	OW_CIMRepository& rep;
	const OW_String& ns;
	const OW_CIMClass& theTopClass;
	OW_CIMInstanceResultHandlerIFC& result;
	OW_Bool deep;
	OW_Bool localOnly;
	OW_Bool includeQualifiers;
	OW_Bool includeClassOrigin;
	const OW_StringArray* propertyList;
};

} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_Bool enumSubClasses, const OW_UserInfo&)
{
	// deep means a different thing here than for enumInstanceNames.  See the spec.
	try
	{
		OW_CIMClass theTopClass = _instGetClass(ns, className);

		m_iStore.getCIMInstances(ns, className, theTopClass, theTopClass, result,
			deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
		
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("OW_CIMRepository Enumerated instances: %1:%2", ns,
				className));
		}

		if (enumSubClasses)
		{
			instEnumerator ie(*this, ns, theTopClass, result, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
			m_mStore.enumClassNames(ns, className, ie, OW_CIMOMHandleIFC::DEEP);
			// TODO: Switch this to use the callback interface.
/*
			OW_StringArray classNames = m_mStore.getClassChildren(ns,
				className);

			for(size_t i = 0; i < classNames.size(); i++)
			{
				OW_CIMClass theClass = _instGetClass(ns, classNames[i]);

				m_iStore.getCIMInstances(ns, classNames[i], theTopClass, theClass, result,
					deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
				if (m_env->getLogger()->getLogLevel() == DebugLevel)
				{
					m_env->getLogger()->logDebug(format("OW_CIMRepository Enumerated derived instances: %1:%2", ns,
						classNames[i]));
				}
			}
*/
		}
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMRepository::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	return getInstance(ns, instanceName, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, NULL, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMRepository::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMClass* pOutClass,
	const OW_UserInfo&)
{
	OW_StringArray lpropList;
	if(propertyList)
	{
		lpropList = *propertyList;
	}

	OW_CIMInstance ci(OW_CIMNULL);

	OW_CIMClass cc(_instGetClass(ns, instanceName.getObjectName()));

	try
	{
		ci = m_iStore.getCIMInstance(ns, instanceName, cc, localOnly, 
			includeQualifiers, includeClassOrigin, propertyList);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}

	OW_ASSERT(ci);
	if(pOutClass)
	{
		*pOutClass = cc;
	}
	
	return ci;
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMRepository::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop_,
	const OW_UserInfo& acl)
{
	OW_CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->getLogger()->logDebug(format("OW_CIMRepository::deleteInstance.  cop = %1",
			cop.toString()));
	}

	try
	{
		OW_CIMClass theClass(OW_CIMNULL);
		OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
			&theClass, acl);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		OW_AssocDbHandle hdl = m_instAssocDb.getHandle();

		// Ensure no associations exist for this instance
		if(hdl.hasAssocEntries(ns, cop))
		{
			// TODO: Revisit this.  Instead of throwing, it is allowed in the
			// spec to to delete the associations that reference the instance.
			// See http://dmtf.org/standards/documents/WBEM/DSP200.html
			//   2.3.2.4. DeleteInstance
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("Instance %1 has associations", cop.toString()).c_str());
		}
		// TODO: It would be good to check for Min(1) relationships to the
		// instance.

		// If we're deleting an association instance, then remove all
		// traces of it in the association database.
		if(theClass.isAssociation())
		{
			hdl.deleteEntries(ns, oldInst);
		}
#endif

		// Delete the instance from the instance repository
		m_iStore.deleteInstance(ns, cop, theClass);

		OW_ASSERT(oldInst);
		return oldInst;
	}
	catch(OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch(OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_CIMRepository::createInstance(
	const OW_String& ns,
	const OW_CIMInstance& ci,
	const OW_UserInfo&)
{
	OW_CIMObjectPath rval(ci);

	try
	{
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->getLogger()->logDebug(format("OW_CIMRepository::createInstance.  ns = %1, "
				"instance = %2", ns, ci.toMOF()));
		}

		OW_CIMClass theClass = _instGetClass(ns, ci.getClassName());

		if (m_checkReferentialIntegrity)
		{
			if(theClass.isAssociation())
			{
				OW_CIMPropertyArray pra = ci.getProperties(
					OW_CIMDataType::REFERENCE);

				for(size_t j = 0; j < pra.size(); j++)
				{
					OW_CIMValue cv = pra[j].getValue();
					if(!cv)
					{
						OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"Association has a NULL reference");
					}

					OW_CIMObjectPath op(OW_CIMNULL);
					cv.get(op);

					if(!op)
					{
						OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							"Association has a NULL reference");
					}

					OW_CIMClass rcc(OW_CIMNULL);
					try
					{
						rcc = _instGetClass(ns,op.getObjectName());
						m_iStore.getCIMInstance(ns, op,rcc,false,true,true,0);
					}
					catch (OW_CIMException&)
					{
						OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							format("Association references an invalid instance:"
								" %1", op.toString()).c_str());
					}

				}

			}


			_validatePropagatedKeys(ns, ci, theClass);
		}
		//TODO: _checkRequiredProperties(theClass, ci);
		m_iStore.createInstance(ns, theClass, ci);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		if(theClass.isAssociation())
		{
			OW_AssocDbHandle hdl = m_instAssocDb.getHandle();
			hdl.addEntries(ns, ci);
		}
#endif

		OW_ASSERT(rval);
		return rval;
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMRepository::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_UserInfo& acl)
{
	try
	{
		OW_CIMClass theClass(OW_CIMNULL);
		OW_CIMObjectPath cop(modifiedInstance);
		OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
			&theClass, acl);

		//TODO: _checkRequiredProperties(theClass, modifiedInstance);

		m_iStore.modifyInstance(ns, cop, theClass, modifiedInstance, oldInst, includeQualifiers, propertyList);

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		// TODO: Verify that this code is needed.  Aren't all references keys, and thus can't be changed?  So why update the assoc db?
		// just create a test to try and break it.
		if(theClass.isAssociation())
		{
			OW_AssocDbHandle adbHdl = m_instAssocDb.getHandle();
			adbHdl.deleteEntries(ns, oldInst);
			adbHdl.addEntries(ns, modifiedInstance);
		}
#endif

		OW_ASSERT(oldInst);
		return oldInst;
	}
	catch (OW_HDBException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}
	catch (OW_IOException&)
	{
		OW_THROWCIM(OW_CIMException::FAILED);
	}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& valueArg,
	const OW_UserInfo& aclInfo)
{

	OW_CIMClass theClass = _instGetClass(ns, name.getObjectName());

	OW_CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	// Ensure value passed in is right data type
	OW_CIMValue cv(valueArg);
	if(cv && (cp.getDataType().getType() != cv.getType()))
	{
		try
		{
			// this throws a FAILED CIMException if it can't convert
			cv = OW_CIMValueCast::castValueToDataType(cv, cp.getDataType());
		}
		catch (OW_CIMException& ce)
		{
			// translate FAILED to TYPE_MISMATCH
			if (ce.getErrNo() == OW_CIMException::FAILED)
			{
				ce.setErrNo(OW_CIMException::TYPE_MISMATCH);
			}
			throw ce;
		}
	}

	OW_CIMInstance ci = getInstance(ns, name, false, true, true, NULL,
		NULL, aclInfo);

	if(!ci)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, name.toString().c_str());
	}

	OW_CIMProperty tcp = ci.getProperty(propertyName);

	if(cp.isKey() && tcp.getValue() && !tcp.getValue().equal(cv))
	{
		OW_String msg("Cannot modify key property: ");
		msg += cp.getName();
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	cp.setValue(cv);
	ci.setProperty(cp);
	modifyInstance(ns, ci, true, 0, aclInfo);
}

#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMRepository::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_UserInfo& aclInfo)
{

	OW_CIMClass theClass = _instGetClass(ns,name.getObjectName());

	OW_CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	OW_CIMInstance ci = getInstance(ns, name, false, true, true, NULL,
		NULL, aclInfo);
	OW_CIMProperty prop = ci.getProperty(propertyName);
	if(!prop)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	return prop.getValue();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMRepository::invokeMethod(
	const OW_String&,
	const OW_CIMObjectPath&,
	const OW_String&, const OW_CIMParamValueArray&,
	OW_CIMParamValueArray&, const OW_UserInfo&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::execQuery(
	const OW_String&,
	OW_CIMInstanceResultHandlerIFC&,
	const OW_String&,
	const OW_String&, const OW_UserInfo&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::associators(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &result, 0, 0,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, 0, 0, &result,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::associatorNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	const OW_UserInfo& aclInfo)
{
	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		false, false, 0, 0, &result, 0, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::references(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &result, 0, 0, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_UserInfo& aclInfo)
{
	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, 0, 0, &result, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::referenceNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	const OW_UserInfo& aclInfo)
{
	_commonReferences(ns, path, resultClass, role, false, false, 0, 0, &result, 0,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class assocClassBuilder : public OW_CIMClassResultHandlerIFC
	{
	public:
		assocClassBuilder(
			OW_CIMClassArray& Assocs_)
		: Assocs(Assocs_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &cc)
		{
			if (!cc.isAssociation())
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("class %1 is not an association", cc.getName()).c_str());
			}
			Assocs.append(cc);
		}
	private:
		OW_CIMClassArray& Assocs;
	};
}



//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_commonReferences(
	const OW_String& ns,
	const OW_CIMObjectPath& path_,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult, const OW_UserInfo& aclInfo)
{
	OW_CIMObjectPath path(path_);
	path.setNameSpace(ns);
	if (!m_nStore.nameSpaceExists(ns))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}

	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.

	OW_CIMClassArray Assocs;

	assocClassBuilder assocClassResult(Assocs);
	_getAssociationClasses(ns, resultClass, path.getObjectName(), assocClassResult, role, aclInfo);
	OW_StringArray resultClassNames;
	for(size_t i = 0; i < Assocs.size(); i++)
	{
		resultClassNames.append(Assocs[i].getName());
	}
	OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());

	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		_staticReferencesClass(path,
			resultClass.empty() ? 0 : &resultClassNamesSet,
			role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, aclInfo);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticReferences(path,
				resultClass.empty() ? 0 : &resultClassNamesSet, role,
				includeQualifiers, includeClassOrigin, propertyList, *piresult, aclInfo);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			_staticReferenceNames(path,
				resultClass.empty() ? 0 : &resultClassNamesSet, role,
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
	class staticReferencesObjectPathResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticReferencesObjectPathResultHandler(
			OW_CIMObjectPathResultHandlerIFC& result_)
		: result(result_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry::entry &e)
		{
			result.handle(e.m_associationPath);
		}
	private:
		OW_CIMObjectPathResultHandlerIFC& result;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticReferencesClassResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticReferencesClassResultHandler(
			OW_CIMClassResultHandlerIFC& result_,
			OW_CIMRepository& server_,
			OW_String& ns_,
			bool includeQualifiers_,
			bool includeClassOrigin_,
			const OW_StringArray* propList_,
			const OW_UserInfo& aclInfo_)
		: result(result_)
		, server(server_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, aclInfo(aclInfo_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry::entry &e)
		{
			OW_CIMObjectPath cop = e.m_associationPath;
			if (cop.getNameSpace().empty())
			{
				cop.setNameSpace(ns);
			}
			OW_CIMClass cc = server.getClass(cop.getNameSpace(),
				cop.getObjectName(), false, includeQualifiers,
				includeClassOrigin, propList, aclInfo);
			result.handle(cc);
		}
	private:
		OW_CIMClassResultHandlerIFC& result;
		OW_CIMRepository& server;
		OW_String& ns;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propList;
		const OW_UserInfo& aclInfo;
	};

//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsInstResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsInstResultHandler(const OW_UserInfo& intAclInfo_,
			OW_CIMRepository& server_, OW_CIMInstanceResultHandlerIFC& result_,
			bool includeQualifiers_, bool includeClassOrigin_,
			const OW_StringArray* propertyList_)
		: intAclInfo(intAclInfo_)
		, server(server_)
		, result(result_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry::entry &e)
		{
			OW_CIMObjectPath op = e.m_associatedObject;
			OW_CIMInstance ci = server.getInstance(op.getNameSpace(), op, false,
				includeQualifiers,includeClassOrigin,propertyList,intAclInfo);
			result.handle(ci);
		}
	private:
		const OW_UserInfo& intAclInfo;
		OW_CIMRepository& server;
		OW_CIMInstanceResultHandlerIFC& result;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propertyList;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticReferencesInstResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticReferencesInstResultHandler(const OW_UserInfo& intAclInfo_,
			OW_CIMRepository& server_, OW_CIMInstanceResultHandlerIFC& result_,
			bool includeQualifiers_, bool includeClassOrigin_,
			const OW_StringArray* propertyList_)
		: intAclInfo(intAclInfo_)
		, server(server_)
		, result(result_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry::entry &e)
		{
			OW_CIMObjectPath op = e.m_associationPath;

			OW_CIMInstance ci = server.getInstance(op.getNameSpace(), op, false,
				includeQualifiers,includeClassOrigin,propertyList,intAclInfo);
			result.handle(ci);
		}
	private:
		const OW_UserInfo& intAclInfo;
		OW_CIMRepository& server;
		OW_CIMInstanceResultHandlerIFC& result;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propertyList;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_staticReferences(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* refClasses, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC& result,
	const OW_UserInfo& aclInfo)
{
	OW_AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticReferencesInstResultHandler handler(aclInfo, *this, result,
		includeQualifiers, includeClassOrigin, propertyList);

	dbhdl.getAllEntries(path,
		refClasses, 0, role, OW_String(), handler);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_staticReferenceNames(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* refClasses, const OW_String& role,
	OW_CIMObjectPathResultHandlerIFC& result)
{
	OW_AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticReferencesObjectPathResultHandler handler(result);
	dbhdl.getAllEntries(path,
		refClasses, 0, role, OW_String(), handler);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_commonAssociators(
	const OW_String& ns,
	const OW_CIMObjectPath& path_,
	const OW_String& assocClassName, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList,
	OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult,
	const OW_UserInfo& aclInfo)
{
	OW_CIMObjectPath path(path_);
	path.setNameSpace(ns);
	if (!m_nStore.nameSpaceExists(ns))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}

	// Get association classes from the association repository
	OW_CIMClassArray Assocs;
	assocClassBuilder assocClassResult(Assocs);
	_getAssociationClasses(ns, assocClassName, path.getObjectName(), assocClassResult, role, aclInfo);

	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	OW_StringArray resultClassNames;
	if(!resultClass.empty())
	{
		resultClassNames = getClassChildren(m_mStore, ns, resultClass);
		resultClassNames.append(resultClass);
	}

	OW_StringArray assocClassNames;
	for(size_t i = 0; i < Assocs.size(); i++)
	{
		assocClassNames.append(Assocs[i].getName());
	}
	OW_SortedVectorSet<OW_String> assocClassNamesSet(assocClassNames.begin(),
			assocClassNames.end());
	OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(),
			resultClassNames.end());

	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		_staticAssociatorsClass(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
			resultClass.empty() ? 0 : &resultClassNamesSet,
			role, resultRole, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, aclInfo);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticAssociators(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
				resultClass.empty() ? 0 : &resultClassNamesSet, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList, *piresult, aclInfo);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			_staticAssociatorNames(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
				resultClass.empty() ? 0 : &resultClassNamesSet, role, resultRole,
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
OW_CIMRepository::_staticAssociators(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* passocClasses,
	const OW_SortedVectorSet<OW_String>* presultClasses,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC& result,
	const OW_UserInfo& aclInfo)
{

	OW_AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticAssociatorsInstResultHandler handler(aclInfo, *this, result,
		includeQualifiers, includeClassOrigin, propertyList);
	dbhdl.getAllEntries(path,
		passocClasses, presultClasses, role, resultRole, handler);
		
}

namespace
{
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsObjectPathResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsObjectPathResultHandler(
			OW_CIMObjectPathResultHandlerIFC& result_)
		: result(result_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry::entry &e)
		{
			result.handle(e.m_associatedObject);
		}
	private:
		OW_CIMObjectPathResultHandlerIFC& result;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsClassResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsClassResultHandler(
			OW_CIMClassResultHandlerIFC& result_,
			OW_CIMRepository& server_,
			OW_String& ns_,
			bool includeQualifiers_,
			bool includeClassOrigin_,
			const OW_StringArray* propList_,
			const OW_UserInfo& aclInfo_)
		: result(result_)
		, server(server_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, aclInfo(aclInfo_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry::entry &e)
		{
			OW_CIMObjectPath cop = e.m_associatedObject;
			if (cop.getNameSpace().empty())
			{
				cop.setNameSpace(ns);
			}
			OW_CIMClass cc = server.getClass(cop.getNameSpace(),
				cop.getObjectName(), false, includeQualifiers,
				includeClassOrigin, propList, aclInfo);
			result.handle(cc);
		}
	private:
		OW_CIMClassResultHandlerIFC& result;
		OW_CIMRepository& server;
		OW_String& ns;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propList;
		const OW_UserInfo& aclInfo;
	};

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_staticAssociatorNames(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* passocClasses,
	const OW_SortedVectorSet<OW_String>* presultClasses,
	const OW_String& role, const OW_String& resultRole,
	OW_CIMObjectPathResultHandlerIFC& result)
{

	OW_AssocDbHandle dbhdl = m_instAssocDb.getHandle();
	staticAssociatorsObjectPathResultHandler handler(result);
	dbhdl.getAllEntries(path,
		passocClasses, presultClasses, role, resultRole, handler);
		
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_staticAssociatorsClass(
	const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* assocClassNames,
	const OW_SortedVectorSet<OW_String>* resultClasses,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult,
	const OW_UserInfo& aclInfo)
{
	OW_AssocDbHandle dbhdl = m_classAssocDb.getHandle();

	// need to run the query for every superclass of the class arg.
	OW_String curClsName = path.getObjectName();
	OW_CIMObjectPath curPath = path;
	while (!curClsName.empty())
	{
		if (popresult != 0)
		{
			staticAssociatorsObjectPathResultHandler handler(*popresult);
			dbhdl.getAllEntries(curPath, assocClassNames, resultClasses, role, resultRole,
				handler);
		}
		else if (pcresult != 0)
		{
			OW_String ns = path.getNameSpace();
			staticAssociatorsClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, aclInfo);
			dbhdl.getAllEntries(curPath, assocClassNames, resultClasses, role, resultRole,
				handler);
		}
		else
		{
			OW_ASSERT(0);
		}

		// get the current class so we can get the name of the superclass
		OW_CIMClass theClass = _getClass(curPath.getNameSpace(), curPath.getObjectName());
		curClsName = theClass.getSuperClass();
		curPath.setObjectName(curClsName);
	}
}



//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_staticReferencesClass(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* resultClasses,
	const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList,
	OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult,
	const OW_UserInfo& aclInfo)
{
	OW_AssocDbHandle dbhdl = m_classAssocDb.getHandle();

	// need to run the query for every superclass of the class arg.
	OW_String curClsName = path.getObjectName();
	OW_CIMObjectPath curPath = path;
	while (!curClsName.empty())
	{
		m_env->getLogger()->logDebug(format("curPath = %1", curPath.toString()));
		if (popresult != 0)
		{
			staticReferencesObjectPathResultHandler handler(*popresult);
			dbhdl.getAllEntries(curPath, resultClasses, 0, role, OW_String(),
				handler);
		}
		else if (pcresult != 0)
		{
			OW_String ns = path.getNameSpace();
			staticReferencesClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, aclInfo);
			dbhdl.getAllEntries(curPath, resultClasses, 0, role, OW_String(),
				handler);
		}
		else
		{
			OW_ASSERT(0);
		}

		// get the current class so we can get the name of the superclass
		OW_CIMClass theClass = _getClass(curPath.getNameSpace(), curPath.getObjectName());
		curClsName = theClass.getSuperClass();
		curPath.setObjectName(curClsName);
	}
}



//////////////////////////////////////////////////////////////////////////////
namespace
{
	class assocHelper : public OW_CIMClassResultHandlerIFC
	{
	public:
		assocHelper(
			OW_CIMClassResultHandlerIFC& handler_,
			OW_MetaRepository& m_mStore_,
			const OW_String& ns_)
		: handler(handler_)
		, m_mStore(m_mStore_)
		, ns(ns_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &cc)
		{
			handler.handle(cc);
			m_mStore.enumClass(ns, cc.getName(), handler, true, false, true, true);
		}
	private:
		OW_CIMClassResultHandlerIFC& handler;
		OW_MetaRepository& m_mStore;
		const OW_String& ns;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_getAssociationClasses(const OW_String& ns,
		const OW_String& assocClassName, const OW_String& className,
		OW_CIMClassResultHandlerIFC& result, const OW_String& role,
		const OW_UserInfo& aclInfo)
{
	if(!assocClassName.empty())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		m_mStore.enumClass(ns, assocClassName, result, true, false, true, true);
		OW_CIMClass cc(OW_CIMNULL);
		OW_CIMException::ErrNoType rc = m_mStore.getCIMClass(ns, assocClassName, false, true, true, 0, cc);
		if (rc != OW_CIMException::SUCCESS)
		{
			OW_THROWCIM(OW_CIMException::FAILED);
		}
		result.handle(cc);
	}
	else
	{
		// need to get all the assoc classes with dynamic providers
		OW_CIMObjectPath cop(className, ns);
		_staticReferencesClass(cop,0,role,true,false,0,0,&result, aclInfo);

		// TODO: test if this is faster
		//assocHelper helper(result, m_mStore, ns);
		//m_mStore.getTopLevelAssociations(ns, helper);
	}
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::checkGetClassRvalAndThrow(OW_CIMException::ErrNoType rval,
	const OW_String& ns, const OW_String& className)
{
	if (rval != OW_CIMException::SUCCESS)
	{
		// check whether the namespace was invalid or not
		if (rval == OW_CIMException::NOT_FOUND)
		{
			if (!m_nStore.nameSpaceExists(ns))
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
			}
		}

		OW_THROWCIMMSG(rval, OW_CIMObjectPath(className, ns).toString().c_str());
	}
}

void
OW_CIMRepository::checkGetClassRvalAndThrowInst(OW_CIMException::ErrNoType rval,
	const OW_String& ns, const OW_String& className)
{
	if (rval != OW_CIMException::SUCCESS)
	{
		// check whether the namespace was invalid or not
		if (rval == OW_CIMException::NOT_FOUND)
		{
			if (!m_nStore.nameSpaceExists(ns))
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
			}
			else
			{
				rval = OW_CIMException::INVALID_CLASS;
			}
		}

		OW_THROWCIMMSG(rval, OW_CIMObjectPath(className, ns).toString().c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
    class ClassNameArrayBuilder : public OW_CIMObjectPathResultHandlerIFC
    {
    public:
        ClassNameArrayBuilder(OW_StringArray& names_)
        : names(names_)
        {}

        void doHandle(const OW_CIMObjectPath& op)
        {
            names.push_back(op.getObjectName());
        }

    private:
        OW_StringArray& names;
    };
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::_validatePropagatedKeys(const OW_String& ns,
	const OW_CIMInstance& ci, const OW_CIMClass& theClass)
{
	OW_CIMObjectPathArray rv;
	OW_CIMPropertyArray kprops = theClass.getKeys();
	if(kprops.size() == 0)
	{
		return;
	}

	OW_Map<OW_String, OW_CIMPropertyArray> theMap;
	OW_Bool hasPropagatedKeys = false;

	// Look at all propagated key properties
	for(size_t i = 0; i < kprops.size(); i++)
	{
		OW_CIMQualifier cq = kprops[i].getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROPAGATED);
		if(!cq)
		{
			continue;
		}

		hasPropagatedKeys = true;
		OW_CIMValue cv = cq.getValue();
		if(!cv)
		{
			continue;
		}

		OW_String cls;
		cv.get(cls);
		if(cls.empty())
		{
			continue;
		}
		size_t idx = cls.indexOf('.');
		OW_String ppropName;
		if (idx != OW_String::npos)
		{
			ppropName = cls.substring(idx+1);
			cls = cls.substring(0,idx);
		}

		OW_CIMProperty cp = ci.getProperty(kprops[i].getName());
		if(!cp || !cp.getValue())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Cannot create instance. Propagated key field missing:"
					" %1", kprops[i].getName()).c_str());
		}
		if (!ppropName.empty())
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

	if(!hasPropagatedKeys)
	{
		return;
	}

	if(theMap.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Cannot create instance. Propagated key properties missing");
	}

	OW_CIMObjectPath op(ci);
	OW_Map<OW_String, OW_CIMPropertyArray>::iterator it = theMap.begin();
	while(it != theMap.end())
	{
		OW_String clsname = it->first;
		
        // since we don't know what class the keys refer to, we get all subclasses
        // and try calling getInstance for each to see if we can find one with
        // the matching keys.
		OW_StringArray classes = getClassChildren(m_mStore, ns,
			theClass.getName());
        classes.push_back(clsname);

        op.setKeys(it->second);
        bool found = false;
        for (size_t i = 0; i < classes.size(); ++i)
        {
            op.setObjectName(classes[i]);

            try
            {
                OW_CIMClass c = _instGetClass(ns,classes[i]);
                m_iStore.getCIMInstance(ns, op, c, false, true, true, 0);
                // if the previous line didn't throw, then we found it.
                found = true;
                break;
            }
            catch (const OW_CIMException&)
            {
            }
        }

        if (!found)
        {
            OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
                format("Propagated keys refer to non-existent object: %1",
                    op.toString()).c_str());
        }

		++it;
	}
}

const char* const OW_CIMRepository::INST_REPOS_NAME = "instances";
const char* const OW_CIMRepository::META_REPOS_NAME = "schema";
const char* const OW_CIMRepository::NS_REPOS_NAME = "namespaces";
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
const char* const OW_CIMRepository::CLASS_ASSOC_REPOS_NAME = "classassociation";
const char* const OW_CIMRepository::INST_ASSOC_REPOS_NAME = "instassociation";
#endif


