#include "OW_CIMException.hpp"
#include "OW_WQLImpl.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "OW_WQLCompile.hpp"

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
		testQuery("select wqlTestClass.* from wqlTestClass");

		// test some equals on the where clause
		testQuery("select * from wqlTestClass where name = \"test1\"");
		testQuery("select * from wqlTestClass where \"test1\" = name");
		testQuery("select * from wqlTestClass where sint32Data = 0");
		testQuery("select * from wqlTestClass where 0 = sint32Data");
		testQuery("select * from wqlTestClass where sint32Data = b'1010'");
		testQuery("select * from wqlTestClass where sint32Data = x'A'");
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
		
		testQuery("select * from wqlTestClass where name = "
			"\"test11\"");

		testQuery("select * from wqlTestClass where name = \"test11\"");
		testQuery("select * from wqlTestClass where name = \"test11\"");

		testQuery("select * from wqlTestClass where name = \"test11\"");

		testQuery("select * from wqlTestClass where name = \"test11\"");
		testQuery("select * from wqlTestClass where name = \"test12\"");

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
		testQuery("SELECT * FROM ClassName WHERE NOT NOT NOT x < 5");
		testQuery("SELECT * FROM ClassName WHERE v IS NULL");
		testQuery("SELECT * FROM myclass WHERE (NOT (x>5) OR (y<1.0) AND (z = \"BLAH\")) AND NOT ((x<10) OR (y>4.0))");
		testQuery("SELECT * FROM myclass WHERE NOT NOT( NOT (NOT (5>x) OR (y<1.0) AND NOT (z = \"BLAH\")) AND NOT NOT ((x<10) OR (y>4.0)))");
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
