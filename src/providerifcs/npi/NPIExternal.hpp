/*
 * NPIExternal.h
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Markus Mueller <markus_mueller@de.ibm.com>
 * Contributors:
 *
 * Description: <Some descriptive text>
 */

// administrative functions
#include "NPIProvider.hpp"

extern "C" PEGASUS_EXPORT
    NPIenv * CreateEnv(Pegasus::CIMRepository * repository,
                       const Pegasus::String& nameSpace);

extern "C" PEGASUS_EXPORT
    void DeleteEnv(NPIenv * npienv);

extern "C" PEGASUS_EXPORT Pegasus::CIMClass NPI_getmyClass(
    NPIHandle * npiHandle,
    const Pegasus::String& nameSpace, const Pegasus::String& className);

extern "C" PEGASUS_EXPORT Pegasus::Array<Pegasus::CIMReference>
    NPI_enumeratemyInstanceNames(
    NPIHandle * npiHandle,
    const Pegasus::String& nameSpace, const Pegasus::String& className);

extern "C" PEGASUS_EXPORT Pegasus::Array<Pegasus::CIMNamedInstance>
    NPI_enumeratemyInstances(
    NPIHandle * npiHandle,
    const Pegasus::String& nameSpace, const Pegasus::String& className);

//Vector functions 
extern "C" PEGASUS_EXPORT Vector VectorNew ( NPIHandle * npiHandle);
extern "C" PEGASUS_EXPORT 
    void _VectorAddTo ( NPIHandle * npiHandle,Vector v, void * obj);
extern "C" PEGASUS_EXPORT 
    int VectorSize( NPIHandle * npiHandle, Vector v);
extern "C" PEGASUS_EXPORT 
    void * _VectorGet( NPIHandle * npiHandle, Vector v, int pos);

//CIMValue functions
extern "C" PEGASUS_EXPORT
    ::CIMType CIMValueGetType (NPIHandle * npiHandle, ::CIMValue cv);
extern "C" PEGASUS_EXPORT
    ::CIMValue CIMValueNewString (NPIHandle * npiHandle, const char * val);
extern "C" PEGASUS_EXPORT
    ::CIMValue CIMValueNewInteger (NPIHandle * npiHandle, int val);
extern "C" PEGASUS_EXPORT
    ::CIMValue CIMValueNewRef (NPIHandle * npiHandle, ::CIMObjectPath cop);
extern "C" PEGASUS_EXPORT
    char * CIMValueGetString (NPIHandle * npiHandle, ::CIMValue cv);
extern "C" PEGASUS_EXPORT
    int CIMValueGetInteger(NPIHandle * npiHandle, ::CIMValue cv);
extern "C" PEGASUS_EXPORT
    ::CIMObjectPath CIMValueGetRef(NPIHandle * npiHandle, ::CIMValue cv);

//CIMParameter functions
extern "C" PEGASUS_EXPORT
    ::CIMType CIMParameterGetType( NPIHandle * npiHandle, ::CIMParameter cp);
extern "C" PEGASUS_EXPORT
    char * CIMParameterGetName( NPIHandle * npiHandle, ::CIMParameter cp);
extern "C" PEGASUS_EXPORT
    ::CIMParameter CIMParameterNewString (NPIHandle * npiHandle,
            const char * name, char * value);
extern "C" PEGASUS_EXPORT
    ::CIMParameter CIMParameterNewInteger ( NPIHandle * npiHandle,
            const char * name, int value);
extern "C" PEGASUS_EXPORT
    ::CIMParameter CIMParameterNewRef ( NPIHandle * npiHandle,
            const char * name, CIMObjectPath value);
extern "C" PEGASUS_EXPORT
    char * CIMParameterGetStringValue ( NPIHandle * npiHandle,
              ::CIMParameter cp);
extern "C" PEGASUS_EXPORT
    int CIMParameterGetIntegerValue ( NPIHandle * npiHandle,
              ::CIMParameter cp);
extern "C" PEGASUS_EXPORT
    CIMObjectPath CIMParameterGetRefValue ( NPIHandle * npiHandle,
              ::CIMParameter cp);

//CIMInstance functions
extern "C" PEGASUS_EXPORT 
    ::CIMInstance CIMClassNewInstance ( NPIHandle * npiHandle, ::CIMClass cc );
extern "C" PEGASUS_EXPORT 
    void CIMInstanceSetStringProperty( NPIHandle * npiHandle, ::CIMInstance ci,
                                       const char * name, const char * value );
extern "C" PEGASUS_EXPORT 
    void CIMInstanceSetIntegerProperty( NPIHandle * npiHandle,::CIMInstance ci,
                                       const char * name, const int value);
extern "C" PEGASUS_EXPORT
  void CIMInstanceSetLongProperty ( NPIHandle *, ::CIMInstance, const char *,
                                   const long long );
extern "C" PEGASUS_EXPORT
  void CIMInstanceSetBooleanProperty ( NPIHandle *, ::CIMInstance,
                                      const char *, const unsigned char );
extern "C" PEGASUS_EXPORT 
    void CIMInstanceSetRefProperty( NPIHandle * npiHandle, ::CIMInstance ci,
                                     const char * name, ::CIMObjectPath value);
extern "C" PEGASUS_EXPORT
char * CIMInstanceGetStringValue(NPIHandle * npiHandle,
                                 ::CIMInstance ci, const char * name);
extern "C" PEGASUS_EXPORT
int CIMInstanceGetIntegerValue(NPIHandle * npiHandle,
                                 ::CIMInstance ci, const char * name);
extern "C" PEGASUS_EXPORT
::CIMObjectPath CIMInstanceGetRefValue(NPIHandle * npiHandle,
                                       CIMInstance ci, const char * name);

// Object path functions
extern "C" PEGASUS_EXPORT 
    ::CIMObjectPath CIMObjectPathNew ( NPIHandle * npiHandle,
                                       const char * classname );
extern "C" PEGASUS_EXPORT 
    void CIMObjectPathDel ( NPIHandle * npiHandle,
                                       ::CIMObjectPath cop);
//extern "C" PEGASUS_EXPORT 
//    ::CIMObjectPath CIMObjectPathNew ( NPIHandle * npiHandle,
//                                       ::CIMInstance ci);
extern "C" PEGASUS_EXPORT 
    ::CIMObjectPath CIMObjectPathFromCIMInstance
                             ( NPIHandle * npiHandle, ::CIMInstance ci);
extern "C" PEGASUS_EXPORT 
    char * CIMObjectPathGetClassName( NPIHandle * npiHandle,
                                       ::CIMObjectPath cop);
extern "C" PEGASUS_EXPORT 
    char * CIMObjectPathGetNameSpace( NPIHandle * npiHandle,
                                       ::CIMObjectPath cop);
extern "C" PEGASUS_EXPORT 
    void CIMObjectPathSetNameSpace( NPIHandle * npiHandle,
                                     ::CIMObjectPath cop, const char * str);
extern "C" PEGASUS_EXPORT 
    void CIMObjectPathSetNameSpaceFromCIMObjectPath ( NPIHandle * npiHandle,
                                     ::CIMObjectPath cop, ::CIMObjectPath src);
extern "C" PEGASUS_EXPORT 
    char * CIMObjectPathGetStringKeyValue (NPIHandle * npiHandle,
                                        ::CIMObjectPath cop, const char * key);
extern "C" PEGASUS_EXPORT
    void CIMObjectPathAddStringKeyValue (NPIHandle * npiHandle,
                                         ::CIMObjectPath cop,
                                         const char * key, const char * value);
extern "C" PEGASUS_EXPORT 
    int CIMObjectPathGetIntegerKeyValue( NPIHandle * npiHandle,
                                        ::CIMObjectPath cop, const char * key);
extern "C" PEGASUS_EXPORT
    void CIMObjectPathAddIntegerKeyValue (NPIHandle * npiHandle,
                                         ::CIMObjectPath cop,
                                         const char * key, const int value);
extern "C" PEGASUS_EXPORT 
    ::CIMObjectPath CIMObjectPathGetRefKeyValue( NPIHandle * npiHandle, 
                                        ::CIMObjectPath cop, const char * key);
extern "C" PEGASUS_EXPORT
    void CIMObjectPathAddRefKeyValue (NPIHandle * npiHandle,
                                      ::CIMObjectPath cop,
                                      const char * key, ::CIMObjectPath cop2);

// CIMOM functions

extern "C" PEGASUS_EXPORT 
    ::CIMClass CIMOMGetClass( NPIHandle * npiHandle,
                             ::CIMObjectPath cop, int localOnly );
extern "C" PEGASUS_EXPORT 
    ::Vector CIMOMEnumInstanceNames(NPIHandle * npiHandle,
                                    ::CIMObjectPath cop, int i);
extern "C" PEGASUS_EXPORT 
    ::Vector CIMOMEnumInstances(NPIHandle * npiHandle,
                                    ::CIMObjectPath cop, int i, int j);
extern "C" PEGASUS_EXPORT 
    ::CIMInstance CIMOMGetInstance(NPIHandle * npiHandle,
                              ::CIMObjectPath cop, int i);

// Error handling classes

extern "C" PEGASUS_EXPORT 
    int errorCheck ( NPIHandle * npiHandle );

extern "C" PEGASUS_EXPORT 
    void raiseError ( NPIHandle * npiHandle, const char * msg );

extern "C" PEGASUS_EXPORT
char * _ObjectToString(NPIHandle * npiHandle, void * co);

/* PegasusProvider_Provider_h */
