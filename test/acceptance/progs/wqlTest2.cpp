#include "OW_CIMException.hpp"
#include "OW_WQLImpl.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMValueCast.hpp"

#include <iostream>
#include <assert.h>

using std::cout;
using std::endl;
using std::cerr;

namespace
{

int queryCount = 0;

void testQuery(const char* query)
{
	++queryCount;
	cout << "\nExecuting query " << queryCount << " local: " << query << endl;
	OW_Reference<OW_WQLIFC> wql(new OW_WQLImpl);

	OW_WQLSelectStatement stmt = wql->createSelectStatement(query);
	stmt.print(cout);

	OW_WQLCompile comp(stmt);
	comp.print(cout);
	comp.printTableau(cout);
}

class InstancePropertySource : public OW_WQLPropertySource
{
public:
	InstancePropertySource(const OW_CIMInstance& ci_)
		: ci(ci_)
	{
	}
	virtual bool evaluateISA(const OW_String &propertyName, const OW_String &className) const 
	{
		// TODO
		(void)propertyName;
		(void)className;
		return false;
	}
	virtual bool getValue(const OW_String &propertyName, OW_WQLOperand &value) const 
	{
		OW_StringArray propNames = propertyName.tokenize(".");
		if (propNames[0] == ci.getClassName())
		{
			propNames.remove(0);
		}
		// don't handle embedded instances/properties yet.
		OW_CIMProperty p = ci.getProperty(propNames[0]);
		if (!p)
		{
			return false;
		}

		OW_CIMValue v = p.getValue();
		switch (v.getType())
		{
			case OW_CIMDataType::DATETIME:
			case OW_CIMDataType::CIMNULL:
				value = OW_WQLOperand();
				break;
			case OW_CIMDataType::UINT8:
			case OW_CIMDataType::SINT8:
			case OW_CIMDataType::UINT16:
			case OW_CIMDataType::SINT16:
			case OW_CIMDataType::UINT32:
			case OW_CIMDataType::SINT32:
			case OW_CIMDataType::UINT64:
			case OW_CIMDataType::SINT64:
			case OW_CIMDataType::CHAR16:
			{
				OW_Int64 x;
				OW_CIMValueCast::castValueToDataType(v, OW_CIMDataType::SINT64).get(x);
				value = OW_WQLOperand(x, WQL_INTEGER_VALUE_TAG);
				break;
			}
			case OW_CIMDataType::STRING:
				value = OW_WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
				break;
			case OW_CIMDataType::BOOLEAN:
			{
				OW_Bool b;
				v.get(b);
				value = OW_WQLOperand(b, WQL_BOOLEAN_VALUE_TAG);
				break;
			}
			case OW_CIMDataType::REAL32:
			case OW_CIMDataType::REAL64:
			{
				OW_Real64 x;
				OW_CIMValueCast::castValueToDataType(v, OW_CIMDataType::REAL64).get(x);
				value = OW_WQLOperand(x, WQL_DOUBLE_VALUE_TAG);
				break;
			}
			case OW_CIMDataType::REFERENCE:
				value = OW_WQLOperand(v.toString(), WQL_STRING_VALUE_TAG);
				break;
			case OW_CIMDataType::EMBEDDEDCLASS:
				// TODO
				value = OW_WQLOperand();
				break;
			case OW_CIMDataType::EMBEDDEDINSTANCE:
				// TODO
				value = OW_WQLOperand();
				break;
			default:
				value = OW_WQLOperand();
				break;
		}

		return true;
	}

private:
	OW_CIMInstance ci;
};

void testQuery(const char* query, const OW_CIMInstance& passingInstance)
{
	++queryCount;
	cout << "\nExecuting query " << queryCount << " local: " << query << endl;
	OW_Reference<OW_WQLIFC> wql(new OW_WQLImpl);

	OW_WQLSelectStatement stmt = wql->createSelectStatement(query);
	stmt.print(cout);

	OW_WQLCompile comp(stmt);
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
		OW_CIMInstance inst;
		inst.setClassName("wqlTestClass");
		inst.setProperty("name", OW_CIMValue("test1"));
		testQuery("select * from wqlTestClass where name = \"test1\"", inst);
		testQuery("select * from wqlTestClass where \"test1\" = name", inst);
		inst.setProperty("sint32Data", OW_CIMValue(OW_Int32(0)));
		testQuery("select * from wqlTestClass where sint32Data = 0", inst);
		testQuery("select * from wqlTestClass where 0 = sint32Data", inst);
		inst.setProperty("sint32Data", OW_CIMValue(OW_Int32(10)));
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
	catch(OW_Exception& e)
	{
		cerr << e << endl;
	}
	catch(...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}
