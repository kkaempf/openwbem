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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "OW_config.h"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_Assertion.hpp"
#include "OW_Format.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"

#include <fstream>

using std::ifstream;
using std::ofstream;
using std::endl;

namespace
{

class OW_MethodProviderTest: public OW_CppMethodProviderIFC
{
public:
	/**
	 * The CIMOM calls this method when the method specified in the parameters
	 * is to be invoked.
	 *
	 * @param cop Contains the path to the instance whose method must be
	 * 	invoked.
	 * @param methodName The name of the method.
	 * @param inParams An array of OW_CIMValues which are the input parameters
	 * 	for this method.
	 * @param outParams An array of OW_CIMValues which are the output
	 * 	parameters for this method.
	 *
	 * @returns OW_CIMValue - The return value of the method.  If the method
	 * 	has no return value, it must return a default OW_CIMValue().
	 *
	 * @throws OW_CIMException
	 */
	void initialize(const OW_ProviderEnvironmentIFCRef& env);
	void cleanup();
	virtual ~OW_MethodProviderTest()
	{
	}
	virtual OW_CIMValue invokeMethod(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& path,
			const OW_String &methodName,
			const OW_CIMParamValueArray &in,
			OW_CIMParamValueArray &out);
private:
};
//////////////////////////////////////////////////////////////////////////////
void
OW_MethodProviderTest::initialize(const OW_ProviderEnvironmentIFCRef& env)
{
	env->getLogger()->logDebug("OW_MethodProviderTest initialize called");
}

//////////////////////////////////////////////////////////////////////////////
void
OW_MethodProviderTest::cleanup()
{
}


OW_String fileName("ow_methodProviderTestFile.txt");
OW_String getState()
{
	ifstream ifstr(fileName.c_str(), std::ios::in);
	if (!ifstr)
	{
		return OW_String("off");
	}
	char buf[80];
	ifstr >> buf;
	OW_String val(buf, strlen(buf));
	if (val.equalsIgnoreCase("on"))
	{
		return OW_String("on");
	}
	else
	{
		return OW_String("off");
	}
 	return OW_String();
}

void setState(const OW_String& newState)
{
	(void)newState;
	ofstream ofstr(fileName.c_str(), std::ios::out | std::ios::trunc);
	ofstr << newState << endl;
}

void toggleState()
{
	if (getState().equals("off"))
	{
		setState(OW_String("on"));
	}
	else
	{
		setState(OW_String("off"));
	}
}


OW_CIMValue
OW_MethodProviderTest::invokeMethod(
		const OW_ProviderEnvironmentIFCRef&,
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String &methodName,
		const OW_CIMParamValueArray &in,
		OW_CIMParamValueArray &out)
{
	(void)ns;
	(void)path;
	if (methodName.equalsIgnoreCase("getstate"))
	{
		// MOF of this method:
		//    string GetState(
		//		[in] string s,
		//		[in] uint8 uint8array[],
		//		[out] boolean b,
		//		[out] real64 r64,
		//		[in, out] sint16 io16,
		//		[out] string msg,
		//		[in, out] CIM_ComputerSystem REF paths[],
		//		[in, out] datetime nullParam);

		OW_ASSERT(in.size() == 5);
		OW_ASSERT(out.size() == 6);
		OW_ASSERT(in[0].getName() == "s");
		OW_ASSERT(in[1].getName() == "uint8array");
		OW_ASSERT(in[2].getName() == "io16");
		OW_ASSERT(in[3].getName() == "paths");
		OW_ASSERT(in[4].getName() == "nullParam");
		OW_ASSERT(out[0].getName() == "b");
		OW_ASSERT(out[1].getName() == "r64");
		OW_ASSERT(out[2].getName() == "io16");
		OW_ASSERT(out[2].getValue() == in[2].getValue());
		OW_ASSERT(out[3].getName() == "msg");
		OW_ASSERT(out[4].getName() == "paths");
		OW_ASSERT(out[4].getValue() == in[3].getValue());
		OW_ASSERT(out[5].getName() == "nullParam");
		OW_ASSERT(out[5].getValue() == in[4].getValue());
		out[0].setValue(OW_CIMValue(true));
		out[1].setValue(OW_CIMValue(OW_Real64(9.87654e32l)));
		out[2].setValue(OW_CIMValue(OW_Int16(555)));
		out[3].setValue(OW_CIMValue(OW_String(
			format("OW_MethodProviderTest::invokeMethod.  in[0] = %1, "
				"in[1] = %2, in[2] = %3, in[3] = %4, in[4] = %5, out[0] = %6, out[1] = %7, "
				"out[2] = %8, out[4] = %9",
				in[0].toString(), in[1].toString(), in[2].toString(), in[3].toString(), in[4].toString(),
				out[0].toString(), out[1].toString(), out[2].toString(), out[4].toString()))));

		return OW_CIMValue(getState());
	}
	else if (methodName.equalsIgnoreCase("setstate"))
	{
		OW_ASSERT(in.size() == 1);
		OW_String newState;
		in[0].getValue().get(newState);
		setState(newState);
		return OW_CIMValue(getState());
	}
	else if (methodName.equalsIgnoreCase("togglestate"))
	{
		toggleState();
		return OW_CIMValue(getState());
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::METHOD_NOT_FOUND, format("Cannot find "
			"method: %1", methodName).c_str());
	}
	(void)out;
}

} // end anonymous namespace

OW_PROVIDERFACTORY(OW_MethodProviderTest, methodtest)
