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
#include "MOFCompilerTestCases.hpp"
#include "OW_MOFCompiler.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMQualifierType.hpp"

using namespace OpenWBEM;

void MOFCompilerTestCases::setUp()
{
}

void MOFCompilerTestCases::tearDown()
{
}

void MOFCompilerTestCases::testcompileMOF()
{
	CIMInstanceArray insts;
	CIMClassArray classes;
	CIMQualifierTypeArray qualTypes;
	unitAssertNoThrow( MOF::compileMOF(
		"INSTANCE OF fooClass {\n"
		"  strprop=\"x\";\n"
		"  intprop=55;\n"
		"};", CIMOMHandleIFCRef(), "", insts, classes, qualTypes) );
	unitAssert(insts.size() == 1);
	unitAssert(insts[0].getClassName() == "fooClass");
	unitAssert(insts[0].getProperties().size() == 2);
	unitAssert(insts[0].getProperty("strprop").getValue() == CIMValue("x"));
	// don't check the actual type, since it probably won't be right.
	unitAssert(insts[0].getProperty("intprop").getValue().toString() == CIMValue(55).toString());
	insts.clear();
	unitAssertThrows(MOF::compileMOF("this is not good mof", CIMOMHandleIFCRef(), "", insts, classes, qualTypes));
	insts.clear();
	unitAssertNoThrow(MOF::compileMOF("instance of one{x=1;}; instance of two{x=2;};", CIMOMHandleIFCRef(), "", insts, classes, qualTypes));
	unitAssert(insts.size() == 2);
}

Test* MOFCompilerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("MOFCompiler");

	ADD_TEST_TO_SUITE(MOFCompilerTestCases, testcompileMOF);

	return testSuite;
}

