/*******************************************************************************
* Copyright (C) 2003-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_CIMClient.hpp"
#include "OW_CIMException.hpp"
#include "OW_URL.hpp"
#include "blocxx/ThreadPool.hpp"
#include "blocxx/Runnable.hpp"
#include "blocxx/Thread.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/NonRecursiveMutex.hpp"
#include "blocxx/NonRecursiveMutexLock.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/ThreadCounter.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMQualifierType.hpp"
#include "blocxx/Runnable.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <typeinfo>

/// @todo list:
// cover *all* read operations
// GetClass - Done
// GetInstance - Done
// EnumerateClasses - Done
// EnumerateClassNames - Done
// EnumerateInstances - Done
// EnumerateInstanceNames - Done
// Associators - Done
// AssociatorNames - Done
// AssociatorsClasses - Done
// References - Done
// ReferenceNames - Done
// ReferencesClasses - Done
// GetProperty
// GetQualifier - Done
// EnumerateQualifiers - Done

// possible modifying operations:
// DeleteClass
// DeleteInstance
// CreateClass
// CreateInstance
// ModifyClass
// ModifyInstance
// SetProperty
// SetQualifier
// DeleteQualifier
// ExecQuery - this will get a write-lock and serialize access to the cimom.

// As we're traversing instances, we need to do association traversal and
// check referential integrity.  When we find an association instance, we
// should be able to get the 2 references, and also call the assoc funcs
// and get the appropriate response back.




#define TEST_ASSERT(CON) if (!(CON)) throw OW_AssertionException(__FILE__, __LINE__, #CON)

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using namespace OpenWBEM;
using namespace WBEMFlags;
using namespace blocxx;


//////////////////////////////////////////////////////////////////////////////
enum RepeatMode
{
	ONCE,
	FOREVER
};

RepeatMode repeatMode = ONCE;

//////////////////////////////////////////////////////////////////
enum ReportMode
{
	ABORT,
	COLLECT,
	IGNORE
};

ReportMode reportMode;

//////////////////////////////////////////////////////////////////
// Error reporting globals
NonRecursiveMutex errorGuard;
StringArray errors;

void handleErrorMessage(const String& errorMsg)
{
	NonRecursiveMutexLock l(errorGuard);
	switch (reportMode)
	{
		case ABORT:
			cerr << errorMsg << endl;
			exit(1); // just bail and kill any threads that may be running.
			break;
		case COLLECT:
			errors.push_back(errorMsg);
			break;
		case IGNORE:
			cerr << errorMsg << endl;
			break;
	}

}

void reportError(const String& operation, const String& param="")
{
	String errorMsg = Format("%1(%2)", operation, param);
	handleErrorMessage(errorMsg);
}

void reportError(const Exception& e, const String& operation, const String& param="")
{
	String errorMsg = Format("%1(%2): %3", operation, param, e);
	handleErrorMessage(errorMsg);
}

void reportError(const std::exception& e, const String& operation, const String& param="")
{
	String errorMsg = Format("%1(%2): std::exception(%3):%4", operation, param, typeid(e).name(), e.what());
	handleErrorMessage(errorMsg);
}

void reportError(const Exception& e)
{
	String errorMsg = Format("%1", e);
	handleErrorMessage(errorMsg);
}

int checkAndReportErrors()
{
	std::copy(errors.begin(), errors.end(), std::ostream_iterator<String>(std::cout, "\n"));
	return errors.size();
}

////////////////////////////////////////////////////////////////
// Threading globals
ThreadPool* pool = 0;

enum ThreadMode
{
	SINGLE,
	POOL
};

ThreadMode threadMode = SINGLE;

String url;

void doWork(const RunnableRef& work)
{
	switch (threadMode)
	{
		case SINGLE:
		{
			try
			{
				work->run();
			}
			catch (const Exception& e)
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
	}
}

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> <single|pool=<size> > <once|forever> <abort|collect|ignore>\n";
	cerr << " single - runs all operations single-threaded serially.  This may fail if the http connection timeout on the server is to low.\n";
	cerr << " pool - creates a thread pool of size number of threads.  Each request is run on it's own thread.\n";
	cerr << " once - the test will run once\n";
	cerr << " forever - the test will continue to loop forever\n";
	cerr << " abort - the test ends once an error has occurred\n";
	cerr << " collect - all errors will be collected and reported when the test finishes.  If you use this with \"forever\", you won't see any errors\n";
	cerr << " ignore - errors are printed to stderr, but won't cause the test to end\n";

	cerr << endl;
}

//////////////////////////////////////////////////////////////////////////////
class ErrorReportRunnable : public Runnable
{
public:
	ErrorReportRunnable(const String& operation , const String& param)
		: m_operation(operation)
		, m_param(param)
	{}

	void run()
	{
		try
		{
			doRun();
		}
		catch (const Exception& e)
		{
			reportError(e, m_operation, m_param);
		}
		catch (const std::exception& e)
		{
			reportError(e, m_operation, m_param);
		}
		catch (...)
		{
			reportError(m_operation, m_param);
			throw;
		}
	}

protected:
	virtual void doRun() = 0;

	void setOperation(const String& operation) { m_operation = operation; }
	void setParam(const String& param) { m_param = param; }

private:
	String m_operation;
	String m_param;

};




//////////////////////////////////////////////////////////////////////////////
class AssociatorsChecker : public ErrorReportRunnable
{
public:
	AssociatorsChecker(const String& ns, const CIMObjectPath& inst, const CIMObjectPath& other, const String& assocName)
		: ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_other(other)
		, m_assocName(assocName)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMInstanceEnumeration insts = rch.associatorsE(m_inst, m_assocName, m_other.getClassName());
		// now make sure that assoc is in insts
		while (insts.hasMoreElements())
		{
			CIMInstance i(insts.nextElement());
			if (CIMObjectPath(m_ns, i) == m_other)
				return;
		}
		// didn't find it
		reportError("AssociatorsChecker", m_inst.toString() + " didn't return " + m_other.toString());
	}
private:
	String m_ns;
	CIMObjectPath m_inst;
	CIMObjectPath m_other;
	String m_assocName;
};

//////////////////////////////////////////////////////////////////////////////
class AssociatorNamesChecker : public ErrorReportRunnable
{
public:
	AssociatorNamesChecker(const String& ns, const CIMObjectPath& inst, const CIMObjectPath& other, const String& assocName)
		: ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_other(other)
		, m_assocName(assocName)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMObjectPathEnumeration cops = rch.associatorNamesE(m_inst, m_assocName, m_other.getClassName());
		// now make sure that assoc is in cops
		while (cops.hasMoreElements())
		{
			CIMObjectPath cop(cops.nextElement());
			if (cop == m_other)
				return;
		}
		// didn't find it
		reportError("AssociatorNamesChecker", m_inst.toString() + " didn't return " + m_other.toString());
	}
private:
	String m_ns;
	CIMObjectPath m_inst;
	CIMObjectPath m_other;
	String m_assocName;
};

//////////////////////////////////////////////////////////////////////////////
class ReferencesChecker : public ErrorReportRunnable
{
public:
	ReferencesChecker(const String& ns, const CIMObjectPath& inst, const CIMObjectPath& assoc)
		: ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_assoc(assoc)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMInstanceEnumeration refs = rch.referencesE(m_inst, m_assoc.getClassName());
		// now make sure that assoc is in refs
		while (refs.hasMoreElements())
		{
			CIMInstance i(refs.nextElement());
			if (CIMObjectPath(m_ns, i) == m_assoc)
				return;
		}
		// didn't find it
		reportError("ReferencesChecker", m_inst.toString() + " didn't return " + m_assoc.toString());
	}
private:
	String m_ns;
	CIMObjectPath m_inst;
	CIMObjectPath m_assoc;
};

//////////////////////////////////////////////////////////////////////////////
class ReferenceNamesChecker : public ErrorReportRunnable
{
public:
	ReferenceNamesChecker(const String& ns, const CIMObjectPath& inst, const CIMObjectPath& assoc)
		: ErrorReportRunnable("references", inst.toString())
		, m_ns(ns)
		, m_inst(inst)
		, m_assoc(assoc)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMObjectPathEnumeration refs = rch.referenceNamesE(m_inst, m_assoc.getClassName());
		// now make sure that assoc is in refs
		while (refs.hasMoreElements())
		{
			CIMObjectPath op(refs.nextElement());

			if (op == m_assoc)
				return;
		}
		// didn't find it
		reportError("ReferenceNamesChecker", m_inst.toString() + " didn't return " + m_assoc.toString());
	}
private:
	String m_ns;
	CIMObjectPath m_inst;
	CIMObjectPath m_assoc;
};

//////////////////////////////////////////////////////////////////////////////
class NoopObjectPathResultHandler : public CIMObjectPathResultHandlerIFC
{
public:
	NoopObjectPathResultHandler(const String& ns)
		: m_ns(ns)
	{}

	void doHandle(const CIMObjectPath& cop)
	{
	}
private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class NoopInstanceResultHandler : public CIMInstanceResultHandlerIFC
{
public:
	NoopInstanceResultHandler(const String& ns)
		: m_ns(ns)
	{}

	void doHandle(const CIMInstance& inst)
	{
	}
private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceAssociatorNames : public ErrorReportRunnable
{
public:
	InstanceAssociatorNames(const String& ns, const CIMObjectPath& name)
		: ErrorReportRunnable("associatorNames", ns + ":" + name.toString())
		, m_ns(ns)
		, m_name(name)
		{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		NoopObjectPathResultHandler objectPathResultHandler(m_ns);
		rch.associatorNames(m_name, objectPathResultHandler);
	}

private:
	String m_ns;
	CIMObjectPath m_name;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceReferenceNames : public ErrorReportRunnable
{
public:
	InstanceReferenceNames(const String& ns, const CIMObjectPath& name)
		: ErrorReportRunnable("referenceNames", ns + ":" + name.toString())
		, m_ns(ns)
		, m_name(name)
		{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		NoopObjectPathResultHandler objectPathResultHandler(m_ns);
		rch.referenceNames(m_name, objectPathResultHandler);
	}

private:
	String m_ns;
	CIMObjectPath m_name;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceAssociators : public ErrorReportRunnable
{
public:
	InstanceAssociators(const String& ns, const CIMObjectPath& name)
		: ErrorReportRunnable("associators", ns + ":" + name.toString())
		, m_ns(ns)
		, m_name(name)
		{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		NoopInstanceResultHandler instanceResultHandler(m_ns);
		rch.associators(m_name, instanceResultHandler);
	}

private:
	String m_ns;
	CIMObjectPath m_name;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceReferences : public ErrorReportRunnable
{
public:
	InstanceReferences(const String& ns, const CIMObjectPath& name)
		: ErrorReportRunnable("references", ns + ":" + name.toString())
		, m_ns(ns)
		, m_name(name)
		{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		NoopInstanceResultHandler instanceResultHandler(m_ns);
		rch.references(m_name, instanceResultHandler);
	}

private:
	String m_ns;
	CIMObjectPath m_name;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceGetter : public ErrorReportRunnable
{
public:
	InstanceGetter(const String& ns, const CIMObjectPath& instPath)
		: ErrorReportRunnable("getInstance", instPath.toString())
		, m_ns(ns)
		, m_instPath(instPath)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMInstance inst2 = rch.getInstance(m_instPath);
		// look for REF properties and start a thread to get them.
		CIMPropertyArray props = inst2.getProperties();
		for (size_t i = 0; i < props.size(); ++i)
		{
			CIMProperty& p = props[i];
			if (p.getDataType().getType() == CIMDataType::REFERENCE)
			{
				CIMObjectPath curPath = p.getValueT().toCIMObjectPath();

				// make sure the ref points to a valid instance
				RunnableRef worker1(new InstanceGetter(m_ns, curPath));
				doWork(worker1);

				// Check 2-way referential integrity of Reference[Name]s.  call reference* on the path and make sure it returns m_instPath
				RunnableRef worker2(new ReferencesChecker(m_ns, curPath, m_instPath));
				doWork(worker2);
				RunnableRef worker3(new ReferenceNamesChecker(m_ns, curPath, m_instPath));
				doWork(worker3);

				// Check 2-way referencial integrity of Associator[Name]s.  call associator[Name]s on the path and make sure it returns all the other REF properties in this instance.
				size_t j;
				for (j = 0; j < props.size(); ++j)
				{
					if (j == i)
						continue;
					if (props[i].getDataType().getType() == CIMDataType::REFERENCE)
						break;
				}
				if (j != props.size())
				{
					CIMObjectPath otherPath = props[j].getValueT().toCIMObjectPath();
					RunnableRef worker4(new AssociatorsChecker(m_ns, curPath, otherPath, m_instPath.getClassName()));
					doWork(worker4);
					RunnableRef worker5(new AssociatorNamesChecker(m_ns, curPath, otherPath, m_instPath.getClassName()));
					doWork(worker5);
				}
			}
		}

		RunnableRef worker6(new InstanceAssociators(m_ns, m_instPath));
		doWork(worker6);

		RunnableRef worker7(new InstanceReferences(m_ns, m_instPath));
		doWork(worker7);

		RunnableRef worker8(new InstanceAssociatorNames(m_ns, m_instPath));
		doWork(worker8);

		RunnableRef worker9(new InstanceReferenceNames(m_ns, m_instPath));
		doWork(worker9);
	}

private:
	String m_ns;
	CIMObjectPath m_instPath;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceResultHandler : public CIMInstanceResultHandlerIFC
{
public:
	InstanceResultHandler(const String& ns)
		: m_ns(ns)
	{}

	void doHandle(const CIMInstance& inst)
	{
		RunnableRef worker(new InstanceGetter(m_ns, CIMObjectPath(m_ns, inst)));
		doWork(worker);
	}
private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class InstanceEnumerator : public ErrorReportRunnable
{
public:
	InstanceEnumerator(const String& ns, const String& name)
		: ErrorReportRunnable("enumInstances", ns + ":" + name)
		, m_ns(ns)
		, m_name(name)
		{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		InstanceResultHandler instanceResultHandler(m_ns);
		rch.enumInstances(m_name, instanceResultHandler);
	}

private:
	String m_ns;
	String m_name;
};

//////////////////////////////////////////////////////////////////////////////
class AssociatorsClassesChecker : public ErrorReportRunnable
{
public:
	AssociatorsClassesChecker(const String& ns, const String& clsName)
		: ErrorReportRunnable("associatorsClasses", clsName)
		, m_ns(ns)
		, m_clsName(clsName)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMClassEnumeration e = rch.associatorsClassesE(CIMObjectPath(m_clsName));
		while (e.hasMoreElements())
		{
			CIMClass c = e.nextElement();
/* don't do this for now, it's too slow
			CIMClassEnumeration e2 = rch.associatorsClassesE(CIMObjectPath(c.getName()));
			bool foundOrigClass = false;
			while (e2.hasMoreElements())
			{
				CIMClass c2 = e2.nextElement();
				if (c2.getName() == m_clsName)
				{
					foundOrigClass = true;
					break;
				}
			}
			if (!foundOrigClass)
			{
				reportError("associatorsClasses referential integrity broken", m_clsName);
			}
*/
		}
	}

private:
	String m_ns;
	String m_clsName;
};

//////////////////////////////////////////////////////////////////////////////
class ReferencesClassesChecker : public ErrorReportRunnable
{
public:
	ReferencesClassesChecker(const String& ns, const String& clsName)
		: ErrorReportRunnable("referencesClasses", clsName)
		, m_ns(ns)
		, m_clsName(clsName)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMClassEnumeration e = rch.referencesClassesE(CIMObjectPath(m_clsName));
/* disabled until it can be fixed, and it takes a LONG time.
		while (e.hasMoreElements())
		{
			CIMClass c = e.nextElement();
			bool foundReferenceToOrigClass = false;
			CIMPropertyArray props = c.getAllProperties();
			for (size_t i = 0; i < props.size(); ++i)
			{
				CIMProperty& p = props[i];
				if (p.getDataType().getType() == CIMDataType::REFERENCE)
				{
					// Fix this: it needs to also check for base class names.
					if (p.getDataType().getRefClassName() == m_clsName)
					{
						foundReferenceToOrigClass = true;
						break;
					}
				}
			}
			if (!foundReferenceToOrigClass)
			{
				reportError("referencesClasses referential integrity broken", m_clsName);
			}
		}
*/
	}

private:
	String m_ns;
	String m_clsName;
};

//////////////////////////////////////////////////////////////////////////////
class ClassGetter : public ErrorReportRunnable
{
public:
	ClassGetter(const String& ns, const String& clsName)
		: ErrorReportRunnable("getClass", clsName)
		, m_ns(ns)
		, m_clsName(clsName)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMClass c = rch.getClass(m_clsName);
	}

private:
	String m_ns;
	String m_clsName;
};

//////////////////////////////////////////////////////////////////////////////
class ClassResultHandler : public CIMClassResultHandlerIFC, public StringResultHandlerIFC
{
public:
	ClassResultHandler(const String& ns)
		: m_ns(ns)
	{}

	void doHandle(const CIMClass& cls)
	{
	}

	void doHandle(const String& clsName)
	{
		RunnableRef worker1(new InstanceEnumerator(m_ns, clsName));
		doWork(worker1);

		RunnableRef worker2(new ClassGetter(m_ns, clsName));
		doWork(worker2);

		RunnableRef worker3(new AssociatorsClassesChecker(m_ns, clsName));
		doWork(worker3);

		RunnableRef worker4(new ReferencesClassesChecker(m_ns, clsName));
		doWork(worker4);
	}

private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class ClassEnumerator : public ErrorReportRunnable
{
public:
	ClassEnumerator(const String& ns)
		: ErrorReportRunnable("enumClass", ns)
		, m_ns(ns) {}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		ClassResultHandler classResultHandler(m_ns);
		rch.enumClass("", classResultHandler, E_DEEP);
	}

private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class ClassNameEnumerator : public ErrorReportRunnable
{
public:
	ClassNameEnumerator(const String& ns)
		: ErrorReportRunnable("enumClassNames", ns)
		, m_ns(ns) {}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		ClassResultHandler classNameResultHandler(m_ns);
		rch.enumClassNames("", classNameResultHandler);
	}

private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class QualifierTypeGetter : public ErrorReportRunnable
{
public:
	QualifierTypeGetter(const String& ns, const String& qtName)
		: ErrorReportRunnable("getQualifierType", qtName)
		, m_ns(ns)
		, m_qtName(qtName)
	{}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		CIMQualifierType c = rch.getQualifierType(m_qtName);
	}

private:
	String m_ns;
	String m_qtName;
};

//////////////////////////////////////////////////////////////////////////////
class QualifierTypeResultHandler : public CIMQualifierTypeResultHandlerIFC
{
public:
	QualifierTypeResultHandler(const String& ns)
		: m_ns(ns)
	{}

	void doHandle(const CIMQualifierType& qt)
	{
		RunnableRef worker(new QualifierTypeGetter(m_ns, qt.getName()));
		doWork(worker);
	}


private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class QualifierEnumerator : public ErrorReportRunnable
{
public:
	QualifierEnumerator(const String& ns)
		: ErrorReportRunnable("enumQualifierTypes", ns)
		, m_ns(ns) {}

	void doRun()
	{
		CIMClient rch(url, m_ns);
		QualifierTypeResultHandler qualifierTypeResultHandler(m_ns);
		rch.enumQualifierTypes(qualifierTypeResultHandler);
	}

private:
	String m_ns;
};

//////////////////////////////////////////////////////////////////////////////
class NamespaceResultHandler : public StringResultHandlerIFC
{
	void doHandle(const String& ns)
	{
		RunnableRef worker1(new ClassEnumerator(ns));
		doWork(worker1);

		RunnableRef worker2(new ClassNameEnumerator(ns));
		doWork(worker2);

		RunnableRef worker3(new QualifierEnumerator(ns));
		doWork(worker3);
	}
};



//////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		if (argc < 5)
		{
			usage(argv[0]);
			return 1;
		}

		String threadModeArg(argv[2]);
		if (threadModeArg == "single")
		{
			threadMode = SINGLE;
		}
		else if (threadModeArg.startsWith("pool"))
		{
			UInt32 poolSize = threadModeArg.substring(threadModeArg.indexOf('=') + 1).toUInt32();
			pool = new ThreadPool(ThreadPool::FIXED_SIZE, poolSize, 0); // unlimited queue since we don't want to be too restrictive and cause a deadlock
			threadMode = POOL;
		}
		else
		{
			usage(argv[0]);
			return 1;
		}

		String repeatModeArg(argv[3]);
		if (repeatModeArg == "once")
		{
			repeatMode = ONCE;
		}
		else if (repeatModeArg == "forever")
		{
			repeatMode = FOREVER;
		}
		else
		{
			usage(argv[0]);
			return 1;
		}

		String reportModeArg(argv[4]);
		if (reportModeArg == "abort")
		{
			reportMode = ABORT;
		}
		else if (reportModeArg == "collect")
		{
			reportMode = COLLECT;
		}
		else if (reportModeArg == "ignore")
		{
			reportMode = IGNORE;
		}
		else
		{
			usage(argv[0]);
			return 1;
		}


		url = argv[1];

		CIMClient rch(url, "root");

		// start this all off by enumerating namespaces
		NamespaceResultHandler namespaceResultHandler;
		rch.enumCIM_Namespace(namespaceResultHandler);

		if (repeatMode == FOREVER)
		{
			while (true)
			{
				// Need to wait to avoid causing a deadlock
				// because all our threads are doing an
				// enumNameSpace and we can't launch any new
				// ones.
				switch (threadMode)
				{
					case SINGLE:
					break; // no need to delay anything.
					case POOL:
						pool->waitForEmptyQueue();
					break;
				}
				rch.enumCIM_Namespace(namespaceResultHandler);
			}
		}

		// wait for everything to finish
		switch (threadMode)
		{
			case SINGLE:
			break; // we're already done
			case POOL:
				// Give the threads a little time to start filling up the queue.
				ThreadImpl::sleep(10);
				// don't immediately shutdown, to give things a chance to run for a while.
				pool->waitForEmptyQueue();
				pool->shutdown();
			break;
		}


	}
	catch (Exception& e)
	{
		reportError(e, "enumNameSpace", url);
	}
	catch (...)
	{
		reportError("enumNameSpace", url);
	}

	return checkAndReportErrors() ? 1 : 0;
}


