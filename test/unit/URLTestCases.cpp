/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "URLTestCases.hpp"
#include "OW_URL.hpp"

using namespace OpenWBEM;

void URLTestCases::setUp()
{
}

void URLTestCases::tearDown()
{
}

void URLTestCases::testURLParsing()
{
	{
		String surl = "proto://user:pass@hostname.foo.com:1234/my/path/:model_path";
		URL url(surl);
		unitAssert(url.host.equals("hostname.foo.com"));
		unitAssert(url.credential.equals("pass"));
		unitAssert(url.namespaceName.equals("my/path"));
		unitAssert(url.port.equals("1234"));
		unitAssert(url.scheme.equals("proto"));
		unitAssert(url.principal.equals("user"));
		unitAssert(url.modelPath.equals("model_path"));
		unitAssert(url.toString().equals(surl));
	}
	{
		String surl = "hostname.foo.com";
		URL url(surl);
		unitAssert(url.host.equals("hostname.foo.com"));
		unitAssert(url.credential.empty());
		unitAssert(url.namespaceName.empty());
		unitAssert(url.port.empty());
		unitAssert(url.scheme.empty());
		unitAssert(url.principal.empty());
		unitAssert(url.modelPath.empty());
		unitAssert(url.toString().equals(surl));
	}
}

Test* URLTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("URL");

	ADD_TEST_TO_SUITE(URLTestCases, testURLParsing);

	return testSuite;
}

