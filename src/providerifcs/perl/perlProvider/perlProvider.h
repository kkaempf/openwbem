/*
 * NPIProvider.h
 *
 * Copyright (c) 2002, International Business Machines
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

#ifndef perlProvider_h
#define perlProvider_h

#include <EXTERN.h>
#include <perl.h>
EXTERN_C void xs_init();

//static const char * _libraryPath = "/home/markus/src/perlProvider/perl/";
//static const char * _perlProviderVersion = "0.3";

#include "perlargs.h"

#include <stdlib.h>
#include <dlfcn.h>

#ifdef NPI_DEBUG
#define DDD(X) X
#else
#define DDD(X)
#endif

#include <npi.h>

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

  PerlContext *         perlcontext;
} PerlFTABLE;


#endif /* perlProvider_h */

