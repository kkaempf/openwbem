/*******************************************************************************
* Copyright (C) 2001-2004 Novell, Inc. All rights reserved.
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
*  - Neither the name of Novell, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Novell, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 */

#include "cmpisrv.h"
#include "blocxx/Logger.hpp"
#include "blocxx/String.hpp"
#include "blocxx/Thread.hpp"
#include "blocxx/NonRecursiveMutex.hpp"
#include "blocxx/NonRecursiveMutexLock.hpp"
#include "blocxx/MemoryBarrier.hpp"
#include <cstring>


using namespace OW_NAMESPACE;
using namespace blocxx;

namespace
{
	const blocxx::String COMPONENT_NAME("ow.provider.cmpi.ifc");
}

#define CM_LOGGER() \
Logger(COMPONENT_NAME)


// Factory section
static char*  mbExtResolveFileName(const char* libName)
{
	try
	{
		String slibName(libName);
		String fileName;
	#if defined(OW_WIN32)
		fileName = slibName + String(".dll");
	#elif defined(OW_HPUX)
		fileName = String("lib") + slibName + String(".sl");
	#elif defined(OW_DARWIN)
		fileName = String("lib") + slibName + String(".dylib");
	#else
		fileName = String("lib") + slibName + String(".so");
	#endif

		return strdup(fileName.c_str());
	}
	catch(...)
	{
		return 0;
	}
}

class CMPIThread : public Thread
{
public:
	CMPIThread(CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *start)(void *),
			   void *param)
		: m_start(start)
		, m_param(param)
		, m_rval(0)
	{
	}
	CMPI_THREAD_RETURN getReturnValue() const { return m_rval; }

protected:
	virtual Int32 run()
	{
		m_rval = m_start(m_param);
		return 0;
	}

private:
	CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *m_start)(void*);
	void* m_param;
	CMPI_THREAD_RETURN m_rval;
};

CMPI_THREAD_TYPE mbExtNewThread (CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *start)(void *),
								 void *parm, int detached)
{
	(void)detached; // detached arg not supported.
	CMPIThread* newThread = new CMPIThread(start, parm);
	newThread->start();
	CMPI_THREAD_TYPE rval = (CMPI_THREAD_TYPE)newThread;
	return rval;
}

int mbExtJoinThread (CMPI_THREAD_TYPE thread, CMPI_THREAD_RETURN *retval )
{
	try
	{
		CMPIThread* theThread = (CMPIThread*)thread;
		theThread->join();
		(*retval) = theThread->getReturnValue();
		delete theThread;
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}

int mbExtExitThread (CMPI_THREAD_RETURN return_code)
{
	return -1;
}

int mbExtCancelThread (CMPI_THREAD_TYPE thread)
{
	try
	{
		CMPIThread* theThread = (CMPIThread*)thread;
		theThread->cancel();
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}

int mbExtThreadSleep (CMPIUint32 msec)
{
	try
	{
		Thread::sleep(msec);
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}

namespace
{
	NonRecursiveMutex onceGuard;
}

int mbExtThreadOnce (int *once, void (*init)(void))
{
	try
	{
		readWriteMemoryBarrier();
		if (*once == 0)
		{
			NonRecursiveMutexLock lock(onceGuard);
			if (*once == 0)
			{
				init();
				*once = 1;
			}
		}
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}

int mbExtCreateThreadKey (CMPI_THREAD_KEY_TYPE *key, void (*cleanup)(void*))
{
	return -1;
}

int mbExtDestroyThreadKey (CMPI_THREAD_KEY_TYPE key)
{
	return -1;
}

void *mbExtGetThreadSpecific (CMPI_THREAD_KEY_TYPE key)
{
	return 0;
}

int mbExtSetThreadSpecific (CMPI_THREAD_KEY_TYPE key, void * value)
{
	return -1;
}

CMPI_MUTEX_TYPE mbExtNewMutex (int opt)
{
	(void) opt; // spec says none defined yet.
	Mutex* mux = new Mutex;
	CMPI_MUTEX_TYPE rval = (CMPI_MUTEX_TYPE)mux;
	return rval;
}

void mbExtDestroyMutex (CMPI_MUTEX_TYPE arg)
{
	Mutex* mux = (Mutex*)arg;
	delete mux;
}

void mbExtLockMutex (CMPI_MUTEX_TYPE arg)
{
	Mutex* mux = (Mutex*)arg;
	mux->acquire();
}

void mbExtUnlockMutex (CMPI_MUTEX_TYPE arg)
{
	Mutex* mux = (Mutex*)arg;
	mux->release();
}

CMPI_COND_TYPE mbExtNewCondition (int opt)
{
	Condition* cond = new Condition;
	CMPI_COND_TYPE rval = (CMPI_COND_TYPE)cond;
	return rval;
}

void mbExtDestroyCondition (CMPI_COND_TYPE arg)
{
	Condition* cond = (Condition*)arg;
	delete cond;
}

int mbExtCondWait (CMPI_COND_TYPE cond,
				   CMPI_MUTEX_TYPE mutex)
{
	/*
	try
	{
		Condition* lcond = (Condition*)cond;
		Mutex* mux = (Mutex*) mutex;
		NonRecursiveMutexLock lock(mux);
		lcond->wait(*mux);
	}
	catch(...)
	{
		return 1;
	}

*/
	return 0;
}

int mbExtTimedCondWait (CMPI_COND_TYPE cond,
						CMPI_MUTEX_TYPE mutex, struct timespec *wait)
{
	return 0;
}

int mbExtSignalCondition (CMPI_COND_TYPE cond)
{
	return 0;
}


static CMPIBrokerExtFT brokerExt_FT={
	CMPICurrentVersion,
	mbExtResolveFileName,
	mbExtNewThread,
	mbExtJoinThread,
	NULL,  // mbExtExitThread,
	mbExtCancelThread,
	mbExtThreadSleep,
	mbExtThreadOnce,
	NULL, // mbExtCreateThreadKey,
	NULL, // mbExtDestroyThreadKey,
	NULL, // mbExtGetThreadSpecific,
	NULL, // mbExtSetThreadSpecific,
	mbExtNewMutex,
	mbExtDestroyMutex,
	mbExtLockMutex,
	mbExtUnlockMutex,
	NULL, // mbExtNewCondition,
	NULL, // mbExtDestroyCondition,
	NULL, // mbExtCondWait,
	NULL, // mbExtTimedCondWait,
	NULL, // mbExtSignalCondition
};

CMPIBrokerExtFT *CMPI_BrokerExt_Ftab=&brokerExt_FT;



