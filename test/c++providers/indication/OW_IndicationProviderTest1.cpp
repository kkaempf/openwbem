/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"

#include "OW_CppIndicationProviderIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMException.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Format.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"

using namespace OpenWBEM;
using namespace WBEMFlags;

// anonymous namespace to prevent symbol conflicts
namespace
{

// This is an example/test of a simple instance/indication provider.  It 
// returns non-zero from mustPoll, so the CIMOM will then "poll" the
// provider by calling enumInstances.  The number of seconds between
// polls is indicated by the return value of mustPoll.  The CIMOM
// will generate life-cycle indications (CIM_InstCreation, 
// CIM_InstModification, CIM_InstDeletion) for any changes in the set of 
// instances returned by enumInstances.

class IndicationProviderTest1 : public CppIndicationProviderIFC, public CppInstanceProviderIFC
{
public:
	// Indication provider methods - Only have to implement getIndicationProviderInfo() and mustPoll()
	// The default implementation of the others suits us fine.
	virtual int mustPoll(const ProviderEnvironmentIFCRef &env, const WQLSelectStatement &, const String &, const String&, const StringArray&) 
	{
		env->getLogger()->logDebug("IndicationProviderTest1::mustPoll");
		// going to be lazy and make the cimom poll
		return 1;
	}

	/**
	 * A provider should override this method to report which classes in
	 * which namespaces it instruments.
	 * It should insert an entry for each class it is responsible for.
	 *  The entry consists of the class name and an optional list of namespaces.
	 *  If the namespace list is empty, all namespaces are implied.
	 * If the method does nothing, then the provider's class must have a
	 * provider qualifier that identifies the provider.  This old behavior is
	 * deprecated and will be removed sometime in the future.  This method
	 * has a default implementation that does nothing, to allow old providers
	 * to be migrated forward with little or no change, but once the old
	 * provider location method is removed, this member function will be pure
	 * virtual.
	 */
	virtual void getIndicationProviderInfo(IndicationProviderInfo& info) 
	{
		// all the life-cycle indications that may be generated by this provider
		IndicationProviderInfoEntry e("CIM_InstCreation");

		// If IndicationProviderTest1 was derived from other classes, we 
		// would need to add all the base classes in as well.
		e.classes.push_back("OW_IndicationProviderTest1");
		info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstModification";
		info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstDeletion";
		info.addInstrumentedClass(e);
		
		// We doesn't directly generate these, but they are
		// base classes of the ones we do generate.  We need to report these
		// so that if someone has a query like "SELECT * FROM CIM_Indication"
		// we will be called.
		e.indicationName = "CIM_InstIndication";
		info.addInstrumentedClass(e);
		e.indicationName = "CIM_Indication";
		info.addInstrumentedClass(e);
	}

	// These are the indication provider methods.
	virtual void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		// The class we are instrumenting
		info.addInstrumentedClass("OW_IndicationProviderTest1");
	}

	virtual void deleteInstance(const ProviderEnvironmentIFCRef &, const String &, const CIMObjectPath &)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Delete not supported");
	}

	virtual CIMObjectPath createInstance(const ProviderEnvironmentIFCRef &, const String &, const CIMInstance &) 
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Create not supported");
	}
	
	virtual CIMInstance getInstance(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const CIMObjectPath &instanceName, 
		ELocalOnlyFlag localOnly, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray *propertyList, 
		const CIMClass &cimClass) 
	{
		(void)ns; (void)cimClass;
		env->getLogger()->logDebug("IndicationProviderTest1::getInstance");
		Int32 id = 0;
		try
		{
			id = instanceName.getKeyT("DeviceID").getValueT().toString().toInt32();
		}
		catch (Exception& e)
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND, "Invalid DeviceID property");
		}

		if (id < 0 || UInt32(id) >= m_insts.size())
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND, Format("Invalid DeviceID property: %1", id).c_str());
		}
		return m_insts[id].clone(localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}

	virtual void enumInstances(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const String &className, 
		CIMInstanceResultHandlerIFC &result, 
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray *propertyList, 
		const CIMClass &requestedClass, 
		const CIMClass &cimClass) 
	{
		(void)ns; (void)className;
		env->getLogger()->logDebug("IndicationProviderTest1::enumInstances");
		// we will simulate changing external conditions by calling updateInstances() every time enumInstances is called.
		// the changes will cause the cimom to send lifecycle indications.
		updateInstances(cimClass);
		for (size_t i = 0; i < m_insts.size(); ++i)
		{
			result.handle(m_insts[i].clone(localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass));
		}
	}
	
	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef &env, 
		const String &ns, 
		const String &className, 
		CIMObjectPathResultHandlerIFC &result, 
		const CIMClass &cimClass) 
	{
		(void)className;
		env->getLogger()->logDebug("IndicationProviderTest1::enumInstanceNames");
		// we will simulate changing external conditions by calling updateInstances() every time enumInstances is called.
		// the changes will cause the cimom to send lifecycle indications.
		updateInstances(cimClass);
		for (size_t i = 0; i < m_insts.size(); ++i)
		{
			result.handle(CIMObjectPath(ns, m_insts[i]));
		}
	}
	
	virtual void modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		(void)env; (void)ns; (void)modifiedInstance; (void)previousInstance; (void)includeQualifiers; (void)propertyList; (void)theClass;
		OW_THROWCIMMSG(CIMException::FAILED, "Modify not supported");
	}

private:
	CIMInstanceArray m_insts;

	void updateInstances(const CIMClass &cimClass)
	{
		if (m_insts.size() == 5)
		{
			m_insts.clear();
		}
		else
		{
			// add an instance
			CIMInstance iToAdd(cimClass.newInstance());
			iToAdd.setProperty("SystemCreationClassName", CIMValue("CIM_System"));
			iToAdd.setProperty("SystemName", CIMValue("localhost"));
			iToAdd.setProperty("CreationClassName", CIMValue("OW_IndicationProviderTest1"));
			iToAdd.setProperty("DeviceID", CIMValue(String(UInt32(m_insts.size()))));
			// PowerOnHours is our property that will be modified
			iToAdd.setProperty("PowerOnHours", CIMValue(UInt64(m_insts.size())));
			m_insts.push_back(iToAdd);

			// now modify the first instance's PowerOnHours property
			UInt64 oldPowerOnHours = m_insts[0].getPropertyT("PowerOnHours").getValueT().toUInt64();
			m_insts[0].setProperty("PowerOnHours", CIMValue(UInt64(oldPowerOnHours + 1)));
		}
	}
};


} // end anonymous namespace


OW_PROVIDERFACTORY(IndicationProviderTest1, indicationtest1)


