#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

using namespace boost::python;

 
void registerOW_String();
void registerOW_CIMClient();

BOOST_PYTHON_MODULE(owclient)
{
    registerOW_String();
    registerOW_CIMClient();

    //
    //    OW_CIMOMInfo
    //
    //    OW_CIMOMLocator
    //
    //    OW_ClientAuthCBIFC ?
    //    OW_ClientAuthCBIFCRef
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
    //    MofCompiler
}

