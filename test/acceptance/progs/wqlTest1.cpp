/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_HTTPClient.hpp"
#include "OW_CIMXMLCIMOMHandle.hpp"
#include "OW_BinaryCIMOMHandle.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"
#include "OW_WQLImpl.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::cerr;
using namespace OpenWBEM;

#define TEST_ASSERT(CON) if(!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)

namespace
{
class CIMInstanceEnumBuilder : public CIMInstanceResultHandlerIFC
{
public:
	CIMInstanceEnumBuilder(CIMInstanceEnumeration& e) : m_e(e) {}
protected:
	virtual void doHandle(const CIMInstance &i)
	{
		m_e.addElement(i);
	}
private:
	CIMInstanceEnumeration& m_e;
};



int queryCount = 0;

CIMInstanceArray testQueryRemote(CIMOMHandleIFCRef& rch, const char* query, int expectedSize)
{
	cout << "\nExecuting query " << queryCount << " remote: " << query << endl;
	CIMInstanceEnumeration cie = rch->execQueryE("/root/testsuite", query, "wql2");
	cout << "Got back " << cie.numberOfElements() << " instances.  Expected " <<
		expectedSize << endl;
	CIMInstanceArray cia;
	while (cie.hasMoreElements())
	{
		CIMInstance i = cie.nextElement();
		cia.push_back(i);
        cout << i.toMOF() << endl;
	}
	if (expectedSize >= 0)
	{
		TEST_ASSERT(cia.size() == static_cast<size_t>(expectedSize));
	}
	return cia;
}

CIMInstanceArray testQueryLocal(CIMOMHandleIFCRef& rch, const char* query, int expectedSize)
{
	cout << "\nExecuting query " << queryCount << " local: " << query << endl;
	Reference<WQLIFC> wql(new WQLImpl);
	CIMInstanceEnumeration cie;
	CIMInstanceEnumBuilder builder(cie);
	wql->evaluate("/root/testsuite", builder, query, "wql2", rch);
	cout << "Got back " << cie.numberOfElements() << " instances.  Expected " <<
		expectedSize << endl;
	CIMInstanceArray cia;
	while (cie.hasMoreElements())
	{
		CIMInstance i = cie.nextElement();
		cia.push_back(i);
        cout << i.toMOF() << endl;
	}
	if (expectedSize >= 0)
	{
		TEST_ASSERT(cia.size() == static_cast<size_t>(expectedSize));
	}
	return cia;
}

CIMInstanceArray testQuery(CIMOMHandleIFCRef& rch, const char* query, int expectedSize)
{
	++queryCount;
	testQueryLocal(rch,query,expectedSize);
	return testQueryRemote(rch,query,expectedSize);
}

} // end anonymous namespace

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Usage: <URL>" << endl;
		return 1;
	}

	try
	{
		String url = argv[1];
		CIMProtocolIFCRef httpClient( new HTTPClient(url) );

		URL owurl(url);
		CIMOMHandleIFCRef rch;

		if (owurl.path.equalsIgnoreCase("/owbinary"))
		{
			rch = new BinaryCIMOMHandle(httpClient);
		}
		else
		{
			rch = new CIMXMLCIMOMHandle(httpClient);
		}

		CIMInstanceArray cia;

		// some basic selects
		testQuery(rch, "select * from wqlTestClass", 10);
		testQuery(rch, "select name from wqlTestClass", 10);
		testQuery(rch, "select sint32Data from wqlTestClass", 10);
		testQuery(rch, "select wqlTestClass.sint32Data from wqlTestClass", 10);
		testQuery(rch, "select booleanData, wqlTestClass.sint32Data from wqlTestClass", 10);
		testQuery(rch, "select wqlTestClass.* from wqlTestClass", 10);

		// test some equals on the where clause
		cia = testQuery(rch, "select * from wqlTestClass where name = \"test1\"", 1);
		TEST_ASSERT( cia[0].getProperty("sint32Data").getValue().equal(CIMValue(static_cast<Int32>(0))) );
		TEST_ASSERT( cia[0].getProperty("name").getValue().equal(CIMValue(String("test1"))) );
		testQuery(rch, "select * from wqlTestClass where \"test1\" = name", 1);
		testQuery(rch, "select * from wqlTestClass where sint32Data = 0", 1);
		testQuery(rch, "select * from wqlTestClass where 0 = sint32Data", 1);
		cia = testQuery(rch, "select * from wqlTestClass where sint32Data = b'1010'", 1);
		TEST_ASSERT( cia[0].getProperty("sint32Data").getValue().equal(CIMValue(static_cast<Int32>(10))) );
		cia = testQuery(rch, "select * from wqlTestClass where sint32Data = x'A'", 1);
		TEST_ASSERT( cia[0].getProperty("sint32Data").getValue().equal(CIMValue(static_cast<Int32>(10))) );
		testQuery(rch, "select * from wqlTestClass where booleanData = TRUE", 1);
		testQuery(rch, "select * from wqlTestClass where TRUE = booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData = FALSE", 1);
		testQuery(rch, "select * from wqlTestClass where FALSE = booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where realData = 123.456", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456 = realData", 1);
		testQuery(rch, "select * from wqlTestClass where realData = NULL", 8);
		testQuery(rch, "select * from wqlTestClass where NULL = realData", 8);
		testQuery(rch, "select * from wqlTestClass where __Path = \"wqlTestClass.name=\\\"test5\\\"\"", 1);
		testQuery(rch, "select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" = __Path", 1);

		// test not equals on the where clause
		testQuery(rch, "select * from wqlTestClass where name <> \"test1\"", 9);
		testQuery(rch, "select * from wqlTestClass where \"test1\" <> name", 9);
		testQuery(rch, "select * from wqlTestClass where sint32Data <> 0", 1);
		testQuery(rch, "select * from wqlTestClass where 0 <> sint32Data", 1);
		cia = testQuery(rch, "select * from wqlTestClass where sint32Data <> b'1010'", 1);
		TEST_ASSERT( cia[0].getProperty("sint32Data").getValue().equal(CIMValue(static_cast<Int32>(0))) );
		cia = testQuery(rch, "select * from wqlTestClass where sint32Data <> x'A'", 1);
		TEST_ASSERT( cia[0].getProperty("sint32Data").getValue().equal(CIMValue(static_cast<Int32>(0))) );
		testQuery(rch, "select * from wqlTestClass where booleanData <> TRUE", 1);
		testQuery(rch, "select * from wqlTestClass where TRUE <> booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData <> FALSE", 1);
		testQuery(rch, "select * from wqlTestClass where FALSE <> booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where realData <> 123.456", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456 <> realData", 1);
		testQuery(rch, "select * from wqlTestClass where realData <> NULL", 2);
		testQuery(rch, "select * from wqlTestClass where NULL <> realData", 2);
		testQuery(rch, "select * from wqlTestClass where __Path <> \"wqlTestClass.name=\\\"test5\\\"\"", 9);
		testQuery(rch, "select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" <> __Path", 9);
		
		// test greater than or equals on the where clause
		testQuery(rch, "select * from wqlTestClass where name >= \"test1\"", 10);
		testQuery(rch, "select * from wqlTestClass where \"test1\" >= name", 1);
		testQuery(rch, "select * from wqlTestClass where sint32Data >= 0", 2);
		testQuery(rch, "select * from wqlTestClass where 0 >= sint32Data", 1);
		testQuery(rch, "select * from wqlTestClass where sint32Data >= b'1010'", 1);
		testQuery(rch, "select * from wqlTestClass where sint32Data >= x'A'", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData >= TRUE", 1);
		testQuery(rch, "select * from wqlTestClass where TRUE >= booleanData", 2);
		testQuery(rch, "select * from wqlTestClass where booleanData >= FALSE", 2);
		testQuery(rch, "select * from wqlTestClass where FALSE >= booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where realData >= 123.456", 2);
		testQuery(rch, "select * from wqlTestClass where 123.456 >= realData", 1);
		testQuery(rch, "select * from wqlTestClass where __Path >= \"wqlTestClass.name=\\\"test5\\\"\"", 5);
		testQuery(rch, "select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" >= __Path", 6);

		// test less than or equals on the where clause
		testQuery(rch, "select * from wqlTestClass where name <= \"test1\"", 1);
		testQuery(rch, "select * from wqlTestClass where \"test1\" <= name", 10);
		testQuery(rch, "select * from wqlTestClass where sint32Data <= 0", 1);
		testQuery(rch, "select * from wqlTestClass where 0 <= sint32Data", 2);
		testQuery(rch, "select * from wqlTestClass where sint32Data <= b'1010'", 2);
		testQuery(rch, "select * from wqlTestClass where sint32Data <= x'A'", 2);
		testQuery(rch, "select * from wqlTestClass where booleanData <= TRUE", 2);
		testQuery(rch, "select * from wqlTestClass where TRUE <= booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData <= FALSE", 1);
		testQuery(rch, "select * from wqlTestClass where FALSE <= booleanData", 2);
		testQuery(rch, "select * from wqlTestClass where realData <= 123.456", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456 <= realData", 2);
		testQuery(rch, "select * from wqlTestClass where __Path <= \"wqlTestClass.name=\\\"test5\\\"\"", 6);
		testQuery(rch, "select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" <= __Path", 5);

		// test greater than on the where clause
		testQuery(rch, "select * from wqlTestClass where name > \"test1\"", 9);
		testQuery(rch, "select * from wqlTestClass where \"test1\" > name", 0);
		testQuery(rch, "select * from wqlTestClass where sint32Data > 0", 1);
		testQuery(rch, "select * from wqlTestClass where 0 > sint32Data", 0);
		testQuery(rch, "select * from wqlTestClass where sint32Data > b'1010'", 0);
		testQuery(rch, "select * from wqlTestClass where sint32Data > x'A'", 0);
		testQuery(rch, "select * from wqlTestClass where booleanData > TRUE", 0);
		testQuery(rch, "select * from wqlTestClass where TRUE > booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData > FALSE", 1);
		testQuery(rch, "select * from wqlTestClass where FALSE > booleanData", 0);
		testQuery(rch, "select * from wqlTestClass where realData > 123.456", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456 > realData", 0);
		testQuery(rch, "select * from wqlTestClass where __Path > \"wqlTestClass.name=\\\"test5\\\"\"", 4);
		testQuery(rch, "select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" > __Path", 5);

		// test less than on the where clause
		testQuery(rch, "select * from wqlTestClass where name < \"test1\"", 0);
		testQuery(rch, "select * from wqlTestClass where \"test1\" < name", 9);
		testQuery(rch, "select * from wqlTestClass where sint32Data < 0", 0);
		testQuery(rch, "select * from wqlTestClass where 0 < sint32Data", 1);
		testQuery(rch, "select * from wqlTestClass where sint32Data < b'1010'", 1);
		testQuery(rch, "select * from wqlTestClass where sint32Data < x'A'", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData < TRUE", 1);
		testQuery(rch, "select * from wqlTestClass where TRUE < booleanData", 0);
		testQuery(rch, "select * from wqlTestClass where booleanData < FALSE", 0);
		testQuery(rch, "select * from wqlTestClass where FALSE < booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where realData < 123.456", 0);
		testQuery(rch, "select * from wqlTestClass where 123.456 < realData", 1);
		testQuery(rch, "select * from wqlTestClass where __Path < \"wqlTestClass.name=\\\"test5\\\"\"", 5);
		testQuery(rch, "select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" < __Path", 4);


		// test IS, TRUE, NOT, and FALSE
		testQuery(rch, "select * from wqlTestClass where booleanData IS TRUE", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData IS NOT TRUE", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData IS FALSE", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData IS NOT FALSE", 1);

		testQuery(rch, "select * from wqlTestClass where realData ISNULL", 8);
		testQuery(rch, "select * from wqlTestClass where realData IS NULL", 8);
		testQuery(rch, "select * from wqlTestClass where realData NOTNULL", 2);
		testQuery(rch, "select * from wqlTestClass where realData IS NOT NULL", 2);

		// test and and or
		testQuery(rch, "select * from wqltestClass where name = \"test4\" or booleanData IS true", 1);
		testQuery(rch, "select name, booleanData from wqltestClass where name = \"test5\" or booleanData IS true", 2);
		testQuery(rch, "select name, sint32Data from wqltestClass where name = \"test4\" and sint32Data = 1", 0);
		testQuery(rch, "select name, booleanData, sint32Data from wqltestClass where name = \"test4\" and booleanData IS TRUE", 1);
		testQuery(rch, "select name, realData from wqltestClass where name IS NOT NULL and realData IS NOT NULL", 2);
		testQuery(rch, "select name, booleanData from wqltestClass where (name IS NOT NULL and realData IS NOT NULL) or (name = \"test5\" or booleanData IS true)", 4);
		testQuery(rch, "select * from wqltestClass where (name = \"test4\" or name = \"test2\") and (sint32Data = 1 or booleanData = true)", 1);
		
		// test a simple insert
		try
		{
			testQueryRemote(rch, "INSERT INTO wqlTestClass (name, booleanData, stringData) VALUES (\"test11\", true, \"test11String\")", 1);
		}
		catch (CIMException& e)
		{
		}
		
		cia = testQuery(rch, "select * from wqlTestClass where name = "
			"\"test11\"", 1);
		TEST_ASSERT( cia[0].getProperty("booleanData").getValue().equal(CIMValue(
			Bool(true))) );
		TEST_ASSERT( cia[0].getProperty("stringData").getValue().equal(CIMValue(
			String("test11String"))) );
		TEST_ASSERT( cia[0].getProperty("name").getValue().equal(CIMValue(
			String("test11"))) );

		testQueryLocal(rch, "INSERT INTO wqlTestClass VALUES (\"test12\", 32, true, 64, \"test12String\", 50.0)", 1);
		

		// test a simple update
		testQuery(rch, "UPDATE wqlTestClass SET booleanData=false, sint32Data="
			"12345 WHERE name=\"test11\"", 1);
		cia = testQuery(rch, "select * from wqlTestClass where name = \"test11\"", 1);
		TEST_ASSERT( cia[0].getProperty("booleanData").getValue().equal(CIMValue(
			Bool(false))) );
		TEST_ASSERT( cia[0].getProperty("stringData").getValue().equal(CIMValue(
			String("test11String"))) );
		TEST_ASSERT( cia[0].getProperty("sint32Data").getValue().equal(CIMValue(
			static_cast<Int32>(12345))) );
		TEST_ASSERT( cia[0].getProperty("name").getValue().equal(CIMValue(
			String("test11"))) );

		testQuery(rch, "UPDATE wqlTestClass SET stringData=\"\" WHERE name=\"test11\"", 1);
		cia = testQuery(rch, "select * from wqlTestClass where name = \"test11\"", 1);
		TEST_ASSERT( cia[0].getProperty("stringData").getValue().equal(CIMValue(
			String(""))) );

		testQuery(rch, "UPDATE wqlTestClass SET stringData=null WHERE name=\"test11\"", 1);
		cia = testQuery(rch, "select * from wqlTestClass where name = \"test11\"", 1);
		TEST_ASSERT( !cia[0].getProperty("stringData").getValue());

		// test a simple delete
		testQueryRemote(rch, "DELETE FROM wqlTestClass WHERE name=\"test11\"", 1);
		testQuery(rch, "select * from wqlTestClass where name = \"test11\"", 0);
		testQueryLocal(rch, "DELETE FROM wqlTestClass WHERE name=\"test12\"", 1);
		testQuery(rch, "select * from wqlTestClass where name = \"test12\"", 0);

		return 0;
	}
	catch(Assertion& a)
	{
		cerr << "Caught Assertion main() " << a.getMessage() << endl;
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
