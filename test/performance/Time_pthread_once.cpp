/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "PerformanceCommon.hpp"
#ifdef OW_USE_PTHREAD
#include <pthread.h>
#endif
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_ThreadImpl.hpp"
#include "OW_ThreadOnce.hpp"

using namespace OpenWBEM;
using namespace std;


class NoOnceFoo
{
public:
	NoOnceFoo(int x) : m_x(x) {}

	static NoOnceFoo* instance()
	{
		if (!ptr)
		{
			onceInit();
		}
		return ptr;
	}

private:
	int m_x;

	static NoOnceFoo* volatile ptr;

	static void onceInit()
	{
		ptr = new NoOnceFoo(1);
	}
	
};

NoOnceFoo* volatile NoOnceFoo::ptr = 0;

#ifdef OW_USE_PTHREAD
class Foo
{
public:
	Foo(int x) : m_x(x) {}

	static Foo* instance()
	{
		pthread_once(&onceCtl, onceInit);
		return ptr;
	}

private:
	int m_x;

	static Foo* volatile ptr;
	static pthread_once_t onceCtl;

	static void onceInit()
	{
		ptr = new Foo(1);
	}
	
};

Foo* volatile Foo::ptr = 0;
pthread_once_t Foo::onceCtl = PTHREAD_ONCE_INIT;
#endif

class DCLPFoo
{
public:
	DCLPFoo(int x) : m_x(x) {}

	static DCLPFoo* instance()
	{
		ThreadImpl::memoryBarrier();
		if (ptr == 0)
		{
			MutexLock lock(s_guard);
			if (ptr == 0)
			{
				onceInit();
			}
		}

		return ptr;
	}

private:
	int m_x;

	static DCLPFoo* volatile ptr;
	static Mutex s_guard;

	static void onceInit()
	{
		ptr = new DCLPFoo(1);
	}
	
};

DCLPFoo* volatile DCLPFoo::ptr = 0;
Mutex DCLPFoo::s_guard;

//////////////////////////////////////////////////
class OWTOFoo
{
public:
	OWTOFoo(int x) : m_x(x) {}

	static OWTOFoo* instance()
	{
		callOnce(onceCtl, onceInit);
		return ptr;
	}

private:
	int m_x;

	static OWTOFoo* volatile ptr;
	static onceFlag onceCtl;

	static void onceInit()
	{
		ptr = new OWTOFoo(1);
	}
	
};

OWTOFoo* volatile OWTOFoo::ptr = 0;
onceFlag OWTOFoo::onceCtl = OW_ONCE_INIT;

/////////////////////////////////////////////////////////////////////////////
struct Nothing
{
	const char* name() const { return "Nothing"; }
	void operator()() const
	{
	}
};

struct getPtrNoOnce
{
	const char* name() const { return "getPtrNoOnce"; }
	void operator()() const
	{
		NoOnceFoo* ptr = NoOnceFoo::instance();
	}
};

#ifdef OW_USE_PTHREAD
struct getPtr
{
	const char* name() const { return "getPtr"; }
	void operator()() const
	{
		Foo* ptr = Foo::instance();
	}
};
#endif

struct getPtrDCLP
{
	const char* name() const { return "getPtrDCLP"; }
	void operator()() const
	{
		Foo* ptr = Foo::instance();
	}
};

struct getPtrOWTO
{
	const char* name() const { return "getPtrOWTO"; }
	void operator()() const
	{
		OWTOFoo* ptr = OWTOFoo::instance();
	}
};

int main(int argc, const char** argv)
{
	static size_t reps = 100000000;
	doTiming(Nothing(), reps);
	doTiming(getPtrNoOnce(), reps);
#ifdef OW_USE_PTHREAD
	doTiming(getPtr(), reps);
#endif
	doTiming(getPtrDCLP(), reps);
	doTiming(getPtrOWTO(), reps);
}

