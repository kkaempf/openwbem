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
#include "OW_InstanceRepository2.hpp"
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

namespace OpenWBEM
{

using namespace WBEMFlags;

//////////////////////////////////////////////////////////////////////////////
InstanceRepository2::~InstanceRepository2()
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
InstanceRepository2::close()
{
	m_instanceNameLists.close();
	m_instances.close();
}

//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository2::open(::DB_ENV* env, ::DB_TXN* txn)
{
	m_instanceNameLists.open("instancenamelists", env, txn, dbDatabase::E_DUPLICATES);
	m_instances.open("instances", env, txn, dbDatabase::E_NO_DUPLICATES);
}

//////////////////////////////////////////////////////////////////////////////
class UtilKeyArray
{
public:
	UtilKeyArray() : m_names(), m_properties() {}
	void addElement(const CIMProperty& prop);
	String toString(const String& className);
private:
	StringArray m_names;
	StringArray m_properties;
};
//////////////////////////////////////////////////////////////////////////////
void
UtilKeyArray::addElement(const CIMProperty& prop)
{
	String propName = prop.getName().toLowerCase();
	for (size_t i = 0; i < m_names.size(); i++)
	{
		int cc = m_names[i].compareTo(propName);
		if (cc == 0)
		{
			m_properties[i] = prop.getValue().toString();
			return;
		}
		else if (cc > 0)
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
String
UtilKeyArray::toString(const String& className)
{
	StringBuffer rv(className.toString().toLowerCase());
	for (size_t i = 0; i < m_names.size(); i++)
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
String
InstanceRepository2::makeInstanceKey(const String& ns, const CIMObjectPath& cop,
	const CIMClass& theClass)
{
	if (!cop)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "no object path");
	}
	// Start return value with the namespace
	StringBuffer rv(makeClassKey(ns, cop.getClassName()));
	rv += '/';
	CIMPropertyArray kprops = theClass.getKeys();
	if (kprops.size() == 0)
	{
		rv += cop.getClassName();
		return rv.releaseString();
		// don't do this to allow for singleton classes without keys.
		//OW_THROWCIMMSG(CIMException::INVALID_CLASS,
		//	format("No key properties for class: %1", theClass.getName()).c_str());
	}
	String oclass = kprops[0].getOriginClass().toLowerCase();
	if (oclass.empty())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			format("No orgin class for key property on class: %1",
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
				"object path has key with missing value");
		}
	}
	// If not all the key properties were specified, throw an exception
	if (pra.size() < kprops.size())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			format("Model path is missing keys: %1", cop.toString()).c_str());
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
				format("Property in model path is not a key: %1", pname).c_str());
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
				format("Property in model path is not a key: %1", pname).c_str());
		}
	}
	UtilKeyArray kra;
	for (size_t i = 0; i < pra.size(); i++)
	{
		kra.addElement(pra[i]);
	}
	return kra.toString(rv.releaseString());
}
//////////////////////////////////////////////////////////////////////////////
String
InstanceRepository2::makeClassKey(const String& ns,
	const String& className)
{
	String rv(ns);
	while (!rv.empty() && rv[0] == '/')
	{
		rv = rv.substring(1);
	}
	rv += "/";
	rv += className;
	return rv.toLowerCase();
}
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository2::getInstanceNames(const String& ns,
	const CIMClass& theClass, CIMObjectPathResultHandlerIFC& result)
{
//     String className = theClass.getName();
//     HDBHandleLock hdl(this, getHandle());
//     String ckey = makeClassKey(ns, className);
//     HDBNode clsNode = hdl->getNode(ckey);
//     if (!clsNode)
//     {
//         OW_THROWCIM(CIMException::INVALID_CLASS);
//     }
//     if (!clsNode.areAllFlagsOn(HDBCLSNODE_FLAG))
//     {
//         OW_THROW(IOException, "Expected class name node for instances");
//     }
//     HDBNode node = hdl->getFirstChild(clsNode);
//     while (node)
//     {
//         CIMInstance ci(CIMNULL);
//         nodeToCIMObject(ci, node);
//         ci.syncWithClass(theClass); // need to do this to set up the keys
//         CIMObjectPath op(ci.getClassName(), ns);
//         op.setKeys(ci.getKeyValuePairs());
//         result.handle(op);
//         node = hdl->getNextSibling(node);
//     }
}

//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository2::getCIMInstances(
	const String& ns,
	const String& className,
	const CIMClass& requestedClass,
	const CIMClass& theClass, CIMInstanceResultHandlerIFC& result,
	EDeepFlag deep, ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin, const StringArray* propertyList)
{
//     throwIfNotOpen();
//     HDBHandleLock hdl(this, getHandle());
//     String ckey = makeClassKey(ns, className);
//     HDBNode clsNode = hdl->getNode(ckey);
//     if (!clsNode)
//     {
//         OW_THROWCIM(CIMException::INVALID_CLASS);
//     }
//     if (!clsNode.areAllFlagsOn(HDBCLSNODE_FLAG))
//     {
//         OW_THROW(IOException, "Expected class name node for instances");
//     }
//     HDBNode node = hdl->getFirstChild(clsNode);
//     while (node)
//     {
//         CIMInstance ci(CIMNULL);
//         nodeToCIMObject(ci, node);
//         ci = ci.clone(localOnly,deep,includeQualifiers,includeClassOrigin,propertyList,requestedClass,theClass);
//         result.handle(ci);
//         node = hdl->getNextSibling(node);
//     }
}
//////////////////////////////////////////////////////////////////////////////
CIMInstance
InstanceRepository2::getCIMInstance(
	const String& ns,
	const CIMObjectPath& instanceName,
	const CIMClass& theClass, ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList)
{
//     throwIfNotOpen();
//     String instanceKey = makeInstanceKey(ns, instanceName, theClass);
//     HDBHandleLock hdl(this, getHandle());
//     HDBNode node = hdl->getNode(instanceKey);
//     if (!node)
//     {
//         CIMObjectPath cop(instanceName);
//         cop.setNameSpace(ns);
//         OW_THROWCIMMSG(CIMException::NOT_FOUND, cop.toString().c_str());
//     }
//     CIMInstance ci(CIMNULL);
//     nodeToCIMObject(ci, node);
//     ci.syncWithClass(theClass, E_INCLUDE_QUALIFIERS);
//
//     // only filter if we need to
//     if (propertyList || localOnly == true || includeQualifiers == false || includeClassOrigin == false)
//     {
//         ci = ci.clone(localOnly, includeQualifiers, includeClassOrigin,
//             propertyList);
//     }
	
    CIMInstance ci(CIMNULL);
	return ci;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository2::deleteInstance(const String& ns, const CIMObjectPath& cop,
	const CIMClass& theClass)
{
//     throwIfNotOpen();
//     String instanceKey = makeInstanceKey(ns, cop, theClass);
//     HDBHandleLock hdl(this, getHandle());
//     HDBNode node = hdl->getNode(instanceKey);
//     if (!node)
//     {
//         CIMObjectPath cop2(cop);
//         cop2.setNameSpace(ns);
//         OW_THROWCIMMSG(CIMException::NOT_FOUND, cop2.toString().c_str());
//     }
//     // Ensure the node belongs to an CIMInstance before we delete it.
//     // If it's not, an exception will be thrown
//     CIMInstance ci(CIMNULL);
//     nodeToCIMObject(ci, node);
//     hdl->removeNode(node);
}
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository2::createInstance(const String& ns,
	const CIMClass& theClass, const CIMInstance& ci_)
{
//     throwIfNotOpen();
//     HDBHandleLock hdl(this, getHandle());
//     CIMInstance ci(ci_);
//     String ckey = makeClassKey(ns, ci.getClassName());
//     HDBNode clsNode = getNameSpaceNode(hdl, ckey);
//     if (!clsNode)
//     {
//         // Theoretically this should never happen, but just in case...
//         OW_THROWCIMMSG(CIMException::INVALID_CLASS, ci.getClassName().c_str());
//     }
//     // Create object path with keys from new instance
//     CIMObjectPath icop(ns, ci);
//     String instanceKey = makeInstanceKey(ns, icop, theClass);
//     HDBNode node = hdl->getNode(instanceKey);
//     if (node)
//     {
//         OW_THROWCIMMSG(CIMException::ALREADY_EXISTS, instanceKey.c_str());
//     }
//     _removeDuplicatedQualifiers(ci, theClass);
//     DataOStream ostrm;
//     ci.writeObject(ostrm);
//     node = HDBNode(instanceKey, ostrm.length(), ostrm.getData());
//     hdl.getHandle().addChild(clsNode, node);
}
//////////////////////////////////////////////////////////////////////////////
// TODO: Is this function actually used?
bool
InstanceRepository2::classHasInstances(const CIMObjectPath& classPath)
{
	bool cc = false;
//     throwIfNotOpen();
//     HDBHandleLock hdl(this, getHandle());
//     String ckey = makeClassKey(classPath.getNameSpace(),
//         classPath.getClassName());
//     HDBNode node = hdl->getNode(ckey);
//     if (node)
//     {
//         if (!node.areAllFlagsOn(HDBCLSNODE_FLAG))
//         {
//             OW_THROW(IOException, "Expected class name node for instances");
//         }
//         cc = node.hasChildren();
//     }
	return cc;
}
//////////////////////////////////////////////////////////////////////////////
void
InstanceRepository2::modifyInstance(const String& ns,
	const CIMObjectPath& cop,
	const CIMClass& theClass, const CIMInstance& ci_,
	const CIMInstance& oldInst,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList)
{
//     throwIfNotOpen();
//     HDBHandleLock hdl(this, getHandle());
//     CIMInstance ci(ci_.createModifiedInstance(oldInst,includeQualifiers,propertyList,theClass));
//     // Now sync the new instance with the class. This will remove any properties
//     // that shouldn't be on the instance and add properties that should be
//     // there.
//     ci.syncWithClass(theClass, E_EXCLUDE_QUALIFIERS);
//     // Ensure key values haven't changed
//     CIMPropertyArray oldKeys = oldInst.getKeyValuePairs();
//     for (size_t i = 0; i < oldKeys.size(); i++)
//     {
//         CIMProperty kprop = ci.getProperty(oldKeys[i].getName());
//         if (!kprop)
//         {
//             String msg("Missing key value: ");
//             msg += oldKeys[i].getName();
//             OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
//         }
//         CIMValue cv1 = kprop.getValue();
//         if (!cv1)
//         {
//             String msg("Missing key value: ");
//             msg += kprop.getName();
//             OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
//         }
//         CIMValue cv2 = oldKeys[i].getValue();
//         if (!cv2)
//         {
//             String msg("Missing key value in object path: ");
//             msg += oldKeys[i].getName();
//             OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
//         }
//         if (!cv1.sameType(cv2))
//         {
//             String msg("Data type for key property changed! property: ");
//             msg += kprop.getName();
//             OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, msg.c_str());
//         }
//         if (!cv1.equal(cv2))
//         {
//             String msg("key value for instance changed: ");
//             msg += kprop.getName();
//             OW_THROWCIMMSG(CIMException::FAILED, msg.c_str());
//         }
//     }
//     _removeDuplicatedQualifiers(ci, theClass);
//     DataOStream ostrm;
//     ci.writeObject(ostrm);
//     String instanceKey = makeInstanceKey(ns, cop, theClass);
//     HDBNode node = hdl->getNode(instanceKey);
//     if (!node)
//     {
//         OW_THROWCIMMSG(CIMException::NOT_FOUND, cop.toString().c_str());
//     }
//     hdl.getHandle().updateNode(node, ostrm.length(), ostrm.getData());
}
#endif
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////
void InstanceRepository2::_removeDuplicatedQualifiers(CIMInstance& inst,
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

} // end namespace OpenWBEM

