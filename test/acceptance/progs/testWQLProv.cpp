/*******************************************************************************
* Copyright (C) 2007 Quest Software All rights reserved.
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
*  - Neither the name of Quest Software nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
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
#include "OW_CppProviderIncludes.hpp"
#include "OW_MOFCompiler.hpp"

using namespace OpenWBEM;


namespace
{
class testWQLProvider : public CppQueryProviderIFC
{
public:
	virtual void getQueryProviderInfoWithEnv(const ProviderRegistrationEnvironmentIFCRef& env, QueryProviderInfo& info)
	{
		info.addInstrumentedClass("wqlTestClass2");
	}
	virtual void queryInstances(const ProviderEnvironmentIFCRef& env, const String& ns, const WQLSelectStatement& query, const WQLCompile& compiledWhereClause, CIMInstanceResultHandlerIFC& result, const CIMClass& cimClass)
	{
		Logger lgr("testWQLProvider");
		OW_LOG_DEBUG(lgr, Format("testWQLProvider::queryInstances. ns = %1, query = %2", ns, query.toString()));
		CIMOMHandleIFCRef hdl(env->getCIMOMHandle());
		CIMInstanceArray ia(testWQLProvider::getInstances(hdl, ns));
		for (size_t i = 0; i < ia.size(); ++i)
		{
			OW_LOG_DEBUG(lgr, Format("evaluating instance for match:\n%1", ia[i]));
			if (compiledWhereClause.evaluate(WQLInstancePropertySource(ia[i], hdl, ns)))
			{
				OW_LOG_DEBUG(lgr, "it was a match");
				result.handle(ia[i]);
			}
		}
	}

private:
	static CIMInstanceArray getInstances(const CIMOMHandleIFCRef& hdl, const String& ns)
	{
		const char mof[] = "instance of wqlTestClass\n"
		"{\n"
		"	name = \"test1\";\n"
		"\n"
		"	sint32Data = 0;\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test2\";\n"
		"\n"
		"	sint32Data = 10;\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test3\";\n"
		"\n"
		"	booleanData = false;\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test4\";\n"
		"\n"
		"	booleanData = true;\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test5\";\n"
		"\n"
		"	uint64Data = 5000000000000;\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test6\";\n"
		"\n"
		"	uint64Data = 10;\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test7\";\n"
		"\n"
		"	stringData = \"abc\";\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test8\";\n"
		"\n"
		"	stringData = \"test\";\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test9\";\n"
		"\n"
		"	realData = 123.456789;\n"
		"};\n"
		"\n"
		"instance of wqlTestClass\n"
		"{\n"
		"	name = \"test10\";\n"
		"\n"
		"	realData = 4567890123.0e18;\n"
		"};\n";

		CIMClassArray ca;
		CIMQualifierTypeArray qta;
		CIMInstanceArray insts;
		MOF::compileMOF(mof, hdl, ns, insts, ca, qta);
		return insts;

	}
};

} // end unnamed namespace


OW_PROVIDERFACTORY(testWQLProvider, testWQLProv)


