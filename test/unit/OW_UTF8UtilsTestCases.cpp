/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7, Inc nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_UTF8UtilsTestCases.hpp"
#include "OW_UTF8Utils.hpp"
#include "OW_Char16.hpp"
#include "OW_String.hpp"

using namespace OpenWBEM;

void OW_UTF8UtilsTestCases::setUp()
{
}

void OW_UTF8UtilsTestCases::tearDown()
{
}

void OW_UTF8UtilsTestCases::testCharCount()
{
	unitAssert(UTF8Utils::charCount("") == 0);
	unitAssert(UTF8Utils::charCount("a") == 1);
	unitAssert(UTF8Utils::charCount("ab") == 2);
	// some examples taken from icu many.txt
	// I used KWrite and selected UTF8 encoding to edit this stuff.
	unitAssert(UTF8Utils::charCount("外国語の勉強と教え") == 9);
	unitAssert(UTF8Utils::charCount("Изучение и обучение иностранных языков") == 38);
	unitAssert(UTF8Utils::charCount("語文教學・语文教学") == 9);
	unitAssert(UTF8Utils::charCount("Enseñanza y estudio de idiomas") == 30);
	unitAssert(UTF8Utils::charCount("Изучаване и Преподаване на Чужди Езипи") == 38);
	unitAssert(UTF8Utils::charCount("ქართული ენის შესწავლა და სწავლება") == 33);
	unitAssert(UTF8Utils::charCount("'læŋɡwidʒ 'lɘr:niŋ ænd 'ti:ʃiŋ") == 30);
	unitAssert(UTF8Utils::charCount("Lus kawm thaib qhia") == 19);
	unitAssert(UTF8Utils::charCount("Ngôn Ngữ, Sự học,") == 17);
	unitAssert(UTF8Utils::charCount("‭‫ללמוד וללמד את השֵפה") == 22);
	unitAssert(UTF8Utils::charCount("L'enseignement et l'étude des langues") == 37);
	unitAssert(UTF8Utils::charCount("㜊㞕㧍㒟㦮 㐀㛲㭘㒟") == 10);
	unitAssert(UTF8Utils::charCount("Nauka języków obcych") == 20);
	unitAssert(UTF8Utils::charCount("Γλωσσική Εκμὰθηση και Διδασκαλία") == 32);
	unitAssert(UTF8Utils::charCount("‭‫ﺗﺪﺭﯾﺲ ﻭ ﯾﺎﺩﮔﯿﺮﯼ ﺯﺑﺎﻥ") == 22);
	unitAssert(UTF8Utils::charCount("Sprachlernen und -lehren") == 24);
	unitAssert(UTF8Utils::charCount("‭‫ﺗﻌﻠ‫ُّ‪‫ﻢ ﻭﺗﺪﺭﻳﺲ ﺍﻟﻌﺮﺑﻴﺔ") == 26);
	unitAssert(UTF8Utils::charCount("เรียนและสอนภาษา") == 15);

	// some from UTF-8-test.txt
	unitAssert(UTF8Utils::charCount("1ࠀ𐀀") == 4);
	unitAssert(UTF8Utils::charCount("߿￿����") == 4);

}

void OW_UTF8UtilsTestCases::testUTF8toUCS2()
{
	unitAssert(UTF8Utils::UTF8toUCS2("a") == 'a');
	unitAssert(UTF8Utils::UTF8toUCS2("") == 0x80);
	unitAssert(UTF8Utils::UTF8toUCS2("ࠀ") == 0x800);
	unitAssert(UTF8Utils::UTF8toUCS2("￿") == 0xFFFF);
}

void OW_UTF8UtilsTestCases::testUCS2toUTF8()
{
	unitAssert(UTF8Utils::UCS2toUTF8('a') == "a");
	unitAssert(UTF8Utils::UCS2toUTF8(0x80) == "");
	unitAssert(UTF8Utils::UCS2toUTF8(0x800) == "ࠀ");
	unitAssert(UTF8Utils::UCS2toUTF8(0xFFFF) == "￿");
}

void OW_UTF8UtilsTestCases::testUTF8toUCS4()
{
	unitAssert(UTF8Utils::UTF8toUCS4("a") == 'a');
	unitAssert(UTF8Utils::UTF8toUCS4("") == 0x80);
	unitAssert(UTF8Utils::UTF8toUCS4("ࠀ") == 0x800);
	unitAssert(UTF8Utils::UTF8toUCS4("￿") == 0xFFFF);
	// TODO: add some 3&4 byte tests here
}

void OW_UTF8UtilsTestCases::testUCS4toUTF8()
{
	unitAssert(UTF8Utils::UCS4toUTF8('a') == "a");
	unitAssert(UTF8Utils::UCS4toUTF8(0x80) == "");
	unitAssert(UTF8Utils::UCS4toUTF8(0x800) == "ࠀ");
	unitAssert(UTF8Utils::UCS4toUTF8(0xFFFF) == "￿");
	// TODO: add some 3&4 byte tests here
}

Test* OW_UTF8UtilsTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_UTF8Utils");

	ADD_TEST_TO_SUITE(OW_UTF8UtilsTestCases, testCharCount);
	ADD_TEST_TO_SUITE(OW_UTF8UtilsTestCases, testUTF8toUCS2);
	ADD_TEST_TO_SUITE(OW_UTF8UtilsTestCases, testUCS2toUTF8);
	ADD_TEST_TO_SUITE(OW_UTF8UtilsTestCases, testUTF8toUCS4);
	ADD_TEST_TO_SUITE(OW_UTF8UtilsTestCases, testUCS4toUTF8);

	return testSuite;
}

