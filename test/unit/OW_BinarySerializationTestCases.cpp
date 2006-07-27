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

/**
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_BinarySerializationTestCases.hpp"
#include "OW_BinarySerialization.hpp"
#include "OW_IOIFCStreamBuffer.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Exec.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_String.hpp"
#include <iostream>

using namespace OpenWBEM;
using namespace std; 

void OW_BinarySerializationTestCases::setUp()
{
}

void OW_BinarySerializationTestCases::tearDown()
{
}

void OW_BinarySerializationTestCases::testSomething()
{
        StringArray cmd; 
        cmd.push_back("/bin/cat"); 
        CIMObjectPath origCop("ClassName", "ns"); 
        ProcessRef rval = OpenWBEM::Exec::spawn(cmd); 
        IOIFCStreamBuffer stdinbuf(rval->in().getPtr(), IOIFCStreamBuffer::E_OUT, 256); 
        IOIFCStreamBuffer stdoutbuf(rval->out().getPtr(), IOIFCStreamBuffer::E_IN, 256); 
        ostream stdinstr(&stdinbuf); 
        istream stdoutstr(&stdoutbuf); 
        OpenWBEM::BinarySerialization::writeObjectPath(stdinbuf, origCop); 
        stdinstr.flush(); 
        rval->in()->close(); 
        CIMObjectPath newCop = OpenWBEM::BinarySerialization::readObjectPath(stdoutbuf);
		rval->waitCloseTerm(3.0, 3.0, 3.0);
        unitAssert(rval->processStatus().terminatedSuccessfully()); 
        unitAssert(newCop.getClassName() == "ClassName"); 
        unitAssert(newCop.getNameSpace() == "ns"); 
}

Test* OW_BinarySerializationTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_BinarySerialization");

	ADD_TEST_TO_SUITE(OW_BinarySerializationTestCases, testSomething);

	return testSuite;
}

