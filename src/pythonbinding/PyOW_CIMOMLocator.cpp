#include <OW_CIMOMLocator.hpp>

// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

using namespace boost::python;

void registerOW_CIMOMLocator()
{
    class_<OW_CIMOMLocator, boost::noncopyable>("OW_CIMOMLocator", no_init)
        .def("findCIMOMs", &OW_CIMOMLocator::findCIMOMs)
    ;

    def("createCIMOMLocator", &OW_CIMOMLocator::createCIMOMLocator);
}

