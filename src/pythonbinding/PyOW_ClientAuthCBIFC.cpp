#include <OW_ClientAuthCBIFC.hpp>

// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

using namespace boost::python;

namespace {

struct OW_ClientAuthCBIFCWrap : OW_ClientAuthCBIFC
{
    OW_ClientAuthCBIFCWrap(PyObject* self_)
        : self(self_) {}
    OW_Bool getCredentials(const OW_String& realm, OW_String& name,
                        OW_String& passwd, const OW_String& details)
    { 
        return call_method<int>(self, "getCredentials", 
                realm, name, passwd, details); 
    }
    PyObject* self;
};

}

void registerOW_ClientAuthCBIFC()
{
    class_<OW_ClientAuthCBIFC, OW_ClientAuthCBIFCWrap, boost::noncopyable>
        ("OW_ClientAuthCBIFC", no_init)
       // .def("getCredentials", &OW_ClientAuthCBIFC::getCredentials)
    ;

}

