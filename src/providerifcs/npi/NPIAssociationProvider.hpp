/*
 * NPIProvider.h
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

#ifndef PegasusProvider_NPIAssociationProvider_h
#define PegasusProvider_NPIAssociationProvider_h

#include "NPIExternal.h"
#include "NPIBaseProvider.h"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider2/CIMAssociationProvider.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PROVIDER_LINKAGE NPIAssociationProvider :
    public NPIBaseProvider,
    public virtual CIMAssociationProvider
{
public:

    NPIAssociationProvider();
    ~NPIAssociationProvider();

    virtual void initialize(CIMOMHandle & cimom) {}
    virtual void terminate() {}

    void assoc_initialize(CIMOMHandle & cimom, char * libraryName)
    {
        NPIBaseProvider::NPI_initialize(cimom,libraryName);
    }
    void assoc_terminate() { NPIBaseProvider::terminate(); }

    // CIMAssociationProvider interface
    void associators(
        const OperationContext & context,
        const CIMReference & objectName,
        const String & associationClass,
        const String & resultClass,
        const String & role,
        const String & resultRole,
        const Uint32 flags,
        const Array<String> & propertyList,
        ResponseHandler<CIMObjectWithPath> & handler);

    void associatorNames(
        const OperationContext & context,
        const CIMReference & objectName,
        const String & associationClass,
        const String & resultClass,
        const String & role,
        const String & resultRole,
        ResponseHandler<CIMReference> & handler);

    void references(
        const OperationContext & context,
        const CIMReference & objectName,
        const String & resultClass,
        const String & role,
        const Uint32 flags,
        const Array<String> & propertyList,
        ResponseHandler<CIMObjectWithPath> & handler);

    void referenceNames(
        const OperationContext & context,
        const CIMReference & objectName,
        const String & resultClass,
        const String & role,
        ResponseHandler<CIMReference> & handler);

protected:

    //FTABLE _functionTable;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusProvider_NPIAssociationProvider_h */
