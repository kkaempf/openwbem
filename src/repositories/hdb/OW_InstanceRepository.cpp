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
#include "OW_InstanceRepository.hpp"
#include "OW_DataStreams.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_Format.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_IOException.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;

namespace
{

const char NS_SEPARATOR_C(':');


} // end unnamed namespace
//////////////////////////////////////////////////////////////////////////////
String
InstanceRepository::makeInstanceKey(const String& ns, const CIMObjectPath& cop,
	const CIMClass& theClass)
{
	if (!cop)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "no object path");
	}
	// Start return value with the namespace
	StringBuffer rv(makeClassKey(ns, cop.getClassName()));
	rv += NS_SEPARATOR_C;
	CIMPropertyArray kprops = theClass.getKeys();
	if (kprops.size() == 0)
	{
		rv += cop.getClassName();
		return rv.releaseString();
		// don't do this to allow for singleton classes without keys.
		//OW_THROWCIMMSG(CIMException::INVALID_CLASS,
		//	Format("No key properties for class: %1", theClass.getName()).c_str());
	}
	String oclass = kprops[0].getOriginClass().toLowerCase();
	if (oclass.empty())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("No orgin class for key property on class: %1",
				theClass.getName()).c_str());
	}
	rv += oclass;
	// Get keys from object path
	CIMPropertyArray pra = cop.getKeys();
	if (pra.size() == 0)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"object path has no keys");
	}
	for (size_t i = 0; i < pra.size(); i++)
	{
		if (!pra[i].getValue())
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("object path (%1) has key (%2) with NULL value", cop, pra[i].getName()).c_str());
		}
	}
	// If not all the key properties were specified, throw an exception
	if (pra.size() < kprops.size())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			Format("Model path is missing keys: %1", cop.toString()).c_str());
	}
	// TODO: Is this necessary?
	if (pra.size() == 1)
	{
		// If only one key property in object path, ensure it is
		// a key property in the class
		String pname = pra[0].getName().toLowerCase();
		if (!pname.empty() && !pname.equalsIgnoreCase(kprops[0].getName()))
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Property in model path is not a key: %1", pname).c_str());
		}
		rv += '.';
		rv += pname;
		rv += '=';
		CIMValue cv = CIMValueCast::castValueToDataType(pra[0].getValue(),
			kprops[0].getDataType());
		if (cv.getType() == CIMDataType::REFERENCE)
		{
			CIMObjectPath cop(cv.toCIMObjectPath());
			if (cop.getNameSpace().empty())
			{
				cop.setNameSpace(ns);
				cv = CIMValue(cop);
			}
		}
		rv += cv.toString();
		return rv.releaseString();
	}
	// TODO: Is this necessary?
	// Ensure no non-key properties were specified in the path
	for (size_t i = 0; i < pra.size(); i++)
	{
		String pname = pra[i].getName();
		size_t j = 0;
		for (; j < kprops.size(); j++)
		{
			if (pname.equalsIgnoreCase(kprops[j].getName()))
			{
				CIMValue cv = CIMValueCast::castValueToDataType(
					pra[i].getValue(), kprops[j].getDataType());
				if (cv.getType() == CIMDataType::REFERENCE)
				{
					CIMObjectPath cop(cv.toCIMObjectPath());
					if (cop.getNameSpace().empty())
					{
						cop.setNameSpace(ns);
						cv = CIMValue(cop);
					}
				}
				pra[i].setValue(cv);
				break;
			}
		}
		if (j == kprops.size())
		{
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Property in model path is not a key: %1", pname).c_str());
		}
	}

	HDBUtilKeyArray kra(pra);
	kra.toString(rv);
	return rv.releaseString();
}
//////////////////////////////////////////////////////////////////////////////
String
InstanceRepository::makeClassKey(const String& ns,
	const String& className)
{
	String rv(ns);
	rv += NS_SEPARATOR_C;
	rv += className;
	return rv.toLowerCase();
}
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::getInstanceNames(const String& ns,
	const CIMClass& theClass, CIMObjectPathResultHandlerIFC& result)
{
	throwIfNotOpen();
	String className = theClass.getName();
	HDBHandleLock hdl(this, getHandle());
	String ckey = makeClassKey(ns, className);
	HDBNode clsNode = hdl->getNode(ckey);
	if (!clsNode)
	{
		OW_THROWCIM(CIMException::INVALID_CLASS);
	}
	if (!clsNode.areAllFlagsOn(HDBCLSNODE_FLAG))
	{
		OW_THROW(IOException, "Expected class name node for instances");
	}
	HDBNode node = hdl->getFirstChild(clsNode);
	while (node)
	{
		CIMInstance ci(CIMNULL);
		nodeToCIMObject(ci, node);
		ci.syncWithClass(theClass, E_INCLUDE_QUALIFIERS); // need to do this to set up the keys
		CIMObjectPath op(ci.getClassName(), ns);
		op.setKeys(ci.getKeyValuePairs());
		result.handle(op);
		node = hdl->getNextSibling(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::getCIMInstances(
	const String& ns,
	const String& className,
	const CIMClass& requestedClass,
	const CIMClass& theClass, CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	String ckey = makeClassKey(ns, className);
	HDBNode clsNode = hdl->getNode(ckey);
	if (!clsNode)
	{
		OW_THROWCIM(CIMException::INVALID_CLASS);
	}
	if (!clsNode.areAllFlagsOn(HDBCLSNODE_FLAG))
	{
		OW_THROW(IOException, "Expected class name node for instances");
	}
	HDBNode node = hdl->getFirstChild(clsNode);
	while (node)
	{
		CIMInstance ci(CIMNULL);
		nodeToCIMObject(ci, node);
		ci = ci.clone(localOnly,deep,includeQualifiers,includeClassOrigin,propertyList,requestedClass,theClass);
		result.handle(ci);
		node = hdl->getNextSibling(node);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
InstanceRepository::getCIMInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	const CIMClass& theClass, ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
	throwIfNotOpen();
	String instanceKey = makeInstanceKey(ns, instanceName, theClass);
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(instanceKey);
	if (!node)
	{
		CIMObjectPath cop(instanceName);
		cop.setNameSpace(ns);
		OW_THROWCIMMSG(CIMException::NOT_FOUND, cop.toString().c_str());
	}
	CIMInstance ci(CIMNULL);
	nodeToCIMObject(ci, node);
	ci.syncWithClass(theClass, E_INCLUDE_QUALIFIERS);
	
	// only filter if we need to
	if (propertyList
		|| localOnly == E_LOCAL_ONLY
		|| includeQualifiers == E_EXCLUDE_QUALIFIERS
		|| includeClassOrigin == E_EXCLUDE_CLASS_ORIGIN)
	{
		ci = ci.clone(localOnly, includeQualifiers, includeClassOrigin,
			propertyList);
	}
	
	ci.setNameSpace(ns); 
	return ci;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::deleteInstance(const String& ns, const CIMObjectPath& cop,
	const CIMClass& theClass)
{
	throwIfNotOpen();
	String instanceKey = makeInstanceKey(ns, cop, theClass);
	HDBHandleLock hdl(this, getHandle());
	HDBNode node = hdl->getNode(instanceKey);
	if (!node)
	{
		CIMObjectPath cop2(cop);
		cop2.setNameSpace(ns);
		OW_THROWCIMMSG(CIMException::NOT_FOUND, cop2.toString().c_str());
	}
	// Ensure the node belongs to an CIMInstance before we delete it.
	// If it's not, an exception will be thrown
	CIMInstance ci(CIMNULL);
	nodeToCIMObject(ci, node);
	hdl->removeNode(node);
}
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::createInstance(const String& ns,
	const CIMClass& theClass, const CIMInstance& ci_)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	CIMInstance ci(ci_);
	String ckey = makeClassKey(ns, ci.getClassName());
	HDBNode clsNode = getNameSpaceNode(hdl, ckey);
	if (!clsNode)
	{
		// Theoretically this should never happen, but just in case...
		OW_THROWCIMMSG(CIMException::INVALID_CLASS, ci.getClassName().c_str());
	}
	// Create object path with keys from new instance
	CIMObjectPath icop(ns, ci);
	String instanceKey = makeInstanceKey(ns, icop, theClass);
	HDBNode node = hdl->getNode(instanceKey);
	if (node)
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, instanceKey.c_str());
	}
	_removeDuplicatedQualifiers(ci, theClass);
	DataOStreamBuf ostrm;
	ci.writeObject(ostrm);
	node = HDBNode(instanceKey, ostrm.length(), ostrm.getData());
	hdl.getHandle().addChild(clsNode, node);
}
//////////////////////////////////////////////////////////////////////////////
// TODO: Is this function actually used?
bool
InstanceRepository::classHasInstances(const CIMObjectPath& classPath)
{
	bool cc = false;
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	String ckey = makeClassKey(classPath.getNameSpace(),
		classPath.getClassName());
	HDBNode node = hdl->getNode(ckey);
	if (node)
	{
		if (!node.areAllFlagsOn(HDBCLSNODE_FLAG))
		{
			OW_THROW(IOException, "Expected class name node for instances");
		}
		cc = node.hasChildren();
	}
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::modifyInstance(const String& ns,
	const CIMObjectPath& cop,
	const CIMClass& theClass, const CIMInstance& ci_,
	const CIMInstance& oldInst,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	CIMInstance ci(ci_.createModifiedInstance(oldInst,includeQualifiers,propertyList,theClass));
	// Now sync the new instance with the class. This will remove any properties
	// that shouldn't be on the instance and add properties that should be
	// there.
	ci.syncWithClass(theClass, E_INCLUDE_QUALIFIERS);
	// Ensure key values haven't changed
	CIMPropertyArray oldKeys = oldInst.getKeyValuePairs();
	for (size_t i = 0; i < oldKeys.size(); i++)
	{
		CIMProperty kprop = ci.getProperty(oldKeys[i].getName());
		if (!kprop)
		{
			String msg("Missing key value: ");
			msg += oldKeys[i].getName();
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
		}
		CIMValue cv1 = kprop.getValue();
		if (!cv1)
		{
			String msg("Missing key value: ");
			msg += kprop.getName();
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
		}
		CIMValue cv2 = oldKeys[i].getValue();
		if (!cv2)
		{
			String msg("Missing key value in object path: ");
			msg += oldKeys[i].getName();
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
		}
		if (!cv1.sameType(cv2))
		{
			String msg("Data type for key property changed! property: ");
			msg += kprop.getName();
			OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
		}
		if (!cv1.equal(cv2))
		{
			String msg("key value for instance changed: ");
			msg += kprop.getName();
			OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
		}
	}
	_removeDuplicatedQualifiers(ci, theClass);
	DataOStreamBuf ostrm;
	ci.writeObject(ostrm);
	String instanceKey = makeInstanceKey(ns, cop, theClass);
	HDBNode node = hdl->getNode(instanceKey);
	if (!node)
	{
		OW_THROWCIMMSG(CIMException::NOT_FOUND, cop.toString().c_str());
	}
	if (!hdl.getHandle().updateNode(node, HDBNode(), ostrm.length(), ostrm.getData()))
	{
		OW_THROW(HDBException, "Failed to update repository node");
	}
}
#endif

#if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::deleteNameSpace(const String& nsName)
{
	throwIfNotOpen();
	// ATTN: Do we need to do more later? Associations?
	GenericHDBRepository::deleteNameSpace(nsName);
}
//////////////////////////////////////////////////////////////////////////////
int
InstanceRepository::createNameSpace(const String& ns)
{
	return GenericHDBRepository::createNameSpace(ns);
}
#endif // #if !defined(OW_DISABLE_INSTANCE_MANIPULATION) && !defined(OW_DISABLE_NAMESPACE_MANIPULATION)

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::createClass(const String& ns,
	const CIMClass& cimClass)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	HDBNode pnode = getNameSpaceNode(hdl, ns);
	if (!pnode)
	{
		OW_THROWCIMMSG(CIMException::INVALID_NAMESPACE,
			ns.c_str());
	}
	String ckey = makeClassKey(ns, cimClass.getName());
	HDBNode node = hdl->getNode(ckey);
	if (node)
	{
		OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, ckey.c_str());
	}
	node = HDBNode(ckey, ckey.length()+1,
		reinterpret_cast<const unsigned char*>(ckey.c_str()));
	hdl->turnFlagsOn(node, HDBNSNODE_FLAG | HDBCLSNODE_FLAG);
	hdl->addChild(pnode, node);
}
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository::deleteClass(const String& ns,
	const String& className)
{
	throwIfNotOpen();
	HDBHandleLock hdl(this, getHandle());
	String ckey = makeClassKey(ns, className);
	HDBNode node = hdl->getNode(ckey);
	if (node)
	{
		if (!node.areAllFlagsOn(HDBCLSNODE_FLAG))
		{
			OW_THROW(IOException, "Expected class name node for instances");
		}
	}
	hdl->removeNode(ckey);
}
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void InstanceRepository::_removeDuplicatedQualifiers(CIMInstance& inst,
	const CIMClass& theClass)
{
	CIMQualifierArray quals(inst.getQualifiers());
	CIMQualifierArray newQuals;
	for (size_t i = 0; i < quals.size(); ++i)
	{
		CIMQualifier& iq = quals[i];
		CIMQualifier cq = theClass.getQualifier(iq.getName());
		if (!cq)
		{
			newQuals.push_back(iq);
			continue;
		}
		if (iq.getValue() != cq.getValue())
		{
			newQuals.push_back(iq);
			continue;
		}
	}
	inst.setQualifiers(newQuals);
	CIMPropertyArray props = inst.getProperties();
	for (size_t i = 0; i < props.size(); ++i)
	{
		CIMProperty& p = props[i];
		CIMProperty clsp = theClass.getProperty(p.getName());
		CIMQualifierArray quals(p.getQualifiers());
		CIMQualifierArray newQuals;
		for (size_t j = 0; j < quals.size(); ++j)
		{
			CIMQualifier& ipq = quals[j];
			CIMQualifier cpq = clsp.getQualifier(ipq.getName());
			if (!cpq)
			{
				newQuals.push_back(ipq);
				continue;
			}
			if (ipq.getValue() != cpq.getValue())
			{
				newQuals.push_back(ipq);
				continue;
			}
		}
		p.setQualifiers(newQuals);
	}
	inst.setProperties(props);
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

} // end namespace OW_NAMESPACE

