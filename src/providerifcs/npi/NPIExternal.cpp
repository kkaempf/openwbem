/*
 * NPIExternal.cpp
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Markus Mueller <markus_mueller@de.ibm.com>
 * Contributors: Marcin Gozdalik <gozdal@gozdal.eu.org>
 *               Viktor Mihajlovski <mihajlov@de.ibm.com>
 *
 * Description: <Some descriptive text>
 */

#include "NPIProvider.h"

// administrative functions

extern "C" PEGASUS_EXPORT
NPIenv * CreateEnv(
    //Pegasus::CIMOMHandle * cimomHandlePtr,
    Pegasus::CIMRepository * repository,
    const Pegasus::String& nameSpace)
{
    NPIenv * npienv = new NPIenv;
    //npienv->_context(0, NULL, 0, 1);
    npienv->_cimomhandle = repository;
    npienv->_nameSpace = nameSpace;
    return npienv;
}

extern "C" PEGASUS_EXPORT
void DeleteEnv(NPIenv * npienv) {delete npienv;}
    
extern "C" PEGASUS_EXPORT
Pegasus::CIMClass NPI_getmyClass(
    NPIHandle * npiHandle,
    const Pegasus::String& nameSpace,
    const Pegasus::String& className)
{
    Pegasus::Array<Pegasus::String> propertyList = Pegasus::EmptyStringArray();

    ((NPIenv *)npiHandle->thisObject)->_cimomhandle->read_lock();
    Pegasus::CIMClass cc =
        ((NPIenv *)npiHandle->thisObject)->_cimomhandle->getClass(
                                  nameSpace, className,
                                  true, true, false, propertyList);
    ((NPIenv *)npiHandle->thisObject)->_cimomhandle->read_unlock();

    return cc;
}

extern "C" PEGASUS_EXPORT
Pegasus::Array<Pegasus::CIMReference> NPI_enumeratemyInstanceNames(
    NPIHandle * npiHandle,
    const Pegasus::String& nameSpace,
    const Pegasus::String& className)
{
    ((NPIenv *)npiHandle->thisObject)->_cimomhandle->read_lock();
    Pegasus::Array<Pegasus::CIMReference> crefs = 
        ((NPIenv *)npiHandle->thisObject)->
           _cimomhandle->enumerateInstanceNames(nameSpace, className);
    ((NPIenv *)npiHandle->thisObject)->_cimomhandle->read_unlock();
    return crefs;
}

extern "C" PEGASUS_EXPORT
Pegasus::Array<Pegasus::CIMNamedInstance> NPI_enumeratemyInstances(
    NPIHandle * npiHandle,
    const Pegasus::String& nameSpace,
    const Pegasus::String& className)
{
    Pegasus::Array<Pegasus::String> propertyList = Pegasus::EmptyStringArray();

    ((NPIenv *)npiHandle->thisObject)->_cimomhandle->read_lock();
    Pegasus::Array<Pegasus::CIMNamedInstance> cinsts = 
         ((NPIenv *)npiHandle->thisObject)->
           _cimomhandle->enumerateInstances(
                       nameSpace, className, true,
                       true , true, false, propertyList);
    ((NPIenv *)npiHandle->thisObject)->_cimomhandle->read_unlock();
    return cinsts;
}



// externalized functions

extern "C" PEGASUS_EXPORT
Vector VectorNew ( NPIHandle * npiHandle) {
    Vector v;
    v.ptr = (void *) new(charVect);
    return v;
}

extern "C" PEGASUS_EXPORT 
void _VectorAddTo ( NPIHandle * npiHandle,Vector v, void * obj) {
   ((charVect *)v.ptr)->append((char *)obj);
}

extern "C" PEGASUS_EXPORT 
int VectorSize( NPIHandle * npiHandle, Vector v) {
   return ((charVect *)v.ptr)->size();
}

extern "C" PEGASUS_EXPORT 
void * _VectorGet( NPIHandle * npiHandle, Vector v, int pos) {
   void * result = NULL;
   result = (void *) ( (*((charVect *)v.ptr))[pos]);
   return result;
}


// CIMValue functions 



extern "C" PEGASUS_EXPORT 
CIMType CIMValueGetType (NPIHandle *, CIMValue cv) {
    Pegasus::CIMValue * pcv = (Pegasus::CIMValue *)cv.ptr;
    Pegasus::CIMType pct = pcv->getType();
    switch(pct) {
        case Pegasus::CIMType::BOOLEAN :
        case Pegasus::CIMType::UINT8 :
        case Pegasus::CIMType::SINT8 :
        case Pegasus::CIMType::UINT16 :
        case Pegasus::CIMType::SINT16 :
        case Pegasus::CIMType::UINT32 :
        case Pegasus::CIMType::SINT32 :
        case Pegasus::CIMType::UINT64 :
        case Pegasus::CIMType::SINT64 :
        case Pegasus::CIMType::REAL32 :
        case Pegasus::CIMType::REAL64 :
        case Pegasus::CIMType::DATETIME :
            return CIM_INTEGER;
        case Pegasus::CIMType::CHAR16 :
        case Pegasus::CIMType::STRING :
            return CIM_STRING;
        case Pegasus::CIMType::REFERENCE :
            return CIM_REF;
    }
    return CIM_INTEGER;
}

extern "C" PEGASUS_EXPORT 
CIMValue CIMValueNewString (NPIHandle * npiHandle, const char * val) {
    //Pegasus::String * mystring = new Pegasus::String(val);
    //Pegasus::CIMValue * pcv = new Pegasus::CIMValue(*mystring);
    Pegasus::CIMValue * pcv =
        new Pegasus::CIMValue(Pegasus::String(val));
    CIMValue cv = {(void *) pcv};
    return cv;
}

extern "C" PEGASUS_EXPORT 
CIMValue CIMValueNewInteger(NPIHandle * npiHandle, int val) {
    Pegasus::CIMValue * pcv = new Pegasus::CIMValue(val);
    CIMValue cv = {(void *) pcv};
    return cv;
}

extern "C" PEGASUS_EXPORT 
CIMValue CIMValueNewRef (NPIHandle * npiHandle, CIMObjectPath cop) {
    Pegasus::CIMObjectWithPath * cowp = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::CIMValue * pcv = new Pegasus::CIMValue(cowp->getReference());
    CIMValue cv = {(void *) pcv};
    return cv;
}

extern "C" PEGASUS_EXPORT 
char * CIMValueGetString (NPIHandle * npiHandle, CIMValue cv) {
    Pegasus::CIMValue * pcv = (Pegasus::CIMValue *) cv.ptr;
    Pegasus::String mystring;
    pcv->get(mystring);
    char * retval = mystring.allocateCString();
    return retval;
}

extern "C" PEGASUS_EXPORT 
int CIMValueGetInteger(NPIHandle * npiHandle, CIMValue cv) {
    Pegasus::CIMValue * pcv = (Pegasus::CIMValue *) cv.ptr;
    int retval;
    pcv->get(retval);
    return retval;
}

extern "C" PEGASUS_EXPORT 
CIMObjectPath CIMValueGetRef(NPIHandle * npiHandle, CIMValue cv) {
    Pegasus::CIMValue * pcv = (Pegasus::CIMValue *) cv.ptr;
    Pegasus::CIMObject cimo;
    Pegasus::CIMReference cref;
    pcv->get(cref);
    Pegasus::CIMObjectWithPath * cowp = 
        new Pegasus::CIMObjectWithPath(cref,cimo);
    CIMObjectPath cop = {(void *) cowp};
    return cop;
}


// CIMParameter functions

extern "C" PEGASUS_EXPORT
    ::CIMType CIMParameterGetType( NPIHandle * npiHandle, ::CIMParameter cp) {
    Pegasus::CIMParamValue * pcp = (Pegasus::CIMParamValue *)cp.ptr;
    Pegasus::CIMType pct = pcp->getValue().getType();
    switch(pct) {
        case Pegasus::CIMType::BOOLEAN :
        case Pegasus::CIMType::UINT8 :
        case Pegasus::CIMType::SINT8 :
        case Pegasus::CIMType::UINT16 :
        case Pegasus::CIMType::SINT16 :
        case Pegasus::CIMType::UINT32 :
        case Pegasus::CIMType::SINT32 :
        case Pegasus::CIMType::UINT64 :
        case Pegasus::CIMType::SINT64 :
        case Pegasus::CIMType::REAL32 :
        case Pegasus::CIMType::REAL64 :
        case Pegasus::CIMType::DATETIME :
            return CIM_INTEGER;
        case Pegasus::CIMType::CHAR16 :
        case Pegasus::CIMType::STRING :
            return CIM_STRING;
        case Pegasus::CIMType::REFERENCE :
            return CIM_REF;
    }
    return CIM_INTEGER;
}

extern "C" PEGASUS_EXPORT
    char * CIMParameterGetName( NPIHandle * npiHandle, ::CIMParameter cp) {
    Pegasus::CIMParamValue * pcp = (Pegasus::CIMParamValue *)cp.ptr;
    char * retval = pcp->getParameter().getName().allocateCString();
    return retval;
}

extern "C" PEGASUS_EXPORT
    ::CIMParameter CIMParameterNewString (NPIHandle * npiHandle,
            const char * name, char * value) {
    ::CIMParameter mycp = {NULL};

    // Sanity check
    if (name == NULL) return mycp;
    if (strlen(name) == 0) return mycp;

    Pegasus::String Name(name);
    Pegasus::CIMParameter * pcp = new Pegasus::CIMParameter(Name,
          Pegasus::CIMType::STRING);
    Pegasus::CIMValue * pcv = new Pegasus::CIMValue(value);
    Pegasus::CIMParamValue * pcpv = new Pegasus::CIMParamValue(*pcp,*pcv);

    mycp.ptr = pcpv;
    return mycp;
}

extern "C" PEGASUS_EXPORT
    ::CIMParameter CIMParameterNewInteger(NPIHandle * npiHandle,
            const char * name, int value) {
    ::CIMParameter mycp = {NULL};

    // Sanity check
    if (name == NULL) return mycp;
    if (strlen(name) == 0) return mycp;

    Pegasus::String Name(name);
    Pegasus::CIMParameter * pcp = new Pegasus::CIMParameter(Name,
          Pegasus::CIMType::SINT32);
    Pegasus::CIMValue * pcv = new Pegasus::CIMValue(value);
    Pegasus::CIMParamValue * pcpv = new Pegasus::CIMParamValue(*pcp,*pcv);

    mycp.ptr = pcpv;
    return mycp;
}

extern "C" PEGASUS_EXPORT
::CIMParameter CIMParameterNewRef(NPIHandle * npiHandle,
            const char * name, CIMObjectPath value) {
    ::CIMParameter mycp = {NULL};

    // Sanity check
    if (name == NULL) return mycp;
    if (strlen(name) == 0) return mycp;

    Pegasus::String Name(name);
    Pegasus::CIMParameter * pcp = new Pegasus::CIMParameter(Name,
          Pegasus::CIMType::REFERENCE);
    Pegasus::CIMObjectWithPath * cowp = (Pegasus::CIMObjectWithPath *)
                                          value.ptr;
    Pegasus::CIMValue * pcv = new Pegasus::CIMValue(cowp->getReference());
    Pegasus::CIMParamValue * pcpv = new Pegasus::CIMParamValue(*pcp,*pcv);

    mycp.ptr = pcpv;
    return mycp;
}

extern "C" PEGASUS_EXPORT 
char * CIMParameterGetStringValue ( NPIHandle * npiHandle,
         ::CIMParameter cp) {
    Pegasus::CIMParamValue * pcpv = (Pegasus::CIMParamValue *)cp.ptr;
    Pegasus::String Value;
    pcpv->getValue().get(Value);
    char * value = Value.allocateCString();
    return value;
}

extern "C" PEGASUS_EXPORT 
int CIMParameterGetIntegerValue ( NPIHandle * npiHandle,
         ::CIMParameter cp) {
    Pegasus::CIMParamValue * pcpv = (Pegasus::CIMParamValue *)cp.ptr;
    int Value;
    pcpv->getValue().get(Value);
    return Value;
}

extern "C" PEGASUS_EXPORT 
CIMObjectPath CIMParameterGetRefValue ( NPIHandle * npiHandle,
         ::CIMParameter cp) {
    Pegasus::CIMParamValue * pcpv = (Pegasus::CIMParamValue *)cp.ptr;

    Pegasus::CIMObject cimo;
    Pegasus::CIMReference cref;

    pcpv->getValue().get(cref);
    Pegasus::CIMObjectWithPath * cowp = 
        new Pegasus::CIMObjectWithPath(cref,cimo);

    CIMObjectPath cop = {(void *) cowp};
    return cop;
}


// Instance functions

 

extern "C" PEGASUS_EXPORT 
CIMInstance CIMClassNewInstance ( NPIHandle * npiHandle, CIMClass cc ) {
   Pegasus::CIMClass * a = (Pegasus::CIMClass *) cc.ptr;
   char * clsName = a->getClassName().allocateCString();
   Pegasus::String className(clsName);
   Pegasus::CIMInstance * ci =new Pegasus::CIMInstance(className);
   CIMInstance b;
   b.ptr = (void *) ci;
   return b;
}

extern "C" PEGASUS_EXPORT 
void CIMInstanceSetStringProperty( NPIHandle * npiHandle,
                 CIMInstance ci, const char * name, const char * value ) {
    // Sanity check
    if (name == NULL) return;
    if (strlen(name) == 0) return;

    Pegasus::CIMInstance * a = (Pegasus::CIMInstance *) ci.ptr;
    Pegasus::String Key(name);
    Pegasus::String Val;

    if (value) {
       if (strlen(value)>0) Val.assign(value);
       else Val.assign("-empty-");
    }
    else Val.assign("-empty-");


    Pegasus::CIMValue Value(Val);
    if (a->existsProperty(Key)) return;
    Pegasus::CIMProperty cip(Key,Value);
    a->addProperty(cip);
}

extern "C" PEGASUS_EXPORT 
void CIMInstanceSetIntegerProperty( NPIHandle * npiHandle,
                 CIMInstance ci, const char * name, const int value) {
    // Sanity check
    if (name == NULL) return;
    if (strlen(name) == 0) return;

    Pegasus::CIMInstance * a = (Pegasus::CIMInstance *) ci.ptr;
    Pegasus::String Key(name);

    Pegasus::CIMValue Value(value);
    if (a->existsProperty(Key)) return;
    Pegasus::CIMProperty cip(Key,Value);
    a->addProperty(cip);
}

extern "C" PEGASUS_EXPORT 
void CIMInstanceSetLongProperty( NPIHandle * npiHandle, CIMInstance ci,
                                const char * name,
                                const long long value) {
    // Sanity check
    if (name == NULL) return;
    if (strlen(name) == 0) return;

    Pegasus::CIMInstance * a = (Pegasus::CIMInstance *) ci.ptr;
    Pegasus::String Key(name);

    Pegasus::CIMValue Value(value);
    if (a->existsProperty(Key)) return;
    Pegasus::CIMProperty cip(Key,Value);
    a->addProperty(cip);
}

extern "C" PEGASUS_EXPORT 
void CIMInstanceSetBooleanProperty( NPIHandle * npiHandle, CIMInstance ci,
                                   const char * name,
                                   const unsigned char value) {
    // Sanity check
    if (name == NULL) return;
    if (strlen(name) == 0) return;

    Pegasus::CIMInstance * a = (Pegasus::CIMInstance *) ci.ptr;
    Pegasus::String Key(name);

    Pegasus::CIMValue Value(Pegasus::Boolean((int)value));
    if (a->existsProperty(Key)) return;
    Pegasus::CIMProperty cip(Key,Value);
    a->addProperty(cip);
}

extern "C" PEGASUS_EXPORT 
void CIMInstanceSetRefProperty( NPIHandle * npiHandle,
                 CIMInstance ci, const char * name, CIMObjectPath value) {
    // Sanity check
    if (name == NULL) return;
    if (strlen(name) == 0) return;

    Pegasus::CIMInstance * a = (Pegasus::CIMInstance *) ci.ptr;
    Pegasus::CIMReference * b = (Pegasus::CIMReference *) value.ptr;

    Pegasus::String Key(name);

    Pegasus::CIMValue Value(*b);
    if (a->existsProperty(Key)) return;
    Pegasus::CIMProperty cip(Key,Value);
    a->addProperty(cip);
}

extern "C" PEGASUS_EXPORT
char * CIMInstanceGetStringValue(NPIHandle * npiHandle,
                                 CIMInstance ci, const char * name)
{
    // Sanity check
    if (name == NULL) return NULL;
    if (strlen(name) == 0) return NULL;

    return NULL;
}

extern "C" PEGASUS_EXPORT
int CIMInstanceGetIntegerValue(NPIHandle * npiHandle,
                              CIMInstance ci, const char * name)
{
    // Sanity check
    if (name == NULL) return 0;
    if (strlen(name) == 0) return 0;

    return 0;
}

extern "C" PEGASUS_EXPORT
CIMObjectPath CIMInstanceGetRefValue(NPIHandle * npiHandle,
                                     CIMInstance ci, const char * name)
{
    CIMObjectPath cop = {NULL};

    // Sanity check
    if (name == NULL) return cop;
    if (strlen(name) == 0) return cop;

    return cop;
}


// Object Path functions



// empty keyBindings here
extern "C" PEGASUS_EXPORT 
CIMObjectPath CIMObjectPathNew ( NPIHandle * npiHandle,
                                 const char * classname ) {
    Pegasus::KeyBindingArray keyBindings;
    Pegasus::String host;
    Pegasus::String className(classname);
    Pegasus::String nameSpace =
         ((NPIenv *)npiHandle->thisObject)->_nameSpace;
    Pegasus::CIMReference cr(host,nameSpace,className,keyBindings);
    Pegasus::CIMObject cimo;
    Pegasus::CIMObjectWithPath * cowp = new Pegasus::CIMObjectWithPath(cr,cimo);
    CIMObjectPath b = {(void *) cowp};
    return b;
}

extern "C" PEGASUS_EXPORT 
void CIMObjectPathDel ( NPIHandle * npiHandle,
                         CIMObjectPath cop)
{
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    delete a;
}

// Call CIMInstance.getClassName
// Loop over CIMInstance.getProperty with CIMInstance.getPropertyCount
// to the the key-value bindings.
extern "C" PEGASUS_EXPORT 
CIMObjectPath CIMObjectPathFromCIMInstance
                             ( NPIHandle * npiHandle, CIMInstance ci) {
    Pegasus::CIMInstance * a = (Pegasus::CIMInstance *) ci.ptr;
    Pegasus::String host;
    Pegasus::String className = a->getClassName();
    Pegasus::KeyBindingArray keyBindings;
    Pegasus::CIMProperty prop;
    for(Pegasus::Uint32 i=0;i<a->getPropertyCount();i++) {
      prop = a->getProperty(i);
      Pegasus::String value = prop.getValue().toString();
      keyBindings.append(Pegasus::KeyBinding(
            prop.getName(), value, Pegasus::KeyBinding::STRING));
    }
    Pegasus::CIMReference cr(host,
         ((NPIenv *)npiHandle->thisObject)->_nameSpace,
         className,keyBindings);
    Pegasus::CIMObjectWithPath * cowp = new Pegasus::CIMObjectWithPath(cr,*a);
    CIMObjectPath c = {(void *)cowp};
    return c;
}

extern "C" PEGASUS_EXPORT 
char * CIMObjectPathGetClassName( NPIHandle * npiHandle, CIMObjectPath cop) {
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    char * result = a->getReference().getClassName().allocateCString();
    return result;
}

extern "C" PEGASUS_EXPORT 
char * CIMObjectPathGetNameSpace( NPIHandle * npiHandle, CIMObjectPath cop) {
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    cout << a->getReference().getNameSpace();
    char * result = a->getReference().getNameSpace().allocateCString();
    return result;
}

extern "C" PEGASUS_EXPORT 
void CIMObjectPathSetNameSpace( NPIHandle * npiHandle,
         CIMObjectPath cop, const char * str) {
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::String nameSpace(str);
    a->getReference().setNameSpace(nameSpace);
}

extern "C" PEGASUS_EXPORT 
void CIMObjectPathSetNameSpaceFromCIMObjectPath(
       NPIHandle * npiHandle, CIMObjectPath cop, CIMObjectPath src) {
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::CIMObjectWithPath * b = (Pegasus::CIMObjectWithPath *) src.ptr;
    a->getReference().setNameSpace(b->getReference().getNameSpace());
}

extern "C" PEGASUS_EXPORT 
char * CIMObjectPathGetStringKeyValue (NPIHandle * npiHandle,
             CIMObjectPath cop, const char * key) {
    // Sanity check
    if (key == NULL) return NULL;
    if (strlen(key) == 0) return NULL;

    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    const Pegasus::Array<Pegasus::KeyBinding>& KeyBindings =
               a->getReference().getKeyBindings();
    Pegasus::String Key(key);
    if (a->getReference().getKeyBindings().size() == 0) return NULL;
    for(Pegasus::Uint32 i=0;i < KeyBindings.size();i++) {
       if (Pegasus::String::equalNoCase(KeyBindings[i].getName(),Key))
          return KeyBindings[i].getValue().allocateCString();
    }
    return NULL;
}

extern "C" PEGASUS_EXPORT 
void CIMObjectPathAddStringKeyValue (NPIHandle * npiHandle, CIMObjectPath cop,
                                     const char * key, const char * value)
{
    // Sanity check
    if (key == NULL) return;
    if (strlen(key) == 0) return;

    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
               a->getReference().getKeyBindings();
    Pegasus::String Key(key);
    Pegasus::String Value(value);
    KeyBindings.append(
        Pegasus::KeyBinding(Key,Value,Pegasus::KeyBinding::STRING));
    a->getReference().setKeyBindings(KeyBindings);
}

extern "C" PEGASUS_EXPORT 
int CIMObjectPathGetIntegerKeyValue (NPIHandle * npiHandle,
             CIMObjectPath cop, const char * key) {
    // Sanity check
    if (key == NULL) return -1;
    if (strlen(key) == 0) return -1;

    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
                       a->getReference().getKeyBindings();
    Pegasus::String Key(key);
    int k;
    for(Pegasus::Uint32 i=0;i < KeyBindings.size();i++) {
       if (KeyBindings[i].getName()==Key) {
          char * myvalue = KeyBindings[i].getValue().allocateCString();
          k=atoi(myvalue);
          return k;
       }
    }
    return -1;
}

extern "C" PEGASUS_EXPORT 
void CIMObjectPathAddIntegerKeyValue (NPIHandle * npiHandle, CIMObjectPath cop,
                                     const char * key, const int value)
{
    // Sanity check
    if (key == NULL) return;
    if (strlen(key) == 0) return;

    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
               a->getReference().getKeyBindings();
    Pegasus::String Key(key);
    char buffer[10];
    sprintf(buffer,"%*d",9,value);
    Pegasus::String Value(buffer);
    KeyBindings.append(
        Pegasus::KeyBinding(Key, Value, Pegasus::KeyBinding::NUMERIC));
    a->getReference().setKeyBindings(KeyBindings);
}


extern "C" PEGASUS_EXPORT 
  CIMObjectPath CIMObjectPathGetRefKeyValue (NPIHandle * npiHandle,
             CIMObjectPath cop, const char * key) {
    // Sanity check
    
    CIMObjectPath c = {NULL};
    if (key == NULL) return c;
    if (strlen(key) == 0) return c;

    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
                       a->getReference().getKeyBindings();
    Pegasus::String Key(key);
    for(Pegasus::Uint32 i=0;i < KeyBindings.size();i++) {
       if (KeyBindings[i].getName()==Key) {
          Pegasus::CIMReference cr(
              KeyBindings[i].getValue().allocateCString());
          Pegasus::CIMObject cimo;
          Pegasus::CIMObjectWithPath * cowp = new Pegasus::CIMObjectWithPath(
             cr,cimo);
          // ERROR ERROR ERROR
          c.ptr = cowp;
          return c;
       }
    }
    return c;
}

extern "C" PEGASUS_EXPORT 
void CIMObjectPathAddRefKeyValue (NPIHandle * npiHandle, CIMObjectPath cop,
                                     const char * key, CIMObjectPath cop2)
{
    // Sanity check
    if (key == NULL) return;
    if (strlen(key) == 0) return;

    DDD( cerr << "NPIProvider: CIMObjectPathAddRefKeyValue"; )
    DDD( cerr << "NPIProvider: ERROR - cannot be implemented !!!"; )
}

// CIMOM functions

extern "C" PEGASUS_EXPORT 
  CIMClass CIMOMGetClass( NPIHandle * npiHandle,
                 CIMObjectPath cop, int localOnly) {
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::String nameSpace = a->getReference().getNameSpace();
    Pegasus::String className = a->getReference().getClassName();
    Pegasus::CIMClass * cc = new Pegasus::CIMClass(
        NPI_getmyClass(npiHandle,nameSpace,className));
    Pegasus::String myclassName = cc->getClassName();
    CIMClass localcc = {cc};
    return localcc;
}

extern "C" PEGASUS_EXPORT 
Vector CIMOMEnumInstanceNames(NPIHandle * npiHandle, CIMObjectPath cop, int i)
{
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::String nameSpace = a->getReference().getNameSpace();
    Pegasus::String className = a->getReference().getClassName();
    Pegasus::Array<Pegasus::CIMReference> instNames;

    instNames = NPI_enumeratemyInstanceNames(npiHandle,nameSpace,className);
    Pegasus::CIMObjectWithPath * cowp;
    Pegasus::CIMObject cimo;
    Vector v = VectorNew(npiHandle);
    for (int i=instNames.size()-1;i>=0;i--)
    {
        cowp = new Pegasus::CIMObjectWithPath(instNames[i],cimo);
        _VectorAddTo(npiHandle, v, (void *)cowp);
    }
    return v;
}

extern "C" PEGASUS_EXPORT 
Vector CIMOMEnumInstances(NPIHandle * npiHandle, CIMObjectPath cop,int i,int j)
{
    Pegasus::CIMObjectWithPath * a = (Pegasus::CIMObjectWithPath *) cop.ptr;
    Pegasus::String nameSpace = a->getReference().getNameSpace();
    Pegasus::String className = a->getReference().getClassName();
    Pegasus::Array<Pegasus::CIMNamedInstance> insts;

    insts = NPI_enumeratemyInstances(npiHandle,nameSpace,className);
    Pegasus::CIMInstance * ci;
    Vector v = VectorNew(npiHandle);
    for (int i=insts.size()-1;i>=0;i--)
    {
        ci = new Pegasus::CIMInstance(insts[i].getInstance());
        _VectorAddTo(npiHandle, v, (void *)ci);
    }
    return v;
}

extern "C" PEGASUS_EXPORT 
CIMInstance CIMOMGetInstance(NPIHandle * npiHandle, CIMObjectPath cop, int i)
{
    CIMInstance ci = {(void *)NULL};
    return ci;
}

extern "C" PEGASUS_EXPORT
char * _ObjectToString(NPIHandle * npiHandle, void * co) {
   Pegasus::Array<Pegasus::Sint8> xml;
   Pegasus::CIMObjectWithPath * powp;
   Pegasus::CIMInstance * pci;
   Pegasus::CIMClass * pcc;
   powp = (Pegasus::CIMObjectWithPath *)co;
   powp->getReference().toXml(xml);
   //if ( (powp = dynamic_cast<Pegasus::CIMObjectWithPath *>(co)) )
   //   powp->toXml(xml);
   //else if ( (pci = dynamic_cast<Pegasus::CIMInstance *>(co)) )
   //   pci->toXml(xml);
   //else if ( (pcc = dynamic_cast<Pegasus::CIMClass *>(co)) )
   //   pcc->toXml(xml);
   //else return NULL;   
   int sz = xml.size();
   char * retval = (char *)malloc(sz+1);
   retval[sz] = '\0';
   for (int i = 0; i<sz; i++) retval[i]=xml[i];
   return retval;
}

// Error handling classes

extern "C" PEGASUS_EXPORT
int errorCheck ( NPIHandle * npiHandle ) {
    return npiHandle->errorOccurred;
}

extern "C" PEGASUS_EXPORT
void raiseError ( NPIHandle * npiHandle, const char * msg ) {
     if ( npiHandle->providerError != NULL )
         free ( (void *) npiHandle->providerError );
     npiHandle->providerError = strdup ( msg );
}

//static void raiseNPIException( void * env, char * msg) {}
//static void throwProviderError (NPIHandle * npiHandle) {}
