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
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_endsWith_overloads, OW_String::endsWith, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_indexOf_overloads, OW_String::indexOf, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_lastIndexOf_overloads, OW_String::lastIndexOf, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_startsWith_overloads, OW_String::startsWith, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_substring_overloads, OW_String::substring, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_erase_overloads, OW_String::erase, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toUInt8_overloads, OW_String::toUInt8, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toInt8_overloads, OW_String::toInt8, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toUInt16_overloads, OW_String::toUInt16, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toInt16_overloads, OW_String::toInt16, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toUInt32_overloads, OW_String::toUInt32, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toInt32_overloads, OW_String::toInt32, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toUInt64_overloads, OW_String::toUInt64, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OW_String_toInt64_overloads, OW_String::toInt64, 0, 1)

OW_String OW_String_getslice(const OW_String& s, int i, int j)
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

PyObject* OW_String_repr(const OW_String& s)
{
    OW_String str("owclient.OW_String(\"" + s + "\")");
    return Py_BuildValue("s#", str.c_str(), str.length());
}

BOOST_PYTHON_MODULE(owclient)
{
    class_<OW_String>("OW_String")
        .def(init<OW_Int32>())
        .def(init<OW_UInt32>())
        .def(init<OW_Int64>())
        .def(init<OW_UInt64>())
        .def(init<OW_Real64>())
        .def(init<const char*>())
// Uncomment this to activate implicit string conversion
//        .def(init<const std::string&>())
        .def(init<const OW_Char16Array&>())
        .def(init<OW_Bool>())
        .def(init<const OW_Char16&>())
        .def(init<const OW_CIMDateTime&>())
        .def(init<const OW_CIMObjectPath&>())
        .def(init<OW_Bool, char*, size_t>())
        .def(init<const char*, size_t>())
// this seems to prevent the const char* ctor from working, and we don't
// really need it since a char and a char* are the same in python.
//        .def(init<char>())
        .def("length", &OW_String::length)
        .def("__len__", &OW_String::length)
        .def("empty", &OW_String::empty)
// Can't wrap variable argument functions
//        .def("format", &OW_String::format)
        .def("tokenize", &OW_String::tokenize, OW_String_tokenize_overloads(args("delims", "returnTokens")))
        .def("c_str", &OW_String::c_str)
        .def("getBytes", &OW_String::getBytes)
        .def("charAt", &OW_String::charAt)
        .def("compareTo", &OW_String::compareTo)
        .def("compareToIgnoreCase", &OW_String::compareToIgnoreCase)
        .def("concat", &OW_String::concat, return_internal_reference<>())
        .def("endsWith", &OW_String::endsWith, OW_String_endsWith_overloads(args("arg", "ignoreCase")))
        .def("equals", &OW_String::equals)
        .def("equalsIgnoreCase", &OW_String::equalsIgnoreCase)
        .def("hashCode", &OW_String::hashCode)
        .def("indexOf", (int (OW_String::*)(char, int) const)(&OW_String::indexOf), OW_String_indexOf_overloads(args("ch", "fromIndex")))
        .def("indexOf", (int (OW_String::*)(const OW_String&, int) const)(&OW_String::indexOf), OW_String_indexOf_overloads(args("arg", "fromIndex")))
        .def("lastIndexOf", (int (OW_String::*)(char, int) const)(&OW_String::lastIndexOf), OW_String_lastIndexOf_overloads(args("ch", "fromIndex")))
        .def("lastIndexOf", (int (OW_String::*)(const OW_String&, int) const)(&OW_String::lastIndexOf), OW_String_lastIndexOf_overloads(args("arg", "fromIndex")))
        .def("startsWith", &OW_String::startsWith, OW_String_startsWith_overloads(args("arg", "ignoreCase")))
        .def("substring", &OW_String::substring, OW_String_substring_overloads(args("beginIndex", "length")))
        .def("__getslice__", &OW_String_getslice)
        // TODO: add __setslice__ and __delslice__
        .def("isSpaces", &OW_String::isSpaces)
        .def("toLowerCase", &OW_String::toLowerCase, return_internal_reference<>())
        .def("toUpperCase", &OW_String::toUpperCase, return_internal_reference<>())
        .def("ltrim", &OW_String::ltrim, return_internal_reference<>())
        .def("rtrim", &OW_String::rtrim, return_internal_reference<>())
        .def("trim", &OW_String::trim, return_internal_reference<>())
        .def("erase", (OW_String& (OW_String::*)())(&OW_String::erase), return_internal_reference<>())
        .def("erase", (OW_String& (OW_String::*)(size_t, size_t))(&OW_String::erase), OW_String_erase_overloads(args("idx", "len"))[return_internal_reference<>()])
        .def("__getitem__", &OW_String::operator[])
        .def(self += self)
        .def("readObject", &OW_String::readObject)
        .def("writeObject", &OW_String::writeObject)
        .def("toString", &OW_String::toString)
        .def("toChar16", &OW_String::toChar16)
        .def("toReal32", &OW_String::toReal32)
        .def("toReal64", &OW_String::toReal64)
        .def("toBool", &OW_String::toBool)
        .def("toUInt8", &OW_String::toUInt8, OW_String_toUInt8_overloads(args("base")))
        .def("toInt8", &OW_String::toInt8, OW_String_toInt8_overloads(args("base")))
        .def("toUInt16", &OW_String::toUInt16, OW_String_toUInt16_overloads(args("base")))
        .def("toInt16", &OW_String::toInt16, OW_String_toInt16_overloads(args("base")))
        .def("toUInt32", &OW_String::toUInt32, OW_String_toUInt32_overloads(args("base")))
        .def("toInt32", &OW_String::toInt32, OW_String_toInt32_overloads(args("base")))
        .def("toUInt64", &OW_String::toUInt64, OW_String_toUInt64_overloads(args("base")))
        .def("toInt64", &OW_String::toInt64, OW_String_toInt64_overloads(args("base")))
        .def("toDateTime", &OW_String::toDateTime)
        .def("strtoull", &OW_String::strtoull)
        .def("strtoll", &OW_String::strtoll)
        .def("getLine", &OW_String::getLine)
        .def(self_ns::str(self))
        .def("__repr__", &OW_String_repr)
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
//    To make a python string implicitly convertible to OW_String, add a
//    constructor to OW_String that takes a std::string, and then uncomment
//    this.
//    implicitly_convertible<std::string, OW_String>();
    //
    //
    //    OW_CIMClient
    //
    //    OW_CIMOMInfo
    //
    //    OW_CIMOMLocator
    //
    //    OW_ClientAuthCBIFC
    //
    //    OW_CIMClass
    //
    //    OW_CIMDataType
    //
    //    OW_CIMDateTime
    //
    //    OW_CIMException
    //
    //    OW_CIMFlavor
    //
    //    OW_CIMInstance
    //
    //    OW_CIMMethod
    //
    //    OW_CIMNameSpace ???
    //
    //    OW_CIMObjectPath
    //
    //    OW_CIMParamValue
    //
    //    OW_CIMParameter
    //
    //    OW_CIMProperty
    //
    //    OW_CIMQualifier
    //
    //    OW_CIMQualifierType
    //
    //    OW_CIMScope
    //
    //    OW_CIMValue
    //
    //    OW_CIMValueCast
    //
    //    OW_StringArray
    //
    //    OW_StringResultHandlerIFC
    //
    //    OW_CIMClassResultHandlerIFC
    //
    //    OW_CIMClassEnumeration
    //
    //    OW_CIMObjectPathResultHandlerIFC
    //
    //    OW_CIMObjectPathEnumeration
    //
    //    OW_CIMInstanceResultHandlerIFC
    //
    //    OW_CIMInstanceEnumeration
    //
    //    OW_CIMQualifierTypeResultHandlerIFC
    //
    //    OW_CIMQualifierTypeEnumeration
    //
    //    OW_CIMParamValueArray
    //
    //
}

