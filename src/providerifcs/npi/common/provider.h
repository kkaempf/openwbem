/*
 * provider.h
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author: Frank Scheffler <frank.scheffler@de.ibm.com>
 * Contributors:
 *
 * Description: InstanceProvider function definitions and the 
 *              <LIBNAME>_initFunctionTable macro.
 */
#ifndef _PROVIDER_H_
#define _PROVIDER_H_

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#include "npi.h"

/* 
 * These are the CIMProvider methods, that have to be written for any provider.
 */
static void initialize ( NPIHandle *, CIMOMHandle );
static void cleanup ( NPIHandle * );


/*
 * This part defines the InstanceProvider methods and their function pointers,
 * if the provider is meant to be an instance provider.
 */
#ifdef INSTANCE_PROVIDER
#define _INCLUDE_INSTANCE_PROVIDER_METHODS \
            enumInstanceNames, enumInstances, \
            getInstance, createInstance, setInstance, deleteInstance, 0,
//            getInstance, createInstance, setInstance, deleteInstance, execQuery,
#else
#define _INCLUDE_INSTANCE_PROVIDER_METHODS \
            0, 0, 0, 0, 0, 0, 0,
#endif

static Vector enumInstanceNames ( NPIHandle *, CIMObjectPath, int, CIMClass );
static Vector enumInstances ( NPIHandle *, CIMObjectPath, int, CIMClass, int );
static CIMInstance getInstance ( NPIHandle *, CIMObjectPath, CIMClass, int );
static CIMObjectPath createInstance ( NPIHandle *, CIMObjectPath, 
				      CIMInstance );
static void setInstance ( NPIHandle *, CIMObjectPath, CIMInstance );
static void deleteInstance ( NPIHandle *, CIMObjectPath );
/*static Vector execQuery ( NPIHandle *, CIMObjectPath, const char *, int, 
			  CIMClass );*/

/* 
 * This part defines the AssociatorProvider methods and their function 
 * pointers, if the provider is meant to be an associator provider.
 */
#ifdef ASSOCIATOR_PROVIDER
#define _INCLUDE_ASSOCIATOR_PROVIDER_METHODS \
            associators, associatorNames, references, referenceNames,
static Vector associators ( NPIHandle *, CIMObjectPath, CIMObjectPath,
			    const char *, const char *, const char *,
			    int, int, const char * [], int);
static Vector associatorNames ( NPIHandle *, CIMObjectPath, CIMObjectPath,
				const char *, const char *, const char * );
static Vector references ( NPIHandle *, CIMObjectPath, CIMObjectPath,
			   const char *, int, int, const char * [], int);
static Vector referenceNames ( NPIHandle *, CIMObjectPath, CIMObjectPath, 
			       const char * );
#else
#define _INCLUDE_ASSOCIATOR_PROVIDER_METHODS \
            0, 0, 0, 0,
#endif

/* 
 * This part defines the MethodProvider interface 
 */
#ifdef METHOD_PROVIDER
#define _INCLUDE_METHOD_PROVIDER_METHODS \
            invokeMethod,
static CIMValue invokeMethod ( NPIHandle *, CIMObjectPath, 
			       const char *, Vector, Vector);
#else
#define _INCLUDE_METHOD_PROVIDER_METHODS \
            0,
#endif



/* the macro to generate the exported <LIBNAME>_initFunctionTable() */
#define PROVIDER_NAME(n) EXTERN_C \
    NPIEXPORT FTABLE NPICALL n##_initFunctionTable ( void ) { \
      FTABLE fTable = { \
             initialize, cleanup, \
             /* the local provider function pointers will be included here */ \
             _INCLUDE_INSTANCE_PROVIDER_METHODS \
             _INCLUDE_ASSOCIATOR_PROVIDER_METHODS \
             _INCLUDE_METHOD_PROVIDER_METHODS \
             }; \
      return fTable; \
    }

#endif



