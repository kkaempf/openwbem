/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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

class OW_IndicationProviderTest1 : public OW_CppIndicationProviderIFC, public OW_CppInstanceProviderIFC
{
public:
	// Indication provider methods
	virtual int mustPoll(const OW_ProviderEnvironmentIFCRef &env, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&) 
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest1::mustPoll");
		// going to be lazy and make the cimom poll
		return 1;
	}
	virtual void authorizeFilter(const OW_ProviderEnvironmentIFCRef &env, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, const OW_String &) 
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest1::authorizeFilter");
		// This is called when someone creates a subscription for an indication we generate.
		// If we wanted to deny access and cause the subscription creation to fail, we would throw an OW_CIMException::ACCESS_DENIED exception here.  
	}
	virtual void activateFilter(const OW_ProviderEnvironmentIFCRef &env, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, bool) 
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest1::activateFilter");
		// this should never be called if we return > 0 from mustPoll
	}
	virtual void deActivateFilter(const OW_ProviderEnvironmentIFCRef &env, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, bool ) 
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest1::deActivateFilter");
		// this should never be called if we return > 0 from mustPoll
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
	virtual void getProviderInfo(OW_IndicationProviderInfo& info) 
	{
		// all the life-cycle indications that may be generated by this provider
        OW_IndicationProviderInfoEntry e("CIM_InstCreation");
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
	virtual void getProviderInfo(OW_InstanceProviderInfo& info)
	{
		// The class we are instrumenting
		info.addInstrumentedClass("OW_IndicationProviderTest1");
	}

	virtual void deleteInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Delete not supported");
	}

	virtual OW_CIMObjectPath createInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMInstance &) 
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Create not supported");
	}
	
	virtual OW_CIMInstance getInstance(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_String &ns, 
		const OW_CIMObjectPath &instanceName, 
		OW_Bool localOnly, 
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin, 
		const OW_StringArray *propertyList, 
		const OW_CIMClass &cimClass) 
	{
		(void)ns; (void)cimClass;
		env->getLogger()->logDebug("OW_IndicationProviderTest1::getInstance");
		OW_Int32 id = 0;
		try
		{
			id = instanceName.getKeyT("DeviceID").getValueT().toString().toInt32();
		}
		catch (OW_Exception& e)
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, "Invalid DeviceID property");
		}

		if (id < 0 || OW_UInt32(id) >= m_insts.size())
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, format("Invalid DeviceID property: %1", id).c_str());
		}
		return m_insts[id].clone(localOnly, includeQualifiers, includeClassOrigin, propertyList);
	}

	virtual void enumInstances(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_String &ns, 
		const OW_String &className, 
		OW_CIMInstanceResultHandlerIFC &result, 
		OW_Bool localOnly, 
		OW_Bool deep, 
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin, 
		const OW_StringArray *propertyList, 
		const OW_CIMClass &requestedClass, 
		const OW_CIMClass &cimClass) 
	{
		(void)ns; (void)className;
		env->getLogger()->logDebug("OW_IndicationProviderTest1::enumInstances");
		// we will simulate changing external conditions by calling updateInstances() every time enumInstances is called.
		// the changes will cause the cimom to send lifecycle indications.
		updateInstances(cimClass);
		for (size_t i = 0; i < m_insts.size(); ++i)
		{
			result.handle(m_insts[i].clone(localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass));
		}
	}
	
	virtual void enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_String &ns, 
		const OW_String &className, 
		OW_CIMObjectPathResultHandlerIFC &result, 
		const OW_CIMClass &cimClass) 
	{
		(void)ns; (void)className;
		env->getLogger()->logDebug("OW_IndicationProviderTest1::enumInstanceNames");
		// we will simulate changing external conditions by calling updateInstances() every time enumInstances is called.
		// the changes will cause the cimom to send lifecycle indications.
		updateInstances(cimClass);
		for (size_t i = 0; i < m_insts.size(); ++i)
		{
			result.handle(OW_CIMObjectPath(m_insts[i]));
		}
	}
	
	virtual void modifyInstance(
		const OW_ProviderEnvironmentIFCRef &, 
		const OW_String &, 
		const OW_CIMInstance &, 
		const OW_CIMInstance &, 
		OW_Bool , 
		const OW_StringArray *, 
		const OW_CIMClass &) 
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Modify not supported");
	}

private:
	OW_CIMInstanceArray m_insts;

	void updateInstances(const OW_CIMClass &cimClass)
	{
		if (m_insts.size() == 5)
		{
			m_insts.clear();
		}
		else
		{
			// add an instance
			OW_CIMInstance iToAdd(cimClass.newInstance());
			iToAdd.setProperty("SystemCreationClassName", OW_CIMValue("CIM_System"));
			iToAdd.setProperty("SystemName", OW_CIMValue("localhost"));
			iToAdd.setProperty("CreationClassName", OW_CIMValue("OW_IndicationProviderTest1"));
			iToAdd.setProperty("DeviceID", OW_CIMValue(OW_String(m_insts.size())));
			// PowerOnHours is our property that will be modified
			iToAdd.setProperty("PowerOnHours", OW_CIMValue(OW_UInt64(m_insts.size())));
			m_insts.push_back(iToAdd);

			// now modify the first instance's PowerOnHours property
			OW_UInt64 oldPowerOnHours = m_insts[0].getPropertyT("PowerOnHours").getValueT().toUInt64();
			m_insts[0].setProperty("PowerOnHours", OW_CIMValue(OW_UInt64(oldPowerOnHours + 1)));
		}
	}
};


} // end anonymous namespace


OW_PROVIDERFACTORY(OW_IndicationProviderTest1, indicationtest1)


