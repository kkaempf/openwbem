#include <OW_CIM.hpp>
#include <boost/python.hpp>

using namespace boost::python;

 

struct OW_BoolToPython
{
    static PyObject* convert(OW_Bool const& x)
    {
        return Py_BuildValue("b", bool(x));
    }
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_tokenize_overloads, OW_String::tokenize, 0, 2)

BOOST_PYTHON_MODULE(owclient)
{
    class_<OW_String>("OW_String")
        .def(init<OW_Int32>())
        .def(init<OW_UInt32>())
        .def(init<OW_Int64>())
        .def(init<OW_UInt64>())
        .def(init<OW_Real64>())
        .def(init<const char*>())
        .def(init<const OW_Char16Array&>())
        .def(init<OW_Bool>())
        .def(init<const OW_Char16&>())
        .def(init<const OW_CIMDateTime&>())
        .def(init<const OW_CIMObjectPath&>())
        .def(init<OW_Bool, char*, size_t>())
        .def(init<const char*, size_t>())
        .def(init<const OW_String&>()) // need copy ctor?
// this seems to prevent the const char* ctor from working, and we don't
// really need it since a char and a char* are the same in python.
//        .def(init<char>())
//        .def("allocateCString", &OW_String::allocateCString)
        .def("length", &OW_String::length)
        .def("empty", &OW_String::empty)
//        .def("format", &OW_String::format)
        .def("tokenize", &OW_String::tokenize, OW_String_tokenize_overloads(args("delims", "returnTokens")))
        .def("c_str", &OW_String::c_str)
        .def("getBytes", &OW_String::getBytes)
        .def("charAt", &OW_String::charAt)
        .def("compareTo", &OW_String::compareTo)
        .def("compareToIgnoreCase", &OW_String::compareToIgnoreCase)
        .def("concat", &OW_String::concat, return_internal_reference<>())
        .def("endsWith", &OW_String::endsWith) // has default
        .def("equals", &OW_String::equals)
        .def("equalsIgnoreCase", &OW_String::equalsIgnoreCase)
        .def("hashCode", &OW_String::hashCode)
        .def("indexOf", (int (OW_String::*)(char, int=0) const)(&OW_String::indexOf)) // has default
        .def("indexOf", (int (OW_String::*)(const OW_String&, int) const)(&OW_String::indexOf)) // has default
        .def("lastIndexOf", (int (OW_String::*)(char, int) const)(&OW_String::lastIndexOf)) // has default
        .def("lastIndexOf", (int (OW_String::*)(const OW_String&, int) const)(&OW_String::lastIndexOf)) // has default
        .def("startsWith", &OW_String::startsWith) // has default
        .def("substring", &OW_String::substring) // has default
        .def("isSpaces", &OW_String::isSpaces)
        .def("toLowerCase", &OW_String::toLowerCase, return_internal_reference<>())
        .def("toUpperCase", &OW_String::toUpperCase, return_internal_reference<>())
        .def("ltrim", &OW_String::ltrim, return_internal_reference<>())
        .def("rtrim", &OW_String::rtrim, return_internal_reference<>())
        .def("trim", &OW_String::trim, return_internal_reference<>())
        .def("erase", (OW_String& (OW_String::*)())(&OW_String::erase), return_internal_reference<>())
        .def("erase", (OW_String& (OW_String::*)(size_t, size_t))(&OW_String::erase), return_internal_reference<>()) // has default
        // assignment operator
//        .def(self[size_t()])
        .def(self += self)
        .def("readObject", &OW_String::readObject)
        .def("writeObject", &OW_String::writeObject)
        .def("toString", &OW_String::toString)
        .def("toChar16", &OW_String::toChar16)
        .def("toReal32", &OW_String::toReal32)
        .def("toReal64", &OW_String::toReal64)
        .def("toBool", &OW_String::toBool)
        .def("toUInt8", &OW_String::toUInt8) // has default
        .def("toInt8", &OW_String::toInt8) // has default
        .def("toUInt16", &OW_String::toUInt16) // has default
        .def("toInt16", &OW_String::toInt16) // has default
        .def("toUInt32", &OW_String::toUInt32) // has default
        .def("toInt32", &OW_String::toInt32) // has default
        .def("toUInt64", &OW_String::toUInt64) // has default
        .def("toInt64", &OW_String::toInt64) // has default
        .def("toDateTime", &OW_String::toDateTime)
        .def("strtoull", &OW_String::strtoull)
        .def("strtoll", &OW_String::strtoll)
// ?        .def("strchr", &OW_String::strchr, return_internal_reference<1>())
        .def("getLine", &OW_String::getLine)
//        .def(str(self))
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

    to_python_converter<OW_Bool, OW_BoolToPython>();
// hmm, this doesn't seem to work    implicitly_convertible<char*,OW_String>();
}

