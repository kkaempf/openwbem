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

#ifndef PegasusProvider_NPIProvider_h
#define PegasusProvider_NPIProvider_h


#include "OW_Bool.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_String.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParameter.hpp"
#include "OW_Array.hpp"
#include "OW_CIMBase.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include <stdlib.h>
#include <dlfcn.h>
#include "cmpisrv.h"


typedef struct {
        char* operationContext;
	OW_CIMOMHandleIFCRef _cimomhandle;
	OW_String _nameSpace;
} NPIenv;

#define PROVIDER_ENTRY \
_npiHandle->thisObject = (void *)createEnv(_repository, nameSpace); \
((NPIenv *)_npiHandle->thisObject)->operationContext = getOperationContext(); \
handler.processing();

#define PROVIDER_EXIT \
deleteEnv((NPIenv *)_npiHandle->thisObject); \
handler.complete();


typedef OW_Array<char *> charVect;

#endif /* PegasusProvider_NPIProvider_h */
