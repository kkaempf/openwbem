/*
 * NPIAssociationProvider.cpp
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

#include "NPIAssociationProvider.h"
#include "NPIExternal.h"


PEGASUS_NAMESPACE_BEGIN

NPIAssociationProvider::NPIAssociationProvider() { }

NPIAssociationProvider::~NPIAssociationProvider() { }

void NPIAssociationProvider::associators(
    const OperationContext & context,
    const CIMReference & objectName,
    const String & associationClass,
    const String & resultClass,
    const String & role,
    const String & resultRole,
    const Uint32 flags,
    const Array<String> & propertyList,
    ResponseHandler<CIMObjectWithPath> & handler)
{
    String nameSpace = objectName.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIAssociationProvider::associators()\n";)
 
    if (_functionTable.fp_associators != NULL) {
        // initialize path
        CIMObject cimo;
        CIMObjectWithPath cowp(objectName,cimo);
        ::CIMObjectPath path = {(void *)&cowp};

        // initialize association class
        ::CIMObjectPath assoc;
        CIMObject cimo2;
        CIMReference cref2(objectName);
        CIMObjectWithPath cowp2;
        if (associationClass.size() > 0)
        {
            cref2.setClassName(associationClass);
            cowp2.set(cref2, cimo2);
            assoc.ptr = (void *)&cowp2;
        }
        else assoc.ptr = NULL;

        const char * _resultClass = resultClass.allocateCString();
        const char * _role = role.allocateCString();
        const char * _resultRole = resultRole.allocateCString();
        int plLen = propertyList.size();
        const char * _propertyList[plLen];

        for (int i = 0; i < plLen; i++)
            _propertyList[i] = propertyList[i].allocateCString();

        //const char * _propertyList[0];
        //int plLen = 0;
        // PropertyList usage not implemented
        int iq = flags & OperationFlag::INCLUDE_QUALIFIERS;
        int ic = flags & OperationFlag::INCLUDE_CLASS_ORIGIN;

        ::Vector v =
            _functionTable.fp_associators(_npiHandle,assoc,path,
                                          _resultClass,_role,_resultRole,
                                          iq, ic, _propertyList, plLen);

        int n=VectorSize(_npiHandle,v);
        DDD ( cerr << "Deliver " << n-1 << " associator instances\n";)
        ::CIMObjectPath cop;
	CIMInstance ci;
        for (int i=0; i < n; i++) {
            cop.ptr = _VectorGet(_npiHandle,v,i);
            ci = *(CIMInstance *)(cop.ptr);
            cowp2.set(cref2,ci); 
//
            handler.deliver(cowp2);
            free((CIMInstance *)cop.ptr);
//
        }
PROVIDER_EXIT
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
PROVIDER_EXIT
    }
}

void NPIAssociationProvider::associatorNames(
    const OperationContext & context,
    const CIMReference & objectName,
    const String & associationClass,
    const String & resultClass,
    const String & role,
    const String & resultRole,
    ResponseHandler<CIMReference> & handler)
{
    String nameSpace = objectName.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIAssociationProvider::associatorNames()\n";)

    if (_functionTable.fp_associatorNames != NULL) {
        // initialize path
        CIMObject cimo;
        CIMObjectWithPath cowp(objectName,cimo);
        ::CIMObjectPath path = {(void *)&cowp};

        // initialize association class
        ::CIMObjectPath assoc;
        CIMObject cimo2;
        CIMReference cref2(objectName);
        CIMObjectWithPath cowp2;
        if (associationClass.size() > 0)
        {
            cref2.setClassName(associationClass);
            cowp2.set(cref2, cimo2);
            assoc.ptr = (void *)&cowp2;
        }
        else assoc.ptr = NULL;

        const char * _resultClass = resultClass.allocateCString();
        const char * _role = role.allocateCString();
        const char * _resultRole = resultRole.allocateCString();

        ::Vector v =
            _functionTable.fp_associatorNames(_npiHandle,assoc,path,
                                             _resultClass,_role,_resultRole);

        ::CIMObjectPath cop;
        for (int i=VectorSize(_npiHandle,v)-1; i >= 0; i--) {
            cop.ptr = _VectorGet(_npiHandle,v,i);
            CIMReference * mycop =
               new CIMReference(((CIMObjectWithPath *)(cop.ptr))->
                     getReference());
            free((CIMObjectWithPath *)cop.ptr);
//
            handler.deliver(*mycop);
//
        }
PROVIDER_EXIT
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
PROVIDER_EXIT
    }
}

void NPIAssociationProvider::references(
    const OperationContext & context,
    const CIMReference & objectName,
    const String & resultClass,
    const String & role,
    const Uint32 flags,
    const Array<String> & propertyList,
    ResponseHandler<CIMObjectWithPath> & handler)
{
    String nameSpace = objectName.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIAssociationProvider::references()\n";)

    if (_functionTable.fp_references != NULL) {
        // initialize path
        CIMObject cimo;
        CIMObjectWithPath cowp(objectName,cimo);
        ::CIMObjectPath path = {(void *)&cowp};

        // initialize result class as assoc class
        ::CIMObjectPath assoc;
        CIMObject cimo2;
        CIMReference cref2(objectName);
        CIMObjectWithPath cowp2;
        if (resultClass.size() > 0)
        {
            cref2.setClassName(resultClass);
            cowp2.set(cref2, cimo2);
            assoc.ptr = (void *)&cowp2;
        }
        else assoc.ptr = NULL;

        const char * _role = role.allocateCString();
        int plLen = propertyList.size();
        const char * _propertyList[plLen];

        for (int i = 0; i < plLen; i++)
            _propertyList[i] = propertyList[i].allocateCString();


        int iq = flags & OperationFlag::INCLUDE_QUALIFIERS;
        int ic = flags & OperationFlag::INCLUDE_CLASS_ORIGIN;

        ::Vector v =
            _functionTable.fp_references(_npiHandle,assoc,path,_role, 
                                         iq, ic, _propertyList, plLen);

        ::CIMObjectPath cop;
	CIMInstance ci;
        for (int i=VectorSize(_npiHandle,v)-1; i >= 0; i--) {
            cop.ptr = _VectorGet(_npiHandle,v,i);
            ci = *(CIMInstance *)(cop.ptr);
            cowp2.set(cref2,ci); 
//
            handler.deliver(cowp2);
            free((CIMInstance *)cop.ptr);
//
        }
PROVIDER_EXIT
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
PROVIDER_EXIT
    }
}

void NPIAssociationProvider::referenceNames(
    const OperationContext & context,
    const CIMReference & objectName,
    const String & resultClass,
    const String & role,
    ResponseHandler<CIMReference> & handler)
{
    String nameSpace = objectName.getNameSpace();
PROVIDER_ENTRY
    DDD( cerr << "NPIAssociationProvider::referenceNames()\n";)

    if (_functionTable.fp_referenceNames != NULL) {
        // initialize path
        CIMObject cimo;
        CIMObjectWithPath cowp(objectName,cimo);
        ::CIMObjectPath path = {(void *)&cowp};

        // initialize association class from result class
        ::CIMObjectPath assoc;
        CIMObject cimo2;
        CIMReference cref2(objectName);
        CIMObjectWithPath cowp2;
        if (resultClass.size() > 0)
        {
            cref2.setClassName(resultClass);
            cowp2.set(cref2, cimo2);
            assoc.ptr = (void *)&cowp2;
        }
        else assoc.ptr = NULL;

        const char * _role = role.allocateCString();

        ::Vector v =
            _functionTable.fp_referenceNames(_npiHandle,assoc,path,_role);

        ::CIMObjectPath cop;
        for (int i=VectorSize(_npiHandle,v)-1; i >= 0; i--) {
            cop.ptr = _VectorGet(_npiHandle,v,i);
            CIMReference * mycop =
               new CIMReference(((CIMObjectWithPath *)(cop.ptr))->
                     getReference());
            free((CIMObjectWithPath *)cop.ptr);
//
            handler.deliver(*mycop);
//
        }
PROVIDER_EXIT
    }
    else {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
PROVIDER_EXIT
    }
}



PEGASUS_NAMESPACE_END

//externalized functions
//#include "NPIExternalImpl.h"
