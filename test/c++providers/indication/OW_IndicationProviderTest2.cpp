/*******************************************************************************
* Copyright (C) 2003 Caldera International, Inc All rights reserved.
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
#include "OW_Thread.hpp"
#include "OW_Mutex.hpp"
#include "OW_Condition.hpp"
#include "OW_WQLCompile.hpp"

#include <iostream>
#include <algorithm>

// anonymous namespace to prevent library symbol conflicts
namespace
{

// This is an example/test of a simple instance/indication provider.  It 
// returns zero from mustPoll.  The CIMOM will call the *Filter methods.
// We'll start up a separate thread to "watch" the instances we are 
// instrumenting.  Really, we'll just change them once a second, so this
// provider has something to do.  A real indication provider should only
// use this model if it can block and wait for some sort of notification
// that something has happened.  There is no need to create a new thread
// if polling is needed, just create a combination indication/polled provider
// see (OW_IndicationProviderTest3).  An indication provider with a separate
// thread is useful for waiting for external events which will notify the
// thread that something has happened.  How this notification happens is up
// to the provider.

// This example is bordering on the edge of complexity where it should be
// split up into a couple of source and header files...

class OW_IndicationProviderTest2;

class OW_TestProviderThread : public OW_Thread
{
public:
	// joinable, so we can wait for it to stop before the library gets 
	// unloaded.  NEVER start a detached thread from a provider.  As soon as
	// the provider library is unloaded, the CIMOM will crash if the thread
	// is still running.
	OW_TestProviderThread(const OW_CIMOMHandleIFCRef& hdl, OW_IndicationProviderTest2* pProv)
		: OW_Thread(true) // true means joinable.
		, m_shuttingDown(false)
		, m_creationFilterCount(0)
		, m_modificationFilterCount(0)
		, m_deletionFilterCount(0)
		, m_hdl(hdl)
		, m_pProv(pProv)
	{
	}

	void shutdown()
	{
		OW_MutexLock l(m_guard);
		m_shuttingDown = true;
		m_cond.notifyAll(); // wake up run() so it will exit.
	}

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

protected:
	virtual void run();

	OW_Mutex m_guard;
	OW_Condition m_cond;
	bool m_shuttingDown;
	int m_creationFilterCount;
	int m_modificationFilterCount;
	int m_deletionFilterCount;
	OW_CIMOMHandleIFCRef m_hdl;
	OW_IndicationProviderTest2* m_pProv;
};
	
class OW_IndicationProviderTest2 : public OW_CppIndicationProviderIFC, public OW_CppInstanceProviderIFC
{
public:
	OW_IndicationProviderTest2()
		: m_theClass(OW_CIMNULL)
	{
	}

	~OW_IndicationProviderTest2()
	{
		cleanup();
	}

	// Indication provider methods
	virtual int mustPoll(const OW_ProviderEnvironmentIFCRef &env, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&) 
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest2::mustPoll");
		// if this were a real provider, we should check that we can really understand the select statement.  If our thread can't generate the correct indications, then we
		// should return 1 here.
		return 0; // means don't poll enumInstances.
	}

	virtual void authorizeFilter(const OW_ProviderEnvironmentIFCRef &env, const OW_WQLSelectStatement &, const OW_String &, const OW_String&, const OW_StringArray&, const OW_String &) 
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest2::authorizeFilter");
		// This is called when someone creates a subscription for an indication we generate.
		// If we wanted to deny access and cause the subscription creation to fail, we would throw an OW_CIMException::ACCESS_DENIED exception here.  
	}
	
	virtual void activateFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& /*nameSpace*/,
		const OW_StringArray& classes, 
		bool firstActivation)
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest2::activateFilter");
		
		// eventType contains the name of the indication the listener subscribed to.
		// this will be one of the class names we indicated in getProviderInfo(OW_IndicationProviderInfo& info)
		// so for this provider (and most other life cycle indication providers), it's got to be one of:
		// CIM_InstCreation, CIM_InstModification, or CIM_InstDeletion
		if (eventType.equalsIgnoreCase("CIM_InstCreation"))
		{
			m_thread->addCreationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstModification"))
		{
			m_thread->addModificationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstDeletion"))
		{
			m_thread->addDeletionFilter();
		}
		else
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! eventType is incorrect!");
		}
		// classes should either be empty (meaning the filter didn't contain an ISA clause), or contain a OW_IndicationProviderTest2 classname
		// (this is the case if the filter contains "SourceInstance ISA OW_IndicationProviderTest2")
		if (!classes.empty() && std::find(classes.begin(), classes.end(), "OW_IndicationProviderTest2") != classes.end())
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! classPath is incorrect!");
		}

		OW_WQLCompile comp(filter);
		// do something with comp
		(void)comp;

		// start the thread now that someone is listening for our events.
		if (firstActivation)
		{
			env->getLogger()->logDebug("OW_IndicationProviderTest2::activateFilter starting helper thread");
			m_thread->start();
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
		env->getLogger()->logDebug("OW_IndicationProviderTest2::deActivateFilter");
		
		// eventType contains the name of the indication the listener subscribed to.
		// this will be one of the class names we indicated in getProviderInfo(OW_IndicationProviderInfo& info)
		// so for this provider (and most other life cycle indication providers), it's got to be one of:
		// CIM_InstCreation, CIM_InstModification, or CIM_InstDeletion
		if (eventType.equalsIgnoreCase("CIM_InstCreation"))
		{
			m_thread->removeCreationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstModification"))
		{
			m_thread->removeModificationFilter();
		}
		else if (eventType.equalsIgnoreCase("CIM_InstDeletion"))
		{
			m_thread->removeDeletionFilter();
		}
		else
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! eventType is incorrect!");
		}
		// classes should either be empty (meaning the filter didn't contain an ISA clause), or contain a OW_IndicationProviderTest2 classname
		// (this is the case if the filter contains "SourceInstance ISA OW_IndicationProviderTest2")
		if (!classes.empty() && std::find(classes.begin(), classes.end(), "OW_IndicationProviderTest2") != classes.end())
		{
			// this isn't really necessary in a normal provider, but since this is a test, we do it to make sure the indication server is working all right
			OW_THROWCIMMSG(OW_CIMException::FAILED, "BIG PROBLEM! classPath is incorrect!");
		}

		OW_WQLCompile comp(filter);
		// do something with comp
		(void)comp;
		
		// terminate the thread if no one is listening for our events.
		if (lastActivation)
		{
			env->getLogger()->logDebug("OW_IndicationProviderTest2::deActivateFilter stopping helper thread");
			m_thread->shutdown();
			m_thread->join();
			env->getLogger()->logDebug("OW_IndicationProviderTest2::deActivateFilter helper thread stopped");
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
	virtual void getProviderInfo(OW_IndicationProviderInfo& info) 
	{
		// all the life-cycle indications that may be generated by this provider
		info.addInstrumentedClass("CIM_InstCreation");
		info.addInstrumentedClass("CIM_InstModification");
		info.addInstrumentedClass("CIM_InstDeletion");
	}

	// These are the indication provider methods.
	virtual void getProviderInfo(OW_InstanceProviderInfo& info)
	{
		// The class we are instrumenting
		info.addInstrumentedClass("OW_IndicationProviderTest2");
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
		env->getLogger()->logDebug("OW_IndicationProviderTest2::getInstance");
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
		env->getLogger()->logDebug("OW_IndicationProviderTest2::enumInstances");
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
		(void)ns; (void)className; (void)cimClass;
		env->getLogger()->logDebug("OW_IndicationProviderTest2::enumInstanceNames");
		// m_insts could be accessed from multiple threads
		OW_MutexLock l(m_guard);
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

	virtual void initialize(const OW_ProviderEnvironmentIFCRef& env)
	{
		env->getLogger()->logDebug("OW_IndicationProviderTest2::initialize - creating the thread");
		m_thread = new OW_TestProviderThread(env->getCIMOMHandle(), this);
	}

	virtual void cleanup() 
	{
		using std::cout;
		using std::endl;
		// we've got to stop the thread we started
		cout << "OW_IndicationProviderTest2::cleanup" << endl;
		if (m_thread->isRunning())
		{
			cout << "OW_IndicationProviderTest2::cleanup - stopping thread" << endl;
			m_thread->shutdown();
			m_thread->join();
			cout << "OW_IndicationProviderTest2::cleanup - thread stopped" << endl;
		}
	}

	void updateInstancesAndSendIndications(const OW_CIMOMHandleIFCRef& hdl, int creat, int mod, int del)
	{
		if (!m_theClass)
		{
			m_theClass = hdl->getClass("root/testsuite", "OW_IndicationProviderTest2");
		}
		// m_insts could be accessed from multiple threads
		OW_MutexLock l(m_guard);
		if (m_insts.size() == 5)
		{
			if (del > 0)
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
			iToAdd.setProperty("CreationClassName", OW_CIMValue("OW_IndicationProviderTest2"));
			iToAdd.setProperty("DeviceID", OW_CIMValue(OW_String(m_insts.size())));
			// PowerOnHours is our property that will be modified
			iToAdd.setProperty("PowerOnHours", OW_CIMValue(OW_UInt64(m_insts.size())));
			if (creat > 0)
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

			if (mod > 0)
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

private:
	OW_CIMInstanceArray m_insts;

	// this is a reference so we can pass in a cimom handle to it's constructor.  We can't get a cimom handle in our constructor, so we have to wait until initialize is called.
	OW_Reference<OW_TestProviderThread> m_thread;
	OW_Mutex m_guard;
	OW_CIMClass m_theClass;
};


void OW_TestProviderThread::run()
{
	OW_MutexLock l(m_guard);
	while (!m_shuttingDown)
	{
		m_pProv->updateInstancesAndSendIndications(m_hdl, m_creationFilterCount, m_modificationFilterCount, m_deletionFilterCount);
		// wait one second.  If this were a real provider we would wait on some IPC mechanism.  This has to be done carefully so that we don't block forever.  The provider needs
		// to be able to stop the thread when the cimom shuts down or restarts.
		m_cond.timedWait(l, 1);
	}
}


} // end anonymous namespace


OW_PROVIDERFACTORY(OW_IndicationProviderTest2, indicationtest2)


