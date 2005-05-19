#include "OW_CerrLogger.hpp"
#include "OW_Format.hpp"
#include "OW_TestResultAccumulator.hpp"

#include <ostream>

namespace OW_NAMESPACE
{
	TestResultAccumulator::TestResultAccumulator()
		:m_passes(0),m_failures(0){}
	TestResultAccumulator::TestResultAccumulator(bool b)
		:m_passes(b),m_failures(!b){}

	/* This class relies on the compiler-generated copy constructor and copy assignment operator.*/

	TestResultAccumulator& TestResultAccumulator::operator&=(bool rhs)
	{
		m_passes+= rhs;
		m_failures+= !rhs;
		return *this;
	}

	TestResultAccumulator& TestResultAccumulator::operator&=(TestResultAccumulator const& rhs)
	{
		m_passes+= rhs.passes();
		m_failures+= rhs.failures();
		return *this;
	}

	int TestResultAccumulator::passes()const{return m_passes;}
	int TestResultAccumulator::failures()const{return m_failures;}
	int TestResultAccumulator::total()const{return passes() + failures();}
	double TestResultAccumulator::ratio()const{return double(passes())/total();}

	::std::ostream& operator<<(::std::ostream& lhs, TestResultAccumulator const& rhs)
	{
		lhs << rhs.passes() << "/" << rhs.total() << " (" << rint(rhs.ratio()*100) << "%)";
		return lhs;
	}

	//FIXME: consider replacing this with a full set of logical ops.
	bool operator!(TestResultAccumulator const& rhs){return rhs.passes() != rhs.total();}

	TestLogger::TestLogger(::OpenWBEM::LoggerRef logger, TestResultAccumulator& testResult, char const* function)
		:logger(logger)
		 ,testResult(&testResult)
		 ,function(function)
	{
		LOG_DEBUG(::OpenWBEM::Format(" Starting test: %1 .", function));
	}

	TestLogger::~TestLogger()
	{
		LOG_DEBUG(::OpenWBEM::Format(" Ending test %1 with result %2", function, *testResult));
	}

	::OpenWBEM::LoggerRef createLogger()
	{
		//FIXME: make this use a logger for a specific file.  FIXME: Why
		//on earth do I have this extra layer of indirection? I think I
		//planned to do more complicated initialization of this later.
		static ::OpenWBEM::LoggerRef logger(new ::OpenWBEM::CerrLogger());
		return logger;
	}

	::OpenWBEM::LoggerRef getLogger()
	{
		static ::OpenWBEM::LoggerRef logger(createLogger());
		return logger;
	}
}

