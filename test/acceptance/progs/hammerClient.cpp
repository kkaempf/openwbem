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

#include <iostream>
#include <iterator>
#include <algorithm>

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

class ClassResultHandler : public OW_CIMClassResultHandlerIFC
{
	void doHandle(const OW_CIMClass& cls)
	{
		(void)cls;
		//InstanceEnumerator worker(cls);
		//doWork(worker);
	}
};

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

private:
	OW_String m_operation;
	OW_String m_param;

};

class ClassEnumerator : public OW_ErrorReportRunnable
{
public:
	ClassEnumerator(const OW_String& ns)
		: OW_ErrorReportRunnable("enumClass", ns)
		, m_ns(ns) {}

	void doRun()
	{
		OW_CIMClient rch(url, m_ns);
		ClassResultHandler classResultHandler;
		rch.enumClass("", classResultHandler, OW_CIMOMHandleIFC::DEEP);
	}

private:
	OW_String m_ns;
};

class NamespaceResultHandler : public OW_StringResultHandlerIFC
{
	void doHandle(const OW_String& ns)
	{
		OW_RunnableRef worker(new ClassEnumerator(ns));
		doWork(worker);

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
		if (argc < 2)
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
		reportError(e);
	}
	catch (...)
	{
		reportError("enumNameSpace", "");
	}
		
	return checkAndReportErrors();
}


