#ifndef TESTRESULTACCUMULATOR_HPP
#define TESTRESULTACCUMULATOR_HPP

#include "OW_Logger.hpp"

#define LOG_DEBUG(X) OW_LOG_DEBUG(logger, (X))
#define LOG_ERROR(X) OW_LOG_ERROR(logger, (X))

//FIXME.
#define LOG_TEST TestLogger test_logger_dont_use_this_name(::OW_NAMESPACE::getLogger(), testResult, __FUNCTION__);

namespace OW_NAMESPACE
{
	struct TestResultAccumulator
	{
		TestResultAccumulator();
		explicit TestResultAccumulator(bool b);

		/* This class relies on the compiler-generated copy constructor and copy assignment operator.*/

		TestResultAccumulator& operator&=(bool rhs);
		TestResultAccumulator& operator&=(TestResultAccumulator const& rhs);

		int passes()const;
		int failures()const;
		int total()const;
		double ratio()const;

	private:
		int m_passes;
		int m_failures;
	};

	::std::ostream& operator<<(::std::ostream& lhs, TestResultAccumulator const& rhs);

	//FIXME: consider replacing this with a full set of logical ops.
	bool operator!(TestResultAccumulator const& rhs);

	struct TestLogger
	{
		TestLogger(::OpenWBEM::LoggerRef logger, TestResultAccumulator& testResult, char const* function);
		~TestLogger();
	private:
		::OpenWBEM::LoggerRef logger;
		TestResultAccumulator const* testResult;
		char const* function;
	};

	::OpenWBEM::LoggerRef createLogger();
	::OpenWBEM::LoggerRef getLogger();

	template<typename Test>
		TestResultAccumulator testWrapper(Test t)
	{
		TestResultAccumulator testResult;
		try
		{
			testResult&= t();
		}
		//FIXME:... or maybe not.
		catch(...)
		{
			testResult&= false;
		}
		return testResult;
	}
}

#endif
//TESTRESULTACCUMULATOR_HPP
