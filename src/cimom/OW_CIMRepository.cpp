/*******************************************************************************
* Copyright (C) 2002 Caldera International, Inc All rights reserved.
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
// TODO: Filter these down.
#include "OW_CIMRepository.hpp"
#include "OW_FileSystem.hpp"
#include "OW_RepositoryStreams.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_CIM.hpp"
#include "OW_Assertion.hpp"
#include "OW_IOException.hpp"
#include "OW_CIMParamValue.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_CIMRepository::OW_CIMRepository(OW_CIMOMEnvironmentRef env)
	: OW_RepositoryIFC()
	, m_nStore(env)
	, m_iStore(env)
	, m_mStore(env)
	, m_classAssocDb(env)
	, m_instAssocDb(env)
	, m_env(env)
{
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
	m_classAssocDb.open(fname + CLASS_ASSOC_REPOS_NAME);
	m_instAssocDb.open(fname + INST_ASSOC_REPOS_NAME);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::close()
{
	m_nStore.close();
	m_iStore.close();
	m_mStore.close();
	m_classAssocDb.close();
	m_instAssocDb.close();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::createNameSpace(const OW_String& ns,
	const OW_ACLInfo&)
{
	if(ns.empty())
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	OW_StringArray nameComps = ns.tokenize("/");

	OW_String parns;
	size_t sz = nameComps.size() - 1;
	for(size_t i = 0; i < sz; i++)
	{
		if(!parns.empty())
		{
			parns += "/";
		}

		parns += nameComps[i].toString();
	}

	if(m_nStore.createNameSpace(nameComps) == -1)
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS,
			ns.c_str());
	}

	// TODO: Make this exception safe.
	m_iStore.createNameSpace(nameComps, true);
	m_mStore.createNameSpace(nameComps, true);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMRepository created namespace: %1", ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::deleteNameSpace(const OW_String& ns,
	const OW_ACLInfo&)
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
		m_env->logDebug(format("OW_CIMRepository deleted namespace: %1", ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMRepository::getQualifierType(const OW_String& ns,
	const OW_String& qualifierName,
	const OW_ACLInfo&)
{
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMRepository getting qualifier type: %1",
			OW_CIMObjectPath(qualifierName,ns).toString()));
	}

	return m_mStore.getQualifierType(ns, qualifierName);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result,
	const OW_ACLInfo&)
{
	m_mStore.enumQualifierTypes(ns, result);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMRepository enumerated qualifiers in namespace: %1", ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumNameSpace(const OW_String& nsName,
	OW_StringResultHandlerIFC& result, OW_Bool deep,
	const OW_ACLInfo&)
{
	// TODO: Move this into m_nStore
	OW_HDBHandleLock hdl(&m_nStore, m_nStore.getHandle());

	OW_HDBNode nsNode = m_nStore.getNameSpaceNode(hdl, nsName);
	if(!nsNode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, nsName.c_str());
	}

	nsNode = hdl->getFirstChild(nsNode);

	while(nsNode)
	{
		if(!deep)
		{
			result.handle(nsNode.getKey());
		}
		else
		{
			_enumNamespaceDeep(hdl.getHandle(), result, nsNode);
		}
		nsNode = hdl->getNextSibling(nsNode);
	}

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMRepository enumerated namespace: %1", nsName));
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMRepository::_enumNamespaceDeep(OW_HDBHandle hdl, OW_StringResultHandlerIFC& result,
	OW_HDBNode node)
{
	result.handle(node.getKey());
	node = hdl.getFirstChild(node);
	while(node)
	{
		_enumNamespaceDeep(hdl, result, node);
		node = hdl.getNextSibling(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::deleteQualifierType(const OW_String& ns, const OW_String& qualName,
	const OW_ACLInfo&)
{
	// TODO: What happens if the qualifier is being used???
	if(!m_mStore.deleteQualifierType(ns, qualName))
	{
		if (m_mStore.nameSpaceExists(ns))
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
		m_env->logDebug(format("OW_CIMRepository deleted qualifier type: %1 in namespace: %2", qualName, ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::setQualifierType(
	const OW_String& ns,
	const OW_CIMQualifierType& qt, const OW_ACLInfo&)
{
	m_mStore.setQualifierType(ns, qt);
	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMRepository set qualifier type: %1 in "
			"namespace: %2", qt.toString(), ns));
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMRepository::getClass(
	const OW_String& ns, const OW_String& className, 
	OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
	const OW_ACLInfo&)
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
			m_env->logDebug(format("OW_CIMRepository got class: %1 from "
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

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassDeleter : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassDeleter(OW_MetaRepository& mr, const OW_String& ns_,
			OW_InstanceRepository& mi, OW_AssocDb& m_assocDb_)
		: m_mStore(mr)
		, ns(ns_)
		, m_iStore(mi)
		, m_assocDb(m_assocDb_)
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

			// remove class from association index
			if (c.isAssociation())
			{
				OW_AssocDbHandle hdl = m_assocDb.getHandle();
				hdl.deleteEntries(ns,c);
			}

		}
	private:
		OW_MetaRepository& m_mStore;
		const OW_String& ns;
		OW_InstanceRepository& m_iStore;
		OW_AssocDb& m_assocDb;
	};
}
//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMRepository::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_ACLInfo& acl)
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
		CIMClassDeleter ccd(m_mStore, ns, m_iStore, m_classAssocDb);
		this->enumClasses(ns, className, ccd,
			OW_CIMOMHandleIFC::DEEP, OW_CIMOMHandleIFC::LOCAL_ONLY,
			OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS,
			OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN,
            acl);
		ccd.handle(cc);

		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMRepository deleted class: %1 in "
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
OW_CIMRepository::createClass(const OW_String& ns, const OW_CIMClass& cimClass,
	const OW_ACLInfo&)
{
	try
	{
		m_mStore.createClass(ns, cimClass);
		m_iStore.createClass(ns, cimClass);
		if (cimClass.isAssociation())
		{
			OW_AssocDbHandle hdl = m_classAssocDb.getHandle();
			hdl.addEntries(ns,cimClass);
		}

		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("Created class: %1:%2", ns, cimClass.toMOF()));
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
	const OW_ACLInfo&)
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
			m_env->logDebug(format("Modified class: %1:%2 from %3 to %4", ns,
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

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo&)
{
	try
	{
		m_mStore.enumClass(ns, className,
			result, deep,
			localOnly, includeQualifiers, includeClassOrigin);
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMRepository enumerated classes: %1:%2", ns,
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
namespace
{
	class CIMClassToCIMObjectPathHandler : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassToCIMObjectPathHandler(OW_CIMObjectPathResultHandlerIFC& oph_,
			OW_CIMObjectPath& lcop_)
		: oph(oph_)
		, lcop(lcop_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &c)
		{
			// namespace is already set on lcop
			lcop.setObjectName(c.getName());
			oph.handle(lcop);
		}
	private:
		OW_CIMObjectPathResultHandlerIFC& oph;
		OW_CIMObjectPath& lcop;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::enumClassNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep, const OW_ACLInfo&)
{
	try
	{
		// TODO: Make this a bit more efficient. getClassChildren()?
		OW_CIMObjectPath lcop(className, ns);
		CIMClassToCIMObjectPathHandler handler(result,lcop);
		m_mStore.enumClass(ns, className, handler,
			deep, false, true, true);
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMRepository enumerated class names: %1:%2", ns,
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
void
OW_CIMRepository::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep,
	const OW_ACLInfo&)
{
	try
	{
		OW_CIMClass theClass = _instGetClass(ns, className);

		m_iStore.getInstanceNames(ns, theClass, result);

		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMRepository enumerated instance names: %1:%2", ns,
				className));
		}
		if(!deep)
		{
			return;
		}

		// This code probably won't ever be executed, because CIMServer
		// has to do each class at a time because of providers, and will
		// thus only call us with deep=false.

		// TODO: Switch this to use a callback interface.
		OW_StringArray classNames = m_mStore.getClassChildren(ns,
			theClass.getName());

		for(size_t i = 0; i < classNames.size(); i++)
		{
			theClass = _instGetClass(ns, classNames[i]);
			m_iStore.getInstanceNames(ns, theClass, result);
			if (m_env->getLogger()->getLogLevel() == DebugLevel)
			{
				m_env->logDebug(format("OW_CIMRepository enumerated derived instance names: %1:%2", ns,
					classNames[i]));
			}
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
void
OW_CIMRepository::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_Bool enumSubClasses, const OW_ACLInfo&)
{
	// deep means a different thing here than for enumInstanceNames.  See the spec.
	try
	{
		OW_CIMClass theTopClass = _instGetClass(ns, className);

		m_iStore.getCIMInstances(ns, className, theTopClass, theTopClass, result,
			deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
		
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMRepository Enumerated instances: %1:%2", ns,
				className));
		}

		if (enumSubClasses)
		{
			// TODO: Switch this to use the callback interface.
			OW_StringArray classNames = m_mStore.getClassChildren(ns,
				className);

			for(size_t i = 0; i < classNames.size(); i++)
			{
				OW_CIMClass theClass = _instGetClass(ns, classNames[i]);

				m_iStore.getCIMInstances(ns, classNames[i], theTopClass, theClass, result,
					deep, localOnly, includeQualifiers, includeClassOrigin, propertyList);
				if (m_env->getLogger()->getLogLevel() == DebugLevel)
				{
					m_env->logDebug(format("OW_CIMRepository Enumerated derived instances: %1:%2", ns,
						classNames[i]));
				}
			}
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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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
	const OW_ACLInfo&)
{
	OW_StringArray lpropList;
	if(propertyList)
	{
		lpropList = *propertyList;
	}

	OW_CIMInstance ci;

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

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMRepository::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop_,
	const OW_ACLInfo& acl)
{
	OW_CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);

	if (m_env->getLogger()->getLogLevel() == DebugLevel)
	{
		m_env->logDebug(format("OW_CIMRepository::deleteInstance.  cop = %1",
			cop.toString()));
	}

	try
	{
		OW_CIMClass theClass(OW_CIMNULL);
		OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
			&theClass, acl);

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
	const OW_ACLInfo&)
{
	OW_CIMObjectPath rval(ci);

	try
	{
		if (m_env->getLogger()->getLogLevel() == DebugLevel)
		{
			m_env->logDebug(format("OW_CIMRepository::createInstance.  ns = %1, "
				"instance = %2", ns, ci.toMOF()));
		}

		OW_CIMClass theClass = _instGetClass(ns, ci.getClassName());

		//_checkRequiredProperties(theClass, ci);
		m_iStore.createInstance(ns, theClass, ci);

		if(theClass.isAssociation())
		{
			OW_AssocDbHandle hdl = m_instAssocDb.getHandle();
			hdl.addEntries(ns, ci);
		}

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
	const OW_ACLInfo& acl)
{
	try
	{
		OW_CIMClass theClass(OW_CIMNULL);
		OW_CIMObjectPath cop(modifiedInstance);
		OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
			&theClass, acl);

		//_checkRequiredProperties(theClass, modifiedInstance);

		m_iStore.modifyInstance(ns, cop, theClass, modifiedInstance, oldInst, includeQualifiers, propertyList);

		// TODO: Verify that this code is needed.  Aren't all references keys, and thus can't be changed?  So why update the assoc db?
		// just create a test to try and break it.
		if(theClass.isAssociation())
		{
			OW_AssocDbHandle adbHdl = m_instAssocDb.getHandle();
			adbHdl.deleteEntries(ns, oldInst);
			adbHdl.addEntries(ns, modifiedInstance);
		}

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
OW_CIMValue
OW_CIMRepository::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_ACLInfo& aclInfo)
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
void
OW_CIMRepository::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& valueArg,
	const OW_ACLInfo& aclInfo)
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

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMRepository::invokeMethod(
	const OW_String&,
	const OW_CIMObjectPath&,
	const OW_String&, const OW_CIMParamValueArray&,
	OW_CIMParamValueArray&, const OW_ACLInfo&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::execQuery(
	const OW_String&,
	OW_CIMInstanceResultHandlerIFC&,
	const OW_String&,
	const OW_String&, const OW_ACLInfo&)
{
	OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMRepository::associators(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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
	const OW_ACLInfo& aclInfo)
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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
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
	const OW_ACLInfo& aclInfo)
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
	OW_CIMClassResultHandlerIFC* pcresult, const OW_ACLInfo& aclInfo)
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

	if (path.getKeys().size() == 0)
	{
		// it's a class path
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
		virtual void doHandle(const OW_AssocDbEntry &e)
		{
			result.handle(e.getAssociationPath());
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
			const OW_ACLInfo& aclInfo_)
		: result(result_)
		, server(server_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, aclInfo(aclInfo_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry &e)
		{
			OW_CIMObjectPath cop = e.getAssociationPath();
			/* I don't think we need to do this, since a namespace should
			   never be empty, it defaults to "root"
			if (cop.getNameSpace().empty())
			{
				cop.setNameSpace(ns);
			}
			*/
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
		const OW_ACLInfo& aclInfo;
	};

//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsInstResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsInstResultHandler(const OW_ACLInfo& intAclInfo_,
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
		virtual void doHandle(const OW_AssocDbEntry &e)
		{
			OW_CIMObjectPath op = e.getAssociatedObject();
			OW_CIMInstance ci = server.getInstance(op.getNameSpace(), op, false,
				includeQualifiers,includeClassOrigin,propertyList,intAclInfo);
			result.handle(ci);
		}
	private:
		const OW_ACLInfo& intAclInfo;
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
		staticReferencesInstResultHandler(const OW_ACLInfo& intAclInfo_,
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
		virtual void doHandle(const OW_AssocDbEntry &e)
		{
			OW_CIMObjectPath op = e.getAssociationPath();

			OW_CIMInstance ci = server.getInstance(op.getNameSpace(), op, false,
				includeQualifiers,includeClassOrigin,propertyList,intAclInfo);
			result.handle(ci);
		}
	private:
		const OW_ACLInfo& intAclInfo;
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
	const OW_ACLInfo& aclInfo)
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
	const OW_ACLInfo& aclInfo)
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
		resultClassNames = m_mStore.getClassChildren(ns, resultClass);
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

	if (path.getKeys().size() == 0)
	{
		// it's a class path
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
	const OW_ACLInfo& aclInfo)
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
		virtual void doHandle(const OW_AssocDbEntry &e)
		{
			result.handle(e.getAssociatedObject());
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
			const OW_ACLInfo& aclInfo_)
		: result(result_)
		, server(server_)
		, ns(ns_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, aclInfo(aclInfo_)
		{}
	protected:
		virtual void doHandle(const OW_AssocDbEntry &e)
		{
			OW_CIMObjectPath cop = e.getAssociatedObject();
			/* I don't think we need to do this, since a namespace should
			   never be empty, it defaults to "root"
			if (cop.getNameSpace().empty())
			{
				cop.setNameSpace(ns);
			}
			*/
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
		const OW_ACLInfo& aclInfo;
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
	const OW_ACLInfo& aclInfo)
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
	const OW_ACLInfo& aclInfo)
{
	OW_AssocDbHandle dbhdl = m_classAssocDb.getHandle();

	// need to run the query for every superclass of the class arg.
	OW_String curClsName = path.getObjectName();
	OW_CIMObjectPath curPath = path;
	while (!curClsName.empty())
	{
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
		const OW_ACLInfo& aclInfo)
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

const char* const OW_CIMRepository::INST_REPOS_NAME = "instances";
const char* const OW_CIMRepository::META_REPOS_NAME = "schema";
const char* const OW_CIMRepository::NS_REPOS_NAME = "namespaces";
const char* const OW_CIMRepository::CLASS_ASSOC_REPOS_NAME = "classassociation";
const char* const OW_CIMRepository::INST_ASSOC_REPOS_NAME = "instassociation";

