#include <OW_CIMOMInfo.hpp>

// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

using namespace boost::python;

namespace {

void OW_CIMOMInfo_setitem_(OW_CIMOMInfo& ci, const OW_String& key, const OW_String& value)
{
    ci[key] = value;
}

OW_String OW_CIMOMInfo_getitem_(const OW_CIMOMInfo& ci, const OW_String& key)
{
    return ci[key];
}

}

void registerOW_CIMOMInfo()
{
    class_<OW_CIMOMInfo>("OW_CIMOMInfo")
        .def("getURL", &OW_CIMOMInfo::getURL)
        .def("__getitem__", &OW_CIMOMInfo_getitem_)
        .def("__setitem__", &OW_CIMOMInfo_setitem_)
        .def(self < self)
        .def(self == self)
        .def(self != self)
    ;
}

