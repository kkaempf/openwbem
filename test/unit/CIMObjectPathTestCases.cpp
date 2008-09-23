/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
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

#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"

#include "OW_CIMObjectPath.hpp"

using namespace OpenWBEM;

AUTO_UNIT_TEST(CIMObjectPathEscape)
{
	unitAssertEquals("a\\rb\\nc\\t", CIMObjectPath::escape("a\rb\nc\t"));
	unitAssertEquals("\\\"This is a quoted string\\\"", CIMObjectPath::escape("\"This is a quoted string\""));

	unitAssertEquals("\\b\\t\\n\\f\\r\\\"\\\\extra", CIMObjectPath::escape("\b\t\n\f\r\"\\extra"));
}

AUTO_UNIT_TEST(CIMObjectPathUnescape)
{
	unitAssertEquals("a\rb\nc\t", CIMObjectPath::unEscape("a\\rb\\nc\\t"));
	unitAssertEquals("\"This is a quoted string\"", CIMObjectPath::unEscape("\\\"This is a quoted string\\\""));
	unitAssertEquals("\b\t\n\f\r\"\\extra", CIMObjectPath::unEscape("\\b\\t\\n\\f\\r\\\"\\\\extra"));

	// We won't escape these, but we are required to unescape them:
	unitAssertEquals("'single' 'quotes' 'abound'", CIMObjectPath::unEscape("\\'single\\' \\'quotes\\' \\'abound\\'"));

	// Hex sequences.  All of the hex numbers were taken directly from
	// the CIM specification.
	unitAssertEquals("\"\"\"", CIMObjectPath::unEscape("\\x0022\\x022\\x22"));
	unitAssertEquals("\b\b\b\b", CIMObjectPath::unEscape("\\x0008\\x008\\x08\\x8"));
	unitAssertEquals("\t\t\t\t", CIMObjectPath::unEscape("\\x0009\\x009\\x09\\x9"));
	unitAssertEquals("\n\n\n\n", CIMObjectPath::unEscape("\\x000a\\x00a\\x0a\\xa"));
	unitAssertEquals("\f\f\f\f", CIMObjectPath::unEscape("\\x000c\\x00c\\x0c\\xc"));
	unitAssertEquals("\r\r\r\r", CIMObjectPath::unEscape("\\x000d\\x00d\\x0d\\xd"));
	unitAssertEquals("'''", CIMObjectPath::unEscape("\\x0027\\x027\\x27"));
	unitAssertEquals("\\\\\\", CIMObjectPath::unEscape("\\x005c\\x05c\\x5c"));

	// An out-of-range condition: The replacement character is generated -- not
	// required by the spec, but it does say some conversion may need to be
	// done.
	unitAssertEquals("Too large: \xef\xbf\xbd", CIMObjectPath::unEscape("Too large: \\xFFAA"));

	// Missing hex digits are ignored (for now)
	unitAssertEquals("abc\"\"zyx", CIMObjectPath::unEscape("abc\\x22\\x\\x22zyx"));
}
