#include <cstdio>
#include "OW_config.h"
#include "NPIExternal.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_Format.hpp"
#include "OW_FTABLERef.hpp"
#include "OW_NPIProviderIFCUtils.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_LocalCIMOMHandle.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_WQLSelectStatement.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
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
extern "C" CIMClass
NPI_getmyClass(NPIHandle* npiHandle, const String& nameSpace,
	const String& className)
{
	ProviderEnvironmentIFCRef * provenv =
		static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
	CIMClass cc(CIMNULL);
	try
	{
		cc = (*provenv)->getCIMOMHandle()->getClass(
			nameSpace, className,
			E_NOT_LOCAL_ONLY,
			E_INCLUDE_QUALIFIERS,
			E_INCLUDE_CLASS_ORIGIN, NULL);
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
extern "C" CIMObjectPathEnumeration
NPI_enumeratemyInstanceNames(NPIHandle* npiHandle,
	const String& nameSpace, const String& className)
{
	ProviderEnvironmentIFCRef * provenv =
		static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
	CIMObjectPathEnumeration crefs;
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
extern "C" CIMInstanceEnumeration
NPI_enumeratemyInstances(NPIHandle* npiHandle, const String& nameSpace,
	const String& className)
{
	ProviderEnvironmentIFCRef * provenv =
		static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
	CIMInstanceEnumeration cinsts;
	try
	{
		cinsts = (*provenv)->getCIMOMHandle()->enumInstancesE(
			nameSpace, className, 
			E_DEEP,
			E_NOT_LOCAL_ONLY,
			E_EXCLUDE_QUALIFIERS,
			E_EXCLUDE_CLASS_ORIGIN, NULL);
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
extern "C" CIMInstance
NPI_getmyInstance(NPIHandle* npiHandle, const CIMObjectPath& owcop,
	const int localOnly)
{
	ProviderEnvironmentIFCRef * provenv =
		static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
	CIMInstance ci(CIMNULL);
	try
	{
		ci = (*provenv)->getCIMOMHandle()->getInstance(owcop.getNameSpace(),
			owcop, localOnly ? E_LOCAL_ONLY : E_NOT_LOCAL_ONLY);
	}
	catch (...)
	{
		// cerr << "Instance does not exist\n";
		// TODO: log this, and catch the correct exception.
		npiHandle->errorOccurred = 1;
	}
	return ci;
}

} // end namespace OpenWBEM

using namespace OpenWBEM;

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
CIMValueGetType(NPIHandle*, ::CIMValue cv)
{
	OpenWBEM::CIMValue* pcv = (OpenWBEM::CIMValue*)cv.ptr;
	int pct = pcv->getType();
	switch (pct)
	{
		case CIMDataType::BOOLEAN :
		case CIMDataType::UINT8:
		case CIMDataType::SINT8:
		case CIMDataType::UINT16:
		case CIMDataType::SINT16:
		case CIMDataType::UINT32:
		case CIMDataType::SINT32:
		case CIMDataType::UINT64:
		case CIMDataType::SINT64:
		case CIMDataType::REAL32:
		case CIMDataType::REAL64:
		case CIMDataType::DATETIME:
			return CIM_INTEGER;
		case CIMDataType::CHAR16:
		case CIMDataType::STRING:
			return CIM_STRING;
		case CIMDataType::REFERENCE:
			return CIM_REF;
	}
	return CIM_INTEGER;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMValue
CIMValueNewString(NPIHandle* npiHandle, const char* val)
{
	(void)npiHandle;
	OpenWBEM::CIMValue* pcv = new OpenWBEM::CIMValue(String(val));
	::CIMValue cv = { (void*) pcv};
	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMValue
CIMValueNewInteger(NPIHandle* npiHandle, int val)
{
	(void)npiHandle;
	OpenWBEM::CIMValue* pcv = new OpenWBEM::CIMValue(val);
	::CIMValue cv = { (void*) pcv};
	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMValue
CIMValueNewRef(NPIHandle* npiHandle, ::CIMObjectPath cop)
{
	(void)npiHandle;
	OpenWBEM::CIMValue* pcv = new OpenWBEM::CIMValue(*((OpenWBEM::CIMObjectPath*)cop.ptr));
	::CIMValue cv = { (void*) pcv};
	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMValueGetString(NPIHandle* npiHandle, ::CIMValue cv)
{
	(void)npiHandle;
	OpenWBEM::CIMValue* pcv = (OpenWBEM::CIMValue*)cv.ptr;
	String mystring = pcv->toString();
	return mystring.allocateCString();
}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMValueGetInteger(NPIHandle* npiHandle, ::CIMValue cv)
{
	(void)npiHandle;
	OpenWBEM::CIMValue* pcv = (OpenWBEM::CIMValue*)cv.ptr;
	int retval;
	pcv->get(retval);
	return retval;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMValueGetRef(NPIHandle* npiHandle, ::CIMValue cv)
{
	(void)npiHandle;
	OpenWBEM::CIMValue* pcv = (OpenWBEM::CIMValue*) cv.ptr;
	OpenWBEM::CIMObjectPath cref(CIMNULL);
	pcv->get(cref);
	OpenWBEM::CIMObjectPath* ncop = new OpenWBEM::CIMObjectPath(cref);
	::CIMObjectPath cop = {(void*) ncop};
	_NPIGarbageCan(npiHandle, (void *) ncop, CIM_OBJECTPATH);
	return cop;
}
// CIMParameter functions
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMType
CIMParameterGetType(NPIHandle* npiHandle, ::CIMParameter cp)
{
	(void)npiHandle;
	OpenWBEM::CIMParamValue* pcp = (OpenWBEM::CIMParamValue*)cp.ptr;
	int dt = pcp->getValue().getType();
	//switch(dt.getType())
	switch (dt)
	{
		case CIMDataType::BOOLEAN :
		case CIMDataType::UINT8 :
		case CIMDataType::SINT8 :
		case CIMDataType::UINT16 :
		case CIMDataType::SINT16 :
		case CIMDataType::UINT32 :
		case CIMDataType::SINT32 :
		case CIMDataType::UINT64 :
		case CIMDataType::SINT64 :
		case CIMDataType::REAL32 :
		case CIMDataType::REAL64 :
		case CIMDataType::DATETIME :
			return CIM_INTEGER;
		case CIMDataType::CHAR16 :
		case CIMDataType::STRING :
			return CIM_STRING;
		case CIMDataType::REFERENCE :
			return CIM_REF;
	}
	return CIM_INTEGER;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMParameterGetName(NPIHandle* npiHandle, ::CIMParameter cp)
{
	(void)npiHandle;
	CIMParamValue* pcp = (CIMParamValue*)cp.ptr;
	return pcp->getName().allocateCString();
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMParameter
CIMParameterNewString(NPIHandle* npiHandle, const char* name, const char* value)
{
	(void)npiHandle;
	::CIMParameter mycp = { NULL};
	// Sanity check
	if (name == NULL)
		return mycp;
	if (strlen(name) == 0)
		return mycp;
	CIMParamValue* pcp = new CIMParamValue(String(name),
		OpenWBEM::CIMValue(String(value)));
	mycp.ptr = pcp;
	_NPIGarbageCan(npiHandle, (void *) mycp.ptr, CIM_PARAMVALUE);
	return mycp;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMParameter
CIMParameterNewInteger(NPIHandle* npiHandle, const char* name, int value)
{
	(void)npiHandle;
	::CIMParameter mycp = { NULL};
	// Sanity check
	if (name == NULL)
		return mycp;
	if (strlen(name) == 0)
		return mycp;
	CIMParamValue * pcp = new CIMParamValue(String(name),
		OpenWBEM::CIMValue(Int32(value)));
	mycp.ptr = pcp;
	_NPIGarbageCan(npiHandle, (void *) mycp.ptr, CIM_PARAMVALUE);
	return mycp;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMParameter
CIMParameterNewRef(NPIHandle* npiHandle, const char* name, ::CIMObjectPath value)
{
	(void)npiHandle;
	::CIMParameter mycp = { NULL};
	// Sanity check
	if (name == NULL)
		return mycp;
	if (strlen(name) == 0)
		return mycp;
	OpenWBEM::CIMValue val(*((OpenWBEM::CIMObjectPath*)value.ptr));
	mycp.ptr = new CIMParamValue(String(name), val);
	_NPIGarbageCan(npiHandle, (void *) mycp.ptr, CIM_PARAMVALUE);
	return mycp;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMParameterGetString(NPIHandle* npiHandle, ::CIMParameter cp)
{
	(void)npiHandle;
	CIMParamValue* pcpv = static_cast<CIMParamValue *> (cp.ptr);
	String value = pcpv->getValue().toString();
	return value.allocateCString();
}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMParameterGetIntegerValue(NPIHandle* npiHandle, ::CIMParameter cp)
{
	(void)npiHandle;
	CIMParamValue* pcpv = (CIMParamValue*)cp.ptr;
	int value;
	pcpv->getValue().get(value);
	return value;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMParameterGetRefValue(NPIHandle* npiHandle, ::CIMParameter cp)
{
	(void)npiHandle;
	CIMParamValue* pcpv = (CIMParamValue*)cp.ptr;
	OpenWBEM::CIMObjectPath op(CIMNULL);
	OpenWBEM::CIMValue val = pcpv->getValue();
	val.get(op);
	OpenWBEM::CIMObjectPath * pop = new OpenWBEM::CIMObjectPath(op);
	::CIMObjectPath cop = { (void*) pop};
	_NPIGarbageCan(npiHandle, (void *) pop, CIM_OBJECTPATH);
	return cop;
}
// Instance functions
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMInstance
CIMClassNewInstance(NPIHandle* npiHandle, ::CIMClass cc)
{
	(void)npiHandle;
	OpenWBEM::CIMClass * owcc = static_cast<OpenWBEM::CIMClass *>(cc.ptr);
	OpenWBEM::CIMInstance * owci = new OpenWBEM::CIMInstance(owcc->newInstance());
	::CIMInstance ci = {static_cast<void *>(owci)};
	_NPIGarbageCan(npiHandle, (void *) owci, CIM_INSTANCE);
	return ci;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetStringProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const char* value )
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	String Key(name);
	String Val;
	if (value)
	{
		if (strlen(value)>0)
		{
			Val = String(value);
		}
		else
		{
			Val = String("-empty-");
		}
	}
	else
	{
		Val = String("-empty-");
	}
	OpenWBEM::CIMValue Value(Val);
	owci->setProperty(Key,Value);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetIntegerProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const int value)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	owci->setProperty(String(name),OpenWBEM::CIMValue(value));
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetLongProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const long long value)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	String Key(name);
	OpenWBEM::CIMValue Value(static_cast<UInt64>(value));
	owci->setProperty(Key,Value);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetBooleanProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const unsigned char value)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	String Key(name);
	OpenWBEM::CIMValue Value(Bool((int)value));
	owci->setProperty(Key,Value);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetRefProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, ::CIMObjectPath value)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return;
	if (strlen(name) == 0) return;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	OpenWBEM::CIMObjectPath * owcop = static_cast<OpenWBEM::CIMObjectPath *> (value.ptr);
	String Key(name);
	OpenWBEM::CIMValue Value(*owcop);
	owci->setProperty(Key,Value);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMInstanceGetStringValue(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return NULL;
	if (strlen(name) == 0) return NULL;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	String Key(name);
	CIMProperty prop = owci->getProperty(Key);
	OpenWBEM::CIMValue cv = prop.getValue();
	if (cv.getType() != CIMDataType::STRING)	return NULL;
	cv.get(Key);
	return Key.allocateCString();
}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMInstanceGetIntegerValue(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name)
{
	(void)npiHandle;
	// Sanity check
	if (name == NULL) return 0;
	if (strlen(name) == 0) return 0;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	String Key(name);
	CIMProperty prop = owci->getProperty(Key);
	OpenWBEM::CIMValue cv = prop.getValue();
	switch (cv.getType())
	{
		case CIMDataType::UINT8: {UInt8 i; cv.get(i); return i; break;}
		case CIMDataType::SINT8: {Int8 i; cv.get(i); return i; break;}
		case CIMDataType::UINT16: {UInt16 i; cv.get(i); return i; break;}
		case CIMDataType::SINT16: {Int16 i; cv.get(i); return i; break;}
		case CIMDataType::UINT32: {UInt32 i; cv.get(i); return i; break;}
		case CIMDataType::SINT32: {Int32 i; cv.get(i); return i; break;}
		case CIMDataType::UINT64: {UInt64 i; cv.get(i); return i; break;}
		case CIMDataType::SINT64: {Int64 i; cv.get(i); return i; break;}
		case CIMDataType::BOOLEAN: {Bool i; cv.get(i); return (i?-1:0); break;}
		default: return 0;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMInstanceGetRefValue(NPIHandle* npiHandle, ::CIMInstance ci, const char* name)
{
	(void)npiHandle;
	::CIMObjectPath cop = {NULL};
	// Sanity check
	if (name == NULL) return cop;
	if (strlen(name) == 0) return cop;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	String Key(name);
	CIMProperty prop = owci->getProperty(Key);
	OpenWBEM::CIMValue cv = prop.getValue();
	if (cv.getType() != CIMDataType::REFERENCE) return cop;
	OpenWBEM::CIMObjectPath owcop(CIMNULL);
	cv.get(owcop);
	cop.ptr = static_cast<void *>(&owcop);
	return cop;
}
// Object Path functions
//////////////////////////////////////////////////////////////////////////////
// empty keyBindings here
extern "C" ::CIMObjectPath
CIMObjectPathNew(NPIHandle* npiHandle, const char* classname)
{
	(void)npiHandle;
	String className(classname);
	OpenWBEM::CIMObjectPath * ref = new OpenWBEM::CIMObjectPath(className);
	::CIMObjectPath cop = { static_cast<void *> (ref)};
	_NPIGarbageCan(npiHandle, (void *) ref, CIM_OBJECTPATH);
	return cop;
}
//////////////////////////////////////////////////////////////////////////////
// Call CIMInstance.getClassName
// Loop over CIMInstance.getProperty with CIMInstance.getPropertyCount
// to the the key-value bindings.
extern "C" ::CIMObjectPath
CIMObjectPathFromCIMInstance(NPIHandle* npiHandle, ::CIMInstance ci)
{
	(void)npiHandle;
	OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
	String host;
	OpenWBEM::CIMObjectPath * ref = new OpenWBEM::CIMObjectPath("", *owci);
	::CIMObjectPath cop = { static_cast<void *>(ref)};
	_NPIGarbageCan(npiHandle, (void *) ref, CIM_OBJECTPATH);
	return cop;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetClassName(NPIHandle* npiHandle, ::CIMObjectPath cop)
{
	(void)npiHandle;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *> (cop.ptr);
	return ref->getObjectName().allocateCString();
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetNameSpace(NPIHandle* npiHandle, ::CIMObjectPath cop)
{
	(void)npiHandle;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	return ref->getNameSpace().allocateCString();
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathSetNameSpace(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* str)
{
	(void)npiHandle;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	ref->setNameSpace(String(str));
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void CIMObjectPathSetNameSpaceFromCIMObjectPath(
	NPIHandle* npiHandle, ::CIMObjectPath cop, ::CIMObjectPath src)
{
	(void)npiHandle;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	OpenWBEM::CIMObjectPath * rsrc = static_cast<OpenWBEM::CIMObjectPath *>(src.ptr);
	ref->setNameSpace(rsrc->getNameSpace());
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetStringKeyValue(NPIHandle* npiHandle,
	::CIMObjectPath cop, const char* key)
{
	(void)npiHandle;
	// Sanity check
	if (key == NULL) return NULL;
	if (strlen(key) == 0) return NULL;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	CIMPropertyArray props = ref->getKeys();
	String Key(key);
	for (int i = props.size()-1; i >= 0; i--)
	{
		CIMProperty cp = props[i];
		if (cp.getName().equalsIgnoreCase(Key))
		{
			OpenWBEM::CIMValue cv = cp.getValue();
			if (!cv) return NULL;
			if (cv.getType() != CIMDataType::STRING)
				return NULL;
			cv.get(Key);
			return Key.allocateCString();
		}
	}
	return NULL;
}
/* ====================================================================== */
static void _CIMObjectPathAddKey(OpenWBEM::CIMObjectPath * ref,
           const String& Key, const OpenWBEM::CIMValue & Value)
{
   if (ref->getKey(Key))
   {
      bool b = false;
      CIMPropertyArray cprops = ref->getKeys();
      for(Int32 i=cprops.size()-1; i >= 0; i--)
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
CIMObjectPathAddStringKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key, const char* value)
{
	(void)npiHandle;
	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	String Key(key);
	String Val(value);
	OpenWBEM::CIMValue Value(Val);
	_CIMObjectPathAddKey(ref, Key, Value);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMObjectPathGetIntegerKeyValue(NPIHandle* npiHandle,
	::CIMObjectPath cop, const char* key)
{
	(void)npiHandle;
	// Sanity check
	if (key == NULL) return -1;
	if (strlen(key) == 0) return -1;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	CIMPropertyArray props = ref->getKeys();
	String Key(key);
	//for (int i = 0, n = props.size(); i < n; i++)
	for (int i = props.size()-1; i >= 0; i--)
	{
		CIMProperty cp = props[i];
		if (cp.getName().equalsIgnoreCase(Key))
		{
			OpenWBEM::CIMValue cv = cp.getValue();
			if (!cv) return 0;
			switch (cv.getType())
			{
				case CIMDataType::UINT8:
					{UInt8 i; cv.get(i); return i; break;}
				case CIMDataType::SINT8:
					{Int8 i; cv.get(i); return i; break;}
				case CIMDataType::UINT16:
					{UInt16 i; cv.get(i); return i; break;}
				case CIMDataType::SINT16:
					{Int16 i; cv.get(i); return i; break;}
				case CIMDataType::UINT32:
					{UInt32 i; cv.get(i); return i; break;}
				case CIMDataType::SINT32:
					{Int32 i; cv.get(i); return i; break;}
				case CIMDataType::UINT64:
					{UInt64 i; cv.get(i); return i; break;}
				case CIMDataType::SINT64:
					{Int64 i; cv.get(i); return i; break;}
				default: return 0;
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddIntegerKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key, const int value)
{
	(void)npiHandle;
	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	String Key(key);
	OpenWBEM::CIMValue Value(value);
	_CIMObjectPathAddKey(ref, Key, Value);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMObjectPathGetRefKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key)
{
	(void)npiHandle;
	// Sanity check
	::CIMObjectPath cop2 = {NULL};
	if (key == NULL) return cop2;
	if (strlen(key) == 0) return cop2;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	CIMPropertyArray props = ref->getKeys();
	String Key(key);
	//for (int i = 0, n = props.size(); i < n; i++)
	for (int i = props.size()-1; i >= 0; i--)
	{
		CIMProperty cp = props[i];
		if (cp.getName().equalsIgnoreCase(Key))
		{
			OpenWBEM::CIMValue cv = cp.getValue();
			if (!cv) return cop2;
			if (cv.getType() != CIMDataType::REFERENCE) return cop2;
			OpenWBEM::CIMObjectPath * ref2 = new OpenWBEM::CIMObjectPath(CIMNULL);
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
CIMObjectPathAddRefKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key, ::CIMObjectPath cop2)
{
	(void)npiHandle;
	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	OpenWBEM::CIMObjectPath * ref2 = static_cast<OpenWBEM::CIMObjectPath *>(cop2.ptr);
	String Key(key);
	OpenWBEM::CIMValue Value(*ref2);
	_CIMObjectPathAddKey(ref, Key, Value);
}
// SelectExp functions
//////////////////////////////////////////////////////////////////////////////
extern "C" char * 
SelectExpGetSelectString(NPIHandle* npiHandle, ::SelectExp sxp)
{
	(void)npiHandle;
	WQLSelectStatement * wf =
		 static_cast<WQLSelectStatement *>(sxp.ptr);
	char * query = wf->toString().allocateCString();
	return query;
}
// CIMOM functions
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMClass
CIMOMGetClass(NPIHandle* npiHandle, ::CIMObjectPath cop, int localOnly)
{
	(void)localOnly;
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	String nameSpace = ref->getNameSpace();
	String className = ref->getObjectName();
	OpenWBEM::CIMClass cc = NPI_getmyClass(npiHandle, nameSpace, className);
	OpenWBEM::CIMClass * my_cc = new OpenWBEM::CIMClass(cc);
	::CIMClass localcc = { static_cast<void *> (my_cc)};
	_NPIGarbageCan(npiHandle, (void *) my_cc, CIM_CLASS);
	return localcc;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::Vector
CIMOMEnumInstanceNames(NPIHandle* npiHandle, ::CIMObjectPath cop, int i)
{
	(void)i;
	OpenWBEM::CIMObjectPath * ref = (OpenWBEM::CIMObjectPath *) cop.ptr;
	String nameSpace = ref->getNameSpace();
	String className = ref->getObjectName();
	CIMObjectPathEnumeration instNames =
		NPI_enumeratemyInstanceNames(npiHandle,nameSpace,className);
	// Full Copy
	Vector v = VectorNew(npiHandle);
	while (instNames.hasMoreElements())
	{
		OpenWBEM::CIMObjectPath * cowp = new
			OpenWBEM::CIMObjectPath(instNames.nextElement());
		_NPIGarbageCan(npiHandle, (void *) cowp, CIM_OBJECTPATH);
		_VectorAddTo(npiHandle, v, (void *) cowp);
	}
	return v;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::Vector
CIMOMEnumInstances(NPIHandle* npiHandle, ::CIMObjectPath cop, int i, int j)
{
	(void)i;
	(void)j;
	OpenWBEM::CIMObjectPath * ref = (OpenWBEM::CIMObjectPath *) cop.ptr;
	String nameSpace = ref->getNameSpace();
	String className = ref->getObjectName();
	CIMInstanceEnumeration insts =
		NPI_enumeratemyInstances(npiHandle,nameSpace,className);
	// Full Copy
	Vector v = VectorNew(npiHandle);
	while (insts.hasMoreElements())
	{
		OpenWBEM::CIMInstance * ci = new OpenWBEM::CIMInstance(insts.nextElement());
		_NPIGarbageCan(npiHandle, (void *) ci, CIM_INSTANCE);
		_VectorAddTo(npiHandle, v, (void *) ci);
	}
	return v;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMInstance
CIMOMGetInstance(NPIHandle* npiHandle, ::CIMObjectPath cop, int i)
{
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	OpenWBEM::CIMInstance ci = NPI_getmyInstance(npiHandle, *ref, i);
	OpenWBEM::CIMInstance * my_ci = new OpenWBEM::CIMInstance(ci);
	_NPIGarbageCan(npiHandle, (void *) my_ci, CIM_INSTANCE);
	::CIMInstance localci = { static_cast<void *> (my_ci)};
	return localci;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMDeliverProcessEvent(NPIHandle* npiHandle, char * ns,
		::CIMInstance indication)
{
	(void)ns;
	ProviderEnvironmentIFCRef * provenv =
		static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
	OpenWBEM::CIMInstance * ow_indication =
           static_cast<OpenWBEM::CIMInstance *>(indication.ptr);
	try
	{
		(*provenv)->getCIMOMHandle()->exportIndication(
			* ow_indication, String("root/cimv2"));
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
			::CIMInstance indication, 
                          ::CIMInstance source, ::CIMInstance previous)
{
	(void)ns;
	ProviderEnvironmentIFCRef * provenv =
		static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
	OpenWBEM::CIMInstance * ow_indication =
           static_cast<OpenWBEM::CIMInstance *>(indication.ptr);
	OpenWBEM::CIMInstance * ow_source = static_cast<OpenWBEM::CIMInstance *>(source.ptr);
	OpenWBEM::CIMInstance * ow_previous =
           static_cast<OpenWBEM::CIMInstance *>(previous.ptr);
        OpenWBEM::CIMValue src_val(* ow_source);
        OpenWBEM::CIMValue prev_val(* ow_previous);
	ow_indication->setProperty(String("SourceInstance"), src_val);
           
	ow_indication->setProperty(String("PreviousInstance"), prev_val);
	try
	{
		(*provenv)->getCIMOMHandle()->exportIndication(
			* ow_indication, String("root/cimv2"));
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
    ProviderEnvironmentIFCRef * provenv =
        static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
    nh->thisObject = new ProviderEnvironmentIFCRef(*provenv);
    // copy NPIContext
    nh->context = new ::NPIContext;
    ((NPIContext *)(nh->context))->scriptName =
           ((NPIContext *)(npiHandle->context))->scriptName;
    // CHECK: do I have to allocate a new perl context here ?
    ((NPIContext *)(nh->context))->my_perl =
           ((NPIContext *)(npiHandle->context))->my_perl;
    // need to worry about errorOccurred and providerError???   
    return nh;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMCancelAttach(NPIHandle* npiHandle)
{
    delete static_cast<ProviderEnvironmentIFCRef *>(npiHandle->thisObject);
    if (npiHandle->providerError != NULL)
        free((void *)(npiHandle->providerError));
    // TODO delete NPIContext
    free(npiHandle);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMAttachThread(NPIHandle* npiHandle)
{
	if (npiHandle == NULL) return;
	npiHandle->errorOccurred = 0;
    ((NPIContext *)(npiHandle->context))->garbage = Array<void *>();
    ((NPIContext *)(npiHandle->context))->garbageType = Array<NPIGarbageType>();
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMDetachThread(NPIHandle* npiHandle)
{
	if (npiHandle == NULL) return;
	npiHandle->errorOccurred = 0;
	// Free the copied npiHandle and NPIContext
	NPIHandleFreer nf(* npiHandle);
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
extern "C" {
// Dummy functions for CMPI Compatibility
void* NPIOnCMPI_Create_InstanceMI(void *, void *, const char *)  {
	return 0;
}
void* NPIOnCMPI_Create_AssociationMI(void *, void *, const char *)
{
	return 0;
}
void* NPIOnCMPI_Create_MethodMI(void *, void *, const char *)
{
	return 0;
}
void* NPIOnCMPI_Create_IndicationMI(void *, void *, const char *)
{
	return 0;
}
}


