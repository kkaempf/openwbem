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
#include "OW_CIMObjectPath.hpp"

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

void MOFCompilerTestCases::testCrossNamespaceAssociations()
{
	CIMInstanceArray insts;
	CIMClassArray classes;
	CIMQualifierTypeArray qualTypes;
	unitAssertNoThrow( MOF::compileMOF(
	"#pragma namespace (\"test1\")\n"
	"Qualifier Key : boolean = false,\n"
	"    Scope(property, reference),\n"
	"    Flavor(DisableOverride);\n"
	
	"class Test\n"
	"{\n"
	"    [key]\n"
	"    string id;\n"
	"};\n"
	
	"INSTANCE OF Test as $test1\n"
	"{\n"
	"    id = \"mytest1\";\n"
	"};\n"
	
	"#pragma namespace (\"test2\")\n"
	"Qualifier Key : boolean = false,\n"
	"    Scope(property, reference),\n"
	"    Flavor(DisableOverride);\n"
	
	"class Test\n"
	"{\n"
	"    [key]\n"
	"    string id;\n"
	"};\n"
	
	"INSTANCE OF Test as $test2\n"
	"{\n"
	"    id = \"mytest2\";\n"
	"    value = 2;\n"
	"};\n"
	
	"#pragma namespace (\"test3\")\n"
	"Qualifier Key : boolean = false,\n"
	"    Scope(property, reference),\n"
	"    Flavor(DisableOverride);\n"
	"Qualifier Association : boolean = false,\n"
	"    Scope(association),\n"
	"    Flavor(DisableOverride);\n"

	
	"[Association]\n"
	"class Test_Assoc\n"
	"{\n"
	"   [Key]\n"
	"   Test REF Antecedent;\n"
	
	"   [Key]\n"
	"   Test REF Dependent;\n"
	"};\n"
	
	"INSTANCE OF Test_Assoc\n"
	"{\n"
	"    antecedent = $test1;\n"
	"    dependent =  $test2;\n"
	"};\n"

		, CIMOMHandleIFCRef(), "", insts, classes, qualTypes) );
	unitAssert(insts.size() == 3);
	unitAssert(insts[2].getClassName() == "Test_Assoc");
	unitAssert(insts[2].getProperty("Antecedent").getValue().toCIMObjectPath().getNameSpace() == "test1");
	unitAssert(insts[2].getProperty("Dependent").getValue().toCIMObjectPath().getNameSpace() == "test2");
}

void MOFCompilerTestCases::testfixParsedString()
{
	unitAssertEquals( MOF::Compiler::fixParsedString("\"\\b\\t\\n\\f\\r\\\"\\'\\\\\""), "\b\t\n\f\r\"\'\\" );
	unitAssertEquals( MOF::Compiler::fixParsedString("\"\\x1\\XA\\X3\\X0F\\x7F\""), "\x1\xA\x3\x0F\x7F" );
	unitAssertEquals( MOF::Compiler::fixParsedString("\"only one escape\\n\""), "only one escape\n" );
	unitAssertEquals( MOF::Compiler::fixParsedString("\"\\nescape at the beginning\""), "\nescape at the beginning" );
	unitAssertEquals( MOF::Compiler::fixParsedString("\"\\nescape at the beginning and end\\n\""), "\nescape at the beginning and end\n" );

	// test too large hex
	unitAssertThrows(MOF::Compiler::fixParsedString("\"\\xFF\""));
	// test too large oct
	unitAssertThrows(MOF::Compiler::fixParsedString("\"\\777\""));

}

Test* MOFCompilerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("MOFCompiler");

	ADD_TEST_TO_SUITE(MOFCompilerTestCases, testcompileMOF);
	ADD_TEST_TO_SUITE(MOFCompilerTestCases, testCrossNamespaceAssociations);
	ADD_TEST_TO_SUITE(MOFCompilerTestCases, testfixParsedString);

	return testSuite;
}

