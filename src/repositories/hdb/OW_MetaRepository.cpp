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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_MetaRepository.hpp"
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

namespace OW_NAMESPACE
{

using namespace WBEMFlags;

namespace
{
const String QUAL_CONTAINER("q");
const String CLASS_CONTAINER("c");
const char NS_SEPARATOR_C(':');

}
//////////////////////////////////////////////////////////////////////////////
MetaRepository::~MetaRepository()
{
}
//////////////////////////////////////////////////////////////////////////////
static void createRootNode(const String& qcontk, HDBHandleLock& hdl)
{
	HDBNode rnode = hdl->getNode(qcontk);
	if (!rnode)
	{
		rnode = HDBNode(qcontk, qcontk.length()+1,
			reinterpret_cast<const unsigned char*>(qcontk.c_str()));
		hdl->turnFlagsOn(rnode, HDBNSNODE_FLAG);
		hdl->addRootNode(rnode);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::open(const String& path)
{
	GenericHDBRepository::open(path);
	OW_LOG_INFO(m_env->getLogger(COMPONENT_NAME), Format("Using MetaRepository: %1", path));

	// Create root qualifier container
	HDBHandleLock hdl(this, getHandle());
	StringBuffer qcontk(QUAL_CONTAINER);
	createRootNode(qcontk.toString(), hdl);
	qcontk += NS_SEPARATOR_C;
	qcontk += String("root");
	createRootNode(qcontk.releaseString(), hdl);

	// Create root class container
	StringBuffer ccontk = CLASS_CONTAINER;
	createRootNode(ccontk.toString(), hdl);
	ccontk += NS_SEPARATOR_C;
	ccontk += String("root");
	createRootNode(ccontk.releaseString(), hdl);
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
MetaRepository::_getQualContainer(HDBHandleLock& hdl, const String& ns)
{
	StringBuffer qcontk(QUAL_CONTAINER);
	if (!ns.empty())
	{
		qcontk += NS_SEPARATOR_C;
		qcontk += ns;
	}
	return getNameSpaceNode(hdl, qcontk.releaseString());
}
//////////////////////////////////////////////////////////////////////////////
String
MetaRepository::_makeQualPath(const String& ns_, const CIMName& qualName)
{
	String ns(ns_);
	StringBuffer qp(QUAL_CONTAINER);
	qp += NS_SEPARATOR_C;
	qp += ns;
	if (qualName != CIMName())
	{
		qp += NS_SEPARATOR_C;
		qp += qualName.toString();
	}
	return qp.releaseString().toLowerCase();
}
//////////////////////////////////////////////////////////////////////////////
String
MetaRepository::_makeClassPath(const String& ns,
	const CIMName& className)
{
	StringBuffer cp(CLASS_CONTAINER);
	cp += NS_SEPARATOR_C;
	cp += ns;
	cp += NS_SEPARATOR_C;
	cp += className.toString();
	return cp.releaseString().toLowerCase();
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierType
MetaRepository::getQualifierType(const String& ns,
	const CIMName& qualName, HDBHandle* phdl)
{
	throwIfNotOpen();
	String qkey = _makeQualPath(ns, qualName);
	CIMQualifierType qualType = m_qualCache.getFromCache(qkey);
	if (qualType)
	{
		return qualType;
	}
	GenericHDBRepository* prep;
	HDBHandle lhdl;
	if (phdl)
	{
		prep = NULL;
		lhdl = *phdl;
	}
	else
	{
		prep = this;
		lhdl = getHandle();
	}
	HDBHandleLock hdl(prep, lhdl);
	getCIMObject(qualType, qkey, hdl.getHandle());
	if (!qualType)
	{
		if (nameSpaceExists(QUAL_CONTAINER + NS_SEPARATOR_C + ns))
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND,
					Format("CIM QualifierType \"%1\" not found in namespace: %2",
						qualName, ns).c_str());
		}
		else
		{
			OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
					ns.c_str());
		}
	}
	m_qualCache.addToCache(qualType, qkey);
	return qualType;
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::enumQualifierTypes(const String& ns,
	CIMQualifierTypeResultHandlerIFC& result)
{
	throwIfNotOpen();
	String nskey = _makeQualPath(ns, String());
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(nskey);
	if (!node)
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	if (!node.areAllFlagsOn(HDBNSNODE_FLAG))
	{
		OW_THROW(HDBException, "Expected namespace node");
	}
	node = hdl->getFirstChild(node);
	while (node)
	{
		// If this is not a namespace node, assume it's a qualifier
		if (!node.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			CIMQualifierType qual(CIMNULL);
			nodeToCIMObject(qual, node);
			result.handle(qual);
		}
		node = hdl->getNextSibling(node);
	}
}
//////////////////////////////////////////////////////////////////////////////
bool
MetaRepository::deleteQualifierType(const String& ns,
	const CIMName& qualName)
{
	throwIfNotOpen();
	String qkey = _makeQualPath(ns, qualName);
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(qkey);
	if (!node)
	{
		// Didn't find a node associated with the key
		return false;
	}
	// To ensure the node is a qualifier type, we create a qualifier
	// type from the node. If the node is not a qualifier type, an
	// exception will be thrown
	CIMQualifierType qt(CIMNULL);
	nodeToCIMObject(qt, node);
	// If we've hit this point, we know this is a qualifier
	hdl->removeNode(node);
	m_qualCache.removeFromCache(qkey);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::_addQualifierType(const String& ns,
	const CIMQualifierType& qt, HDBHandle* phdl)
{
	throwIfNotOpen();
	if (!qt)
	{
		OW_THROWCIM(CIMException::INVALID_PARAMETER);
	}
	GenericHDBRepository* prep;
	HDBHandle lhdl;
	if (phdl)
	{
		prep = 0;
		lhdl = *phdl;
	}
	else
	{
		prep = this;
		lhdl = getHandle();
	}
	HDBHandleLock hdl(prep, lhdl);
	String qkey = _makeQualPath(ns, qt.getName());
	HDBNode node = hdl->getNode(qkey);
	if (node)
	{
		String msg(ns);
		if (!ns.empty())
		{
			msg += "/";
		}
		msg += qt.getName();
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, msg.c_str());
	}
	HDBNode pnode = _getQualContainer(hdl, ns);
	if (!pnode)
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	addCIMObject(qt, qkey, pnode, hdl.getHandle());
	m_qualCache.addToCache(qt, qkey);
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::setQualifierType(const String& ns,
	const CIMQualifierType& qt)
{
	throwIfNotOpen();
	String qkey = _makeQualPath(ns, qt.getName());
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(qkey);
	if (!node)
	{
		HDBHandle lhdl = hdl.getHandle();
		_addQualifierType(ns, qt, &lhdl);
	}
	else
	{
		// No ensure the node is a qualifier type before we delete it
		//CIMQualifierType tqt(CIMNULL);
		//nodeToCIMObject(tqt, node);
		// If we made it to this point, we know we have a qualifier type
		// So go ahead and update it.
		updateCIMObject(qt, node, hdl.getHandle());
		m_qualCache.removeFromCache(qkey);
		m_qualCache.addToCache(qt, qkey);
	}
}
#endif // #ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
CIMException::ErrNoType
MetaRepository::getCIMClass(const String& ns, const CIMName& className,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	CIMClass& cc)
{
	throwIfNotOpen();
	String ckey = _makeClassPath(ns, className);
	cc = m_classCache.getFromCache(ckey);
	if (!cc)
	{
		HDBHandleLock hdl(this, getHandle());
		HDBNode node = hdl->getNode(ckey);
		if (node)
		{
			// _getClassFromNode throws if unable to get class.
			cc = _getClassFromNode(node, hdl.getHandle(), ns);
			if (!cc)
			{
				return CIMException::FAILED;
			}
			m_classCache.addToCache(cc, ckey);
		}
		else
		{
			return CIMException::NOT_FOUND;
		}
	}
	// now do some filtering
	if (propertyList
		|| localOnly == E_LOCAL_ONLY
		|| includeQualifiers == E_EXCLUDE_QUALIFIERS
		|| includeClassOrigin == E_EXCLUDE_CLASS_ORIGIN)
	{ // only clone if we have to
		StringArray lpropList;
		bool noProps = false;
		if (propertyList)
		{
			if (propertyList->size() == 0)
			{
				noProps = true;
			}
			else
			{
				lpropList = *propertyList;
			}
		}
		cc = cc.clone(localOnly, includeQualifiers, includeClassOrigin,
			lpropList, noProps);
		if (!cc) // clone doesn't throw
		{
			return CIMException::FAILED;
		}
	}
	return CIMException::SUCCESS;
}
//////////////////////////////////////////////////////////////////////////////
CIMClass
MetaRepository::_getClassFromNode(HDBNode& node, HDBHandle hdl,
	const String& ns)
{
	CIMClass theClass(CIMNULL);
	nodeToCIMObject(theClass, node);
	if (!theClass)
	{
		return theClass;
	}
	_resolveClass(theClass, node, hdl, ns);
	return theClass;
}
//////////////////////////////////////////////////////////////////////////////
CIMName
MetaRepository::_getClassNameFromNode(HDBNode& node)
{
	CIMName name;
	DataIStream istrm(node.getDataLen(), node.getData());
	// Not going to do this, it's too slow! cimObj.readObject(istrm);
	// This is breaking abstraction, and may bite us later if CIMClass::readObject() ever changes..., but in some cases efficiency wins out.
	CIMBase::readSig( istrm, OW_CIMCLASSSIG, OW_CIMCLASSSIG_V, CIMClass::SERIALIZATION_VERSION );
	name.readObject(istrm);
	return name;
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::_resolveClass(CIMClass& child, HDBNode& node,
	HDBHandle& hdl, const String& ns)
{
	// If this class has an association qualifier, then ensure
	// the association flag is on
//     CIMQualifier childAssocQual = child.getQualifier(CIMQualifier::CIM_QUAL_ASSOCIATION);
//     if (childAssocQual)
//     {
//         if (!childAssocQual.getValue()
//             || childAssocQual.getValue() != CIMValue(false))
//         {
//             child.setIsAssociation(true);
//         }
//     }
	// Determine if any properties are keys
	CIMPropertyArray propArray = child.getAllProperties();
	for (size_t i = 0; i < propArray.size(); i++)
	{
		if (propArray[i].isKey())
		{
			child.setIsKeyed(true);
			break;
		}
	}
	HDBNode pnode;
	CIMClass parentClass(CIMNULL);
	CIMName superID = child.getSuperClass();
	// If class doesn't have a super class - don't propagate anything
	// Should always have a parent because of namespaces
	if (superID == CIMName())
	{
		return;
	}
	String pkey = _makeClassPath(ns, superID);
	parentClass = m_classCache.getFromCache(pkey);
	if (!parentClass)
	{
		// If there is no node or the parent node is a namespace
		// then we have a base class and there is nothing to propagate.
		pnode = hdl.getParent(node);
		if (!pnode || pnode.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			return;
		}
		nodeToCIMObject(parentClass, pnode);
		_resolveClass(parentClass, pnode, hdl, ns);
		m_classCache.addToCache(parentClass, pkey);
	}
	//if (parentClass.isAssociation())
	//{
	//	child.setIsAssociation(true);
	//}
	if (parentClass.isKeyed())
	{
		child.setIsKeyed(true);
	}
	// Propagate appropriate class qualifiers
	CIMQualifierArray qualArray = parentClass.getQualifiers();
	for (size_t i = 0; i < qualArray.size(); i++)
	{
		CIMQualifier qual = qualArray[i];
		if (!qual.hasFlavor(CIMFlavor::RESTRICTED))
		//if (qual.hasFlavor(CIMFlavor::TOSUBCLASS))
		{
			if (!child.hasQualifier(qual))
			{
				qual.setPropagated(true);
				child.addQualifier(qual);
			}
		}
	}
	// Propagate Properties from parent class.
	//
	// TODO: Regardless of whether there is an override
	// this will perform override like behavior - probably
	// need to add validation code...
	//
	propArray = parentClass.getAllProperties();
	for (size_t i = 0; i < propArray.size(); i++)
	{
		CIMProperty parentProp = propArray[i];
		CIMProperty childProp = child.getProperty(parentProp.getName());
		if (!childProp)
		{
			parentProp.setPropagated(true);
			child.addProperty(parentProp);
		}
		else if (!childProp.getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
		{
			//
			// Propagate any qualifiers that have not been
			// re-defined
			//
			qualArray = parentProp.getQualifiers();
			for (size_t qi = 0; qi < qualArray.size(); qi++)
			{
				CIMQualifier parentQual = qualArray[qi];
				if (!childProp.getQualifier(parentQual.getName()))
				{
					//
					// Qualifier not defined on child property
					// so propagate it
					//
					parentQual.setPropagated(true);
					childProp.addQualifier(parentQual);
				}
			}
			child.setProperty(childProp);
		}
	}
	// Propagate methods from parent class
	CIMMethodArray methods = parentClass.getAllMethods();
	for (size_t i = 0; i < methods.size(); i++)
	{
		CIMMethod cm = methods[i];
		CIMMethod childMethod = child.getMethod(methods[i].getName());
		if (!childMethod)
		{
			cm.setPropagated(true);
			child.addMethod(cm);
		}
		else if (!childMethod.getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
		{
			//
			// Propagate any qualifiers that have not been
			// re-defined by the method declaration
			//
			qualArray = cm.getQualifiers();
			for (size_t mi = 0; mi < qualArray.size(); mi++)
			{
				CIMQualifier methQual = qualArray[mi];
				if (!childMethod.getQualifier(methQual.getName()))
				{
					methQual.setPropagated(true);
					childMethod.addQualifier(methQual);
				}
			}
			child.setMethod(childMethod);
		}
	}
}
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
bool
MetaRepository::deleteClass(const String& ns, const CIMName& className)
{
	throwIfNotOpen();
	String ckey = _makeClassPath(ns, className);
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(ckey);
	if (!node)
	{
		return false;
	}
	// Just to be safe, we will attempt to create an CIMClass object
	// from the node. If the node is not for an CIMClass, an exception
	// will be thrown.
	CIMClass theClassToDelete(CIMNULL);
	nodeToCIMObject(theClassToDelete, node);
	m_classCache.removeFromCache(ckey);		// Ensure class is not in the cache
	return hdl->removeNode(node);
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::createClass(const String& ns, CIMClass& cimClass)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	CIMQualifierArray qra = cimClass.getQualifiers();
	_resolveQualifiers(ns, qra, hdl.getHandle());
	cimClass.setQualifiers(qra);
	// Ensure integrity with any super classes
	HDBNode pnode = adjustClass(ns, cimClass, hdl.getHandle());
	// pnode is null if there is no parent class, so get namespace node
	if (!pnode)
	{
		if (!(pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + NS_SEPARATOR_C + ns)))
		{
			OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
				ns.c_str());
		}
	}
	String ckey = _makeClassPath(ns, cimClass.getName());
	HDBNode node = hdl->getNode(ckey);
	if (node)
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, ckey.c_str());
	}
	UInt32 flags = (cimClass.isAssociation()) ? HDBCLSASSOCNODE_FLAG
		: 0;
	addCIMObject(cimClass, ckey, pnode, hdl.getHandle(), flags);
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::modifyClass(const String& ns,
	const CIMClass& cimClass_)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	CIMClass cimClass(cimClass_);
	adjustClass(ns, cimClass, hdl.getHandle());
	String ckey = _makeClassPath(ns, cimClass.getName());
	HDBNode node = hdl->getNode(ckey);
	if (!node)
	{
		OW_THROWCIMMSG(CIMException::NOT_FOUND, ckey.c_str());
	}
	// Create an CIMClass object out of the node we just read to ensure
	// the data belongs to an CIMClass.
	CIMClass clsToUpdate(CIMNULL);
	nodeToCIMObject(clsToUpdate, node);
	// At this point we know we are updating an CIMClass
	m_classCache.removeFromCache(ckey);
	updateCIMObject(cimClass, node, hdl.getHandle());
}
//////////////////////////////////////////////////////////////////////////////
HDBNode
MetaRepository::adjustClass(const String& ns, CIMClass& childClass,
	HDBHandle hdl)
{
	CIMName childName = childClass.getName();
	CIMName parentName = childClass.getSuperClass();
	CIMClass parentClass(CIMNULL);
	HDBNode parentNode;
	if (parentName != CIMName())
	{
		// Get the parent class
		String superID = _makeClassPath(ns, parentName);
		//parentClass = m_classCache.getFromCache(superID);
		if (!parentClass)
		{
			parentNode = hdl.getNode(superID);
			if (!parentNode)
			{
				OW_THROWCIMMSG(CIMException::INVALID_SUPERCLASS,
						parentName.toString().c_str());
			}
			parentClass = _getClassFromNode(parentNode, hdl, ns);
			if (!parentClass)
			{
				OW_THROWCIMMSG(CIMException::INVALID_SUPERCLASS,
						parentName.toString().c_str());
			}
		}
	}
	if (!parentClass)
	{
		// No parent class. Must be a base class
		CIMQualifierArray qualArray = childClass.getQualifiers();
		for (size_t i = 0; i < qualArray.size(); i++)
		{
			qualArray[i].setPropagated(false);
		}
		CIMPropertyArray propArray = childClass.getAllProperties();
		for (size_t i = 0; i < propArray.size(); i++)
		{
			propArray[i].setPropagated(false);
			propArray[i].setOriginClass(childName);
		}
		childClass.setProperties(propArray);
		CIMMethodArray methArray = childClass.getAllMethods();
		for (size_t i = 0; i < methArray.size(); i++)
		{
			methArray[i].setPropagated(false);
			methArray[i].setOriginClass(childName);
		}
		childClass.setMethods(methArray);
		//_throwIfBadClass(childClass, parentClass);
		return parentNode;
	}
	//////////
	// At this point we know we have a parent class
	CIMQualifierArray newQuals;
	CIMQualifierArray qualArray = childClass.getQualifiers();
	for (size_t i = 0; i < qualArray.size(); i++)
	{
		CIMQualifier qual = qualArray[i];
		CIMQualifier pqual = parentClass.getQualifier(qual.getName());
		if (pqual)
		{
			if (pqual.getValue().equal(qual.getValue()))
			{
				if (pqual.hasFlavor(CIMFlavor::RESTRICTED))
				{
					// NOT PROPAGATED.  qual.setPropagated(true);
					newQuals.append(qual);
				}
			}
			else
			{
				if (pqual.hasFlavor(CIMFlavor::DISABLEOVERRIDE))
				{
					OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							Format("Parent class qualifier %1 has DISABLEOVERRIDE flavor. "
								"Child cannot override it.", pqual.getName()).c_str());
				}
				newQuals.append(qual);
			}
		}
		else
		{
			newQuals.push_back(qual);
		}
	}
	childClass.setQualifiers(newQuals);
	CIMPropertyArray propArray = childClass.getAllProperties();
	for (size_t i = 0; i < propArray.size(); i++)
	{
		CIMProperty parentProp = parentClass.getProperty(propArray[i].getName());
		if (parentProp)
		{
			if (propArray[i].getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
			{
				if (propArray[i].getOriginClass().empty())
				{
					propArray[i].setOriginClass(childName);
					propArray[i].setPropagated(false);
				}
				else
				{
					propArray[i].setPropagated(true);
				}
				// now make sure any qualifiers are properly set
				CIMQualifierArray parentQuals = parentProp.getQualifiers();
				for (size_t j = 0; j < parentQuals.size(); ++j)
				{
					CIMQualifier& qual = parentQuals[j];
					// If the qualifier has DisableOverride flavor, the
					// subclass can't change it.  (e.g. Key). It gets the
					// parent qualifier.
					if (qual.hasFlavor(CIMFlavor::DISABLEOVERRIDE))
					{
						if (!propArray[i].getQualifier(qual.getName()))
						{
							propArray[i].addQualifier(qual);
						}
						else
						{
							// TODO: look at this message, it seems the dmtf cim schema causes it quite often.
							// maybe we should only output it if the value is different?
							OW_LOG_INFO(m_env->getLogger(COMPONENT_NAME), Format("Warning: %1.%2: qualifier %3 was "
										"overridden, but the qualifier can't be "
										"overridden because it has DisableOverride flavor",
										childClass.getName(), propArray[i].getName(),
										qual.getName()));
							propArray[i].setQualifier(qual);
						}
					}
					// If the qualifier has ToSubclass (not Restricted), then
					// only propagate it down if it's not overridden in the
					// subclass.
					else if (!qual.hasFlavor(CIMFlavor::RESTRICTED))
					{
						if (!propArray[i].getQualifier(qual.getName()))
						{
							propArray[i].addQualifier(qual);
						}
					}
				}
			}
			else
			{
				propArray[i].setOriginClass(parentProp.getOriginClass());
				propArray[i].setPropagated(true);
			}
		}
		else
		{
			// According to the 2.2 spec. If the parent class has key properties,
			// the child class cannot declare additional key properties.
			if (propArray[i].isKey())
			{
				// This is a key property, so the parent class better not be a
				// keyed class.
				if (parentClass.isKeyed())
				{
					OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
							Format("Parent class has keys. Child cannot have additional"
								" key properties: %1", childClass.getName()).c_str());
				}
			}
			propArray[i].setOriginClass(childName);
			propArray[i].setPropagated(false);
		}
	}
	childClass.setProperties(propArray);
	CIMMethodArray methArray = childClass.getAllMethods();
	for (size_t i = 0; i < methArray.size(); i++)
	{
		if (parentClass.getMethod(methArray[i].getName()) &&
				!methArray[i].getQualifier(CIMQualifier::CIM_QUAL_OVERRIDE))
		{
			methArray[i].setOriginClass(parentName);
			methArray[i].setPropagated(true);
		}
		else
		{
			methArray[i].setOriginClass(childName);
			methArray[i].setPropagated(false);
		}
	}
	childClass.setMethods(methArray);
	if (parentClass.isKeyed())
	{
		childClass.setIsKeyed();
	}
	// Don't allow the child class to be an association if the parent class isn't.
	// This shouldn't normally happen, because the association qualifier has
	// a DISABLEOVERRIDE flavor, so it will be caught in an earlier test.
	if (childClass.isAssociation() && !parentClass.isAssociation())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Association class is derived from non-association class: %1",
					childClass.getName()).c_str());
	}
	//_throwIfBadClass(childClass, parentClass);
	return parentNode;
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::_resolveQualifiers(const String& ns,
	CIMQualifierArray& quals, HDBHandle hdl)
{
	for (size_t i = 0; i < quals.size(); i++)
	{
		CIMQualifierType qt = getQualifierType(ns, quals[i].getName(), &hdl);
		if (qt)
		{
			CIMFlavorArray fra = qt.getFlavors();
			for (size_t j = 0; j < fra.size(); j++)
			{
				quals[i].addFlavor(fra[j]);
			}
		}
		else
		{
			OW_LOG_ERROR(m_env->getLogger(COMPONENT_NAME), Format("Unable to find qualifier: %1",
				quals[i].getName()));
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Unable to find qualifier: %1",
				quals[i].getName()).c_str());
		}
	}
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::getTopLevelAssociations(const String& ns,
	CIMClassResultHandlerIFC& result)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = getNameSpaceNode(hdl, CLASS_CONTAINER + NS_SEPARATOR_C + ns);
	if (!node)
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	node = hdl->getFirstChild(node);
	while (node)
	{
		if (!node.areAllFlagsOn(HDBNSNODE_FLAG)
			&& node.areAllFlagsOn(HDBCLSASSOCNODE_FLAG))
		{
			CIMClass cc(CIMNULL);
			nodeToCIMObject(cc, node);
			OW_ASSERT(cc.isAssociation());
			result.handle(cc);
		}
		node = hdl->getNextSibling(node);
	}
}
#endif
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::enumClass(const String& ns, const CIMName& className,
	CIMClassResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	HDBNode pnode;
	if (className != CIMName())
	{
		String ckey = _makeClassPath(ns, className);
		pnode = hdl->getNode(ckey);
		if (!pnode)
		{
			pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + NS_SEPARATOR_C + ns);
			if (!pnode)
			{
				OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
			}
			else
			{
				OW_THROWCIMMSG(CIMException::INVALID_CLASS, className.toString().c_str());
			}
		}
	}
	else
	{
		pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + NS_SEPARATOR_C + ns);
		if (!pnode)
		{
			OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
		}
	}
	pnode = hdl->getFirstChild(pnode);
	while (pnode)
	{
		if (!pnode.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			_getClassNodes(ns, result, pnode, hdl.getHandle(), deep, localOnly,
				includeQualifiers, includeClassOrigin);
		}
		pnode = hdl->getNextSibling(pnode);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::_getClassNodes(const String& ns, CIMClassResultHandlerIFC& result, HDBNode node,
	HDBHandle hdl, EDeepFlag deep, ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin)
{
	CIMClass cimCls = _getClassFromNode(node, hdl, ns);
	// TODO: Check cimCls for NULL?
	result.handle(cimCls.clone(localOnly, includeQualifiers,
		includeClassOrigin));
	if (deep)
	{
		node = hdl.getFirstChild(node);
		while (node)
		{
			_getClassNodes(ns, result, node, hdl, deep, localOnly, includeQualifiers,
				includeClassOrigin);
			node = hdl.getNextSibling(node);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::enumClassNames(const String& ns, const CIMName& className,
	StringResultHandlerIFC& result,
	EDeepFlag deep)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	HDBNode pnode;
	if (className != CIMName())
	{
		String ckey = _makeClassPath(ns, className);
		pnode = hdl->getNode(ckey);
		if (!pnode)
		{
			pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + NS_SEPARATOR_C + ns);
			if (!pnode)
			{
				OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
			}
			else
			{
				OW_THROWCIMMSG(CIMException::INVALID_CLASS, className.toString().c_str());
			}
		}
	}
	else
	{
		pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + NS_SEPARATOR_C + ns);
		if (!pnode)
		{
			OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE, ns.c_str());
		}
	}
	pnode = hdl->getFirstChild(pnode);
	while (pnode)
	{
		if (!pnode.areAllFlagsOn(HDBNSNODE_FLAG))
		{
			_getClassNameNodes(result, pnode, hdl.getHandle(), deep);
		}
		pnode = hdl->getNextSibling(pnode);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::_getClassNameNodes(StringResultHandlerIFC& result, HDBNode node,
	HDBHandle hdl, EDeepFlag deep)
{
	CIMName cimClsName = _getClassNameFromNode(node);
	result.handle(cimClsName.toString());
	if (deep)
	{
		node = hdl.getFirstChild(node);
		while (node)
		{
			_getClassNameNodes(result, node, hdl, deep);
			node = hdl.getNextSibling(node);
		}
	}
}
#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::deleteNameSpace(const String& nsName)
{
	throwIfNotOpen();
	// ATTN: Do we need to do more later? Associations?
	GenericHDBRepository::deleteNameSpace(QUAL_CONTAINER + NS_SEPARATOR_C + nsName);
	GenericHDBRepository::deleteNameSpace(CLASS_CONTAINER + NS_SEPARATOR_C + nsName);
	/*
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = _getQualContainer(hdl, nsName);
	if (node)
	{
		hdl->removeNode(node);
	}
	 */
	m_classCache.clearCache();
}
//////////////////////////////////////////////////////////////////////////////
int
MetaRepository::createNameSpace(const String& ns)
{
	// First create the name space in the class container.
	if (GenericHDBRepository::createNameSpace(CLASS_CONTAINER + NS_SEPARATOR_C + ns) == -1)
	{
		return -1;
	}
	// Now create the same name space in the qualifier container.
	// TODO: If the second create fails, we need to undo the first one.
	return GenericHDBRepository::createNameSpace(QUAL_CONTAINER + NS_SEPARATOR_C + ns);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
MetaRepository::MetaRepository()
{
}

//////////////////////////////////////////////////////////////////////////////
void
MetaRepository::init(const ServiceEnvironmentIFCRef& env)
{
	this->GenericHDBRepository::init(env);
	String maxCacheSizeOpt = env->getConfigItem(ConfigOpts::MAX_CLASS_CACHE_SIZE_opt, OW_DEFAULT_MAX_CLASS_CACHE_SIZE);
	try
	{
		m_classCache.setMaxCacheSize(maxCacheSizeOpt.toUInt32());
	}
	catch (const StringConversionException&)
	{
		m_classCache.setMaxCacheSize(OW_DEFAULT_MAX_CLASS_CACHE_SIZE_I);
	}
}

} // end namespace OW_NAMESPACE

