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
#include "OW_MetaRepository2.hpp"
#include "OW_DataStreams.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_Array.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Logger.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;

//////////////////////////////////////////////////////////////////////////////
bool
MetaRepository2::nameSpaceExists(const String& ns)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::enumNameSpace(StringResultHandlerIFC&, OperationContext&)
{
}

//////////////////////////////////////////////////////////////////////////////
MetaRepository2::~MetaRepository2()
{
	try
	{
		close();
	}
	catch (...)
	{
		// logDebug or close could throw.
	}
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::open(::DB_ENV* env, ::DB_TXN* txn)
{
	m_namespaces.open("namespaces", env, txn, dbDatabase::E_NO_DUPLICATES);
	m_qualifiers.open("qualifiers", env, txn, dbDatabase::E_NO_DUPLICATES);
	m_classes.open("classes", env, txn, dbDatabase::E_NO_DUPLICATES);
	m_subclasslists.open("subclasslists", env, txn, dbDatabase::E_DUPLICATES);
	m_qualifierlists.open("qualifierlists", env, txn, dbDatabase::E_DUPLICATES);
//     // Create root qualifier container
//     HDBHandleLock hdl(this, getHandle());
//     String qcontk(QUAL_CONTAINER);
//     createRootNode(qcontk, hdl);
//     qcontk += "/" + String("root");
//     createRootNode(qcontk,hdl);
//     // Create root class container
//     qcontk = CLASS_CONTAINER;
//     createRootNode(qcontk, hdl);
//     qcontk += "/" + String("root");
//     createRootNode(qcontk,hdl);
}

//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::close()
{
	m_namespaces.close();
	m_qualifiers.close();
	m_classes.close();
	m_subclasslists.close();
	m_qualifierlists.close();
}

//////////////////////////////////////////////////////////////////////////////
String
MetaRepository2::_makeQualPath(const String& ns, const String& qualName)
{
	OW_ASSERT(!ns.empty());
	OW_ASSERT(!qualName.empty());

	StringBuffer qp(ns);
	qp += "/";
	qp += qualName;
	
	return qp.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
MetaRepository2::_makeClassPath(const String& ns,
	const String& className)
{
	OW_ASSERT(!ns.empty());
	OW_ASSERT(!className.empty());

	StringBuffer cp(ns);
	cp += "/";
	cp += className;
	return cp.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
MetaRepository2::getQualifierType(const String& ns,
	const String& qualName)
{
//     throwIfNotOpen();
//     String qkey = _makeQualPath(ns, qualName);
//     CIMQualifierType qualType = m_qualCache.getFromCache(qkey);
//     if (qualType)
//     {
//         return qualType;
//     }
//     GenericHDBRepository* prep;
//     HDBHandle lhdl;
//     if (phdl)
//     {
//         prep = NULL;
//         lhdl = *phdl;
//     }
//     else
//     {
//         prep = this;
//         lhdl = getHandle();
//     }
//     HDBHandleLock hdl(prep, lhdl);
//     getCIMObject(qualType, qkey, hdl.getHandle());
//     if (!qualType)
//     {
//         if (nameSpaceExists(QUAL_CONTAINER + "/" + ns))
//         {
//             OW_THROWCIMMSG(CIMException::NOT_FOUND,
//                     format("CIM QualifierType \"%1\" not found in namespace: %2",
//                         qualName, ns).c_str());
//         }
//         else
//         {
//             OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
//                     ns.c_str());
//         }
//     }
//     m_qualCache.addToCache(qualType, qkey);
	return CIMQualifierType();
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::enumQualifierTypes(const String& ns,
	CIMQualifierTypeResultHandlerIFC& result)
{
//     throwIfNotOpen();
//     String nskey = _makeQualPath(ns, String());
//     HDBHandleLock hdl(this, getHandle());
//     HDBNode node = hdl->getNode(nskey);
//     if (!node)
//     {
//         OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
//     }
//     if (!node.areAllFlagsOn(HDBNSNODE_FLAG))
//     {
//         OW_THROW(HDBException, "Expected namespace node");
//     }
//     node = hdl->getFirstChild(node);
//     while (node)
//     {
//         // If this is not a namespace node, assume it's a qualifier
//         if (!node.areAllFlagsOn(HDBNSNODE_FLAG))
//         {
//             CIMQualifierType qual(CIMNULL);
//             nodeToCIMObject(qual, node);
//             result.handle(qual);
//         }
//         node = hdl->getNextSibling(node);
//     }
}
//////////////////////////////////////////////////////////////////////////////
bool
MetaRepository2::deleteQualifierType(const String& ns,
	const String& qualName)
{
//     throwIfNotOpen();
//     String qkey = _makeQualPath(ns, qualName);
//     HDBHandleLock hdl(this, getHandle());
//     HDBNode node = hdl->getNode(qkey);
//     if (!node)
//     {
//         // Didn't find a node associated with the key
//         return false;
//     }
//     // To ensure the node is a qualifier type, we create a qualifier
//     // type from the node. If the node is not a qualifier type, an
//     // exception will be thrown
//     CIMQualifierType qt(CIMNULL);
//     nodeToCIMObject(qt, node);
//     // If we've hit this point, we know this is a qualifier
//     hdl->removeNode(node);
//     m_qualCache.removeFromCache(qkey);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::_addQualifierType(const String& ns,
	const CIMQualifierType& qt)
{
//     throwIfNotOpen();
//     if (!qt)
//     {
//         OW_THROWCIM(CIMException::INVALID_PARAMETER);
//     }
//     GenericHDBRepository* prep;
//     HDBHandle lhdl;
//     if (phdl)
//     {
//         prep = 0;
//         lhdl = *phdl;
//     }
//     else
//     {
//         prep = this;
//         lhdl = getHandle();
//     }
//     HDBHandleLock hdl(prep, lhdl);
//     String qkey = _makeQualPath(ns, qt.getName());
//     HDBNode node = hdl->getNode(qkey);
//     if (node)
//     {
//         String msg(ns);
//         if (!ns.empty())
//         {
//             msg += "/";
//         }
//         msg += qt.getName();
//         OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, msg.c_str());
//     }
//     HDBNode pnode = _getQualContainer(hdl, ns);
//     if (!pnode)
//     {
//         OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
//     }
//     addCIMObject(qt, qkey, pnode, hdl.getHandle());
//     m_qualCache.addToCache(qt, qkey);
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::setQualifierType(const String& ns,
	const CIMQualifierType& qt)
{
//     throwIfNotOpen();
//     String qkey = _makeQualPath(ns, qt.getName());
//     HDBHandleLock hdl(this, getHandle());
//     HDBNode node = hdl->getNode(qkey);
//     if (!node)
//     {
//         HDBHandle lhdl = hdl.getHandle();
//         _addQualifierType(ns, qt, &lhdl);
//     }
//     else
//     {
//         // No ensure the node is a qualifier type before we delete it
//         //CIMQualifierType tqt(CIMNULL);
//         //nodeToCIMObject(tqt, node);
//         // If we made it to this point, we know we have a qualifier type
//         // So go ahead and update it.
//         updateCIMObject(qt, node, hdl.getHandle());
//         m_qualCache.removeFromCache(qkey);
//         m_qualCache.addToCache(qt, qkey);
//     }
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMException::ErrNoType
MetaRepository2::getCIMClass(const String& ns, const String& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	CIMClass& cc)
{
//     throwIfNotOpen();
//     String ckey = _makeClassPath(ns, className);
//     cc = m_classCache.getFromCache(ckey);
//     if (!cc)
//     {
//         HDBHandleLock hdl(this, getHandle());
//         HDBNode node = hdl->getNode(ckey);
//         if (node)
//         {
//             // _getClassFromNode throws if unable to get class.
//             cc = _getClassFromNode(node, hdl.getHandle());
//             if (!cc)
//             {
//                 return CIMException::FAILED;
//             }
//             m_classCache.addToCache(cc, ckey);
//         }
//         else
//         {
//             return CIMException::NOT_FOUND;
//         }
//     }
//     // now do some filtering
//     if (propertyList || localOnly == true || includeQualifiers == false || includeClassOrigin == false)
//     { // only clone if we have to
//         StringArray lpropList;
//         bool noProps = false;
//         if (propertyList)
//         {
//             if (propertyList->size() == 0)
//             {
//                 noProps = true;
//             }
//             else
//             {
//                 lpropList = *propertyList;
//             }
//         }
//         cc = cc.clone(localOnly, includeQualifiers, includeClassOrigin,
//             lpropList, noProps);
//         if (!cc) // clone doesn't throw
//         {
//             return CIMException::FAILED;
//         }
//     }
	return CIMException::SUCCESS;
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool
MetaRepository2::deleteClass(const String& ns, const String& className)
{
//     throwIfNotOpen();
//     String ckey = _makeClassPath(ns, className);
//     HDBHandleLock hdl(this, getHandle());
//     HDBNode node = hdl->getNode(ckey);
//     if (!node)
//     {
//         return false;
//     }
//     // Just to be safe, we will attempt to create an CIMClass object
//     // from the node. If the node is not for an CIMClass, an exception
//     // will be thrown.
//     CIMClass theClassToDelete(CIMNULL);
//     nodeToCIMObject(theClassToDelete, node);
//     m_classCache.removeFromCache(ckey);     // Ensure class is not in the cache
//     return hdl->removeNode(node);
	return false;
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::createClass(const String& ns, CIMClass& cimClass)
{
//     throwIfNotOpen();
//     HDBHandleLock hdl(this, getHandle());
//     CIMQualifierArray qra = cimClass.getQualifiers();
//     _resolveQualifiers(ns, qra, hdl.getHandle());
//     cimClass.setQualifiers(qra);
//     // Ensure integrity with any super classes
//     HDBNode pnode = adjustClass(ns, cimClass, hdl.getHandle());
//     // pnode is null if there is no parent class, so get namespace node
//     if (!pnode)
//     {
//         if (!(pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + "/" + ns)))
//         {
//             OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
//                 ns.c_str());
//         }
//     }
//     String ckey = _makeClassPath(ns, cimClass.getName());
//     HDBNode node = hdl->getNode(ckey);
//     if (node)
//     {
//         OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, ckey.c_str());
//     }
//     UInt32 flags = (cimClass.isAssociation()) ? HDBCLSASSOCNODE_FLAG
//         : 0;
//     addCIMObject(cimClass, ckey, pnode, hdl.getHandle(), flags);
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::modifyClass(const String& ns,
	const CIMClass& cimClass_)
{
//     throwIfNotOpen();
//     HDBHandleLock hdl(this, getHandle());
//     CIMClass cimClass(cimClass_);
//     adjustClass(ns, cimClass, hdl.getHandle());
//     String ckey = _makeClassPath(ns, cimClass.getName());
//     HDBNode node = hdl->getNode(ckey);
//     if (!node)
//     {
//         OW_THROWCIMMSG(CIMException::NOT_FOUND, ckey.c_str());
//     }
//     // Create an CIMClass object out of the node we just read to ensure
//     // the data belongs to an CIMClass.
//     CIMClass clsToUpdate(CIMNULL);
//     nodeToCIMObject(clsToUpdate, node);
//     // At this point we know we are updating an CIMClass
//     m_classCache.removeFromCache(ckey);
//     updateCIMObject(cimClass, node, hdl.getHandle());
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::enumClass(const String& ns, const String& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
//     throwIfNotOpen();
//     HDBHandleLock hdl(this, getHandle());
//     HDBNode pnode;
//     if (!className.empty())
//     {
//         String ckey = _makeClassPath(ns, className);
//         pnode = hdl->getNode(ckey);
//         if (!pnode)
//         {
//             pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + "/" + ns);
//             if (!pnode)
//             {
//                 OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
//             }
//             else
//             {
//                 OW_THROWCIMMSG(CIMException::INVALID_CLASS, className.c_str());
//             }
//         }
//     }
//     else
//     {
//         String ns2(ns);
//         while (!ns2.empty() && ns2[0] == '/')
//         {
//             ns2 = ns2.substring(1);
//         }
//         pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + "/" + ns2);
//         if (!pnode)
//         {
//             OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns2.c_str());
//         }
//     }
//     pnode = hdl->getFirstChild(pnode);
//     while (pnode)
//     {
//         if (!pnode.areAllFlagsOn(HDBNSNODE_FLAG))
//         {
//             _getClassNodes(result, pnode, hdl.getHandle(), deep, localOnly,
//                 includeQualifiers, includeClassOrigin);
//         }
//         pnode = hdl->getNextSibling(pnode);
//     }
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::enumClassNames(const String& ns, const String& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep)
{
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::deleteNameSpace(const String& nsName, OperationContext& context)
{
}

::DB_TXN* getTxn(OperationContext& context)
{
	return 0; // TODO: write this
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository2::createNameSpace(String ns, OperationContext& context)
{
	if (ns.empty())
	{
		OW_THROWCIM(CIMException::INVALID_PARAMETER);
	}
	std::vector<unsigned char> tmpData;
	::DB_TXN* txn = getTxn(context);
	if (m_namespaces.get(ns, tmpData, txn))
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS,
			ns.c_str());
	}
	m_namespaces.put(ns, std::vector<unsigned char>(), txn);
	if (m_env->getLogger(COMPONENT_NAME)->getLogLevel() == E_DEBUG_LEVEL)
	{
		OW_LOG_DEBUG(m_env->getLogger(COMPONENT_NAME), Format("CIMRepository created namespace: %1", ns));
	}
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
MetaRepository2::MetaRepository2(const ServiceEnvironmentIFCRef& env)
	: m_env(env)
{
	String maxCacheSizeOpt = env->getConfigItem(ConfigOpts::MAX_CLASS_CACHE_SIZE_opt, OW_DEFAULT_MAX_CLASS_CACHE_SIZE_S);
	try
	{
		m_classCache.setMaxCacheSize(maxCacheSizeOpt.toUInt32());
	}
	catch (const StringConversionException&)
	{
		m_classCache.setMaxCacheSize(OW_DEFAULT_MAX_CLASS_CACHE_SIZE);
	}
}

} // end namespace OW_NAMESPACE

