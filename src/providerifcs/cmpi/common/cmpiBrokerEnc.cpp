
/*
 *
 * cmpiBrokerEnc.cpp
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
 * Description: CMPIClass support
 *
 */

#include <iostream>
#include "cmpisrv.h"
//#include <Pegasus/Common/CIMName.h>
//#include <Pegasus/Common/CIMPropertyList.h>
//#include <Pegasus/Provider/CIMOMHandle.h>
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OW_LocalCIMOMHandle.hpp"

// Factory section

static CMPIInstance* mbEncNewInstance(CMPIBroker* mb, CMPIObjectPath* eCop, CMPIStatus *rc) {
   (void) mb;

   OW_CIMObjectPath * cop = static_cast<OW_CIMObjectPath *>(eCop->hdl);
   std::cout<<"--- mbEncNewInstance() of name " << cop->getObjectName() << std::endl;
   OW_CIMInstance * ci = new OW_CIMInstance(cop->getObjectName());
   ci->setClassName(cop->getObjectName());
   std::cout<<"--- mbEncNewInstance() with name " << ci->getClassName() << std::endl;

   CMPIInstance * neInst = (CMPIInstance *)new CMPI_Object(ci);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return neInst;
}

static CMPIObjectPath* mbEncNewObjectPath(CMPIBroker* mb, char *cls, char *ns,
                  CMPIStatus *rc) {
   (void) mb;

   //cout<<"--- mbEncNewObjectPath()"<<endl;
   OW_String className(cls);
   OW_String nameSpace(ns);
   OW_CIMObjectPath * cop =
	 new OW_CIMObjectPath(className,nameSpace);

   CMPIObjectPath * nePath = (CMPIObjectPath*)new CMPI_Object(cop);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return nePath;
}

static CMPIArgs* mbEncNewArgs(CMPIBroker* mb, CMPIStatus *rc) {
   (void) mb;

   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return (CMPIArgs*)new CMPI_Object(new OW_Array<OW_CIMParamValue>());
}

static CMPIString* mbEncNewString(CMPIBroker* mb, char *cStr, CMPIStatus *rc) {
   (void) mb;

   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return (CMPIString*)new CMPI_Object(cStr);
}

static CMPIArray* mbEncNewArray(CMPIBroker* mb, CMPICount count, CMPIType type,
                                CMPIStatus *rc) {
   (void) mb;

   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   CMPIData *dta=new CMPIData[count+1];
   dta->type=type;
   dta->value.uint32=count;
   for (uint i=1; i<=count; i++) {
      dta[i].type=type;
      dta[i].state=CMPI_nullValue;
      dta[i].value.uint64=0;
   }
   return (CMPIArray*)new CMPI_Object(dta);
}

extern CMPIDateTime *newDateTime();

static CMPIDateTime* mbEncNewDateTime(CMPIBroker* mb, CMPIStatus *rc) {
   (void) mb;

   //cout<<"--- mbEncNewDateTime()"<<endl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return newDateTime();
}

extern CMPIDateTime *newDateTime(CMPIUint64,CMPIBoolean);

static CMPIDateTime* mbEncNewDateTimeFromBinary(CMPIBroker* mb, CMPIUint64 time,
      CMPIBoolean interval ,CMPIStatus *rc) {
   (void) mb;

   //cout<<"--- mbEncNewDateTimeFromBinary()"<<endl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return newDateTime(time,interval);
}

extern CMPIDateTime *newDateTime(char*);

static CMPIDateTime* mbEncNewDateTimeFromString(CMPIBroker* mb, char *t ,CMPIStatus *rc) {
   (void) mb;

   //cout<<"--- mbEncNewDateTimeFromString()"<<endl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return newDateTime(t);
}

static CMPIString* mbEncToString(CMPIBroker * mb,void * o, CMPIStatus * rc) {
   (void) mb;

   CMPI_Object *obj=(CMPI_Object*)o;
   OW_String str;
   char msg[128];
   if (obj==NULL) {
      sprintf(msg,"** Null object ptr (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*)new CMPI_Object(msg);
   }

   if (obj->hdl==NULL) {
      sprintf(msg,"** Null object hdl (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*)new CMPI_Object(msg);
   }

   if (obj->ftab==(void*)CMPI_Instance_Ftab ||
       obj->ftab==(void*)CMPI_InstanceOnStack_Ftab) {
      sprintf(msg,"** Object not supported (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*) new CMPI_Object(msg);
      // str=((OW_CIMInstance*)obj->hdl)->toString();
   }
   else if (obj->ftab==(void*)CMPI_ObjectPath_Ftab ||
       obj->ftab==(void*)CMPI_ObjectPathOnStack_Ftab) {
       str=((OW_CIMObjectPath*)obj->hdl)->toString();
   }
   /* else if (obj->ftab==(void*)CMPI_SelectExp_Ftab ||
       obj->ftab==(void*)CMPI_SelectCondDoc_Ftab ||
       obj->ftab==(void*)CMPI_SelectCondCod_Ftab ||
       obj->ftab==(void*)CMPI_SubCond_Ftab ||
       obj->ftab==(void*)CMPI_Predicate_Ftab) {
      sprintf(msg,"** Object not supported (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*) new CMPI_Object(msg);
   } */
   else {
      sprintf(msg,"** Object not recognized (0x%x) **",(int)o);
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
      return (CMPIString*) new CMPI_Object(msg);
   }

   sprintf(msg,"0x%X: ",(int)o);
   return (CMPIString*) new CMPI_Object(OW_String(msg)+str);
}

static CMPIBoolean mbEncIsA(CMPIBroker *mb, void *obj, char *type, CMPIStatus *rc) {
   (void) mb;
   (void) obj;
   (void) type;
   (void) rc;
   return 0;
}

static CMPIString* mbEncGetType(CMPIBroker *mb, void *obj, CMPIStatus *rc) {
   (void) mb;
   (void) obj;
   (void) rc;
   return NULL;
}
// BMMU
// Cheating again - retrieve the LocalCIMOMHandle from the operation context;
OW_CIMOMHandleIFCRef CM_CIMOM(CMPIContext * ctx)
{
   CMPI_ContextOnStack * sCtx = (CMPI_ContextOnStack *)ctx;
   OW_ProviderEnvironmentIFCRef provenv = sCtx->ctx->cimom;
   OW_CIMOMHandleIFCRef lch = provenv->getCIMOMHandle();
   return lch;
}
//#define CM_CIMOM(mb) ((OW_LocalCIMOMHandle*)mb->hdl)

#define CM_Context(ctx) ((OperationContext*)ctx->hdl)
#define CM_ObjectPath(cop) ((OW_CIMObjectPath*)cop->hdl)
#define CM_LocalOnly(flgs) (OW_Bool)(((flgs) & CMPI_FLAG_LocalOnly)!=0)
#define CM_ClassOrigin(flgs) (OW_Bool)(((flgs) & CMPI_FLAG_IncludeClassOrigin)!=0)
#define CM_IncludeQualifiers(flgs) (OW_Bool)(((flgs) & CMPI_FLAG_IncludeQualifiers)!=0)

// Broker function section

static CMPIEnumeration* mbEnumInstanceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

#if 0
OW_CIMPropertyArray *getList(char** l) {
   (void) l;
   return new OW_CIMPropertyArray;
}
#endif
OW_StringArray * getList(char ** l) {
   (void) l;
   return new OW_StringArray;
}

static CMPIInstance* mbGetInstance(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char **properties, CMPIStatus *rc) {
   (void) mb;
   //cout<<"--- mbGetInstance()"<<endl;
   CMPIFlags flgs=ctx->ft->getEntry(ctx,CMPIInvocationFlags,NULL).value.uint32;
   //OW_PropertyArray *props=getList(properties);
   OW_StringArray *props=getList(properties);
   OW_CIMObjectPath ocop(*CM_ObjectPath(cop));

// BMMU 
   try {
      OW_CIMInstance ci=CM_CIMOM(ctx)->getInstance( 
      //OW_CIMInstance ci=CM_CIMOM(mb)->getInstance( 
                  //OperationContext(*CM_Context(ctx)),
   		CM_ObjectPath(cop)->getNameSpace(),
		ocop,
		CM_LocalOnly(flgs),
		CM_IncludeQualifiers(flgs),CM_ClassOrigin(flgs),
		props);
      //cout<<"--- mbGetInstance() back"<<endl;
      delete props;
      if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return (CMPIInstance*)new CMPI_Object(new OW_CIMInstance(ci));
   }
   //catch (const OW_CIMException &e) {
   catch (...) {
      //cout<<"### exception: "<<e.getMessage()<<endl;
      exit(39);
   }
   delete props;
   return NULL;
}

static CMPIObjectPath* mbCreateInstance(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIInstance *ci, CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) ci;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

static CMPIStatus mbSetInstance(CMPIBroker *mb, CMPIContext *ctx, CMPIInstance *ci) {
   (void) mb;
   (void) ctx;
   (void) ci;
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIStatus mbDeleteInstance (CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop) {
   (void) mb;
   (void) ctx;
   (void) cop;
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIEnumeration* mbExecQuery(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *query, char *lang, CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) query;
   (void) lang;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

static CMPIEnumeration* mbEnumInstances(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char **properties, CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) properties;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

static CMPIEnumeration* mbAssociators(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
                 char *role, char *resultRole, char **properties, CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) assocClass;
   (void) resultClass;
   (void) role;
   (void) resultRole;
   (void) properties;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

static CMPIEnumeration* mbAssociatorNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *assocClass, char *resultClass,
		 char *role, char *resultRole, CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) assocClass;
   (void) resultClass;
   (void) role;
   (void) resultRole;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

static CMPIEnumeration* mbReferences(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *role , char *resultRole, char **properties,
                 CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) role;
   (void) resultRole;
   (void) properties;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

static CMPIEnumeration* mbReferenceNames(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *role, char *resultRole,
                 CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) role;
   (void) resultRole;
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return NULL;
}

static CMPIData mbInvokeMethod(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *method, CMPIArgs *in, CMPIArgs *out,
		 CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) method;
   (void) in;
   (void) out;

   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return data;
}

static CMPIStatus mbSetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop, char *name, CMPIValue *val,
                 CMPIType type) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) name;
   (void) val;
   (void) type;
   CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

static CMPIData mbGetProperty(CMPIBroker *mb, CMPIContext *ctx,
                 CMPIObjectPath *cop,char *name, CMPIStatus *rc) {
   (void) mb;
   (void) ctx;
   (void) cop;
   (void) name;
   CMPIData data={(CMPIType) 0, CMPI_nullValue, CMPIValue() };
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
   return data;
}





static CMPIBrokerEncFT brokerEnc_FT={
     CMPICurrentVersion,
     mbEncNewInstance,
     mbEncNewObjectPath,
     mbEncNewArgs,
     mbEncNewString,
     mbEncNewArray,
     mbEncNewDateTime,
     mbEncNewDateTimeFromBinary,
     mbEncNewDateTimeFromString,
     NULL,
     NULL,
     mbEncToString,
     mbEncIsA,
     mbEncGetType,
};

static CMPIBrokerFT broker_FT={
     0, // brokerClassification;
     0, // brokerVersion;
     "Pegasus",
     NULL,
     NULL,
     NULL,
     NULL,
     mbEnumInstanceNames,
     mbGetInstance,
     mbCreateInstance,
     mbSetInstance,
     mbDeleteInstance,
     mbExecQuery,
     mbEnumInstances,
     mbAssociators,
     mbAssociatorNames,
     mbReferences,
     mbReferenceNames,
     mbInvokeMethod,
     mbSetProperty,
     mbGetProperty,
};

CMPIBrokerFT *CMPI_Broker_Ftab=& broker_FT;
CMPIBrokerEncFT *CMPI_BrokerEnc_Ftab=&brokerEnc_FT;
