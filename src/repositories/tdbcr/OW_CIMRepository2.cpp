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
#include "OW_CIMRepository2.hpp"
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
#include "OW_Logger.hpp"
#include "OW_dbUtils.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION(CIMRepository2);

using namespace WBEMFlags;


//////////////////////////////////////////////////////////////////////////////
namespace
{
LoggerRef g_logger;
void dbErrorCallback(const char *errpfx, char *msg)
{
	// have to use the global here since we can't pass in any sort of handle into db :-(
	if (g_logger)
	{
		OW_LOG_ERROR(g_logger, msg);
	}
}
}

//////////////////////////////////////////////////////////////////////////////
CIMRepository2::CIMRepository2(ServiceEnvironmentIFCRef env)
	: RepositoryIFC()
	, m_iStore(env)
	, m_mStore(env)
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	, m_classAssocDb(env)
	, m_instAssocDb(env)
#endif
	, m_env(env)
	, m_checkReferentialIntegrity(false)
	, m_dbenv(0)
{
	g_logger = m_env->getLogger(COMPONENT_NAME);

	if (m_env->getConfigItem(ConfigOpts::CHECK_REFERENTIAL_INTEGRITY_opt,
		OW_DEFAULT_CHECK_REFERENTIAL_INTEGRITY).equalsIgnoreCase("true"))
	{
		m_checkReferentialIntegrity = true;
	}

	OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 initializing with libdb version: %1", ::db_version(0, 0, 0)));

}
//////////////////////////////////////////////////////////////////////////////
CIMRepository2::~CIMRepository2()
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
namespace
{
class DBTransactionScope
{
public:
	DBTransactionScope(::DB_ENV* env)
		: m_tid(0)
	{
		OW_ASSERT(env != 0);
		int ret = env->txn_begin(env, 0, &m_tid, 0);
		dbUtils::checkReturn(ret, "txn_begin");
	}
	~DBTransactionScope()
	{
		if (m_tid != 0)
		{
			try
			{
				abort();
			}
			catch (...)
			{
				// can't let exceptions escape
			}
		}
	}
	void commit()
	{
		OW_ASSERT(m_tid != 0);
		int ret = m_tid->commit(m_tid, 0);
		dbUtils::checkReturn(ret, "txn_commit");
		m_tid = 0;
	}
	void abort()
	{
		OW_ASSERT(m_tid != 0);
		int ret = m_tid->abort(m_tid);
		dbUtils::checkReturn(ret, "txn_abort");
		m_tid = 0;
	}

	operator ::DB_TXN*()
	{
		return m_tid;
	}

private:
	::DB_TXN* m_tid;
};
} // end unnamed namespace

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::open(const String& path)
{
	FileSystem::makeDirectory(path, 0700);
	if (!FileSystem::exists(path))
	{
		String msg("failed to create directory: " );
		msg += path;
		OW_THROW(IOException, msg.c_str());
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
	
	// Create a db environment object and initialize it for error
	// reporting.
	int ret;
	if ((ret = ::db_env_create(&m_dbenv, 0)) != 0)
	{
		OW_THROW(CIMRepository2Exception, format("::db_env_create() failed: %1(%2)", ret, ::db_strerror(ret)).c_str());
	}
	m_dbenv->set_errcall(m_dbenv, dbErrorCallback);
	m_dbenv->set_errpfx(m_dbenv, "CIMRepository2");

	// We want to specify the shared memory buffer pool cachesize,
	// but everything else is the default.
	const int size = 64 * 1024;
	if ((ret = m_dbenv->set_cachesize(m_dbenv, 0, size, 0)) != 0)
	{
		m_dbenv->close(m_dbenv, 0);
		OW_THROW(CIMRepository2Exception, format("m_dbenv->set_cachesize() failed: %1(%2)", ret, ::db_strerror(ret)).c_str());
	}

#ifdef OW_DEBUG
	m_dbenv->set_verbose(m_dbenv,DB_VERB_CHKPOINT | DB_VERB_DEADLOCK | DB_VERB_RECOVERY | DB_VERB_REPLICATION | DB_VERB_WAITSFOR, 1);
#endif


	// Open the db environment with full transactional support.
	// DB_CREATE creates the db env if this is the first time we're running.  It's also required if DB_RECOVER is specified.
	// DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN initializes the sub-systems we need for transactions.
	// DB_PRIVATE is used since we (currently) only access the db from the cimom.  Recovery has to run single-threaded and we don't have any way of
	// ensuring that if multiple processes are involved.  If the cimom is changed to use a fork() model this should be changed.
	// DB_THREAD since we are multi-threaded.  This shouldn't be removed if the cimom is changed to fork() for each connection unless all threads are removed.
	// DB_RECOVER replays the transaction logs and will clean up the db if the cimom didn't correctly shut down previously.
	ret = m_dbenv->open(m_dbenv, fname.c_str(),
		DB_CREATE |
		DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN |
		DB_PRIVATE |
		DB_THREAD |
		DB_RECOVER,
		0600);
	dbUtils::checkReturn(ret, "m_dbenv->open");

	// opens happen in the context of a transaction
	DBTransactionScope tid(m_dbenv);
	m_iStore.open(m_dbenv, tid);

	m_mStore.open(m_dbenv, tid);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	m_classAssocDb.open("classassoc", m_dbenv, tid);
	m_instAssocDb.open("instanceassoc", m_dbenv, tid);
#endif
	tid.commit();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::close()
{
	m_iStore.close();
	m_mStore.close();
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	m_classAssocDb.close();
	m_instAssocDb.close();
#endif
	if (m_dbenv)
	{
		m_dbenv->close(m_dbenv, 0);
		m_dbenv = 0;
	}
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::createNameSpace(const String& ns,
	OperationContext& context)
{
	m_mStore.createNameSpace(ns, context);

	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 created namespace: %1", ns));
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::deleteNameSpace(const String& ns,
	OperationContext& context)
{
	m_mStore.deleteNameSpace(ns, context);
	
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 deleted namespace: %1", ns));
	}
}
#endif
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::enumNameSpace(StringResultHandlerIFC& result,
	OperationContext& context)
{
	m_mStore.enumNameSpace(result, context);
	
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), "CIMRepository2 enumerated namespaces");
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
CIMRepository2::getQualifierType(const String& ns,
	const String& qualifierName,
	OperationContext&)
{
	CIMQualifierType rv = m_mStore.getQualifierType(ns, qualifierName);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 getting qualifier type: %1",
			CIMObjectPath(qualifierName,ns).toString()));
	}
	return rv;
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::enumQualifierTypes(
	const String& ns,
	CIMQualifierTypeResultHandlerIFC& result,
	OperationContext&)
{
	m_mStore.enumQualifierTypes(ns, result);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 enumerated qualifiers in namespace: %1", ns));
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::deleteQualifierType(const String& ns, const String& qualName,
	OperationContext&)
{
	// TODO: What happens if the qualifier is being used???
	if (!m_mStore.deleteQualifierType(ns, qualName))
	{
		if (m_mStore.nameSpaceExists(ns))
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
	
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 deleted qualifier type: %1 in namespace: %2", qualName, ns));
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::setQualifierType(
	const String& ns,
	const CIMQualifierType& qt, OperationContext&)
{
	m_mStore.setQualifierType(ns, qt);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 set qualifier type: %1 in "
			"namespace: %2", qt.toString(), ns));
	}
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository2::getClass(
	const String& ns, const String& className,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList,
	OperationContext&)
{
	CIMClass theClass(CIMNULL);
	CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className,
		localOnly, includeQualifiers, includeClassOrigin, propertyList,
		theClass);
	checkGetClassRvalAndThrow(rval, ns, className);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 got class: %1 from "
			"namespace: %2", theClass.getName(), ns));
	}
	return theClass;
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository2::_getClass(const String& ns, const String& className)
{
	CIMClass theClass(CIMNULL);
	CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, theClass);
	checkGetClassRvalAndThrow(rval, ns, className);
	return theClass;
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository2::_instGetClass(const String& ns, const String& className)
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
		CIMClassDeleter(MetaRepository2& mr, const String& ns_,
			InstanceRepository2& mi
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			, AssocDb2& m_assocDb_
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
			String cname = c.getName();
			// TODO: this doesn't work quite right. Fix deleteInstance to also delete
			// associations, then we could just call enumInstances and then
			// deleteInstance for all instances.
			// delete any instances of the class
			//m_iStore.deleteClass(ns, cname);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
			// remove class from association index
			if (c.isAssociation())
			{
				m_assocDb.deleteEntries(ns,c);
			}
#endif
			if (!m_mStore.deleteClass(ns, cname))
			{
				OW_THROWCIM(CIMException::NOT_FOUND);
			}
		}
	private:
		MetaRepository2& m_mStore;
		const String& ns;
		InstanceRepository2& m_iStore;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		AssocDb2& m_assocDb;
#endif
	};
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository2::deleteClass(const String& ns, const String& className,
	OperationContext& acl)
{
	CIMClass cc = _getClass(ns, className);
	OW_ASSERT(cc);
	// TODO: delete associations to the instances we delete
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
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 deleted class: %1:%2",
			ns, className));
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::createClass(const String& ns, const CIMClass& cimClass_,
	OperationContext&)
{
	// m_mStore.createClass modifies cimClass to be consistent with base
	// classes, etc.
	CIMClass cimClass(cimClass_);
	m_mStore.createClass(ns, cimClass);
	// we need to re-get the class, so that it will be consistent.  currently
	// cimClass only contains "unique" items that are added in the child class.
	cimClass = _getClass(ns, cimClass.getName());
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	if (cimClass.isAssociation())
	{
		m_classAssocDb.addEntries(ns,cimClass);
	}
#endif
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("Created class: %1:%2", ns, cimClass.toMOF()));
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
CIMRepository2::modifyClass(
	const String& ns,
	const CIMClass& cc,
	OperationContext&)
{
	OW_ASSERT(cc);
	CIMClass origClass = _getClass(ns, cc.getName());
	// TODO: this needs to update the subclasses of the modified class.
	//			If that's not possible, then we need to throw a
	//			CLASS_HAS_CHILDREN CIMException.
	// TODO: Need to update the instances of the class and any subclasses.
	//			If that's not possible, then we need to throw a
	//			CLASS_HAS_INSTANCES CIMException.
	m_mStore.modifyClass(ns, cc);
	OW_ASSERT(origClass);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("Modified class: %1:%2 from %3 to %4", ns,
			cc.getName(), origClass.toMOF(), cc.toMOF()));
	}
	return origClass;
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::enumClasses(const String& ns,
		const String& className,
		CIMClassResultHandlerIFC& result,
		EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin, OperationContext&)
{
	m_mStore.enumClass(ns, className,
		result, deep,
		localOnly, includeQualifiers, includeClassOrigin);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 enumerated classes: %1:%2", ns,
			className));
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::enumClassNames(
	const String& ns,
	const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep, OperationContext&)
{
	m_mStore.enumClassNames(ns, className, result, deep);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 enumerated class names: %1:%2", ns,
			className));
	}
}
//////////////////////////////////////////////////////////////////////////////
namespace {
class stringArrayBuilder : public StringResultHandlerIFC
{
public:
	stringArrayBuilder(StringArray& result)
		: m_result(result)
	{}
	void doHandle(const String& name)
	{
		m_result.push_back(name);
	}
private:
	StringArray& m_result;
};
// utility function
StringArray getClassChildren(MetaRepository2& rep, const String& ns, const String& clsName)
{
	StringArray result;
	stringArrayBuilder handler(result);
	rep.enumClassNames(ns, clsName, handler, E_DEEP);
	return result;
}
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::enumInstanceNames(
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	EDeepFlag deep,
	OperationContext&)
{
	CIMClass theClass = _instGetClass(ns, className);
	m_iStore.getInstanceNames(ns, theClass, result);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 enumerated instance names: %1:%2", ns,
			className));
	}
	if (!deep)
	{
		return;
	}
	// This code probably won't ever be executed, because CIMServer
	// has to do each class at a time because of providers, and will
	// thus only call us with deep=false.
	// If the situation ever changes, fix and enable the code below.
	OW_THROWCIMMSG(CIMException::FAILED, "Internal server error");
	// TODO: Switch this to use a callback interface.
	/*
	StringArray classNames = m_mStore.getClassChildren(ns,
		theClass.getName());
	for (size_t i = 0; i < classNames.size(); i++)
	{
		theClass = _instGetClass(ns, classNames[i]);
		m_iStore.getInstanceNames(ns, theClass, result);
		if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
		{
			OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 enumerated derived instance names: %1:%2", ns,
				classNames[i]));
		}
	}
	*/
}
//////////////////////////////////////////////////////////////////////////////
namespace {
class instEnumerator : public StringResultHandlerIFC
{
public:
	instEnumerator(CIMRepository2& rep_,
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
		if (rep.m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
		{
			OW_LOG_DEBUG(rep.m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 Enumerated derived instances: %1:%2", ns, className));
		}
	}
private:
	CIMRepository2& rep;
	const String& ns;
	const CIMClass& theTopClass;
	CIMInstanceResultHandlerIFC& result;
	EDeepFlag deep;
	ELocalOnlyFlag localOnly;
	EIncludeQualifiersFlag includeQualifiers;
	EIncludeClassOriginFlag includeClassOrigin;
	const StringArray* propertyList;
};
} // end unnamed namespace
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::enumInstances(
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
	CIMClass theTopClass = _instGetClass(ns, className);
	m_iStore.getCIMInstances(ns, className, theTopClass, theTopClass, result,
		deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
	
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2 Enumerated instances: %1:%2", ns,
			className));
	}
	if (enumSubclasses)
	{
		instEnumerator ie(*this, ns, theTopClass, result, deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
		m_mStore.enumClassNames(ns, className, ie, E_DEEP);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMRepository2::getInstance(
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
CIMRepository2::getInstance(
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
	ci = m_iStore.getCIMInstance(ns, instanceName, cc, localOnly,
		includeQualifiers, includeClassOrigin, propertyList);
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
CIMRepository2::deleteInstance(const String& ns, const CIMObjectPath& cop_,
	OperationContext& acl)
{
	CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2::deleteInstance.  cop = %1",
			cop.toString()));
	}
	CIMClass theClass(CIMNULL);
	CIMInstance oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		&theClass, acl);
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	// Ensure no associations exist for this instance
	if (m_instAssocDb.hasAssocEntries(ns, cop))
	{
		// TODO: Revisit this.  Instead of throwing, it is allowed in the
		// spec to to delete the associations that reference the instance.
		// See http://dmtf.org/standards/documents/WBEM/DSP200.html
		//   2.3.2.4. DeleteInstance
		OW_THROWCIMMSG(CIMException::FAILED,
			format("Instance %1 has associations", cop.toString()).c_str());
	}
	// TODO: It would be good to check for Min(1) relationships to the
	// instance.
	
	// If we're deleting an association instance, then remove all
	// traces of it in the association database.
	if (theClass.isAssociation())
	{
		m_instAssocDb.deleteEntries(ns, oldInst);
	}
#endif
	// Delete the instance from the instance repository
	m_iStore.deleteInstance(ns, cop, theClass);
	OW_ASSERT(oldInst);
	return oldInst;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPath
CIMRepository2::createInstance(
	const String& ns,
	const CIMInstance& ci,
	OperationContext&)
{
	CIMObjectPath rval(ns, ci);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("CIMRepository2::createInstance.  ns = %1, "
			"instance = %2", ns, ci.toMOF()));
	}
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
					rcc = _instGetClass(ns,op.getClassName());
					m_iStore.getCIMInstance(ns, op,rcc,E_NOT_LOCAL_ONLY,E_INCLUDE_QUALIFIERS,E_INCLUDE_CLASS_ORIGIN,0);
				}
				catch (CIMException&)
				{
					OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
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
	if (theClass.isAssociation())
	{
		m_instAssocDb.addEntries(ns, ci);
	}
#endif
	OW_ASSERT(rval);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
CIMRepository2::modifyInstance(
	const String& ns,
	const CIMInstance& modifiedInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	OperationContext& acl)
{
	CIMClass theClass(CIMNULL);
	CIMObjectPath cop(ns, modifiedInstance);
	CIMInstance oldInst = getInstance(ns, cop, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, NULL,
		&theClass, acl);
	//TODO: _checkRequiredProperties(theClass, modifiedInstance);
	m_iStore.modifyInstance(ns, cop, theClass, modifiedInstance, oldInst, includeQualifiers, propertyList);
	// No need to update the assoc db.  All references are keys, and thus can't be changed.
	OW_ASSERT(oldInst);
	return oldInst;
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::setProperty(
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
	StringArray props;
	props.push_back(propertyName);
	modifyInstance(ns, ci, E_INCLUDE_QUALIFIERS, &props, context);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMRepository2::getProperty(
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
//////////////////////////////////////////////////////////////////////////////
CIMValue
CIMRepository2::invokeMethod(
	const String&,
	const CIMObjectPath&,
	const String&, const CIMParamValueArray&,
	CIMParamValueArray&, OperationContext&)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED);
}
//////////////////////////////////////////////////////////////////////
void
CIMRepository2::execQuery(
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
CIMRepository2::associators(
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
CIMRepository2::associatorsClasses(
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
CIMRepository2::associatorNames(
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
CIMRepository2::references(
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
CIMRepository2::referencesClasses(
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
CIMRepository2::referenceNames(
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
					format("class %1 is not an association", cc.getName()).c_str());
			}
			Assocs.append(cc);
		}
	private:
		CIMClassArray& Assocs;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::_commonReferences(
	const String& ns,
	const CIMObjectPath& path_,
	const String& resultClass, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult, OperationContext& context)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	if (!m_mStore.nameSpaceExists(ns))
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
	SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());
	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		_staticReferencesClass(path,
			resultClass.empty() ? 0 : &resultClassNamesSet,
			role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, context);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticReferences(path,
				resultClass.empty() ? 0 : &resultClassNamesSet, role,
				includeQualifiers, includeClassOrigin, propertyList, *piresult, context);
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
	class staticReferencesObjectPathResultHandler : public AssocDbEntry2ResultHandlerIFC
	{
	public:
		staticReferencesObjectPathResultHandler(
			CIMObjectPathResultHandlerIFC& result_)
		: result(result_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry2::entry &e)
		{
			result.handle(e.m_associationPath);
		}
	private:
		CIMObjectPathResultHandlerIFC& result;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticReferencesClassResultHandler : public AssocDbEntry2ResultHandlerIFC
	{
	public:
		staticReferencesClassResultHandler(
			CIMClassResultHandlerIFC& result_,
			CIMRepository2& server_,
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
		virtual void doHandle(const AssocDbEntry2::entry &e)
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
		CIMRepository2& server;
		String& ns;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propList;
		OperationContext& context;
	};
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsInstResultHandler : public AssocDbEntry2ResultHandlerIFC
	{
	public:
		staticAssociatorsInstResultHandler(
			OperationContext& intAclInfo_,
			CIMRepository2& server_,
			CIMInstanceResultHandlerIFC& result_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propertyList_)
		: intAclInfo(intAclInfo_)
		, server(server_)
		, result(result_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry2::entry &e)
		{
			CIMObjectPath op = e.m_associatedObject;
			CIMInstance ci = server.getInstance(op.getNameSpace(), op, E_NOT_LOCAL_ONLY,
				includeQualifiers,includeClassOrigin,propertyList,intAclInfo);
			result.handle(ci);
		}
	private:
		OperationContext& intAclInfo;
		CIMRepository2& server;
		CIMInstanceResultHandlerIFC& result;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propertyList;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticReferencesInstResultHandler : public AssocDbEntry2ResultHandlerIFC
	{
	public:
		staticReferencesInstResultHandler(OperationContext& intAclInfo_,
			CIMRepository2& server_,
			CIMInstanceResultHandlerIFC& result_,
			EIncludeQualifiersFlag includeQualifiers_,
			EIncludeClassOriginFlag includeClassOrigin_,
			const StringArray* propertyList_)
		: intAclInfo(intAclInfo_)
		, server(server_)
		, result(result_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propertyList(propertyList_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry2::entry &e)
		{
			CIMObjectPath op = e.m_associationPath;
			CIMInstance ci = server.getInstance(op.getNameSpace(), op, E_NOT_LOCAL_ONLY,
				includeQualifiers,includeClassOrigin,propertyList,intAclInfo);
			result.handle(ci);
		}
	private:
		OperationContext& intAclInfo;
		CIMRepository2& server;
		CIMInstanceResultHandlerIFC& result;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propertyList;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::_staticReferences(const CIMObjectPath& path,
	const SortedVectorSet<String>* refClasses, const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC& result,
	OperationContext& context)
{
	staticReferencesInstResultHandler handler(context, *this, result,
		includeQualifiers, includeClassOrigin, propertyList);
	m_instAssocDb.getAllEntries(path,
		refClasses, 0, role, String(), handler);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::_staticReferenceNames(const CIMObjectPath& path,
	const SortedVectorSet<String>* refClasses, const String& role,
	CIMObjectPathResultHandlerIFC& result)
{
	staticReferencesObjectPathResultHandler handler(result);
	m_instAssocDb.getAllEntries(path,
		refClasses, 0, role, String(), handler);
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::_commonAssociators(
	const String& ns,
	const CIMObjectPath& path_,
	const String& assocClassName, const String& resultClass,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	CIMInstanceResultHandlerIFC* piresult,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	CIMObjectPath path(path_);
	path.setNameSpace(ns);
	if (!m_mStore.nameSpaceExists(ns))
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	// Get association classes from the association repository
	CIMClassArray Assocs;
	assocClassBuilder assocClassResult(Assocs);
	_getAssociationClasses(ns, assocClassName, path.getClassName(), assocClassResult, role, context);
	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	StringArray resultClassNames;
	if (!resultClass.empty())
	{
		resultClassNames = getClassChildren(m_mStore, ns, resultClass);
		resultClassNames.append(resultClass);
	}
	StringArray assocClassNames;
	for (size_t i = 0; i < Assocs.size(); i++)
	{
		assocClassNames.append(Assocs[i].getName());
	}
	SortedVectorSet<String> assocClassNamesSet(assocClassNames.begin(),
			assocClassNames.end());
	SortedVectorSet<String> resultClassNamesSet(resultClassNames.begin(),
			resultClassNames.end());
	if (path.isClassPath())
	{
		// Process all of the association classes without providers
		_staticAssociatorsClass(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
			resultClass.empty() ? 0 : &resultClassNamesSet,
			role, resultRole, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult, context);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticAssociators(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
				resultClass.empty() ? 0 : &resultClassNamesSet, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList, *piresult, context);
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
CIMRepository2::_staticAssociators(const CIMObjectPath& path,
	const SortedVectorSet<String>* passocClasses,
	const SortedVectorSet<String>* presultClasses,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMInstanceResultHandlerIFC& result,
	OperationContext& context)
{
	staticAssociatorsInstResultHandler handler(context, *this, result,
		includeQualifiers, includeClassOrigin, propertyList);
	m_instAssocDb.getAllEntries(path,
		passocClasses, presultClasses, role, resultRole, handler);
		
}
namespace
{
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsObjectPathResultHandler : public AssocDbEntry2ResultHandlerIFC
	{
	public:
		staticAssociatorsObjectPathResultHandler(
			CIMObjectPathResultHandlerIFC& result_)
		: result(result_)
		{}
	protected:
		virtual void doHandle(const AssocDbEntry2::entry &e)
		{
			result.handle(e.m_associatedObject);
		}
	private:
		CIMObjectPathResultHandlerIFC& result;
	};
	
//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsClassResultHandler : public AssocDbEntry2ResultHandlerIFC
	{
	public:
		staticAssociatorsClassResultHandler(
			CIMClassResultHandlerIFC& result_,
			CIMRepository2& server_,
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
		virtual void doHandle(const AssocDbEntry2::entry &e)
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
		CIMRepository2& server;
		String& ns;
		EIncludeQualifiersFlag includeQualifiers;
		EIncludeClassOriginFlag includeClassOrigin;
		const StringArray* propList;
		OperationContext& context;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::_staticAssociatorNames(const CIMObjectPath& path,
	const SortedVectorSet<String>* passocClasses,
	const SortedVectorSet<String>* presultClasses,
	const String& role, const String& resultRole,
	CIMObjectPathResultHandlerIFC& result)
{
	staticAssociatorsObjectPathResultHandler handler(result);
	m_instAssocDb.getAllEntries(path,
		passocClasses, presultClasses, role, resultRole, handler);
		
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::_staticAssociatorsClass(
	const CIMObjectPath& path,
	const SortedVectorSet<String>* assocClassNames,
	const SortedVectorSet<String>* resultClasses,
	const String& role, const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	// need to run the query for every superclass of the class arg.
	String curClsName = path.getClassName();
	CIMObjectPath curPath = path;
	while (!curClsName.empty())
	{
		if (popresult != 0)
		{
			staticAssociatorsObjectPathResultHandler handler(*popresult);
			m_classAssocDb.getAllEntries(curPath, assocClassNames, resultClasses, role, resultRole,
				handler);
		}
		else if (pcresult != 0)
		{
			String ns = path.getNameSpace();
			staticAssociatorsClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, context);
			m_classAssocDb.getAllEntries(curPath, assocClassNames, resultClasses, role, resultRole,
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
CIMRepository2::_staticReferencesClass(const CIMObjectPath& path,
	const SortedVectorSet<String>* resultClasses,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	CIMObjectPathResultHandlerIFC* popresult,
	CIMClassResultHandlerIFC* pcresult,
	OperationContext& context)
{
	// need to run the query for every superclass of the class arg.
	String curClsName = path.getClassName();
	CIMObjectPath curPath = path;
	while (!curClsName.empty())
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), format("curPath = %1", curPath.toString()));
		if (popresult != 0)
		{
			staticReferencesObjectPathResultHandler handler(*popresult);
			m_classAssocDb.getAllEntries(curPath, resultClasses, 0, role, String(),
				handler);
		}
		else if (pcresult != 0)
		{
			String ns = path.getNameSpace();
			staticReferencesClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, context);
			m_classAssocDb.getAllEntries(curPath, resultClasses, 0, role, String(),
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
			MetaRepository2& m_mStore_,
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
		MetaRepository2& m_mStore;
		const String& ns;
	};
}
//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::_getAssociationClasses(const String& ns,
		const String& assocClassName, const String& className,
		CIMClassResultHandlerIFC& result, const String& role,
		OperationContext& context)
{
	if (!assocClassName.empty())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		m_mStore.enumClass(ns, assocClassName, result, E_DEEP, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN);
		CIMClass cc(CIMNULL);
		CIMException::ErrNoType rc = m_mStore.getCIMClass(ns, assocClassName, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0, cc);
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
CIMRepository2::checkGetClassRvalAndThrow(CIMException::ErrNoType rval,
	const String& ns, const String& className)
{
	if (rval != CIMException::SUCCESS)
	{
		// check whether the namespace was invalid or not
		if (rval == CIMException::NOT_FOUND)
		{
			if (!m_mStore.nameSpaceExists(ns))
			{
				OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
			}
		}
		OW_THROWCIMMSG(rval, CIMObjectPath(className, ns).toString().c_str());
	}
}
void
CIMRepository2::checkGetClassRvalAndThrowInst(CIMException::ErrNoType rval,
	const String& ns, const String& className)
{
	if (rval != CIMException::SUCCESS)
	{
		// check whether the namespace was invalid or not
		if (rval == CIMException::NOT_FOUND)
		{
			if (!m_mStore.nameSpaceExists(ns))
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
CIMRepository2::_validatePropagatedKeys(const String& ns,
	const CIMInstance& ci, const CIMClass& theClass)
{
	CIMObjectPathArray rv;
	CIMPropertyArray kprops = theClass.getKeys();
	if (kprops.size() == 0)
	{
		return;
	}
	Map<String, CIMPropertyArray> theMap;
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
		String ppropName;
		if (idx != String::npos)
		{
			ppropName = cls.substring(idx+1);
			cls = cls.substring(0,idx);
		}
		CIMProperty cp = ci.getProperty(kprops[i].getName());
		if (!cp || !cp.getValue())
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
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
	Map<String, CIMPropertyArray>::iterator it = theMap.begin();
	while (it != theMap.end())
	{
		String clsname = it->first;
		
		// since we don't know what class the keys refer to, we get all subclasses
		// and try calling getInstance for each to see if we can find one with
		// the matching keys.
		StringArray classes = getClassChildren(m_mStore, ns,
			theClass.getName());
		classes.push_back(clsname);
		op.setKeys(it->second);
		bool found = false;
		for (size_t i = 0; i < classes.size(); ++i)
		{
			op.setClassName(classes[i]);
			try
			{
				CIMClass c = _instGetClass(ns,classes[i]);
				m_iStore.getCIMInstance(ns, op, c, E_NOT_LOCAL_ONLY, E_INCLUDE_QUALIFIERS, E_INCLUDE_CLASS_ORIGIN, 0);
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
				format("Propagated keys refer to non-existent object: %1",
					op.toString()).c_str());
		}
		++it;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::beginOperation(WBEMFlags::EOperationFlag op, OperationContext& context)
{
}

//////////////////////////////////////////////////////////////////////////////
void
CIMRepository2::endOperation(WBEMFlags::EOperationFlag op, OperationContext& context, WBEMFlags::EOperationResultFlag result)
{
}

} // end namespace OpenWBEM

