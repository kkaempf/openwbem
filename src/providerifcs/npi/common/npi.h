/*
 * npi.h
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
 * Description: All typedefs and general definitions for the NPI
 */

#ifndef _NPI_H_
#define _NPI_H_

#ifndef _COMPILE_UNIX
#define _COMPILE_UNIX
#include "os_compile.h"
#endif

#ifdef _COMPILE_WIN32
#define NPIEXPORT __declspec(dllexport)
#define NPICALL __stdcall
#else
#define NPIEXPORT
#define NPICALL
#endif

/* the default maximum number of libraries to be linked simultaneously */
#ifndef NPI_MAX_DLL
#define NPI_MAX_DLL 100
#endif

/* 
 * typedef for an NPI handle, given with any function to store errors
 * and to resolve them 
 */  
typedef struct {
  void *          jniEnv;
  int             errorOccurred;
  const char *    providerError;
  void *          thisObject;
} NPIHandle;

typedef struct {
  char *          operationContext;
} NPIenvironment;

/* CIM Value Type */
typedef enum {
  CIM_INTEGER,
  CIM_STRING,
  CIM_REF 
} CIMType;
  
/* these are the pseudo structs to handle the CIMOM objects in C */
typedef struct { void * ptr; } CIMObjectPath;
typedef struct { void * ptr; } CIMOMHandle;
typedef struct { void * ptr; } CIMInstance;
typedef struct { void * ptr; } CIMClass;
typedef struct { void * ptr; } CIMParameter;
typedef struct { void * ptr; } CIMValue;
typedef struct { void * ptr; } Vector;
  
/* typedefs for CIMProvider function pointers for the linked libraries */
typedef void (*FP_INITIALIZE) ( NPIHandle *, CIMOMHandle );
typedef void (*FP_CLEANUP) ( NPIHandle * );

/* typedefs for InstanceProvider function pointers for the linked libraries */
typedef Vector (*FP_ENUMINSTANCENAMES) ( NPIHandle *, CIMObjectPath, int, 
					 CIMClass );
typedef Vector (*FP_ENUMINSTANCES) ( NPIHandle *, CIMObjectPath, int, CIMClass,
				     int );
typedef CIMInstance (*FP_GETINSTANCE) ( NPIHandle *, CIMObjectPath, CIMClass, int );
typedef CIMObjectPath (*FP_CREATEINSTANCE) ( NPIHandle *, CIMObjectPath, 
					     CIMInstance );
typedef void (*FP_SETINSTANCE) ( NPIHandle *, CIMObjectPath, CIMInstance );
typedef void (*FP_DELETEINSTANCE) ( NPIHandle *, CIMObjectPath );
typedef Vector (*FP_EXECQUERY) ( NPIHandle *, CIMObjectPath, const char *, int,
				 CIMClass );

/* typedefs for AssociatorProvider function pointers for the linked libraries */
typedef Vector (*FP_ASSOCIATORS) ( NPIHandle *, CIMObjectPath, CIMObjectPath,
				   const char *, const char *, const char *,
				   int, int, const char *[], int );
typedef Vector (*FP_ASSOCIATORNAMES) ( NPIHandle *, CIMObjectPath,
				       CIMObjectPath, const char *, 
				       const char *, const char * );
typedef Vector (*FP_REFERENCES) ( NPIHandle *, CIMObjectPath, CIMObjectPath,
				  const char *, int, int, const char *[], 
				  int );
typedef Vector (*FP_REFERENCENAMES) ( NPIHandle *, CIMObjectPath, 
				      CIMObjectPath, const char * );
typedef CIMValue (*FP_INVOKEMETHOD) ( NPIHandle *, CIMObjectPath, 
				      const char *, Vector, Vector );
  

/* the function pointer table returned by <LIBNAME>_initFunctionTable() */  
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
}FTABLE;

/* the <LIBNAME>_initFunctionTable() pointer */
typedef FTABLE (*FP_INIT_FT) (void);

/* structure for the cache{Instance/Static}MethodNames arrays */
typedef struct {
  int clsIndex;
  const char * methodName;
  const char * signature;
} METHOD_STRUCT;

#endif
