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
#include "OW_UUIDTestCases.hpp"
#include "OW_UUID.hpp"
#include "OW_Array.hpp"

#include <algorithm> // for std::sort

#include <iostream>

void OW_UUIDTestCases::setUp()
{
}

void OW_UUIDTestCases::tearDown()
{
}

void OW_UUIDTestCases::testUnique()
{
	// Create 10000, and then make sure they're all different.  
	// More would be better, but we need to keep the tests fast.
	const int NUM = 100000;
	OW_Array<OW_UUID> uuids;
	uuids.reserve(NUM); 
	for (int i = 0; i < NUM; ++i)
		uuids.push_back(OW_UUID());

	std::sort(uuids.begin(), uuids.end());
	for (int i = 0; i < NUM-1; ++i)
	{
		unitAssert(uuids[i] != uuids[i+1]);
	}
}

void OW_UUIDTestCases::testStringConversion()
{
	OW_UUID uuid1;
	OW_String suuid1(uuid1.toString());
	OW_UUID uuid1_2(suuid1);
	unitAssert(uuid1 == uuid1_2);

	// test round-tripping
	OW_String suuid1_2(uuid1_2.toString());
	unitAssert(suuid1 == suuid1_2);

	// construct from invalid make sure it throws.
	unitAssertThrows(OW_UUID("too short"));
	// wrong format
	unitAssertThrows(OW_UUID("6ba7b810-9dad-11d1-80b40-0c04fd430c8"));
	// non-hex value
	unitAssertThrows(OW_UUID("6ba7b810-9dad-11d1-80b4-X0c04fd430c8"));

	// construct 2 from the same string, make sure they're equal
	unitAssert(OW_UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8") ==
		OW_UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8"));
	unitAssert(OW_UUID("11111111-1111-1111-1111-111111111111") !=
		OW_UUID("22222222-2222-2222-2222-222222222222"));

	// copy 1 and make sure it outputs the same string
	OW_UUID uuid3;
	OW_UUID uuid3_2(uuid3);
	unitAssert(uuid3.toString() == uuid3_2.toString());
}

Test* OW_UUIDTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_UUID");

	ADD_TEST_TO_SUITE(OW_UUIDTestCases, testUnique);
	ADD_TEST_TO_SUITE(OW_UUIDTestCases, testStringConversion);

	return testSuite;
}

