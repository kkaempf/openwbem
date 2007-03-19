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
#include "OW_FileSystem.hpp"
#include "OW_File.hpp"
#include "OW_Thread.hpp"
#include "OW_Logger.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMErrorException.hpp"

using namespace std;
using namespace OpenWBEM;
using namespace WBEMFlags;
using namespace MethodLocking;

namespace
{
const String COMPONENT_NAME("ow.test.testMethodLocking");

void invoke(const CIMOMHandleSource& chs, const String& ns, const String& cls, const String& method)
{
	CIMOMHandleIFCRef hdl = chs.getCIMOMHandle();
	CIMParamValueArray inparams;
	CIMParamValueArray outparams;
	CIMObjectPath path(cls);
	CIMValue rv = hdl->invokeMethod(ns, path, method, inparams, outparams);
	// just ignore rv for this test.
}

void invoke(const CIMOMHandleSource& chs, const String& ns, const String& cls, const String& method, const String& startedFile, const String& waitForFile)
{
	CIMOMHandleIFCRef hdl = chs.getCIMOMHandle();
	CIMParamValueArray inparams;
	inparams.push_back(CIMParamValue("startedFile", CIMValue(startedFile)));
	inparams.push_back(CIMParamValue("waitForFile", CIMValue(waitForFile)));
	CIMParamValueArray outparams;
	CIMObjectPath path(cls);
	CIMValue rv = hdl->invokeMethod(ns, path, method, inparams, outparams);
	// just ignore rv for this test.
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
		invoke(m_chs, m_ns, m_waitClass, "wait", m_startedFile, m_waitForFile);
		return 0;
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
	{
	}
	~WaitReleaser()
	{
		FileSystem::createFile(m_waitForFile);
		m_waitingThread.join();
	}
private:
	String m_waitForFile;
	Thread& m_waitingThread;
};

} // end anonymous namespace

namespace MethodLocking
{

int test(const String& ns, const String& waitClass, const String& testClass, const CIMOMHandleSource& chs)
{
	String startedFile = FileSystem::Path::getCurrentWorkingDirectory() + "/startedFile";
	String waitForFile = FileSystem::Path::getCurrentWorkingDirectory() + "/waitForFile";
	FileSystem::removeFile(startedFile);
	FileSystem::removeFile(waitForFile);
	WaitThread waitThread(ns, waitClass, chs, startedFile, waitForFile);
	waitThread.start();
	while (!FileSystem::exists(startedFile))
	{
		Thread::yield();
	}
	WaitReleaser waitReleaser(waitForFile, waitThread);

	try
	{
		invoke(chs, ns, testClass, "test");
	}
	catch (CIMErrorException& e)
	{
		return 0;
	}
	return 1;
}

int test(const String& ns, const String& waitClass1, const String& waitClass2, const String& testClass, const CIMOMHandleSource& chs)
{
	String startedFile1 = FileSystem::Path::getCurrentWorkingDirectory() + "/startedFile1";
	String waitForFile1 = FileSystem::Path::getCurrentWorkingDirectory() + "/waitForFile1";
	FileSystem::removeFile(startedFile1);
	FileSystem::removeFile(waitForFile1);
	WaitThread waitThread1(ns, waitClass1, chs, startedFile1, waitForFile1);
	waitThread1.start();
	while (!FileSystem::exists(startedFile1))
	{
		Thread::yield();
	}
	WaitReleaser waitReleaser1(waitForFile1, waitThread1);

	String startedFile2 = FileSystem::Path::getCurrentWorkingDirectory() + "/startedFile2";
	String waitForFile2 = FileSystem::Path::getCurrentWorkingDirectory() + "/waitForFile2";
	FileSystem::removeFile(startedFile2);
	FileSystem::removeFile(waitForFile2);
	WaitThread waitThread2(ns, waitClass2, chs, startedFile2, waitForFile2);
	waitThread2.start();
	while (!FileSystem::exists(startedFile2))
	{
		Thread::yield();
	}
	WaitReleaser waitReleaser2(waitForFile2, waitThread2);

	try
	{
		invoke(chs, ns, testClass, "test");
	}
	catch (CIMErrorException& e)
	{
		return 0;
	}
	return 1;
}

CIMOMHandleSource::~CIMOMHandleSource()
{
}

} // end namespace MethodLocking


