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
#ifndef NPIExternal_h
#define NPIExternal_h

#include "NPIProvider.hpp"

        class OW_CIMParamValue
        {
        public:
                OW_CIMParamValue() : m_name(), m_value() {}
                OW_CIMParamValue(const OW_String& name, const OW_CIMValue& value)
                        : m_name(name), m_value(value) {}
                OW_CIMParamValue(const OW_CIMParamValue& arg)
                        : m_name(arg.m_name), m_value(arg.m_value) {}

                const OW_CIMParamValue& operator= (const OW_CIMParamValue& arg)
                {
                        m_name = arg.m_name;
                        m_value = arg.m_value;
                        return *this;
                }

                OW_String getName() const { return m_name; }
                OW_CIMValue getValue() const { return m_value; }
                void setName(const OW_String& name) { m_name = name; }
                void setValue(const OW_CIMValue& value) { m_value = value; }

        private:
                OW_String m_name;
                OW_CIMValue m_value;
        };


extern "C" NPIenv*
createEnv(OW_CIMOMHandleIFCRef repository, const OW_String& nameSpace);

extern "C"
void deleteEnv(NPIenv* npienv);

extern "C"  OW_CIMClass NPI_getmyClass(
    NPIHandle* npiHandle,
    const OW_String& nameSpace, const OW_String& className);

//extern "C"  OW_Array<OW_CIMObjectPath>
extern "C" OW_CIMObjectPathEnumeration
    NPI_enumeratemyInstanceNames(
    NPIHandle* npiHandle,
    const OW_String& nameSpace, const OW_String& className);

//extern "C"  OW_Array<OW_CIMInstance>
extern "C" OW_CIMInstanceEnumeration
    NPI_enumeratemyInstances(
    NPIHandle* npiHandle,
    const OW_String& nameSpace, const OW_String& className);

extern "C" OW_CIMInstance
    NPI_getmyInstance(
    NPIHandle* npiHandle,
    const OW_CIMObjectPath & owcop, const int localOnly);

//Vector functions
extern "C"  Vector VectorNew ( NPIHandle* npiHandle);
extern "C"
    void _VectorAddTo ( NPIHandle* npiHandle,Vector v, void* obj);
extern "C"
    int VectorSize( NPIHandle* npiHandle, Vector v);
extern "C"
    void* _VectorGet( NPIHandle* npiHandle, Vector v, int pos);

//CIMValue functions
extern "C"
    ::CIMType CIMValueGetType (NPIHandle* npiHandle, ::CIMValue cv);
extern "C"
    ::CIMValue CIMValueNewString (NPIHandle* npiHandle, const char* val);
extern "C"
    ::CIMValue CIMValueNewInteger (NPIHandle* npiHandle, int val);
extern "C"
    ::CIMValue CIMValueNewRef (NPIHandle* npiHandle, ::CIMObjectPath cop);
extern "C"
    char* CIMValueGetString (NPIHandle* npiHandle, ::CIMValue cv);
extern "C"
    int CIMValueGetInteger(NPIHandle* npiHandle, ::CIMValue cv);
extern "C"
    ::CIMObjectPath CIMValueGetRef(NPIHandle* npiHandle, ::CIMValue cv);

//CIMParameter functions
extern "C"
    ::CIMType CIMParameterGetType( NPIHandle* npiHandle, ::CIMParameter cp);
extern "C"
    char* CIMParameterGetName( NPIHandle* npiHandle, ::CIMParameter cp);
extern "C"
    ::CIMParameter CIMParameterNewString (NPIHandle* npiHandle,
            const char* name, const char* value);
extern "C"
    ::CIMParameter CIMParameterNewInteger ( NPIHandle* npiHandle,
            const char* name, int value);
extern "C"
    ::CIMParameter CIMParameterNewRef ( NPIHandle* npiHandle,
            const char* name, CIMObjectPath value);
extern "C"
    char* CIMParameterGetString ( NPIHandle* npiHandle,
              ::CIMParameter cp);
extern "C"
    int CIMParameterGetIntegerValue ( NPIHandle* npiHandle,
              ::CIMParameter cp);
extern "C"
    CIMObjectPath CIMParameterGetRefValue ( NPIHandle* npiHandle,
              ::CIMParameter cp);

//CIMInstance functions
extern "C"
    ::CIMInstance CIMClassNewInstance ( NPIHandle* npiHandle, ::CIMClass cc );
extern "C"
    void CIMInstanceSetStringProperty( NPIHandle* npiHandle, ::CIMInstance ci,
                                       const char* name, const char* value );
extern "C"
    void CIMInstanceSetIntegerProperty( NPIHandle* npiHandle,::CIMInstance ci,
                                       const char* name, const int value);
extern "C"
  void CIMInstanceSetLongProperty ( NPIHandle*, ::CIMInstance , const char*,
                                   const long long );
extern "C"
  void CIMInstanceSetBooleanProperty ( NPIHandle*, ::CIMInstance,
                                      const char*, const unsigned char );
extern "C"
    void CIMInstanceSetRefProperty( NPIHandle* npiHandle, ::CIMInstance ci,
                                     const char* name, ::CIMObjectPath value);
extern "C"
char* CIMInstanceGetStringValue(NPIHandle* npiHandle,
                                 ::CIMInstance ci, const char* name);
extern "C"
int CIMInstanceGetIntegerValue(NPIHandle* npiHandle,
                                 ::CIMInstance ci, const char* name);
extern "C"
::CIMObjectPath CIMInstanceGetRefValue(NPIHandle* npiHandle,
                                       CIMInstance ci, const char* name);

// Object path functions
extern "C"
    ::CIMObjectPath CIMObjectPathNew ( NPIHandle* npiHandle,
                                       const char* classname );
extern "C"
    void CIMObjectPathDel ( NPIHandle* npiHandle,
                                       ::CIMObjectPath cop);
//extern "C"
//    ::CIMObjectPath CIMObjectPathNew ( NPIHandle* npiHandle,
//                                       ::CIMInstance ci);
extern "C"
    ::CIMObjectPath CIMObjectPathFromCIMInstance
                             ( NPIHandle* npiHandle, ::CIMInstance ci);
extern "C"
    char* CIMObjectPathGetClassName( NPIHandle* npiHandle,
                                       ::CIMObjectPath cop);
extern "C"
    char* CIMObjectPathGetNameSpace( NPIHandle* npiHandle,
                                       ::CIMObjectPath cop);
extern "C"
    void CIMObjectPathSetNameSpace( NPIHandle* npiHandle,
                                    ::CIMObjectPath cop, const char* str);
extern "C"
    void CIMObjectPathSetNameSpaceFromCIMObjectPath ( NPIHandle* npiHandle,
                                    ::CIMObjectPath cop, ::CIMObjectPath src);
extern "C"
    char* CIMObjectPathGetStringKeyValue (NPIHandle* npiHandle,
                                        ::CIMObjectPath cop, const char* key);
extern "C"
    void CIMObjectPathAddStringKeyValue (NPIHandle* npiHandle,
                                         ::CIMObjectPath cop,
                                         const char* key, const char* value);
extern "C"
    int CIMObjectPathGetIntegerKeyValue( NPIHandle* npiHandle,
                                        ::CIMObjectPath cop, const char* key);
extern "C"
    void CIMObjectPathAddIntegerKeyValue (NPIHandle* npiHandle,
                                         ::CIMObjectPath cop,
                                         const char* key, const int value);
extern "C"
    ::CIMObjectPath CIMObjectPathGetRefKeyValue( NPIHandle* npiHandle,
                                        ::CIMObjectPath cop, const char* key);
extern "C"
    void CIMObjectPathAddRefKeyValue (NPIHandle* npiHandle,
                                      ::CIMObjectPath cop,
                                      const char* key, ::CIMObjectPath cop2);

// CIMOM functions

extern "C"
    ::CIMClass CIMOMGetClass( NPIHandle* npiHandle,
                             ::CIMObjectPath cop, int localOnly );
extern "C"
    ::Vector CIMOMEnumInstanceNames(NPIHandle* npiHandle,
                                    ::CIMObjectPath cop, int i);
extern "C"
    ::Vector CIMOMEnumInstances(NPIHandle* npiHandle,
                                    ::CIMObjectPath cop, int i, int j);
extern "C"
    ::CIMInstance CIMOMGetInstance(NPIHandle* npiHandle,
                              ::CIMObjectPath cop, int i);

// Error handling classes

extern "C" int errorCheck(NPIHandle* npiHandle );

extern "C" void raiseError(NPIHandle* npiHandle, const char* msg );

extern "C"
char* _ObjectToString(NPIHandle* npiHandle, void* co);

#endif /* NPIExternal_Provider_h*/
