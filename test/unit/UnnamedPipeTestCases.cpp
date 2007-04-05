/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "UnnamedPipeTestCases.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_String.hpp"

using namespace OpenWBEM;

void UnnamedPipeTestCases::setUp()
{
}

void UnnamedPipeTestCases::tearDown()
{
}

void UnnamedPipeTestCases::testDescriptorPassing()
{
	UnnamedPipeRef up1 = UnnamedPipe::createUnnamedPipe();
	UnnamedPipeRef up2, up3;
	UnnamedPipe::createConnectedPipes(up2, up3);
	up1->passDescriptor(up3->getInputDescriptor());
	AutoDescriptor d1 = up1->receiveDescriptor(up1);
	up1->passDescriptor(up3->getOutputDescriptor());
	AutoDescriptor d2 = up1->receiveDescriptor(up1);

	UnnamedPipeRef upPassed = UnnamedPipe::createUnnamedPipeFromDescriptor(d2, d1);

	String text = "abc";
	up2->writeString(text);
	String s;
	upPassed->readString(s);
	unitAssert(s == text);

	text = "xyz";
	upPassed->writeString(text);
	up2->readString(s);
	unitAssert(s == text);
}

Test* UnnamedPipeTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("UnnamedPipe");

	ADD_TEST_TO_SUITE(UnnamedPipeTestCases, testDescriptorPassing);

	return testSuite;
}

