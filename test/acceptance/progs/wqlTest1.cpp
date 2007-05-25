/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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

#include "OW_ClientCIMOMHandle.hpp"
#include "OW_AppenderLogger.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMName.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMNameSpaceUtils.hpp"
#include "OW_CIMException.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_Format.hpp"
#include "OW_FileAppender.hpp"
#include "OW_Logger.hpp"
#include "OW_LogAppender.hpp"
#include "OW_WQLImpl.hpp"
#include "OW_Bool.hpp"
#include "OW_ResultHandlerIFC.hpp"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::cerr;
using namespace OpenWBEM;
using namespace OpenWBEM::WQL;

#define TEST_ASSERT(CON) if (!(CON)) throw AssertionException(__FILE__, __LINE__, #CON)


#ifdef __GNUC__
#define OW_FUNCTION_NAME	__PRETTY_FUNCTION__
#else
#define OW_FUNCTION_NAME "Function name not available"
#endif

#define LOG_DEBUG(message)												\
do																								\
{																									\
	String messageWithFunction(message);						\
	messageWithFunction+= String(" ") + String(OW_FUNCTION_NAME) + String(" "); \
	OW_LOG_DEBUG(Logger("wqlTest1"), messageWithFunction);	\
}while(0);


namespace
{
	void createWqlTestLogger()
	{
		StringArray components; components.push_back("test.wql.schema_query");
		StringArray categories; categories.push_back("*");
		String messageFormat("%r [%t] %p %c - %m");
        LogAppender::setDefaultLogAppender(new FileAppender(components, categories, (String("results/") + components[0]).c_str(), messageFormat, 0, 0, true));
	}
	
	
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
	WQLIFCRef::element_type wql(new WQLImpl);
	CIMInstanceEnumeration cie;
	CIMInstanceEnumBuilder builder(cie);
	LOG_DEBUG("wql->evaluate(\"/root/testsuite\", builder, query, \"wql2\", rch);");
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
	LOG_DEBUG(Format("Testing query %1 %2 locally.", queryCount, query));
	testQueryLocal(rch,query,expectedSize);
	LOG_DEBUG(Format("Testing query %1 %2 remotely.", queryCount, query));
	return testQueryRemote(rch,query,expectedSize);
}

#ifndef OW_DISABLE_SCHEMA_MANIPULATION
namespace testSchemaQuery
{
	void mkChildren(CIMClassArray& result, char const* parentName, size_t children= 3)
	{
		LOG_DEBUG(Format("Making %1 children of %2 .", children, parentName));
		for(size_t current= 0; current < children ; ++current)
		{
			std::ostringstream childName;
			childName << "Child" << current << "Of" << parentName;
			LOG_DEBUG(Format("Making child %1 of %2 .", childName.str(), parentName));
			CIMClass child(childName.str().c_str());
			child.setSuperClass(CIMName(parentName));
			result.push_back(child);
		}
	}
	void mkChildren(CIMOMHandleIFCRef& rch, char const* ns, char const* parentName, size_t children= 3)
	{
		LOG_DEBUG(Format("Making %1 children of %2 .", children, parentName));
		CIMClassArray ch;
		mkChildren(ch, parentName, children);
		for(size_t current= 0; current < children; ++current)
		{
			rch->createClass(ns, ch[current]);
		}
	}
	void mkChildren2Level(CIMOMHandleIFCRef& rch, char const* ns, char const* parentName, size_t children= 3)
	{
		LOG_DEBUG(Format("Making %1 children of %2 .", children, parentName));
		CIMClassArray ch;
		mkChildren(ch, parentName, children);
		for(size_t current= 0; current < children; ++current)
		{
			rch->createClass(ns, ch[current]);
			CIMClassArray ch2;
			mkChildren(ch2, ch[current].getName().c_str());
			for(size_t current2= 0; current2 < children; ++current2)
			{
				rch->createClass(ns, ch2[current2]);
			}
		}
	}
	void addTestClasses(CIMOMHandleIFCRef& rch)
	{
		String ns("/root/testsuite");
		LOG_DEBUG(Format("Adding test classes to namespace %1 .", ns));
		rch->createClass(ns, CIMClass("ClassWithNoChildren"));
		rch->createClass(ns, CIMClass("ClassWithManyChildren"));
		rch->createClass(ns, CIMClass("ClassWithManyChildren2Level"));
		mkChildren(rch, ns.c_str(), "ClassWithManyChildren");
		mkChildren2Level(rch, ns.c_str(), "ClassWithManyChildren2Level");
	}
	
	void doesNotExist(CIMOMHandleIFCRef& rch, String const& schemaQueryOperator, String const& schemaQueryOperand)
	{
		try
		{
			LOG_DEBUG(Format("schemaQueryOperator: %1 schemaQueryOperand: %2", rch, schemaQueryOperator, schemaQueryOperand));
			std::ostringstream query;
			query << "SELECT \"*\" FROM meta_class WHERE " << schemaQueryOperand << " " << schemaQueryOperator << "  \"ClassWhichDoesNotExist\" ";
			testQuery(rch, query.str().c_str(), 0);
		}
		catch(CIMException const& ex)
		{
			//This query is supposed to throw an exception.
			LOG_DEBUG(Format("CIMException %1 caught, good.", ex.what()));
			return;
		}
		TEST_ASSERT(0);
	}
	void wrongOperator(CIMOMHandleIFCRef& rch, String const& schemaQueryOperator, String const& schemaQueryOperand)
	{
		LOG_DEBUG(Format("schemaQueryOperator: %1 schemaQueryOperand: %2", rch, schemaQueryOperator, schemaQueryOperand));
		try
		{
			std::ostringstream query;
			query << "SELECT \"*\" FROM meta_class WHERE " << schemaQueryOperand << " " << schemaQueryOperator << " \"ClassWhichDoesNotExist\" ";
			testQuery(rch, query.str().c_str(), 0);
		}
		catch(CIMException const& ex)
		{
			//This query is supposed to throw an exception.
			LOG_DEBUG(Format("CIMException %1 caught, good.", ex.what()));
			return;
		}
		LOG_DEBUG("CIMException not caught, bad.");
		TEST_ASSERT(0);
	}
	void noChildren(CIMOMHandleIFCRef& rch, String const& schemaQueryOperator, String const& schemaQueryOperand)
	{
		LOG_DEBUG(Format("schemaQueryOperator: %1 schemaQueryOperand: %2", rch, schemaQueryOperator, schemaQueryOperand));
		std::ostringstream query;
		query << "SELECT \"*\" FROM meta_class WHERE " << schemaQueryOperand << " " << schemaQueryOperator << " \"ClassWithNoChildren\" ";
		testQuery(rch, query.str().c_str(), 1);
	}
	namespace thisTests
	{
		void caseInsensitive(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			size_t const sz= 3;
			String thisCase[sz]=
				{String("__this"), String("__This"), String("__THIS")};

			for(size_t i= 0; i < sz ; ++i)
			{
				doesNotExist(rch, String("ISA"), thisCase[i]);
			}
		}

		void manyChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __this ISA \"ClassWithManyChildren\" ", 4);
		}
		
		void manyLevelsOfChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __this ISA \"ClassWithManyChildren2Level\" ", 13);
		}

		void root(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			//FIXME: differentiate this from manyLevelsOfChildren somehow.
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __this ISA \"ClassWithManyChildren2Level\" ", 13);
		}
		
		void notRoot(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __this ISA \"Child1OfClassWithManyChildren2Level\" ", 4);
		}
	}
	namespace classTests
	{
		void caseInsensitive(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			size_t const sz= 3;
			String thisCase[sz]=
				{String("__class"), String("__Class"), String("__CLASS")};

			for(size_t i= 0; i < sz ; ++i)
			{
				doesNotExist(rch, String("="), thisCase[i]);
			}
		}
		
		void noChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __class = \"ClassWithNoChildren\" ", 1);
		}

		void manyChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __class = \"ClassWithManyChildren\" ", 1);
		}
		
		void manyLevelsOfChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __class = \"ClassWithManyChildren2Level\" ", 1);
		}

		void notRoot(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __class = \"Child1OfClassWithManyChildren2Level\" ", 1);
		}
		
	}
	namespace dynastyTests
	{
		void caseInsensitive(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			size_t const sz= 3;
			String thisCase[sz]=
				{String("__dynasty"), String("__Dynasty"), String("__DYNASTY")};

			for(size_t i= 0; i < sz ; ++i)
			{
				doesNotExist(rch, String("="), thisCase[i]);
			}
		}
		
		void noChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __dynasty = \"ClassWithNoChildren\" ", 1);
		}

		void manyChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __dynasty = \"ClassWithManyChildren\" ", 4);
		}
		
		void manyLevelsOfChildren(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __dynasty = \"ClassWithManyChildren2Level\" ", 13);
		}

		void notRoot(CIMOMHandleIFCRef& rch)
		{
			LOG_DEBUG("");
			testQuery(rch, "SELECT \"*\" FROM meta_class WHERE __dynasty = \"Child1OfClassWithManyChildren2Level\" ", 0);
		}
		
	}
	void testThis(CIMOMHandleIFCRef& rch)
	{
		LOG_DEBUG("");
		thisTests::caseInsensitive(rch);
		doesNotExist(rch, "ISA", "__this");
		wrongOperator(rch, "=", "__this");
		//thisTests::withoutMetaClass(rch);
		noChildren(rch, "ISA", "__this");
		thisTests::manyChildren(rch);
		thisTests::manyLevelsOfChildren(rch);
		thisTests::notRoot(rch);
	}

	void testClass(CIMOMHandleIFCRef& rch)
	{
		LOG_DEBUG("");
		classTests::caseInsensitive(rch);
		doesNotExist(rch, "=", "__class");
		wrongOperator(rch, "ISA", "__class");
		noChildren(rch, "=", "__class");
		classTests::manyChildren(rch);
		classTests::manyLevelsOfChildren(rch);
		classTests::notRoot(rch);
	}

	void testDynasty(CIMOMHandleIFCRef& rch)
	{
		LOG_DEBUG("");
		dynastyTests::caseInsensitive(rch);
		doesNotExist(rch, "=", "__dynasty");
		wrongOperator(rch, "ISA", "__dynasty");
		noChildren(rch, "=", "__dynasty");
		dynastyTests::manyChildren(rch);
		dynastyTests::manyLevelsOfChildren(rch);
		//For __dynasty, notRoot *must* fail.
		try
		{
			dynastyTests::notRoot(rch);
		}
		catch(CIMException const& ex)
		{
			//This query is supposed to throw an exception.
			LOG_DEBUG(Format("CIMException %1 caught, good.", ex.what()));
			return;
		}
		LOG_DEBUG("CIMException not caught, bad.");
		TEST_ASSERT(0);
	}
}

void testSchemaQueries(CIMOMHandleIFCRef& rch)
{
	testSchemaQuery::addTestClasses(rch);
	testSchemaQuery::testThis(rch);
	testSchemaQuery::testClass(rch);
	testSchemaQuery::testDynasty(rch);
	testSchemaQuery::testClass(rch);
	testSchemaQuery::testDynasty(rch);
}

#endif //#ifndef OW_DISABLE_SCHEMA_MANIPULATION

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
		
		CIMOMHandleIFCRef rch = ClientCIMOMHandle::createFromURL(url);

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
		testQuery(rch, "select * from wqlTestClass where realData = 123.456789", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456789 = realData", 1);
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
		testQuery(rch, "select * from wqlTestClass where realData <> 123.456789", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456789 <> realData", 1);
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
		testQuery(rch, "select * from wqlTestClass where realData >= 123.456789", 2);
		testQuery(rch, "select * from wqlTestClass where 123.456789 >= realData", 1);
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
		testQuery(rch, "select * from wqlTestClass where realData <= 123.456789", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456789 <= realData", 2);
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
		testQuery(rch, "select * from wqlTestClass where realData > 123.456789", 1);
		testQuery(rch, "select * from wqlTestClass where 123.456789 > realData", 0);
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
		testQuery(rch, "select * from wqlTestClass where realData < 123.456789", 0);
		testQuery(rch, "select * from wqlTestClass where 123.456789 < realData", 1);
		testQuery(rch, "select * from wqlTestClass where __Path < \"wqlTestClass.name=\\\"test5\\\"\"", 5);
		testQuery(rch, "select * from wqlTestClass where \"wqlTestClass.name=\\\"test5\\\"\" < __Path", 4);

		// test conversion to string
		testQuery(rch, "select * from wqlTestClass where sint32Data <> \"0\"", 1);
		testQuery(rch, "select * from wqlTestClass where \"0\" <> sint32Data", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData <> \"true\"", 1);
		testQuery(rch, "select * from wqlTestClass where \"true\" <> booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where booleanData <> \"false\"", 1);
		testQuery(rch, "select * from wqlTestClass where \"false\" <> booleanData", 1);
		testQuery(rch, "select * from wqlTestClass where realData <> \"123.456789\"", 1);
		testQuery(rch, "select * from wqlTestClass where \"123.456789\" <> realData", 1);

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


		// test some simple schema queries.
#ifndef OW_DISABLE_SCHEMA_MANIPULATION
		testSchemaQueries(rch);
#endif // #ifndef OW_DISABLE_SCHEMA_MANIPULATION

		return 0;
	}
	catch(AssertionException& a)
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
