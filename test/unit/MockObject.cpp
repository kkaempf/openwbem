#include "MockObject.hpp"
#include "OW_Exception.hpp"
#include "OW_String.hpp"
#include <exception>

using namespace OpenWBEM;
using namespace std;

namespace
{
	// We need to make sure that this is something that would not
	// normally be caught, so we do not derive from OpenWBEM::Exception.
	//
	struct MockException : exception
	{
		MockException(String const & message)
		: m_message(message)
		{
		}

		virtual ~MockException() throw()
		{
		}

		virtual char const * what() const throw()
		{
			return m_message.c_str();
		}

	private:
		String m_message;
	};
}

MockObject::MockObject()
: m_assertFailed(false),
  m_failureMessage()
{
}

MockObject::~MockObject()
{
}

void MockObject::mockAssert(bool test, String const & message)
{
	if (!test)
	{
		m_assertFailed = true;
		m_failureMessage = message;
		throw MockException(message);
	}
}

void MockObject::verify()
{
	if (m_assertFailed)
	{
		// mockAssert threw an exception but it was caught before we
		// reached the end of the test case, and hence not logged as
		// an error.  We report the error again here.
		//
		throw MockException(
			"Detected absorbed MockException: " + m_failureMessage);
	}
	this->doVerify();
}

void MockObject::doVerify()
{
}
