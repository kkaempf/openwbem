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

#include "OW_Thread.hpp"
#include "OW_Assertion.hpp"

#include <cstring>
#include <cstdio>

//////////////////////////////////////////////////////////////////////////////
class OW_RunnableThread : public OW_Thread
{
public:
	OW_RunnableThread(OW_RunnableRef theRunnable) :
		OW_Thread(false), m_runnable(theRunnable)
	{
		setSelfDelete(true);
	}

	virtual void run()
	{
		try
		{
			m_runnable->run();
		}
		catch(...)
		{
			// Ignore?
		}

	}

private:
	OW_RunnableRef m_runnable;
};



static OW_Thread_t zeroThread();
static OW_Thread_t NULLTHREAD = zeroThread();

//////////////////////////////////////////////////////////////////////
static inline OW_Bool
sameId(const OW_Thread_t& t1, const OW_Thread_t& t2)
{
	return OW_ThreadImpl::sameThreads(t1, t2);
}

//////////////////////////////////////////////////////////////////////////////
// Constructor
OW_Thread::OW_Thread(OW_Bool isjoinable) :
	m_id(NULLTHREAD), m_isJoinable(isjoinable), m_deleteSelf(false),
	m_isRunning(false), m_isStarting(false)
{
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
OW_Thread::~OW_Thread()
{
	// Should never have to do this
	// while(m_isRunning)
	//{
	//	OW_Thread::sleep(1);
	//}

	OW_ASSERT(m_isRunning == false);
	if(!sameId(m_id, NULLTHREAD))
	{
		OW_ThreadImpl::destroyThread(m_id);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Start the thread
void
OW_Thread::start(OW_Reference<OW_ThreadDoneCallback> cb) /*throw (OW_ThreadException)*/
{
	if(isRunning())
	{
		OW_THROW(OW_ThreadException,
			"OW_Thread::start - thread is already running");
	}

	if(!sameId(m_id, NULLTHREAD))
	{
		OW_THROW(OW_ThreadException,
			"OW_Thread::start - cannot start previously run thread");
	}

	m_isStarting = true;

	OW_UInt32 flgs = (m_isJoinable == true) ? OW_THREAD_FLG_JOINABLE : 0;

	// p will be delted by threadRunner
	OW_ThreadParam* p = new OW_ThreadParam;
	p->thread = this;
	p->cb = cb;
	if(OW_ThreadImpl::createThread(m_id, threadRunner, p, flgs) != 0)
	{
		OW_THROW(OW_Assertion, "OW_ThreadImpl::createThread failed");
	}

	while(!m_isRunning)
	{
		yield();
	}

	m_isStarting = false;
}

//////////////////////////////////////////////////////////////////////////////
// Wait for this object's thread execution (if any) to complete.
void
OW_Thread::join() /*throw (OW_ThreadException)*/
{
	if(!isJoinable())
	{
		OW_THROW(OW_ThreadException, "OW_Thread::join - thread is not joinable");
	}

	if(!sameId(m_id, NULLTHREAD))
	{
		// If thread is starting up, let it finish
		while(!m_isRunning && m_isStarting)
		{
			yield();
		}

		if(OW_ThreadImpl::joinThread(m_id) != 0)
		{
			OW_THROW(OW_ThreadException,
				"OW_Thread::join - OW_ThreadImpl::joinThread");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_Thread::run(OW_RunnableRef theRunnable, OW_Bool separateThread, OW_Reference<OW_ThreadDoneCallback> cb)
{
	if(separateThread)
	{
		OW_RunnableThread* prt = new OW_RunnableThread(theRunnable);
		prt->start(cb);
	}
	else
	{
		try
		{
			theRunnable->run();
		}
		catch(...)
		{
			// Ignore?
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
// Method used for starting threads
void*
OW_Thread::threadRunner(void* paramPtr)
{
	OW_Thread_t theThreadID;
	try
	{
		// scope is important so destructors will run before the thread is clobbered by exitThread
		OW_ASSERT(paramPtr != NULL);

		OW_ThreadParam* pParam = (OW_ThreadParam*)paramPtr;
		OW_Thread* pTheThread = pParam->thread;
		theThreadID = pTheThread->m_id;
		pTheThread->m_isRunning = true;
		OW_Reference<OW_ThreadDoneCallback> cb = pParam->cb;

		while(pTheThread->m_isStarting)
		{
			yield();
		}

		pTheThread->run();

		pTheThread->m_isRunning = pTheThread->m_isStarting = false;

		if(pTheThread->getSelfDelete() == true)
		{
			delete pTheThread;
		}

		delete pParam;

		if (cb)
		{
			cb->notifyThreadDone(pTheThread);
		}
	}
	catch(OW_Exception& ex)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Exception: " << ex.type() << " caught in OW_Thread class\n";
		std::cerr << ex << std::endl;
#endif
	}
	catch(...)
	{
#ifdef OW_DEBUG		
		std::cerr << "!!! Unknown Exception caught in OW_Thread class" << std::endl;
#endif
	}

	// end the thread.  exitThread never returns.
	OW_ThreadImpl::exitThread(theThreadID);
	return NULL;
}

//////////////////////////////////////////////////////////////////////
static OW_Thread_t
zeroThread()
{
	OW_Thread_t zthr;
	::memset(&zthr, 0, sizeof(zthr));
	return zthr;
}

