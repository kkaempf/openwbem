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
#include <OW_String.hpp>
#include <OW_Array.hpp>
#include <OW_CIMDateTime.hpp>
#include <OW_Char16.hpp>
#include <OW_CIMObjectPath.hpp>
// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

namespace OpenWBEM
{

using namespace boost::python;
 
namespace {
struct BoolToPython
{
	static PyObject* convert(Bool const& x)
	{
		return Py_BuildValue("b", bool(x));
	}
};
// String
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_tokenize_overloads, String::tokenize, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_endsWith_overloads, String::endsWith, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_indexOf_overloads, String::indexOf, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_lastIndexOf_overloads, String::lastIndexOf, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_startsWith_overloads, String::startsWith, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_substring_overloads, String::substring, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_erase_overloads, String::erase, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toUInt8_overloads, String::toUInt8, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toInt8_overloads, String::toInt8, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toUInt16_overloads, String::toUInt16, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toInt16_overloads, String::toInt16, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toUInt32_overloads, String::toUInt32, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toInt32_overloads, String::toInt32, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toUInt64_overloads, String::toUInt64, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(String_toInt64_overloads, String::toInt64, 0, 1)
String String_getslice(const String& s, int i, int j)
{
	int len = s.length();
	if (i < 0)
	{
		i = 0;
	}
	if (j < 0)
	{
		j = 0;
	}
	if (i > j)
		return "";
	return s.substring(i, j - i);
}
PyObject* String_repr(const String& s)
{
	String str("owclient.String(\"" + s + "\")");
	return Py_BuildValue("s#", str.c_str(), str.length());
}
int String_count(const String& s, char c)
{
	int rval = 0;
	for (size_t i = 0; i < s.length(); ++i)
	{
		if (s[i] == c)
			++rval;
	}
	return rval;
}
void String_append(String& s, char c)
{
	s.concat(c);
}
int String_index(const String& s, char c)
{
	for (size_t i = 0; i < s.length(); ++i)
	{
		if (s[i] == c)
			return i;
	}
	// Raise ValueError
	PyErr_SetString(PyExc_ValueError,
			"String.index(x): x not in list");
	boost::python::throw_error_already_set();
}
void String_insert(String& s, int i, char x)
{
	size_t length = s.length();
	if (i < 0)
	{
		i = 0;
	}
	s = s.substring(0, i) + String(x) + s.substring(i);
}
char String_pop(String& s, int i = -1)
{
	size_t length = s.length();
	while (true)
	{
		if (i < 0)
		{
			i += length;
		}
		else if (i >= length)
		{
			i -= length;
		}
		else
		{
			break;
		}
	}
	char rval = s[i];
	s = s.substring(0, i) + s.substring(i+1);
	return rval;
}
BOOST_PYTHON_FUNCTION_OVERLOADS(String_pop_overloads, String_pop, 1, 2)
void String_remove(String& s, char x)
{
	int i = String_index(s, x);
	s = s.substring(0, i) + s.substring(i+1);
}
void String_reverse(String& s)
{
	std::reverse(&s[0], &s[s.length()]);
}
void String_sort(String& s)
{
	std::sort(&s[0], &s[s.length()]);
}
char String_getitem_(const String& s, int i)
{
	if (i < 0 || i >= s.length())
	{
		// raise IndexError
		PyErr_SetString(PyExc_IndexError,
				"String index out of range");
		boost::python::throw_error_already_set();
	}
	return s[i];
}
void String_setitem_(String& s, int i, char c)
{
	if (i < 0 || i >= s.length())
	{
		// raise IndexError
		PyErr_SetString(PyExc_IndexError,
				"String index out of range");
		boost::python::throw_error_already_set();
	}
	s[i] = c;
}
void String_delitem_(String& s, int i)
{
	if (i < 0 || i >= s.length())
	{
		// raise IndexError
		PyErr_SetString(PyExc_IndexError,
				"String index out of range");
		boost::python::throw_error_already_set();
	}
	s = s.substring(0, i) + s.substring(i+1);
}
}
void registerString()
{
	class_<String>("String")
		.def(init<Int32>())
		.def(init<UInt32>())
		.def(init<Int64>())
		.def(init<UInt64>())
		.def(init<Real64>())
		.def(init<const char*>())
// Uncomment this to activate implicit string conversion
//        .def(init<const std::string&>())
		.def(init<const Char16Array&>())
		.def(init<Bool>())
		.def(init<const Char16&>())
		.def(init<const CIMDateTime&>())
		.def(init<const CIMObjectPath&>())
		.def(init<Bool, char*, size_t>())
		.def(init<const char*, size_t>())
// this seems to prevent the const char* ctor from working, and we don't
// really need it since a char and a char* are the same in python.
//        .def(init<char>())
		.def("length", &String::length)
		.def("__len__", &String::length)
		.def("empty", &String::empty)
// Can't wrap variable argument functions
//        .def("format", &String::format)
		.def("tokenize", &String::tokenize, String_tokenize_overloads(args("delims", "returnTokens")))
		.def("c_str", &String::c_str)
		.def("getBytes", &String::getBytes)
		.def("charAt", &String::charAt)
		.def("compareTo", &String::compareTo)
		.def("compareToIgnoreCase", &String::compareToIgnoreCase)
		.def("concat", (String& (String::*)(const String&))(&String::concat), return_internal_reference<>())
		.def("endsWith", &String::endsWith, String_endsWith_overloads(args("arg", "ignoreCase")))
		.def("equals", &String::equals)
		.def("equalsIgnoreCase", &String::equalsIgnoreCase)
		.def("hashCode", &String::hashCode)
		.def("indexOf", (int (String::*)(char, int) const)(&String::indexOf), String_indexOf_overloads(args("ch", "fromIndex")))
		.def("indexOf", (int (String::*)(const String&, int) const)(&String::indexOf), String_indexOf_overloads(args("arg", "fromIndex")))
		.def("lastIndexOf", (int (String::*)(char, int) const)(&String::lastIndexOf), String_lastIndexOf_overloads(args("ch", "fromIndex")))
		.def("lastIndexOf", (int (String::*)(const String&, int) const)(&String::lastIndexOf), String_lastIndexOf_overloads(args("arg", "fromIndex")))
		.def("startsWith", &String::startsWith, String_startsWith_overloads(args("arg", "ignoreCase")))
		.def("substring", &String::substring, String_substring_overloads(args("beginIndex", "length")))
		// The next few methods are to support enumlating a python container type
		.def("__getslice__", &String_getslice)
		// TODO: add __setslice__ and __delslice__ 
		.def("append", &String_append)
		.def("count", &String_count)
		.def("index", &String_index)
		.def("insert", &String_insert)
		.def("pop", &String_pop, String_pop_overloads(args("i")))
		.def("remove", &String_remove)
		.def("reverse", &String_reverse)
		.def("sort", &String_sort)
		.def("isSpaces", &String::isSpaces)
		.def("toLowerCase", &String::toLowerCase, return_internal_reference<>())
		.def("toUpperCase", &String::toUpperCase, return_internal_reference<>())
		.def("ltrim", &String::ltrim, return_internal_reference<>())
		.def("rtrim", &String::rtrim, return_internal_reference<>())
		.def("trim", &String::trim, return_internal_reference<>())
		.def("erase", (String& (String::*)())(&String::erase), return_internal_reference<>())
		.def("erase", (String& (String::*)(size_t, size_t))(&String::erase), String_erase_overloads(args("idx", "len"))[return_internal_reference<>()])
		.def("__getitem__", &String_getitem_)
		.def("__setitem__", &String_setitem_)
		.def("__delitem__", &String_delitem_)
		.def(self += self)
		.def("readObject", &String::readObject)
		.def("writeObject", &String::writeObject)
		.def("toString", &String::toString)
		.def("toChar16", &String::toChar16)
		.def("toReal32", &String::toReal32)
		.def("toReal64", &String::toReal64)
		.def("toBool", &String::toBool)
		.def("toUInt8", &String::toUInt8, String_toUInt8_overloads(args("base")))
		.def("toInt8", &String::toInt8, String_toInt8_overloads(args("base")))
		.def("toUInt16", &String::toUInt16, String_toUInt16_overloads(args("base")))
		.def("toInt16", &String::toInt16, String_toInt16_overloads(args("base")))
		.def("toUInt32", &String::toUInt32, String_toUInt32_overloads(args("base")))
		.def("toInt32", &String::toInt32, String_toInt32_overloads(args("base")))
		.def("toUInt64", &String::toUInt64, String_toUInt64_overloads(args("base")))
		.def("toInt64", &String::toInt64, String_toInt64_overloads(args("base")))
		.def("toDateTime", &String::toDateTime)
		.def("strtoull", &String::strtoull)
		.def("strtoll", &String::strtoll)
		.def("getLine", &String::getLine)
		.def(self_ns::str(self))
		.def("__repr__", &String_repr)
		.def(self + self)
		.def((const char*)0 + self)
		.def(self + (const char*)0)
		.def(char() + self)
		.def(self + char())
		.def(self == self)
		.def(self == (const char*)0)
		.def((const char*)0 == self)
		.def(self != self)
		.def(self != (const char*)0)
		.def((const char*)0 != self)
		.def(self < self)
		.def(self < (const char*)0)
		.def((const char*)0 < self)
		.def(self <= self)
		.def(self <= (const char*)0)
		.def((const char*)0 <= self)
		.def(self > self)
		.def(self > (const char*)0)
		.def((const char*)0 > self)
		.def(self >= self)
		.def(self >= (const char*)0)
		.def((const char*)0 >= self)
		;
	to_python_converter<Bool, BoolToPython>();
//    To make a python string implicitly convertible to String, add a
//    constructor to String that takes a std::string, and then uncomment
//    this.
//    implicitly_convertible<std::string, String>();
}

} // end namespace OpenWBEM

