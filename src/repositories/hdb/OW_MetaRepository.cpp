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
#include "OW_MetaRepository.hpp"
#include "OW_RepositoryStreams.hpp"
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

static const OW_String QUAL_CONTAINER("q");
static const OW_String CLASS_CONTAINER("c");

//////////////////////////////////////////////////////////////////////////////
OW_MetaRepository::~OW_MetaRepository()
{
}

//////////////////////////////////////////////////////////////////////////////
static void createRootNode(OW_String& qcontk, OW_HDBHandleLock& hdl)
{
	qcontk.toLowerCase();
	OW_HDBNode rnode = hdl->getNode(qcontk);
	if(!rnode)
	{
		rnode = OW_HDBNode(qcontk, qcontk.length()+1,
			reinterpret_cast<const unsigned char*>(qcontk.c_str()));

		hdl->turnFlagsOn(rnode, OW_HDBNSNODE_FLAG);
		hdl->addRootNode(rnode);
	}
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
    createRootNode(qcontk, hdl);

	qcontk += "/" + OW_String("root");
    createRootNode(qcontk,hdl);

    // Create root class container
    qcontk = CLASS_CONTAINER;
    createRootNode(qcontk, hdl);
    
    qcontk += "/" + OW_String("root");
    createRootNode(qcontk,hdl);

}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_MetaRepository::_getQualContainer(OW_HDBHandleLock& hdl, const OW_String& ns_)
{
	OW_String qcontk(QUAL_CONTAINER);
	OW_String ns(ns_);
	//while (!ns.empty() && ns[0] == '/')
	//{
	//	ns = ns.substring(1);
	//}
	if(!ns.empty())
	{
		qcontk += "/";                        
		qcontk += ns;
	}

	return getNameSpaceNode(hdl, qcontk);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_MetaRepository::_makeQualPath(const OW_String& ns_, const OW_String& qualName)
{
	OW_String ns(ns_);
	//while (!ns.empty() && ns[0] == '/')
	//{
	//	ns = ns.substring(1);
	//}

	OW_String qp(QUAL_CONTAINER);
    qp += "/";
	qp += ns;
    if (!qualName.empty())
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
	//while (!cp.empty() && cp[0] == '/')
	//{
	//	cp = cp.substring(1);
	//}
	OW_String cp(CLASS_CONTAINER);
    cp += "/";
    cp += ns;
	cp += "/";
	cp += className;

	return cp.toLowerCase();
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierType
OW_MetaRepository::getQualifierType(const OW_String& ns,
	const OW_String& qualName, OW_HDBHandle* phdl)
{
	throwIfNotOpen();
	OW_String qkey = _makeQualPath(ns, qualName);

	OW_CIMQualifierType qualType = m_qualCache.getFromCache(qkey);

    if (qualType)
    {
        return qualType;
    }

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
		if (nameSpaceExists(QUAL_CONTAINER + "/" + ns))
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				format("CIM QualifierType \"%1\" not found in namespace: %2",
					qualName, ns).c_str());
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
				ns.c_str());
		}
	}

    m_qualCache.addToCache(qualType, qkey);

	return qualType;
}

#ifndef OW_DISABLE_QUALIFIER_DECLARATION
//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::enumQualifierTypes(const OW_String& ns,
	OW_CIMQualifierTypeResultHandlerIFC& result)
{
	throwIfNotOpen();

	OW_String nskey = _makeQualPath(ns, OW_String());
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(nskey);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}

	if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
	{
		OW_THROW(OW_HDBException, "Expected namespace node");
	}

	node = hdl->getFirstChild(node);
	while(node)
	{
		// If this is not a namespace node, assume it's a qualifier
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			OW_CIMQualifierType qual(OW_CIMNULL);
			nodeToCIMObject(qual, node);
			result.handle(qual);
		}
		node = hdl->getNextSibling(node);
	}
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
	OW_CIMQualifierType qt(OW_CIMNULL);
	nodeToCIMObject(qt, node);

	// If we've hit this point, we know this is a qualifier

	hdl->removeNode(node);

    m_qualCache.removeFromCache(qkey);

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
		if(!ns.empty())
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

    m_qualCache.addToCache(qt, qkey);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::setQualifierType(const OW_String& ns,
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
		//OW_CIMQualifierType tqt(OW_CIMNULL);
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
OW_CIMException::ErrNoType
OW_MetaRepository::getCIMClass(const OW_String& ns, const OW_String& className,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList,
	OW_CIMClass& cc)
{
	throwIfNotOpen();
	OW_String ckey = _makeClassPath(ns, className);
	cc = m_classCache.getFromCache(ckey);
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
			m_classCache.addToCache(cc, ckey);
		}
		else
		{
			return OW_CIMException::NOT_FOUND;
		}
	}

	// now do some filtering
	if (propertyList || localOnly == true || includeQualifiers == false || includeClassOrigin == false)
	{ // only clone if we have to

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
		cc = cc.clone(localOnly, includeQualifiers, includeClassOrigin,
			lpropList, noProps);
		if (!cc) // clone doesn't throw
		{
			return OW_CIMException::FAILED;
		}
	}
	return OW_CIMException::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClass
OW_MetaRepository::_getClassFromNode(OW_HDBNode& node, OW_HDBHandle hdl,
	const OW_String& ns)
{
	OW_CIMClass theClass(OW_CIMNULL);
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
//     OW_CIMQualifier childAssocQual = child.getQualifier(OW_CIMQualifier::CIM_QUAL_ASSOCIATION);
//     if(childAssocQual)
//     {
//         if (!childAssocQual.getValue()
//             || childAssocQual.getValue() != OW_CIMValue(false))
//         {
//             child.setIsAssociation(true);
//         }
//     }

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
	OW_CIMClass parentClass(OW_CIMNULL);
	OW_String superID = child.getSuperClass();

	// If class doesn't have a super class - don't propagate anything
	// Should always have a parent because of namespaces
	if(superID.empty())
	{
		return;
	}

	OW_String pkey = _makeClassPath(ns, superID);
	parentClass = m_classCache.getFromCache(pkey);
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
		m_classCache.addToCache(parentClass, pkey);
	}

	//if(parentClass.isAssociation())
	//{
	//	child.setIsAssociation(true);
	//}
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
	// TODO: Regardless of whether there is an override
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
		else if (!childProp.getQualifier(OW_CIMQualifier::CIM_QUAL_OVERRIDE))
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
		else if (!childMethod.getQualifier(OW_CIMQualifier::CIM_QUAL_OVERRIDE))
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

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
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
	OW_CIMClass theClassToDelete(OW_CIMNULL);
	nodeToCIMObject(theClassToDelete, node);

	m_classCache.removeFromCache(ckey);		// Ensure class is not in the cache
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
		if(!(pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + "/" + ns)))
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


	OW_UInt32 flags = (cimClass.isAssociation()) ? OW_HDBCLSASSOCNODE_FLAG
		: 0;

	addCIMObject(cimClass, ckey, pnode, hdl.getHandle(), flags);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::modifyClass(const OW_String& ns,
	const OW_CIMClass& cimClass_)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_CIMClass cimClass(cimClass_);
	adjustClass(ns, cimClass, hdl.getHandle());

	OW_String ckey = _makeClassPath(ns, cimClass.getName());
	OW_HDBNode node = hdl->getNode(ckey);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, ckey.c_str());
	}

	// Create an OW_CIMClass object out of the node we just read to ensure
	// the data belongs to an OW_CIMClass.
	OW_CIMClass clsToUpdate(OW_CIMNULL);
	nodeToCIMObject(clsToUpdate, node);

	// At this point we know we are updating an OW_CIMClass
	m_classCache.removeFromCache(ckey);
	updateCIMObject(cimClass, node, hdl.getHandle());
}

//////////////////////////////////////////////////////////////////////////////
OW_HDBNode
OW_MetaRepository::adjustClass(const OW_String& ns, OW_CIMClass& childClass,
	OW_HDBHandle hdl)
{
	OW_String childName = childClass.getName();
	OW_String parentName = childClass.getSuperClass();
	OW_CIMClass parentClass(OW_CIMNULL);
    OW_HDBNode parentNode;

	if(!parentName.empty())
	{
		// Get the parent class
		OW_String superID = _makeClassPath(ns, parentName);
        //parentClass = m_classCache.getFromCache(superID);

        if (!parentClass)
        {
            parentNode = hdl.getNode(superID);
            if(!parentNode)
            {
                OW_THROWCIMMSG(OW_CIMException::INVALID_SUPERCLASS,
                    parentName.c_str());
            }

            parentClass = _getClassFromNode(parentNode, hdl, ns);
            if(!parentClass)
            {
                OW_THROWCIMMSG(OW_CIMException::INVALID_SUPERCLASS,
                    parentName.c_str());
            }
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
		//_throwIfBadClass(childClass, parentClass);
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
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
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
			if (propArray[i].getQualifier(OW_CIMQualifier::CIM_QUAL_OVERRIDE))
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
                OW_CIMQualifierArray parentQuals = parentProp.getQualifiers();
                for (size_t j = 0; j < parentQuals.size(); ++j)
                {
                    OW_CIMQualifier& qual = parentQuals[j];
                    // If the qualifier has DisableOverride flavor, the 
                    // subclass can't change it.  (e.g. Key). It gets the
                    // parent qualifier.
                    if (qual.hasFlavor(OW_CIMFlavor::DISABLEOVERRIDE))
                    {
                        if (!propArray[i].getQualifier(qual.getName()))
                        {
                            propArray[i].addQualifier(qual);
                        }
                        else
                        {
							logCustInfo(format("Error: %1.%2: qualifier %3 was "
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
                    else if (!qual.hasFlavor(OW_CIMFlavor::RESTRICTED))
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
			if(propArray[i].isKey())
			{
				// This is a key property, so the parent class better not be a
				// keyed class.
				if(parentClass.isKeyed())
				{
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
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
		if(parentClass.getMethod(methArray[i].getName()) &&
		   !methArray[i].getQualifier(OW_CIMQualifier::CIM_QUAL_OVERRIDE))
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
  // This shouldn't normally happen, because the association qualifier has
  // a DISABLEOVERRIDE flavor, so it will be caught in an earlier test.
  if(childClass.isAssociation() && !parentClass.isAssociation())
  {
	  OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
		  format("Association class is derived from non-association class: %1",
			  childClass.getName()).c_str());
  }

  //_throwIfBadClass(childClass, parentClass);
  return parentNode;
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

			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Unable to find qualifier: %1",
				quals[i].getName()).c_str());
		}
	}
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION


#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::getTopLevelAssociations(const OW_String& ns,
	OW_CIMClassResultHandlerIFC& result)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = getNameSpaceNode(hdl, CLASS_CONTAINER + "/" + ns);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
	}
	node = hdl->getFirstChild(node);
	while(node)
	{
		if(!node.areAllFlagsOn(OW_HDBNSNODE_FLAG)
			&& node.areAllFlagsOn(OW_HDBCLSASSOCNODE_FLAG))
		{
			OW_CIMClass cc(OW_CIMNULL);
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
OW_MetaRepository::enumClass(const OW_String& ns, const OW_String& className,
	OW_CIMClassResultHandlerIFC& result,
	OW_Bool deep, OW_Bool localOnly, OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode pnode;

	if(!className.empty())
	{
		OW_String ckey = _makeClassPath(ns, className);
		pnode = hdl->getNode(ckey);
		if(!pnode)
		{
			pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + "/" + ns);
			if(!pnode)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns.c_str());
			}
			else
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, className.c_str());
			}
		}
	}
	else
	{
		OW_String ns2(ns);
		while (!ns2.empty() && ns2[0] == '/')
		{
			ns2 = ns2.substring(1);
		}
		pnode = getNameSpaceNode(hdl, CLASS_CONTAINER + "/" + ns2);
		if(!pnode)
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE, ns2.c_str());
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
		OW_CIMClass parentClass(OW_CIMNULL);
		nodeToCIMObject(parentClass, pnode);
	}
	// */

	pnode = hdl->getFirstChild(pnode);
	while(pnode)
	{
		if(!pnode.areAllFlagsOn(OW_HDBNSNODE_FLAG))
		{
			_getClassNodes(result, pnode, hdl.getHandle(), deep, localOnly,
				includeQualifiers, includeClassOrigin);
		}

		pnode = hdl->getNextSibling(pnode);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MetaRepository::_getClassNodes(OW_CIMClassResultHandlerIFC& result, OW_HDBNode node,
	OW_HDBHandle hdl, OW_Bool deep, OW_Bool localOnly,
	OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_CIMClass cimCls = _getClassFromNode(node, hdl);
	result.handle(cimCls.clone(localOnly, includeQualifiers,
		includeClassOrigin));

	if(deep)
	{
		node = hdl.getFirstChild(node);
		while(node)
		{
			_getClassNodes(result, node, hdl, deep, localOnly, includeQualifiers,
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
		OW_CIMClass verifyClass(OW_CIMNULL);
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
	OW_CIMClass cimCls(OW_CIMNULL);
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

	OW_GenericHDBRepository::deleteNameSpace(QUAL_CONTAINER + "/" + nsName);
	OW_GenericHDBRepository::deleteNameSpace(CLASS_CONTAINER + "/" + nsName);
    /*
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = _getQualContainer(hdl, nsName);
	if(node)
	{
		hdl->removeNode(node);
	}
    */

	m_classCache.clearCache();
}

//////////////////////////////////////////////////////////////////////////////
int
OW_MetaRepository::createNameSpace(OW_String ns)
{
	// First create the name space in the class container.
	if(OW_GenericHDBRepository::createNameSpace(CLASS_CONTAINER + "/" + ns) == -1)
	{
		return -1;
	}

	// Now create the same name space in the qualifier container.
    // TODO: If the second create fails, we need to undo the first one.
	return OW_GenericHDBRepository::createNameSpace(QUAL_CONTAINER + "/" + ns);
}

//////////////////////////////////////////////////////////////////////////////
OW_MetaRepository::OW_MetaRepository(OW_ServiceEnvironmentIFCRef env)
	: OW_GenericHDBRepository(env)
{
	OW_String maxCacheSizeOpt = env->getConfigItem(OW_ConfigOpts::MAX_CLASS_CACHE_SIZE_opt, OW_DEFAULT_MAX_CLASS_CACHE_SIZE_S);
	try
	{
		m_classCache.setMaxCacheSize(maxCacheSizeOpt.toUInt32());
	}
	catch (const OW_StringConversionException&)
	{
		m_classCache.setMaxCacheSize(OW_DEFAULT_MAX_CLASS_CACHE_SIZE);
	}
}

