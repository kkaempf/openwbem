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
#include "OW_NameSpaceProvider.hpp"
#include "OW_RepositoryStreams.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Format.hpp"
#include "OW_WQLIFC.hpp"
#include "OW_CIM.hpp"
#include "OW_Assertion.hpp"

OW_Reference<OW_AccessMgr> m_accessMgr;

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
	 * @param cop	The CIM object path the operation is being done on.
	 * @param aclInfo The ACL info for the user request.
	 */
	void checkAccess(int op, const OW_CIMObjectPath& cop,
		const OW_ACLInfo& aclInfo);

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
void
OW_AccessMgr::checkAccess(int op, const OW_CIMObjectPath& cop,
	const OW_ACLInfo& aclInfo)
{
	OW_String ns = cop.getNameSpace();
	checkAccess(op, ns, aclInfo);
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
	for(;;)
	{
		if (aclInfo.getUserName().length() > 0)
		{
			OW_String superUser =
				m_env->getConfigItem(OW_ConfigOpts::ACL_SUPERUSER_opt);

			if (superUser.equalsIgnoreCase(aclInfo.getUserName()))
			{
				m_env->logDebug("User is SuperUser: checkAccess returning.");
				return;
			}
			OW_CIMObjectPath cop("OpenWBEM_UserACL", "root/security");

			OW_CIMClass cc = m_pServer->getClass(cop, false, true, true, NULL,
				intACLInfo);

			if (!cc)
			{
				m_env->logDebug("OpenWBEM_UserACL class non-existent in"
					" /root/security. ACLs disabled");
				return;
			}
			cop.addKey("username", OW_CIMValue(aclInfo.getUserName()));
			cop.addKey("nspace", OW_CIMValue(lns));
			OW_CIMInstance ci;
			try
			{
				ci = m_pServer->getCIMInstance(cop, false, true, true, NULL,
					NULL, intACLInfo);
			}
			catch(const OW_CIMException&)
			{
				ci.setNull();
			}
			if (ci)
			{
				OW_String capability;
				OW_CIMValue cv = ci.getProperty("capability").getValue();
				if (cv)
				{
					cv.get(capability);
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
		OW_CIMObjectPath cop("OpenWBEM_NamespaceACL", "root/security");
		OW_CIMClass cc = m_pServer->getClass(cop, false, true, true, NULL,
			intACLInfo);
		if (!cc)
		{
			m_env->logDebug("OpenWBEM_NamespaceACL class non-existent in"
				" /root/security. namespace ACLs disabled");
			return;
		}
		cop.addKey("nspace", OW_CIMValue(lns));
		OW_CIMInstance ci;
		try
		{
			ci = m_pServer->getCIMInstance(cop, false, true, true, NULL,
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
			ci.getProperty("capability").getValue().get(capability);
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
	, m_nameSpaceClass()
	, m_env(env)
{
	// Add the name space provider to the provider manager
	m_provManager->addCIMOMProvider(OW_String(NAMESPACE_PROVIDER),
		OW_CppProviderBaseIFCRef(OW_SharedLibraryRef(), new OW_NameSpaceProvider));
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMServer::~OW_CIMServer()
{
	close();
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
OW_CIMServer::createNameSpace(const OW_CIMNameSpace& ns,
	const OW_ACLInfo& aclInfo)
{
	OW_String nsName = ns.getNameSpace();
	if(nsName.length() == 0)
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	OW_StringArray nameComps = nsName.tokenize("/");

	OW_String parns;
	size_t sz = nameComps.size() - 1;
	for(size_t i = 0; i < sz; i++)
	{
		if(parns.length() > 0)
		{
			parns += "/";
		}

		parns += nameComps[i].toString().toLowerCase();
	}

	// Check to see if user has rights to create the namespace
	m_accessMgr->checkAccess(OW_AccessMgr::CREATENAMESPACE, parns, aclInfo);

	m_env->logDebug(format("OW_CIMServer creating namespace: %1", nsName));

	if(m_nStore.createNameSpace(nameComps) == -1)
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS,
			ns.getNameSpace().c_str());
	}

	m_iStore.createNameSpace(nameComps, true);
	m_mStore.createNameSpace(nameComps, true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteNameSpace(const OW_CIMNameSpace& ns,
	const OW_ACLInfo& aclInfo)
{
	OW_String nsName = ns.getNameSpace();

	// Check to see if user has rights to delete the namespace
	m_accessMgr->checkAccess(OW_AccessMgr::DELETENAMESPACE, nsName, aclInfo);

	if(nsName.length() == 0)
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	m_nStore.deleteNameSpace(nsName);
	m_iStore.deleteNameSpace(nsName);
	m_mStore.deleteNameSpace(nsName);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_CIMServer::getQualifierType(const OW_CIMObjectPath& objPath,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get a qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::GETQUALIFIER, objPath, aclInfo);

	return m_mStore.getQualifierType(objPath.getNameSpace(),
		objPath.getObjectName());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_CIMServer::enumQualifierTypes(const OW_CIMObjectPath& path,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get a qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEQUALIFIERS, path, aclInfo);

	return m_mStore.enumQualifierTypes(path.getNameSpace());
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_CIMServer::enumNameSpace(const OW_CIMNameSpace& ns, OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	OW_String nsName(ns.getNameSpace());
	nsName.toLowerCase();

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
	OW_StringArray ra;

	while(nsNode)
	{
		if(!deep)
		{
			ra.append(nsNode.getKey());
		}
		else
		{
			_getChildKeys(hdl.getHandle(), ra, nsNode);
		}
		nsNode = hdl->getNextSibling(nsNode);
	}

	return ra;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
void
OW_CIMServer::_getChildKeys(OW_HDBHandle hdl, OW_StringArray& ra,
	OW_HDBNode node)
{
	ra.append(node.getKey());
	node = hdl.getFirstChild(node);
	while(node)
	{
		_getChildKeys(hdl, ra, node);
		node = hdl.getNextSibling(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::deleteQualifierType(const OW_CIMObjectPath& objPath,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to delete the qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEQUALIFIER, objPath, aclInfo);

	if(!m_mStore.deleteQualifierType(objPath.getNameSpace(),
		objPath.getObjectName()))
	{
		OW_THROWCIM(OW_CIMException::NOT_FOUND);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::addQualifierType(const OW_CIMObjectPath& name,
	const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to add the qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::SETQUALIFIER, name, aclInfo);

	m_mStore.addQualifierType(name.getNameSpace(), qt);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::updateQualifierType(const OW_CIMObjectPath& name,
	const OW_CIMQualifierType& qt, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to update the qualifier
	m_accessMgr->checkAccess(OW_AccessMgr::SETQUALIFIER, name, aclInfo);
	m_mStore.updateQualiferType(name.getNameSpace(), qt);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::getClass(const OW_CIMObjectPath& path, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	OW_CIMClass theClass = _getNameSpaceClass(path.getObjectName());
	if(!theClass)
	{
		// Check to see if user has rights to get the class
		m_accessMgr->checkAccess(OW_AccessMgr::GETCLASS, path, aclInfo);
		theClass = m_mStore.getCIMClass(path);
	}

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

	return theClass;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::deleteClass(const OW_CIMObjectPath& path,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to delete the class
	m_accessMgr->checkAccess(OW_AccessMgr::DELETECLASS, path, aclInfo);

	OW_CIMClass cc = m_mStore.getCIMClass(path);
		
	if(!cc)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
			path.getObjectName().c_str());
	}

	//if(m_iStore.classHasInstances(path))
	//{
	//	OW_THROWCIM(OW_CIMException::CLASS_HAS_INSTANCES);
	//}

	OW_String ns = path.getNameSpace();
	OW_String cname = path.getObjectName();
	if(!m_mStore.deleteClass(ns, cname))
	{
		OW_THROWCIM(OW_CIMException::NOT_FOUND);
	}

	m_iStore.deleteClass(ns, cname);
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::createClass(const OW_CIMObjectPath& path, OW_CIMClass& cimClass,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to create the class
	m_accessMgr->checkAccess(OW_AccessMgr::CREATECLASS, path, aclInfo);

	if(cimClass.getName().equals(OW_CIMClass::NAMESPACECLASS))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
			format("Creation of class %1 is not allowed",
				cimClass.getName()).c_str());
	}

	OW_String ns = path.getNameSpace();
	m_mStore.createClass(ns, cimClass);
	m_iStore.createClass(ns, cimClass);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::updateClass(const OW_CIMObjectPath& name, OW_CIMClass& cc,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to create the class
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYCLASS, name, aclInfo);

	if (!cc)
	{
		OW_THROW(OW_Exception, "Calling updateClass with a NULL class");
	}

	OW_CIMClass origClass = m_mStore.getCIMClass(name);

	if (!origClass)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
			format("Attempting to modify class \"%1\" that doesn't exist",
			cc.getName()).c_str());
	}

	m_mStore.updateClass(name.getNameSpace(), cc);
	return origClass;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_CIMServer::enumClass(const OW_CIMObjectPath& path,
		OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate classes
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSES, path, aclInfo);
	return m_mStore.enumClass(path.getNameSpace(), path.getObjectName(), deep,
		localOnly, includeQualifiers, includeClassOrigin);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMServer::enumClassNames(const OW_CIMObjectPath& path, OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate classes
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATECLASSNAMES, path, aclInfo);

	OW_CIMClassEnumeration cenum = m_mStore.enumClass(path.getNameSpace(),
        path.getObjectName(), deep, false, true, true);

	OW_CIMObjectPath lcop(path);
	lcop.setKeys(OW_CIMPropertyArray());

	OW_CIMObjectPathEnumeration openum;
	while(cenum.hasMoreElements())
	{
		OW_CIMClass cc = cenum.nextElement();
		lcop.setObjectName(cc.getName());
		openum.addElement(lcop);
	}

	return openum;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMServer::getCIMInstanceNames(const OW_CIMObjectPath& path, OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate instance names
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCENAMES, path,
		aclInfo);

	OW_ACLInfo intAclInfo;
	OW_CIMObjectPathEnumeration en;
	OW_CIMObjectPath lcop(path);
	OW_CIMClass theClass = _getNameSpaceClass(path.getObjectName());
	if(!theClass)
	{
		theClass = m_mStore.getCIMClass(lcop);

		if(!theClass)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
				lcop.getObjectName().c_str());
		}
	}

	_getCIMInstanceNames(lcop, theClass, en, deep, aclInfo);

	// If this is the namespace class then just return now
	if(theClass.getName().equals(OW_CIMClass::NAMESPACECLASS)
		|| !deep)
	{
		return en;
	}

	OW_StringArray classNames = m_mStore.getClassChildren(lcop.getNameSpace(),
		theClass.getName());

	for(size_t i = 0; i < classNames.size(); i++)
	{
		lcop.setObjectName(classNames[i]);
		theClass = m_mStore.getCIMClass(lcop);

		if(!theClass)
		{
			OW_String msg("Invalid class in repository: ");
			msg += classNames[i];
			OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
		}

		_getCIMInstanceNames(lcop, theClass, en, deep, aclInfo);
	}

	return en;
}

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
OW_CIMServer::_getCIMInstanceNames(const OW_CIMObjectPath cop,
	const OW_CIMClass& theClass, OW_CIMObjectPathEnumeration& en, OW_Bool deep,
	const OW_ACLInfo& aclInfo)
{
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);

	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	OW_CIMQualifier cq;

	if(!theClass.isAssociation())
	{
		cq = theClass.getQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	}

	if(cq)
	{
		OW_InstanceProviderIFCRef instancep =
			m_provManager->getInstanceProvider(createProvEnvRef(internal_ch),
				cq);

		if(!instancep)
		{
			OW_String msg("Unknown provider: ");
			msg += cq.getValue().toString();
			OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, msg.c_str());
		}

		OW_CIMObjectPathEnumeration theEnum = instancep->enumInstances(
			createProvEnvRef(real_ch),
			cop, deep, theClass);

		while(theEnum.hasMoreElements())
		{
			en.addElement(theEnum.nextElement());
		}
	}
	else
	{
		m_iStore.getInstanceNames(cop, theClass, en);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMServer::getCIMInstances(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to enumerate instances
	m_accessMgr->checkAccess(OW_AccessMgr::ENUMERATEINSTANCES, path, aclInfo);

	OW_ACLInfo intAclInfo;
	OW_CIMInstanceEnumeration en;
	OW_CIMObjectPath lcop(path);
	OW_CIMClass theClass = _getNameSpaceClass(path.getObjectName());
	if(!theClass)
	{
		theClass = m_mStore.getCIMClass(lcop);
		if(!theClass)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
				lcop.getObjectName().c_str());
		}
	}

	OW_Bool deepHonored = _getCIMInstances(lcop, theClass, en, deep, localOnly,
		includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	// DEBUG
	//OW_ASSERT(en.numberOfElements() == 1);
	//OW_CIMInstance debugInst = en.nextElement();
	//en.addElement(debugInst);
	//OW_CIMPropertyArray debugProps = debugInst.getKeyValuePairs();
	//cerr << "*** number of properties: " << debugProps.size() << endl;
	//OW_CIMProperty debugProp = debugProps[0];
	//cerr << "*** property name: " << debugProp.getName() << " value: " << debugProp.getValue() << endl;
	// end of DEBUG

	// If this is the namespace class or we're not going deep or the deep flag
	// was already honored, then we're done
	if(theClass.getName().equals(OW_CIMClass::NAMESPACECLASS)
		|| !deep
		|| deepHonored)
	{
		return en;
	}

	// Now set the deep flag to false, because we are going to enumerate all
	// the child class instances explicitly. We don't want the instance
	// providers to go deep now.
	deep = false;

	OW_StringArray classNames = m_mStore.getClassChildren(lcop.getNameSpace(),
		theClass.getName());

	for(size_t i = 0; i < classNames.size(); i++)
	{
		lcop.setObjectName(classNames[i]);
		theClass = m_mStore.getCIMClass(lcop);
		if(!theClass)
		{
			OW_String msg("Invalid class in repository: ");
			msg += classNames[i];
			OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
		}

		_getCIMInstances(lcop, theClass, en, deep, localOnly,
			includeQualifiers, includeClassOrigin, propertyList, aclInfo);
	}

	return en;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE
OW_Bool
OW_CIMServer::_getCIMInstances(const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass, OW_CIMInstanceEnumeration& en, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	OW_StringArray lpropList;
	if(propertyList)
	{
		lpropList = *propertyList;
	}

	OW_Bool deepHonored = false;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo, true);

	OW_CIMQualifier cq;

	if(!theClass.isAssociation())
	{
		cq = theClass.getQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	}

	if(cq)
	{
		OW_InstanceProviderIFCRef instancep =
			m_provManager->getInstanceProvider(createProvEnvRef(internal_ch),
				cq);

		if(!instancep)
		{
			OW_String msg("Unknown provider: ");
			msg += cq.getValue().toString();
			OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, msg.c_str());
		}

		OW_CIMInstanceEnumeration theEnum = instancep->enumInstances(
			createProvEnvRef(real_ch), cop, deep, theClass, localOnly);
		deepHonored = true;

		while(theEnum.hasMoreElements())
		{
			OW_CIMInstance ci = theEnum.nextElement();
			OW_CIMObjectPath lcop(cop);
			lcop.setObjectName(ci.getClassName());
			lcop.setKeys(ci.getKeyValuePairs());

			_getProviderProperties(lcop, ci, theClass, aclInfo);
			en.addElement(ci.clone(localOnly, includeQualifiers,
				includeClassOrigin, lpropList));
		}
	}
	else
	{
		m_iStore.getCIMInstances(cop, theClass, en, localOnly,
			includeQualifiers, includeClassOrigin, propertyList, this,
			&aclInfo);
	}

	return deepHonored;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::getCIMInstance(const OW_CIMObjectPath& cop, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	return getCIMInstance(cop, localOnly, includeQualifiers, includeClassOrigin,
		propertyList, NULL, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::getCIMInstance(const OW_CIMObjectPath& cop, OW_Bool localOnly,
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
	m_accessMgr->checkAccess(OW_AccessMgr::GETINSTANCE, cop, aclInfo);

	OW_CIMClass cc;
	OW_CIMInstance ci;
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(cop, cc,
		aclInfo);

	if(instancep)
	{
		ci = instancep->getInstance(
			createProvEnvRef(real_ch),
				cop, cc, localOnly);
	}
	else
	{
		cc = m_mStore.getCIMClass(cop);

		if (!cc) // can't call getCIMInstance with a null cc
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
				format("%1", cop.getObjectName()).c_str());
		}

		ci = m_iStore.getCIMInstance(cop, cc);
	}

	if(ci)
	{
		if(pOutClass)
		{
			*pOutClass = cc;
		}

		ci.syncWithClass(cc, true);
		_getProviderProperties(cop, ci, cc, aclInfo);
		ci = ci.clone(localOnly, includeQualifiers, includeClassOrigin,
			propertyList);
	}

	return ci;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::deleteInstance(const OW_CIMObjectPath& cop,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to delete the instance
	m_accessMgr->checkAccess(OW_AccessMgr::DELETEINSTANCE, cop, aclInfo);

	m_env->logDebug(format("OW_CIMServer::deleteInstance.  cop = %1",
		cop.toString()));

	OW_ACLInfo intAclInfo;

	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);

	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	OW_CIMClass theClass;
	OW_CIMInstance oldInst = getCIMInstance(cop, false, true, true, NULL,
		&theClass, intAclInfo);

	if (!oldInst)
	{
		OW_THROWCIM(OW_CIMException::NOT_FOUND);
	}

	OW_AssocDbHandle hdl = m_assocDb.getHandle();

	// Ensure no associations exist for this instance
	OW_String instStr = cop.toString();
	if(hdl.isEntries(instStr))
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED,
			format("Instance has associations: %1", instStr).c_str());
	}

	// If we're deleting an association instance, then remove all
	// traces of it in the association database.
	if(theClass.isAssociation())
	{
		m_env->logDebug("OW_CIMServer::deleteInstance."
			" theClass.isAssociation() == true");

		OW_AssociatorProviderIFCRef assocP(0);
		OW_CIMQualifier cq = theClass.getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROVIDER);

		if(cq)
		{
			assocP = m_provManager->getAssociatorProvider(
				createProvEnvRef(internal_ch), cq);
		}

		// If there is no associator provider for this instance, then go ahead
		// delete the entries from the database for this association.
		if(!assocP)
		{
			hdl.deleteEntries(cop, oldInst);
		}
	}

	OW_CIMClass tc;
	OW_InstanceProviderIFCRef instancep = _getInstanceProvider(cop, tc,
		aclInfo);

	if(instancep)	// If there is an instance provider, let it do the delete.
	{
		instancep->deleteInstance(
			createProvEnvRef(real_ch), cop);
	}
	else
	{
		// Delete the instance from the instance repository
		m_iStore.deleteInstance(cop, theClass);
	}

	return oldInst;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_CIMServer::createInstance(const OW_CIMObjectPath& cop, OW_CIMInstance& ci,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMObjectPath rval = cop;
	// Check to see if user has rights to create the instance
	m_accessMgr->checkAccess(OW_AccessMgr::CREATEINSTANCE, cop, aclInfo);

	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		OW_ACLInfo(), true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	OW_CIMClass theClass = _getNameSpaceClass(cop.getObjectName());
	if(!theClass)
	{
		theClass = m_mStore.getCIMClass(cop.getNameSpace(), ci.getClassName());

		if(!theClass)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
				cop.getObjectName().c_str());
		}
	}

	OW_CIMQualifier acq = theClass.getQualifier(
			OW_CIMQualifier::CIM_QUAL_ABSTRACT);
	if(acq)
	{
		OW_CIMValue v = acq.getValue();
		if (v)
		{
			OW_Bool b;
			v.get(b);
			if (b)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
						format("Unable to create instance because class (%1)"
							" is abstract", theClass.getName()).c_str());
			}
		}
	}

	OW_CIMQualifier cq = theClass.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);

	OW_Bool created = false;
	if(cq)
	{
		OW_InstanceProviderIFCRef instancep =
			m_provManager->getInstanceProvider(createProvEnvRef(internal_ch),
				cq);

		if(instancep)
		{
			rval = instancep->createInstance(createProvEnvRef(real_ch), cop, ci);
			created = true;
		}
	}

	if(!created)
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

				OW_CIMClass rcc = m_mStore.getCIMClass(op);

				if(rcc)
				{
					OW_CIMInstance rci = m_iStore.getCIMInstance(op, rcc);

					if(!rci)
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

		m_iStore.createInstance(cop, theClass, ci);
		_validatePropagatedKeys(cop, ci, theClass);
	}

	if(theClass.isAssociation())
	{
		OW_AssociatorProviderIFCRef assocP(0);

		if(cq)
		{
			assocP = m_provManager->getAssociatorProvider(
				createProvEnvRef(internal_ch), cq);
		}

		if(!assocP)
		{
			OW_AssocDbHandle hdl = m_assocDb.getHandle();
			OW_CIMObjectPath icop(cop);
			icop.setKeys(ci.getKeyValuePairs());
			hdl.addEntries(icop, ci);
		}
	}

	_setProviderProperties(cop, ci, theClass, aclInfo);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_CIMServer::updateInstance(const OW_CIMObjectPath& cop, OW_CIMInstance& ci,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to modify the instance
	m_accessMgr->checkAccess(OW_AccessMgr::MODIFYINSTANCE, cop, aclInfo);

	OW_ACLInfo intAclInfo;
	OW_CIMInstance oldInst = getCIMInstance(cop, false, true, true, NULL,
		NULL, intAclInfo);

	if (!oldInst)
	{
		OW_THROWCIM(OW_CIMException::NOT_FOUND);
	}

	OW_CIMClass theClass = m_mStore.getCIMClass(cop.getNameSpace(),
		ci.getClassName());

	if(!theClass)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
			cop.getObjectName().c_str());
	}

	OW_CIMQualifier cq = theClass.getQualifier(
		OW_CIMQualifier::CIM_QUAL_PROVIDER);

	OW_InstanceProviderIFCRef instancep(0);
	if(cq)
	{
		OW_CIMClass tc;
		instancep = _getInstanceProvider(cop, tc, aclInfo);
	}

	if(!instancep)
	{
		// No instance provider qualifier found
		m_iStore.updateInstance(cop, theClass, ci);
	}
	else
	{
		// Look for dynamic instances
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, true);
		instancep->setInstance(createProvEnvRef(real_ch), cop, ci);
	}

	if(theClass.isAssociation())
	{
		OW_LocalCIMOMHandle ch(m_env, OW_RepositoryIFCRef(this, true),
			OW_ACLInfo(), true);
		OW_AssociatorProviderIFCRef assocP(0);

		if(cq)
		{
			assocP = m_provManager->getAssociatorProvider(
				createProvEnvRef(ch), cq);
		}

		if(!assocP)
		{
			OW_AssocDbHandle adbHdl = m_assocDb.getHandle();
			adbHdl.deleteEntries(cop, oldInst);
			adbHdl.addEntries(cop, ci);
		}
	}

	_setProviderProperties(cop, ci, theClass, aclInfo);
	return oldInst;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_CIMServer::_instanceExists(const OW_CIMObjectPath& icop,
	const OW_ACLInfo& aclInfo)
{
	OW_CIMClass cc;
	OW_InstanceProviderIFCRef ip = _getInstanceProvider(icop, cc, aclInfo);
	OW_String classname = icop.getObjectName();

	if(ip)
	{
		//
		//  Ask the provider for the instance
		//
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, true);

		return OW_Bool((ip->getInstance(createProvEnvRef(real_ch), icop, cc,
			false)) != 0);
	}
	else
	{
		cc = m_mStore.getCIMClass(icop);
		if (!cc)
		{
			return false;
		}
	}

	return m_iStore.instanceExists(icop, cc);
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMServer::getProperty(const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::GETPROPERTY, name, aclInfo);

	OW_ACLInfo intAclInfo;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	OW_CIMClass theClass = m_mStore.getCIMClass(name);
	if(!theClass)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
			name.getObjectName().c_str());
	}

	OW_CIMProperty cp = theClass.getProperty(propertyName);
	if(!cp)
	{
		OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
			propertyName.c_str());
	}

	OW_CIMQualifier cq = cp.getQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if(!cq)
	{
		OW_CIMInstance ci = getCIMInstance(name, false, true, true, NULL,
			NULL, aclInfo);
		OW_CIMProperty prop = ci.getProperty(propertyName);
		if(!prop)
		{
			OW_THROWCIMMSG(OW_CIMException::NO_SUCH_PROPERTY,
				propertyName.c_str());
		}

		return prop.getValue();
	}

	OW_PropertyProviderIFCRef propp = m_provManager->getPropertyProvider(
		createProvEnvRef(internal_ch), cq);
	if(!propp)
	{
		OW_String msg("Unknown provider: ");
		msg += cq.getValue().toString();
		OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, msg.c_str());
	}

	return propp->getPropertyValue(
		createProvEnvRef(real_ch),
			name, cp.getOriginClass(), propertyName);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::setProperty(const OW_CIMObjectPath& name,
	const OW_String& propertyName, const OW_CIMValue& valueArg,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::SETPROPERTY, name, aclInfo);

	OW_ACLInfo intAclInfo;
	OW_CIMClass theClass = m_mStore.getCIMClass(name);
	if(!theClass)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
			name.getObjectName().c_str());
	}

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
		cv = OW_CIMValueCast::castValueToDataType(cv, cp.getDataType());
	}

	OW_CIMQualifier cq = cp.getQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if(!cq)
	{
		OW_CIMInstance ci = getCIMInstance(name, false, true, true, NULL,
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
			OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, msg.c_str());
		}

		cp.setValue(cv);
		ci.setProperty(cp);
		updateInstance(name, ci, intAclInfo);
	}
	else
	{
		OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
			intAclInfo, true);
		OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
			aclInfo, true);

		OW_PropertyProviderIFCRef propp = m_provManager->getPropertyProvider(
			createProvEnvRef(internal_ch), cq);
		if(!propp)
		{
			OW_String msg("Unknown provider: ");
			if(cq.getValue())
			{
				msg += cq.getValue().toString();
			}
			OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, msg.c_str());
		}

		propp->setPropertyValue(
			createProvEnvRef(real_ch),
				name, cp.getOriginClass(), cp.getName(), cv);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue
OW_CIMServer::invokeMethod(const OW_CIMObjectPath& name,
	const OW_String& methodName, const OW_CIMValueArray& inParams,
	OW_CIMValueArray& outParams, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get the property
	m_accessMgr->checkAccess(OW_AccessMgr::INVOKEMETHOD, name, aclInfo);

	// Not creating any read/write locks here because the provider
	// could call back into the CIM Server for something and then we
	// could end up with a deadlock condition.

	OW_ACLInfo intAclInfo;
	OW_CIMMethod method;
	OW_CIMClass cc = m_mStore.getCIMClass(name);
	if(!cc)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, name.toString().c_str());
	}

	OW_CIMPropertyArray keys = name.getKeys();

	// If this is an instance, ensure it exists.
	if(keys.size() > 0)
	{
		if(!_instanceExists(name, aclInfo))
		{
			// Dynamic - Get the provider information
			OW_CIMQualifier cq = cc.getQualifier(
				OW_CIMQualifier::CIM_QUAL_PROVIDER);

			if(!cq)
			{
				OW_String msg("Instance not found: ");
				msg += OW_String(name);
				OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, msg.c_str());
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
			OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED,
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

		cv = methodp->invokeMethod(
			createProvEnvRef(real_ch),
				name, methodName, inParams, outParams);
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
// Get the instance provider information - we locate the provider by traversing
// up the class hierarchy to find the provider closest to the class that is
// identified by the instance
//
// Assumes a read or write lock has been acquired on the repository
OW_InstanceProviderIFCRef
OW_CIMServer::_getInstanceProvider(const OW_CIMObjectPath cop,
	OW_CIMClass& outClass, const OW_ACLInfo& aclInfo)
{
	return _getInstanceProvider(cop.getNameSpace(), cop.getObjectName(),
		outClass, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
// Assumes a read or write lock has been acquired on the repository
OW_InstanceProviderIFCRef
OW_CIMServer::_getInstanceProvider(const OW_String& ns,
	const OW_String& classNameArg, OW_CIMClass& outClass,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_LocalCIMOMHandle ch(m_env, OW_RepositoryIFCRef(this, true), OW_ACLInfo(),
		true);

	OW_String className(classNameArg);
	OW_CIMClass cc = _getNameSpaceClass(className);

	if(cc)
	{
		outClass = cc;
		OW_CIMQualifier cq = cc.getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROVIDER);

		return m_provManager->getInstanceProvider(
			createProvEnvRef(ch), cq);
	}

	outClass.setNull();
	while(className.length() > 0)
	{
		try
		{
			cc = m_mStore.getCIMClass(ns, className);
			if(!cc)
			{
				break;
			}

			if(!outClass)
			{
				outClass = cc;
			}

			if(cc.isAssociation())
			{
				break;
			}

			OW_CIMQualifier cq = cc.getQualifier(
				OW_CIMQualifier::CIM_QUAL_PROVIDER);

			if(cq)
			{
				try
				{
					return m_provManager->getInstanceProvider(
						createProvEnvRef(ch), cq);
				}
				catch(...)
				{
					// Ignore?
				}
			}

			className = cc.getSuperClass();
		}
		catch(...)
		{
			className = "";
		}
	}

	return OW_InstanceProviderIFCRef(NULL);
}

//////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_CIMServer::_getNameSpaceClass(const OW_String& className)
{
	if(!className.equalsIgnoreCase(OW_CIMClass::NAMESPACECLASS))
	{
		return OW_CIMClass();
	}

	if(!m_nameSpaceClass)
	{
		m_nameSpaceClass = OW_CIMClass(OW_CIMClass::NAMESPACECLASS);

		OW_CIMQualifier cimQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
		cimQualifier.setValue(OW_CIMValue(OW_String(NAMESPACE_PROVIDER)));
		OW_CIMProperty cimProp(OW_CIMProperty::NAME_PROPERTY);
		cimProp.setDataType(OW_CIMDataType(OW_CIMDataType::STRING));
		cimProp.addQualifier(OW_CIMQualifier::createKeyQualifier());
		m_nameSpaceClass.addQualifier(cimQualifier);
		m_nameSpaceClass.addProperty(cimProp);
	}

	return m_nameSpaceClass;
}

//////////////////////////////////////////////////////////////////////
OW_CIMInstanceArray
OW_CIMServer::execQuery(const OW_CIMNameSpace& ns, const OW_String &query,
	const OW_String& queryLanguage, const OW_ACLInfo& aclInfo)
{
	OW_WQLIFCRef wql = m_env->getWQLRef();
	if (wql && wql->supportsQueryLanguage(queryLanguage))
	{
		OW_CIMOMHandleIFCRef lch(new OW_LocalCIMOMHandle(m_env,
			OW_RepositoryIFCRef(this, true), aclInfo, true));

		OW_CIMInstanceArray rval = wql->evaluate(ns, query, queryLanguage,
			lch);

		return rval;
	}
	else
	{
		OW_THROWCIM(OW_CIMException::QUERY_LANGUAGE_NOT_SUPPORTED);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMServer::associators(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORS, path, aclInfo);

	OW_CIMInstanceEnumeration ienum;
	_commonAssociators(path, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, &ienum, NULL,
		aclInfo);

	return ienum;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMServer::associatorNames(const OW_CIMObjectPath& path,
	const OW_String& assocClass, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::ASSOCIATORNAMES, path, aclInfo);

	OW_CIMObjectPathEnumeration ope;
	_commonAssociators(path, assocClass, resultClass, role, resultRole,
		false, false, NULL, NULL, &ope, aclInfo);

	return ope;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMServer::references(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCES, path, aclInfo);

	OW_CIMInstanceEnumeration ie;
	_commonReferences(path, resultClass, role, includeQualifiers,
		includeClassOrigin, propertyList, &ie, NULL, aclInfo);

	return ie;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMServer::referenceNames(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role,
	const OW_ACLInfo& aclInfo)
{
	// Check to see if user has rights to get associators
	m_accessMgr->checkAccess(OW_AccessMgr::REFERENCENAMES, path, aclInfo);

	OW_CIMObjectPathEnumeration ope;
	_commonReferences(path, resultClass, role, false, false, NULL, NULL, &ope,
		aclInfo);

	return ope;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_commonReferences(const OW_CIMObjectPath& path,
	const OW_String& resultClass, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceEnumeration* pienum,
	OW_CIMObjectPathEnumeration* poenum, const OW_ACLInfo& aclInfo)
{
	OW_CIMClass assocClass;
	OW_String ns = path.getNameSpace();

	// Get all association classes from the repository
	// If the assoc class was specified, only children of it will be
	// returned.
	OW_CIMClassEnumeration assocClassEnum = _getAssociationClasses(ns,
		resultClass);

	OW_CIMClassArray staticAssocs;
	OW_CIMClassArray dynamicAssocs;

	// Now separate the association classes that have associator provider from
	// the ones that don't
	while(assocClassEnum.hasMoreElements())
	{
		OW_CIMClass cc = assocClassEnum.nextElement();
		OW_CIMClassArray* pra = (_isDynamicAssoc(cc, aclInfo)) ? &dynamicAssocs
			: &staticAssocs;
		pra->append(cc);
	}

	// Process all of the association classes without providers
	OW_CIMInstanceEnumeration renum;
	_staticReferences(path, staticAssocs, role, includeQualifiers,
		includeClassOrigin, propertyList, pienum, poenum, aclInfo);

	// Process all of the association classes with providers
	_dynamicReferences(path, dynamicAssocs, role, includeQualifiers,
		includeClassOrigin, propertyList, pienum, poenum, aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_dynamicReferences(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& assocClasses, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceEnumeration* pienum,
	OW_CIMObjectPathEnumeration* poenum, const OW_ACLInfo& aclInfo)
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

		OW_CIMQualifier cq = cc.getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROVIDER);

		if(!cq)
		{
			continue;
		}

		OW_AssociatorProviderIFCRef assocP =
			m_provManager->getAssociatorProvider(createProvEnvRef(internal_ch),
				cq);

		if(!assocP)
		{
			continue;
		}

		OW_CIMObjectPath assocClassPath(assocClasses[i].getName(),
			path.getNameSpace());

		// If the object path enumeration pointer is null, then assume we
		// are doing references and not referenceNames
		if(poenum == NULL)
		{
			OW_CIMInstanceEnumeration provienum = assocP->references(
				createProvEnvRef(real_ch),
				assocClassPath, path, role, includeQualifiers,
				includeClassOrigin, propertyList);

			while(provienum.hasMoreElements())
			{
				pienum->addElement(provienum.nextElement());
			}
		}
		else
		{
			OW_CIMObjectPathEnumeration provoenum = assocP->referenceNames(
				createProvEnvRef(real_ch),
				assocClassPath, path, role);

			while(provoenum.hasMoreElements())
			{
				poenum->addElement(provoenum.nextElement());
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_staticReferences(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& refClasses, const OW_String& role,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceEnumeration* pienum,
	OW_CIMObjectPathEnumeration* poenum, const OW_ACLInfo& /*aclInfo*/)
{
	// refClasses should always have something in it
	if(refClasses.size() == 0)
	{
		return;
	}

	OW_StringArray assocClassNames;
	for(size_t i = 0; i < refClasses.size(); i++)
	{
		assocClassNames.append(refClasses[i].getName());
	}

	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();
	OW_AssocDbEntryArray adbentries = dbhdl.getAllEntries(path.toString(),
		assocClassNames, role);

	OW_ACLInfo intAclInfo;
	for(size_t i = 0; i < adbentries.size(); i++)
	{
		OW_CIMObjectPath assocPath = OW_CIMObjectPath::parse(
			adbentries[i].getAssocKey());

		// If we are only doing reference names, then just add the association's
		// object path to the object path enumeration
		// Get the instance of the association
		OW_CIMClass cc;
		OW_CIMInstance ainst = getCIMInstance(assocPath, false, true, true,
			NULL, &cc, intAclInfo);

		if(!ainst)
		{
			OW_THROW(OW_Exception,
				format("Instance not found for association: %1",
					assocPath.toString()).c_str());
		}

		ainst.syncWithClass(cc, includeQualifiers);
		if(poenum != NULL)
		{
			assocPath.setKeys(ainst.getKeyValuePairs());
			poenum->addElement(assocPath);
		}
		else
		{
			pienum->addElement(ainst.clone(false, includeQualifiers,
				includeClassOrigin, propertyList));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_commonAssociators(const OW_CIMObjectPath& path,
	const OW_String& assocClassName, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceEnumeration* pienum,
	OW_CIMObjectPathEnumeration* poenum, const OW_ACLInfo& aclInfo)
{
	OW_CIMClass assocClass;
	OW_String ns = path.getNameSpace();

	// Get all association classes from the repository
	// If the assoc class was specified, only children of it will be
	// returned.
	OW_CIMClassEnumeration assocClassEnum = _getAssociationClasses(ns,
		assocClassName);

	OW_CIMClassArray staticAssocs;
	OW_CIMClassArray dynamicAssocs;

	// Now separate the association classes that have associator provider from
	// the ones that don't
	while(assocClassEnum.hasMoreElements())
	{
		OW_CIMClass cc = assocClassEnum.nextElement();
		OW_CIMClassArray* pra = (_isDynamicAssoc(cc, aclInfo)) ? &dynamicAssocs
			: &staticAssocs;
		pra->append(cc);
	}

	// If the result class was specified, get a list of all the classes the
	// objects must be instances of.
	OW_StringArray resultClassNames;
	if(resultClass.length() > 0)
	{
		resultClassNames = m_mStore.getClassChildren(ns, resultClass);
		resultClassNames.append(resultClass);
	}

	// Process all of the association classes without providers
	OW_CIMInstanceEnumeration renum;
	_staticAssociators(path, staticAssocs, resultClassNames, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, pienum, poenum,
		aclInfo);

	// Process all of the association classes with providers
	_dynamicAssociators(path, dynamicAssocs, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList, pienum, poenum,
		aclInfo);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_dynamicAssociators(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& assocClasses, const OW_String& resultClass,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceEnumeration* pienum,
	OW_CIMObjectPathEnumeration* poenum, const OW_ACLInfo& aclInfo)
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

		OW_CIMQualifier cq = cc.getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROVIDER);

		if(!cq)
		{
			continue;
		}

		OW_AssociatorProviderIFCRef assocP =
			m_provManager->getAssociatorProvider(createProvEnvRef(internal_ch),
				cq);

		if(!assocP)
		{
			continue;
		}

		OW_CIMObjectPath assocClassPath(assocClasses[i].getName(),
			path.getNameSpace());

		if(poenum == NULL)
		{
			OW_CIMInstanceEnumeration provienum = assocP->associators(
				createProvEnvRef(real_ch),
				assocClassPath, path, resultClass, role, resultRole,
				includeQualifiers, includeClassOrigin, propertyList);

			while(provienum.hasMoreElements())
			{
				pienum->addElement(provienum.nextElement());
			}
		}
		else
		{
			OW_CIMObjectPathEnumeration provoenum = assocP->associatorNames(
				createProvEnvRef(real_ch),
				assocClassPath, path, resultClass, role, resultRole);

			while(provoenum.hasMoreElements())
			{
				poenum->addElement(provoenum.nextElement());
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_staticAssociators(const OW_CIMObjectPath& path,
	const OW_CIMClassArray& assocClasses, const OW_StringArray& resultClasses,
	const OW_String& role, const OW_String& resultRole,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, OW_CIMInstanceEnumeration* pienum,
	OW_CIMObjectPathEnumeration* poenum, const OW_ACLInfo& /*aclInfo*/)
{
	// AssocClasses should always have something in it
	if(assocClasses.size() == 0)
	{
		return;
	}

	OW_StringArray assocClassNames;
	for(size_t i = 0; i < assocClasses.size(); i++)
	{
		assocClassNames.append(assocClasses[i].getName());
	}

	OW_AssocDbHandle dbhdl = m_assocDb.getHandle();
	OW_AssocDbEntryArray adbentries = dbhdl.getAllEntries(path.toString(),
		assocClassNames, role);

	OW_ACLInfo intAclInfo;
	for(size_t i = 0; i < adbentries.size(); i++)
	{
		// Get the instance of the association
		OW_CIMObjectPath assocPath = OW_CIMObjectPath::parse(
			adbentries[i].getAssocKey());

		OW_CIMInstance ainst = getCIMInstance(assocPath, false, true, true,
			NULL, NULL, intAclInfo);

		if(!ainst)
		{
			OW_THROW(OW_Exception,
				format("Instance not found for association: %1",
					assocPath.toString()).c_str());
		}

		OW_CIMPropertyArray pra;

		// If result role specified, only get the property with the name of
		// the result role.
		if(resultRole.length() > 0)
		{
			OW_CIMProperty cprop = ainst.getProperty(resultRole);
			if(!cprop)
			{
				continue;
			}

			pra.append(cprop);
		}
		else
		{
			// No result role specified, so get all reference properties
			pra = ainst.getProperties(OW_CIMDataType::REFERENCE);
		}

		// Check instances referred to by the reference properties of the
		// association instance
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

			// Ignore the target object reference
			if(path.equals(op))
			{
				continue;
			}

			// If result class specified, ensure it is a desired class
			if(resultClasses.size() > 0
				&& !_isInStringArray(resultClasses, op.getObjectName()))
			{
				continue;
			}

			OW_CIMClass cc;
			OW_CIMInstance ci = getCIMInstance(op, false, true, true, NULL,
				&cc, intAclInfo);

			if(!ci)
			{
				OW_THROW(OW_Exception,
					format("Instance referenced by association does not exist:"
						" %1", op.toString()).c_str());
			}

			ci.syncWithClass(cc, includeQualifiers);

			if(poenum != NULL)
			{
				op.setKeys(ci.getKeyValuePairs());
				poenum->addElement(op);
			}
			else
			{
				pienum->addElement(ci.clone(false, includeQualifiers,
					includeClassOrigin, propertyList));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_CIMServer::_getAssociationClasses(const OW_String& ns,
	const OW_String& className)
{
	OW_CIMClassEnumeration renum;
	if(className.length() > 0)
	{
		renum = m_mStore.enumClass(ns, className, true, false, true, true);
		OW_CIMClass cc = m_mStore.getCIMClass(ns, className);
		renum.addElement(cc);
	}
	else
	{
		OW_CIMClassEnumeration topAssocs = m_mStore.getTopLevelAssociations(ns);

		while(topAssocs.hasMoreElements())
		{
			OW_CIMClass cc = topAssocs.nextElement();
			renum.addElement(cc);
			m_mStore.enumClass(ns, cc.getName(), true, false, true, true,
				&renum);
		}
	}

	return renum;
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
OW_CIMServer::_isDynamicAssoc(const OW_CIMClass& cc,
	const OW_ACLInfo& /*aclInfo*/)
{
	OW_Bool rv = false;
	OW_CIMQualifier cq = cc.getQualifier(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	if(cq)
	{
		OW_LocalCIMOMHandle ch(m_env, OW_RepositoryIFCRef(this, true),
			OW_ACLInfo(), true);
		OW_AssociatorProviderIFCRef assocP =
			m_provManager->getAssociatorProvider(createProvEnvRef(ch), cq);

		if(assocP)
		{
			rv = true;
		}
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_validatePropagatedKeys(const OW_CIMObjectPath& cop,
	OW_CIMInstance& ci, const OW_CIMClass& theClass)
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
		if(cls.length() == 0)
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
			format("Cannot create instance. Propagated key properties missing:"
				" %1", cop.toString()).c_str());
	}

	OW_String ns = cop.getNameSpace();
	OW_CIMObjectPath op(cop);
	OW_Map<OW_String, OW_CIMPropertyArray>::iterator it = theMap.begin();
	while(it != theMap.end())
	{
		op.setObjectName(it->first);
		op.setKeys(it->second);

		OW_CIMClass cc = m_mStore.getCIMClass(ns, it->first);
		if(!cc)
		{
			OW_THROW(OW_HDBException,
				format("Failed to get class for propagated key: %1",
					it->first).c_str());
		}

		if(!m_iStore.instanceExists(op, cc))
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				format("Propagated keys refer to non-existent object: %1",
					op.toString()).c_str());
		}

		it++;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_setProviderProperties(const OW_CIMObjectPath& cop,
	OW_CIMInstance& ci, const OW_CIMClass& theClass, const OW_ACLInfo& aclInfo)
{
	OW_ACLInfo intAclInfo;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);

	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true), aclInfo,
		true);

	// Get the properties from the class so we can check for provider
	// qualifiers
	OW_CIMPropertyArray pra = theClass.getAllProperties();
	for(size_t i = 0; i < pra.size(); i++)
	{
		OW_CIMProperty clsProp = pra[i];

		// Get the provider qualifier if it exists
		OW_CIMQualifier cq = clsProp.getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROVIDER);

		if(cq)
		{	// We have a provider for this property
			
			OW_CIMProperty cp = ci.getProperty(clsProp.getName());
			if(cp)
			{
				// Get the provider for the property
				OW_PropertyProviderIFCRef propp =
					m_provManager->getPropertyProvider(createProvEnvRef(
						internal_ch), cq);

				// If we found a provider, let it set the value for this
				// property
				if(propp)
				{
					OW_CIMValue cv = cp.getValue();
					propp->setPropertyValue(createProvEnvRef(real_ch),
						cop, cp.getOriginClass(),
						cp.getName(), cv);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_CIMServer::_getProviderProperties(const OW_CIMObjectPath& cop,
	OW_CIMInstance& ci, const OW_CIMClass& theClass, const OW_ACLInfo& aclInfo)
{
	OW_ACLInfo intAclInfo;
	OW_LocalCIMOMHandle internal_ch(m_env, OW_RepositoryIFCRef(this, true),
		intAclInfo, true);
	OW_LocalCIMOMHandle real_ch(m_env, OW_RepositoryIFCRef(this, true),
		aclInfo, true);

	// Get the properties from the class so we can check for provider
	// qualifiers
	OW_CIMPropertyArray pra = theClass.getAllProperties();
	for(size_t i = 0; i < pra.size(); i++)
	{
		OW_CIMProperty clsProp = pra[i];

		// Get the provider qualifier if it exists
		OW_CIMQualifier cq = clsProp.getQualifier(
			OW_CIMQualifier::CIM_QUAL_PROVIDER);

		if(cq)
		{	// We have a provider for this property

			OW_PropertyProviderIFCRef propp =
				m_provManager->getPropertyProvider(createProvEnvRef(
					internal_ch), cq);

			// If we found a provider, ask it for the value of this property
			if(propp)
			{
				clsProp.setValue(propp->getPropertyValue(
					createProvEnvRef(real_ch), cop,
					clsProp.getOriginClass(), clsProp.getName()));
				ci.setProperty(clsProp);
			}
		}
	}
}

const char* const OW_CIMServer::INST_REPOS_NAME = "instances";
const char* const OW_CIMServer::META_REPOS_NAME = "schema";
const char* const OW_CIMServer::NS_REPOS_NAME = "namespaces";
const char* const OW_CIMServer::ASSOC_REPOS_NAME = "association";
const char* const OW_CIMServer::NAMESPACE_PROVIDER = "cimom::namespace";
