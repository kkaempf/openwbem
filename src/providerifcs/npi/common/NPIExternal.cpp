#include <cstdio>
#include "OW_config.h"
#include "NPIExternal.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_Format.hpp"
#include "OW_FTABLERef.hpp"
#include "OW_NPIProviderIFCUtils.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_WQLSelectStatement.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.npi.ifc");
}

using namespace WBEMFlags;
// Garbage Collection helper functions
void _NPIGarbageCan(NPIHandle * nh, void * object, NPIGarbageType type)
{
   ((NPIContext *)(nh->context))->garbage.append(object);
   ((NPIContext *)(nh->context))->garbageType.append(type);
}
void _NPIGarbageRetrieve(NPIHandle * nh, void * object)
{
   for (int i = ((NPIContext *)(nh->context))->garbage.size()-1;i >=0;i--)
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
		raiseError(npiHandle,"Class or Namespace does not exist");
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
		raiseError(npiHandle,"Class or Namespace does not exist");
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
		raiseError(npiHandle,"Class or Namespace does not exist");
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
		raiseError(npiHandle,"Class or Namespace does not exist");
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
	Vector v;
	v.ptr = (void*) new charVect;
	_NPIGarbageCan(npiHandle, v.ptr, VECTOR);
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
	OpenWBEM::CIMValue* pcv = new OpenWBEM::CIMValue(String(val));
	::CIMValue cv = { (void*) pcv};
	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMValue
CIMValueNewInteger(NPIHandle* npiHandle, int val)
{
	OpenWBEM::CIMValue* pcv = new OpenWBEM::CIMValue(val);
	::CIMValue cv = { (void*) pcv};
	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMValue
CIMValueNewRef(NPIHandle* npiHandle, ::CIMObjectPath cop)
{
	OpenWBEM::CIMValue* pcv = new OpenWBEM::CIMValue(*((OpenWBEM::CIMObjectPath*)cop.ptr));
	::CIMValue cv = { (void*) pcv};
	 _NPIGarbageCan(npiHandle, (void *) pcv, CIM_VALUE);
	return cv;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMValueGetString(NPIHandle* npiHandle, ::CIMValue cv)
{
	try {
		OpenWBEM::CIMValue* pcv = (OpenWBEM::CIMValue*)cv.ptr;
		String mystring = pcv->toString();
		return mystring.allocateCString();
	} catch (...) {
		raiseError(npiHandle, "Error getting string value");
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMValueGetInteger(NPIHandle* npiHandle, ::CIMValue cv)
{
	int retval;
	try {
		OpenWBEM::CIMValue* pcv = (OpenWBEM::CIMValue*)cv.ptr;
		pcv->get(retval);
	} catch (...) {
		raiseError(npiHandle, "Error getting int value");
	}
	return retval;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMValueGetRef(NPIHandle* npiHandle, ::CIMValue cv)
{
	::CIMObjectPath cop = { NULL};
	try {
		OpenWBEM::CIMValue* pcv = (OpenWBEM::CIMValue*) cv.ptr;
		OpenWBEM::CIMObjectPath cref(CIMNULL);
		pcv->get(cref);
		OpenWBEM::CIMObjectPath* ncop = new OpenWBEM::CIMObjectPath(cref);
		::CIMObjectPath cop = {(void*) ncop};
		_NPIGarbageCan(npiHandle, (void *) ncop, CIM_OBJECTPATH);
	} catch (...) {
		raiseError(npiHandle, "Error getting ref value");
	}
	return cop;
}
// CIMParameter functions
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMType
CIMParameterGetType(NPIHandle* npiHandle, ::CIMParameter cp)
{
	int dt;
	try {
		OpenWBEM::CIMParamValue* pcp = (OpenWBEM::CIMParamValue*)cp.ptr;
		dt = pcp->getValue().getType();
	} catch (...) {
		raiseError(npiHandle, "Error getting parameter type");
	}

	//switch (dt.getType())
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
	try {
		CIMParamValue* pcp = (CIMParamValue*)cp.ptr;
		return pcp->getName().allocateCString();
	} catch (...) {
		raiseError(npiHandle, "Error getting parameter name");

		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMParameter
CIMParameterNewString(NPIHandle* npiHandle, const char* name, const char* value)
{
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
	try {
		CIMParamValue* pcpv = static_cast<CIMParamValue *> (cp.ptr);
		String value = pcpv->getValue().toString();
		return value.allocateCString();
	} catch (...) {
		raiseError(npiHandle, "Retrieving string parameter failed. Possible attemt to retrieve non-string parameter");

		return NULL;
	}	
}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMParameterGetIntegerValue(NPIHandle* npiHandle, ::CIMParameter cp)
{
	int value;
	try {
		CIMParamValue* pcpv = (CIMParamValue*)cp.ptr;
		pcpv->getValue().get(value);
	} catch (...) {
		raiseError(npiHandle, "Retrieving integer parameter failed. Possible attemt to retrieve non-integer parameter");

	}	
	return value;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMParameterGetRefValue(NPIHandle* npiHandle, ::CIMParameter cp)
{
	::CIMObjectPath cop = { NULL};
	try {
		CIMParamValue* pcpv = (CIMParamValue*)cp.ptr;
		OpenWBEM::CIMObjectPath op(CIMNULL);
		OpenWBEM::CIMValue val = pcpv->getValue();
		val.get(op);
		OpenWBEM::CIMObjectPath * pop = new OpenWBEM::CIMObjectPath(op);
		::CIMObjectPath cop = { (void*) pop};
		_NPIGarbageCan(npiHandle, (void *) pop, CIM_OBJECTPATH);
	} catch (...) {
		raiseError(npiHandle, "Retrieving ref parameter failed. Possible attemt to retrieve non-ref parameter");

	}	
	return cop;
}
// Instance functions
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMInstance
CIMClassNewInstance(NPIHandle* npiHandle, ::CIMClass cc)
{
	::CIMInstance ci ={ NULL};
	try {
		OpenWBEM::CIMClass * owcc = static_cast<OpenWBEM::CIMClass *>(cc.ptr);
		OpenWBEM::CIMInstance * owci = new OpenWBEM::CIMInstance(owcc->newInstance());
		ci.ptr = static_cast<void *>(owci);
		_NPIGarbageCan(npiHandle, (void *) owci, CIM_INSTANCE);
	} catch (...) {
		raiseError(npiHandle,"Error creating instance");

	}
	return ci;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetStringProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const char* value )
{
	// Sanity check
	if (name == NULL) return;

	try {
		if (strlen(name) == 0) return;		// try this too, might be non-string
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
	} catch (...) {
		raiseError(npiHandle,"Error setting string property");

		return;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetIntegerProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const int value)
{
	// Sanity check
	if (name == NULL) return;
	
	try {
		if (strlen(name) == 0) return;
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		owci->setProperty(String(name),OpenWBEM::CIMValue(value));
	} catch (...) {
		raiseError(npiHandle,"Error setting integer property");

		return;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetLongProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const long long value)
{
	// Sanity check
	if (name == NULL) return;
	try {
		if (strlen(name) == 0) return;
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		OpenWBEM::CIMValue Value(static_cast<UInt64>(value));
		owci->setProperty(Key,Value);
	} catch (...) {
		raiseError(npiHandle,"Error creating instance");
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetBooleanProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const unsigned char value)
{
	// Sanity check
	if (name == NULL) return;
	try {
		if (strlen(name) == 0) return;
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		OpenWBEM::CIMValue Value(Bool((int)value));
		owci->setProperty(Key,Value);
} catch (...) {
		raiseError(npiHandle,"Error setting boolean property");
		return;
	}		
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMInstanceSetRefProperty(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, ::CIMObjectPath value)
{
	// Sanity check
	if (name == NULL) return;
	try {
		if (strlen(name) == 0) return;
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		OpenWBEM::CIMObjectPath * owcop = static_cast<OpenWBEM::CIMObjectPath *> (value.ptr);
		String Key(name);
		OpenWBEM::CIMValue Value(*owcop);
		owci->setProperty(Key,Value);
	} catch (...) {
		raiseError(npiHandle,"Error setting ref property");

		return;
	}
}
//////////////////////////////////////////////////////////////////////////////
//
// NPI needs arrays, too
// as NPI doesn't know anything about OpenWBEM::* classes, we cannot expect
// or handle a OpenWBEM::StringArray in our parameter list - we would break
// NPI if we did.
// But we can dig a char*, cast it to a OpenWBEM::String and add it to
// our property.
//
// This means that users will have to add every String of a StringArray
// separately by calling something like
//
// char * name = 'Foo';
// char * value = 'Bar';
// CIMInstanceAddStringArrayProperty(npiHandle, ci, name, value);
//
// but this is the only way to fill our array unless we define a NPI
// StringArray type
//
extern "C" void
CIMInstanceAddStringArrayPropertyValue(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const char* value)
{
	// Sanity check
	if (name == NULL) return;
	try {
		if (strlen(name) == 0) return;
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		String Value(value);
		// get current value
		OpenWBEM::CIMProperty cp = owci->getProperty(Key);
		OpenWBEM::CIMValue cv = cp.getValue();
		OpenWBEM::StringArray sa;
		if (!cv) {
			// printf ("Got empty CIMValue\n");
		} else {
			// printf("Got StringArray\n");
			sa = cv.toStringArray();
		}
		sa.append(Value);
		OpenWBEM::CIMValue newcv(sa);
		owci->setProperty(Key,newcv);
	} catch (...) {
		raiseError(npiHandle,"Error adding string array property");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////
// ...and NPI also needs to read that stuff
//
extern "C" char*
CIMInstanceGetStringArrayPropertyValue(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name, const int pos)
{
	char * result = NULL;
	// Sanity check
	if (name == NULL) return result;
	try {
		if (strlen(name) == 0) return result;
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		// get current value
		OpenWBEM::CIMProperty cp = owci->getProperty(Key);
		OpenWBEM::CIMValue cv = cp.getValue();
		OpenWBEM::StringArray sa;
		sa = cv.toStringArray();
		return sa[pos].toString().allocateCString();
	} catch (...) {
		raiseError(npiHandle, "Error retrieving array property element. Possible attempt to retrive element of non-array property");
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
// ...and we need to know how long that is.
extern "C" int
CIMInstanceGetStringArrayPropertySize(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name)
{
	// Sanity check
	if (name == NULL) return -1;
	if (strlen(name) == 0) return -1;
	try {
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		// get current value
		OpenWBEM::CIMProperty cp = owci->getProperty(Key);
		OpenWBEM::CIMValue cv = cp.getValue();
		OpenWBEM::StringArray sa;
		if (!cv) {
			return -1;
		}
		sa = cv.toStringArray();
		return sa.size();
	} catch (...) {
		raiseError(npiHandle, "Error retrieving array property size. Possible attempt to retrive size of non-array property");
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMInstanceGetStringValue(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name)
{
	// Sanity check
	if (name == NULL) return NULL;
	try {	
		if (strlen(name) == 0) return NULL;
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		CIMProperty prop = owci->getProperty(Key);
		if (!prop) return NULL;
		OpenWBEM::CIMValue cv = prop.getValue();
		if (!cv) return NULL;
		if (cv.getType() != CIMDataType::STRING)	return NULL;
		cv.get(Key);
		return Key.allocateCString();
	} catch (...) {
		raiseError(npiHandle, "Error getting string property");
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMInstanceGetIntegerValue(NPIHandle* npiHandle, ::CIMInstance ci,
	const char* name)
{
	// Sanity check
	if (name == NULL) return 0;
	if (strlen(name) == 0) return 0;
	try {
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		CIMProperty prop = owci->getProperty(Key);
		if (!prop) return 0;
		OpenWBEM::CIMValue cv = prop.getValue();
		if (!cv) return 0;
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
	} catch (...) {
		raiseError(npiHandle, "Error getting integer property");	
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMInstanceGetRefValue(NPIHandle* npiHandle, ::CIMInstance ci, const char* name)
{
	::CIMObjectPath cop = {NULL};
	// Sanity check
	if (name == NULL) return cop;
	if (strlen(name) == 0) return cop;
	try {	
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String Key(name);
		CIMProperty prop = owci->getProperty(Key);
		OpenWBEM::CIMValue cv = prop.getValue();
		if (cv.getType() != CIMDataType::REFERENCE) return cop;
		OpenWBEM::CIMObjectPath owcop(CIMNULL);
		cv.get(owcop);
		cop.ptr = static_cast<void *>(&owcop);
	} catch (...) {
		raiseError(npiHandle, "Error getting ref property");
	}
	return cop;
}
// Object Path functions
//////////////////////////////////////////////////////////////////////////////
// empty keyBindings here
extern "C" ::CIMObjectPath
CIMObjectPathNew(NPIHandle* npiHandle, const char* classname)
{
	::CIMObjectPath cop = {NULL};	
	try {
		String className(classname);
		OpenWBEM::CIMObjectPath * ref = new OpenWBEM::CIMObjectPath(className);
		cop.ptr = static_cast<void *>(ref);
		_NPIGarbageCan(npiHandle, (void *) ref, CIM_OBJECTPATH);
	} catch (...) {
		raiseError(npiHandle, "Error creating object path");
	}
	return cop;
}
//////////////////////////////////////////////////////////////////////////////
// Call CIMInstance.getClassName
// Loop over CIMInstance.getProperty with CIMInstance.getPropertyCount
// to the the key-value bindings.
extern "C" ::CIMObjectPath
CIMObjectPathFromCIMInstance(NPIHandle* npiHandle, ::CIMInstance ci)
{
	::CIMObjectPath cop = {NULL};
	try {
		OpenWBEM::CIMInstance * owci = static_cast<OpenWBEM::CIMInstance *>(ci.ptr);
		String host;
		OpenWBEM::CIMObjectPath * ref = new OpenWBEM::CIMObjectPath("", *owci);
		cop.ptr = static_cast<void *>(ref);
		_NPIGarbageCan(npiHandle, (void *) ref, CIM_OBJECTPATH);
	} catch (...) {
		raiseError(npiHandle, "Error creating object path from instance");
	}
	return cop;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetClassName(NPIHandle* npiHandle, ::CIMObjectPath cop)
{
	try {
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *> (cop.ptr);
		return ref->getClassName().allocateCString();
	} catch (...) {
		raiseError(npiHandle, "Error getting class name");
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetNameSpace(NPIHandle* npiHandle, ::CIMObjectPath cop)
{
	try {
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
		return ref->getNameSpace().allocateCString();
	} catch (...) {
		raiseError(npiHandle, "Error getting namespace");
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathSetNameSpace(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* str)
{
	try {	
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
		ref->setNameSpace(String(str));
	} catch (...) {
		raiseError(npiHandle, "Error getting string property");
		return;
	}
	
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void CIMObjectPathSetNameSpaceFromCIMObjectPath(
	NPIHandle* npiHandle, ::CIMObjectPath cop, ::CIMObjectPath src)
{
	try {
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
		OpenWBEM::CIMObjectPath * rsrc = static_cast<OpenWBEM::CIMObjectPath *>(src.ptr);
		ref->setNameSpace(rsrc->getNameSpace());
	} catch (...) {
		raiseError(npiHandle, "Error seting namespace from COP");
		return;
	}

}
//////////////////////////////////////////////////////////////////////////////
extern "C" char*
CIMObjectPathGetStringKeyValue(NPIHandle* npiHandle,
	::CIMObjectPath cop, const char* key)
{
	// Sanity check
	if (key == NULL) return NULL;
	if (strlen(key) == 0) return NULL;
	try {
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
	} catch (...) {
		raiseError(npiHandle, "Error getting string property");
		return NULL;
	}

}
/* ====================================================================== */
static void _CIMObjectPathAddKey(OpenWBEM::CIMObjectPath * ref,
		   const String& Key, const OpenWBEM::CIMValue & Value)
{
   if (ref->getKey(Key))
   {
	  bool b = false;
	  CIMPropertyArray cprops = ref->getKeys();
	  for (Int32 i=cprops.size()-1; i >= 0; i--)
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
   ref->setKeyValue(Key,Value);
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddStringKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key, const char* value)
{
	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;
	try {
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
		String Key(key);
		String Val(value);
		OpenWBEM::CIMValue Value(Val);
		_CIMObjectPathAddKey(ref, Key, Value);
	} catch (...) {
		raiseError(npiHandle, "Error adding string key");
		return;
	}

}
//////////////////////////////////////////////////////////////////////////////
extern "C" int
CIMObjectPathGetIntegerKeyValue(NPIHandle* npiHandle,
	::CIMObjectPath cop, const char* key)
{
	// Sanity check
	if (key == NULL) return -1;
	if (strlen(key) == 0) return -1;
	try {
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
	} catch (...) {
		raiseError(npiHandle, "Error getting integer key");
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddIntegerKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key, const int value)
{
	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;
	try {
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
		String Key(key);
		OpenWBEM::CIMValue Value(value);
		_CIMObjectPathAddKey(ref, Key, Value);
	} catch (...) {
		raiseError(npiHandle, "Error adding int key");
		return;
	}

}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMObjectPath
CIMObjectPathGetRefKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key)
{
	// Sanity check
	::CIMObjectPath cop2 = {NULL};
	if (key == NULL) return cop2;
	if (strlen(key) == 0) return cop2;
	try {
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
	} catch (...) {
		raiseError(npiHandle, "Error getting ref key");
	}
	return cop2;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMObjectPathAddRefKeyValue(NPIHandle* npiHandle, ::CIMObjectPath cop,
	const char* key, ::CIMObjectPath cop2)
{
	// Sanity check
	if (key == NULL) return;
	if (strlen(key) == 0) return;
	try {
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
		OpenWBEM::CIMObjectPath * ref2 = static_cast<OpenWBEM::CIMObjectPath *>(cop2.ptr);
		String Key(key);
		OpenWBEM::CIMValue Value(*ref2);
		_CIMObjectPathAddKey(ref, Key, Value);
	} catch (...) {
		raiseError(npiHandle, "Error adding ref key");
		return;
	}
}
// SelectExp functions
//////////////////////////////////////////////////////////////////////////////
extern "C" char *
SelectExpGetSelectString(NPIHandle* npiHandle, ::SelectExp sxp)
{
	try {
		WQLSelectStatement * wf =
			 static_cast<WQLSelectStatement *>(sxp.ptr);
		char * query = wf->toString().allocateCString();
		return query;
	} catch (...) {
		raiseError(npiHandle, "Error getting select string from select exp");
		return NULL;
	}
}
// CIMOM functions
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMClass
CIMOMGetClass(NPIHandle* npiHandle, ::CIMObjectPath cop, int localOnly)
{
	OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
	String nameSpace = ref->getNameSpace();
	String className = ref->getClassName();
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
	Vector v = VectorNew(npiHandle);
	try {
		OpenWBEM::CIMObjectPath * ref = (OpenWBEM::CIMObjectPath *) cop.ptr;
		String nameSpace = ref->getNameSpace();
		String className = ref->getClassName();
		CIMObjectPathEnumeration instNames =
			NPI_enumeratemyInstanceNames(npiHandle,nameSpace,className);
		// Full Copy
		while (instNames.hasMoreElements())
		{
			OpenWBEM::CIMObjectPath * cowp = new
				OpenWBEM::CIMObjectPath(instNames.nextElement());
			_NPIGarbageCan(npiHandle, (void *) cowp, CIM_OBJECTPATH);
			_VectorAddTo(npiHandle, v, (void *) cowp);
		}
	} catch (...) {
		raiseError(npiHandle, "Error enumerating instance names");
	}
	return v;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::Vector
CIMOMEnumInstances(NPIHandle* npiHandle, ::CIMObjectPath cop, int i, int j)
{
	Vector v = VectorNew(npiHandle);
	try {
		OpenWBEM::CIMObjectPath * ref = (OpenWBEM::CIMObjectPath *) cop.ptr;
		String nameSpace = ref->getNameSpace();
		String className = ref->getClassName();
		CIMInstanceEnumeration insts =
			NPI_enumeratemyInstances(npiHandle,nameSpace,className);
		// Full Copy
		while (insts.hasMoreElements())
		{
			OpenWBEM::CIMInstance * ci = new OpenWBEM::CIMInstance(insts.nextElement());
			_NPIGarbageCan(npiHandle, (void *) ci, CIM_INSTANCE);
			_VectorAddTo(npiHandle, v, (void *) ci);
		}
	} catch (...) {
		raiseError(npiHandle, "Error enumerating instances");
	}
	return v;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" ::CIMInstance
CIMOMGetInstance(NPIHandle* npiHandle, ::CIMObjectPath cop, int i)
{
	::CIMInstance localci = { NULL};
	try {
		OpenWBEM::CIMObjectPath * ref = static_cast<OpenWBEM::CIMObjectPath *>(cop.ptr);
		OpenWBEM::CIMInstance ci = NPI_getmyInstance(npiHandle, *ref, i);
		OpenWBEM::CIMInstance * my_ci = new OpenWBEM::CIMInstance(ci);
		_NPIGarbageCan(npiHandle, (void *) my_ci, CIM_INSTANCE);
		localci.ptr = static_cast<void *> (my_ci);
	} catch (...) {
		raiseError(npiHandle, "Error getting instance");
	}
	return localci;
}
//////////////////////////////////////////////////////////////////////////////
extern "C" void
CIMOMDeliverProcessEvent(NPIHandle* npiHandle, char * ns,
		::CIMInstance indication)
{
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
	(*provenv)->getLogger(COMPONENT_NAME)->logDebug(Format("NPIExternal: Deliver %1", npiHandle->errorOccurred));
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
	//((NPIContext *)(npiHandle->context))->garbage = Array<void *>();
	//((NPIContext *)(npiHandle->context))->garbageType = Array<NPIGarbageType>();
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


