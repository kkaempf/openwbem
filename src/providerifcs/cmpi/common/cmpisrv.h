
/*
 *
 * cmpisrv.cpp
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
 * Author:        Adrian Schuur <schuur@de.ibm.com>
 *
 * Contributor:   Markus Mueller <sedgewick_de@yahoo.de>
 *
 * Description: CMPI internal services header
 *
 */


#ifndef _CMPISRV_H_
#define _CMPISRV_H_
#include "OW_config.h"

#include <stdlib.h>
#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"

#include "OW_config.h"
#include "OW_Cache.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMValue.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"

// BMMU
typedef struct {
  void * oc;
  OW_ProviderEnvironmentIFCRef cimom;
} OperationContext;
//OperationContext::OperationContext(void * Oc, void * Cimom) :
//	 oc = OC, cimom = Cimom {}


#define RESULT_Instance   1

#define RESULT_Object     2
#define RESULT_ObjectPath 4
#define RESULT_Value      8
#define RESULT_Method     16
#define RESULT_Indication 32
#define RESULT_Response   64
#define RESULT_set        128
#define RESULT_done       256


#define ENQ_BOT_LIST(i,f,l,n,p) { if (l) l->n=i; else f=i; \
                                  i->p=l; i->n=NULL; l=i;}
#define ENQ_TOP_LIST(i,f,l,n,p) { if (f) f->p=i; else l=i; \
                                   i->p=NULL; i->n=f; f=i;}
#define DEQ_FROM_LIST(i,f,l,n,p) \
                    { if (i->n) i->n->p=i->p; else l=i->p; \
                      if (i->p) i->p->n=i->n; else f=i->n;}

#define CMPI_MIType_Instance    1
#define CMPI_MIType_Association 2
#define CMPI_MIType_Method      4
#define CMPI_MIType_Property    8
#define CMPI_MIType_Indication 16

typedef CMPIInstanceMI*        (*CREATE_INST_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIAssociationMI*     (*CREATE_ASSOC_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIMethodMI*          (*CREATE_METH_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIPropertyMI*        (*CREATE_PROP_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIIndicationMI*      (*CREATE_IND_MI)(CMPIBroker*,CMPIContext*);

typedef CMPIInstanceMI*    (*CREATE_GEN_INST_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIAssociationMI* (*CREATE_GEN_ASSOC_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIMethodMI*      (*CREATE_GEN_METH_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIPropertyMI*    (*CREATE_GEN_PROP_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIIndicationMI*  (*CREATE_GEN_IND_MI)(CMPIBroker*,CMPIContext*,const char*);

typedef struct {
   int                 miTypes;
   int                 genericMode;
   CMPIInstanceMI      *instMI;
   CMPIAssociationMI   *assocMI;
   CMPIMethodMI        *methMI;
   CMPIPropertyMI      *propMI;
   CMPIIndicationMI    *indMI;
   CREATE_INST_MI      createInstMI;
   CREATE_ASSOC_MI     createAssocMI;
   CREATE_METH_MI      createMethMI;
   CREATE_PROP_MI      createPropMI;
   CREATE_IND_MI       createIndMI;
   CREATE_GEN_INST_MI  createGenInstMI;
   CREATE_GEN_ASSOC_MI         createGenAssocMI;
   CREATE_GEN_METH_MI  createGenMethMI;
   CREATE_GEN_PROP_MI  createGenPropMI;
   CREATE_GEN_IND_MI   createGenIndMI;
} MIs;


typedef struct _CMPIResultRefFT : public CMPIResultFT {
} CMPIResultRefFT;
typedef struct _CMPIResultInstFT : public CMPIResultFT {
} CMPIResultInstFT;
typedef struct _CMPIResultDataFT : public CMPIResultFT {
} CMPIResultDataFT;
typedef struct _CMPIResultMethFT : public CMPIResultFT {
} CMPIResultMethFT;

extern CMPIInstanceFT *CMPI_Instance_Ftab;
extern CMPIInstanceFT *CMPI_InstanceOnStack_Ftab;

extern CMPIObjectPathFT *CMPI_ObjectPath_Ftab;
extern CMPIObjectPathFT *CMPI_ObjectPathOnStack_Ftab;

extern CMPIArgsFT *CMPI_Args_Ftab;
extern CMPIArgsFT *CMPI_ArgsOnStack_Ftab;

extern CMPIContextFT *CMPI_Context_Ftab;
extern CMPIContextFT *CMPI_ContextOnStack_Ftab;

extern CMPIResultFT *CMPI_ResultRefOnStack_Ftab;
extern CMPIResultFT *CMPI_ResultInstOnStack_Ftab;
extern CMPIResultFT *CMPI_ResultData_Ftab;
extern CMPIResultFT *CMPI_ResultMethOnStack_Ftab;
extern CMPIResultFT *CMPI_ResultResponseOnStack_Ftab;

extern CMPIDateTimeFT *CMPI_DateTime_Ftab;
extern CMPIArrayFT *CMPI_Array_Ftab;
extern CMPIStringFT *CMPI_String_Ftab;

extern CMPIBrokerFT *CMPI_Broker_Ftab;
extern CMPIBrokerEncFT *CMPI_BrokerEnc_Ftab;

extern CMPIEnumerationFT *CMPI_ObjEnumeration_Ftab;
extern CMPIEnumerationFT *CMPI_InstEnumeration_Ftab;
extern CMPIEnumerationFT *CMPI_OpEnumeration_Ftab;

struct CMPI_Broker;

struct CMPI_Object {
   void *hdl;
   void *ftab;
   CMPI_Object *next,*prev;
   void *priv;    // CMPI type specific usage
   CMPI_Object(OW_CIMInstance*);
   CMPI_Object(OW_CIMObjectPath*);
   CMPI_Object(OW_CIMDateTime*);
   CMPI_Object(const OW_String&);
   CMPI_Object(char*);
   CMPI_Object(void*,void*);
   CMPI_Object(OW_Array<OW_CIMParamValue>*);
   CMPI_Object(CMPIData*);
   CMPI_Object(CMPI_Object*);
   ~CMPI_Object();
   void unlinkAndDelete();
};

struct CMPI_Context : CMPIContext {
   CMPI_Object *next,*prev;
   OperationContext* ctx;
   CMPI_Context(const OperationContext& ct);
};

struct CMPI_ContextOnStack : CMPIContext {
   CMPI_Object *next,*prev;
   OperationContext* ctx;
   CMPI_ContextOnStack(const OperationContext& ct);
   ~CMPI_ContextOnStack();
};

// BMMU
template<typename T>
class CMPISingleValueResultHandler : public OW_ResultHandlerIFC<T>
{
public:
	CMPISingleValueResultHandler()
		: _t(OW_CIMNULL)
	{
	}

	T& getValue()
	{
		return _t;
	}
protected:
        virtual void doHandle(const T& t)
        {
                _t = t;
        }
private:
        T _t;
};

typedef CMPISingleValueResultHandler<OW_CIMClass> CMPIClassValueResultHandler;
typedef CMPISingleValueResultHandler<OW_CIMObjectPath> CMPIObjectPathValueResultHandler;
typedef CMPISingleValueResultHandler<OW_CIMInstance> CMPIInstanceValueResultHandler;
typedef CMPISingleValueResultHandler<OW_CIMValue> CMPIValueValueResultHandler;


struct CMPI_Result : CMPIResult {
   CMPI_Object *next,*prev;
   long flags;
   CMPI_Broker *xBroker;
};

struct CMPI_ResultOnStack : CMPIResult {
   CMPI_Object *next,*prev;
   long flags;
   CMPI_Broker *xBroker;
   CMPI_ResultOnStack(const OW_CIMObjectPathResultHandlerIFC & handler);
   CMPI_ResultOnStack(const OW_CIMInstanceResultHandlerIFC& handler);
   CMPI_ResultOnStack(const CMPIObjectPathValueResultHandler & handler);
   CMPI_ResultOnStack(const CMPIValueValueResultHandler & handler);
#if 0
   CMPI_ResultOnStack(const MethodResultResponseHandler& handler);
   CMPI_ResultOnStack(const ResponseHandler& handler);
#endif
   CMPI_ResultOnStack();
   ~CMPI_ResultOnStack();
};

struct CMPI_InstanceOnStack : CMPIInstance {
   CMPI_Object *next,*prev;
   void *priv;    // CMPI type specific usage
   CMPI_InstanceOnStack(const OW_CIMInstance& ci);
};


struct CMPI_ObjectPathOnStack : CMPIObjectPath {
   CMPI_Object *next,*prev;
   CMPI_ObjectPathOnStack(const OW_CIMObjectPath& cop);
};

struct CMPI_ArgsOnStack : CMPIArgs {
   CMPI_Object *next,*prev;
   CMPI_ArgsOnStack(const OW_Array<OW_CIMParamValue>& args);
};


struct CMPI_String : CMPIString {
   CMPI_Object *next,*prev;
};

struct CMPI_DateTime : CMPIDateTime {
   CMPI_Object *next,*prev;
};

struct CMPI_ObjEnumeration : CMPIEnumeration {
   CMPI_Object *next,*prev;
   int max,cursor;
   //CMPI_ObjEnumeration(OW_Array<OW_CIMBase>* ia);
   CMPI_ObjEnumeration(OW_Array<OW_CIMInstance>* ia);
};

struct CMPI_InstEnumeration : CMPIEnumeration {
   CMPI_Object *next,*prev;
   int max,cursor;
   CMPI_InstEnumeration(OW_Array<OW_CIMInstance>* ia);
};

struct CMPI_OpEnumeration : CMPIEnumeration {
   CMPI_Object *next,*prev;
   int max,cursor;
   CMPI_OpEnumeration(OW_Array<OW_CIMObjectPath>* opa);
};

//typedef HashTable<String, CIMClass *,
//      EqualFunc<String>,  HashFunc<String> > ClassCache;

typedef OW_Cache<OW_CIMClass> ClassCache;

struct CMPI_Broker : CMPIBroker {
   ClassCache * clsCache;
   static  CMPIBroker *staticBroker;
};


#include "cmpiThreadContext.h"

CMPI_String* string2CMPIString(const OW_String &s);
CMPIType type2CMPIType(OW_CIMDataType pt, int array);
OW_CIMValue value2CIMValue(CMPIValue* data, CMPIType type, CMPIrc *rc);
CMPIrc key2CMPIData(const OW_String& v, OW_CIMDataType t, CMPIData *data);
//CMPIrc key2CMPIData(const OW_String& v, KeyBinding::Type t, CMPIData *data);
CMPIrc value2CMPIData(const OW_CIMValue&,CMPIType,CMPIData *data);

OW_CIMClass *mbGetClass(CMPIBroker *mb, const OW_CIMObjectPath &cop);

#endif

