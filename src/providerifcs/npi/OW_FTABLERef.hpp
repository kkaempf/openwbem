/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef OW_FTABLE_REF_HPP_
#define OW_FTABLE_REF_HPP_

//#include <string.h>
#include "OW_config.h"
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

#include "OW_SharedLibraryReference.hpp"
#include "npi.h"

typedef OW_Array<char *> charVect;

typedef enum {
  NOTHING,
  STRING,
  VECTOR,
  CIM_VALUE,
  CIM_QUALIFIER,
  CIM_PARAMVALUE,
  CIM_PROPERTY,
  CIM_INSTANCE,
  CIM_OBJECTPATH,
  CIM_CLASS
} NPIGarbageType;


class NPIContext {
public:
        char * scriptName;
        void * my_perl;
	OW_Array<void *> garbage;
	OW_Array<NPIGarbageType> garbageType;
};
// my_perl serves as pointer to the perl environment for the perlIFC

class NPIFTABLE {

public:
  //CIMProvider;
  FP_INITIALIZE         fp_initialize;
  FP_CLEANUP            fp_cleanup;
  //InstanceProvider;
  FP_ENUMINSTANCENAMES  fp_enumInstanceNames;
  FP_ENUMINSTANCES      fp_enumInstances;
  FP_GETINSTANCE        fp_getInstance;
  FP_CREATEINSTANCE     fp_createInstance;
  FP_SETINSTANCE        fp_setInstance;
  FP_DELETEINSTANCE     fp_deleteInstance;
  FP_EXECQUERY          fp_execQuery;
  //AssociatorProvider
  FP_ASSOCIATORS        fp_associators;
  FP_ASSOCIATORNAMES    fp_associatorNames;
  FP_REFERENCES         fp_references;
  FP_REFERENCENAMES     fp_referenceNames;
  //MethodProvider
  FP_INVOKEMETHOD       fp_invokeMethod;
  //EventProvider
  FP_AUTHORIZEFILTER    fp_authorizeFilter;
  FP_MUSTPOLL           fp_mustPoll;
  FP_ACTIVATEFILTER     fp_activateFilter;
  FP_DEACTIVATEFILTER   fp_deActivateFilter;

  NPIContext 		* npicontext;

  //NPIFTABLE& operator = (const ::FTABLE& original) {return *this;}
  NPIFTABLE& operator = (const ::FTABLE& original) 
  {
	memcpy(this, &original, sizeof(::FTABLE));
	return *this;
  }
};


typedef ::FTABLE (*FP_INIT_FT) (void);

typedef NPIFTABLE (*NPIFP_INIT_FT) (void);

typedef OW_SharedLibraryReference< ::NPIFTABLE> OW_FTABLERef;

#define OW_NOIDPROVIDERFACTORY(prov) OW_PROVIDERFACTORY(prov, NO_ID)

#define OW_PROVIDERFACTORY(prov, name) \
extern "C" const char* \
getOWVersion() \
{ \
        return OW_VERSION; \
} \
extern "C" OW_FTABLERef* \
createProvider##name() \
{ \
        return new prov; \
}


#endif

