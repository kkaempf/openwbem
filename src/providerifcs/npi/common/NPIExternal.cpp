#include <cstdio>
#include "OW_config.h"
#include "NPIExternal.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_Format.hpp"

#include "OW_FTABLERef.hpp"

#include "OW_CIMOMEnvironment.hpp"
#include "OW_LocalCIMOMHandle.hpp"

#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"


// Garbage Collection helper functions
void _NPIGarbageCan(NPIHandle * nh, void * object, NPIGarbageType type)
{
   ((NPIContext *)(nh->context))->garbage.append(object);
   ((NPIContext *)(nh->context))->garbageType.append(type);
}

void _NPIGarbageRetrieve(NPIHandle * nh, void * object)
{
   for(int i = ((NPIContext *)(nh->context))->garbage.size()-1;i >=0;i--)
   {
      if ( ((NPIContext *)(nh->context))->garbage[i] == object)
         ((NPIContext *)(nh->context))->garbageType[i] = NOTHING;
   }
}

// administrative functions

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_CIMClass
NPI_getmyClass(NPIHandle* npiHandle, const OW_String& nameSpace,
	const OW_String& className)
{
	OW_ProviderEnvironmentIFCRef * provenv =
		static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);

	OW_CIMClass cc(OW_CIMNULL);
	try
	{
		cc = (*provenv)->getCIMOMHandle()->getClass(
			nameSpace, className,
			OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
			OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS,
			OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN, NULL);
	}
	catch (...)
	{
		// cerr << "Class or Namespace do not exist\n";
		// TODO: log this, and catch the correct exception.
		npiHandle->errorOccurred = 1;
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_CIMObjectPathEnumeration
NPI_enumeratemyInstanceNames(NPIHandle* npiHandle,
	const OW_String& nameSpace, const OW_String& className)
{
	OW_ProviderEnvironmentIFCRef * provenv =
		static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);

	OW_CIMObjectPathEnumeration crefs;
	try
	{
		crefs =
			(*provenv)->getCIMOMHandle()->enumInstanceNamesE(nameSpace, className);
	}
	catch (...)
	{
		// cerr << "Class or Namespace do not exist\n";
		// TODO: log this, and catch the correct exception.
		npiHandle->errorOccurred = 1;
	}
	return crefs;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_CIMInstanceEnumeration
NPI_enumeratemyInstances(NPIHandle* npiHandle, const OW_String& nameSpace,
	const OW_String& className)
{
	OW_ProviderEnvironmentIFCRef * provenv =
		static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);

	OW_CIMInstanceEnumeration cinsts;
	try
	{
		cinsts = (*provenv)->getCIMOMHandle()->enumInstancesE(
			nameSpace, className, OW_CIMOMHandleIFC::DEEP,
			OW_CIMOMHandleIFC::NOT_LOCAL_ONLY,
			OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS,
			OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN, NULL);
	}
	catch (...)
	{
		// cerr << "Class or Namespace do not exist\n";
		// TODO: log this, and catch the correct exception.
		npiHandle->errorOccurred = 1;
	}

	return cinsts;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" OW_CIMInstance
NPI_getmyInstance(NPIHandle* npiHandle, const OW_CIMObjectPath& owcop,
	const int localOnly)
{
	OW_ProviderEnvironmentIFCRef * provenv =
		static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);

	OW_CIMInstance ci(OW_CIMNULL);

	try
	{
		ci = (*provenv)->getCIMOMHandle()->getInstance(owcop.getNameSpace(),
			owcop, localOnly);
	}
	catch (...)
	{
		// cerr << "Instance does not exist\n";
		// TODO: log this, and catch the correct exception.
		npiHandle->errorOccurred = 1;
	}

	return ci;
}


// externalized functions

//////////////////////////////////////////////////////////////////////////////
extern "C" Vector
VectorNew(NPIHandle* npiHandle)
{
	(void)npiHandle;
	Vector v;
	v.ptr = (void*) new charVect;
	_NPIGarbageCan(npiHandle, v.ptr, VECTOR);
	return v;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
_VectorAddTo(NPIHandle* npiHandle, Vector v, void* obj)
{
	(void)npiHandle;
	((charVect*)v.ptr)->append((char*)obj);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
VectorSize(NPIHandle* npiHandle, Vector v)
{
	(void)npiHandle;
	return((charVect*)v.ptr)->size();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void*
_VectorGet(NPIHandle* npiHandle, Vector v, int pos)
{
	(void)npiHandle;
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
	switch (pct)
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
	(void)npiHandle;
	OW_CIMValue* pcv = new OW_CIMValue(OW_String(val));
	CIMValue cv = { (void*) pcv};

	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMValue
CIMValueNewInteger(NPIHandle* npiHandle, int val)
{
	(void)npiHandle;
	OW_CIMValue* pcv = new OW_CIMValue(val);
	CIMValue cv = { (void*) pcv};

	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMValue
CIMValueNewRef(NPIHandle* npiHandle, CIMObjectPath cop)
{
	(void)npiHandle;
	OW_CIMValue* pcv = new OW_CIMValue(*((OW_CIMObjectPath*)cop.ptr));
	CIMValue cv = { (void*) pcv};

	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMValueGetString(NPIHandle* npiHandle, CIMValue cv)
{
	(void)npiHandle;
	OW_CIMValue* pcv = (OW_CIMValue*)cv.ptr;
	OW_String mystring = pcv->toString();
	return mystring.allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMValueGetInteger(NPIHandle* npiHandle, CIMValue cv)
{
	(void)npiHandle;
	OW_CIMValue* pcv = (OW_CIMValue*)cv.ptr;
	int retval;
	pcv->get(retval);
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMValueGetRef(NPIHandle* npiHandle, CIMValue cv)
{
	(void)npiHandle;
	OW_CIMValue* pcv = (OW_CIMValue*) cv.ptr;
	OW_CIMObjectPath cref(OW_CIMNULL);
	pcv->get(cref);
	OW_CIMObjectPath* ncop = new OW_CIMObjectPath(cref);
	CIMObjectPath cop = {(void*) ncop};

	_NPIGarbageCan(npiHandle, (void *) ncop, CIM_OBJECTPATH);
	return cop;
}

// CIMParameter functions

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMType
CIMParameterGetType(NPIHandle* npiHandle, CIMParameter cp)
{
	(void)npiHandle;
	OW_CIMParamValue* pcp = (OW_CIMParamValue*)cp.ptr;
	int dt = pcp->getValue().getType();
	//switch(dt.getType())
	switch (dt)
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
	(void)npiHandle;
	OW_CIMParamValue* pcp = (OW_CIMParamValue*)cp.ptr;
	return pcp->getName().allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMParameter
CIMParameterNewString(NPIHandle* npiHandle, const char* name, const char* value)
{
	(void)npiHandle;
	CIMParameter mycp = { NULL};

	// Sanity check
	if (name == NULL)
		return mycp;

	if (strlen(name) == 0)
		return mycp;

	OW_CIMParamValue* pcp = new OW_CIMParamValue(OW_String(name),
		OW_CIMValue(OW_String(value)));
	mycp.ptr = pcp;

	_NPIGarbageCan(npiHandle, (void *) mycp.ptr, CIM_PARAMVALUE);
	return mycp;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMParameter
CIMParameterNewInteger(NPIHandle* npiHandle, const char* name, int value)
{
	(void)npiHandle;
	CIMParameter mycp = { NULL};

	// Sanity check
	if (name == NULL)
		return mycp;

	if (strlen(name) == 0)
		return mycp;

	OW_CIMParamValue * pcp = new OW_CIMParamValue(OW_String(name),
		OW_CIMValue(OW_Int32(value)));
	mycp.ptr = pcp;

	_NPIGarbageCan(npiHandle, (void *) mycp.ptr, CIM_PARAMVALUE);
	return mycp;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMParameter
CIMParameterNewRef(NPIHandle* npiHandle, const char* name, CIMObjectPath value)
{
	(void)npiHandle;
	CIMParameter mycp = { NULL};

	// Sanity check
	if (name == NULL)
		return mycp;

	if (strlen(name) == 0)
		return mycp;

	OW_CIMValue val(*((OW_CIMObjectPath*)value.ptr));
	mycp.ptr = new OW_CIMParamValue(OW_String(name), val);

	_NPIGarbageCan(npiHandle, (void *) mycp.ptr, CIM_PARAMVALUE);
	return mycp;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMParameterGetString(NPIHandle* npiHandle, CIMParameter cp)
{
	(void)npiHandle;
	OW_CIMParamValue* pcpv = static_cast<OW_CIMParamValue *> (cp.ptr);

	OW_String value = pcpv->getValue().toString();

	return value.allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMParameterGetIntegerValue(NPIHandle* npiHandle, CIMParameter cp)
{
	(void)npiHandle;
	OW_CIMParamValue* pcpv = (OW_CIMParamValue*)cp.ptr;
	int value;
	pcpv->getValue().get(value);
	return value;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMParameterGetRefValue(NPIHandle* npiHandle, CIMParameter cp)
{
	(void)npiHandle;
	OW_CIMParamValue* pcpv = (OW_CIMParamValue*)cp.ptr;
	OW_CIMObjectPath op(OW_CIMNULL);
	OW_CIMValue val = pcpv->getValue();
	val.get(op);
	OW_CIMObjectPath * pop = new OW_CIMObjectPath(op);
	CIMObjectPath cop = { (void*) pop};

	_NPIGarbageCan(npiHandle, (void *) pop, CIM_OBJECTPATH);
	return cop;
}


// Instance functions

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMInstance
CIMClassNewInstance(NPIHandle* npiHandle, CIMClass cc)
{
	(void)npiHandle;
	OW_CIMClass * owcc = static_cast<OW_CIMClass *>(cc.ptr);

	OW_CIMInstance * owci = new OW_CIMInstance(owcc->newInstance());

	CIMInstance ci = {static_cast<void *>(owci)};

	_NPIGarbageCan(npiHandle, (void *) owci, CIM_INSTANCE);
	return ci;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetStringProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const char* value )
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_String Key(name);
	OW_String Val;

	if (value)
	{
		if (strlen(value)>0)
		{
			Val = OW_String(value);
		}
		else
		{
			Val = OW_String("-empty-");
		}
	}
	else
	{
		Val = OW_String("-empty-");
	}


	OW_CIMValue Value(Val);
	owci->setProperty(Key,Value);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetIntegerProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const int value)
{
	(void)npiHandle;

	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	owci->setProperty(OW_String(name),OW_CIMValue(value));
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetLongProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const long long value)
{
	(void)npiHandle;

	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_String Key(name);

	OW_CIMValue Value(value);

	owci->setProperty(Key,Value);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetBooleanProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, const unsigned char value)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_String Key(name);

	OW_CIMValue Value(OW_Bool((int)value));

	owci->setProperty(Key,Value);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetRefProperty(NPIHandle* npiHandle, CIMInstance ci,
	const char* name, CIMObjectPath value)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_CIMObjectPath * owcop = static_cast<OW_CIMObjectPath *> (value.ptr);

	OW_String Key(name);

	OW_CIMValue Value(*owcop);

	owci->setProperty(Key,Value);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMInstanceGetStringValue(NPIHandle* npiHandle, CIMInstance ci,
	const char* name)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return NULL;
	if (strlen(name) == 0) return NULL;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_String Key(name);

	OW_CIMProperty prop = owci->getProperty(Key);
	OW_CIMValue cv = prop.getValue();

	if (cv.getType() != OW_CIMDataType::STRING)	return NULL;

	cv.get(Key);

	return Key.allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMInstanceGetIntegerValue(NPIHandle* npiHandle, CIMInstance ci,
	const char* name)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return 0;
	if (strlen(name) == 0) return 0;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_String Key(name);
	OW_CIMProperty prop = owci->getProperty(Key);
	OW_CIMValue cv = prop.getValue();

	switch (cv.getType())
	{
		case OW_CIMDataType::UINT8: {OW_UInt8 i; cv.get(i); return i; break;}
		case OW_CIMDataType::SINT8: {OW_Int8 i; cv.get(i); return i; break;}
		case OW_CIMDataType::UINT16: {OW_UInt16 i; cv.get(i); return i; break;}
		case OW_CIMDataType::SINT16: {OW_Int16 i; cv.get(i); return i; break;}
		case OW_CIMDataType::UINT32: {OW_UInt32 i; cv.get(i); return i; break;}
		case OW_CIMDataType::SINT32: {OW_Int32 i; cv.get(i); return i; break;}
		case OW_CIMDataType::UINT64: {OW_UInt64 i; cv.get(i); return i; break;}
		case OW_CIMDataType::SINT64: {OW_Int64 i; cv.get(i); return i; break;}
		case OW_CIMDataType::BOOLEAN: {OW_Bool i; cv.get(i); return (i?-1:0); break;}
		default: return 0;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMInstanceGetRefValue(NPIHandle* npiHandle, CIMInstance ci, const char* name)
{
	(void)npiHandle;
	CIMObjectPath cop = {NULL};

	// Sanity check
	if (name == NULL) return cop;
	if (strlen(name) == 0) return cop;

	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_String Key(name);
	OW_CIMProperty prop = owci->getProperty(Key);
	OW_CIMValue cv = prop.getValue();

	if (cv.getType() != OW_CIMDataType::REFERENCE) return cop;

	OW_CIMObjectPath owcop(OW_CIMNULL);
	cv.get(owcop);

	cop.ptr = static_cast<void *>(&owcop);

	return cop;
}


// Object Path functions


//////////////////////////////////////////////////////////////////////////////
// empty keyBindings here
extern "C" CIMObjectPath
CIMObjectPathNew(NPIHandle* npiHandle, const char* classname)
{
	(void)npiHandle;
	OW_String className(classname);
	OW_CIMObjectPath * ref = new OW_CIMObjectPath(className);

	CIMObjectPath cop = { static_cast<void *> (ref)};

	_NPIGarbageCan(npiHandle, (void *) ref, CIM_OBJECTPATH);
	return cop;
}

//////////////////////////////////////////////////////////////////////////////
// Call CIMInstance.getClassName
// Loop over CIMInstance.getProperty with CIMInstance.getPropertyCount
// to the the key-value bindings.
extern "C" CIMObjectPath
CIMObjectPathFromCIMInstance(NPIHandle* npiHandle, CIMInstance ci)
{
	(void)npiHandle;
	OW_CIMInstance * owci = static_cast<OW_CIMInstance *>(ci.ptr);

	OW_String host;

	OW_CIMObjectPath * ref = new OW_CIMObjectPath(*owci);

	CIMObjectPath cop = { static_cast<void *>(ref)};

	_NPIGarbageCan(npiHandle, (void *) ref, CIM_OBJECTPATH);
	return cop;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetClassName(NPIHandle* npiHandle, CIMObjectPath cop)
{
	(void)npiHandle;
	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *> (cop.ptr);

	return ref->getObjectName().allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetNameSpace(NPIHandle* npiHandle, CIMObjectPath cop)
{
	(void)npiHandle;
	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	return ref->getNameSpace().allocateCString();
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathSetNameSpace(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* str)
{
	(void)npiHandle;
	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	ref->setNameSpace(OW_String(str));
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void CIMObjectPathSetNameSpaceFromCIMObjectPath(
	NPIHandle* npiHandle, CIMObjectPath cop, CIMObjectPath src)
{
	(void)npiHandle;
	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);
	OW_CIMObjectPath * rsrc = static_cast<OW_CIMObjectPath *>(src.ptr);

	ref->setNameSpace(rsrc->getNameSpace());
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetStringKeyValue(NPIHandle* npiHandle,
	CIMObjectPath cop, const char* key)
{
	(void)npiHandle;

	// Sanity check
	if (key == NULL) return NULL;
	if (strlen(key) == 0) return NULL;

	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	OW_CIMPropertyArray props = ref->getKeys();
	OW_String Key(key);

	for (int i = props.size()-1; i >= 0; i--)
	{
		OW_CIMProperty cp = props[i];
		if (cp.getName().equalsIgnoreCase(Key))
		{
			OW_CIMValue cv = cp.getValue();
			if (!cv) return NULL;
			if (cv.getType() != OW_CIMDataType::STRING)
				return NULL;
			cv.get(Key);
			return Key.allocateCString();
		}
	}

	return NULL;
}

/* ====================================================================== */
void _CIMObjectPathAddKey(OW_CIMObjectPath * ref,
           const OW_String& Key, const OW_CIMValue & Value)
{
   if (ref->getKey(Key))
   {
      OW_Bool b = false;
      OW_CIMPropertyArray cprops = ref->getKeys();
      for(OW_Int32 i=cprops.size()-1; i >= 0; i--)
      {
       if (cprops[i].getName().equalsIgnoreCase(Key))
        {
            cprops[i].setValue(Value);
            b = true;
         }
      }
      if (b)
      {
         ref->setKeys(cprops);
         return;
      }
   }
   ref->addKey(Key,Value);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddStringKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key, const char* value)
{
	(void)npiHandle;

	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;

	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	OW_String Key(key);
	OW_String Val(value);

	OW_CIMValue Value(Val);

	_CIMObjectPathAddKey(ref, Key, Value);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMObjectPathGetIntegerKeyValue(NPIHandle* npiHandle,
	CIMObjectPath cop, const char* key)
{
	(void)npiHandle;

	// Sanity check
	if (key == NULL) return -1;
	if (strlen(key) == 0) return -1;

	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	OW_CIMPropertyArray props = ref->getKeys();
	OW_String Key(key);

	//for (int i = 0, n = props.size(); i < n; i++)
	for (int i = props.size()-1; i >= 0; i--)
	{
		OW_CIMProperty cp = props[i];
		if (cp.getName().equalsIgnoreCase(Key))
		{
			OW_CIMValue cv = cp.getValue();

			if (!cv) return 0;
			switch (cv.getType())
			{
				case OW_CIMDataType::UINT8:
					{OW_UInt8 i; cv.get(i); return i; break;}
				case OW_CIMDataType::SINT8:
					{OW_Int8 i; cv.get(i); return i; break;}
				case OW_CIMDataType::UINT16:
					{OW_UInt16 i; cv.get(i); return i; break;}
				case OW_CIMDataType::SINT16:
					{OW_Int16 i; cv.get(i); return i; break;}
				case OW_CIMDataType::UINT32:
					{OW_UInt32 i; cv.get(i); return i; break;}
				case OW_CIMDataType::SINT32:
					{OW_Int32 i; cv.get(i); return i; break;}
				case OW_CIMDataType::UINT64:
					{OW_UInt64 i; cv.get(i); return i; break;}
				case OW_CIMDataType::SINT64:
					{OW_Int64 i; cv.get(i); return i; break;}
				default: return 0;
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddIntegerKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key, const int value)
{
	(void)npiHandle;

	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;

	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	OW_String Key(key);

	OW_CIMValue Value(value);

	_CIMObjectPathAddKey(ref, Key, Value);
}


//////////////////////////////////////////////////////////////////////////////
extern "C" CIMObjectPath
CIMObjectPathGetRefKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key)
{
	(void)npiHandle;
	// Sanity check

	CIMObjectPath cop2 = {NULL};
	if (key == NULL) return cop2;
	if (strlen(key) == 0) return cop2;

	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	OW_CIMPropertyArray props = ref->getKeys();
	OW_String Key(key);

	//for (int i = 0, n = props.size(); i < n; i++)
	for (int i = props.size()-1; i >= 0; i--)
	{
		OW_CIMProperty cp = props[i];
		if (cp.getName().equalsIgnoreCase(Key))
		{
			OW_CIMValue cv = cp.getValue();

			if (!cv) return cop2;
			if (cv.getType() != OW_CIMDataType::REFERENCE) return cop2;

			OW_CIMObjectPath * ref2 = new OW_CIMObjectPath(OW_CIMNULL);
			cv.get(*ref2);

			cop2.ptr = (void *) ref;

			_NPIGarbageCan(npiHandle,(void *)ref2,CIM_OBJECTPATH);
			return cop2;
		}
	}

	return cop;

}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddRefKeyValue(NPIHandle* npiHandle, CIMObjectPath cop,
	const char* key, CIMObjectPath cop2)
{
	(void)npiHandle;

	// Sanity check
	if (key == NULL) return;

	if (strlen(key) == 0) return;

	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);
	OW_CIMObjectPath * ref2 = static_cast<OW_CIMObjectPath *>(cop2.ptr);

	OW_String Key(key);

	OW_CIMValue Value(*ref2);

	_CIMObjectPathAddKey(ref, Key, Value);
}

// SelectExp functions

//////////////////////////////////////////////////////////////////////////////
extern "C" char * 
SelectExpGetSelectString(NPIHandle* npiHandle, SelectExp sxp)
{
	(void)npiHandle;

	char * query = static_cast<char *>(sxp.ptr);
	return query;
}

// CIMOM functions

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMClass
CIMOMGetClass(NPIHandle* npiHandle, CIMObjectPath cop, int localOnly)
{
	(void)localOnly;
	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	OW_String nameSpace = ref->getNameSpace();

	OW_String className = ref->getObjectName();

	OW_CIMClass cc = NPI_getmyClass(npiHandle, nameSpace, className);

	OW_CIMClass * my_cc = new OW_CIMClass(cc);

	CIMClass localcc = { static_cast<void *> (my_cc)};

	_NPIGarbageCan(npiHandle, (void *) my_cc, CIM_CLASS);
	return localcc;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" Vector
CIMOMEnumInstanceNames(NPIHandle* npiHandle, CIMObjectPath cop, int i)
{
	(void)i;
	OW_CIMObjectPath * ref = (OW_CIMObjectPath *) cop.ptr;

	OW_String nameSpace = ref->getNameSpace();

	OW_String className = ref->getObjectName();

	OW_CIMObjectPathEnumeration instNames =
		NPI_enumeratemyInstanceNames(npiHandle,nameSpace,className);

	// Full Copy
	Vector v = VectorNew(npiHandle);

	while (instNames.hasMoreElements())
	{
		OW_CIMObjectPath * cowp = new
			OW_CIMObjectPath(instNames.nextElement());
		_NPIGarbageCan(npiHandle, (void *) cowp, CIM_OBJECTPATH);
		_VectorAddTo(npiHandle, v, (void *) cowp);
	}

	return v;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" Vector
CIMOMEnumInstances(NPIHandle* npiHandle, CIMObjectPath cop, int i, int j)
{
	(void)i;
	(void)j;
	OW_CIMObjectPath * ref = (OW_CIMObjectPath *) cop.ptr;

	OW_String nameSpace = ref->getNameSpace();

	OW_String className = ref->getObjectName();

	OW_CIMInstanceEnumeration insts =
		NPI_enumeratemyInstances(npiHandle,nameSpace,className);

	// Full Copy
	Vector v = VectorNew(npiHandle);
	while (insts.hasMoreElements())
	{
		OW_CIMInstance * ci = new OW_CIMInstance(insts.nextElement());
		_NPIGarbageCan(npiHandle, (void *) ci, CIM_INSTANCE);
		_VectorAddTo(npiHandle, v, (void *) ci);
	}
	return v;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" CIMInstance
CIMOMGetInstance(NPIHandle* npiHandle, CIMObjectPath cop, int i)
{
	OW_CIMObjectPath * ref = static_cast<OW_CIMObjectPath *>(cop.ptr);

	OW_CIMInstance ci = NPI_getmyInstance(npiHandle, *ref, i);

	OW_CIMInstance * my_ci = new OW_CIMInstance(ci);

	_NPIGarbageCan(npiHandle, (void *) my_ci, CIM_INSTANCE);
	CIMInstance localci = { static_cast<void *> (my_ci)};

	return localci;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMDeliverProcessEvent(NPIHandle* npiHandle, char * ns,
		CIMInstance indication)
{
	(void)ns;
	OW_ProviderEnvironmentIFCRef * provenv =
		static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);

	OW_CIMInstance * ow_indication =
           static_cast<OW_CIMInstance *>(indication.ptr);

	try
	{
		(*provenv)->getCIMOMHandle()->exportIndication(
			* ow_indication, OW_String("root/cimv2"));
	}
	catch (...)
	{
		// cerr << "Whatever the cause it went wrong\n";
		// TODO: log this, and catch the correct exception.
		npiHandle->errorOccurred = 1;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMDeliverInstanceEvent(NPIHandle* npiHandle, char * ns,
			CIMInstance indication, 
                          CIMInstance source, CIMInstance previous)
{
	(void)ns;
	OW_ProviderEnvironmentIFCRef * provenv =
		static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
	OW_CIMInstance * ow_indication =
           static_cast<OW_CIMInstance *>(indication.ptr);
	OW_CIMInstance * ow_source = static_cast<OW_CIMInstance *>(source.ptr);
	OW_CIMInstance * ow_previous =
           static_cast<OW_CIMInstance *>(previous.ptr);

        OW_CIMValue src_val(* ow_source);
        OW_CIMValue prev_val(* ow_previous);

	ow_indication->setProperty(OW_String("SourceInstance"), src_val);
           
	ow_indication->setProperty(OW_String("PreviousInstance"), prev_val);

	try
	{
		(*provenv)->getCIMOMHandle()->exportIndication(
			* ow_indication, OW_String("root/cimv2"));
	}
	catch (...)
	{
		// cerr << "Whatever the cause it went wrong\n";
		// TODO: log this, and catch the correct exception.
		npiHandle->errorOccurred = 1;
	}
	(*provenv)->getLogger()->logDebug(format("NPIExternal: Deliver %1", npiHandle->errorOccurred));
}
//////////////////////////////////////////////////////////////////////////////
extern "C" NPIHandle *
CIMOMPrepareAttach(NPIHandle* npiHandle)
{
    ::NPIHandle * nh = new ::NPIHandle(*npiHandle);
        // clone the providerenvironment
    OW_ProviderEnvironmentIFCRef * provenv =
        static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
    nh->thisObject = new OW_ProviderEnvironmentIFCRef(*provenv);
    // need to worry about errorOccurred and providerError???   
    return nh;
}


//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMCancelAttach(NPIHandle* npiHandle)
{
    delete static_cast<OW_ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
    if (npiHandle->providerError != NULL)
        free((void *)(npiHandle->providerError));
    free(npiHandle);
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMAttachThread(NPIHandle* npiHandle)
{
	if (npiHandle == NULL) return;
	npiHandle->errorOccurred = 0;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMDetachThread(NPIHandle* npiHandle)
{
	if (npiHandle == NULL) return;
	npiHandle->errorOccurred = 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
_ObjectToString(NPIHandle* npiHandle, void* co)
{
	(void)npiHandle;
	(void)co;
	return 0;
// is not supported
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
errorReset(NPIHandle* npiHandle )
{
	npiHandle->errorOccurred = 0;
}

//////////////////////////////////////////////////////////////////////////////
extern "C" void
raiseError(NPIHandle* npiHandle, const char* msg)
{
	if (npiHandle->providerError != NULL)
		free ( (void*) npiHandle->providerError );

	npiHandle->errorOccurred = 1;
	npiHandle->providerError = strdup ( msg );
}

//static void raiseNPIException( void* env, char* msg) {}
//static void throwProviderError (NPIHandle* npiHandle) {}
