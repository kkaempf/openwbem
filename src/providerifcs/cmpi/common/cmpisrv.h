
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
 * Contributors:
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
#define RESULT_ObjectPath 2
#define RESULT_Value      4
#define RESULT_Method     8
#define RESULT_Indication 16
#define RESULT_Response   32
#define RESULT_set        128
#define RESULT_done       256


#define ENQ_BOT_LIST(i,f,l,n,p) { if (l) l->n=i; else f=i; \
                                  i->p=l; i->n=NULL; l=i;}
#define ENQ_TOP_LIST(i,f,l,n,p) { if (f) f->p=i; else l=i; \
                                   i->p=NULL; i->n=f; f=i;}
#define DEQ_FROM_LIST(i,f,l,n,p) \
                    { if (i->n) i->n->p=i->p; else l=i->p; \
                      if (i->p) i->p->n=i->n; else f=i->n;}


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
extern CMPIResultFT *CMPI_ResultMeth_Ftab;
extern CMPIResultFT *CMPI_ResultResponseOnStack_Ftab;

extern CMPIDateTimeFT *CMPI_DateTime_Ftab;
extern CMPIArrayFT *CMPI_Array_Ftab;
extern CMPIStringFT *CMPI_String_Ftab;

extern CMPIBrokerFT *CMPI_Broker_Ftab;
extern CMPIBrokerEncFT *CMPI_BrokerEnc_Ftab;

struct CMPI_Object {
   void *hdl;
   void *ftab;
   CMPI_Object *next,*prev;
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


struct CMPI_Result : CMPIResult {
   CMPI_Object *next,*prev;
   long flags;
};

struct CMPI_ResultOnStack : CMPIResult {
   CMPI_Object *next,*prev;
   long flags;
   CMPI_ResultOnStack(const OW_CIMObjectPathResultHandlerIFC & handler);
   CMPI_ResultOnStack(const OW_CIMInstanceResultHandlerIFC& handler);
   CMPI_ResultOnStack(const CMPIObjectPathValueResultHandler & handler);
#if 0
   CMPI_ResultOnStack(const MethodResultResponseHandler& handler);
   CMPI_ResultOnStack(const ResponseHandler& handler);
#endif
   CMPI_ResultOnStack();
   ~CMPI_ResultOnStack();
};

struct CMPI_InstanceOnStack : CMPIInstance {
   CMPI_Object *next,*prev;
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

#include "cmpiThreadContext.h"

CMPI_String* string2CMPIString(const OW_String &s);
CMPIType type2CMPIType(OW_CIMDataType pt, int array);
OW_CIMValue value2CIMValue(CMPIValue* data, CMPIType type, CMPIrc *rc);
CMPIrc key2CMPIData(const OW_String& v, OW_CIMDataType t, CMPIData *data);
//CMPIrc key2CMPIData(const OW_String& v, KeyBinding::Type t, CMPIData *data);
CMPIrc value2CMPIData(const OW_CIMValue&,CMPIType,CMPIData *data);

#endif

