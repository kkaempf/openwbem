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

#ifndef PegasusProvider_NPIInstanceProvider_h
#define PegasusProvider_NPIInstanceProvider_h

#include "NPIExternal.h"
#include "NPIBaseProvider.h"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PROVIDER_LINKAGE NPIInstanceProvider :
     public NPIBaseProvider,
     public virtual CIMInstanceProvider
{
public:

    NPIInstanceProvider();
    ~NPIInstanceProvider();

    virtual void initialize(CIMOMHandle & cimom) {}
    virtual void terminate() {}

    void inst_initialize(CIMOMHandle & cimom, char * libraryName)
    {
        NPIBaseProvider::NPI_initialize(cimom,libraryName);
    }
    void inst_terminate() { NPIBaseProvider::terminate(); }

    // CIMInstanceProvider interface
    void getInstance(
        const OperationContext & context,
        const CIMReference & instanceReference,
        const Uint32 flags,
        const Array<String> & propertyList,
        ResponseHandler<CIMInstance> & handler);

    void deleteInstance(
        const OperationContext & context,
        const CIMReference & instanceReference,
        ResponseHandler<CIMInstance> & handler);

    void createInstance(
        const OperationContext & context,
        const CIMReference & instanceReference,
        const CIMInstance & instanceObject,
        ResponseHandler<CIMReference> & handler);

    void modifyInstance(
        const OperationContext & context,
        const CIMReference & instanceReference,
        const CIMInstance & instanceObject,
        const Uint32 flags,
        const Array<String> & propertyList,
        ResponseHandler<CIMInstance> & handler);

    void enumerateInstances(
        const OperationContext & context,
        const CIMReference & classReference,
        const Uint32 flags,
        const Array<String> & propertyList,
        ResponseHandler<CIMNamedInstance> & handler);

    void enumerateInstanceNames(
        const OperationContext & context,
        const CIMReference & classReference,
        ResponseHandler<CIMReference> & handler);

protected:

    //FTABLE _functionTable;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusProvider_NPIInstanceProvider_h */
