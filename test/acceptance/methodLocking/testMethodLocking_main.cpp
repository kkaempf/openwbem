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
#include "OW_ClientCIMOMHandle.hpp"
#include "OW_Assertion.hpp"
#include "blocxx/SocketBaseImpl.hpp"
#include "blocxx/LogAppender.hpp"
#include "blocxx/CerrAppender.hpp"
#include "testMethodLocking.hpp"

#include <iostream>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::find;
using namespace OpenWBEM;
using namespace WBEMFlags;


// create our own TEST_ASSERT, because assert and ASSERT will be turned off
// in non-debug mode.
#define TEST_ASSERT(CON) if (!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

namespace
{

//////////////////////////////////////////////////////////////////////////////
void
usage(const char* name)
{
	cerr << "Usage: " << name << " <url> [dump file extension]" << endl;
}
		

class ClientCIMOMHandleSource : public MethodLocking::CIMOMHandleSource
{
public:
	ClientCIMOMHandleSource(const String& url)
	: m_url(url)
	{
	}

	virtual CIMOMHandleIFCRef getCIMOMHandle() const
	{
		return ClientCIMOMHandle::createFromURL(m_url);
	}
private:
	String m_url;
};

} // end anonymous namespace

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

		if (argc == 3)
		{
			String sockDumpOut = argv[2];
			String sockDumpIn = argv[2];
			sockDumpOut += "SockDumpOut";
			sockDumpIn += "SockDumpIn";
			SocketBaseImpl::setDumpFiles(sockDumpIn.c_str(), sockDumpOut.c_str());
		}
		else
		{
			SocketBaseImpl::setDumpFiles("","");
		}

		LogAppender::setDefaultLogAppender(LogAppenderRef(new CerrAppender()));
		String url(argv[1]);
		
		ClientCIMOMHandleSource chs(url);
		const char* const cppNS = "root/methodLockingTest-cpp";
		const char* const oopNS = "root/methodLockingTest-oop";
		const char* const no = "no";
		const char* const no2 = "no2";
		const char* const read = "read";
		const char* const write = "write";
		// first cpp
		TEST_ASSERT(MethodLocking::test(cppNS, read, no, chs));
		TEST_ASSERT(MethodLocking::test(cppNS, write, no, chs));
		TEST_ASSERT(MethodLocking::test(cppNS, read, read, chs));
		TEST_ASSERT(MethodLocking::test(cppNS, no, write, chs));
		TEST_ASSERT(MethodLocking::test(cppNS, no, no, chs));
		// oop
		TEST_ASSERT(MethodLocking::test(oopNS, no, no2, chs));
		TEST_ASSERT(MethodLocking::test(oopNS, write, no, chs));
		TEST_ASSERT(!MethodLocking::test(oopNS, write, no, read, chs));
		TEST_ASSERT(!MethodLocking::test(oopNS, read, write, chs));
			
		return 0;

	}
	catch (Exception& e)
	{
		cerr << e << endl;
	}
	return 1;
}


