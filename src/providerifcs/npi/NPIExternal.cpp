#include "OW_config.h"
#include "NPIProvider.hpp"

namespace
{
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
}

// administrative functions

//////////////////////////////////////////////////////////////////////////////
extern "C" NPIenv*
createEnv(OW_CIMOMHandleIFCRef repository, const OW_String& nameSpace)
{
	NPIenv* npienv = new NPIenv;
	npienv->_cimomhandle = repository;
	npienv->_nameSpace = nameSpace;
	return npienv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
deleteEnv(NPIenv* npienv)
{
	delete npienv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_CIMClass
NPI_getmyClass(NPIHandle* npiHandle, const OW_String& nameSpace,
	const OW_String& className)
{
	OW_StringArray propertyList;
	OW_CIMObjectPath op(className, nameSpace);
	OW_CIMClass cc = ((NPIenv*)npiHandle->thisObject)->_cimomhandle->getClass(
		op, NOT_LOCAL_ONLY, INCLUDE_QUALIFIERS, INCLUDE_CLASS_ORIGIN, NULL);
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_CIMObjectPathEnumeration
NPI_enumeratemyInstanceNames(NPIHandle* npiHandle,
	const OW_String& nameSpace, const OW_String& className)
{
	OW_CIMObjectPath op(className, nameSpace);
	OW_CIMObjectPathEnumeration crefs = ((NPIenv*)npiHandle->thisObject)->
		_cimomhandle->enumInstanceNames(op, DEEP);
	return crefs;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_CIMInstanceEnumeration
NPI_enumeratemyInstances(NPIHandle* npiHandle, const OW_String& nameSpace,
	const OW_String& className)
{
	OW_CIMObjectPath op(className, nameSpace);
	OW_CIMInstanceEnumeration cinsts = ((NPIenv*)npiHandle->thisObject)->
		_cimomhandle->enumInstances(op, DEEP, NOT_LOCAL_ONLY,
			EXCLUDE_QUALIFIERS, EXCLUDE_CLASS_ORIGIN, NULL);

	return cinsts;
}


// externalized functions

//////////////////////////////////////////////////////////////////////////////
extern "C" Vector
VectorNew(NPIHandle* npiHandle)
{
	Vector v;
	v.ptr = (void*) new charVect;
	return v;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
_VectorAddTo(NPIHandle* npiHandle, Vector v, void* obj)
{
	((charVect*)v.ptr)->append((char*)obj);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
VectorSize(NPIHandle* npiHandle, Vector v)
{
	return((charVect*)v.ptr)->size();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void*
_VectorGet(NPIHandle* npiHandle, Vector v, int pos)
{
	void* result = NULL;
	result = (void*) ((*((charVect*)v.ptr))[pos]);
	return result;
}


// CIMValue functions 


//////////////////////////////////////////////////////////////////////////////
extern "C" CIMType
CIMValueGetType(NPIHandle*, CIMValue cv)
{
	OW_CIMValue* pcv = (OW_CIMValue*)cv.ptr;
	int pct = pcv->getType();
	switch(pct)
	{
		case OW_CIMDataType::BOOLEAN :
		case OW_CIMDataType::UINT8:
		case OW_CIMDataType::SINT8:
		case OW_CIMDataType::UINT16:
		case OW_CIMDataType::SINT16:
		case OW_CIMDataType::UINT32:
		case OW_CIMDataType::SINT32:
		case OW_CIMDataType::UINT64:
		case OW_CIMDataType::SINT64:
		case OW_CIMDataType::REAL32:
		case OW_CIMDataType::REAL64:
		case OW_CIMDataType::DATETIME:
			return CIM_INTEGER;

		case OW_CIMDataType::CHAR16:
		case OW_CIMDataType::STRING:
			return CIM_STRING;

		case OW_CIMDataType::REFERENCE:
			return CIM_REF;
	}

	return CIM_INTEGER;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMValue
CIMValueNewString(NPIHandle* npiHandle, const char* val)
{
	OW_CIMValue* pcv = new OW_CIMValue(OW_String(val));
	CIMValue cv = { (void*) pcv };
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMValue
CIMValueNewInteger(NPIHandle* npiHandle, int val)
{
	OW_CIMValue* pcv = new OW_CIMValue(val);
	CIMValue cv = { (void*) pcv };
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMValue
CIMValueNewRef(NPIHandle* npiHandle, CIMObjectPath cop)
{
	OW_CIMValue* pcv = new OW_CIMValue(*((OW_CIMObjectPath*)cop.ptr))
	CIMValue cv = { (void*) pcv };
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMValueGetString(NPIHandle* npiHandle, CIMValue cv)
{
	OW_CIMValue* pcv = (OW_CIMValue*)cv.ptr;
	OW_String mystring = pcv->toString();
	return mystring.allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMValueGetInteger(NPIHandle* npiHandle, CIMValue cv)
{
	OW_CIMValue* pcv = (OW_CIMValue*)cv.ptr;
	int retval;
	pcv->get(retval);
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMValueGetRef(NPIHandle* npiHandle, CIMValue cv)
{
	OW_CIMValue* pcv = (OW_CIMValue*) cv.ptr;
	OW_CIMObjectPath cref;
	pcv->get(cref);
	OW_CIMObjectPath* ncop = new OW_CIMObjectPath(cref);
	CIMObjectPath cop = {(void*) ncop };
	return cop;
}

// CIMParameter functions

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMType
CIMParameterGetType(NPIHandle* npiHandle, CIMParameter cp)
{
	OW_CIMParamValue* pcp = (OW_CIMParamValue*)cp.ptr;
	int dt = pcp->getValue().getType();
	switch(dt.getType())
	{
		case OW_CIMDataType::BOOLEAN :
		case OW_CIMDataType::UINT8 :
		case OW_CIMDataType::SINT8 :
		case OW_CIMDataType::UINT16 :
		case OW_CIMDataType::SINT16 :
		case OW_CIMDataType::UINT32 :
		case OW_CIMDataType::SINT32 :
		case OW_CIMDataType::UINT64 :
		case OW_CIMDataType::SINT64 :
		case OW_CIMDataType::REAL32 :
		case OW_CIMDataType::REAL64 :
		case OW_CIMDataType::DATETIME :
			return CIM_INTEGER;

		case OW_CIMDataType::CHAR16 :
		case OW_CIMDataType::STRING :
			return CIM_STRING;

		case OW_CIMDataType::REFERENCE :
			return CIM_REF;
	}

	return CIM_INTEGER;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMParameterGetName(NPIHandle* npiHandle, CIMParameter cp)
{	
	OW_CIMParamValue* pcp = (OW_CIMParamValue*)cp.ptr;
	return pcp->getName().allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMParameter
CIMParameterNewString(NPIHandle* npiHandle, const char* name, char* value)
{
	CIMParameter mycp = { NULL };

	// Sanity check
	if(name == NULL)
		return mycp;

	if(strlen(name) == 0)
		return mycp;

	OW_CIMParamValue* pcp = new OW_CIMParamValue(OW_String(name),
		OW_CIMValue(OW_String(value)));
	mycp.ptr = pcp;
	return mycp;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMParameter
CIMParameterNewInteger(NPIHandle* npiHandle, const char* name, int value)
{
	CIMParameter mycp = { NULL };

	// Sanity check
	if(name == NULL)
		return mycp;

	if(strlen(name) == 0)
		return mycp;

	OW_CIMParamValue* pcp = new OW_CIMParamValue(OW_String(name),
		OW_CIMValue(OW_Int32(value));
	mycp.ptr = pcp;
	return mycp;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMParameter
CIMParameterNewRef(NPIHandle* npiHandle, const char* name, CIMObjectPath value)
{
	CIMParameter mycp = { NULL };

	// Sanity check
	if(name == NULL)
		return mycp;

	if(strlen(name) == 0)
		return mycp;

	OW_CIMValue val(*((OW_CIMObjectPath*)value.ptr));
	mycp.ptr = new OW_CIMParamValue(OW_String(name), val);
	return mycp;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMParameterGetStringValue(NPIHandle* npiHandle, CIMParameter cp)
{
	OW_CIMParamValue* pcpv = (OW_CIMParamValue*) cp.ptr;
	OW_String value = pcpv->getValue().toString();
	return value.allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMParameterGetIntegerValue(NPIHandle* npiHandle, CIMParameter cp)
{
	OW_CIMParamValue* pcpv = (OW_CIMParamValue*)cp.ptr;
	int value;
	pcpv->getValue().get(value);
	return value;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMParameterGetRefValue(NPIHandle* npiHandle, CIMParameter cp)
{
	OW_CIMParamValue* pcpv = (OW_CIMParamValue*)cp.ptr;
	OW_CIMObjectPath op;
	OW_CIMValue val = pcpv->getValue();
	val.get(op);
	OW_CIMObjectPath pop = new OW_CIMObjectPath(op);
	CIMObjectPath cop = { (void*) pop };
	return cop;
}


// Instance functions

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMInstance
CIMClassNewInstance(NPIHandle* npiHandle, CIMClass cc)
{
	/*
	Pegasus::CIMClass* a = (Pegasus::CIMClass*) cc.ptr;
	char* clsName = a->getClassName().allocateCString();
	OW_String className(clsName);
	Pegasus::CIMInstance* ci = new Pegasus::CIMInstance(className);
	CIMInstance b;
	b.ptr = (void*) ci;
	return b;
	*/

	OW_CIMClass* a = (OW_CIMClass*) cc.ptr;
	OW_String clsName = a->getName();
	
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetStringProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const char* value )
{
	// Sanity check
	if(name == NULL)
		return;

	if(strlen(name) == 0)
		return;

	Pegasus::CIMInstance* a = (Pegasus::CIMInstance*) ci.ptr;
	OW_String Key(name);
	OW_String Val;

	if(value)
	{
		if(strlen(value)>0)	Val.assign(value);
		else Val.assign("-empty-");
	}
	else Val.assign("-empty-");


	Pegasus::CIMValue Value(Val);
	if(a->existsProperty(Key)) return;
	Pegasus::CIMProperty cip(Key,Value);
	a->addProperty(cip);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetIntegerProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const int value)
{
	// Sanity check
	if(name == NULL) return;
	if(strlen(name) == 0) return;

	Pegasus::CIMInstance* a = (Pegasus::CIMInstance*) ci.ptr;
	OW_String Key(name);

	Pegasus::CIMValue Value(value);
	if(a->existsProperty(Key)) return;
	Pegasus::CIMProperty cip(Key,Value);
	a->addProperty(cip);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetLongProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const long long value)
{
	// Sanity check
	if(name == NULL) return;
	if(strlen(name) == 0) return;

	Pegasus::CIMInstance* a = (Pegasus::CIMInstance*) ci.ptr;
	OW_String Key(name);

	Pegasus::CIMValue Value(value);
	if(a->existsProperty(Key)) return;
	Pegasus::CIMProperty cip(Key,Value);
	a->addProperty(cip);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetBooleanProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const unsigned char value)
{
	// Sanity check
	if(name == NULL) return;
	if(strlen(name) == 0) return;

	Pegasus::CIMInstance* a = (Pegasus::CIMInstance*) ci.ptr;
	OW_String Key(name);

	Pegasus::CIMValue Value(Pegasus::Boolean((int)value));
	if(a->existsProperty(Key)) return;
	Pegasus::CIMProperty cip(Key,Value);
	a->addProperty(cip);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetRefProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, CIMObjectPath value)
{
	// Sanity check
	if(name == NULL) return;
	if(strlen(name) == 0) return;

	Pegasus::CIMInstance* a = (Pegasus::CIMInstance*) ci.ptr;
	Pegasus::CIMReference* b = (Pegasus::CIMReference*) value.ptr;

	OW_String Key(name);

	Pegasus::CIMValue Value(*b);
	if(a->existsProperty(Key)) return;
	Pegasus::CIMProperty cip(Key,Value);
	a->addProperty(cip);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMInstanceGetStringValue(NPIHandle* npiHandle, CIMInstance ci,
	const char* name)
{
	// Sanity check
	if(name == NULL) return NULL;
	if(strlen(name) == 0) return NULL;

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMInstanceGetIntegerValue(NPIHandle* npiHandle, CIMInstance ci,
	const char* name)
{
	// Sanity check
	if(name == NULL) return 0;
	if(strlen(name) == 0) return 0;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMInstanceGetRefValue(NPIHandle* npiHandle, CIMInstance ci, const char* name)
{
	CIMObjectPath cop = {NULL};

	// Sanity check
	if(name == NULL) return cop;
	if(strlen(name) == 0) return cop;

	return cop;
}


// Object Path functions


//////////////////////////////////////////////////////////////////////////////
// empty keyBindings here
extern "C" CIMObjectPath
CIMObjectPathNew(NPIHandle* npiHandle, const char* classname)
{
	Pegasus::KeyBindingArray keyBindings;
	OW_String host;
	OW_String className(classname);
	OW_String nameSpace =
		((NPIenv*)npiHandle->thisObject)->_nameSpace;
	Pegasus::CIMReference cr(host,nameSpace,className,keyBindings);
	Pegasus::CIMObject cimo;
	Pegasus::CIMObjectWithPath* cowp = new Pegasus::CIMObjectWithPath(cr,cimo);
	CIMObjectPath b = {(void*) cowp};
	return b;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathDel(NPIHandle* npiHandle, CIMObjectPath cop)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	delete a;
}

//////////////////////////////////////////////////////////////////////////////
// Call CIMInstance.getClassName
// Loop over CIMInstance.getProperty with CIMInstance.getPropertyCount
// to the the key-value bindings.
extern "C" CIMObjectPath
CIMObjectPathFromCIMInstance(NPIHandle* npiHandle, CIMInstance ci)
{
	Pegasus::CIMInstance* a = (Pegasus::CIMInstance*) ci.ptr;
	OW_String host;
	OW_String className = a->getClassName();
	Pegasus::KeyBindingArray keyBindings;
	Pegasus::CIMProperty prop;
	for(Pegasus::Uint32 i=0;i<a->getPropertyCount();i++)
	{
		prop = a->getProperty(i);
		OW_String value = prop.getValue().toString();
		keyBindings.append(Pegasus::KeyBinding(
			prop.getName(), value, Pegasus::KeyBinding::STRING));
	}
	Pegasus::CIMReference cr(host,
		((NPIenv*)npiHandle->thisObject)->_nameSpace,
		className,keyBindings);
	Pegasus::CIMObjectWithPath* cowp = new Pegasus::CIMObjectWithPath(cr,*a);
	CIMObjectPath c = {(void*)cowp};
	return c;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetClassName(NPIHandle* npiHandle, CIMObjectPath cop)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	char* result = a->getReference().getClassName().allocateCString();
	return result;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetNameSpace(NPIHandle* npiHandle, CIMObjectPath cop)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	cout << a->getReference().getNameSpace();
	char* result = a->getReference().getNameSpace().allocateCString();
	return result;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathSetNameSpace(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* str)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	OW_String nameSpace(str);
	a->getReference().setNameSpace(nameSpace);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void CIMObjectPathSetNameSpaceFromCIMObjectPath(
	NPIHandle* npiHandle, CIMObjectPath cop, CIMObjectPath src)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	Pegasus::CIMObjectWithPath* b = (Pegasus::CIMObjectWithPath*) src.ptr;
	a->getReference().setNameSpace(b->getReference().getNameSpace());
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetStringKeyValue(NPIHandle* npiHandle,
	CIMObjectPath cop, const char* key)
{
	// Sanity check
	if(key == NULL)	return NULL;
	if(strlen(key) == 0) return NULL;

	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	const Pegasus::Array<Pegasus::KeyBinding>& KeyBindings =
		a->getReference().getKeyBindings();
	OW_String Key(key);
	if(a->getReference().getKeyBindings().size() == 0) return NULL;
	for(Pegasus::Uint32 i=0;i < KeyBindings.size();i++)
	{
		if(OW_String::equalNoCase(KeyBindings[i].getName(),Key))
			return KeyBindings[i].getValue().allocateCString();
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddStringKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key, const char* value)
{
	// Sanity check
	if(key == NULL)	return;
	if(strlen(key) == 0) return;

	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
		a->getReference().getKeyBindings();
	OW_String Key(key);
	OW_String Value(value);
	KeyBindings.append(
		Pegasus::KeyBinding(Key,Value,Pegasus::KeyBinding::STRING));
	a->getReference().setKeyBindings(KeyBindings);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMObjectPathGetIntegerKeyValue(NPIHandle* npiHandle, 
	CIMObjectPath cop, const char* key)
{
	// Sanity check
	if(key == NULL)	return -1;
	if(strlen(key) == 0) return -1;

	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
		a->getReference().getKeyBindings();
	OW_String Key(key);
	int k;
	for(Pegasus::Uint32 i=0;i < KeyBindings.size();i++)
	{
		if(KeyBindings[i].getName()==Key)
		{
			char* myvalue = KeyBindings[i].getValue().allocateCString();
			k=atoi(myvalue);
			return k;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddIntegerKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key, const int value)
{
	// Sanity check
	if(key == NULL)	return;
	if(strlen(key) == 0) return;

	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
		a->getReference().getKeyBindings();
	OW_String Key(key);
	char buffer[10];
	sprintf(buffer,"%*d",9,value);
	OW_String Value(buffer);
	KeyBindings.append(
		Pegasus::KeyBinding(Key, Value, Pegasus::KeyBinding::NUMERIC));
	a->getReference().setKeyBindings(KeyBindings);
}


//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMObjectPathGetRefKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key)
{
	// Sanity check

	CIMObjectPath c = {NULL};
	if(key == NULL)
		return c;

	if(strlen(key) == 0)
		return c;

	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	Pegasus::Array<Pegasus::KeyBinding> KeyBindings =
		a->getReference().getKeyBindings();
	OW_String Key(key);
	for(Pegasus::Uint32 i=0;i < KeyBindings.size();i++)
	{
		if(KeyBindings[i].getName()==Key)
		{
			Pegasus::CIMReference cr(
				KeyBindings[i].getValue().allocateCString());
			Pegasus::CIMObject cimo;
			Pegasus::CIMObjectWithPath* cowp = new Pegasus::CIMObjectWithPath(
				cr,cimo);
			// ERROR ERROR ERROR
			c.ptr = cowp;
			return c;
		}
	}
	return c;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddRefKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key, CIMObjectPath cop2)
{
	// Sanity check
	if(key == NULL)
		return;

	if(strlen(key) == 0)
		return;

	DDD( cerr << "NPIProvider: CIMObjectPathAddRefKeyValue"; )
	DDD( cerr << "NPIProvider: ERROR - cannot be implemented !!!"; )
}

// CIMOM functions

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMClass
CIMOMGetClass(NPIHandle* npiHandle, CIMObjectPath cop, int localOnly)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	OW_String nameSpace = a->getReference().getNameSpace();
	OW_String className = a->getReference().getClassName();
	Pegasus::CIMClass* cc = new Pegasus::CIMClass(
		NPI_getmyClass(npiHandle,nameSpace,className));
	OW_String myclassName = cc->getClassName();
	CIMClass localcc = {cc};
	return localcc;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" Vector
CIMOMEnumInstanceNames(NPIHandle* npiHandle, CIMObjectPath cop, int i)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	OW_String nameSpace = a->getReference().getNameSpace();
	OW_String className = a->getReference().getClassName();
	Pegasus::Array<Pegasus::CIMReference> instNames;

	instNames = NPI_enumeratemyInstanceNames(npiHandle,nameSpace,className);
	Pegasus::CIMObjectWithPath* cowp;
	Pegasus::CIMObject cimo;
	Vector v = VectorNew(npiHandle);
	for(int i=instNames.size()-1;i>=0;i--)
	{
		cowp = new Pegasus::CIMObjectWithPath(instNames[i],cimo);
		_VectorAddTo(npiHandle, v, (void*)cowp);
	}
	return v;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" Vector
CIMOMEnumInstances(NPIHandle* npiHandle, CIMObjectPath cop, int i, int j)
{
	Pegasus::CIMObjectWithPath* a = (Pegasus::CIMObjectWithPath*) cop.ptr;
	OW_String nameSpace = a->getReference().getNameSpace();
	OW_String className = a->getReference().getClassName();
	Pegasus::Array<Pegasus::CIMNamedInstance> insts;

	insts = NPI_enumeratemyInstances(npiHandle,nameSpace,className);
	Pegasus::CIMInstance* ci;
	Vector v = VectorNew(npiHandle);
	for(int i=insts.size()-1;i>=0;i--)
	{
		ci = new Pegasus::CIMInstance(insts[i].getInstance());
		_VectorAddTo(npiHandle, v, (void*)ci);
	}
	return v;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMInstance
CIMOMGetInstance(NPIHandle* npiHandle, CIMObjectPath cop, int i)
{
	CIMInstance ci = {(void*)NULL};
	return ci;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
_ObjectToString(NPIHandle* npiHandle, void* co)
{
	Pegasus::Array<Pegasus::Sint8> xml;
	Pegasus::CIMObjectWithPath* powp;
	Pegasus::CIMInstance* pci;
	Pegasus::CIMClass* pcc;
	powp = (Pegasus::CIMObjectWithPath*)co;
	powp->getReference().toXml(xml);
	//if ( (powp = dynamic_cast<Pegasus::CIMObjectWithPath*>(co)) )
	//   powp->toXml(xml);
	//else if ( (pci = dynamic_cast<Pegasus::CIMInstance*>(co)) )
	//   pci->toXml(xml);
	//else if ( (pcc = dynamic_cast<Pegasus::CIMClass*>(co)) )
	//   pcc->toXml(xml);
	//else return NULL;   
	int sz = xml.size();
	char* retval = (char*)malloc(sz+1);
	retval[sz] = '\0';
	for(int i = 0; i<sz; i++) retval[i]=xml[i];
	return retval;
}

// Error handling classes

//////////////////////////////////////////////////////////////////////////////
extern "C" int
errorCheck(NPIHandle* npiHandle )
{
	return npiHandle->errorOccurred;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void 
raiseError(NPIHandle* npiHandle, const char* msg)
{
	if(npiHandle->providerError != NULL)
		free ( (void*) npiHandle->providerError );
	npiHandle->providerError = strdup ( msg );
}

//static void raiseNPIException( void* env, char* msg) {}
//static void throwProviderError (NPIHandle* npiHandle) {}
