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
#ifndef __OWCPIPROV_H__
#define __OWCPIPROV_H__

#ifdef __cplusplus

#define CPI_EXPORT extern "C"
extern "C" {

#else

#define CPI_EXPORT

#endif	/* __cplusplus */

#include <stddef.h>

#define cpiFALSE 0
#define cpiTRUE  1

#define cpiSUCCESS 0
#define cpiFAILURE -1

typedef int cpiBool;
typedef int cpiFlavor;
typedef unsigned char cpiUint8;
typedef signed char cpiSint8;
typedef unsigned short cpiUint16;
typedef signed short cpiSint16;
typedef unsigned int cpiUint32;
typedef signed int cpiSint32;
typedef unsigned long long cpiUint64;
typedef signed long long cpiSint64;
typedef float cpiReal32;
typedef double cpiReal64;
typedef unsigned short cpiChar16;

typedef struct
{
	cpiUint16 year;
	cpiUint8 month;
	cpiUint32 days;
	cpiUint8 hours;
	cpiUint8 minutes;
	cpiUint8 seconds;
	cpiUint32 microSeconds;
	cpiSint16 utc;
	cpiBool isInterval;

} cpiDateTimeVals;


/*
	Scope types
*/
enum
{
	/** Schema scope */
	cpiScopeSCHEMA		= 1,

	/** Class scope */
	cpiScopeCLASS		= 2,

	/** Association scope */
	cpiScopeASSOCIATION	= 3,

	/** Inidcation scope */
	cpiScopeINDICATION	= 4,

	/** Property scope */
	cpiScopePROPERTY	= 5,

	/** Reference scope */
	cpiScopeREFERENCE	= 6,

	/** Method scope */
	cpiScopeMETHOD		= 7,

	/** Parameter scope */
	cpiScopePARAMETER	= 8,

	/** Qualifier scope */
	cpiScopeQUALIFIER	= 9,

	/** Instance scope */
	cpiScopeINSTANCE	= 10,

	/** Any */
	cpiScopeANY			= 11
};

/*
	Qualifier flavor types.
*/
enum
{
	/** Invalid flavor */
	cpiFlavorINVALID = 0,

	/** Overridable */
	cpiFlavorENABLEOVERRIDE	= 1,

	/** Cannot be overriden */
	cpiFlavorDISABLEOVERRIDE = 2,

	/** Applies only to the declaring class */
	cpiFlavorRESTRICTED	= 3,

	/** Qualifier is inherited */
	cpiFlavorTOSUBCLASS	= 4,

	/** Qualifier can be specified in multiple locales */
	cpiFlavorTRANSLATE = 5,

	/** Qualifier appears in instances */
	cpiFlavorTOINSTANCE	= 6,

	// Mark limit for scopes
	cpiFlavorLASTVALUE = 7
};

/*
	Qualifier types
*/

/* Meta qualifiers */
#define CPI_QUAL_ASSOCIATION    "association"
#define CPI_QUAL_INDICATION		"indication"

/* Standard qualifiers */
#define CPI_QUAL_ABSTRACT 		"abstract"
#define CPI_QUAL_AGGREGATE 		"aggregate"
#define CPI_QUAL_AGGREGATION 	"aggregation"
#define CPI_QUAL_ALIAS 			"alias"
#define CPI_QUAL_ARRAYTYPE 		"arraytype"
#define CPI_QUAL_BITMAP 		"bitmap"
#define CPI_QUAL_BITVALUES 		"bitvalues"
#define CPI_QUAL_COUNTER 		"counter"
#define CPI_QUAL_DESCRIPTION 	"description"
#define CPI_QUAL_DISPLAYNAME 	"displayname"
#define CPI_QUAL_GAUGE 			"gauge"
#define CPI_QUAL_IN 			"in"
#define CPI_QUAL_KEY 			"key"
#define CPI_QUAL_MAPPINGSTRINGS	"mappingstrings"
#define CPI_QUAL_MAX 			"max"
#define CPI_QUAL_MAXLEN 		"maxlen"
#define CPI_QUAL_MAXVALUE 		"maxvalue"
#define CPI_QUAL_MIN 			"min"
#define CPI_QUAL_MINVALUE 		"minvalue"
#define CPI_QUAL_NONLOCAL 		"nonlocal"
#define CPI_QUAL_NONLOCALTYPE 	"nonlocaltype"
#define CPI_QUAL_NULLVALUE 		"nullvalue"
#define CPI_QUAL_OUT 			"out"
#define CPI_QUAL_OVERRIDE 		"override"
#define CPI_QUAL_PROPAGATED 	"propagated"
#define CPI_QUAL_READ 			"read"
#define CPI_QUAL_REQUIRED 		"required"
#define CPI_QUAL_REVISION 		"revision"
#define CPI_QUAL_SCHEMA 		"schema"
#define CPI_QUAL_SOURCE 		"source"
#define CPI_QUAL_SOURCETYPE 	"sourcetype"
#define CPI_QUAL_STATIC 		"static"
#define CPI_QUAL_TERMINAL 		"terminal"
#define CPI_QUAL_UNITS 			"units"
#define CPI_QUAL_VALUEMAP 		"valuemap"
#define CPI_QUAL_VALUES 		"values"
#define CPI_QUAL_VERSION 		"version"
#define CPI_QUAL_WEAK 			"weak"
#define CPI_QUAL_WRITE 			"write"
#define CPI_QUAL_PROVIDER		"provider"

/*
	CIM Data types
*/
enum
{
	cpiCIMUINT8		= 1, 
	cpiCIMSINT8	    = 2, 
	cpiCIMUINT16   	= 3, 
	cpiCIMSINT16   	= 4, 
	cpiCIMUINT32   	= 5, 
	cpiCIMSINT32   	= 6, 
	cpiCIMUINT64   	= 7, 
	cpiCIMSINT64   	= 8, 
	cpiCIMSTRING   	= 9, 
	cpiCIMBOOLEAN  	= 10,
	cpiCIMREAL32   	= 11,
	cpiCIMREAL64   	= 12,
	cpiCIMDATETIME 	= 13,
	cpiCIMCHAR16   	= 14,
	cpiCIMREFERENCE	= 15,
};

/**
 * A handle given to providers when they are called. This handle will have to
 * be used in calls requesting CIMOM services.
 */
typedef void* cpiProviderHdl;

/** CIM Class handle */
typedef void* cpiClsHdl;

/** CIM Instance handle */
typedef void* cpiInstHdl;

/** CIM Method handle */
typedef void* cpiMethHdl;

/** CIM Namespace handle */
typedef void* cpiNSHdl;

/** CIM Reference handle (Object Path) */
typedef void* cpiRefHdl;

/** CIM Parameter handle */
typedef void* cpiParmHdl;

/** CIM Property handle */
typedef void* cpiPropHdl;

/** CIM Qualifier handle */
typedef void* cpiQualHdl;

/** CIM Qualifier type handle */
typedef void* cpiQualTHdl;

/** CIM Value handle */
typedef void* cpiValHdl;

/** CIM DateTime handle */
typedef void* cpiDTHdl;

/**
 * Generic object handle. Used primarily in vector related services.
 * The intent is to be able to represent all of the other handles
 * with a cpiObjHdl with the exception of a cpiProviderHdl.
 */
typedef void* cpiObjHdl;

/** A handle to a generic vector */
typedef void* cpiVectHdl;

/*-----------------------------------------------------------------------------
	General APIs
-----------------------------------------------------------------------------*/

/**
 * Get the string description of the given error code.
 * @param errorCode The error code to get the description for.
 * @return A null terminated string that contains the description of the
 *		given error code.
 */
const char* getErrorString(int errorCode);

/*-----------------------------------------------------------------------------
	Vector related APIs
-----------------------------------------------------------------------------*/

/**
 * Allocate and initialize a new vector
 * @param pVect A pointer to a cpiVectHdl that will hold the new vector handle.
 * @return 0 on success. Otherwise an error code.
 * Note: Any cpiVectHal allocated with cpiVectorAlloc must be deallocated with
 *		all call to cpiVectorFree
 */
int cpiVectorAlloc(cpiVectHdl* pVect);

/**
 * Deallocate a vector that was previously allocated with cpiVectorAlloc.
 * @param vect The cpiVectHdl to deallocate all resources for.
 * @return 0 on success. Otherwise an error code.
 */
int cpiVectorFree(cpiVectHdl vect);

/**
 * Append an item to the end of a vector. The cpiVectHdl must have been
 * previously initialized with a call to cpiVectorAlloc.
 * @param vect A handle to the vector perform this operation on.
 * @param obj The item to append to the end of the given vector.
 * @return 0 on success. Otherwise an error code.
 */
int cpiVectorAppend(cpiVectHdl vect, cpiObjHdl obj);

int cpiVectorAppendUint8(cpiVectHdl vect, cpiUint8 v);
int cpiVectorAppendSint8(cpiVectHdl vect, cpiSint8 v);
int cpiVectorAppendUint16(cpiVectHdl vect, cpiUint16 v);
int cpiVectorAppendSint16(cpiVectHdl vect, cpiSint16 v);
int cpiVectorAppendUint32(cpiVectHdl vect, cpiUint32 v);
int cpiVectorAppendSint32(cpiVectHdl vect, cpiSint32 v);
int cpiVectorAppendUint64(cpiVectHdl vect, cpiUint64 v);
int cpiVectorAppendSint64(cpiVectHdl vect, cpiSint64 v);
int cpiVectorAppendReal32(cpiVectHdl vect, cpiReal32 v);
int cpiVectorAppendReal64(cpiVectHdl vect, cpiReal64 v);
int cpiVectorAppendString(cpiVectHdl vect, const char* v);
int cpiVectorAppendBool(cpiVectHdl vect, cpiBool v);
int cpiVectorAppendChar16(cpiVectHdl vect, cpiChar16 v);

int cpiVectorSetUint8(cpiVectHdl vect, cpiUint8 v, int ndx);
int cpiVectorSetSint8(cpiVectHdl vect, cpiSint8 v, int ndx);
int cpiVectorSetUint16(cpiVectHdl vect, cpiUint16 v, int ndx);
int cpiVectorSetSint16(cpiVectHdl vect, cpiSint16 v, int ndx);
int cpiVectorSetUint32(cpiVectHdl vect, cpiUint32 v, int ndx);
int cpiVectorSetSint32(cpiVectHdl vect, cpiSint32 v, int ndx);
int cpiVectorSetUint64(cpiVectHdl vect, cpiUint64 v, int ndx);
int cpiVectorSetSint64(cpiVectHdl vect, cpiSint64 v, int ndx);
int cpiVectorSetReal32(cpiVectHdl vect, cpiReal32 v, int ndx);
int cpiVectorSetReal64(cpiVectHdl vect, cpiReal64 v, int ndx);
int cpiVectorSetString(cpiVectHdl vect, const char* v, int ndx);
int cpiVectorSetBool(cpiVectHdl vect, cpiBool v, int ndx);
int cpiVectorSetChar16(cpiVectHdl vect, cpiChar16 v, int ndx);

int cpiVectorGetUint8(cpiVectHdl vect, cpiUint8* v, int ndx);
int cpiVectorGetSint8(cpiVectHdl vect, cpiSint8* v, int ndx);
int cpiVectorGetUint16(cpiVectHdl vect, cpiUint16* v, int ndx);
int cpiVectorGetSint16(cpiVectHdl vect, cpiSint16* v, int ndx);
int cpiVectorGetUint32(cpiVectHdl vect, cpiUint32* v, int ndx);
int cpiVectorGetSint32(cpiVectHdl vect, cpiSint32* v, int ndx);
int cpiVectorGetUint64(cpiVectHdl vect, cpiUint64* v, int ndx);
int cpiVectorGetSint64(cpiVectHdl vect, cpiSint64* v, int ndx);
int cpiVectorGetReal32(cpiVectHdl vect, cpiReal32* v, int ndx);
int cpiVectorGetReal64(cpiVectHdl vect, cpiReal64* v, int ndx);
const char* cpiVectorGetString(cpiVectHdl vect, int ndx, int* resCode);
int cpiVectorGetBool(cpiVectHdl vect, cpiBool* v, int ndx);
int cpiVectorGetChar16(cpiVectHdl vect, cpiChar16* v, int ndx);

/**
 * Remove an item from a vector. The cpiVectHdl must have been previously
 * initialized with a call to cpiVectorAlloc. This operation does not free any
 * resources associated with the item that is removed.
 * @param vect A handle to the vector perform this operation on.
 * @param ndx The zero relative index of the item to remove.
 * @return 0 on success. Otherwise an error code.
 */
int cpiVectorRemove(cpiVectHdl vect, size_t ndx);

/**
 * Get an item from a vector at the given zero relative index.
 * @param vect A handle to the vector perform this operation on.
 * @param pObj A pointer to the handle that will contain the item's handle that
 *		is returned.
 * @param ndx The zero relative index of the item to retrieve.
 * @return 0 on success. Otherwise an error code.
 */
int cpiVectorGet(cpiVectHdl vect, cpiObjHdl* pObj, size_t ndx);

/**
 * Overwrite a previously added item within the vector at a given index.
 * @param vect A handle to the vector perform this operation on.
 * @param obj The handle of the object that will be place at the specified
 *		index.
 * @param ndx The zero relative index of the item to overwrite with the new
 *		object. No resources for the old object will be released when it is 
 *		overwritten.
 * @return 0 on success. Otherwise an error code.
 */
int cpiVectorSet(cpiVectHdl vect, cpiObjHdl obj, size_t ndx);

/**
 * Query the number of items a vector contains.
 * @param vect A handle to the vector perform this operation on.
 * @return A value that is Greater than or equal to zero on success. Otherwise
 * 		an error code.
 */
int cpiVectSize(cpiVectHdl vect);

/**
 * Clear all of the items in a given vector. This will not free any resources
 * associated with the objects that are removed from the vector. They must be
 * deallocated with the appropriate cpiXXXFree call.
 * @param vect A handle to a vector to remove all items for.
 * @return 0 on success. Otherwise an error code.
 */
int cpiVectClear(cpiVectHdl vect);

/*-----------------------------------------------------------------------------
	Class related APIs
-----------------------------------------------------------------------------*/
int cpiClsAlloc(cpiClsHdl* pCls);
int cpiClsFree(cpiClsHdl cls);
int cpiClsNewInst(cpiClsHdl cls, cpiInstHdl* instance);
const char* cpiClsGetName(cpiClsHdl cls, int* result);
int cpiClsSetName(cpiClsHdl cls, const char* name);
const char* cpiClsToString(cpiClsHdl cls, int* result);
const char* cpiClsGetSuperCls(cpiClsHdl cls, int* result);
int cpiClsSetSuperCls(cpiClsHdl cls, const char* superClassName);
int cpiClsGetQual(cpiClsHdl cls, const char* qualName, cpiQualHdl* pQualifier);
int cpiClsGetQuals(cpiClsHdl cls, cpiVectHdl* qualifiers);
int cpiClsAddQual(cpiClsHdl cls, cpiQualHdl qualifier);
int cpiClsSetQual(cpiClsHdl cls, cpiQualHdl qualifier);
int cpiClsSetQuals(cpiClsHdl cls, cpiVectHdl qualifiers);
int cpiClsRemoveQual(cpiClsHdl cls, const char* qualifierName);
int cpiClsGetProps(cpiClsHdl cls, cpiVectHdl* properties);
int cpiClsGetProp(cpiClsHdl cls, const char* propName, cpiPropHdl* pProperty);
int cpiClsAddProp(cpiClsHdl cls, cpiPropHdl property);
int cpiClsSetProp(cpiClsHdl cls, cpiPropHdl property);
int cpiClsSetProps(cpiClsHdl cls, cpiVectHdl properties);
int cpiClsRemoveProp(cpiClsHdl cls, const char* propertyName);
int cpiClsGetMeth(cpiClsHdl cls, const char* methName, cpiMethHdl* pMethod);
int cpiClsGetMeths(cpiClsHdl cls, cpiVectHdl* methods);
int cpiClsAddMeth(cpiClsHdl cls, cpiMethHdl method);
int cpiClsSetMeth(cpiClsHdl cls, cpiMethHdl method);
int cpiClsSetMeths(cpiClsHdl cls, cpiVectHdl methods);

/*-----------------------------------------------------------------------------
	Instance related APIs
-----------------------------------------------------------------------------*/
int cpiInstAlloc(cpiInstHdl* pInst);
int cpiInstFree(cpiInstHdl inst);
const char* cpiInstGetClsName(cpiInstHdl inst, int* result);
int cpiInstSetClsName(cpiInstHdl inst, const char* name);
const char* cpiInstToString(cpiInstHdl inst, int* result);
int cpiInstGetKeys(cpiInstHdl inst, cpiVectHdl* keyProperties);
const char* cpiInstGetAlias(cpiInstHdl inst, int* result);
int cpiInstSetAlias(cpiInstHdl inst, const char* aliasName);

int cpiInstGetQual(cpiInstHdl inst, const char* qualName, cpiQualHdl* pQual);
int cpiInstGetQuals(cpiInstHdl inst, cpiVectHdl* qualifiers);
int cpiInstSetQual(cpiInstHdl inst, cpiQualHdl qualifier);
int cpiInstSetQuals(cpiInstHdl inst, cpiVectHdl qualifiers);
int cpiInstRemoveQual(cpiInstHdl inst, const char* qualifierName);

int cpiInstGetProps(cpiInstHdl inst, cpiVectHdl* properties);
int cpiInstGetProp(cpiInstHdl inst, const char* propName, cpiPropHdl* pProp);
int cpiInstAddProp(cpiInstHdl inst, cpiPropHdl property);
int cpiInstSetProp(cpiInstHdl inst, cpiPropHdl property);
int cpiInstSetProps(cpiInstHdl inst, cpiVectHdl properties);
int cpiInstRemoveProp(cpiInstHdl inst, const char* propertyName);

/*-----------------------------------------------------------------------------
	Method related APIs
-----------------------------------------------------------------------------*/
int cpiMethAlloc(cpiMethHdl* pMeth);
int cpiMethFree(cpiMethHdl meth);
const char* cpiMethGetName(cpiMethHdl meth, int* result);
int cpiMethSetName(cpiMethHdl meth, const char* name);
const char* cpiMethToString(cpiMethHdl meth, int* result);
int cpiMethGetQual(cpiMethHdl meth, const char* qualName, cpiQualHdl* pQual);
int cpiMethGetQuals(cpiMethHdl meth, cpiVectHdl* qualifiers);
int cpiMethAddQual(cpiMethHdl meth, cpiQualHdl qualifier);
int cpiMethSetQuals(cpiMethHdl meth, cpiVectHdl qualifiers);
const char* cpiMethGetOriginCls(cpiMethHdl meth, int *result);
int cpiMethSetOriginCls(cpiMethHdl meth, const char* originClassName);
int cpiMethAddParm(cpiMethHdl meth, cpiParmHdl parm);
int cpiMethGetParms(cpiMethHdl meth, cpiVectHdl* parms);
int cpiMethSetReturnType(cpiMethHdl meth, int cimDataType);
int cpiMethGetReturnType(cpiMethHdl meth);
int cpiMethSetOverridingMeth(cpiMethHdl meth, const char* orMethName);
const char* cpiMethGetOverridingMeth(cpiMethHdl meth, int *result);
int cpiMethSetPropagated(cpiMethHdl meth, cpiBool isPropagated);
int cpiMethGetPropagated(cpiMethHdl meth, cpiBool* isPropagated);

/*-----------------------------------------------------------------------------
	Namespace related APIs
-----------------------------------------------------------------------------*/
int cpiNSAlloc(cpiNSHdl* pns);
int cpiNSFree(cpiNSHdl ns);
const char* cpiNSToString(cpiNSHdl ns, int* result);
const char* cpiNSGetNameSpacePart(cpiNSHdl ns, int* result);
int cpiNSSetNameSpacePart(cpiNSHdl ns, const char* nsPart);
const char* cpiNSGetHostPart(cpiNSHdl ns, int* result);
int cpiNSSetHostPart(cpiNSHdl ns, const char* hostPart);
const char* cpiNSGetProtocolPart(cpiNSHdl ns, int* result);
int cpiNSSetProtocolPart(cpiNSHdl ns, const char* protocolPart);
int cpiNSGetPort(cpiNSHdl ns);
const char* cpiNSGetFileNamePart(cpiNSHdl ns, int* result);

/*-----------------------------------------------------------------------------
	Reference (Object Path)  related APIs
-----------------------------------------------------------------------------*/
int cpiRefAlloc(cpiRefHdl* pref);
int cpiRefFree(cpiRefHdl ref);
const char* cpiRefToString(cpiRefHdl ref, int* result);
const char* cpiRefEscape(cpiRefHdl ref, int* result);
const char* cpiRefUnEscape(cpiRefHdl ref, int* result);
int cpiRefSetFromStr(cpiRefHdl ref, const char* refStr);
int cpiRefSetNS(cpiRefHdl ref, const char* nsStr);
int cpiRefAddKeyProp(cpiRefHdl ref, const char* propertyName, cpiValHdl propValue);
int cpiRefGetKeys(cpiRefHdl ref, cpiVectHdl* keyProps);
int cpiRefSetKeys(cpiRefHdl ref, cpiVectHdl newKeyProps);
const char* cpiRefGetNS(cpiRefHdl ref, int* result);
int cpiRefSetNS(cpiRefHdl ref, const char* nsStr);
const char* cpiRefGetHost(cpiRefHdl ref, int* result);
int cpiRefSetHost(cpiRefHdl ref, const char* hostStr);
const char* cpiRefGetClsName(cpiRefHdl ref, int* result);
int cpiRefSetClsName(cpiRefHdl ref, const char* clsNamaeStr);
int cpiRefIsEqual(cpiRefHdl ref1, cpiRefHdl ref2, cpiBool* result);
const char* cpiRefGetModelPath(cpiRefHdl ref, int* result);

/*-----------------------------------------------------------------------------
	Parameter related APIs
-----------------------------------------------------------------------------*/
int cpiParmAlloc(cpiParmHdl* pparm);
int cpiParmFree(cpiParmHdl parm);
const char* cpiParmGetName(cpiParmHdl parm);
int cpiParmSetName(cpiParmHdl parm, const char* name);
const char* cpiParmToString(cpiParmHdl parm);
int cpiParmGetQual(cpiParmHdl parm, const char* qualName, cpiQualHdl* pQual);
int cpiParmGetQuals(cpiParmHdl parm, cpiVectHdl* qualifiers);
int cpiParmAddQual(cpiParmHdl parm, cpiQualHdl qualifier);
int cpiParmSetQual(cpiParmHdl parm, cpiQualHdl qualifier);
int cpiParmSetQuals(cpiParmHdl parm, cpiVectHdl qualifiers);
int cpiParmRemoveQual(cpiParmHdl parm, const char* qualifierName);
int cpiParmSetDataType(cpiParmHdl parm, int dataType);
int cpiParmGetDataType(cpiParmHdl parm);
int cpiParmGetDataSize(cpiParmHdl parm);

/*-----------------------------------------------------------------------------
	Property related APIs
-----------------------------------------------------------------------------*/
int cpiPropAlloc(cpiPropHdl* pprop);
int cpiPropFree(cpiPropHdl prop);
const char* cpiPropGetName(cpiPropHdl prop);
int cpiPropSetName(cpiPropHdl prop, const char* name);
const char* cpiPropToString(cpiPropHdl prop);
int cpiPropGetQual(cpiPropHdl prop, const char* qualName, cpiQualHdl* pQual);
int cpiPropGetQuals(cpiPropHdl prop, cpiVectHdl* qualifiers);
int cpiPropAddQual(cpiPropHdl prop, cpiQualHdl qualifier);
int cpiPropSetQual(cpiPropHdl prop, cpiQualHdl qualifier);
int cpiPropSetQuals(cpiPropHdl prop, cpiVectHdl qualifiers);
int cpiPropRemoveQual(cpiPropHdl prop, const char* qualifierName);
const char* cpiPropGetOriginCls(cpiPropHdl prop);
int cpiPropSetOriginCls(cpiPropHdl prop, const char* originCls);
int cpiPropSetValue(cpiPropHdl prop, cpiValHdl value);
int cpiPropGetValue(cpiPropHdl prop, cpiValHdl* pvalue);
int cpiPropSetDataType(cpiPropHdl prop, int dataType);
int cpiPropGetDataType(cpiPropHdl prop);
int cpiPropGetDataSize(cpiPropHdl prop);
int cpiPropSetDataSize(cpiPropHdl prop, int dataSize);
int cpiPropSetOverridingProp(cpiPropHdl prop, const char* opropName);
const char* cpiPropGetOverridingProp(cpiPropHdl prop);
int cpiPropSetPropagated(cpiPropHdl prop, cpiBool isPropagated);
int cpiPropGetPropagated(cpiPropHdl prop, cpiBool* isPropagated);
int cpiPropClearQuals(cpiPropHdl prop);

/*-----------------------------------------------------------------------------
	Qualifier related APIs
-----------------------------------------------------------------------------*/
int cpiQualAlloc(cpiQualHdl* pqual);
int cpiQualFree(cpiQualHdl qual);
const char* cpiQualGetName(cpiQualHdl qual);
int cpiQualSetName(cpiQualHdl qual, const char* name);
const char* cpiQualToString(cpiQualHdl qual);
int cpiQualGetValue(cpiQualHdl qual, cpiValHdl* pvalue);
int cpiQualSetValue(cpiQualHdl qual, cpiValHdl value);
int cpiQualSetDefaults(cpiQualHdl qual, cpiQualTHdl qaulType);
int cpiQualGetDefaults(cpiQualHdl qual, cpiQualTHdl* pqaulType);
int cpiQualAddFlavor(cpiQualHdl qual, int flavor);
int cpiQualRemoveFlavor(cpiQualHdl qual, int flavor);
int cpiQualGetFlavors(cpiQualHdl qual, cpiVectHdl* flavors);
int cpiQualEquals(cpiQualHdl qual1, cpiQualHdl qual2, cpiBool* isEqual);
int cpiQualSetPropagated(cpiQualHdl qual1, cpiBool isPropagated);
int cpiQualGetPropagated(cpiQualHdl qual1, cpiBool* pIsPropagated);


/*-----------------------------------------------------------------------------
	Qualifier Type related APIs
-----------------------------------------------------------------------------*/
int cpiQualTAlloc(cpiQualTHdl* pqualt);
int cpiQualTFree(cpiQualTHdl qualt);
const char* cpiQualTGetName(cpiQualTHdl qualt);
int cpiQualTSetName(cpiQualTHdl qualt, const char* name);
const char* cpiQualTToString(cpiQualTHdl qualt);
int cpiQualTGetScope(cpiQualTHdl qualt);
int cpiQualTGetDataType(cpiQualTHdl qualt);
int cpiQualTSetDataType(cpiQualTHdl qualt, int dataType);
int cpiQualTGetDataSize(cpiQualTHdl qualt);
int cpiQualTGetDefaltVal(cpiQualTHdl qualt, cpiValHdl* pval);
int cpiQualTSetDefaltVal(cpiQualTHdl qualt, cpiValHdl val);
int cpiQualTAddScope(cpiQualTHdl qualt, int scope);
int cpiQualTHasScope(cpiQualTHdl qualt, int scope);
int cpiQualTHasFlavor(cpiQualTHdl qualt, int flavor);
int cpiQualTAddFlavor(cpiQualTHdl qualt, int flavor);
int cpiQualTRemoveFlavor(cpiQualTHdl qualt, int flavor);
int cpiQualTGetFlavors(cpiQualTHdl qualt, cpiVectHdl* flavors);

/*-----------------------------------------------------------------------------
	Date Time related APIs
-----------------------------------------------------------------------------*/
int cpiDTAlloc(cpiDTHdl* pval);
int cpiDTFree(cpiDTHdl val);
const char* cpiDTToString(cpiDTHdl val);
int cpiDTGet(cpiDateTimeVals* vals);
int cpiDTSet(cpiDateTimeVals vals);

/*-----------------------------------------------------------------------------
	Value related APIs
-----------------------------------------------------------------------------*/
int cpiValAlloc(cpiValHdl* pval);
int cpiValFree(cpiValHdl val);
const char* cpiValToString(cpiValHdl val);
int cpiValGetType(cpiValHdl val);
int cpiValIsArray(cpiValHdl val, cpiBool* pIsArray);
int cpiValSetUint8(cpiValHdl val, cpiUint8 v);
int cpiValSetSint8(cpiValHdl val, cpiSint8 v);
int cpiValSetUint16(cpiValHdl val, cpiUint16 v);
int cpiValSetSint16(cpiValHdl val, cpiSint16 v);
int cpiValSetUint32(cpiValHdl val, cpiUint32 v);
int cpiValSetSint32(cpiValHdl val, cpiSint32 v);
int cpiValSetUint64(cpiValHdl val, cpiUint64 v);
int cpiValSetSint64(cpiValHdl val, cpiSint64 v);
int cpiValSetString(cpiValHdl val, const char* v);
int cpiValSetBool(cpiValHdl val, cpiBool v);
int cpiValSetReal32(cpiValHdl val, cpiReal32 v);
int cpiValSetReal64(cpiValHdl val, cpiReal64 v);
int cpiValSetDateTime(cpiValHdl val, cpiDTHdl v);
int cpiValSetChar16(cpiValHdl val, cpiChar16 v);
int cpiValSetRef(cpiValHdl val, cpiRefHdl v);

int cpiValSetUint8Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetSint8Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetUint16Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetSint16Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetUint32Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetSint32Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetUint64Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetSint64Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetStringArray(cpiValHdl val, cpiVectHdl ra);
int cpiValSetBoolArray(cpiValHdl val, cpiVectHdl ra);
int cpiValSetReal32Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetReal64Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetChar16Array(cpiValHdl val, cpiVectHdl ra);
int cpiValSetDateTimeArray(cpiValHdl val, cpiVectHdl ra);
int cpiValSetRefArray(cpiValHdl val, cpiVectHdl ra);

int cpiValGetUint8(cpiValHdl val, cpiUint8* pv);
int cpiValGetSint8(cpiValHdl val, cpiSint8* pv);
int cpiValGetUint16(cpiValHdl val, cpiUint16* pv);
int cpiValGetSint16(cpiValHdl val, cpiSint16* pv);
int cpiValGetUint32(cpiValHdl val, cpiUint32* pv);
int cpiValGetSint32(cpiValHdl val, cpiSint32* pv);
int cpiValGetUint64(cpiValHdl val, cpiUint64* pv);
int cpiValGetSint64(cpiValHdl val, cpiSint64* pv);
const char* cpiValGetString(cpiValHdl val);
int cpiValGetBool(cpiValHdl val, cpiBool* pv);
int cpiValGetReal32(cpiValHdl val, cpiReal32* pv);
int cpiValGetReal64(cpiValHdl val, cpiReal64* pv);
int cpiValGetDateTime(cpiValHdl val, cpiDTHdl* pv);
int cpiValGetChar16(cpiValHdl val, cpiChar16* pv);
int cpiValGetRef(cpiValHdl val, cpiRefHdl* pv);

int cpiValGetUint8Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetSint8Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetUint16Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetSint16Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetUint32Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetSint32Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetUint64Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetSint64Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetStringArray(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetBoolArray(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetReal32Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetReal64Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetChar16Array(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetDateTimeArray(cpiValHdl val, cpiVectHdl* ra);
int cpiValGetRefArray(cpiValHdl val, cpiVectHdl* ra);

// Error codes
#define CPI_SUCCESS 0
#define CPI_E_INVALID_OBJHANDLE -1
#define CPI_E_INVALID_VECTHANDLE -2
#define CPI_E_INCOMPAT_OBJHANDLE -3
#define CPI_E_BAD_VECTOR_CONTENTS -4
#define CPI_E_INVALID_DATATYPE -5
#define CPI_E_NULL_VECTOR -6
#define CPI_E_OUTOFBOUNDS -7
#define CPI_E_BAD_INDEX -8
#define CPI_E_INVALID_PARM -9
#define CPI_E_NOTFOUND -10
#define CPI_E_NOQUALS -11
#define CPI_E_INVALID_QUALHANDLE -12
#define CPI_E_INVALID_QUALVECTOR -13
#define CPI_E_INVALID_PROPHANDLE -14
#define CPI_E_INVALID_PROPVECTOR -15
#define CPI_E_INVALID_METHVECTOR -16
#define CPI_E_INVALID_METHHANDLE -17
#define CPI_E_INVALID_PARMHANDLE -18
#define CPI_E_INVALID_PARMVECTOR -19


#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif
