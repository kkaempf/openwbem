/*
 * NPIInstanceProvider.cpp
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

#include <Pegasus/Common/Config.h>
#include <unistd.h>
#include <stdio.h>

#include <Pegasus/Provider2/SimpleResponseHandler.cpp>

#include "NPIInstanceProvider.h"
#include "NPIExternal.h"


PEGASUS_NAMESPACE_BEGIN

NPIInstanceProvider::NPIInstanceProvider() { }

NPIInstanceProvider::~NPIInstanceProvider() { }

void NPIInstanceProvider::getInstance(
        const OperationContext & context,
        const CIMReference & instanceReference,
        const Uint32 flags,
        const Array<String> & propertyList,
        ResponseHandler<CIMInstance> & handler)
{
    String nameSpace = instanceReference.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIInstanceProvider::getInstance("<< instanceReference.toString() <<")\n";)

    if (!instanceReference.isInstanceName())
            throw CIMException(CIM_ERR_INVALID_PARAMETER);
    if (_functionTable.fp_getInstance != NULL) {
        CIMObject cimo;
        CIMObjectWithPath cowp(instanceReference,cimo);

        ::CIMObjectPath cop = {(void *)&cowp};
        CIMClass myclass = NPI_getmyClass(
                  _npiHandle, nameSpace,instanceReference.getClassName());
        ::CIMClass cc = {(void *)&myclass};
        ::CIMInstance ci =
                _functionTable.fp_getInstance(_npiHandle, cop, cc, flags);
        CIMInstance * myci;
        if (ci.ptr) {
           myci = new CIMInstance(*(CIMInstance *)(ci.ptr));
           free((CIMInstance *)ci.ptr);
//
           handler.deliver(*myci);
//
        }
        else {
            DDD( cerr << "NPIInstanceProvider::getInstance is empty\n";)
PROVIDER_EXIT
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
PROVIDER_EXIT
    }
    else {
PROVIDER_EXIT
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
}

void NPIInstanceProvider::deleteInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    ResponseHandler<CIMInstance> & handler)
{
    String nameSpace = instanceReference.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIInstanceProvider::deleteInstance()\n";)
    if (_functionTable.fp_deleteInstance != NULL) {
        CIMObject cimo;
        CIMObjectWithPath cowp(instanceReference,cimo);
        ::CIMObjectPath cop = {(void *)&cowp};
        _functionTable.fp_deleteInstance(_npiHandle,cop);
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
PROVIDER_EXIT
}

void NPIInstanceProvider::createInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    const CIMInstance & instanceObject,
    ResponseHandler<CIMReference> & handler)
{
    String nameSpace = instanceReference.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIInstanceProvider::createInstance()\n";)
    if (_functionTable.fp_createInstance != NULL) {
        CIMObject cimo;
        CIMObjectWithPath cowp(instanceReference,cimo);
        ::CIMObjectPath cop = {(void *)&cowp};
        ::CIMInstance ci = {(void *)&instanceObject};
        _functionTable.fp_createInstance(_npiHandle,cop,ci);
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
PROVIDER_EXIT
}

void NPIInstanceProvider::modifyInstance(
    const OperationContext & context,
    const CIMReference & instanceReference,
    const CIMInstance & instanceObject,
    const Uint32 flags,
    const Array<String> & propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    String nameSpace = instanceReference.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIInstanceProvider::modifyInstance()\n";)
    if (_functionTable.fp_setInstance != NULL) {
        CIMObject cimo;
        CIMObjectWithPath cowp(instanceReference,cimo);
        ::CIMObjectPath cop = {(void *)&cowp};
        ::CIMInstance ci = {(void *)&instanceObject};
        _functionTable.fp_setInstance(_npiHandle,cop,ci);
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
PROVIDER_EXIT
}

void NPIInstanceProvider::enumerateInstances(
    const OperationContext & context,
    const CIMReference & classReference,
    const Uint32 flags,
    const Array<String> & propertyList,
    ResponseHandler<CIMNamedInstance> & handler)
{
    String nameSpace = classReference.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIInstanceProvider::enumerateInstances()\n";)

    if (_functionTable.fp_enumInstances!= NULL) {

        CIMClass myclass = NPI_getmyClass(_npiHandle, nameSpace,
           classReference.getClassName());
        ::CIMClass cc = {(void *)&myclass};

        CIMObject cimo;
        CIMObjectWithPath cowp(classReference,cimo);
        ::CIMObjectPath cop = {(void *)&cowp};

	int di = flags | OperationFlag::DEEP_INHERITANCE;
	int lo = flags | OperationFlag::LOCAL_ONLY;

        ::Vector v =
            _functionTable.fp_enumInstances(_npiHandle,cop,di,cc,lo);

        ::CIMInstance ci;
        for (int i=VectorSize(_npiHandle,v)-1; i >= 0; i--) {
            ci.ptr = _VectorGet(_npiHandle,v,i);
            CIMNamedInstance * myci = new CIMNamedInstance(
               classReference, *(CIMInstance *)(ci.ptr));
            free((CIMInstance *)ci.ptr);
//
            handler.deliver(*myci);
//
        }
PROVIDER_EXIT
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
PROVIDER_EXIT
    }
}

void NPIInstanceProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMReference & classReference,
    ResponseHandler<CIMReference> & handler)
{
    DDD( cerr << "NPIInstanceProvider::enumerateInstanceNames()\n";)
    String nameSpace = classReference.getNameSpace();
PROVIDER_ENTRY
    cerr << "Operation Context is " << ((NPIenv *)_npiHandle->thisObject)->operationContext << endl;
    DDD( cerr << "NPIInstanceProvider::enumerateInstanceNames()\n";)
    if (_functionTable.fp_enumInstanceNames != NULL) {

        CIMClass myclass = NPI_getmyClass(_npiHandle, nameSpace,
                 classReference.getClassName());
        ::CIMClass cc = {(void *)&myclass};

        CIMObject cimo;
        CIMObjectWithPath cowp(classReference,cimo);
        ::CIMObjectPath cop = {(void *)&cowp};

        ::Vector v =
            _functionTable.fp_enumInstanceNames(_npiHandle,cop,true,cc);

        for (int i=0,n=VectorSize(_npiHandle,v); i < n; i++) {
            cop.ptr = _VectorGet(_npiHandle,v,i);
	    CIMObjectWithPath * mycowp =
                new CIMObjectWithPath(*(CIMObjectWithPath *)(cop.ptr));
            free((CIMObjectWithPath *)cop.ptr);
//
            handler.deliver(mycowp->getReference());
//
        }
PROVIDER_EXIT
    }
    else {
PROVIDER_EXIT
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
}

PEGASUS_NAMESPACE_END

//externalized functions
//#include "NPIExternalImpl.h"
