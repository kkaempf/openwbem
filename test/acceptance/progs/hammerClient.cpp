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
#include "OW_CIMClient.hpp"
#include "OW_CIMException.hpp"
#include "OW_URL.hpp"
#include "OW_ThreadPool.hpp"
#include "OW_Runnable.hpp"
#include "OW_Thread.hpp"
#include "OW_Format.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "OW_Array.hpp"
#include "OW_ThreadCounter.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>

// TODO list:
// cover *all* read operations
//  wql queries
//  assoc*
//  reference*
//  enum*Names




#define TEST_ASSERT(CON) if(!(CON)) throw OW_AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

//////////////////////////////////////////////////////////////////
// Error reporting globals
OW_NonRecursiveMutex errorGuard;
OW_StringArray errors;

void reportError(const OW_String& operation, const OW_String& param="")
{
	OW_String errorMsg = format("%1(%2)", operation, param);
	OW_NonRecursiveMutexLock l(errorGuard);
	errors.push_back(errorMsg);
}

void reportError(const OW_Exception& e, const OW_String& operation, const OW_String& param="")
{
	OW_String errorMsg = format("%1(%2): %3", operation, param, e);
	OW_NonRecursiveMutexLock l(errorGuard);
	errors.push_back(errorMsg);
}

void reportError(const OW_Exception& e)
{
	OW_String errorMsg = format("%1", e);
	OW_NonRecursiveMutexLock l(errorGuard);
	errors.push_back(errorMsg);
}

int checkAndReportErrors()
{
	std::copy(errors.begin(), errors.end(), std::ostream_iterator<OW_String>(std::cout, "\n"));
	return errors.size();
}

////////////////////////////////////////////////////////////////
// Threading globals
OW_ThreadPool* pool = 0;

enum ThreadMode
{
	SINGLE,
	POOL,
	THREAD
};

ThreadMode threadMode = SINGLE;

OW_String url;

OW_ThreadCounterRef threadCount(new OW_ThreadCounter(400)); // more than 400 threads will break on Linux.

void doWork(const OW_RunnableRef& work)
{
	switch (threadMode)
	{
		case SINGLE:
		{
			try
			{
				work->run();
			}
			catch (const OW_Exception& e)
			{
				reportError(e, "doWork");
			}
			catch (...)
			{
				reportError("doWork");
			}
		}
		break;
		case POOL:
		{
			pool->addWork(work);
		}
		break;
		case THREAD:
		{
			threadCount->incThreadCount(600, 0); // 10 min timeout.
			OW_Thread::run(work, true, OW_ThreadDoneCallbackRef(new OW_ThreadCountDecrementer(threadCount)));
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> <single|pool=<size>|thread>" << endl;
}

//////////////////////////////////////////////////////////////////////////////
class OW_ErrorReportRunnable : public OW_Runnable
{
public:
	OW_ErrorReportRunnable(const OW_String& operation , const OW_String& param)
		: m_operation(operation)
		, m_param(param)
	{}

	void run()
	{
		try
		{
			doRun();
		}
		catch (const OW_Exception& e)
		{
			reportError(e, m_operation, m_param);
		}
		catch (...)
		{
			reportError(m_operation, m_param);
		}
	}

protected:
	virtual void doRun() = 0;

	void setOperation(const OW_String& operation) { m_operation = operation; }
	void setParam(const OW_String& param) { m_param = param; }

private:
	OW_String m_operation;
	OW_String m_param;

};




//////////////////////////////////////////////////////////////////////////////
class AssociatorsChecker : public OW_ErrorReportRunnable
{
public:
	AssociatorsChecker(const OW_String& ns, const OW_CIMObjectPath& inst, const OW_CIMObjectPath& other, const OW_String& assocName)
		: OW_ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_other(other)
		, m_assocName(assocName)
	{}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		OW_CIMInstanceEnumeration insts = rch.associatorsE(m_inst, m_assocName, m_other.getClassName());
		// now make sure that assoc is in insts
		while (insts.hasMoreElements())
		{
			OW_CIMInstance i(insts.nextElement());
			if (OW_CIMObjectPath(m_ns, i) == m_other)
				return;
		}
		// didn't find it
		reportError("AssociatorsChecker", m_inst.toString() + " didn't return " + m_other.toString());
	}
private:
	OW_String m_ns;
	OW_CIMObjectPath m_inst;
	OW_CIMObjectPath m_other;
	OW_String m_assocName;
};

//////////////////////////////////////////////////////////////////////////////
class AssociatorNamesChecker : public OW_ErrorReportRunnable
{
public:
	AssociatorNamesChecker(const OW_String& ns, const OW_CIMObjectPath& inst, const OW_CIMObjectPath& other, const OW_String& assocName)
		: OW_ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_other(other)
		, m_assocName(assocName)
	{}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		OW_CIMObjectPathEnumeration cops = rch.associatorNamesE(m_inst, m_assocName, m_other.getClassName());
		// now make sure that assoc is in cops
		while (cops.hasMoreElements())
		{
			OW_CIMObjectPath cop(cops.nextElement());
			if (cop == m_other)
				return;
		}
		// didn't find it
		reportError("AssociatorNamesChecker", m_inst.toString() + " didn't return " + m_other.toString());
	}
private:
	OW_String m_ns;
	OW_CIMObjectPath m_inst;
	OW_CIMObjectPath m_other;
	OW_String m_assocName;
};

//////////////////////////////////////////////////////////////////////////////
class ReferencesChecker : public OW_ErrorReportRunnable
{
public:
	ReferencesChecker(const OW_String& ns, const OW_CIMObjectPath& inst, const OW_CIMObjectPath& assoc)
		: OW_ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_assoc(assoc)
	{}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		OW_CIMInstanceEnumeration refs = rch.referencesE(m_inst, m_assoc.getClassName());
		// now make sure that assoc is in refs
		while (refs.hasMoreElements())
		{
			OW_CIMInstance i(refs.nextElement());
			if (OW_CIMObjectPath(m_ns, i) == m_assoc)
				return;
		}
		// didn't find it
		reportError("ReferencesChecker", m_inst.toString() + " didn't return " + m_assoc.toString());
	}
private:
	OW_String m_ns;
	OW_CIMObjectPath m_inst;
	OW_CIMObjectPath m_assoc;
};

//////////////////////////////////////////////////////////////////////////////
class ReferenceNamesChecker : public OW_ErrorReportRunnable
{
public:
	ReferenceNamesChecker(const OW_String& ns, const OW_CIMObjectPath& inst, const OW_CIMObjectPath& assoc)
		: OW_ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_assoc(assoc)
	{}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		OW_CIMObjectPathEnumeration refs = rch.referenceNamesE(m_inst, m_assoc.getClassName());
		// now make sure that assoc is in refs
		while (refs.hasMoreElements())
		{
			OW_CIMObjectPath op(refs.nextElement());
			
			if (op == m_assoc)
				return;
		}
		// didn't find it
		reportError("ReferenceNamesChecker", m_inst.toString() + " didn't return " + m_assoc.toString());
	}
private:
	OW_String m_ns;
	OW_CIMObjectPath m_inst;
	OW_CIMObjectPath m_assoc;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceGetter : public OW_ErrorReportRunnable
{
public:
	InstanceGetter(const OW_String& ns, const OW_CIMObjectPath& instPath)
		: OW_ErrorReportRunnable("getInstance", instPath.toString())
		, m_ns(ns)
		, m_instPath(instPath)
	{}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		OW_CIMInstance inst2 = rch.getInstance(m_instPath);
		// look for REF properties and start a thread to get them.
		OW_CIMPropertyArray props = inst2.getProperties();
		for (size_t i = 0; i < props.size(); ++i)
		{
			OW_CIMProperty& p = props[i];
			if (p.getDataType().getType() == OW_CIMDataType::REFERENCE)
			{
				OW_CIMObjectPath curPath = p.getValueT().toCIMObjectPath();

				// make sure the ref points to a valid instance
				OW_RunnableRef worker1(new InstanceGetter(m_ns, curPath));
				doWork(worker1);

				// Check 2-way referential integrity of Reference[Name]s.  call reference* on the path and make sure it returns m_instPath
				OW_RunnableRef worker2(new ReferencesChecker(m_ns, curPath, m_instPath));
				doWork(worker2);
				OW_RunnableRef worker3(new ReferenceNamesChecker(m_ns, curPath, m_instPath));
				doWork(worker3);

				// Check 2-way referencial integrity of Associator[Name]s.  call associator[Name]s on the path and make sure it returns all the other REF properties in this instance.
				size_t j;
				for (j = 0; j < props.size(); ++j)
				{
					if (j == i)
						continue;
					if (props[i].getDataType().getType() == OW_CIMDataType::REFERENCE)
						break;
				}
				if (j != props.size())
				{
					OW_CIMObjectPath otherPath = props[j].getValueT().toCIMObjectPath();
					OW_RunnableRef worker4(new AssociatorsChecker(m_ns, curPath, otherPath, m_instPath.getClassName()));
					doWork(worker4);
					OW_RunnableRef worker5(new AssociatorNamesChecker(m_ns, curPath, otherPath, m_instPath.getClassName()));
					doWork(worker5);
				}
			}
		}

		// TODO: call assoc* things on it.
	}

private:
	OW_String m_ns;
	OW_CIMObjectPath m_instPath;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceResultHandler : public OW_CIMInstanceResultHandlerIFC
{
public:
	InstanceResultHandler(const OW_String& ns)
		: m_ns(ns)
	{}

	void doHandle(const OW_CIMInstance& inst)
	{
		OW_RunnableRef worker(new InstanceGetter(m_ns, OW_CIMObjectPath(m_ns, inst)));
		doWork(worker);
	}
private:
	OW_String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceEnumerator : public OW_ErrorReportRunnable
{
public:
	InstanceEnumerator(const OW_String& ns, const OW_String& name)
		: OW_ErrorReportRunnable("enumInstances", ns + ":" + name)
		, m_ns(ns)
		, m_name(name)
		{}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		InstanceResultHandler instanceResultHandler(m_ns);
		rch.enumInstances(m_name, instanceResultHandler);
	}

private:
	OW_String m_ns;
	OW_String m_name;
};

//////////////////////////////////////////////////////////////////////////////
class ClassResultHandler : public OW_CIMClassResultHandlerIFC
{
public:
	ClassResultHandler(const OW_String& ns)
		: m_ns(ns)
	{}

	void doHandle(const OW_CIMClass& cls)
	{
		OW_RunnableRef worker(new InstanceEnumerator(m_ns, cls.getName()));
		doWork(worker);
		// TODO: call assoc* things on it.
	}
private:
	OW_String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class ClassEnumerator : public OW_ErrorReportRunnable
{
public:
	ClassEnumerator(const OW_String& ns)
		: OW_ErrorReportRunnable("enumClass", ns)
		, m_ns(ns) {}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		ClassResultHandler classResultHandler(m_ns);
		rch.enumClass("", classResultHandler, OW_CIMOMHandleIFC::DEEP);
	}

private:
	OW_String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class NamespaceResultHandler : public OW_StringResultHandlerIFC
{
	void doHandle(const OW_String& ns)
	{
		OW_RunnableRef worker(new ClassEnumerator(ns));
		doWork(worker);

		// TODO
		//ClassNameEnumerator worker(ns);
		//doWork(worker);
	}
};

	

// As we're traversing instances, we need to do association traversal and
// check referential integrity.  When we find an association instance, we
// should be able to get the 2 references, and also call the assoc funcs
// and get the appropriate response back.
//////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		if (argc < 3)
		{
			usage(argv[0]);
			return 1;
		}

		OW_String threadModeArg(argv[2]);
		if (threadModeArg == "single")
		{
			threadMode = SINGLE;
		}
		else if (threadModeArg.startsWith("pool"))
		{
			OW_UInt32 poolSize = threadModeArg.substring(threadModeArg.indexOf('=') + 1).toUInt32();
			pool = new OW_ThreadPool(poolSize, 1000); // large queue since we don't want to be too restrictive
			threadMode = POOL;
		}
		else if (threadModeArg == "thread")
		{
			threadMode = THREAD;
		}
			

		url = argv[1];

		OW_CIMClient rch(url, "root");
		
		// start this all off by enumerating namespaces
		NamespaceResultHandler namespaceResultHandler;
		rch.enumNameSpace(namespaceResultHandler);

		// wait for everything to finish
		switch (threadMode)
		{
			case SINGLE:
			break; // we're already done
			case POOL:
				pool->shutdown();
			break;
			case THREAD:
			{
				threadCount->waitForAll(60000, 0); // 1000 min timeout
			}
			break;
		}


	}
	catch (OW_Exception& e)
	{
		reportError(e, "enumNameSpace", url);
	}
	catch (...)
	{
		reportError("enumNameSpace", url);
	}
		
	return checkAndReportErrors() ? 1 : 0;
}


