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
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CIMException.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Format.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_Thread.hpp"
#include "OW_Mutex.hpp"
#include "OW_Condition.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_Assertion.hpp"
#include "OW_MutexLock.hpp"

// TODO: Find a way to get rid of these
#include "OW_PollingManager.hpp"
#include "OW_CppProxyProvider.hpp"

// anonymous namespace to prevent library symbol conflicts
namespace
{

// This is an example/test of a simple instance/indication/polled provider.  It 
// doesn't implement mustPoll.  The CIMOM will call the *Filter methods.
// We'll start up a the polled provider to "watch" the objects we are 
// instrumenting.  Really, we'll just change them once a second, so this
// provider has something to do.
// Once the subscriptions are deleted, deActivateFilter will cause the filter
// counts to be 0.  Then the next time poll() is run, it will return 0, and
// this will cause the polling manager to stop polling the provider.

// This example is bordering on the edge of complexity where it should be
// split up into a couple of source and header files...


class OW_IndicationProviderTest3 : public OW_CppIndicationProviderIFC, public OW_CppInstanceProviderIFC, public OW_CppPolledProviderIFC
{
public:
	
	OW_IndicationProviderTest3()
		: m_theClass(OW_CIMNULL)
		, m_creationFilterCount(0)
		, m_modificationFilterCount(0)
		, m_deletionFilterCount(0)
	{
	}

	~OW_IndicationProviderTest3()
	{
	}

	virtual void activateFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes, 
		bool firstActivation)
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest3::activateFilter");
		
		// eventType contains the name of the indication the listener subscribed to.
		// this will be one of the class names we indicated in getIndicationProviderInfo(OW_IndicationProviderInfo& info)
		// so for this provider (and most other life cycle indication providers), it's got to be one of:
		// CIM_InstCreation, CIM_InstModification, or CIM_InstDeletion
		if (eventType.equalsIgnoreCase("CIM_InstCreation"))
		{
			addCreationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstModification"))
		{
			addModificationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstDeletion"))
		{
			addDeletionFilter();
		}
        else if (eventType.equalsIgnoreCase("CIM_InstIndication") || eventType.equalsIgnoreCase("CIM_Indication"))
        {
            addCreationFilter();
            addModificationFilter();
            addDeletionFilter();
        }
		else
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! eventType is incorrect!");
		}
		// classes should either be empty (meaning the filter didn't contain an ISA clause), or contain a OW_IndicationProviderTest2 classname
		// (this is the case if the filter contains "SourceInstance ISA OW_IndicationProviderTest3")
		if (!classes.empty() && std::find(classes.begin(), classes.end(), "OW_IndicationProviderTest3") == classes.end())
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! classPath is incorrect!");
		}

		if (!m_theClass)
		{
			m_theClass = env->getCIMOMHandle()->getClass(nameSpace, "OW_IndicationProviderTest3");
		}

		OW_WQLCompile comp(filter);
		// do something with comp
		(void)comp;

		if (firstActivation)
		{
			// TODO: Make a better way to do this that doesn't bypass the provider manager or use a no-delete reference or use g_cimomEnvironment.
			OW_PollingManagerRef pm = OW_CIMOMEnvironment::g_cimomEnvironment->getPollingManager();
//			pm->addPolledProvider(OW_PolledProviderIFCRef(new OW_CppPolledProviderProxy(OW_CppPolledProviderIFCRef(OW_SharedLibraryRef(), OW_Reference<OW_CppPolledProviderIFC>(this, true)))));
		}
	}
	
	virtual void deActivateFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& /*nameSpace*/,
		const OW_StringArray& classes, 
		bool lastActivation)
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest3::deActivateFilter");
		
		// eventType contains the name of the indication the listener subscribed to.
		// this will be one of the class names we indicated in getIndicationProviderInfo(OW_IndicationProviderInfo& info)
		// so for this provider (and most other life cycle indication providers), it's got to be one of:
		// CIM_InstCreation, CIM_InstModification, or CIM_InstDeletion
		if (eventType.equalsIgnoreCase("CIM_InstCreation"))
		{
			removeCreationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstModification"))
		{
			removeModificationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstDeletion"))
		{
			removeDeletionFilter();
		}
        else if (eventType.equalsIgnoreCase("CIM_InstIndication") || eventType.equalsIgnoreCase("CIM_Indication"))
        {
            removeCreationFilter();
            removeModificationFilter();
            removeDeletionFilter();
        }
		else
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! eventType is incorrect!");
		}
		// classes should either be empty (meaning the filter didn't contain an ISA clause), or contain a OW_IndicationProviderTest2 classname
		// (this is the case if the filter contains "SourceInstance ISA OW_IndicationProviderTest3")
		if (!classes.empty() && std::find(classes.begin(), classes.end(), "OW_IndicationProviderTest3") == classes.end())
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! classPath is incorrect!");
		}

		OW_WQLCompile comp(filter);
		// do something with comp
		(void)comp;
		
		if (lastActivation)
		{
			OW_MutexLock l(m_guard);
            OW_ASSERT(m_creationFilterCount == 0);
			OW_ASSERT(m_modificationFilterCount == 0);
			OW_ASSERT(m_deletionFilterCount == 0);
		}
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
	virtual void getIndicationProviderInfo(OW_IndicationProviderInfo& info) 
	{
		// all the life-cycle indications that may be generated by this provider
        OW_IndicationProviderInfoEntry e("CIM_InstCreation");

		// If OW_IndicationProviderTest3 was derived from other classes, we 
		// would need to add all the base classes in as well.
        e.classes.push_back("OW_IndicationProviderTest3");
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
	virtual void getInstanceProviderInfo(OW_InstanceProviderInfo& info)
	{
		// The class we are instrumenting
		info.addInstrumentedClass("OW_IndicationProviderTest3");
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
		env->getLogger()->logDebug("OW_IndicationProviderTest3::getInstance");
		OW_Int32 id = 0;
		try
		{
			id = instanceName.getKeyT("DeviceID").getValueT().toString().toInt32();
		}
		catch (OW_Exception& e)
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, "Invalid DeviceID property");
		}

		// m_insts could be accessed from multiple threads
		OW_MutexLock l(m_guard);
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
		env->getLogger()->logDebug("OW_IndicationProviderTest3::enumInstances");
		// m_insts could be accessed from multiple threads
		OW_MutexLock l(m_guard);
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
		(void)className; (void)cimClass;
		env->getLogger()->logDebug("OW_IndicationProviderTest3::enumInstanceNames");
		// m_insts could be accessed from multiple threads
		OW_MutexLock l(m_guard);
		for (size_t i = 0; i < m_insts.size(); ++i)
		{
			result.handle(OW_CIMObjectPath(ns, m_insts[i]));
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

	// Polled provider methods
	virtual OW_Int32 poll(const OW_ProviderEnvironmentIFCRef& env)
	{
		OW_MutexLock l(m_guard);
		// return a 1 second interval if we need to be polled, otherwise return 0 which means we won't be polled.
		if (m_creationFilterCount + m_modificationFilterCount + m_deletionFilterCount > 0)
		{
			updateInstancesAndSendIndications(env);
			return 1;
		}
		else
		{
			return 0;
		}
	}

	virtual OW_Int32 getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env)
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest3::getInitialPollingInterval");
		OW_MutexLock l(m_guard);
		// return a 1 second interval if we need to be polled, otherwise return 0 which means we won't be polled.
		if (m_creationFilterCount + m_modificationFilterCount + m_deletionFilterCount > 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

private:
	void updateInstancesAndSendIndications(const OW_ProviderEnvironmentIFCRef& env)
	{
		OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();

		// m_insts could be accessed from multiple threads
		OW_MutexLock l(m_guard);
		if (m_insts.size() == 5)
		{
			if (m_deletionFilterCount > 0)
			{
				// send out CIM_InstDeletion indications
				for (size_t i = 0; i < m_insts.size(); ++i)
				{
					OW_CIMInstance expInst;
					expInst.setClassName("CIM_InstDeletion");
					expInst.setProperty("SourceInstance", OW_CIMValue(m_insts[i]));
					hdl->exportIndication(expInst, "root/testsuite");
				}
			}
			m_insts.clear();
		}
		else
		{
			// add an instance
			OW_CIMInstance iToAdd(m_theClass.newInstance());
			iToAdd.setProperty("SystemCreationClassName", OW_CIMValue("CIM_System"));
			iToAdd.setProperty("SystemName", OW_CIMValue("localhost"));
			iToAdd.setProperty("CreationClassName", OW_CIMValue("OW_IndicationProviderTest3"));
			iToAdd.setProperty("DeviceID", OW_CIMValue(OW_String(m_insts.size())));
			// PowerOnHours is our property that will be modified
			iToAdd.setProperty("PowerOnHours", OW_CIMValue(OW_UInt64(m_insts.size())));
			if (m_creationFilterCount > 0)
			{
				// send out CIM_InstCreation indications
				OW_CIMInstance expInst;
				expInst.setClassName("CIM_InstCreation");
				expInst.setProperty("SourceInstance", OW_CIMValue(iToAdd));
				hdl->exportIndication(expInst, "root/testsuite");
			}
			m_insts.push_back(iToAdd);

			// now modify the first instance's PowerOnHours property
			OW_CIMInstance prevInst = m_insts[0];
			OW_UInt64 oldPowerOnHours = prevInst.getPropertyT("PowerOnHours").getValueT().toUInt64();
			m_insts[0].setProperty("PowerOnHours", OW_CIMValue(OW_UInt64(oldPowerOnHours + 1)));

			if (m_modificationFilterCount > 0)
			{
				// send out CIM_InstModification indications
				OW_CIMInstance expInst;
				expInst.setClassName("CIM_InstModification");
				expInst.setProperty("PreviousInstance", OW_CIMValue(prevInst));
				expInst.setProperty("SourceInstance", OW_CIMValue(m_insts[0]));
				hdl->exportIndication(expInst, "root/testsuite");
			}
		}
	}

	OW_CIMInstanceArray m_insts;

	OW_Mutex m_guard;
	OW_CIMClass m_theClass;

	int m_creationFilterCount;
	int m_modificationFilterCount;
	int m_deletionFilterCount;

	void addCreationFilter()
	{
		OW_MutexLock l(m_guard);
		++m_creationFilterCount;
	}

	void addModificationFilter()
	{
		OW_MutexLock l(m_guard);
		++m_modificationFilterCount;
	}

	void addDeletionFilter()
	{
		OW_MutexLock l(m_guard);
		++m_deletionFilterCount;
	}

	void removeCreationFilter()
	{
		OW_MutexLock l(m_guard);
		--m_creationFilterCount;
	}

	void removeModificationFilter()
	{
		OW_MutexLock l(m_guard);
		--m_modificationFilterCount;
	}

	void removeDeletionFilter()
	{
		OW_MutexLock l(m_guard);
		--m_deletionFilterCount;
	}
};


} // end anonymous namespace


OW_PROVIDERFACTORY(OW_IndicationProviderTest3, indicationtest3)


