#include "AutoTest.hpp"
#include "TestRunner.hpp"

namespace TestNamespace
{
	namespace // anonymous
	{
		struct AutoTestList
		{
			const char* name;
			Test* test;
			AutoTestList* next;
		};

		AutoTestList g_AutoTests = { NULL, NULL, NULL };

		// Stick the first test into the given test runner.
		void popTest(TestRunner& runner)
		{
			AutoTestList* tl = g_AutoTests.next;

			if( tl )
			{
				runner.addTest( tl->name, tl->test );
				g_AutoTests.next = tl->next;
				delete tl;
			}
		}

		bool haveMoreTests()
		{
			return g_AutoTests.next != NULL;
		}

	} // end anonymous namespace


	void AddAutoTest(const char* name, Test* t)
	{
		AutoTestList* tl = new AutoTestList;
		tl->name = name;
		tl->test = t;
		tl->next = NULL;

		// Add to the end of the list.
		AutoTestList* current = &g_AutoTests;
		for(; current->next; current = current->next)
		{
		}
		current->next = tl;
	}

	int unitTestAutoMainImpl(int argc, const char** argv)
	{
		TestRunner runner;

		// Pull all the tests out of the global object and shove them in the TestRunner.
		while( haveMoreTests() )
		{
			popTest(runner);
		}

		if ( argc < 2 || ( argc == 2 && (::strcmp("all", argv[1]) == 0) ) )
		{
			runner.runAll();
		}
		else
		{
			runner.run( argv[1] );
		}
		return 0;
	}
} // end namespace TestNamespace
