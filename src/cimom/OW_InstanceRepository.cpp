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
#include "OW_RepositoryStreams.hpp"
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

class UtilKeyArray
{
public:
	UtilKeyArray() : m_names(), m_properties() {}
	void addElement(const OW_CIMProperty& prop);
	OW_String toString(const OW_String& className);

private:
	OW_StringArray m_names;
	OW_StringArray m_properties;
};

//////////////////////////////////////////////////////////////////////////////
void
UtilKeyArray::addElement(const OW_CIMProperty& prop)
{
	OW_String propName = prop.getName().toLowerCase();

	for(size_t i = 0; i < m_names.size(); i++)
	{
		int cc = m_names[i].compareTo(propName);
		if(cc == 0)
		{
			m_properties[i] = prop.getValue().toString();
			return;
		}
		else if(cc > 0)
		{
			m_names.insert(i, propName);
			m_properties.insert(i, prop.getValue().toString());
			return;
		}
	}

	m_names.append(propName);
	m_properties.append(prop.getValue().toString());
}

//////////////////////////////////////////////////////////////////////////////
OW_String
UtilKeyArray::toString(const OW_String& className)
{
	OW_StringBuffer rv(className.toString().toLowerCase());

	for(size_t i = 0; i < m_names.size(); i++)
	{
		char c = (i == 0) ? '.' : ',';
		rv += c;
		rv += m_names[i];
		rv += '=';
		rv += m_properties[i];
	}

	return rv.releaseString();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_InstanceRepository::makeInstanceKey(const OW_String& ns, const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass)
{
	if(!cop)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "no object path");
	}

	OW_CIMPropertyArray kprops = theClass.getKeys();
	if(kprops.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS,
			format("No key properties for class: %1", theClass.getName()).c_str());
	}

	OW_String oclass = kprops[0].getOriginClass().toLowerCase();
	if(oclass.length() == 0)
	{
		OW_THROW(OW_Exception,
			format("No orgin class for key property on class: %1",
				theClass.getName()).c_str());
	}

	// Start return value with the namespace
	OW_String rv = makeClassKey(ns, cop.getObjectName()) + "/";
	rv += oclass;

	// Get keys from object path
	OW_CIMPropertyArray pra = cop.getKeys();
	if(pra.size() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"object path has no keys");
	}

	for(size_t i = 0; i < pra.size(); i++)
	{
		if(!pra[i].getValue())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"object path has key with missing value");
		}
	}

	// If not all the key properties were specified, throw an exception
	if(pra.size() < kprops.size())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			format("Model path is missing keys: %1", cop.toString()).c_str());
	}

	if(pra.size() == 1)
	{
		// If only one key property in object path, ensure it is
		// a key property in the class
		OW_String pname = pra[0].getName().toLowerCase();
		if(pname.length() > 0 && !pname.equalsIgnoreCase(kprops[0].getName()))
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Property in model path is not a key: %1", pname).c_str());
		}

		rv += ".";
		rv += pname + "=";

		OW_CIMValue cv = OW_CIMValueCast::castValueToDataType(pra[0].getValue(),
			kprops[0].getDataType());

		rv += cv.toString();
		return rv;
	}

	// Ensure no non-key properties were specified in the path
	for(size_t i = 0; i < pra.size(); i++)
	{
		OW_String pname = pra[i].getName();
		size_t j = 0;
		for(; j < kprops.size(); j++)
		{
			if(pname.equalsIgnoreCase(kprops[j].getName()))
			{
				OW_CIMValue cv = OW_CIMValueCast::castValueToDataType(
					pra[i].getValue(), kprops[j].getDataType());

				pra[i].setValue(cv);
				break;
			}
		}

		if(j == kprops.size())
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				format("Property in model path is not a key: %1", pname).c_str());
		}
	}

	UtilKeyArray kra;
	for(size_t i = 0; i < pra.size(); i++)
	{
		kra.addElement(pra[i]);
	}

	return kra.toString(rv);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_InstanceRepository::makeClassKey(const OW_String& ns,
	const OW_String& className)
{
	OW_String rv(ns);
	while (rv.length() > 0 && rv[0] == '/')
	{
		rv = rv.substring(1);
	}
	rv += "/";
	rv += className;
	return rv.toLowerCase();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::getInstanceNames(const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass, OW_CIMObjectPathResultHandlerIFC& result)
{
	throwIfNotOpen();
	OW_String ns = cop.getNameSpace();
	OW_String className = theClass.getName();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_String ckey = makeClassKey(ns, className);
	OW_HDBNode clsNode = hdl->getNode(ckey);
	if(!clsNode)
	{
		OW_THROWCIM(OW_CIMException::INVALID_CLASS);
	}

	if(!clsNode.areAllFlagsOn(OW_HDBCLSNODE_FLAG))
	{
		OW_THROW(OW_IOException, "Expected class name node for instances");
	}

	OW_HDBNode node = hdl->getFirstChild(clsNode);
	while(node)
	{
		OW_CIMInstance ci;
		nodeToCIMObject(ci, node);
		OW_CIMObjectPath op(ci.getClassName(), cop.getNameSpace());
		op.setKeys(ci.getKeyValuePairs());
		result.handle(op);
		node = hdl->getNextSibling(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::getCIMInstances(const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass, OW_CIMInstanceResultHandlerIFC& result,
	OW_Bool includeQualifiers,
	OW_Bool includeClassOrigin, const OW_StringArray* propertyList,
	OW_CIMServer* pServer, const OW_ACLInfo* pACLInfo)
{
	throwIfNotOpen();

	OW_String ns = cop.getNameSpace();
	OW_String className = theClass.getName();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_String ckey = makeClassKey(ns, className);
	OW_HDBNode clsNode = hdl->getNode(ckey);
	if(!clsNode)
	{
		OW_THROWCIM(OW_CIMException::INVALID_CLASS);
	}

	if(!clsNode.areAllFlagsOn(OW_HDBCLSNODE_FLAG))
	{
		OW_THROW(OW_IOException, "Expected class name node for instances");
	}

	OW_HDBNode node = hdl->getFirstChild(clsNode);
	while(node)
	{
		OW_CIMInstance ci;
		nodeToCIMObject(ci, node);
		ci.syncWithClass(theClass, true);

		if(pServer && pACLInfo)
		{
			OW_CIMObjectPath lcop(cop);
			lcop.setObjectName(ci.getClassName());
			lcop.setKeys(ci.getKeyValuePairs());

			pServer->_getProviderProperties(lcop, ci, theClass, *pACLInfo);
		}

		result.handle(ci.clone(false, includeQualifiers,
			includeClassOrigin, propertyList));
		node = hdl->getNextSibling(node);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_InstanceRepository::getCIMInstance(const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass)
{
	throwIfNotOpen();
	OW_String instanceKey = makeInstanceKey(cop.getNameSpace(), cop, theClass);

	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(instanceKey);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, cop.toString().c_str());
	}

	OW_CIMInstance ci;
	nodeToCIMObject(ci, node);
	return ci;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::deleteInstance(const OW_String& ns, const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass)
{
	throwIfNotOpen();
	OW_String instanceKey = makeInstanceKey(ns, cop, theClass);
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(instanceKey);
	if(!node)
	{
		OW_CIMObjectPath cop2(cop);
		cop2.setNameSpace(ns);
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, cop2.toString().c_str());
	}

	// Ensure the node belongs to an OW_CIMInstance before we delete it.
	// If it's not, an exception will be thrown
	OW_CIMInstance ci;
	nodeToCIMObject(ci, node);

	hdl->removeNode(node);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::createInstance(const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass, OW_CIMInstance& ci)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());

	OW_String ckey = makeClassKey(cop.getNameSpace(), ci.getClassName());
	OW_HDBNode clsNode = getNameSpaceNode(hdl, ckey);
	if(!clsNode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_CLASS, ci.getClassName().c_str());
	}

	// Make sure instance jives with class definition
	ci.syncWithClass(theClass, false);

	// Create object path with keys from new instance
	OW_CIMObjectPath icop(cop);
	icop.setKeys(ci.getKeyValuePairs());
	OW_String instanceKey = makeInstanceKey(icop.getNameSpace(), icop, theClass);
	OW_HDBNode node = hdl->getNode(instanceKey);
	if(node)
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS, instanceKey.c_str());
	}

	OW_RepositoryOStream ostrm;
	ci.writeObject(ostrm, false);
	node = OW_HDBNode(instanceKey, ostrm.length(), ostrm.getData());
	hdl.getHandle().addChild(clsNode, node);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_InstanceRepository::classHasInstances(const OW_CIMObjectPath& classPath)
{
	OW_Bool cc = false;
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_String ckey = makeClassKey(classPath.getNameSpace(),
		classPath.getObjectName());
	OW_HDBNode node = hdl->getNode(ckey);
	if(node)
	{
		if(!node.areAllFlagsOn(OW_HDBCLSNODE_FLAG))
		{
			OW_THROW(OW_IOException, "Expected class name node for instances");
		}

		cc = node.hasChildren();
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::modifyInstance(const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass, OW_CIMInstance& ci)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());

	// Get old instance
	OW_String instanceKey = makeInstanceKey(cop.getNameSpace(), cop, theClass);
	OW_HDBNode node = hdl->getNode(instanceKey);
	if(!node)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, cop.toString().c_str());
	}

	// Retrieve the old instance
	OW_CIMInstance oldInst;
	nodeToCIMObject(oldInst, node);

	// Now move properties from the old instance that are missing in the
	// new instance
	OW_CIMPropertyArray pra = oldInst.getProperties();
	OW_CIMPropertyArray npra = ci.getProperties();
	for(size_t i = 0; i < pra.size(); i++)
	{
		if(!ci.getProperty(pra[i].getName()))
		{
			npra.append(pra[i]);
		}
	}

	ci.setProperties(npra);

	// Now sync the new instance with the class. This will remove any properties
	// that shouldn't be on the instance and add properties that should be
	// there.
	ci.syncWithClass(theClass, false);

	// Ensure key values haven't changed
	OW_CIMPropertyArray oldKeys = oldInst.getKeyValuePairs();
	for(size_t i = 0; i < oldKeys.size(); i++)
	{
		OW_CIMProperty kprop = ci.getProperty(oldKeys[i].getName());
		if(!kprop)
		{
			OW_String msg("Missing key value: ");
			msg += oldKeys[i].getName();
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, msg.c_str());
		}

		OW_CIMValue cv1 = kprop.getValue();
		if(!cv1)
		{
			OW_String msg("Missing key value: ");
			msg += kprop.getName();
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, msg.c_str());
		}

		OW_CIMValue cv2 = oldKeys[i].getValue();
		if(!cv2)
		{
			OW_String msg("Missing key value in object path: ");
			msg += oldKeys[i].getName();
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, msg.c_str());
		}

		if(!cv1.sameType(cv2))
		{
			OW_String msg("Data type for key property changed! property: ");
			msg += kprop.getName();
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, msg.c_str());
		}

		if(!cv1.equal(cv2))
		{
			OW_String msg("key value for instance changed: ");
			msg += kprop.getName();
			OW_THROWCIMMSG(OW_CIMException::FAILED, msg.c_str());
		}
	}

	OW_RepositoryOStream ostrm;
	ci.writeObject(ostrm, false);
	hdl.getHandle().updateNode(node, ostrm.length(), ostrm.getData());
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_InstanceRepository::instanceExists(const OW_CIMObjectPath& cop,
	const OW_CIMClass& theClass)
{
	throwIfNotOpen();
	OW_Bool cc = false;
	OW_String instanceKey = makeInstanceKey(cop.getNameSpace(), cop, theClass);
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode node = hdl->getNode(instanceKey);
	if(node)
	{
		/*
		// Ensure this node is actually an OW_CIMInstance.
		// If not, an exception will be thrown
		OW_CIMInstance verifyInst;
		nodeToCIMObject(verifyInst, node);
		*/

		cc = true;
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::deleteNameSpace(const OW_String& nsName)
{
	throwIfNotOpen();

	// ATTN: Do we need to do more later? Associations?
	OW_GenericHDBRepository::deleteNameSpace(nsName);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_InstanceRepository::createNameSpace(const OW_StringArray& nameComps,
	OW_Bool rootCheck)
{
	return OW_GenericHDBRepository::createNameSpace(nameComps, rootCheck);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::createClass(const OW_String& ns,
	const OW_CIMClass& cimClass)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_HDBNode pnode = getNameSpaceNode(hdl, ns);
	if(!pnode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_NAMESPACE,
			ns.c_str());
	}

	OW_String ckey = makeClassKey(ns, cimClass.getName());
	OW_HDBNode node = hdl->getNode(ckey);
	if(node)
	{
		OW_THROWCIMMSG(OW_CIMException::ALREADY_EXISTS, ckey.c_str());
	}

	node = OW_HDBNode(ckey, ckey.length()+1,
		(unsigned char*) ckey.c_str());
	hdl->turnFlagsOn(node, OW_HDBNSNODE_FLAG | OW_HDBCLSNODE_FLAG);
	hdl->addChild(pnode, node);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_InstanceRepository::deleteClass(const OW_String& ns,
	const OW_String& className)
{
	throwIfNotOpen();
	OW_HDBHandleLock hdl(this, getHandle());
	OW_String ckey = makeClassKey(ns, className);
	OW_HDBNode node = hdl->getNode(ckey);
	if(node)
	{
		if(!node.areAllFlagsOn(OW_HDBCLSNODE_FLAG))
		{
			OW_THROW(OW_IOException, "Expected class name node for instances");
		}
	}

	hdl->removeNode(ckey);
}





