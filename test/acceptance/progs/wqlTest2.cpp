/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc. All rights reserved.
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
#include "OW_CIMException.hpp"
#include "OW_WQLImpl.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMValueCast.hpp"
#include "OW_Bool.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::cerr;
using namespace OpenWBEM;


namespace
{

int queryCount = 0;

void testQuery(const char* query)
{
	++queryCount;
	cout << "\nExecuting query " << queryCount << " local: " << query << endl;
	Reference<WQLIFC> wql(new WQLImpl);

	WQLSelectStatement stmt = wql->createSelectStatement(query);
	stmt.print(cout);

	WQLCompile comp(stmt);
	comp.print(cout);
	comp.printTableau(cout);
}

class InstancePropertySource : public WQLPropertySource
{
public:
	InstancePropertySource(const CIMInstance& ci_)
		: ci(ci_)
	{
	}
	virtual bool evaluateISA(const String &propertyName, const String &className) const 
	{
		// TODO
		(void)propertyName;
		(void)className;
		return false;
	}
	virtual bool getValue(const String &propertyName, WQLOperand &value) const 
	{
		StringArray propNames = propertyName.tokenize(".");
		if (propNames[0] == ci.getClassName())
		{
			propNames.remove(0);
		}
		// don't handle embedded instances/properties yet.
		CIMProperty p = ci.getProperty(propNames[0]);
		if (!p)
		{
			return false;
		}

		CIMValue v = p.getValue();
		switch (v.getType())
		{
			case CIMDataType::DATETIME:
			case CIMDataType::CIMNULL:
				value = WQLOperand();
				break;
			case CIMDataType::UINT8:
			case CIMDataType::SINT8:
			case CIMDataType::UINT16:
			case CIMDataType::SINT16:
			case CIMDataType::UINT32:
			case CIMDataType::SINT32:
			case CIMDataType::UINT64:
			case CIMDataType::SINT64:
			case CIMDataType::CHAR16:
			{
				Int64 x;
				CIMValueCast::castValueToDataType(v, CIMDataType::SINT64).get(x);
				value = WQLOperand(x, WQL_INTEGER_VALUE_TAG);
				break;
			}
			case CIMDataType::STRING:
				value = WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
				break;
			case CIMDataType::BOOLEAN:
			{
				Bool b;
				v.get(b);
				value = WQLOperand(b, WQL_BOOLEAN_VALUE_TAG);
				break;
			}
			case CIMDataType::REAL32:
			case CIMDataType::REAL64:
			{
				Real64 x;
				CIMValueCast::castValueToDataType(v, CIMDataType::REAL64).get(x);
				value = WQLOperand(x, WQL_DOUBLE_VALUE_TAG);
				break;
			}
			case CIMDataType::REFERENCE:
				value = WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
				break;
			case CIMDataType::EMBEDDEDCLASS:
				// TODO
				value = WQLOperand();
				break;
			case CIMDataType::EMBEDDEDINSTANCE:
				// TODO
				value = WQLOperand();
				break;
			default:
				value = WQLOperand();
				break;
		}

		return true;
	}

private:
	CIMInstance ci;
};

void testQuery(const char* query, const CIMInstance& passingInstance)
{
	++queryCount;
	cout << "\nExecuting query " << queryCount << " local: " << query << endl;
	Reference<WQLIFC> wql(new WQLImpl);

	WQLSelectStatement stmt = wql->createSelectStatement(query);
	stmt.print(cout);

	WQLCompile comp(stmt);
	comp.print(cout);
	comp.printTableau(cout);
	InstancePropertySource source(passingInstance);
	if (stmt.evaluateWhereClause(&source))
	{
		cout << "stmt.evaluateWhereClause(&source): passed\n";
	}
	else
	{
		cout << "stmt.evaluateWhereClause(&source): failed\n";
		abort();
	}

	if (comp.evaluate(source))
	{
		cout << "comp.evaluate(&source): passed\n";
	}
	else
	{
		cout << "comp.evaluate(&source): failed\n";
		abort();
	}
}


} // end anonymous namespace

int main(int , char**)
{
	try
	{
		// some basic selects
		testQuery("select * from wqlTestClass");
		testQuery("select name from wqlTestClass");
		testQuery("select sint32Data from wqlTestClass");
		testQuery("select wqlTestClass.sint32Data from wqlTestClass");
		testQuery("select booleanData, wqlTestClass.sint32Data from wqlTestClass");
		testQuery("select wqlTestClass.* from wqlTestClass");

		// test some equals on the where clause
		CIMInstance inst;
		inst.setClassName("wqlTestClass");
		inst.setProperty("name", CIMValue("test1"));
		testQuery("select * from wqlTestClass where name = \"test1\"", inst);
		testQuery("select * from wqlTestClass where \"test1\" = name", inst);
		inst.setProperty("sint32Data", CIMValue(Int32(0)));
		testQuery("select * from wqlTestClass where sint32Data = 0", inst);
		testQuery("select * from wqlTestClass where 0 = sint32Data", inst);
		inst.setProperty("sint32Data", CIMValue(Int32(10)));
		testQuery("select * from wqlTestClass where sint32Data = b'1010'", inst);
		testQuery("select * from wqlTestClass where sint32Data = x'A'", inst);
		testQuery("select * from wqlTestClass where booleanData = TRUE");
		testQuery("select * from wqlTestClass where TRUE = booleanData");
		testQuery("select * from wqlTestClass where booleanData = FALSE");
		testQuery("select * from wqlTestClass where FALSE = booleanData");
		testQuery("select * from wqlTestClass where realData = 123.456");
		testQuery("select * from wqlTestClass where 123.456 = realData");
		testQuery("select * from wqlTestClass where realData = NULL");
		testQuery("select * from wqlTestClass where NULL = realData");
		testQuery("select * from wqlTestClass where __Path = \"wqlTestClass.name=\\\"test5\\\"\"");
		testQuery("select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" = __Path");

		// test not equals on the where clause
		testQuery("select * from wqlTestClass where name <> \"test1\"");
		testQuery("select * from wqlTestClass where \"test1\" <> name");
		testQuery("select * from wqlTestClass where sint32Data <> 0");
		testQuery("select * from wqlTestClass where 0 <> sint32Data");
		testQuery("select * from wqlTestClass where sint32Data <> b'1010'");
		testQuery("select * from wqlTestClass where sint32Data <> x'A'");
		testQuery("select * from wqlTestClass where booleanData <> TRUE");
		testQuery("select * from wqlTestClass where TRUE <> booleanData");
		testQuery("select * from wqlTestClass where booleanData <> FALSE");
		testQuery("select * from wqlTestClass where FALSE <> booleanData");
		testQuery("select * from wqlTestClass where realData <> 123.456");
		testQuery("select * from wqlTestClass where 123.456 <> realData");
		testQuery("select * from wqlTestClass where realData <> NULL");
		testQuery("select * from wqlTestClass where NULL <> realData");
		testQuery("select * from wqlTestClass where __Path <> \"wqlTestClass.name=\\\"test5\\\"\"");
		testQuery("select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" <> __Path");
		
		// test greater than or equals on the where clause
		testQuery("select * from wqlTestClass where name >= \"test1\"");
		testQuery("select * from wqlTestClass where \"test1\" >= name");
		testQuery("select * from wqlTestClass where sint32Data >= 0");
		testQuery("select * from wqlTestClass where 0 >= sint32Data");
		testQuery("select * from wqlTestClass where sint32Data >= b'1010'");
		testQuery("select * from wqlTestClass where sint32Data >= x'A'");
		testQuery("select * from wqlTestClass where booleanData >= TRUE");
		testQuery("select * from wqlTestClass where TRUE >= booleanData");
		testQuery("select * from wqlTestClass where booleanData >= FALSE");
		testQuery("select * from wqlTestClass where FALSE >= booleanData");
		testQuery("select * from wqlTestClass where realData >= 123.456");
		testQuery("select * from wqlTestClass where 123.456 >= realData");
		testQuery("select * from wqlTestClass where __Path >= \"wqlTestClass.name=\\\"test5\\\"\"");
		testQuery("select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" >= __Path");

		// test less than or equals on the where clause
		testQuery("select * from wqlTestClass where name <= \"test1\"");
		testQuery("select * from wqlTestClass where \"test1\" <= name");
		testQuery("select * from wqlTestClass where sint32Data <= 0");
		testQuery("select * from wqlTestClass where 0 <= sint32Data");
		testQuery("select * from wqlTestClass where sint32Data <= b'1010'");
		testQuery("select * from wqlTestClass where sint32Data <= x'A'");
		testQuery("select * from wqlTestClass where booleanData <= TRUE");
		testQuery("select * from wqlTestClass where TRUE <= booleanData");
		testQuery("select * from wqlTestClass where booleanData <= FALSE");
		testQuery("select * from wqlTestClass where FALSE <= booleanData");
		testQuery("select * from wqlTestClass where realData <= 123.456");
		testQuery("select * from wqlTestClass where 123.456 <= realData");
		testQuery("select * from wqlTestClass where __Path <= \"wqlTestClass.name=\\\"test5\\\"\"");
		testQuery("select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" <= __Path");

		// test greater than on the where clause
		testQuery("select * from wqlTestClass where name > \"test1\"");
		testQuery("select * from wqlTestClass where \"test1\" > name");
		testQuery("select * from wqlTestClass where sint32Data > 0");
		testQuery("select * from wqlTestClass where 0 > sint32Data");
		testQuery("select * from wqlTestClass where sint32Data > b'1010'");
		testQuery("select * from wqlTestClass where sint32Data > x'A'");
		testQuery("select * from wqlTestClass where booleanData > TRUE");
		testQuery("select * from wqlTestClass where TRUE > booleanData");
		testQuery("select * from wqlTestClass where booleanData > FALSE");
		testQuery("select * from wqlTestClass where FALSE > booleanData");
		testQuery("select * from wqlTestClass where realData > 123.456");
		testQuery("select * from wqlTestClass where 123.456 > realData");
		testQuery("select * from wqlTestClass where __Path > \"wqlTestClass.name=\\\"test5\\\"\"");
		testQuery("select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" > __Path");

		// test less than on the where clause
		testQuery("select * from wqlTestClass where name < \"test1\"");
		testQuery("select * from wqlTestClass where \"test1\" < name");
		testQuery("select * from wqlTestClass where sint32Data < 0");
		testQuery("select * from wqlTestClass where 0 < sint32Data");
		testQuery("select * from wqlTestClass where sint32Data < b'1010'");
		testQuery("select * from wqlTestClass where sint32Data < x'A'");
		testQuery("select * from wqlTestClass where booleanData < TRUE");
		testQuery("select * from wqlTestClass where TRUE < booleanData");
		testQuery("select * from wqlTestClass where booleanData < FALSE");
		testQuery("select * from wqlTestClass where FALSE < booleanData");
		testQuery("select * from wqlTestClass where realData < 123.456");
		testQuery("select * from wqlTestClass where 123.456 < realData");
		testQuery("select * from wqlTestClass where __Path < \"wqlTestClass.name=\\\"test5\\\"\"");
		testQuery("select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" < __Path");


		// test IS, TRUE, NOT, and FALSE
		testQuery("select * from wqlTestClass where booleanData IS TRUE");
		testQuery("select * from wqlTestClass where booleanData IS NOT TRUE");
		testQuery("select * from wqlTestClass where booleanData IS FALSE");
		testQuery("select * from wqlTestClass where booleanData IS NOT FALSE");

		testQuery("select * from wqlTestClass where realData ISNULL");
		testQuery("select * from wqlTestClass where realData IS NULL");
		testQuery("select * from wqlTestClass where realData NOTNULL");
		testQuery("select * from wqlTestClass where realData IS NOT NULL");

		// test and and or
		testQuery("select * from wqltestClass where name = \"test4\" or booleanData IS true");
		testQuery("select * from wqltestClass where name = \"test5\" or booleanData IS true");
		testQuery("select * from wqltestClass where name = \"test4\" and sint32Data = 1");
		testQuery("select * from wqltestClass where name = \"test4\" and booleanData IS TRUE");
		testQuery("select * from wqltestClass where name IS NOT NULL and realData IS NOT NULL");
		testQuery("select * from wqltestClass where (name IS NOT NULL and realData IS NOT NULL) or (name = \"test5\" or booleanData IS true)");
		testQuery("select * from wqltestClass where (name = \"test4\" or name = \"test2\") and (sint32Data = 1 or booleanData = true)");
		
		// test ISA and embedded properties
		testQuery("select * from wqlTestClass where embed ISA fooClass");
		testQuery("select * from wqlTestClass where embed ISA \"fooClass\"");
		testQuery("select * from wqlTestClass where wqlTestClass.embed ISA \"fooClass\"");
		testQuery("select * from wqlTestClass where wqlTestClass.embed.embed2 ISA \"fooClass\"");
		testQuery("select * from wqlTestClass where embed.embed2 ISA \"fooClass\"");
		testQuery("select embed from wqlTestClass where embed.embed2 ISA \"fooClass\"");
		testQuery("select embed from wqlTestClass where wqlTestClass.embed.embedInt = 1");
		testQuery("select embed.Name from wqlTestClass where wqlTestClass.embed.embedInt = 1");
		testQuery("select wqlTestClass.embed.Name from wqlTestClass where wqlTestClass.embed.embedInt = 1");

		// Markus' tests
		testQuery("SELECT x,y,z FROM myclass WHERE x > 5 AND y < 25 AND z > 1.2");
		testQuery("SELECT x,y,z,a FROM myclass WHERE (x > 5) AND ((y < 25) OR (a = \"Foo\")) AND (z > 1.2)");
		testQuery("SELECT x,y,z,a FROM myclass WHERE (y > x) AND (((y < 25) OR (a = \"Bar\")) OR (z > 1.2))");
		testQuery("SELECT w,x,y,z FROM ClassName");
		testQuery("SELECT w,x,y,z FROM ClassName WHERE w = TRUE OR w = FALSE");
		testQuery("SELECT w,x,y,z FROM ClassName WHERE w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\"");
		testQuery("SELECT * FROM ClassName WHERE (w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\") AND NOT w = TRUE IS NOT TRUE");
		testQuery("SELECT * FROM ClassName WHERE (w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\") AND NOT ((w = TRUE) IS NOT TRUE)");
		testQuery("SELECT * FROM ClassName WHERE NOT NOT NOT x < 5");
		testQuery("SELECT * FROM ClassName WHERE v IS NULL");
		testQuery("SELECT * FROM myclass WHERE (NOT (x>5) OR (y<1.0) AND (z = \"BLAH\")) AND NOT ((x<10) OR (y>4.0))");
		testQuery("SELECT * FROM myclass WHERE NOT NOT( NOT (NOT (5>x) OR (y<1.0) AND NOT (z = \"BLAH\")) AND NOT NOT ((x<10) OR (y>4.0)))");

		// more tests
		testQuery("select * from wqlTestClass", inst);

		return 0;
	}
	catch(Exception& e)
	{
		cerr << e << endl;
	}
	catch(...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}
