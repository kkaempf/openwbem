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
#include "OW_MetaRepository.hpp"
#include "OW_RepositoryStreams.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMMethod.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_Array.hpp"

#include <iostream>
#if defined(OW_HAVE_STREAMBUF)
#include <streambuf>
#elif defined(OW_HAVE_STREAMBUF_H)
#include <streambuf.h>
#endif
#include <ctime>
#include <climits>

#define QUAL_CONTAINER "openwbemqualifiers"

struct ClassCacheRec
{
	ClassCacheRec() :
		key(), cc(), lastAccess(0) {}

	ClassCacheRec(const ClassCacheRec& arg) :
		key(arg.key), cc(arg.cc), lastAccess(arg.lastAccess) {}

	ClassCacheRec& operator= (const ClassCacheRec& arg)
	{
		key = arg.key;
		cc = arg.cc;
		lastAccess = arg.lastAccess;
		return *this;
	}

	OW_String key;
	OW_CIMClass cc;
	time_t lastAccess;
};

#define CACHE_SIZE 20

static OW_Array<ClassCacheRec> theCache;
static OW_Mutex cacheGuard;
static bool cacheInitialized = false;

//////////////////////////////////////////////////////////////////////////////
static void
addClassToCache(const OW_CIMClass& cc, const OW_String& key)
{
	OW_MutexLock l(cacheGuard);

	if(!cacheInitialized)
	{
		theCache = OW_Array<ClassCacheRec>();
		cacheInitialized = true;
	}

	ClassCacheRec cr;
	cr.key = key;
	cr.cc = cc;
	cr.lastAccess = time(NULL);
	if(theCache.size() > CACHE_SIZE)
	{
		size_t toRem = 0;
		time_t lru = LONG_MAX;
		for(size_t i = 0; i < theCache.size(); i++)
		{
			if(theCache[i].lastAccess < lru)
			{
				lru = theCache[i].lastAccess;
				toRem = i;
			}
		}

		theCache.remove(toRem);
	}

	theCache.append(cr);
}

//////////////////////////////////////////////////////////////////////////////
static OW_CIMClass
getClassFromCache(const OW_String& key)
{
	OW_MutexLock l(cacheGuard);
	if(!cacheInitialized)
	{
		theCache = OW_Array<ClassCacheRec>();
		cacheInitialized = true;
	}

	OW_CIMClass cc;
	for(size_t i = 0; i < theCache.size(); i++)
	{
		if(theCache[i].key.equals(key))
		{
			theCache[i].lastAccess = time(NULL);
			cc = theCache[i].cc;
			break;
		}
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
static void
removeClassFromCache(const OW_String& key)
{
	OW_MutexLock l(cacheGuard);
	if(!cacheInitialized)
	{
		theCache = OW_Array<ClassCacheRec>();
		cacheInitialized = true;
	}

	for(size_t i = 0; i < theCache.size(); i++)
	{
		if(theCache[i].key.equals(key))
		{
			theCache.remove(i);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void
clearClassCache()
{
	OW_MutexLock l(cacheGuard);
	theCache.clear();
}

//////////////////////////////////////////////////////////////////////////////
OW_MetaRepository::~OW_MetaRepository()
{
	clearClassCache();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::open(const OW_String& path)
{
	OW_GenericHDBRepository::open(path);
	logCustInfo(format("Using MetaRepository: %1", path));

	// Create root qualifier container
	OW_HDBHandleLock hdl(this, getHandle());
	OW_String qcontk(QUAL_CONTAINER);
	qcontk.toLowerCase();
	OW_HDBNode rnode = hdl->getNode(qcontk);
	if(!rnode)
	{
		rnode = OW_HDBNode(qcontk, qcontk.length()+1,
			(const unsigned char*) qcontk.c_str());

		hdl->turnFlagsOn(rnode, OW_HDBNSNODE_FLAG);
		hdl->addRootNode(rnode);
	}

	qcontk = QUAL_CONTAINER "/" OW_ROOT_CONTAINER;
	qcontk.toLowerCase();
	OW_HDBNode node = hdl->getNode(qcontk);
	if(!node)
	{
		node = OW_HDBNode(qcontk, qcontk.length()+1,
			(const unsigned char*) qcontk.c_str());
		hdl->turnFlagsOn(node, OW_HDBNSNODE_FLAG);
		hdl->addChild(rnode, node);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_MetaRepository::_getQualContainer(OW_HDBHandleLock& hdl, const OW_String& ns)
{
	OW_String qcontk(QUAL_CONTAINER);
	if(ns.length() > 0)
	{
		qcontk += "/";
		qcontk += ns;
	}

	return getNameSpaceNode(hdl, qcontk);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_MetaRepository::_makeQualPath(const OW_String& ns, const OW_String& qualName)
{
	OW_String qp(QUAL_CONTAINER "/");
	if(ns.length() > 0)
	{
		qp += ns;
	}

	if(qualName.length() > 0)
	{
		qp += "/";
		qp += qualName;
	}

	return qp.toLowerCase();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_MetaRepository::_makeClassPath(const OW_String& ns,
	const OW_String& className)
{
	OW_String cp(ns);
	if(cp.length() > 0)
	{
		cp += "/";
	}
	cp += className;
	return cp.toLowerCase();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_MetaRepository::enumQualifierTypes(const OW_String& ns)
{
	throwIfNotOpen();

	OW_CIMQualifierTypeEnumeration qen;
	OW_String nskey = _makeQualPath(ns, OW_String());
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(nskey);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}

	if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
	{
		OW_THROW(OW_IOException, "Expected namespace node");
	}

	node = hdl->getFirstChild(node);
	while(node)
	{
		// If this is not a namespace node, assume it's a qualifier
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			OW_CIMQualifierType qual;
			nodeToCIMObject(qual, node);
			qen.addElement(qual);
		}
		node = hdl->getNextSibling(node);
	}
	return qen;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_MetaRepository::getQualifierType(const OW_String& ns,
	const OW_String& qualName, OW_HDBHandle* phdl)
{
	throwIfNotOpen();
	OW_CIMQualifierType qualType;
	OW_String qkey = _makeQualPath(ns, qualName);

	OW_GenericHDBRepository* prep;
	OW_HDBHandle lhdl;
	if(phdl)
	{
		prep = NULL;
		lhdl = *phdl;

	}
	else
	{
		prep = this;
		lhdl = getHandle();
	}

	OW_HDBHandleLock hdl(prep, lhdl);
	getCIMObject(qualType, qkey, hdl.getHandle());

	if (!qualType)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
			format("CIM QualifierType \"%1\" not found in namespace: %2",
				qualName, ns).c_str());
	}
	return qualType;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_MetaRepository::deleteQualifierType(const OW_String& ns,
	const OW_String& qualName)
{
	throwIfNotOpen();
	OW_String qkey = _makeQualPath(ns, qualName);
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(qkey);
	if(!node)
	{
		// Didn't find a node associated with the key
		return false;
	}

	// To ensure the node is a qualifier type, we create a qualifier
	// type from the node. If the node is not a qualifier type, an
	// exception will be thrown
	OW_CIMQualifierType qt;
	nodeToCIMObject(qt, node);

	// If we've hit this point, we know this is a qualifier

	hdl->removeNode(node);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::_addQualifierType(const OW_String& ns,
	const OW_CIMQualifierType& qt, OW_HDBHandle* phdl)
{
	throwIfNotOpen();
	if(!qt)
	{
		OW_THROWCIM(OW_CIMException::INVALID_PARAMETER);
	}

	OW_GenericHDBRepository* prep;
	OW_HDBHandle lhdl;
	if(phdl)
	{
		prep = 0;
		lhdl = *phdl;
	}
	else
	{
		prep = this;
		lhdl = getHandle();
	}


	OW_HDBHandleLock hdl(prep, lhdl);
	OW_String qkey = _makeQualPath(ns, qt.getName());
	OW_HDBNode node = hdl->getNode(qkey);

	if(node)
	{
		OW_String msg(ns);
		if(ns.length() > 0)
		{
			msg += "/";
		}
		msg += qt.getName();
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS, msg.c_str());
	}

	OW_HDBNode pnode = _getQualContainer(hdl, ns);
	if(!pnode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}

	addCIMObject(qt, qkey, pnode, hdl.getHandle());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::setQualiferType(const OW_String& ns,
	const OW_CIMQualifierType& qt)
{
	throwIfNotOpen();

	OW_String qkey = _makeQualPath(ns, qt.getName());
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(qkey);
	if(!node)
	{
		OW_HDBHandle lhdl = hdl.getHandle();
		_addQualifierType(ns, qt, &lhdl);
	}
	else
	{
		// No ensure the node is a qualifier type before we delete it
		OW_CIMQualifierType tqt;
		nodeToCIMObject(tqt, node);

		// If we made it to this point, we know we have a qualifier type
		// So go ahead and update it.
		updateCIMObject(qt, node, hdl.getHandle());
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_MetaRepository::getCIMClass(const OW_CIMObjectPath& op, OW_CIMClass& cc)
{
	return getCIMClass(op.getNameSpace(), op.getObjectName(), cc);
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_MetaRepository::getCIMClass(const OW_String& ns, const OW_String& className, 
	OW_CIMClass& cc)
{
	throwIfNotOpen();
	OW_String ckey = _makeClassPath(ns, className);
	cc = getClassFromCache(ckey);
	if(!cc)
	{
		OW_HDBHandleLock hdl(this, getHandle());
		OW_HDBNode node = hdl->getNode(ckey);
		if(node)
		{
			// _getClassFromNode throws if unable to get class. 
			cc = _getClassFromNode(node, hdl.getHandle());
			if (!cc)
			{
				return OW_CIMException::FAILED;
			}
			addClassToCache(cc, ckey);
		}
		else
		{
			return OW_CIMException::NOT_FOUND;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_MetaRepository::_getClassFromNode(OW_HDBNode& node, OW_HDBHandle hdl,
	const OW_String& ns)
{
	OW_CIMClass theClass;
	nodeToCIMObject(theClass, node);
	if (!theClass)
	{
		return theClass;
	}
	_resolveClass(theClass, node, hdl, ns);
	return theClass;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::_resolveClass(OW_CIMClass& child, OW_HDBNode& node,
	OW_HDBHandle& hdl, const OW_String& ns)
{
	// If this class has an association qualifier, then ensure
	// the association flag is on
	if(child.getQualifier(OW_CIMQualifier::CIM_QUAL_ASSOCIATION))
	{
		child.setIsAssociation(true);
	}

	// Determine if any properties are keys
	OW_CIMPropertyArray propArray = child.getAllProperties();
	for(size_t i = 0; i < propArray.size(); i++)
	{
		if(propArray[i].isKey())
		{
			child.setIsKeyed(true);
			break;
		}
	}

	OW_HDBNode pnode;
	OW_CIMClass parentClass;
	OW_String superID = child.getSuperClass();

	// If class doesn't have a super class - don't propagate anything
	// Should always have a parent because of namespaces
	if(superID.length() == 0)
	{
		return;
	}

	OW_String pkey = _makeClassPath(ns, superID);
	parentClass = getClassFromCache(pkey);
	if(!parentClass)
	{
		// If there is no node or the parent node is a namespace
		// then we have a base class and there is nothing to propagate.
		pnode = hdl.getParent(node);
		if(!pnode || pnode.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			return;
		}

		nodeToCIMObject(parentClass, pnode);
		_resolveClass(parentClass, pnode, hdl, ns);
		addClassToCache(parentClass, pkey);
	}

	if(parentClass.isAssociation())
	{
		child.setIsAssociation(true);
	}
	if(parentClass.isKeyed())
	{
		child.setIsKeyed(true);
	}

	// Propagate appropriate class qualifiers
	OW_CIMQualifierArray qualArray = parentClass.getQualifiers();
	for(size_t i = 0; i < qualArray.size(); i++)
	{
		OW_CIMQualifier qual = qualArray[i];
		if(!qual.hasFlavor(OW_CIMFlavor::RESTRICTED))
		//if(qual.hasFlavor(OW_CIMFlavor::TOSUBCLASS))
		{
			if(!child.hasQualifier(qual))
			{
				qual.setPropagated(true);
				child.addQualifier(qual);
			}
		}
	}

	// Propagate Properties from parent class.
	//
	// ATTN: Regardless of whether there is an override
	// this will perform override like behavior - probably
	// need to add validation code...
	//
	propArray = parentClass.getAllProperties();
	for(size_t i = 0; i < propArray.size(); i++)
	{
		OW_CIMProperty parentProp = propArray[i];
		OW_CIMProperty childProp = child.getProperty(parentProp.getName());

		if(!childProp)
		{
			parentProp.setPropagated(true);
			child.addProperty(parentProp);
		}
		else
		{
			//
			// Propagate any qualifiers that have not been
			// re-defined
			//
			qualArray = parentProp.getQualifiers();
			for(size_t qi = 0; qi < qualArray.size(); qi++)
			{
				OW_CIMQualifier parentQual = qualArray[qi];
				if(!childProp.getQualifier(parentQual.getName()))
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
	OW_CIMMethodArray methods = parentClass.getAllMethods();
	for(size_t i = 0; i < methods.size(); i++)
	{
		OW_CIMMethod cm = methods[i];
		OW_CIMMethod childMethod = child.getMethod(methods[i].getName());
		if(!childMethod)
		{
			cm.setPropagated(true);
			child.addMethod(cm);
		}
		else
		{
			//
			// Propagate any qualifiers that have not been
			// re-defined by the method declaration
			//
			qualArray = cm.getQualifiers();
			for(size_t mi = 0; mi < qualArray.size(); mi++)
			{
				OW_CIMQualifier methQual = qualArray[mi];
				if(!childMethod.getQualifier(methQual.getName()))
				{
					methQual.setPropagated(true);
					childMethod.addQualifier(methQual);
				}
			}

			child.setMethod(childMethod);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_MetaRepository::deleteClass(const OW_String& ns, const OW_String& className)
{
	throwIfNotOpen();
	OW_String ckey = _makeClassPath(ns, className);
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(ckey);
	if(!node)
	{
		return false;
	}

	// Just to be safe, we will attempt to create an OW_CIMClass object
	// from the node. If the node is not for an OW_CIMClass, an exception
	// will be thrown.
	OW_CIMClass theClassToDelete;
	nodeToCIMObject(theClassToDelete, node);

	removeClassFromCache(ckey);		// Ensure class is not in the cache
	return hdl->removeNode(node);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::createClass(const OW_String& ns, OW_CIMClass& cimClass)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());

	OW_CIMQualifierArray qra = cimClass.getQualifiers();
	_resolveQualifiers(ns, qra, hdl.getHandle());
	cimClass.setQualifiers(qra);

	// Ensure integrity with any super classes
	OW_HDBNode pnode = adjustClass(ns, cimClass, hdl.getHandle());

	// pnode is null if there is no parent class, so get namespace node
	if(!pnode)
	{
		if(!(pnode = getNameSpaceNode(hdl, ns)))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
				ns.c_str());
		}
	}

	OW_String ckey = _makeClassPath(ns, cimClass.getName());
	OW_HDBNode node = hdl->getNode(ckey);
	if(node)
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS, ckey.c_str());
	}


	unsigned int flags = (cimClass.isAssociation()) ? OW_HDBCLSASSOCNODE_FLAG
		: 0;

	addCIMObject(cimClass, ckey, pnode, hdl.getHandle(), flags);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::_resolveQualifiers(const OW_String& ns,
	OW_CIMQualifierArray& quals, OW_HDBHandle hdl)

{
	for(size_t i = 0; i < quals.size(); i++)
	{
		OW_CIMQualifierType qt = getQualifierType(ns, quals[i].getName(), &hdl);
		if(qt)
		{
			OW_CIMFlavorArray fra = qt.getFlavors();
			for(size_t j = 0; j < fra.size(); j++)
			{
				quals[i].addFlavor(fra[j]);
			}
		}
		else
		{
			logError(format("Unable to find qualifier: %1",
				quals[i].getName()));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_MetaRepository::adjustClass(const OW_String& ns, OW_CIMClass& childClass,
	OW_HDBHandle hdl)
{
	OW_String childName = childClass.getName();
	OW_String parentName = childClass.getSuperClass();
	OW_CIMClass parentClass;
	OW_HDBNode parentNode;

	if(parentName.length() > 0)
	{
		// Get the parent class
		OW_String superID = _makeClassPath(ns, parentName);
		parentNode = hdl.getNode(superID);
		if(!parentNode)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_SUPERCLASS,
				superID.c_str());
		}

		parentClass = _getClassFromNode(parentNode, hdl, ns);
		if(!parentClass)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_SUPERCLASS,
				superID.c_str());
		}
	}

	if(!parentClass)
	{
		// No parent class. Must be a base class

		OW_CIMQualifierArray qualArray = childClass.getQualifiers();
		for(size_t i = 0; i < qualArray.size(); i++)
		{
			qualArray[i].setPropagated(false);
		}

		OW_CIMPropertyArray propArray = childClass.getAllProperties();
		for(size_t i = 0; i < propArray.size(); i++)
		{
			propArray[i].setPropagated(false);
			propArray[i].setOriginClass(childName);
		}
		childClass.setProperties(propArray);

		OW_CIMMethodArray methArray = childClass.getAllMethods();
		for(size_t i = 0; i < methArray.size(); i++)
		{
			methArray[i].setPropagated(false);
			methArray[i].setOriginClass(childName);
		}
		childClass.setMethods(methArray);
		_throwIfBadClass(childClass, parentClass);
		return parentNode;
	}

	//////////
	// At this point we know we have a parent class

	OW_CIMQualifierArray newQuals;
	OW_CIMQualifierArray qualArray = childClass.getQualifiers();
	for(size_t i = 0; i < qualArray.size(); i++)
	{
		OW_CIMQualifier qual = qualArray[i];
		OW_CIMQualifier pqual = parentClass.getQualifier(qual.getName());
		if (pqual)
		{
			if (pqual.getValue().equal(qual.getValue()))
			{
				if (pqual.hasFlavor(OW_CIMFlavor::RESTRICTED))
				{
					// NOT PROPAGATED.  qual.setPropagated(true);
					newQuals.append(qual);
				}
			}
			else
			{
				if (pqual.hasFlavor(OW_CIMFlavor::DISABLEOVERRIDE))
				{
					OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
						format("Parent class qualifier %1 has DISABLEOVERRIDE flavor. "
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

	OW_CIMPropertyArray propArray = childClass.getAllProperties();
	for(size_t i = 0; i < propArray.size(); i++)
	{
		OW_CIMProperty parentProp = parentClass.getProperty(propArray[i].getName());
		if(parentProp)
		{
			propArray[i].setOriginClass(parentProp.getOriginClass());
			propArray[i].setPropagated(true);
		}
		else
		{
			// According to the 2.2 spec. If the parent class has key properties,
			// the child class cannot declare additional key properties.
			if(propArray[i].isKey())
			{
				// This is a key property, so the parent class better not be a
				// keyed class.
				if(parentClass.isKeyed())
				{
					OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
						format("Parent class has keys. Child cannot have additional"
							" key properties: %1", childClass.getName()).c_str());
				}
			}

			propArray[i].setOriginClass(childName);
			propArray[i].setPropagated(false);
		}
	}
	childClass.setProperties(propArray);

	OW_CIMMethodArray methArray = childClass.getAllMethods();
	for(size_t i = 0; i < methArray.size(); i++)
	{
		if(parentClass.getMethod(methArray[i].getName()))
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

  if(parentClass.isKeyed())
  {
	  childClass.setIsKeyed();
  }

  // Don't allow the child class to be an association if the parent class isn't.
  if(childClass.isAssociation() && !parentClass.isAssociation())
  {
	  OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
		  format("Association class is derived from non-association class: %1",
			  childClass.getName()).c_str());
  }

  _throwIfBadClass(childClass, parentClass);
  return parentNode;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::modifyClass(const OW_String& ns,
	OW_CIMClass& cimClass)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	adjustClass(ns, cimClass, hdl.getHandle());

	OW_String ckey = _makeClassPath(ns, cimClass.getName());
	OW_HDBNode node = hdl->getNode(ckey);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, ckey.c_str());
	}

	// Create an OW_CIMClass object out of the node we just read to ensure
	// the data belongs to an OW_CIMClass.
	OW_CIMClass clsToUpdate;
	nodeToCIMObject(clsToUpdate, node);

	// At this point we know we are updating an OW_CIMClass
	removeClassFromCache(ckey);
	updateCIMObject(cimClass, node, hdl.getHandle());
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_MetaRepository::getTopLevelAssociations(const OW_String& ns)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = getNameSpaceNode(hdl, ns);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	OW_CIMClassEnumeration cenum;
	node = hdl->getFirstChild(node);
	while(node)
	{
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG)
			&& node.areAllFlagsOn(OW_HDBCLSASSOCNODE_FLAG))
		{
			OW_CIMClass cc;
			nodeToCIMObject(cc, node);

			OW_ASSERT(cc.isAssociation());

			cenum.addElement(cc);
		}
		node = hdl->getNextSibling(node);
	}

	return cenum;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_MetaRepository::enumClass(const OW_String& ns, const OW_String& className,
	OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, OW_CIMClassEnumeration* penum)
{
	throwIfNotOpen();
	OW_CIMClassEnumeration en;
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode pnode;

	if(!penum)
	{
		penum = &en;
	}

	if(className.length() > 0)
	{
		OW_String ckey = _makeClassPath(ns, className);
		pnode = hdl->getNode(ckey);
		if(!pnode)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, className.c_str());
		}
	}
	else
	{
		pnode = getNameSpaceNode(hdl, ns);
		if(!pnode)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
		}
	}

	/*
	// The following code is really a paranoid check. It is meant to ensure
	// the node we have is read a cim class node.
	//
	if(className.length() > 0)
	{
		// Create a parent class from the node just to ensure we have the node
		// to an OW_CIMClass. If we don't, an exception will be thrown.
		OW_CIMClass parentClass;
		nodeToCIMObject(parentClass, pnode);
	}
	// */

	pnode = hdl->getFirstChild(pnode);
	while(pnode)
	{
		if(!pnode.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			_getClassNodes(*penum, pnode, hdl.getHandle(), deep, localOnly,
				includeQualifiers, includeClassOrigin);
		}

		pnode = hdl->getNextSibling(pnode);
	}

	return *penum;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::_getClassNodes(OW_CIMClassEnumeration& en, OW_HDBNode node,
	OW_HDBHandle hdl, OW_Bool deep, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_CIMClass cimCls = _getClassFromNode(node, hdl);
	en.addElement(cimCls.clone(localOnly, includeQualifiers,
		includeClassOrigin));

	if(deep)
	{
		node = hdl.getFirstChild(node);
		while(node)
		{
			_getClassNodes(en, node, hdl, deep, localOnly, includeQualifiers,
				includeClassOrigin);
			node = hdl.getNextSibling(node);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_MetaRepository::getClassChildren(const OW_String& ns,
	const OW_String& className)
{
	throwIfNotOpen();
	OW_StringArray ra;
	OW_String classID = _makeClassPath(ns, className);
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(classID);
	if(node)
	{
		// Ensure node belongs to an OW_CIMClass
		OW_CIMClass verifyClass;
		nodeToCIMObject(verifyClass, node);

		node = hdl->getFirstChild(node);
		while(node)
		{
			_getClassChildNames(ra, node, hdl.getHandle());
			node = hdl->getNextSibling(node);
		}
	}

	return ra;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::_getClassChildNames(OW_StringArray& ra, OW_HDBNode node,
	OW_HDBHandle hdl)
{
	OW_CIMClass cimCls;
	nodeToCIMObject(cimCls, node);
	ra.append(cimCls.getName());

	node = hdl.getFirstChild(node);
	while(node)
	{
		_getClassChildNames(ra, node, hdl);
		node = hdl.getNextSibling(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::deleteNameSpace(const OW_String& nsName)
{
	throwIfNotOpen();

	// ATTN: Do we need to do more later? Associations?

	OW_GenericHDBRepository::deleteNameSpace(nsName);
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = _getQualContainer(hdl, nsName);
	if(node)
	{
		hdl->removeNode(node);
	}

	clearClassCache();
}

//////////////////////////////////////////////////////////////////////////////
int
OW_MetaRepository::createNameSpace(const OW_StringArray& nameComps,
	OW_Bool /*rootCheck*/)
{
	if(OW_GenericHDBRepository::createNameSpace(nameComps) == -1)
	{
		return -1;
	}

	// Now create the same name space in the qualifier container.
	OW_StringArray ra;
	ra.append(QUAL_CONTAINER);
	if(!nameComps[0].equalsIgnoreCase(OW_ROOT_CONTAINER))
	{
		ra.append(OW_ROOT_CONTAINER);
	}

	ra.appendArray(nameComps);
	return OW_GenericHDBRepository::createNameSpace(ra, false);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::_throwIfBadClass(const OW_CIMClass& cc, const OW_CIMClass& parentClass)
{
	OW_Bool isKeyed = cc.isKeyed();

	enum QualState
	{
		UNSET,
		TRUE,
		FALSE
	};

	QualState isIndication = UNSET;
	QualState isAbstract = UNSET;
	
	if(!isKeyed)
	{
		// Right now. all reference fields are part of an association's key
		// We need to find out if this is the right thing to do.
		isKeyed = cc.isAssociation();
	}

	OW_CIMQualifierArray qra = cc.getQualifiers();
	if (parentClass)
	{
		OW_CIMQualifierArray pqra = parentClass.getQualifiers();
		for (OW_CIMQualifierArray::const_iterator iter = pqra.begin();
				iter != pqra.end(); ++iter)
		{
			qra.push_back(*iter);
		}
	}
	/*
	if (parentClass)
	{
		qra.appendArray(parentClass.getQualifiers());
	}
	*/
	for(size_t i = 0; i < qra.size(); i++)
	{
		OW_String qname = qra[i].getName();
		OW_CIMValue cv = qra[i].getValue();
		if(isAbstract == UNSET 
			&& qname.equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_ABSTRACT) 
			&& cv)
		{
			if (cv == OW_CIMValue(OW_Bool(true)))
			{
				isAbstract = TRUE;
			}
			else
			{
				isAbstract = FALSE;
			}
			if(isIndication != UNSET)
			{
				break;
			}
			continue;
		}

		if(isIndication == UNSET
			&& qname.equalsIgnoreCase(OW_CIMQualifier::CIM_QUAL_INDICATION)
			&& cv)
		{
			if (cv == OW_CIMValue(OW_Bool(true)))
			{
				isIndication = TRUE;
			}
			else
			{
				isIndication = FALSE;
			}
			if(isAbstract != UNSET)
			{
				break;
			}
		}
	}

	if(isIndication == FALSE && isAbstract == FALSE && !isKeyed)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Non-abstract class must have key properties: %1",
				cc.getName()).c_str());
	}
}


