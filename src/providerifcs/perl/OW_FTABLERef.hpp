/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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

#include "OW_config.h"

#include "OW_SharedLibraryReference.hpp"
#include "npi.h"

typedef struct {
  char * scriptName;
  void * my_perl;
} PerlContext;

typedef struct {
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

  PerlContext 		* perlcontext;
} PerlFTABLE;

typedef PerlFTABLE (*PerlFP_INIT_FT) (void);

typedef OW_SharedLibraryReference< ::PerlFTABLE> OW_FTABLERef;

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

