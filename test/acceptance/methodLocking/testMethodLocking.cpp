/*******************************************************************************
* Copyright (C) 2007 Quest Software. All rights reserved.
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
*  - Neither the name of Quest Software nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
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
#include "testMethodLocking.hpp"
#include "blocxx/FileSystem.hpp"
#include "blocxx/File.hpp"
#include "blocxx/Thread.hpp"
#include "OW_Logger.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMErrorException.hpp"
#include "blocxx/Timeout.hpp"
#include "blocxx/TimeoutTimer.hpp"
#include "blocxx/Bool.hpp"
#include "blocxx/Format.hpp"
#include "OW_IOException.hpp"

using namespace std;
using namespace OpenWBEM;
using namespace WBEMFlags;
using namespace MethodLocking;

namespace
{
const String COMPONENT_NAME("ow.test.testMethodLocking");

bool invoke(const CIMOMHandleSource& chs, const String& ns, const String& cls, const String& method)
{
	CIMOMHandleIFCRef hdl = chs.getCIMOMHandle();
	CIMParamValueArray inparams;
	CIMParamValueArray outparams;
	CIMObjectPath path(cls);
	CIMValue rv = hdl->invokeMethod(ns, path, method, inparams, outparams);
	return rv.toBool();
}

bool invoke(const CIMOMHandleSource& chs, const String& ns, const String& cls, const String& method, const String& startedFile, const String& waitForFile)
{
	CIMOMHandleIFCRef hdl = chs.getCIMOMHandle();
	CIMParamValueArray inparams;
	inparams.push_back(CIMParamValue("startedFile", CIMValue(startedFile)));
	inparams.push_back(CIMParamValue("waitForFile", CIMValue(waitForFile)));
	CIMParamValueArray outparams;
	CIMObjectPath path(cls);
	CIMValue rv = hdl->invokeMethod(ns, path, method, inparams, outparams);
	return rv.toBool();
}

class WaitThread : public Thread
{
public:
	WaitThread(const String& ns, const String& waitClass, const CIMOMHandleSource& chs, const String& startedFile, const String& waitForFile)
	: m_ns(ns)
	, m_waitClass(waitClass)
	, m_chs(chs)
	, m_startedFile(startedFile)
	, m_waitForFile(waitForFile)
	{
	}

	virtual Int32 run()
	{
		try
		{
			return invoke(m_chs, m_ns, m_waitClass, "wait", m_startedFile, m_waitForFile) ? 1 : 0;
		}
		catch (Exception& e)
		{
			Logger lgr(COMPONENT_NAME);
			OW_LOG_ERROR(lgr, Format("WaitThread::run() caught exception: %1", e));
			return 0;
		}
	}
private:
	String m_ns;
	String m_waitClass;
	const CIMOMHandleSource& m_chs;
	String m_startedFile;
	String m_waitForFile;
};

class WaitReleaser
{
public:
	WaitReleaser(const String& waitForFile, Thread& waitingThread)
	: m_waitForFile(waitForFile)
	, m_waitingThread(waitingThread)
	, m_released(false)
	{
	}
	~WaitReleaser()
	{
		release();
	}

	bool release()
	{
		if (!m_released)
		{
			if (!FileSystem::createFile(m_waitForFile))
			{
				Logger lgr(COMPONENT_NAME);
				OW_LOG_ERROR(lgr, Format("WaitReleaser::release() failed to create: %1, %2", m_waitForFile, strerror(errno)));
			}
			bool rv = m_waitingThread.join() == 0 ? false : true;
			m_released = true;
			return rv;
		}
		return true;
	}
private:
	String m_waitForFile;
	Thread& m_waitingThread;
	bool m_released;
};

} // end anonymous namespace

namespace MethodLocking
{

bool test(const String& ns, const String& waitClass, const String& testClass, const CIMOMHandleSource& chs)
{
	Logger lgr(COMPONENT_NAME);
	String startedFile = FileSystem::Path::getCurrentWorkingDirectory() + "/startedFile";
	String waitForFile = FileSystem::Path::getCurrentWorkingDirectory() + "/waitForFile";
	FileSystem::removeFile(startedFile);
	FileSystem::removeFile(waitForFile);
	WaitThread waitThread(ns, waitClass, chs, startedFile, waitForFile);
	waitThread.start();
	Timeout to = Timeout::relative(10.0);
	TimeoutTimer timer(to);
	while (!FileSystem::exists(startedFile) && !timer.expired())
	{
		Thread::yield();
		timer.loop();
	}
	if (!FileSystem::exists(startedFile))
	{
		OW_LOG_ERROR(lgr, Format("test timed out waiting for creation of: %1", startedFile));
		return false;
	}
	WaitReleaser waitReleaser(waitForFile, waitThread);

	try
	{
		if (!invoke(chs, ns, testClass, "test"))
		{
			OW_LOG_ERROR(lgr, "test invocation of test failed");
			return false;
		}
	}
	catch (CIMErrorException& e)
	{
		// this happens in a client
		OW_LOG_ERROR(lgr, Format("test invocation of test got exception: %1", e));
		return false;
	}
	catch (IOException& e)
	{
		// this happens in a provider
		OW_LOG_ERROR(lgr, Format("test invocation of test got exception: %1", e));
		return false;
	}

	if (!waitReleaser.release())
	{
		OW_LOG_ERROR(lgr, "test waitReleaser.release() failed");
		return false;
	}
	return true;
}

bool test(const String& ns, const String& waitClass1, const String& waitClass2, const String& testClass, const CIMOMHandleSource& chs)
{
	Logger lgr(COMPONENT_NAME);
	String startedFile1 = FileSystem::Path::getCurrentWorkingDirectory() + "/startedFile1";
	String waitForFile1 = FileSystem::Path::getCurrentWorkingDirectory() + "/waitForFile1";
	FileSystem::removeFile(startedFile1);
	FileSystem::removeFile(waitForFile1);
	WaitThread waitThread1(ns, waitClass1, chs, startedFile1, waitForFile1);
	waitThread1.start();
	Timeout to = Timeout::relative(10.0);
	TimeoutTimer timer(to);
	while (!FileSystem::exists(startedFile1) && !timer.expired())
	{
		Thread::yield();
		timer.loop();
	}
	if (!FileSystem::exists(startedFile1))
	{
		OW_LOG_ERROR(lgr, Format("test timed out waiting for creation of: %1", startedFile1));
		return false;
	}
	WaitReleaser waitReleaser1(waitForFile1, waitThread1);

	String startedFile2 = FileSystem::Path::getCurrentWorkingDirectory() + "/startedFile2";
	String waitForFile2 = FileSystem::Path::getCurrentWorkingDirectory() + "/waitForFile2";
	FileSystem::removeFile(startedFile2);
	FileSystem::removeFile(waitForFile2);
	WaitThread waitThread2(ns, waitClass2, chs, startedFile2, waitForFile2);
	waitThread2.start();
	timer = TimeoutTimer(to); // reset it
	while (!FileSystem::exists(startedFile2) && !timer.expired())
	{
		Thread::yield();
		timer.loop();
	}
	if (!FileSystem::exists(startedFile2))
	{
		OW_LOG_ERROR(lgr, Format("test timed out waiting for creation of: %1", startedFile2));
		return false;
	}
	WaitReleaser waitReleaser2(waitForFile2, waitThread2);

	try
	{
		if (!invoke(chs, ns, testClass, "test"))
		{
			OW_LOG_ERROR(lgr, "test invocation of test failed");
			return false;
		}
	}
	catch (CIMErrorException& e)
	{
		// this happens in a client
		OW_LOG_ERROR(lgr, Format("test invocation of test got exception: %1", e));
		return false;
	}
	catch (IOException& e)
	{
		// this happens in a provider
		OW_LOG_ERROR(lgr, Format("test invocation of test got exception: %1", e));
		return false;
	}

	if (!waitReleaser2.release())
	{
		OW_LOG_ERROR(lgr, "test waitReleaser2.release() failed");
		return false;
	}
	if (!waitReleaser1.release())
	{
		OW_LOG_ERROR(lgr, "test waitReleaser1.release() failed");
		return false;
	}
	return true;
}

CIMOMHandleSource::~CIMOMHandleSource()
{
}

} // end namespace MethodLocking


