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

/** CIM Value handle */
typedef void* cpiValHdl;

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
int cpiVectorAppend(cpiVectHdl vect, cpiObjHdl obj)

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
const char* cpiClsGetName(cpiClsHdl cls);
int cpiClsSetName(cpiClsHdl cls, const char* name);
const char* cpiClsToString(cpiClsHdl cls);
const char* cpiClsGetSuperCls(cpiClsHdl cls);
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
const char* cpiInstGetClsName(cpiInstHdl inst);
int cpiInstSetClsName(cpiInstHdl inst, const char* name);
const char* cpiInstToString(cpiInstHdl inst);
int cpiInstGetKeys(cpiInstHdl inst, cpiVectHdl* keyProperties);
const char* cpiInstGetAlias(cpiInstHdl inst);
int cpiInstSetAlias(cpiInstHdl inst, const char* aliasName);

int cpiInstGetQual(cpiInstHdl inst, const char* qualName, cpiQualHdl* pQual);
int cpiInstGetQuals(cpiInstHdl inst, cpiVectHdl* qualifiers);
int cpiInstAddQual(cpiInstHdl inst, cpiQualHdl qualifier);
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
const char* cpiMethGetName(cpiMethHdl meth);
int cpiMethSetName(cpiMethHdl meth, const char* name);
const char* cpiMethToString(cpiMethHdl meth);
const char* cpiMethGetAlias(cpiMethHdl meth);
int cpiMethSetAlias(cpiMethHdl meth, const char* aliasName);

int cpiMethGetQual(cpiMethHdl meth, const char* qualName, cpiQualHdl* pQual);
int cpiMethGetQuals(cpiMethHdl meth, cpiVectHdl* qualifiers);
int cpiMethAddQual(cpiMethHdl meth, cpiQualHdl qualifier);
int cpiMethSetQual(cpiMethHdl meth, cpiQualHdl qualifier);
int cpiMethSetQuals(cpiMethHdl meth, cpiVectHdl qualifiers);
int cpiMethRemoveQual(cpiMethHdl meth, const char* qualifierName);

const char* cpiMethGetOriginCls(cpiMethHdl meth);
int cpiMethSetOriginCls(cpiMethHdl meth, const char* originClassName);
int cpiMethAddParm(cpiMethHdl meth, cpiParmHdl parm);
int cpiMethSetParm(cpiMethHdl meth, cpiParmHdl parm);
int cpiMethGetParms(cpiMethHdl meth, cpiVectHdl* parms);
int cpiMethSetReturnType(cpiMethHdl meth, int cimDataType);
int cpiMethGetReturnType(cpiMethHdl meth);
int cpiMethSetOverridingMeth(cpiMethHdl meth, const char* orMethName);
const char* cpiMethGetOverridingMeth(cpiMethHdl meth);
int cpiMethSetPropagated(cpiMethHdl meth, cpiBool isPropagated);
cpiBool cpiMethGetPropagated(cpiMethHdl meth);

/*-----------------------------------------------------------------------------
	Namespace related APIs
-----------------------------------------------------------------------------*/
int cpiNSAlloc(cpiNSHdl* pns);
int cpiNSFree(cpiNSHdl ns);
const char* cpiNSToString(cpiNSHdl ns);
const char* cpiNSGetNameSpacePart(cpiNSHdl ns);
int cpiNSSetNameSpacePart(cpiNSHdl ns, const char* nsPart);
const char* cpiNSGetHostPart(cpiNSHdl ns);
int cpiNSSetHostPart(cpiNSHdl ns, const char* hostPart);
const char* cpiNSGetProtocolPart(cpiNSHdl ns);
int cpiNSSetProtocolPart(cpiNSHdl ns, const char* protocolPart);
int cpiNSGetPort(cpiNSHdl ns);
const char* cpiNSGetFileNamePart(cpiNSHdl ns);


#ifdef __cplusplus
}
#endif	/* __cplusplus */



#endif
