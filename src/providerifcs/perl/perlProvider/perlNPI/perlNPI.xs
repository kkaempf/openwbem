#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#undef EXTERN_C
#include "perlargs.h"
#include "provider.h"
#include "npi.h"
#include "npi_import.h"

typedef NPIHandle * NPIHandlePtr;

MODULE = perlNPI		PACKAGE = perlNPI		

char *
VectorNew(npiHandle)
	char * npiHandle
	PREINIT:
	Vector v;
	NPIHandle * nh;
	CODE:
	nh = argsfromperl(npiHandle);
	v = VectorNew(nh);
	RETVAL = argsforperl(v.ptr);
	OUTPUT:
	RETVAL

void
_VectorAddTo (npiHandle,v,o)
	char * npiHandle
	char * v
	char * o
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	Vector vv;
	CODE:
	vv.ptr = argsfromperl(v);
	_VectorAddTo(nh,vv,argsfromperl(o));

char *  _VectorGet( npiHandle, v, i)
	char * npiHandle
	char * v
	int i
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	Vector vv = {argsfromperl(v)};
	CODE:
	RETVAL = argsforperl(_VectorGet(nh,vv,i));
	OUTPUT:
	RETVAL

int	VectorSize(npiHandle, v)
	char * npiHandle
	char * v
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	Vector vv = {argsfromperl(v)};
	CODE:
	RETVAL = VectorSize(nh,vv);
	OUTPUT:
	RETVAL


int	CIMValueGetType(npiHandle, cimValue)
	char * npiHandle
	char * cimValue
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMValue cv = {argsfromperl(cimValue)};
	CIMType ctyp;
	CODE:
	ctyp = CIMValueGetType(nh, cv);
	RETVAL = ctyp;
	OUTPUT:
	RETVAL

char *	CIMValueNewString(npiHandle, str)
	char * npiHandle
	char * str
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMValue cv;
	CODE:
	cv = CIMValueNewString(nh, str);
	RETVAL = argsforperl(cv.ptr);
	OUTPUT:
	RETVAL

char *	CIMValueNewInteger(npiHandle, i)
	char * npiHandle
	int i
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMValue cv;
	CODE:
	cv = CIMValueNewInteger(nh, i);
	RETVAL = argsforperl(cv.ptr);
	OUTPUT:
	RETVAL

char *	CIMValueNewRef(npiHandle, cref)
	char * npiHandle
	char * cref
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath cop = {argsfromperl(cref)};
	CIMValue cv;
	CODE:
	cv = CIMValueNewRef(nh, cop);
	RETVAL = argsforperl(cv.ptr);
	OUTPUT:
	RETVAL

int	CIMParameterGetType(npiHandle, cp)
	char * npiHandle
	void * cp
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMParameter ccp = {argsfromperl(cp)};
	CODE:
	RETVAL = CIMParameterGetType(nh, ccp);
	OUTPUT:
	RETVAL

char *	CIMParameterGetName(npiHandle, cp)
	char * npiHandle
	void * cp
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMParameter ccp = {argsfromperl(cp)};
	CODE:
	RETVAL = CIMParameterGetName(nh, ccp);
	OUTPUT:
	RETVAL
	
char *	CIMParameterNewString(npiHandle, name, value)
	char * npiHandle
	char * name
	char * value
	PREINIT:
	CIMParameter cp;
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	cp = CIMParameterNewString(nh, name, value);
	RETVAL = argsforperl(cp.ptr);
	OUTPUT:
	RETVAL

char*	CIMParameterNewInteger(npiHandle, name, value)
	char * npiHandle
	char * name
	int value
	PREINIT:
	CIMParameter cp;
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	cp = CIMParameterNewInteger(nh, name, value);
	RETVAL = argsforperl(cp.ptr);
	OUTPUT:
	RETVAL

char *	CIMParameterNewRef(npiHandle, name, value)
	char * npiHandle
	char * name
	char * value
	PREINIT:
	CIMParameter cp;
	CIMObjectPath cop = {argsfromperl(value)};
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	cp = CIMParameterNewRef(nh, name, cop);
	RETVAL = argsforperl(cp.ptr);
	OUTPUT:
	RETVAL

char *	CIMParameterGetString(npiHandle, cp)
	char * npiHandle
	void * cp
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMParameter ccp = {argsfromperl(cp)};
	CODE:
	RETVAL = CIMParameterGetString(nh, ccp);
	OUTPUT:
	RETVAL
	
int	CIMParameterGetInteger(npiHandle, cp)
	char * npiHandle
	void * cp
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMParameter ccp = {argsfromperl(cp)};
	CODE:
	RETVAL = CIMParameterGetInteger(nh, ccp);
	OUTPUT:
	RETVAL
	
char *	CIMParameterGetRefValue(npiHandle, cp)
	char * npiHandle
	void * cp
	PREINIT:
	CIMObjectPath cop;
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMParameter ccp = {argsfromperl(cp)};
	CODE:
	cop = CIMParameterGetRefValue(nh, ccp);
	RETVAL = argsforperl(cop.ptr);
	OUTPUT:
	RETVAL
	

char *	CIMClassNewInstance(npiHandle ,cc)
	char * npiHandle
	char * cc
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMClass ccc = {argsfromperl(cc)};
	CIMInstance ci;
	CODE:
	ci = CIMClassNewInstance(nh,ccc);
	RETVAL = argsforperl(ci.ptr);
	OUTPUT:
	RETVAL

void	CIMInstanceSetStringProperty(npiHandle, ci, key, value)
	char * npiHandle
	char * ci
	char * key
	char * value
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CODE:
	CIMInstanceSetStringProperty(nh,cci,key,value);

void	CIMInstanceSetIntegerProperty(npiHandle, ci, key, value)
	char * npiHandle
	char * ci
	char * key
	int value
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CODE:
	CIMInstanceSetIntegerProperty(nh,cci,key,value);

void	CIMInstanceSetBooleanProperty(npiHandle, ci, key, value)
	char * npiHandle
	char * ci
	char * key
	int value
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CODE:
	CIMInstanceSetBooleanProperty(nh,cci,key,value);

void	CIMInstanceSetRefProperty(npiHandle, ci, key, value)
	char * npiHandle
	char * ci
	char * key
	char * value
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CIMObjectPath cop = {argsfromperl(value)};
	CODE:
	CIMInstanceSetRefProperty(nh,cci,key,cop);

char *	CIMInstanceGetStringProperty(npiHandle, ci, key)
	char * npiHandle
	char * ci
	char * key
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CODE:
	RETVAL = CIMInstanceGetStringValue(nh,cci,key);
	OUTPUT:
	RETVAL

int	CIMInstanceGetIntegerProperty(npiHandle, ci, key)
	char * npiHandle
	char * ci
	char * key
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CODE:
	RETVAL = CIMInstanceGetIntegerValue(nh,cci,key);
	OUTPUT:
	RETVAL

char *	CIMInstanceGetRefProperty(npiHandle, ci, key)
	char * npiHandle
	char * ci
	char * key
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CIMObjectPath cop;
	CODE:
	cop = CIMInstanceGetRefValue(nh,cci,key);
	RETVAL = argsforperl(cop.ptr);
	OUTPUT:
	RETVAL


char *	CIMObjectPathFromCIMInstance(npiHandle ,ci)
	char * npiHandle
	char * ci
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cci = {argsfromperl(ci)};
	CIMObjectPath cop;
	CODE:
	cop = CIMObjectPathFromCIMInstance(nh,cci);
	RETVAL = argsforperl(cop.ptr);
	OUTPUT:
	RETVAL

char *	CIMObjectPathNew(npiHandle, name)
	char * npiHandle
	char * name
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath cop;
	CODE:
	cop = CIMObjectPathNew(nh,name);
	RETVAL = argsforperl(cop.ptr);
	OUTPUT:
	RETVAL

char *	CIMObjectPathGetClassName(npiHandle ,cop)
	char * npiHandle
	char * cop
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CODE:
	RETVAL = CIMObjectPathGetClassName(nh,ccop);
	OUTPUT:
	RETVAL

void	CIMObjectPathSetNameSpace(npiHandle ,cop, name)
	char * npiHandle
	char * cop
	char * name
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CODE:
	CIMObjectPathSetNameSpace(nh, ccop, name);

void	CIMObjectPathSetNameSpaceFromCIMObjectPath(npiHandle ,cop, cop2)
	char * npiHandle
	char * cop	
	char * cop2
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CIMObjectPath ccop2 = {argsfromperl(cop2)};
	CODE:
	CIMObjectPathSetNameSpaceFromCIMObjectPath(nh ,ccop, ccop2);

char *	CIMObjectPathGetNameSpace(npiHandle ,cop)
	char * npiHandle
	char * cop
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CODE:
	RETVAL = CIMObjectPathGetNameSpace(nh,ccop);
	OUTPUT:
	RETVAL

void	CIMObjectPathAddStringKeyValue (npiHandle, cop, key, value)
	char * npiHandle
	char * cop
	char * key
	char * value
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop;
	CODE:
	ccop.ptr = argsfromperl(cop);
	CIMObjectPathAddStringKeyValue (nh,ccop,key,value);

void	CIMObjectPathAddIntegerKeyValue(npiHandle, cop, key, value)
	char * npiHandle
	char * cop
	char * key
	int value
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CODE:
	CIMObjectPathAddIntegerKeyValue (nh,ccop,key,value);

void	CIMObjectPathAddRefKeyValue(npiHandle, cop, key, value)
	char * npiHandle
	char * cop
	char * key
	char * value
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CIMObjectPath cv = {argsfromperl(value)};
	CODE:
	CIMObjectPathAddRefKeyValue (nh, ccop, key, cv);


char *	CIMObjectPathGetStringKeyValue(npiHandle, cop, key)
	char * npiHandle
	char * cop
	char * key
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CODE:
	RETVAL = CIMObjectPathGetStringKeyValue(nh, ccop, key);
	OUTPUT:
	RETVAL

int	CIMObjectPathGetIntegerKeyValue(npiHandle, cop, key)
	char * npiHandle
	char * cop
	char * key
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CODE:
	RETVAL = CIMObjectPathGetIntegerKeyValue(nh, ccop, key);
	OUTPUT:
	RETVAL

char *	CIMObjectPathGetRefKeyValue(npiHandle, cop, key)
	char * npiHandle
	char * cop
	char * key
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CIMObjectPath cv;
	CODE:
	cv = CIMObjectPathGetRefKeyValue(nh, ccop, key);
	RETVAL = argsforperl(cv.ptr);
	OUTPUT:
	RETVAL

char *	SelectExpSelectString(npiHandle, sxp)
	char * npiHandle
	char * sxp
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	SelectExp sxpp = {argsfromperl(sxp)};
	CODE:
	RETVAL = SelectExpGetSelectString(nh, sxpp);
	OUTPUT:
	RETVAL

char *	CIMOMGetClass(npiHandle, cop, i)
	char * npiHandle
	char * cop
	int i
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CIMClass cc;
	CODE:
	cc = CIMOMGetClass(nh, ccop, i);
	RETVAL = argsforperl(cc.ptr);
	OUTPUT:
	RETVAL

char *	CIMOMEnumInstanceNames(npiHandle, cop, i)
	char * npiHandle
	char * cop
	int i
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	Vector v;
	CODE:
	v = CIMOMEnumInstanceNames(nh, ccop, i);
	RETVAL = v.ptr;
	OUTPUT:
	RETVAL

char *	CIMOMEnumInstances(npiHandle, cop, i, j)
	char * npiHandle
	char * cop
	int i
	int j
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	Vector v;
	CODE:
	v = CIMOMEnumInstances(nh, ccop, i, j);
	RETVAL = argsforperl(v.ptr);
	OUTPUT:
	RETVAL

char *	CIMOMGetInstance(npiHandle, cop , i)
	char * npiHandle
	char * cop
	int i
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMObjectPath ccop = {argsfromperl(cop)};
	CIMInstance ci;
	CODE:
	ci = CIMOMGetInstance(nh, ccop, i);
	RETVAL = argsforperl(ci.ptr);
	OUTPUT:
	RETVAL

void	CIMOMDeliverProcessEvent(npiHandle, ns , ind)
	char * npiHandle
	char * ns 
	char * ind 
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cin = {argsfromperl(ind)};
	CODE:
	CIMOMDeliverProcessEvent(nh, ns, cin);

void	CIMOMDeliverInstanceEvent(npiHandle, ns , ind, src, prev)
	char * npiHandle
	char * ns 
	char * ind 
	char * src
	char * prev
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CIMInstance cin = {argsfromperl(ind)};
	CIMInstance cs = {argsfromperl(src)};
	CIMInstance cp = {argsfromperl(prev)};
	CODE:
	CIMOMDeliverInstanceEvent(nh, ns, cin, cs, cp);

char*	CIMOMPrepareAttach(npiHandle)
	char * npiHandle
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	RETVAL = argsforperl(CIMOMPrepareAttach(nh));
	OUTPUT:
	RETVAL

void	CIMOMCancelAttach(npiHandle)
	char * npiHandle
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	CIMOMCancelAttach(nh);

void	CIMOMAttachThread(npiHandle)
	char * npiHandle
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	CIMOMAttachThread(nh);

void	CIMOMDetachThread(npiHandle)
	char * npiHandle
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	CIMOMDetachThread(nh);

int	errorCheck(npiHandle)
	char * npiHandle
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	RETVAL = errorCheck(nh);
	OUTPUT:
	RETVAL

void	errorReset(npiHandle)
	char * npiHandle
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	errorReset(nh);

void	raiseError(npiHandle, errorstr)
	char * npiHandle
	char * errorstr
	PREINIT:
	NPIHandle * nh = argsfromperl(npiHandle);
	CODE:
	raiseError(nh, errorstr);

