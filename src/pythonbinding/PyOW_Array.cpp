#include <OW_String.hpp>
#include <OW_Array.hpp>

// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

using namespace boost::python;

template <typename T>
void registerOW_ArrayImpl(const char* className)
{
    typedef typename OW_Array<T>::iterator iter_t;
    typedef typename OW_Array<T>::const_iterator const_iter_t;
    typedef typename OW_Array<T>::size_type size_type;
    class_<OW_Array<T> >(className)
        .def(init<size_type, const T&>())
        .def(init<int, const T&>())
        .def(init<long, const T&>())
        .def(init<size_type>())
        // these call the template constructor.  Since we have to instantiate
        // each type we want to expose, let's not go overboard!
        .def(init<iter_t, iter_t>())
        .def(init<const_iter_t, const_iter_t>())
        .def("begin", (iter_t (OW_Array<T>::*)())(&OW_Array<T>::begin))
        .def("end", (iter_t (OW_Array<T>::*)())(&OW_Array<T>::end))
        .def("__iter__", iterator<OW_Array<T> >())

    ;
}


void registerOW_Array()
{
    registerOW_ArrayImpl<OW_String>("OW_StringArray");
    //    OW_Array template instantiated for:
    //      OW_String
    //      OW_CIMOMLocator
    //      OW_CIMClass
    //      OW_CIMDataType
    //      OW_CIMDateTime
    //      OW_CIMFlavor
    //      OW_CIMInstance
    //      OW_CIMMethod
    //      OW_CIMObjectPath
    //      OW_CIMParamValue
    //      OW_CIMParameter
    //      OW_CIMProperty
    //      OW_CIMQualifier
    //      OW_CIMQualifierType
    //      OW_CIMScope
    //      OW_CIMValue
}

