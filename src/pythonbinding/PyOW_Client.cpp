#include <boost/python/module.hpp>
#ifdef ANY
#undef ANY
#endif

void registerOW_String();
void registerOW_CIMClient();
void registerOW_CIMOMInfo();
void registerOW_Array();
void registerOW_CIMOMLocator();
void registerOW_ClientAuthCBIFC();

BOOST_PYTHON_MODULE(owclient)
{
    registerOW_String();
    registerOW_CIMClient();
    registerOW_CIMOMInfo();
    registerOW_Array();
    registerOW_CIMOMLocator();
    registerOW_ClientAuthCBIFC();
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
    //    MofCompiler
}

