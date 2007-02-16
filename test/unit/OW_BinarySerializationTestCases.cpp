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
#include "OW_DataStreams.hpp"
#include "OW_CIMValue.hpp"
#include "OW_Char16.hpp"
#include "OW_CIMDateTime.hpp"

#include <iostream>
#include <cstdlib>

using namespace OpenWBEM;
using namespace std; 

void OW_BinarySerializationTestCases::setUp()
{
}

void OW_BinarySerializationTestCases::tearDown()
{
}

void OW_BinarySerializationTestCases::testObjectPath()
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

void OW_BinarySerializationTestCases::testCIMValueSerialization()
{
	// null
	testCIMValueSerializationAux(CIMValue(CIMNULL));

	// array types
	BoolArray ba; ba.push_back(true); ba.push_back(false);
	testCIMValueSerializationAux(CIMValue(ba));
	UInt8Array u8a(2, 55);
	testCIMValueSerializationAux(CIMValue(u8a));
	Int8Array i8a(2, 54);
	testCIMValueSerializationAux(CIMValue(i8a));
	Char16Array c16a(6, Char16('a'));
	testCIMValueSerializationAux(CIMValue(c16a));
	UInt16Array u16a(5, 16);
	testCIMValueSerializationAux(CIMValue(u16a));
	Int16Array i16a(16, 16);
	testCIMValueSerializationAux(CIMValue(i16a));
	UInt32Array u32a(2, 32);
	testCIMValueSerializationAux(CIMValue(u32a));
	Int32Array i32a(2, 32);
	testCIMValueSerializationAux(CIMValue(i32a));
	i32a.resize(1);
	testCIMValueSerializationAux(CIMValue(i32a));
	i32a.resize(0);
	testCIMValueSerializationAux(CIMValue(i32a));
	UInt64Array u64a(256, 64);
	testCIMValueSerializationAux(CIMValue(u64a));
	Int64Array i64a(256, 64);
	testCIMValueSerializationAux(CIMValue(i64a));
	Real32Array r32a(2, 32.32);
	testCIMValueSerializationAux(CIMValue(r32a));
	Real64Array r64a(1, 64.64);
	testCIMValueSerializationAux(CIMValue(r64a));
	StringArray sa(1, "Hello");
	testCIMValueSerializationAux(CIMValue(sa));
	CIMDateTimeArray cdta(1, CIMDateTime("20070215173034.123456-420"));
	testCIMValueSerializationAux(CIMValue(cdta));
	cdta = CIMDateTimeArray(1, CIMDateTime("12345678901234.123456:000"));
	testCIMValueSerializationAux(CIMValue(cdta));
	CIMObjectPathArray copa(1, CIMObjectPath("x", "y"));
	testCIMValueSerializationAux(CIMValue(copa));
	CIMClassArray cca(1, CIMClass("t"));
	testCIMValueSerializationAux(CIMValue(cca));
	CIMInstanceArray cia(1, CIMInstance("i"));
	testCIMValueSerializationAux(CIMValue(cia));

	// non-array types
	testCIMValueSerializationAux(CIMValue(Bool(true)));
	testCIMValueSerializationAux(CIMValue(UInt8(8)));
	testCIMValueSerializationAux(CIMValue(Int8(-8)));
	testCIMValueSerializationAux(CIMValue(UInt16(16)));
	testCIMValueSerializationAux(CIMValue(Int16(-16)));
	testCIMValueSerializationAux(CIMValue(UInt32(32)));
	testCIMValueSerializationAux(CIMValue(Int32(-32)));
	testCIMValueSerializationAux(CIMValue(UInt64(64)));
	testCIMValueSerializationAux(CIMValue(Int64(-64)));
	testCIMValueSerializationAux(CIMValue(Real32(-32.32)));
	testCIMValueSerializationAux(CIMValue(Real64(-64.64)));
	testCIMValueSerializationAux(CIMValue(Char16('x')));
	testCIMValueSerializationAux(CIMValue("Test"));
	testCIMValueSerializationAux(CIMValue(CIMDateTime("31181026204145.234567-531")));
	testCIMValueSerializationAux(CIMValue(CIMDateTime("98765432198765.432198:000")));
	testCIMValueSerializationAux(CIMValue(CIMObjectPath("a", "b")));
	testCIMValueSerializationAux(CIMValue(CIMClass("c")));
	testCIMValueSerializationAux(CIMValue(CIMInstance("i")));

}

void OW_BinarySerializationTestCases::testCIMValueSerializationAux(const CIMValue& v)
{
	DataOStreamBuf ostrbuf;
	BinarySerialization::writeValue(ostrbuf, v);
	if (getenv(OWLONGTEST))
	{
		for (int i = 0; i < ostrbuf.length() - 1; ++i)
		{
			DataIStreamBuf ibuf(i, ostrbuf.getData());
			unitAssertThrows(BinarySerialization::readValue(ibuf))
		}
	}
	else
	{
		// chop it in half
		DataIStreamBuf ibuf(ostrbuf.length() / 2, ostrbuf.getData());
		unitAssertThrows(BinarySerialization::readValue(ibuf))
	}
	DataIStreamBuf ibuf(ostrbuf.length(), ostrbuf.getData());
	CIMValue reconstituted = BinarySerialization::readValue(ibuf);
	unitAssertEquals(v, reconstituted);
}

Test* OW_BinarySerializationTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_BinarySerialization");

	ADD_TEST_TO_SUITE(OW_BinarySerializationTestCases, testObjectPath);
	ADD_TEST_TO_SUITE(OW_BinarySerializationTestCases, testCIMValueSerialization);
	return testSuite;
}

