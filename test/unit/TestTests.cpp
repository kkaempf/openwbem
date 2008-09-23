// This file is a very simple example of how an auto unit test can be
// created.  This is a complete test and does not need to have extra
// items added to UnitMain.cpp or have extra class members defined.
//
// A limitation of these auto unit tests is that there is no setup or
// teardown code before/after your tests, so what you write is what
// you have.

#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"

AUTO_UNIT_TEST(SimpleAssert)
{
	unitAssert(1 > 0);
}
