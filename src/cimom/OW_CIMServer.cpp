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
#include "OW_CIMServer.hpp"
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

class OW_AccessMgr
{
public:
	enum
	{
		GETCLASS,
		GETINSTANCE,
		DELETECLASS,
		DELETEINSTANCE,
		CREATECLASS,
		CREATEINSTANCE,
		MODIFYCLASS,
		MODIFYINSTANCE,
		ENUMERATECLASSES,
		ENUMERATECLASSNAMES,
		ENUMERATEINSTANCES,
		ENUMERATEINSTANCENAMES,
		ASSOCIATORS,
		ASSOCIATORNAMES,
		REFERENCES,
		REFERENCENAMES,
		GETPROPERTY,
		SETPROPERTY,
		GETQUALIFIER,
		SETQUALIFIER,
		DELETEQUALIFIER,
		ENUMERATEQUALIFIERS,
		CREATENAMESPACE,
		DELETENAMESPACE,
		ENUMERATENAMESPACE,
		INVOKEMETHOD
	};

	OW_AccessMgr(OW_CIMServer* pServer, OW_CIMOMEnvironmentRef env);

	/**
	 * checkAccess will check that access is granted through the ACL. If
	 * Access is not granted, an OW_CIMException will be thrown.
	 * @param op	The operation that access is being checked for.
	 * @param ns	The name space that access is being check on.
	 * @param aclInfo The ACL info for the user request.
	 */
	void checkAccess(int op, const OW_String& ns, const OW_ACLInfo& aclInfo);

private:

	OW_String getMethodType(int op);
	OW_CIMServer* m_pServer;
	OW_CIMOMEnvironmentRef m_env;
};

//////////////////////////////////////////////////////////////////////////////
OW_AccessMgr::OW_AccessMgr(OW_CIMServer* pServer,
	OW_CIMOMEnvironmentRef env)
	: m_pServer(pServer)
	, m_env(env)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_AccessMgr::getMethodType(int op)
{
	switch(op)
	{
		case GETCLASS:
		case GETINSTANCE:
		case ENUMERATECLASSES:
		case ENUMERATECLASSNAMES:
		case ENUMERATEINSTANCES:
		case ENUMERATEINSTANCENAMES:
		case ENUMERATEQUALIFIERS:
		case GETPROPERTY:
		case GETQUALIFIER:
		case ENUMERATENAMESPACE:
		case ASSOCIATORS:
		case ASSOCIATORNAMES:
		case REFERENCES:
		case REFERENCENAMES:
			return OW_String("r");
		case DELETECLASS:
		case DELETEINSTANCE:
		case CREATECLASS:
		case CREATEINSTANCE:
		case MODIFYCLASS:
		case MODIFYINSTANCE:
		case SETPROPERTY:
		case SETQUALIFIER:
		case DELETEQUALIFIER:
		case CREATENAMESPACE:
		case DELETENAMESPACE:
			return OW_String("w");
		case INVOKEMETHOD:
			return OW_String("rw");
		default:
			OW_THROW(OW_Exception, "Unknown operation type passed to "
				"AccessMgr.  This shouldn't happen");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AccessMgr::checkAccess(int op, const OW_String& ns,
	const OW_ACLInfo& aclInfo)
{
	if (aclInfo.m_internal)
	{
		return;
	}
	OW_ACLInfo intACLInfo;

	m_env->logDebug(format("Checking access to namespace: \"%1\"", ns));
	m_env->logDebug(format("UserName is: \"%1\" Operation is : %2",
		aclInfo.getUserName(), op));

	OW_String lns(ns);
	while (!lns.empty() && lns[0] == '/')
	{
		lns = lns.substring(1);
	}
	lns.toLowerCase();

	for(;;)
	{
		if (!aclInfo.getUserName().empty())
		{
			OW_String superUser =
				m_env->getConfigItem(OW_ConfigOpts::ACL_SUPERUSER_opt);

			if (superUser.equalsIgnoreCase(aclInfo.getUserName()))
			{
				m_env->logDebug("User is SuperUser: checkAccess returning.");
				return;
			}

			try
			{
				OW_CIMClass cc = m_pServer->getClass("root/security",
					"OpenWBEM_UserACL", false, true, true, NULL,
					intACLInfo);
			}
			catch(OW_CIMException&)
			{
				m_env->logDebug("OpenWBEM_UserACL class non-existent in"
					" /root/security. ACLs disabled");
				return;
			}
			
			OW_CIMObjectPath cop("OpenWBEM_UserACL");
			cop.addKey("username", OW_CIMValue(aclInfo.getUserName()));
			cop.addKey("nspace", OW_CIMValue(lns));
			OW_CIMInstance ci;
			try
			{
				ci = m_pServer->getInstance("root/security", cop, false, true, true, NULL,
					NULL, intACLInfo);
			}
			catch(const OW_CIMException&)
			{
				ci.setNull();
			}
			if (ci)
			{
				OW_String capability;
				OW_CIMProperty capabilityProp = ci.getProperty("capability");
				if (capabilityProp)
				{
					OW_CIMValue cv = capabilityProp.getValue();
					if (cv)
					{
						capability = cv.toString();
					}
				}
				OW_String opType = getMethodType(op);
				capability.toLowerCase();
				if (opType.length() == 1)
				{
					if (capability.indexOf(opType) < 0)
					{
						m_env->logError(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							aclInfo.m_userName, lns));
						OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
					}
				}
				else
				{
					if (!capability.equals("rw") && !capability.equals("wr"))
					{
						m_env->logError(format(
							"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
							aclInfo.m_userName, lns));
						OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
					}
				}
				m_env->logCustInfo(format(
					"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
					aclInfo.m_userName, lns));
				return;
			}
		}
		// use default policy for namespace
		try
		{
			OW_CIMClass cc = m_pServer->getClass("root/security",
				"OpenWBEM_NamespaceACL", false, true, true, NULL,
				intACLInfo);
		}
		catch(OW_CIMException&)
		{
			m_env->logDebug("OpenWBEM_NamespaceACL class non-existent in"
				" /root/security. namespace ACLs disabled");
			return;
		}
		OW_CIMObjectPath cop("OpenWBEM_NamespaceACL");
		cop.addKey("nspace", OW_CIMValue(lns));
		OW_CIMInstance ci;
		try
		{
			ci = m_pServer->getInstance("root/security", cop, false, true, true, NULL,
				NULL, intACLInfo);
		}
		catch(const OW_CIMException& ce)
		{
			m_env->logDebug(format("Caught exception: %1 in"
				" OW_AccessMgr::checkAccess", ce));
			ci.setNull();
		}
	
		if (ci)
		{
			OW_String capability;
			OW_CIMProperty capabilityProp = ci.getProperty("capability");
			if (capabilityProp)
			{
				OW_CIMValue v = capabilityProp.getValue();
				if (v)
				{
					capability = v.toString();
				}
			}
			capability.toLowerCase();

			OW_String opType = getMethodType(op);

			if (opType.length() == 1)
			{
				if (capability.indexOf(opType) < 0)
				{
					m_env->logError(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						aclInfo.m_userName, lns));
					OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
				}
			}
			else
			{
				if (!capability.equals("rw") && !capability.equals("wr"))
				{
					m_env->logError(format(
						"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
						aclInfo.m_userName, lns));
					OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
				}
			}

			m_env->logCustInfo(format(
				"ACCESS GRANTED to user \"%1\" for namespace \"%2\"",
				aclInfo.m_userName, lns));
			return;

		}
		int idx = lns.lastIndexOf('/');
		if (idx <= 0)
		{
			break;
		}
		lns = lns.substring(0, idx);
	}
	m_env->logError(format(
		"ACCESS DENIED to user \"%1\" for namespace \"%2\"",
		aclInfo.m_userName, lns));
	OW_THROWCIM(OW_CIMException::ACCESS_DENIED);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMServer::OW_CIMServer(OW_CIMOMEnvironmentRef env,
	OW_ProviderManagerRef provManager)
	: OW_RepositoryIFC()
	, m_nStore(env)
	, m_iStore(env)
	, m_mStore(env)
	, m_provManager(provManager)
	, m_assocDb(env)
	, m_rwLocker()
	, m_accessMgr(new OW_AccessMgr(this, env))
	, m_nsClass__Namespace()
	, m_nsClassCIM_Namespace()
	, m_env(env)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMServer::~OW_CIMServer()
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
OW_CIMServer::open(const OW_String& path)
{
	OW_WriteLock wl = m_rwLocker.getWriteLock();

	if(m_nStore.isOpen())
	{
		close();
	}

	if(!OW_FileSystem::exists(path))
	{
		if(!OW_FileSystem::makeDirectory(path))
		{
			OW_String msg("failed to create directory: " );
			msg += path;
			OW_THROW(OW_IOException, msg.c_str());
		}
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
	m_assocDb.open(fname + ASSOC_REPOS_NAME);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::close()
{
	if(m_nStore.isOpen())
	{
		OW_WriteLock wl = m_rwLocker.getWriteLock();
		m_nStore.close();
		m_iStore.close();
		m_mStore.close();
		m_assocDb.close();
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::createNameSpace(const OW_String& ns,
	const OW_ACLInfo& aclInfo)
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

	// Check to see if user has rights to create the namespace
	m_accessMgr->checkAccess(OW_AccessMgr::CREATENAMESPACE, parns, aclInfo);

	m_env->logDebug(format("OW_CIMServer creating namespace: %1", ns));

	if(m_nStore.createNameSpace(nameComps) == -1)
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS,
			ns.c_str());
	}

	m_iStore.createNameSpace(nameComps, true);
	m_mStore.createNameSpace(nameComps, true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteNameSpace(const OW_String& ns,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to delete the namespace
	m_accessMgr->checkAccess(OW_AccessMgr::DELETENAMESPACE, ns, aclInfo);

	if(ns.empty())
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	m_nStore.deleteNameSpace(ns);
	m_iStore.deleteNameSpace(ns);
	m_mStore.deleteNameSpace(ns);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMServer::getQualifierType(const OW_String& ns,
	const OW_String& qualifierName,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get a qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::GETQUALIFIER, ns, aclInfo);
	
	m_env->logDebug(format("OW_CIMServer getting qualifier type: %1",
		OW_CIMObjectPath(qualifierName,ns).toString()));

	return m_mStore.getQualifierType(ns, qualifierName);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumQualifierTypes(
	const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get a qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEQUALIFIERS, ns, aclInfo);

	m_mStore.enumQualifierTypes(ns, result);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumNameSpace(const OW_String& nsName,
	OW_StringResultHandlerIFC& result, OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate the namespace
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATENAMESPACE, nsName,
		aclInfo);

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
			_getChildKeys(hdl.getHandle(), result, nsNode);
		}
		nsNode = hdl->getNextSibling(nsNode);
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMServer::_getChildKeys(OW_HDBHandle hdl, OW_StringResultHandlerIFC& result,
	OW_HDBNode node)
{
	result.handle(node.getKey());
	node = hdl.getFirstChild(node);
	while(node)
	{
		_getChildKeys(hdl, result, node);
		node = hdl.getNextSibling(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteQualifierType(const OW_String& ns, const OW_String& qualName,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to delete the qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEQUALIFIER, ns, aclInfo);

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
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::setQualifierType(
	const OW_String& ns,
	const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to update the qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::SETQUALIFIER, ns, aclInfo);
	m_env->logDebug(format("OW_CIMServer setting qualifier type: %1 in "
		"namespace: %2", qt.toString(), ns));
	m_mStore.setQualifierType(ns, qt);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::getClass(
	const OW_String& ns, const OW_String& className, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// we don't check for __Namespace, so that clients can get it before they
	// create one.
	if (!className.equalsIgnoreCase("__Namespace"))
	{
		m_accessMgr->checkAccess(OW_AccessMgr::GETCLASS, ns, aclInfo);
	}
	try
	{
		OW_CIMClass theClass = _getClass(ns,className);

		OW_StringArray lpropList;
		OW_Bool noProps = false;
		if(propertyList)
		{
			if(propertyList->size() == 0)
			{
				noProps = true;
			}
			else
			{
				lpropList = *propertyList;
			}
		}

		theClass = theClass.clone(localOnly, includeQualifiers, includeClassOrigin,
			lpropList, noProps);
		if (!theClass)
		{
			// clone doesn't throw
			OW_THROWCIM(OW_CIMException::FAILED);
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
OW_CIMServer::_getClass(const OW_String& ns, const OW_String& className)
{
	OW_CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		OW_CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, theClass);
		checkGetClassRvalAndThrow(rval, ns, className);
	}

	return theClass;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_instGetClass(const OW_String& ns, const OW_String& className)
{
	OW_CIMClass theClass = _getNameSpaceClass(className);
	if(!theClass)
	{
		OW_CIMException::ErrNoType rval = m_mStore.getCIMClass(ns, className, theClass);
		checkGetClassRvalAndThrowInst(rval, ns, className);
	}

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
			// the instances we delete?

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
OW_CIMServer::deleteClass(const OW_String& ns, const OW_String& className,
	const OW_ACLInfo& aclInfo)
{
	m_accessMgr->checkAccess(OW_AccessMgr::DELETECLASS, ns, aclInfo);
	try
	{
		OW_CIMClass cc = _getClass(ns, className);
		OW_ASSERT(cc);

		// TODO: this doesn't work quite right.  what about associations to
		// the instances we delete?
		// should this operation be atomic?  If something fails, how can we
		// undo so as to not leave things in a weird state?

		// delete the class and any subclasses
		OW_ACLInfo intAcl;
		CIMClassDeleter ccd(m_mStore, ns, m_iStore, m_assocDb);
		this->enumClasses(ns, className, ccd,
			OW_CIMOMHandleIFC::DEEP, OW_CIMOMHandleIFC::LOCAL_ONLY,
			OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS,
			OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN,
            intAcl);
		ccd.handle(cc);

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
OW_CIMServer::createClass(const OW_String& ns, const OW_CIMClass& cimClass,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to create the class
	m_accessMgr->checkAccess(OW_AccessMgr::CREATECLASS, ns, aclInfo);

	if(cimClass.getName().equals(OW_CIMClass::NAMESPACECLASS))
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS,
			format("Creation of class %1 is not allowed",
				cimClass.getName()).c_str());
	}

	try
	{
		m_env->logDebug(format("Creating class: %1:%2", ns, cimClass.toMOF()));
		m_mStore.createClass(ns, cimClass);
		m_iStore.createClass(ns, cimClass);
		if (cimClass.isAssociation())
		{
			OW_AssocDbHandle hdl = m_assocDb.getHandle();
			hdl.addEntries(ns,cimClass);
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
OW_CIMServer::modifyClass(
	const OW_String& ns,
	const OW_CIMClass& cc,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to create the class
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYCLASS, ns, aclInfo);

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
OW_CIMServer::enumClasses(const OW_String& ns,
		const OW_String& className,
		OW_CIMClassResultHandlerIFC& result,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate classes
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSES, ns, aclInfo);
	try
	{
		m_mStore.enumClass(ns, className,
			result, deep,
			localOnly, includeQualifiers, includeClassOrigin);
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
			lcop.setObjectName(c.getName());
			oph.handle(lcop);
		}
	private:
		OW_CIMObjectPathResultHandlerIFC& oph;
		OW_CIMObjectPath& lcop;
	};

	class CIMClassEnumerationBuilder : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassEnumerationBuilder(OW_CIMClassEnumeration& enu_)
		: enu(enu_)
		{}

	protected:
		virtual void doHandle(const OW_CIMClass &c)
		{
			enu.addElement(c);
		}
	private:
		OW_CIMClassEnumeration& enu;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumClassNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate classes
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSNAMES, ns, aclInfo);

	try
	{
		OW_CIMObjectPath lcop(className, ns);
		CIMClassToCIMObjectPathHandler handler(result,lcop);
		m_mStore.enumClass(ns, className, handler,
			deep, false, true, true);
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
OW_CIMServer::enumInstanceNames(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate instance names
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCENAMES, ns,
		aclInfo);

	try
	{
		OW_CIMClass theClass = _instGetClass(ns, className);

		_getCIMInstanceNames(ns, className, theClass, result, deep, aclInfo);

		// If this is the namespace class then just return now
		if(theClass.getName().equals(OW_CIMClass::NAMESPACECLASS)
			|| !deep)
		{
			return;
		}

		OW_StringArray classNames = m_mStore.getClassChildren(ns,
			theClass.getName());

		for(size_t i = 0; i < classNames.size(); i++)
		{
			theClass = _instGetClass(ns, classNames[i]);
			_getCIMInstanceNames(ns, classNames[i], theClass, result, deep, aclInfo);
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

/////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMServerProviderEnvironment : public OW_ProviderEnvironmentIFC
	{
	public:

		CIMServerProviderEnvironment(const OW_LocalCIMOMHandle& ch)
			: m_ch(new OW_LocalCIMOMHandle(ch))
		{}

		virtual OW_String getConfigItem(const OW_String &name) const
		{
			return ((OW_LocalCIMOMHandle*) m_ch.getPtr())->getEnvironment()->
				getConfigItem(name);
		}

		virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const
		{
			return m_ch;
		}
		
		virtual OW_LoggerRef getLogger() const
		{
			return ((OW_LocalCIMOMHandle*) m_ch.getPtr())->getEnvironment()->
				getLogger();
		}

	private:
		OW_CIMOMHandleIFCRef m_ch;
	};

	OW_ProviderEnvironmentIFCRef createProvEnvRef(const OW_LocalCIMOMHandle& ch)
	{
		return OW_ProviderEnvironmentIFCRef(new CIMServerProviderEnvironment(ch));
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMServer::_getCIMInstanceNames(const OW_String& ns, const OW_String& className,
	const OW_CIMClass& theClass, OW_CIMObjectPathResultHandlerIFC& result,
	OW_Bool deep, const OW_ACLInfo& aclInfo)
{
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);

	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(theClass);
	if (instancep)
	{
		instancep->enumInstanceNames(createProvEnvRef(real_ch),
			ns, className, result, deep, theClass);
	}
	else
	{
		m_iStore.getInstanceNames(ns, theClass, result);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::enumInstances(
	const OW_String& ns,
	const OW_String& className,
	OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate instances
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCES, ns, aclInfo);

	try
	{
		OW_CIMClass theTopClass = _instGetClass(ns, className);

		_getCIMInstances(ns, className, theTopClass, theTopClass, result, deep, localOnly,
			includeQualifiers, includeClassOrigin, propertyList, aclInfo);
		
		// If this is the namespace class then we're done.
		if(theTopClass.getName().equals(OW_CIMClass::NAMESPACECLASS))
		{
			return;
		}

		OW_StringArray classNames = m_mStore.getClassChildren(ns,
			className);

		for(size_t i = 0; i < classNames.size(); i++)
		{
			OW_CIMClass theClass = _instGetClass(ns, classNames[i]);

			_getCIMInstances(ns, classNames[i], theTopClass, theClass, result, deep, localOnly,
				includeQualifiers, includeClassOrigin, propertyList, aclInfo);
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

namespace
{
	class HandleProviderInstance : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		HandleProviderInstance(const OW_CIMClass& theClass_,
			const OW_ACLInfo& aclInfo_,
			bool includeQualifiers_, bool includeClassOrigin_,
			const OW_StringArray* propList_, const OW_String& ns_,
			OW_CIMServer& server_, OW_CIMInstanceResultHandlerIFC& result_)
		: theClass(theClass_)
		, aclInfo(aclInfo_)
		, includeQualifiers(includeQualifiers_)
		, includeClassOrigin(includeClassOrigin_)
		, propList(propList_)
		, ns(ns_)
		, server(server_)
		, result(result_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &c)
		{
			OW_CIMInstance ci(c);
			OW_CIMObjectPath lcop(ci);

			server._getProviderProperties(ns, lcop, ci, theClass, aclInfo, propList);
			result.handle(ci.clone(false, includeQualifiers,
				includeClassOrigin, propList));
		}
	private:
		const OW_CIMClass& theClass;
		const OW_ACLInfo& aclInfo;
		bool includeQualifiers, includeClassOrigin;
		const OW_StringArray* propList;
		const OW_String& ns;
		OW_CIMServer& server;
		OW_CIMInstanceResultHandlerIFC& result;
	};

	class HandleLocalOnlyAndDeep : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		HandleLocalOnlyAndDeep(
			OW_CIMInstanceResultHandlerIFC& result_,
			const OW_CIMClass& requestedClass_,
			bool localOnly_,
			bool deep_)
		: result(result_)
		, requestedClass(requestedClass_)
		, localOnly(localOnly_)
		, deep(deep_)
		{}
	protected:
		virtual void doHandle(const OW_CIMInstance &inst)
		{
			if (deep == true && localOnly == false) // don't filter anything
			{
				result.handle(inst);
				return;
			}

			OW_CIMPropertyArray props = inst.getProperties();
			OW_CIMPropertyArray newprops;
			OW_CIMInstance newInst(inst);
			OW_String requestedClassName = requestedClass.getName();
			for (size_t i = 0; i < props.size(); ++i)
			{
				OW_CIMProperty p = props[i];
				OW_CIMProperty clsp = requestedClass.getProperty(p.getName());
				if (clsp)
				{
					if (clsp.getOriginClass().equalsIgnoreCase(requestedClassName))
					{
						newprops.push_back(p);
						continue;
					}
				}
				if (deep == true)
				{
					if (!clsp
						|| !p.getOriginClass().equalsIgnoreCase(clsp.getOriginClass()))
					{
						// the property is from a derived class
						newprops.push_back(p);
						continue;
					}
				}
				if (localOnly == false)
				{
					if (clsp)
					{
						// the property has to be from a superclass
						newprops.push_back(p);
						continue;
					}
				}

			}
			newInst.setProperties(newprops);
			newInst.setKeys(inst.getKeyValuePairs());
			result.handle(newInst);
		}
	private:
		OW_CIMInstanceResultHandlerIFC& result;
		const OW_CIMClass& requestedClass;
		bool localOnly;
		bool deep;
	};
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMServer::_getCIMInstances(
	const OW_String& ns,
	const OW_String& className,
	const OW_CIMClass& theTopClass,
	const OW_CIMClass& theClass, OW_CIMInstanceResultHandlerIFC& result, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo, true);

	OW_InstanceProviderIFCRef instancep(_getInstanceProvider(theClass));
	if (instancep)
	{
		HandleLocalOnlyAndDeep handler1(result,theClass,localOnly,deep);
		HandleProviderInstance handler2(theClass,aclInfo,
			includeQualifiers, includeClassOrigin, propertyList, ns, *this, handler1);
		instancep->enumInstances(
			createProvEnvRef(real_ch), ns, className, handler2, deep, theClass, localOnly);
	}
	else
	{
		HandleLocalOnlyAndDeep handler(result, theTopClass, localOnly, deep);
		m_iStore.getCIMInstances(ns, className, theClass, handler,
			includeQualifiers, includeClassOrigin, propertyList, this,
			&aclInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::getInstance(
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
OW_CIMServer::getInstance(
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMClass* pOutClass,
	const OW_ACLInfo& aclInfo)
{
	OW_StringArray lpropList;
	if(propertyList)
	{
		lpropList = *propertyList;
	}

	// Check to see if user has rights to get the instance
	m_accessMgr->checkAccess(OW_AccessMgr::GETINSTANCE, ns, aclInfo);

	OW_CIMClass cc;
	OW_CIMInstance ci;
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	try
	{
		// this doesn't use m_mStore because of __Namespace
		cc = getClass(ns, instanceName.getObjectName(),
			OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
			OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
			OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN,
			0, aclInfo);
	}
	catch (OW_CIMException& ce)
	{
		if (ce.getErrNo() == OW_CIMException::NOT_FOUND)
		{
			ce.setErrNo(OW_CIMException::INVALID_CLASS);
		}
		throw ce;
	}

	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(cc);

	if(instancep)
	{
		ci = instancep->getInstance(
			createProvEnvRef(real_ch),
				ns, instanceName, cc, localOnly);
		if (!ci)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Provider erroneously returned a NULL CIMInstance");
		}
	}
	else
	{
		try
		{
			ci = m_iStore.getCIMInstance(ns, instanceName, cc);
		}
		catch (OW_IOException&)
		{
			OW_THROWCIM(OW_CIMException::FAILED);
		}
	}

	OW_ASSERT(ci);
	if(pOutClass)
	{
		*pOutClass = cc;
	}
	
	ci.syncWithClass(cc, true);
	
	_getProviderProperties(ns, instanceName, ci, cc, aclInfo, propertyList);
	ci = ci.clone(localOnly, includeQualifiers, includeClassOrigin,
		propertyList);
	
	return ci;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop_,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMObjectPath cop(cop_);
	cop.setNameSpace(ns);
	// Check to see if user has rights to delete the instance
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEINSTANCE, ns, aclInfo);

	m_env->logDebug(format("OW_CIMServer::deleteInstance.  cop = %1",
		cop.toString()));

	OW_ACLInfo intAclInfo;

	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);

	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	try
	{
		OW_CIMClass theClass;
		OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
			&theClass, intAclInfo);

		OW_AssocDbHandle hdl = m_assocDb.getHandle();

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
			m_env->logDebug("OW_CIMServer::deleteInstance."
				" theClass.isAssociation() == true");

			// If there is no associator provider for this instance, then go ahead
			// delete the entries from the database for this association.
			if(!_getAssociatorProvider(theClass))
			{
				hdl.deleteEntries(ns, oldInst);
			}
		}

		OW_InstanceProviderIFCRef instancep = _getInstanceProvider(theClass);

		if(instancep)	// If there is an instance provider, let it do the delete.
		{
			instancep->deleteInstance(
				createProvEnvRef(real_ch), ns, cop);
		}
		else
		{
			// Delete the instance from the instance repository
			m_iStore.deleteInstance(ns, cop, theClass);
		}

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
OW_CIMServer::createInstance(
	const OW_String& ns,
	const OW_CIMInstance& ci,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMObjectPath rval(ci);

	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess(OW_AccessMgr::CREATEINSTANCE, ns, aclInfo);

	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	try
	{
		OW_String className = ci.getClassName();

		OW_CIMClass theClass = _instGetClass(ns, className);

		OW_CIMQualifier acq = theClass.getQualifier(
				OW_CIMQualifier::CIM_QUAL_ABSTRACT);
		if(acq)
		{
			if (acq.getValue() == OW_CIMValue(true))
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
						format("Unable to create instance because class (%1)"
							" is abstract", theClass.getName()).c_str());
			}
		}

		// Make sure instance jives with class definition
		OW_CIMInstance lci(ci);
		lci.syncWithClass(theClass, false);

		m_env->logDebug(format("OW_CIMServer::createInstance.  ns = %1, "
			"instance = %2", ns, lci.toString()));

		OW_InstanceProviderIFCRef instancep = _getInstanceProvider(theClass);
		if (instancep)
		{
			rval = instancep->createInstance(createProvEnvRef(real_ch), ns, ci);
		}
		else
		{
			if(theClass.isAssociation())
			{
				OW_ACLInfo intAclInfo;
				OW_CIMPropertyArray pra = ci.getProperties(
					OW_CIMDataType::REFERENCE);

				for(size_t j = 0; j < pra.size(); j++)
				{
					OW_CIMValue cv = pra[j].getValue();
					if(!cv)
					{
						continue;
					}

					OW_CIMObjectPath op;
					cv.get(op);

					if(!op)
					{
						continue;
					}

					OW_CIMClass rcc;
					OW_CIMException::ErrNoType rc = m_mStore.getCIMClass(op.getNameSpace(), op.getObjectName(), rcc);

					if(rc == OW_CIMException::SUCCESS)
					{
						try
						{
							OW_CIMInstance rci = m_iStore.getCIMInstance(op.getNameSpace(), op, rcc);
						}
						catch (OW_CIMException&)
						{
							OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								format("Association references an invalid instance:"
									" %1", op.toString()).c_str());
						}
					}
					else
					{
						OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
							format("Reference property has an invalid class: %1 in "
								"namespace: %2", op.getObjectName(),
								op.getNameSpace()).c_str());
					}

				}
			}

			m_iStore.createInstance(ns, theClass, lci);
			_validatePropagatedKeys(ns, lci, theClass);
		}

		if(theClass.isAssociation())
		{
			OW_AssociatorProviderIFCRef assocP(_getAssociatorProvider(theClass));

			if(!assocP)
			{
				OW_AssocDbHandle hdl = m_assocDb.getHandle();
				hdl.addEntries(ns, ci);
			}
		}

		_setProviderProperties(ns, ci, theClass, aclInfo);
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
OW_CIMServer::modifyInstance(
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	OW_Bool includeQualifiers,
	OW_StringArray* propertyList,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to modify the instance
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYINSTANCE, ns, aclInfo);

	try
	{
		OW_ACLInfo intAclInfo;
		OW_CIMClass theClass;
		OW_CIMObjectPath cop(modifiedInstance);
		OW_CIMInstance oldInst = getInstance(ns, cop, false, true, true, NULL,
			&theClass, intAclInfo);

		OW_CIMInstance newInst(modifiedInstance);

		if (!includeQualifiers)
		{
			newInst.setQualifiers(oldInst.getQualifiers());
		}

		if (propertyList)
		{
			newInst.setProperties(oldInst.getProperties());
			for (OW_StringArray::const_iterator i = propertyList->begin();
				 i != propertyList->end(); ++i)
			{
				OW_CIMProperty p = modifiedInstance.getProperty(*i);
				if (p)
				{
					if (!includeQualifiers)
					{
						OW_CIMProperty cp = theClass.getProperty(*i);
						if (cp)
						{
							p.setQualifiers(cp.getQualifiers());
						}
					}
					newInst.setProperty(p);
				}
				else
				{
					OW_CIMProperty cp = theClass.getProperty(*i);
					if (cp)
					{
						newInst.setProperty(cp);
					}
					else
					{
						newInst.removeProperty(*i);
					}
				}
			}
		}

		OW_InstanceProviderIFCRef instancep(_getInstanceProvider(theClass));

		if(!instancep)
		{
			// No instance provider qualifier found
			m_iStore.modifyInstance(ns, cop, theClass, newInst, oldInst);
		}
		else
		{
			// Look for dynamic instances
			OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
				aclInfo, true);
			instancep->modifyInstance(createProvEnvRef(real_ch), ns,
				newInst);
		}

		if(theClass.isAssociation())
		{
			OW_LocalCIMOMHandle ch(m_env, OW_RepositoryIFCRef(this, true),
				OW_ACLInfo(), true);
			OW_AssociatorProviderIFCRef assocP(_getAssociatorProvider(theClass));

			if(!assocP)
			{
				OW_AssocDbHandle adbHdl = m_assocDb.getHandle();
				adbHdl.deleteEntries(ns, oldInst);
				adbHdl.addEntries(ns, newInst);
			}
		}

		_setProviderProperties(ns, newInst, theClass, aclInfo);
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
OW_Bool
OW_CIMServer::_instanceExists(const OW_String& ns, const OW_CIMObjectPath& icop,
	const OW_ACLInfo& aclInfo)
{
	OW_String classname = icop.getObjectName();

	OW_CIMClass cc;
	try
	{
		cc = getClass(ns, classname,
			OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
			OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
			OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN,
			0, aclInfo);
	}
	catch(OW_CIMException&)
	{
		return false;
	}

	OW_InstanceProviderIFCRef ip = _getInstanceProvider(cc);
	if(ip)
	{
		//
		//  Ask the provider for the instance
		//
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, true);

		return OW_Bool((ip->getInstance(createProvEnvRef(real_ch), ns, icop, cc,
			false)) != 0);
	}
	else
	{
		OW_CIMException::ErrNoType rc = m_mStore.getCIMClass(ns, icop.getObjectName(), cc);
		if (rc != OW_CIMException::SUCCESS)
		{
			return false;
		}
	}

	return m_iStore.instanceExists(ns, icop, cc);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMServer::getProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::GETPROPERTY, ns, aclInfo);

	OW_ACLInfo intAclInfo;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	OW_CIMClass theClass = _instGetClass(ns,name.getObjectName());

	OW_CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	OW_PropertyProviderIFCRef propp = _getPropertyProvider(cp);

	if(!propp)
	{
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
	else
	{
		return propp->getPropertyValue(
			createProvEnvRef(real_ch),
				ns, name, cp.getOriginClass(), propertyName);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::setProperty(
	const OW_String& ns,
	const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& valueArg,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::SETPROPERTY, ns, aclInfo);

	OW_ACLInfo intAclInfo;
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

	OW_PropertyProviderIFCRef propp = _getPropertyProvider(cp);
	if(!propp)
	{
		OW_CIMInstance ci = getInstance(ns, name, false, true, true, NULL,
			NULL, intAclInfo);

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
		modifyInstance(ns, ci, true, 0, intAclInfo);
	}
	else
	{
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, true);

		propp->setPropertyValue(
			createProvEnvRef(real_ch),
				ns, name, cp.getOriginClass(), cp.getName(), cv);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMServer::invokeMethod(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	const OW_String& methodName, const OW_CIMParamValueArray& inParams,
	OW_CIMParamValueArray& outParams, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::INVOKEMETHOD, ns, aclInfo);

	// Not creating any read/write locks here because the provider
	// could call back into the CIM Server for something and then we
	// could end up with a deadlock condition.

	OW_ACLInfo intAclInfo;
	OW_CIMMethod method;
	OW_CIMClass cc = _getClass(ns, path.getObjectName());

	OW_CIMPropertyArray keys = path.getKeys();

	// If this is an instance, ensure it exists.
	if(keys.size() > 0)
	{
		if(!_instanceExists(ns, path, aclInfo))
		{
			// Dynamic - Get the provider information
			OW_CIMQualifier cq = cc.getQualifier(
				OW_CIMQualifier::CIM_QUAL_PROVIDER);

			if(!cq)
			{
				OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
					format("Instance not found: %1", path.toString()).c_str());
			}
		}
	}

	// Get the method from the class definition
	method = cc.getMethod(methodName);
	if(!method)
	{
		OW_THROWCIMMSG(OW_CIMException::METHOD_NOT_FOUND, methodName.c_str());
	}

	//
	// If a provider is defined, go call it to get the real property
	//
	OW_CIMQualifier cq = method.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);

	if(!cq)
	{
		//
		// Look to see if class instance provider can act as
		// a method provider
		//
		cq = cc.getQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
		if(!cq)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"No method provider");
		}
	}

	OW_CIMValue cv;
	OW_MethodProviderIFCRef methodp;

	try
	{
		OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
			intAclInfo, true);
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, true);
		methodp = m_provManager->getMethodProvider(
			createProvEnvRef(internal_ch), cq);
		if(methodp.isNull())
		{
			OW_String msg("Unknown provider: ");
			msg += cq.getValue().toString();
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, msg.c_str());
		}

		OW_CIMParameterArray methodInParams = method.getINParameters();
		if (inParams.size() != methodInParams.size())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Incorrect number of parameters");
		}

		OW_CIMParameterArray methodOutParams = method.getOUTParameters();
		outParams.resize(methodOutParams.size());
		// set the names on outParams
		for (size_t i = 0; i < methodOutParams.size(); ++i)
		{
			outParams[i].setName(methodOutParams[i].getName());
		}

		OW_CIMParamValueArray orderedParams;
		OW_CIMParamValueArray inParams2(inParams);
		for (size_t i = 0; i < methodInParams.size(); ++i)
		{
			OW_String parameterName = methodInParams[i].getName();
			size_t paramIdx;
			for (paramIdx = 0; paramIdx < inParams2.size(); ++paramIdx)
			{
				if (inParams2[paramIdx].getName().equalsIgnoreCase(parameterName))
				{
					break;
				}
			}

			if (paramIdx == inParams2.size())
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
					"Parameter %1 was not specified.", parameterName).c_str());
			}

			// move the param from inParams2 to orderedParams
			orderedParams.push_back(inParams2[paramIdx]);
			inParams2.erase(inParams2.begin() + paramIdx);

			// make sure the type is right
			OW_CIMValue v = orderedParams[i].getValue();
			if (v)
			{
				if (methodInParams[i].getType().getType() != v.getType())
				{
                    try
                    {
                        orderedParams[i].setValue(OW_CIMValueCast::castValueToDataType(
                            v, methodInParams[i].getType()));
                    }
                    catch (OW_CIMException& ce)
                    {
                        ce.setErrNo(OW_CIMException::INVALID_PARAMETER);
                        throw;
                    }
				}
			}

			// if the in param is also an out param, assign the value to the out
			// params array
			if (methodInParams[i].getQualifier(OW_CIMQualifier::CIM_QUAL_OUT))
			{
				size_t j;
				for (j = 0; j < outParams.size(); ++j)
				{
					if (outParams[j].getName() == parameterName)
					{
						outParams[j].setValue(orderedParams[i].getValue());
						break;
					}
				}
				if (j == outParams.size())
				{
					OW_ASSERT(0);
				}
			}
		}



		// all the params should have been moved to orderedParams.  If there are
		// some left, it means we have an unknown/invalid parameter.
		if (inParams2.size() > 0)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, format(
				"Unknown or duplicate parameter: %1", inParams2[0].getName()).c_str());
		}


		OW_StringBuffer methodStr;
		methodStr += "OW_CIMServer invoking extrinsic method provider: ";
		methodStr += cq.getValue().toString();
		methodStr += '\n';
		methodStr += ns;
		methodStr += ':';
		methodStr += path.toString();
		methodStr += '.';
		methodStr += methodName;
		methodStr += '(';
		for (size_t i = 0; i < orderedParams.size(); ++i)
		{
			methodStr += orderedParams[i].getName();
			methodStr += '=';
			methodStr += orderedParams[i].getValue().toString();
			if (i != orderedParams.size() - 1)
			{
				methodStr += ", ";
			}
		}
		methodStr += ')';

		m_env->logDebug(methodStr.toString());

		cv = methodp->invokeMethod(
			createProvEnvRef(real_ch),
				ns, path, methodName, orderedParams, outParams);
		
		// make sure the type is right on the outParams
		for (size_t i = 0; i < methodOutParams.size(); ++i)
		{
			OW_CIMValue v = outParams[i].getValue();
			if (v)
			{
				if (methodOutParams[i].getType().getType() != v.getType())
				{
                    outParams[i].setValue(OW_CIMValueCast::castValueToDataType(
                        v, methodOutParams[i].getType()));
				}
			}
		}

		methodStr.reset();
		methodStr += "OW_CIMServer finished invoking extrinsic method provider: ";
		methodStr += cq.getValue().toString();
		methodStr += '\n';
		methodStr += ns;
		methodStr += ':';
		methodStr += path.toString();
		methodStr += '.';
		methodStr += methodName;
		methodStr += " OUT Params(";
		for (size_t i = 0; i < outParams.size(); ++i)
		{
			methodStr += outParams[i].getName();
			methodStr += '=';
			methodStr += outParams[i].getValue().toString();
			if (i != outParams.size() - 1)
			{
				methodStr += ", ";
			}
		}
		methodStr += ") return value: ";
		methodStr += cv.toString();
		m_env->logDebug(methodStr.toString());
	}
	catch(OW_CIMException& e)
	{
		throw;
	}
	catch(OW_Exception& e)
	{
		OW_String msg("Got exception: ");
		msg += e.type();
		msg += " calling provider: ";
		msg += cq.getValue().toString() + " MSG: ";
		msg += e.getMessage();
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}
	catch(...)
	{
		OW_String msg("calling provider: ");
		msg += cq.getValue().toString();
		OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
	}

	return cv;
}

//////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef
OW_CIMServer::_getInstanceProvider(const OW_CIMClass& cc)
{
	OW_CIMQualifier cq = cc.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	
	if(cq)
	{
		OW_InstanceProviderIFCRef instancep;
		OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
			OW_ACLInfo(), true);
		instancep =  m_provManager->getInstanceProvider(
			createProvEnvRef(internal_ch), cq);
		if(!instancep)
		{
			// if it's not an instance or associator provider, then ERROR!
			if (!m_provManager->getAssociatorProvider(
				createProvEnvRef(internal_ch), cq))
			{
				OW_String msg("Unknown provider: ");
				msg += cq.getValue().toString();
				OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
			}
		}
		return instancep;
	}

	return OW_InstanceProviderIFCRef();
}

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef
OW_CIMServer::_getAssociatorProvider(const OW_CIMClass& cc)
{
	OW_CIMQualifier cq = cc.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	
	if(cq)
	{
		OW_AssociatorProviderIFCRef ap;
		OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
			OW_ACLInfo(), true);
		ap =  m_provManager->getAssociatorProvider(
			createProvEnvRef(internal_ch), cq);
		if(!ap)
		{
			// if it's not an instance or associator provider, then ERROR!
			if (!m_provManager->getInstanceProvider(
				createProvEnvRef(internal_ch), cq))
			{
				OW_String msg("Unknown provider: ");
				msg += cq.getValue().toString();
				OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
			}
		}
		return ap;
	}

	return OW_AssociatorProviderIFCRef();
}

//////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef
OW_CIMServer::_getPropertyProvider(const OW_CIMProperty& cc)
{
	OW_CIMQualifier cq = cc.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);
	
	if(cq)
	{
		OW_PropertyProviderIFCRef p;
		OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
			OW_ACLInfo(), true);
		p =  m_provManager->getPropertyProvider(
			createProvEnvRef(internal_ch), cq);
		if(!p)
		{
			OW_String msg("Unknown provider: ");
			msg += cq.getValue().toString();
			OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
		}
		return p;
	}

	return OW_PropertyProviderIFCRef();
}

//////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_getNameSpaceClass(const OW_String& className)
{

	if (className.equalsIgnoreCase("__Namespace"))
	{
		if(!m_nsClass__Namespace)
		{
			m_nsClass__Namespace = OW_CIMClass("__Namespace");

			OW_CIMQualifier cimQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
			cimQualifier.setValue(OW_CIMValue(OW_String(NAMESPACE_PROVIDER)));
			OW_CIMProperty cimProp(OW_CIMProperty::NAME_PROPERTY);
			cimProp.setDataType(OW_CIMDataType::STRING);
			cimProp.addQualifier(OW_CIMQualifier::createKeyQualifier());
			m_nsClass__Namespace.addQualifier(cimQualifier);
			m_nsClass__Namespace.addProperty(cimProp);
		}

		return m_nsClass__Namespace;
	}
	/*
	else if (className.equalsIgnoreCase("CIM_Namespace"))
	{
		if (!m_nsClassCIM_Namespace)
		{
			m_nsClassCIM_Namespace = OW_CIMClass("CIM_Namespace");

		}
		return m_nsClassCIM_Namespace;
	}
	*/
	else
	{
		return OW_CIMClass();
	}
}

//////////////////////////////////////////////////////////////////////
void
OW_CIMServer::execQuery(
	const OW_String& ns,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String &query,
	const OW_String& queryLanguage, const OW_ACLInfo& aclInfo)
{
	OW_WQLIFCRef wql = m_env->getWQLRef();
	if (wql && wql->supportsQueryLanguage(queryLanguage))
	{
		OW_CIMOMHandleIFCRef lch(new OW_LocalCIMOMHandle(m_env,
			OW_RepositoryIFCRef(this, true), aclInfo, true));

		try
		{
			wql->evaluate(ns, result, query, queryLanguage, lch);
		}
		catch (const OW_CIMException& ce)
		{
			// translate any error except INVALID_NAMESPACE, INVALID_QUERY,
			// ACCESS_DENIED or FAILED into an INVALID_QUERY
			if (ce.getErrNo() != OW_CIMException::FAILED
				&& ce.getErrNo() != OW_CIMException::INVALID_NAMESPACE
				&& ce.getErrNo() != OW_CIMException::INVALID_QUERY
				&& ce.getErrNo() != OW_CIMException::ACCESS_DENIED)
			{
				// the " " added to the beginning of the message is a little
				// trick to fool the OW_CIMException constructor from stripping
				// away the old "canned" CIMException message.
				throw OW_CIMException(ce.getFile(), ce.getLine(),
					OW_CIMException::INVALID_QUERY,
					OW_String(OW_String(" ") + ce.getMessage()).c_str());
			}
			else
			{
				throw ce;
			}
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::QUERY_LANGUAGE_NOT_SUPPORTED, queryLanguage.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::associators(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORS, ns, aclInfo);

	m_env->logDebug(format("OW_CIMServer associators. ns = %1, path = %2, assocClass ="
		" %3, resultClass = %4, role = %5, resultRole = %6, includeQualifiers ="
		" %7, includeClassOrigin = %8",
		ns, path, assocClass, resultClass, role, resultRole, includeQualifiers,
		includeClassOrigin));

	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &result, 0, 0,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::associatorsClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORS, ns, aclInfo);

	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, 0, 0, &result,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::associatorNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORNAMES, ns, aclInfo);

	_commonAssociators(ns, path, assocClass, resultClass, role, resultRole,
		false, false, 0, 0, &result, 0, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::references(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMInstanceResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCES, ns, aclInfo);

	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &result, 0, 0, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::referencesClasses(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMClassResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCES, ns, aclInfo);

	_commonReferences(ns, path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, 0, 0, &result, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::referenceNames(
	const OW_String& ns,
	const OW_CIMObjectPath& path,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass, const OW_String& role,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCENAMES, ns, aclInfo);

	_commonReferences(ns, path, resultClass, role, false, false, 0, 0, &result, 0,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class assocClassSeparator : public OW_CIMClassResultHandlerIFC
	{
	public:
		assocClassSeparator(
			OW_CIMClassArray& staticAssocs_,
			OW_CIMClassArray& dynamicAssocs_,
			OW_CIMServer& server_,
			const OW_ACLInfo& aclInfo_)
		: staticAssocs(staticAssocs_)
		, dynamicAssocs(dynamicAssocs_)
		, server(server_)
		, aclInfo(aclInfo_)
		{}
	protected:
		virtual void doHandle(const OW_CIMClass &cc)
		{
			if (!cc.isAssociation())
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					format("class %1 is not an association", cc.getName()).c_str());
			}
			// Now separate the association classes that have associator provider from
			// the ones that don't
			OW_CIMClassArray* pra = (server._isDynamicAssoc(cc)) ? &dynamicAssocs
				: &staticAssocs;
			pra->append(cc);
		}
	private:
		OW_CIMClassArray& staticAssocs;
		OW_CIMClassArray& dynamicAssocs;
		OW_CIMServer& server;
		const OW_ACLInfo& aclInfo;
	};
}



//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_commonReferences(
	const OW_String& ns,
	const OW_CIMObjectPath& path_,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMClass assocClass;
	OW_CIMObjectPath path(path_);
	path.setNameSpace(ns);
	if (!m_nStore.nameSpaceExists(ns))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}

	// Get all association classes from the repository
	// If the result class was specified, only children of it will be
	// returned.

	OW_CIMClassArray staticAssocs;
	OW_CIMClassArray dynamicAssocs;

	assocClassSeparator assocClassResult(staticAssocs, dynamicAssocs, *this, aclInfo);
	_getAssociationClasses(ns, resultClass, path.getObjectName(), assocClassResult, role);
	OW_StringArray resultClassNames;
	for(size_t i = 0; i < staticAssocs.size(); i++)
	{
		resultClassNames.append(staticAssocs[i].getName());
	}
	OW_SortedVectorSet<OW_String> resultClassNamesSet(resultClassNames.begin(), resultClassNames.end());

	if (path.getKeys().size() == 0)
	{
		// it's a class path
		// Process all of the association classes without providers
		_staticReferencesClass(path,
			resultClass.empty() ? 0 : &resultClassNamesSet,
			role, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticReferences(path,
				resultClass.empty() ? 0 : &resultClassNamesSet, role,
				includeQualifiers, includeClassOrigin, propertyList, *piresult);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			_staticReferences(path,
				resultClass.empty() ? 0 : &resultClassNamesSet, role,
				*popresult);
		}
		else
		{
			OW_ASSERT(0);
		}

		// Process all of the association classes with providers
		_dynamicReferences(path, dynamicAssocs, role, includeQualifiers,
			includeClassOrigin, propertyList, piresult, popresult, aclInfo);
	}

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_dynamicReferences(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& assocClasses, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult, const OW_ACLInfo& aclInfo)
{
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	// assocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}

	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		OW_CIMClass cc = assocClasses[i];

		OW_AssociatorProviderIFCRef assocP = _getAssociatorProvider(cc);
		if(!assocP)
		{
			continue;
		}

		OW_CIMObjectPath assocClassPath(assocClasses[i].getName(),
			path.getNameSpace());

		// If the object path enumeration pointer is null, then assume we
		// are doing references and not referenceNames
		if(piresult != 0)
		{
			assocP->references(
				createProvEnvRef(real_ch),
				path.getNameSpace(), assocClassPath, path, *piresult, role, includeQualifiers,
				includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			assocP->referenceNames(
				createProvEnvRef(real_ch),
				path.getNameSpace(), assocClassPath, path, *popresult, role);
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
			OW_CIMServer& server_,
			OW_String& ns_,
			bool includeQualifiers_,
			bool includeClassOrigin_,
			const OW_StringArray* propList_,
			OW_ACLInfo& aclInfo_)
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
		OW_CIMServer& server;
		OW_String& ns;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propList;
		OW_ACLInfo& aclInfo;
	};

//////////////////////////////////////////////////////////////////////////////
	class staticAssociatorsInstResultHandler : public OW_AssocDbEntryResultHandlerIFC
	{
	public:
		staticAssociatorsInstResultHandler(const OW_ACLInfo& intAclInfo_,
			OW_CIMServer& server_, OW_CIMInstanceResultHandlerIFC& result_,
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
		OW_CIMServer& server;
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
			OW_CIMServer& server_, OW_CIMInstanceResultHandlerIFC& result_,
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
		OW_CIMServer& server;
		OW_CIMInstanceResultHandlerIFC& result;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propertyList;
	};
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_staticReferences(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* refClasses, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC& result)
{
	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();
	OW_ACLInfo intAclInfo;
	staticReferencesInstResultHandler handler(intAclInfo, *this, result,
		includeQualifiers, includeClassOrigin, propertyList);

	dbhdl.getAllEntries(path,
		refClasses, 0, role, OW_String(), handler);
}


//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_staticReferences(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* refClasses, const OW_String& role,
	OW_CIMObjectPathResultHandlerIFC& result)
{
	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();
	staticReferencesObjectPathResultHandler handler(result);
	dbhdl.getAllEntries(path,
		refClasses, 0, role, OW_String(), handler);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_commonAssociators(
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
	OW_CIMClassArray staticAssocs;
	OW_CIMClassArray dynamicAssocs;
	assocClassSeparator assocClassResult(staticAssocs, dynamicAssocs, *this, aclInfo);
	_getAssociationClasses(ns, assocClassName, path.getObjectName(), assocClassResult, role);

	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	OW_StringArray resultClassNames;
	if(!resultClass.empty())
	{
		resultClassNames = m_mStore.getClassChildren(ns, resultClass);
		resultClassNames.append(resultClass);
	}

	OW_StringArray assocClassNames;
	for(size_t i = 0; i < staticAssocs.size(); i++)
	{
		assocClassNames.append(staticAssocs[i].getName());
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
			role, resultRole, includeQualifiers, includeClassOrigin, propertyList, popresult, pcresult);
	}
	else // it's an instance path
	{
		// Process all of the association classes without providers
		if (piresult != 0)
		{
			// do instances
			_staticAssociators(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
				resultClass.empty() ? 0 : &resultClassNamesSet, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList, *piresult);
		}
		else if (popresult != 0)
		{
			// do names (object paths)
			_staticAssociators(path, assocClassName.empty() ? 0 : &assocClassNamesSet,
				resultClass.empty() ? 0 : &resultClassNamesSet, role, resultRole,
				*popresult);
		}
		else
		{
			OW_ASSERT(0);
		}

		// Process all of the association classes with providers
		_dynamicAssociators(path, dynamicAssocs, resultClass, role, resultRole,
			includeQualifiers, includeClassOrigin, propertyList, piresult, popresult,
			aclInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_dynamicAssociators(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& assocClasses, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC* piresult,
	OW_CIMObjectPathResultHandlerIFC* popresult, const OW_ACLInfo& aclInfo)
{
	// AssocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}

	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		OW_CIMClass cc = assocClasses[i];

		OW_AssociatorProviderIFCRef assocP = _getAssociatorProvider(cc);
		if(!assocP)
		{
			continue;
		}

		OW_CIMObjectPath assocClassPath(assocClasses[i].getName(),
			path.getNameSpace());

		if(piresult != 0)
		{
			assocP->associators(createProvEnvRef(real_ch), path.getNameSpace(),
				assocClassPath, path, *piresult, resultClass, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList);
		}
		else if (popresult != 0)
		{
			assocP->associatorNames(createProvEnvRef(real_ch),
				path.getNameSpace(),
				assocClassPath, path, *popresult, resultClass, role, resultRole);
		}
		else
		{
			OW_ASSERT(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_staticAssociators(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* passocClasses,
	const OW_SortedVectorSet<OW_String>* presultClasses,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceResultHandlerIFC& result)
{

	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();
	OW_ACLInfo intAclInfo;
	staticAssociatorsInstResultHandler handler(intAclInfo, *this, result,
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
			OW_CIMServer& server_,
			OW_String& ns_,
			bool includeQualifiers_,
			bool includeClassOrigin_,
			const OW_StringArray* propList_,
			OW_ACLInfo& aclInfo_)
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
		OW_CIMServer& server;
		OW_String& ns;
		bool includeQualifiers;
		bool includeClassOrigin;
		const OW_StringArray* propList;
		OW_ACLInfo& aclInfo;
	};

}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_staticAssociators(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* passocClasses,
	const OW_SortedVectorSet<OW_String>* presultClasses,
	const OW_String& role, const OW_String& resultRole,
	OW_CIMObjectPathResultHandlerIFC& result)
{

	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();
	staticAssociatorsObjectPathResultHandler handler(result);
	dbhdl.getAllEntries(path,
		passocClasses, presultClasses, role, resultRole, handler);
		
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_staticAssociatorsClass(
	const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* assocClassNames,
	const OW_SortedVectorSet<OW_String>* resultClasses,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult)
{
	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();

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
			OW_ACLInfo intAclInfo;
			OW_String ns = path.getNameSpace();
			staticAssociatorsClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, intAclInfo);
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
OW_CIMServer::_staticReferencesClass(const OW_CIMObjectPath& path,
	const OW_SortedVectorSet<OW_String>* resultClasses,
	const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList,
	OW_CIMObjectPathResultHandlerIFC* popresult,
	OW_CIMClassResultHandlerIFC* pcresult)
{
	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();

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
			OW_ACLInfo intAclInfo;
			OW_String ns = path.getNameSpace();
			staticReferencesClassResultHandler handler(*pcresult,*this,
				ns, includeQualifiers, includeClassOrigin,
				propertyList, intAclInfo);
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
OW_CIMServer::_getAssociationClasses(const OW_String& ns,
		const OW_String& assocClassName, const OW_String& className,
		OW_CIMClassResultHandlerIFC& result, const OW_String& role)
{
	if(!assocClassName.empty())
	{
		// they gave us a class name so we can use the class association index
		// to only look at the ones that could provide associations
		m_mStore.enumClass(ns, assocClassName, result, true, false, true, true);
		OW_CIMClass cc;
		OW_CIMException::ErrNoType rc = m_mStore.getCIMClass(ns, assocClassName, cc);
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
		_staticReferencesClass(cop,0,role,true,false,0,0,&result);

		// TODO: test if this is faster
		//assocHelper helper(result, m_mStore, ns);
		//m_mStore.getTopLevelAssociations(ns, helper);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMServer::_isInStringArray(const OW_StringArray& sra, const OW_String& val)
{
	for(size_t i = 0; i < sra.size(); i++)
	{
		if(sra[i].equalsIgnoreCase(val))
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMServer::_isDynamicAssoc(const OW_CIMClass& cc)
{
	return _getAssociatorProvider(cc) ? true : false;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_validatePropagatedKeys(const OW_String& ns,
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

		OW_CIMProperty cp = ci.getProperty(kprops[i].getName());
		if(!cp || !cp.getValue())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Cannot create instance. Propagated key field missing:"
					" %1", kprops[i].getName()).c_str());
		}

		OW_Map<OW_String, OW_CIMPropertyArray>::iterator it = theMap.find(cls);
		if(it != theMap.end())
		{
			it->second.append(cp);
		}
		else
		{
			OW_CIMPropertyArray tprops;
			tprops.append(cp);
			theMap[cls] = tprops;
		}
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
		OW_CIMClass cc;
		OW_String clsname = it->first;
		int idx = clsname.indexOf('.');
		if (idx != -1)
		{
			clsname = clsname.substring(0,idx);
		}
		
		op.setObjectName(clsname);
		op.setKeys(it->second);

		OW_CIMException::ErrNoType err = m_mStore.getCIMClass(ns, clsname, cc);
		if(err != OW_CIMException::SUCCESS)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Failed to get class %1 for propagated key: %2 err = %3",
					ns + "/" + clsname, it->first, err).c_str());
		}

		if(!m_iStore.instanceExists(ns, op, cc))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Propagated keys refer to non-existent object: %1",
					op.toString()).c_str());
		}

		it++;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_setProviderProperties(const OW_String& ns,
	const OW_CIMInstance& ci,
	const OW_CIMClass& theClass, const OW_ACLInfo& aclInfo)
{
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	// Get the properties from the class so we can check for provider
	// qualifiers
	OW_CIMPropertyArray pra = theClass.getAllProperties();
	for(size_t i = 0; i < pra.size(); i++)
	{
		OW_CIMProperty clsProp = pra[i];

		// Get the provider for the property
		OW_PropertyProviderIFCRef propp = _getPropertyProvider(clsProp);
		if(propp)
		{	// We have a provider for this property
			OW_CIMProperty cp = ci.getProperty(clsProp.getName());
			if(cp)
			{
				OW_CIMValue cv = cp.getValue();
				OW_CIMObjectPath cop(ci);
				propp->setPropertyValue(createProvEnvRef(real_ch),
					ns, cop, cp.getOriginClass(),
					cp.getName(), cv);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_getProviderProperties(const OW_String& ns, const OW_CIMObjectPath& cop,
	OW_CIMInstance& ci, const OW_CIMClass& theClass, const OW_ACLInfo& aclInfo,
	const OW_StringArray* propertyList)
{
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	// Get the properties from the class so we can check for provider
	// qualifiers
	OW_CIMPropertyArray pra = theClass.getAllProperties();
	for(size_t i = 0; i < pra.size(); i++)
	{
		OW_CIMProperty clsProp = pra[i];

		if (propertyList)
		{
			if (std::find(propertyList->begin(), propertyList->end(),
				clsProp.getName()) == propertyList->end())
			{
				continue;
			}
		}
		OW_PropertyProviderIFCRef propp = _getPropertyProvider(clsProp);
		if(propp)
		{	// We have a provider for this property
			clsProp.setValue(propp->getPropertyValue(
				createProvEnvRef(real_ch), ns, cop,
				clsProp.getOriginClass(), clsProp.getName()));
			ci.setProperty(clsProp);
		}
	}
}

void
OW_CIMServer::checkGetClassRvalAndThrow(OW_CIMException::ErrNoType rval,
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
OW_CIMServer::checkGetClassRvalAndThrowInst(OW_CIMException::ErrNoType rval,
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

const char* const OW_CIMServer::INST_REPOS_NAME = "instances";
const char* const OW_CIMServer::META_REPOS_NAME = "schema";
const char* const OW_CIMServer::NS_REPOS_NAME = "namespaces";
const char* const OW_CIMServer::ASSOC_REPOS_NAME = "association";
const char* const OW_CIMServer::NAMESPACE_PROVIDER = "owcimomd::namespace";
